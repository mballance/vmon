/*
 * vmon_m2h_poll_if.h
 *
 *  Created on: May 31, 2018
 *      Author: ballance
 */

#ifndef SRC_CLIENT_VMON_M2H_POLL_IF_H_
#define SRC_CLIENT_VMON_M2H_POLL_IF_H_
#include <stdint.h>
#include <vector>


/**
 * Interface that allows the vmon_client to poll for data
 */
class vmon_m2h_if;
class vmon_m2h_poll_if {
public:

	virtual ~vmon_m2h_poll_if() { }

	int32_t poll(
			const std::vector<vmon_m2h_if *> &m2h_l,
			int32_t timeout=-1);

};



#endif /* SRC_CLIENT_VMON_M2H_POLL_IF_H_ */
