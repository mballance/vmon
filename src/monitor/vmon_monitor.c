/****************************************************************************
 * vmon_monitor.c
 ****************************************************************************/
#include "vmon_monitor.h"
#include "vmon_monitor_services.h"
#include "vmon_msgs.h"
#include "vmon_ep0_msgs.h"
#include <string.h>
#include <stdarg.h>
#include <stdio.h>


typedef struct vmon_monitor_s {
	uint8_t				h2m_id;
	uint8_t				h2m_idx;
	uint8_t				m2h_id;
	uint8_t				m2h_idx;
	uint8_t				m2h_buf[4];
	uint8_t 			m2h_buf_idx;
	vmon_closure_t		h2m[4];
	vmon_closure_t		m2h[4];
	uint8_t				active;
	uint8_t				buf[16];
	uint32_t			lock;
} vmon_monitor_t;

static vmon_monitor_t glbl_mon = {0};

void __attribute__((weak)) vmon_monitor_lock_impl(uint32_t *l) {
	// Do nothing
}

void __attribute__((weak)) vmon_monitor_unlock_impl(uint32_t *l) {
	// Do nothing
}

void __attribute__((weak)) vmon_monitor_lock_init_impl(uint32_t *l) {
	// Do nothing
}

void vmon_monitor_lock(void) {
	vmon_monitor_lock_impl(&glbl_mon.lock);
}

void vmon_monitor_unlock(void) {
	vmon_monitor_unlock_impl(&glbl_mon.lock);
}

static uint8_t vmon_monitor_getb(void) {
	uint8_t d;

	glbl_mon.h2m[glbl_mon.h2m_id].f(
			glbl_mon.h2m[glbl_mon.h2m_id].ud, &d, 1);

	return d;
}

static void vmon_monitor_outb(uint8_t d) {
//	fprintf(stdout, "OUTB: buf_idx=%d m2h_id=%d\n",
//			glbl_mon.m2h_buf_idx, glbl_mon.m2h_id);
//	fflush(stdout);
	glbl_mon.m2h_buf[glbl_mon.m2h_buf_idx++] = d;

	if (glbl_mon.m2h_buf_idx >= 4) {
		glbl_mon.m2h[glbl_mon.m2h_id].f(
				glbl_mon.m2h[glbl_mon.m2h_id].ud,
				glbl_mon.m2h_buf, 4);
		glbl_mon.m2h_buf_idx = 0;
	}
}

