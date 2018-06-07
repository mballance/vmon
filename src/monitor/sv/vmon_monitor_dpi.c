/*
 * vmon_monitor_dpi.c
 *
 *  Created on: May 31, 2018
 *      Author: ballance
 */
#include "vmon_monitor.h"
#include <stdio.h>

extern void *svGetScope(void);
extern void svSetScope(void *);

extern int m2h_access_data(
		int unsigned		id,
		unsigned char		*data,
		unsigned int		sz,
		int					*ret);

static void *prv_scope = 0;
static int unsigned prv_m2h_id = 0;

static int m2h_data(void *ud, uint8_t *data, uint32_t sz) {
	int unsigned id = (int unsigned)ud;
	int ret = 1;
	uint8_t tmp[8] = {0};

	if (sz) {
		memcpy(tmp, data, sz);
	}

	svSetScope(prv_scope);
	if (m2h_access_data(id, tmp, sz, &ret)) {
		fprintf(stdout, "TODO: abort\n");
	}

	return ret;
}

void _vmon_monitor_init() {
	prv_scope = svGetScope();

	vmon_monitor_init();
}

int _vmon_monitor_msg(const char *msg) {
	vmon_monitor_msg(msg);
	return 0;
}

int _vmon_monitor_tracepoint(uint32_t tp) {
	vmon_monitor_tracepoint(tp);
	return 0;
}

int unsigned _vmon_monitor_add_m2h_path(void) {
	vmon_monitor_add_m2h_path(&m2h_data, (void *)prv_m2h_id);
	return (prv_m2h_id++);
}






