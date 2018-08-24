/*
 * vmon_m2h_client_ep.cpp
 *
 *  Created on: Jun 5, 2018
 *      Author: ballance
 */

#include "vmon_m2h_client_ep.h"
#include "vmon_client.h"
#include "vmon_msgs.h"
#include <string.h>
#include <stdio.h>

vmon_m2h_client_ep::vmon_m2h_client_ep(vmon_client *client) {
	m_client = client;
	m_data = 0;
	m_data_read_idx = 0;
	m_data_write_idx = 0;
	m_data_max = 0;

	m_msg_state = MsgState_Cmd;
	m_msg_ep = 0;
	m_msg_cmd = 0;
	m_msg_len = 0;
	m_msg_fixed = false;
	m_debug = false;
}

vmon_m2h_client_ep::~vmon_m2h_client_ep() {
	// TODO Auto-generated destructor stub
}

int32_t vmon_m2h_client_ep::write(void *data, uint32_t sz) {
	// Append the data to the buffer

	if ((m_data_write_idx-m_data_read_idx+sz) > m_data_max) {
		// realloc
		uint32_t new_sz = m_data_max + (((sz-1)/1024)+1)*1024;
		uint8_t *data_t = m_data;
		m_data = new uint8_t [new_sz];

		if (data_t) {
			if (m_data_write_idx > 0) {
				::memcpy(m_data, data_t, m_data_write_idx);
			}
			delete [] data_t;
		}
	}

	::memcpy(&m_data[m_data_write_idx], data, sz);
	m_data_write_idx += sz;

	process();

	return sz;
}

void vmon_m2h_client_ep::process() {
	bool done = false;
	if (avail() == 0) {
		fprintf(stdout, "Internal Error: process called without data\n");
		return;
	}

	do {
		switch (m_msg_state) {
		case MsgState_Cmd: {
			m_msg_cmd = getb();

			if (m_debug) {
				fprintf(stdout, "MsgState_Cmd: %d\n", m_msg_cmd);
			}

			switch (m_msg_cmd) {
			case VMON_MSG_PING_REQ:
			case VMON_MSG_PING_ACK:
			case VMON_MSG_RSP_OK:
			case VMON_MSG_RSP_ERR:
				// Single-byte message
				m_client->process_msg(m_msg_cmd, 0, 0, 0);
				break;
			case VMON_MSG_FIXLEN_REQ:
				m_msg_state = MsgState_FixedLenHdr;
				break;
			case VMON_MSG_VARLEN_REQ:
				m_msg_state = MsgState_VarLenHdr;
				break;
			default:
				fprintf(stdout, "Error: unknown msg_cmd %d\n", m_msg_cmd);
			}
		} break;

		case MsgState_FixedLenHdr: {
			uint8_t b = getb();
			uint8_t p = 0; // todo: parity((b & 0xFE));

			if (p == (b & 1)) {
				m_msg_ep = (b >> 3); // 7:3
				m_msg_len = 2 << ((b >> 1) & 0x3);
				m_msg_fixed = true;
				m_msg_state = MsgState_Data;

				fprintf(stdout, "FIXED: ep=%d\n", m_msg_ep);
				fprintf(stdout, "FIXED: len=%d\n", m_msg_len);
			} else {
				fprintf(stdout, "fixed-length parity error\n");
			}
			m_msg_state = MsgState_Data;

			if (m_debug) {
				fprintf(stdout, "MsgState_FixedLenHdr: %d\n", m_msg_len);
			}
		} break;

		case MsgState_VarLenHdr: {
			uint8_t h = getb();
			m_msg_ep = (h >> 3);
			m_msg_state = MsgState_VarLenSize;
		} break;

		case MsgState_VarLenSize: {
			if (avail() >= 2) {
				m_msg_len = 0;
				for (int i=0; i<2; i++) {
					m_msg_len |= (getb() << 8*i);
				}
				m_msg_state = MsgState_Data;
			} else {
				done = true;
			}
		} break;

		// Wait until we have a complete message
		case MsgState_Data: {
			uint32_t len = (m_msg_fixed)?m_msg_len:(m_msg_len+1);
			if (avail() >= len) {
				// Message starts at m_data_read_idx;
				m_client->process_msg(
						m_msg_cmd,
						m_msg_ep,
						&m_data[m_data_read_idx],
						m_msg_len);

				// update the state of the buffer
				skip(len);

				m_msg_state = MsgState_Cmd;
			} else {
				done = true;
			}
		} break;

		default: {
			fprintf(stdout, "Error: bad message state %d\n", m_msg_state);
		} break;

		}
	} while (avail() > 0 && !done);
}

uint8_t vmon_m2h_client_ep::getb() {
	uint8_t ret = 0;
	if (m_data_write_idx > m_data_read_idx) {
		ret = m_data[m_data_read_idx++];
	} else {
		fprintf(stdout, "Error: no data: m_data_write_idx=%d m_data_read_idx=%d\n",
				m_data_write_idx, m_data_read_idx);
	}
	return ret;
}

void vmon_m2h_client_ep::skip(uint32_t amt) {
	if (amt >= avail()) {
		// Just reset everything
		m_data_read_idx = 0;
		m_data_write_idx = 0;
	} else {
		// Update the buffer the hard way
		m_data_read_idx += amt;
		::memmove(m_data, &m_data[m_data_read_idx], avail());
	}
}