static void vmon_monitor_flush(void) {
	if (glbl_mon.m2h_buf_idx > 0) {
		glbl_mon.m2h[glbl_mon.m2h_id].f(
				glbl_mon.m2h[glbl_mon.m2h_id].ud,
				glbl_mon.m2h_buf, glbl_mon.m2h_buf_idx);
		glbl_mon.m2h_buf_idx = 0;
	}
	glbl_mon.m2h[glbl_mon.m2h_id].f(
			glbl_mon.m2h[glbl_mon.m2h_id].ud, 0, 0);
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

void vmon_monitor_init(void) {
	glbl_mon.lock = 0;
	glbl_mon.h2m_id = 0;
	glbl_mon.h2m_idx = 0;
	glbl_mon.m2h_id = 0;
	glbl_mon.m2h_idx = 0;
	glbl_mon.m2h_buf_idx = 0;
	glbl_mon.active = 0;
}

uint32_t vmon_monitor_add_h2m_path(
		vmon_data_f			f,
		void				*ud) {
	glbl_mon.h2m[glbl_mon.h2m_idx].f  = f;
	glbl_mon.h2m[glbl_mon.h2m_idx].ud = ud;
	return (glbl_mon.h2m_idx++);
}

uint32_t vmon_monitor_add_m2h_path(
		vmon_data_f			f,
		void				*ud) {
	glbl_mon.m2h[glbl_mon.m2h_idx].f  = f;
	glbl_mon.m2h[glbl_mon.m2h_idx].ud = ud;
	return (glbl_mon.m2h_idx++);
}

void vmon_monitor_msg(const char *msg) {
	uint8_t cs;
	uint32_t len = 0, len_t, i;
	const char *t = msg;

	while (t && *t) {
		len++;
		t++;
	}

//	vmon_monitor_tracepoint(len);

	// strlen+cmd+null+checksum
	len_t = len + 1 + 1 + 1;

	vmon_monitor_lock();

	// Send an EP0 MSG message
	vmon_monitor_outb(VMON_MSG_VARLEN_REQ);
	vmon_monitor_outb(0); // EP0

	// Total length
	vmon_monitor_outb(len_t);
	vmon_monitor_outb(len_t>>8);

	cs = VMON_EP0_MSG;
	vmon_monitor_outb(VMON_EP0_MSG);

	for (i=0; i<=len; i++) {
		vmon_monitor_outb(msg[i]);
		cs += msg[i];
	}
	vmon_monitor_outb(cs);
	vmon_monitor_flush(); // ensure the entire message is sent

	vmon_monitor_unlock();
}

void vmon_monitor_tracepoint(uint32_t tp) {
	vmon_monitor_lock_impl(&glbl_mon.lock);
	vmon_monitor_outb(VMON_MSG_FIXLEN_REQ);
	vmon_monitor_outb(0x02);
	vmon_monitor_outb(VMON_EP0_TP);
	vmon_monitor_outb(tp);
	vmon_monitor_outb(tp >> 8);
	vmon_monitor_outb(tp >> 16);

	vmon_monitor_flush();
	vmon_monitor_unlock_impl(&glbl_mon.lock);
}

void vmon_monitor_endtest(int32_t status) {
	int i;
	uint8_t cmd_ep = (2 << 1); // EP=0, len=8
	uint32_t stat_u = (uint32_t)status;
	cmd_ep |= (parity(cmd_ep) & 1);

	vmon_monitor_outb(VMON_MSG_FIXLEN_REQ);
	vmon_monitor_outb(cmd_ep);
	vmon_monitor_outb(VMON_EP0_ENDTEST);
	for (i=0; i<3; i++) { // pad
		vmon_monitor_outb(0);
	}
	for (i=0; i<4; i++) {
		vmon_monitor_outb((stat_u >> (8*i)));
	}
	vmon_monitor_flush();
}

/**
 * All fixed-length EP0 commands are formatted as:
 * [0] Command
 *
 */
int vmon_monitor_handle_ep0_fixed(
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

		vmon_monitor_outb(VMON_MSG_RSP_OK);
		vmon_monitor_flush();

		// Do the exec
		vmon_monitor_exec(addr);
	} break;

	case VMON_EP0_SET_M2H_EP: {
		uint8_t id = buf[1];

		if (id < glbl_mon.m2h_idx) {
			vmon_monitor_outb(VMON_MSG_RSP_OK);
			glbl_mon.m2h_id = id;
		} else {
			vmon_monitor_outb(VMON_MSG_RSP_ERR);
		}
		vmon_monitor_flush();
	} break;

	case VMON_EP0_SET_H2M_EP: {
		uint8_t id = buf[1];

		if (id < glbl_mon.h2m_idx) {
			vmon_monitor_outb(VMON_MSG_RSP_OK);
			glbl_mon.h2m_id = id;
		} else {
			vmon_monitor_outb(VMON_MSG_RSP_ERR);
		}
		vmon_monitor_flush();
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

		vmon_monitor_outb(VMON_MSG_RSP_OK);
		vmon_monitor_outb(VMON_MSG_VARLEN_REQ);
		vmon_monitor_outb(0); // EP
		vmon_monitor_outb((len & 0xFF));
		vmon_monitor_outb(((len >> 8) & 0xFF));

		for (i=0; i<len; i++) {
			tmp = vmon_monitor_read8(addr);
			vmon_monitor_outb(tmp);
			cs += tmp;
			addr++;
		}

		vmon_monitor_outb(cs);
		vmon_monitor_flush();

	} break;

	case VMON_EP0_EXIT: {
		vmon_monitor_outb(VMON_MSG_RSP_OK);
		vmon_monitor_flush();
		glbl_mon.active = 0;
	} break;

	default:
		vmon_monitor_outb(VMON_MSG_RSP_ERR);
		vmon_monitor_flush();
		break;

	}
}

void vmon_monitor_handle_ep0_var(uint32_t		len) {
	uint8_t c = vmon_monitor_getb();

	switch (c) {
	case VMON_EP0_WRITE: {
		uint32_t i;
		uint64_t addr = 0, tmp;
		uint8_t t, cs=c, cs_t;

		for (i=0; i<8; i++) {
			t = vmon_monitor_getb();
			cs += t;
			tmp = t;
			tmp <<= 8*i;
			addr |= tmp;
		}

		for (i=0; i<(len-8-1); i++) {
			t=vmon_monitor_getb();
			cs += t;
			vmon_monitor_write8(addr, t);
			addr++;
		}

		// Finally, get the checksum
		cs_t = vmon_monitor_getb();

		if (cs == cs_t) {
			vmon_monitor_outb(VMON_MSG_RSP_OK);
		} else {
//			fprintf(stdout, "Bad CS: expect 0x%02x ; 0x%02x\n", cs, cs_t);
			vmon_monitor_outb(VMON_MSG_RSP_ERR);
		}
		vmon_monitor_flush();
	} break;

	default:
		vmon_monitor_outb(VMON_MSG_RSP_ERR);
		vmon_monitor_flush();
		break;
	}
}

