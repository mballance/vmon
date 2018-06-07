/*
 * vmon_printf.c
 *
 *  Created on: Jun 7, 2018
 *      Author: ballance
 */
#include "vmon_monitor.h"
#include <stdarg.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif
void vmon_monitor_outb(uint8_t d);
#ifdef __cplusplus
}
#endif

void vmon_format(uint32_t doit, const char *fmt, va_list ap) {

}

void vmon_vprintf(const char *fmt, va_list ap) {
	// First, process the format string to determine the size

}

void vmon_printf(const char *fmt, ...) {
	va_list ap;
	va_start(ap, fmt);

	vmon_vprintf(fmt, ap);

	va_end(ap);
}



