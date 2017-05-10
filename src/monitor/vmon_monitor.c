/****************************************************************************
 * vmon_monitor.c
 ****************************************************************************/
#include "vmon_monitor.h"
#include "vmon_services.h"
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

void vmon_monitor_run(vmon_monitor_t *mon) {
	uint8_t b, p;
	uint32_t i;

	while (1) {

		// Wait for a header byte
		b = getb(mon);

		fprintf(stdout, "monitor: b=0x%02x\n", b);

		if (b == 0xEA) {
			// PING response
			outb(mon, 0xE5);
		} else if (b == 0xA5) {
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

				fprintf(stdout, "ep=%d\n", ep);
				fprintf(stdout, "data[0]=%d\n", mon->buf[0]);

				outb(mon, 0xE0);
			} else {
				// error
				outb(mon, 0xE1);
			}
		} else if (b == 0xA6) {
			// variable-length message
		} else {
			// unknown
		}
	}
}
