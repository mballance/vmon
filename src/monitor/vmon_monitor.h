/****************************************************************************
 * vmon_monitor.h
 ****************************************************************************/
#ifndef INCLUDED_VMON_MONITOR_H
#define INCLUDED_VMON_MONITOR_H
#include <stdint.h>
#include <stdarg.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef int32_t (*vmon_data_f)(void *, uint8_t *, uint32_t);

typedef enum {
	FIXEDLEN_2,
	FIXEDLEN_4,
	FIXEDLEN_8,
	FIXEDLEN_16
} vmon_fixedlen_e;

typedef struct vmon_closure_s {
	vmon_data_f			f;
	void				*ud;
} vmon_closure_t;

void vmon_monitor_init(void);

uint32_t vmon_monitor_add_h2m_path(
		vmon_data_f			f,
		void				*ud);

uint32_t vmon_monitor_add_m2h_path(
		vmon_data_f			f,
		void				*ud);

void vmon_monitor_msg(
		const char			*msg);

void vmon_monitor_tracepoint(uint32_t tp);

void vmon_printf(const char *fmt, ...);

void vmon_vprintf(const char *fmt, va_list ap);

void vmon_monitor_endtest(
		int32_t				status);

/**
 * Locks access to the monitor. Must be done before beginning a variable-length message
 */
void vmon_monitor_lock(void);

/**
 * Unlocks access to the monitor.
 */
void vmon_monitor_unlock(void);

/**
 * Sends a fixed-length message.
 * This function locks and unlocks on its own.
 */
void vmon_monitor_fixedlen_msg(
		uint8_t				ep,
		vmon_fixedlen_e		len,
		uint8_t				*data);

/**
 * vmon_monitor_run()
 *
 * Runs the main loop of the vmon_monitor
 */
void vmon_monitor_run(void);


#ifdef __cplusplus
}
#endif

#endif /* INCLUDED_VMON_MONITOR_H */
