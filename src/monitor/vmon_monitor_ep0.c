
#include "vmon_monitor_services.h"
#include <stdint.h>
#include <stdio.h>

int32_t vmon_monitor_exec(uint64_t addr) {
	fprintf(stdout, "vmon_monitor_exec: 0x%08llx\n", addr);
	fflush(stdout);
	return ((int32_t (*)(void))addr)();
}
int32_t __attribute__((weak)) vmon_monitor_exec(uint64_t addr);

/**
 *
 */
void vmon_monitor_write32(uint64_t addr, uint32_t data) {
	*((volatile uint32_t *)addr) = data;
}

void __attribute__((weak)) vmon_monitor_write32(uint64_t addr, uint32_t data);

/**
 *
 */
void vmon_monitor_write8(uint64_t addr, uint8_t data) {
	*((volatile uint8_t *)addr) = data;
}
void __attribute__((weak)) vmon_monitor_write8(uint64_t addr, uint8_t data);

uint32_t vmon_monitor_read32(uint64_t addr) {
	return *((volatile uint32_t *)addr);
}
uint32_t __attribute__((weak)) vmon_monitor_read32(uint64_t addr);

uint8_t vmon_monitor_read8(uint64_t addr) {
	return *((volatile uint8_t *)addr);
}

uint8_t __attribute__((weak)) vmon_monitor_read8(uint64_t addr);
