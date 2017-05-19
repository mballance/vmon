
#include "vmon_monitor_services.h"
#include <stdint.h>

int32_t vmon_monitor_exec(uint64_t addr) {
#ifdef VMON_ARCH_32
	return ((int32_t (*)(void))((uint32_t)addr))();
#else
	return ((int32_t (*)(void))addr)();
#endif
}
int32_t __attribute__((weak)) vmon_monitor_exec(uint64_t addr);

/**
 *
 */
void vmon_monitor_write8(uint64_t addr, uint8_t data) {
#ifdef VMON_ARCH_32
	*((volatile uint8_t *)((uint32_t)addr)) = data;
#else
	*((volatile uint8_t *)addr) = data;
#endif
}
void __attribute__((weak)) vmon_monitor_write8(uint64_t addr, uint8_t data);

uint8_t vmon_monitor_read8(uint64_t addr) {
#ifdef VMON_ARCH_32
	return *((volatile uint8_t *)((uint32_t)addr));
#else
	return *((volatile uint8_t *)addr);
#endif
}

uint8_t __attribute__((weak)) vmon_monitor_read8(uint64_t addr);
