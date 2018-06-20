/*
 * vmon_m2h_client_ep.h
 *
 *  Created on: Jun 5, 2018
 *      Author: ballance
 */

#ifndef SRC_CLIENT_VMON_M2H_CLIENT_EP_H_
#define SRC_CLIENT_VMON_M2H_CLIENT_EP_H_
#include "vmon_write_if.h"

class vmon_client;
class vmon_m2h_client_ep : public virtual vmon_write_if {
public:
	vmon_m2h_client_ep(vmon_client *client);

	virtual ~vmon_m2h_client_ep();

	virtual int32_t write(
			void 			*data,
			uint32_t		sz);

private:
	enum MsgState {
		MsgState_Cmd,
		MsgState_FixedLenHdr,
		MsgState_VarLenHdr,
		MsgState_VarLenSize,
		MsgState_Data
	};

private:
	void process();

	uint8_t getb();

	uint32_t avail() const { return m_data_write_idx-m_data_read_idx; }

	void skip(uint32_t amt);

private:
	bool					m_debug;
	vmon_client				*m_client;
	uint8_t					*m_data;
	uint32_t				m_data_read_idx;
	uint32_t				m_data_write_idx;
	uint32_t				m_data_max;

	MsgState				m_msg_state;
	uint8_t					m_msg_ep;
	uint8_t					m_msg_cmd;
	uint32_t				m_msg_len;
	bool					m_msg_fixed;

};

#endif /* SRC_CLIENT_VMON_M2H_CLIENT_EP_H_ */
