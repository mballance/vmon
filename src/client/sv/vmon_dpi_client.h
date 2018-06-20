/*
 * vmon_dpi_client.h
 *
 *  Created on: May 31, 2018
 *      Author: ballance
 */

#ifndef SRC_CLIENT_SV_VMON_DPI_CLIENT_H_
#define SRC_CLIENT_SV_VMON_DPI_CLIENT_H_
#include "vmon_client.h"
#include "vmon_client_ep0_if.h"

class vmon_dpi_client :
		public vmon_client,
		public virtual vmon_client_ep0_if {
public:
	vmon_dpi_client();

	virtual ~vmon_dpi_client();

	virtual void msg(const char *msg);

	virtual void tracepoint(int unsigned tp);

	virtual void endtest(int32_t status);

protected:
	int32_t poll(
			const std::vector<vmon_m2h_if *> &m2h_l,
			int32_t timeout=-1);

private:

	void				*m_scope;

};

#endif /* SRC_CLIENT_SV_VMON_DPI_CLIENT_H_ */
