/****************************************************************************
 * vmon_services.h
 *
 * Functions that must be provided by the monitor
 ****************************************************************************/
#ifndef INCLUDED_VMON_SERVICES_H
#define INCLUDED_VMON_SERVICES_H
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif


/**
 * Provides a specific implementation for exec
 */
int32_t vmon_monitor_exec(uint64_t addr);

/**
 *
 */
void vmon_monitor_write32(uint64_t addr, uint32_t data);

/**
 *
 */
void vmon_monitor_write8(uint64_t addr, uint8_t data);

/**
 *
 */
uint32_t vmon_monitor_read32(uint64_t addr);

/**
 *
 */
uint8_t vmon_monitor_read8(uint64_t addr);



#ifdef __cplusplus
}
#endif

#endif /* INCLUDED_VMON_SERVICES_H */
