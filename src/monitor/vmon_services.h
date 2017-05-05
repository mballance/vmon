/****************************************************************************
 * vmon_services.h
 *
 * Functions that must be provided by the monitor
 ****************************************************************************/
#ifndef INCLUDED_VMON_SERVICES_H
#define INCLUDED_VMON_SERVICES_H
#include <stdint.h>

/**
 * Receives data
 */
int32_t vmon_recv(
		uint8_t		h2m_path,
		uint8_t		*data,
		uint32_t	data_len);

int32_t vmon_send(
		uint8_t		m2h_path,
		uint8_t		*data,
		uint32_t	data_len);

#endif /* INCLUDED_VMON_SERVICES_H */
