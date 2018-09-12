/*
 * vmon_client_ep_if.h
 *
 * Specifies the API an endpoint can can use to interact
 * with the vmon_client class
 *
 *  Created on: May 31, 2018
 *      Author: ballance
 */

#ifndef INCLUDED_VMON_CLIENT_EP_IF_H_
#define INCLUDED_VMON_CLIENT_EP_IF_H_
#include "vmon_databuf.h"

class vmon_client_ep_if {
public:

	virtual ~vmon_client_ep_if() { }

	virtual void process_msg(uint8_t ep, vmon_databuf &data);

};





#endif /* INCLUDED_VMON_CLIENT_EP_IF_H_ */
