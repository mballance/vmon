/*
 * vmon_dpi_client.cpp
 *
 *  Created on: May 31, 2018
 *      Author: ballance
 */

#include "vmon_dpi_client.h"
#include "vmon_client_dpi.h"

vmon_dpi_client::vmon_dpi_client() {
	m_scope = svGetScope();
	add_ep0_listener(this);
}

vmon_dpi_client::~vmon_dpi_client() {
	// TODO Auto-generated destructor stub
}

void vmon_dpi_client::msg(const char *msg) {
	svSetScope(m_scope);
	_vmon_client_ep0_msg(this, msg);
}

void vmon_dpi_client::tracepoint(int unsigned tp) {
	svSetScope(m_scope);
	_vmon_client_ep0_tracepoint(this, tp);
}

void vmon_dpi_client::endtest(int32_t status) {

}

int32_t vmon_dpi_client::poll(
			const std::vector<vmon_m2h_if *> 	&m2h_l,
			int32_t								timeout) {
	return 0;
}
