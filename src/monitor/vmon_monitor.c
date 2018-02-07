/****************************************************************************
 * vmon_monitor.c
 ****************************************************************************/
#include "vmon_monitor.h"
#include "vmon_monitor_services.h"
#include "vmon_msgs.h"
#include "vmon_ep0_msgs.h"
#include <string.h>


vmon_monitor_t *glbl_mon;

static uint8_t getb(vmon_monitor_t *mon) {
	uint8_t d;

	mon->h2m[mon->h2m_id].f(mon->h2m[mon->h2m_id].ud, &d, 1);

	return d;
}

static void outb(vmon_monitor_t *mon, uint8_t d) {
	mon->m2h[mon->m2h_id].f(mon->m2h[mon->m2h_id].ud, &d, 1);
}

static uint8_t parity(uint8_t b) {
	uint8_t ret = 0;
	while (b) {
		if (b&1) {
			ret++;
		}
		b >>= 1;
	}

	return !(ret&1);
}

void vmon_monitor_init(vmon_monitor_t  *mon) {
	memset(mon, 0, sizeof(vmon_monitor_t));
}

void vmon_monitor_add_h2m_path(
		vmon_monitor_t		*mon,
		vmon_data_f			f,
		void				*ud) {
	mon->h2m[mon->h2m_idx].f  = f;
	mon->h2m[mon->h2m_idx].ud = ud;
	mon->h2m_idx++;
}

void vmon_monitor_add_m2h_path(
		vmon_monitor_t		*mon,
		vmon_data_f			f,
		void				*ud) {
	mon->m2h[mon->m2h_idx].f  = f;
	mon->m2h[mon->m2h_idx].ud = ud;
	mon->m2h_idx++;
}

void vmon_monitor_msg(
		vmon_monitor_t		*mon,
		const char			*msg) {
	uint8_t cs;
	uint32_t len = strlen(msg), i;
	// strlen+cmd+null
	uint32_t len_t = len + 1 + 1;

	// Send an EP0 MSG message
	outb(mon, VMON_MSG_VARLEN_REQ);
	outb(mon, 0); // EP0
	// Total length
	outb(mon, len_t);
	outb(mon, len_t>>8);

	cs = VMON_EP0_MSG;
	outb(mon, VMON_EP0_MSG);

	for (i=0; i<=len; i++) {
		outb(mon, msg[i]);
		cs += msg[i];
	}
	outb(mon, cs);
}

void vmon_monitor_endtest(
		vmon_monitor_t			*mon,
		int32_t					status) {
	int i;
	uint8_t cmd_ep = (2 << 1); // EP=0, len=8
	uint32_t stat_u = (uint32_t)status;
	cmd_ep |= (parity(cmd_ep) & 1);

	outb(mon, VMON_MSG_FIXLEN_REQ);
	outb(mon, cmd_ep);
	outb(mon, VMON_EP0_ENDTEST);
	for (i=0; i<3; i++) { // pad
		outb(mon, 0);
	}
	for (i=0; i<4; i++) {
		outb(mon, (stat_u >> (8*i)));
	}
}

/**
 * All fixed-length EP0 commands are formatted as:
 * [0] Command
 *
 */
