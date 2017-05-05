/****************************************************************************
 * vmon_monitor.c
 ****************************************************************************/
#include "vmon_monitor.h"
#include "vmon_services.h"

static uint8_t		vm_h2m_p = 0;
static uint8_t		vm_m2h_p = 0;
static uint8_t		vm_buf[16];

static uint8_t getb() {
	uint8_t d;

	vmon_recv(vm_h2m_p, &d, 1);

	return d;
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

void vmon_monitor_run(
		uint8_t		n_h2m_paths,
		uint8_t		n_m2h_paths) {
	uint8_t b, p;
	uint32_t i;

	while (true) {

		// Wait for a header byte
		b = getb();

		if (b == 0xEA) {
			// PING response
			outb(0xE5);
		} else if (b == 0xA5) {
			// fixed-length message
			b = getb();
			p = parity(b);

			if (p == (b&1)) {
				uint8_t ep;
				uint8_t len;

				// okay
				ep = (b >> 3); // 7:3
				len = 2 << ((b >> 1) & 0x3);
				for (i=0; i<len; i++) {
					vm_buf[i] = getb();
				}


				outb(0xE0);
			} else {
				// error
				outb(0xE1);
			}
		} else if (b == 0xA6) {
			// variable-length message
		} else {
			// unknown
		}
	}
}
