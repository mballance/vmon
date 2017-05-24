/****************************************************************************
 * vmon_monitor.h
 ****************************************************************************/
#ifndef INCLUDED_VMON_MONITOR_H
#define INCLUDED_VMON_MONITOR_H
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef int32_t (*vmon_data_f)(void *, uint8_t *, uint32_t);

typedef struct vmon_closure_s {
	vmon_data_f			f;
	void				*ud;
} vmon_closure_t;

typedef struct vmon_monitor_s {
	uint8_t				h2m_id;
	uint8_t				h2m_idx;
	uint8_t				m2h_id;
	uint8_t				m2h_idx;
	vmon_closure_t		h2m[4];
	vmon_closure_t		m2h[4];
	uint8_t				buf[16];
} vmon_monitor_t;

extern vmon_monitor_t *glbl_mon;

void vmon_monitor_init(vmon_monitor_t *mon);

void vmon_monitor_add_h2m_path(
		vmon_monitor_t		*mon,
		vmon_data_f			f,
		void				*ud);

void vmon_monitor_add_m2h_path(
		vmon_monitor_t		*mon,
		vmon_data_f			f,
		void				*ud);

void vmon_monitor_msg(
		vmon_monitor_t		*mon,
		const char			*msg);

void vmon_monitor_endtest(
		vmon_monitor_t		*mon,
		int32_t				status);

/**
 * vmon_monitor_run()
 *
 * Runs the main loop of the vmon_monitor
 */
void vmon_monitor_run(vmon_monitor_t *mon);


#ifdef __cplusplus
}
#endif

#endif /* INCLUDED_VMON_MONITOR_H */