int vmon_monitor_handle_ep0_fixed(
		vmon_monitor_t 	*mon,
		uint32_t		len,
		uint8_t			*buf) {
	switch (buf[0]) {
	case VMON_EP0_EXEC: {
		uint64_t addr = 0;
		int i;

		for (i=7; i>=0; i--) {
			addr <<= 8;
			addr |= buf[8+i];
		}

		outb(mon, VMON_MSG_RSP_OK);

		// Do the exec
		glbl_mon = mon;
		vmon_monitor_exec(addr);
	} break;

	case VMON_EP0_SET_M2H_EP: {
		uint8_t id = buf[1];

		if (id < mon->m2h_idx) {
			outb(mon, VMON_MSG_RSP_OK);
			mon->m2h_id = id;
		} else {
			outb(mon, VMON_MSG_RSP_ERR);
		}
	} break;

	case VMON_EP0_SET_H2M_EP: {
		uint8_t id = buf[1];

		if (id < mon->h2m_idx) {
			outb(mon, VMON_MSG_RSP_OK);
			mon->h2m_id = id;
		} else {
			outb(mon, VMON_MSG_RSP_ERR);
		}
	} break;

	case VMON_EP0_READ: {
		uint64_t addr = 0;
		uint16_t len = 0;
		uint8_t cs=0, tmp;
		int i;

		for (i=7; i>=0; i--) {
			addr <<= 8;
			addr |= buf[i+1];
		}

		for (i=1; i>=0; i--) {
			len <<= 8;
			len |= buf[i+9];
		}

		outb(mon, VMON_MSG_RSP_OK);
		outb(mon, VMON_MSG_VARLEN_REQ);
		outb(mon, 0); // EP
		outb(mon, (len & 0xFF));
		outb(mon, ((len >> 8) & 0xFF));

		for (i=0; i<len; i++) {
			tmp = vmon_monitor_read8(addr);
			outb(mon, tmp);
			cs += tmp;
			addr++;
		}

		outb(mon, cs);

	} break;

	case VMON_EP0_EXIT: {
		outb(mon, VMON_MSG_RSP_OK);
		mon->active = 0;
	} break;

	default:
		outb(mon, VMON_MSG_RSP_ERR);
		break;

	}
}

void vmon_monitor_handle_ep0_var(
		vmon_monitor_t 	*mon,
		uint32_t		len) {
	uint8_t c = getb(mon);

	switch (c) {
	case VMON_EP0_WRITE: {
		uint32_t i;
		uint64_t addr = 0, tmp;
		uint8_t t, cs=c, cs_t;

		for (i=0; i<8; i++) {
			t = getb(mon);
			cs += t;
			tmp = t;
			tmp <<= 8*i;
			addr |= tmp;
		}

		for (i=0; i<(len-8-1); i++) {
			t=getb(mon);
			cs += t;
			vmon_monitor_write8(addr, t);
			addr++;
		}

		// Finally, get the checksum
		cs_t = getb(mon);

		if (cs == cs_t) {
			outb(mon, VMON_MSG_RSP_OK);
		} else {
//			fprintf(stdout, "Bad CS: expect 0x%02x ; 0x%02x\n", cs, cs_t);
			outb(mon, VMON_MSG_RSP_ERR);
		}
	} break;

	default:
		outb(mon, VMON_MSG_RSP_ERR);
		break;
	}
}

void vmon_monitor_run(vmon_monitor_t *mon) {
	uint8_t b, p;
	uint32_t i;

	mon->active = 1;

	while (mon->active) {

		// Wait for a header byte
		b = getb(mon);

		if (b == VMON_MSG_PING_REQ) {
			// PING response
			outb(mon, VMON_MSG_PING_ACK);
		} else if (b == VMON_MSG_FIXLEN_REQ) {
			// fixed-length message
			b = getb(mon);
			p = parity((b & 0xFE)); // Don't include the parity bit

			if (p == (b&1)) {
				uint8_t ep;
				uint8_t len;

				// okay
				ep = (b >> 3); // 7:3
				// 2, 4, 8, 16
				len = 2 << ((b >> 1) & 0x3);
				for (i=0; i<len; i++) {
					mon->buf[i] = getb(mon);
				}

				if (ep == 0) {
					vmon_monitor_handle_ep0_fixed(mon, len, mon->buf);
				} else {
					// Only EP0 supported at the moment
					outb(mon, VMON_MSG_RSP_ERR);
				}
			} else {
				// error
				outb(mon, VMON_MSG_RSP_ERR);
			}
		} else if (b == VMON_MSG_VARLEN_REQ) {
			// variable-length message
			b = getb(mon);
			p = parity((b & 0xFE));

			if (b == (b&1)) {
				uint8_t ep = (b >> 3); // 7:3
				uint16_t len = 0;

				for (i=0; i<2; i++) {
					len |= (getb(mon) << 8*i);
				}

				if (ep == 0) {
					vmon_monitor_handle_ep0_var(mon, len);
				} else {
					// Only EP0 supported at the moment
					outb(mon, VMON_MSG_RSP_ERR);
				}
			} else {
				outb(mon, VMON_MSG_RSP_ERR);
			}
		} else {
			// unknown
		}
	}
}
