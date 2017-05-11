/****************************************************************************
 * vmon_monitor.c
 ****************************************************************************/
#include "vmon_monitor.h"
#include "vmon_monitor_services.h"
#include "vmon_msgs.h"
#include "vmon_ep0_msgs.h"
#include <string.h>
#include <stdio.h>

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

/**
 * All EP0 commands are formatted as:
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

		for (i=0; i<8; i++) {
			addr <<= 8;
			addr |= buf[8+i];
		}

		outb(mon, VMON_MSG_RSP_OK);

		// Do the exec
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
			fprintf(stdout, "OK: h2m now %d\n", id);
			outb(mon, VMON_MSG_RSP_OK);
			mon->h2m_id = id;
		} else {
			outb(mon, VMON_MSG_RSP_ERR);
		}
	} break;
	}
}

void vmon_monitor_run(vmon_monitor_t *mon) {
	uint8_t b, p;
	uint32_t i;

	while (1) {

		// Wait for a header byte
		b = getb(mon);

		fprintf(stdout, "monitor: b=0x%02x\n", b);

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
				fprintf(stdout, "len=%d\n", len);
				for (i=0; i<len; i++) {
					mon->buf[i] = getb(mon);
				}

				fprintf(stdout, "ep=%d\n", ep);
				fprintf(stdout, "data[0]=%d data[1]=%d\n", mon->buf[0], mon->buf[1]);

				vmon_monitor_handle_ep0_fixed(mon, len, mon->buf);

			} else {
				// error
				outb(mon, VMON_MSG_RSP_ERR);
			}
		} else if (b == VMON_MSG_VARLEN_REQ) {
			// variable-length message
		} else {
			// unknown
		}
	}
}
