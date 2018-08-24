/*
 * vmon_client_dpi.h
 *
 *  Created on: Jun 14, 2018
 *      Author: ballance
 */

#ifndef INCLUDED_VMON_CLIENT_DPI_H
#define INCLUDED_VMON_CLIENT_DPI_H
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

void *svGetScope();

void svSetScope(void *s);

void *_vmon_client_new(void);

void _vmon_client_set_ep_listener(void *, uint32_t);

void _vmon_client_ep_process_msg(void *, uint8_t, void *);

uint32_t _vmon_databuf_sz(void *);

uint8_t _vmon_databuf_at(void *, uint32_t);

int _vmon_m2h_if_recv(
		void			*if_h,
		uint8_t			*data,
		int				size,
		int				timeout,
		int				*ret);

int _vmon_h2m_if_send(
		void			*if_h,
		uint8_t			*data,
		int				size,
		int				*ret);

void *_vmon_client_add_m2h_if(void *);

void *_vmon_client_add_h2m_if(void *);

int _vmon_client_connect(void *, int *);

int _vmon_client_read(void *, uint64_t, uint8_t *, uint32_t);

uint64_t _vmon_client_get_entry_addr(void *, const char *path);

int _vmon_client_exec(void *client_p, uint64_t addr);

int _vmon_client_exit(void *client_p);

int _vmon_client_wait_endtest(void *client_p, int *status);

int _vmon_m2h_client_ep_write(
		void			*m2h_h,
		uint8_t			*data,
		uint32_t		sz);

void _vmon_client_ep0_msg(
		void			*client_h,
		const char 		*msg);

void _vmon_client_ep0_tracepoint(
		void			*client_h,
		int unsigned 	tp);

void _vmon_client_ep0_endtest(
		void			*client_h,
		int				status);


#ifdef __cplusplus
}
#endif




#endif /* INCLUDED_VMON_CLIENT_DPI_H */
