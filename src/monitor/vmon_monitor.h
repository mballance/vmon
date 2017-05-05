/****************************************************************************
 * vmon_monitor.h
 ****************************************************************************/
#ifndef INCLUDED_VMON_MONITOR_H
#define INCLUDED_VMON_MONITOR_H
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * vmon_monitor_run()
 *
 * Runs the main loop of the vmon_monitor
 */
void vmon_monitor_run(
		uint8_t		n_h2m_paths,
		uint8_t		n_m2h_paths);


#ifdef __cplusplus
}
#endif

#endif /* INCLUDED_VMON_MONITOR_H */
