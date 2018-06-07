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
 * vmon_monitor_run()
 *
 * Runs the main loop of the vmon_monitor
 */
void vmon_monitor_run(void);


#ifdef __cplusplus
}
#endif

#endif /* INCLUDED_VMON_MONITOR_H */
