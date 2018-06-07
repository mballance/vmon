/*
 * vmon_dpi_client.h
 *
 *  Created on: May 31, 2018
 *      Author: ballance
 */

#ifndef SRC_CLIENT_SV_VMON_DPI_CLIENT_H_
#define SRC_CLIENT_SV_VMON_DPI_CLIENT_H_
#include "vmon_client.h"

class vmon_dpi_client : public vmon_client {
public:
	vmon_dpi_client();

	virtual ~vmon_dpi_client();

protected:
	int32_t poll(
			const std::vector<vmon_m2h_if *> &m2h_l,
			int32_t timeout=-1);

};

#endif /* SRC_CLIENT_SV_VMON_DPI_CLIENT_H_ */
