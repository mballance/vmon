/*
 * vmon_client_ep_if.h
 *
 * Specifies the API an endpoint can can use to interact
 * with the vmon_client class
 *
 *  Created on: May 31, 2018
 *      Author: ballance
 */

#ifndef SRC_CLIENT_VMON_CLIENT_EP_IF_H_
#define SRC_CLIENT_VMON_CLIENT_EP_IF_H_

class vmon_client_ep_if {
public:

	virtual ~vmon_client_ep_if() { }

	/*
	 * ep_data()
	 *
	 * Called by an endpoint to submit received data to the client
	 */
	virtual void ep_data(uint32_t ep, void *data, uint32_t sz) = 0;

};





#endif /* SRC_CLIENT_VMON_CLIENT_EP_IF_H_ */