void vmon_monitor_fixedlen_msg(
		uint8_t					ep,
		vmon_fixedlen_e			len,
		uint8_t					*data) {
	uint32_t len_b = 2 << len;
	uint32_t i;
	uint8_t hdr;
	vmon_monitor_lock_impl(&glbl_mon.lock);

	vmon_monitor_outb(VMON_MSG_FIXLEN_REQ);
	hdr = (ep << 3) | (len << 1);
	vmon_monitor_outb(hdr);

	for (i=0; i<len_b; i++) {
		vmon_monitor_outb(data[i]);
	}

	vmon_monitor_flush();

	vmon_monitor_unlock_impl(&glbl_mon.lock);
}

void vmon_monitor_varlen_hdr_msg(
		uint8_t						ep,
		const vmon_databuf_t		*hdr,
		const uint8_t				*data,
		uint32_t					data_len) {
	// Header+data+checksum
	uint32_t i, len_t = hdr->idx + data_len + 1;
	uint8_t cs = 0;
	vmon_monitor_lock_impl(&glbl_mon.lock);

	vmon_monitor_outb(VMON_MSG_VARLEN_REQ);
	vmon_monitor_outb(ep << 3); // EP always occupies the same space

	// Total length of data
	vmon_monitor_outb(len_t);
	vmon_monitor_outb(len_t>>8);

	// First, send header data
	for (i=0; i<hdr->idx; i++) {
		vmon_monitor_outb(hdr->data[i]);
		cs += hdr->data[i];
	}

	// Finally, send payload data
	for (i=0; i<data_len; i++) {
		vmon_monitor_outb(data[i]);
		cs += data[i];
	}

	// Send the checksum
	vmon_monitor_outb(cs);

	vmon_monitor_flush();
	vmon_monitor_unlock_impl(&glbl_mon.lock);
}

void vmon_monitor_run() {
	uint8_t b, p;
	uint32_t i;

	glbl_mon.active = 1;

	while (glbl_mon.active) {

		// Wait for a header byte
		b = vmon_monitor_getb();

		if (b == VMON_MSG_PING_REQ) {
			// PING response
			vmon_monitor_outb(VMON_MSG_PING_ACK);
			vmon_monitor_flush();
		} else if (b == VMON_MSG_FIXLEN_REQ) {
			// fixed-length message
			b = vmon_monitor_getb();
			p = parity((b & 0xFE)); // Don't include the parity bit

			if (p == (b&1)) {
				uint8_t ep;
				uint8_t len;

				// okay
				ep = (b >> 3); // 7:3
				// 2, 4, 8, 16
				len = 2 << ((b >> 1) & 0x3);
				for (i=0; i<len; i++) {
					glbl_mon.buf[i] = vmon_monitor_getb();
				}

				if (ep == 0) {
					vmon_monitor_handle_ep0_fixed(len, glbl_mon.buf);
				} else {
					// Only EP0 supported at the moment
					vmon_monitor_outb(VMON_MSG_RSP_ERR);
				}
			} else {
				// error
				vmon_monitor_outb(VMON_MSG_RSP_ERR);
			}
			vmon_monitor_flush();
		} else if (b == VMON_MSG_VARLEN_REQ) {
			// variable-length message
			b = vmon_monitor_getb();
			p = parity((b & 0xFE));

			if (b == (b&1)) {
				uint8_t ep = (b >> 3); // 7:3
				uint16_t len = 0;

				for (i=0; i<2; i++) {
					len |= (vmon_monitor_getb() << 8*i);
				}

				if (ep == 0) {
					vmon_monitor_handle_ep0_var(len);
				} else {
					// Only EP0 supported at the moment
					vmon_monitor_outb(VMON_MSG_RSP_ERR);
				}
			} else {
				vmon_monitor_outb(VMON_MSG_RSP_ERR);
			}
			vmon_monitor_flush();
		} else {
			// unknown
		}
	}
}
