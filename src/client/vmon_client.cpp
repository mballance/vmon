
#include "vmon_client.h"
#include "vmon_msgs.h"
#include "vmon_ep0_msgs.h"

vmon_client::vmon_client() {
	m_m2h_if_id = 0;
	m_h2m_if_id = 0;
	m_connected = false;
}

void vmon_client::add_m2h_if(vmon_m2h_if *i) {
	m_m2h_if.push_back(i);
}

void vmon_client::add_h2m_if(vmon_h2m_if *i) {
	m_h2m_if.push_back(i);
}

bool vmon_client::connect() {
	uint8_t data;

	// NOP
	if (m_connected) {
		return true;
	}

	// Ping monitor and wait for a response
	do {
		outb(VMON_MSG_PING_REQ);

		// Now, wait for some data back
		//
		data = getb(10);
	} while (data != VMON_MSG_PING_ACK); // ping response

	return (data == VMON_MSG_PING_ACK);
}

bool vmon_client::ping() {
	uint8_t data;

	outb(VMON_MSG_PING_REQ);

	// Now, wait for some data back
	//
	data = getb(10);

	return (data == VMON_MSG_PING_ACK);
}

bool vmon_client::exec(uint64_t addr) {
	return send_fixedlen_msg(0, LEN_16, VMON_EP0_EXEC, addr);
}

bool vmon_client::set_m2h_path(uint8_t p) {
	bool ret = send_fixedlen_msg(0, LEN_2,
			(VMON_EP0_SET_M2H_EP | (p << 8)), 0);

	if (ret) {
		m_m2h_if_id = p;
	}

	return ret;
}

bool vmon_client::set_h2m_path(uint8_t p) {
	bool ret = send_fixedlen_msg(0, LEN_2,
			(VMON_EP0_SET_H2M_EP | (p << 8)), 0);

	if (ret) {
		m_h2m_if_id = p;
	}

	return ret;
}

uint8_t vmon_client::parity(uint8_t b) {
	uint8_t ret = 0;
	while (b) {
		if (b&1) {
			ret++;
		}
		b >>= 1;
	}

	return !(ret&1);
}

bool vmon_client::send_fixedlen_msg(
		uint8_t			ep,
		len_t			len,
		uint64_t		data1,
		uint64_t		data2) {
	uint8_t msg[32];
	uint8_t byte_len = 2 << len;
	msg[0] = 0xA5; // fixed-len
	msg[1] = ((ep & 0x1F) << 3) | (len << 1);
	msg[1] |= parity(msg[1]);

	fprintf(stdout, "fixedlen: data1=0x%08llx data2=0x%08llx\n", data1, data2);

	for (uint32_t i=0; i<byte_len; i++) {
		msg[2+i] = (data1 >> 8*i);
	}
	for (uint32_t i=8; i<byte_len; i++) {
		msg[2+i] = (data2 >> 8*(i-8));
	}

	for (uint32_t i=0; i<byte_len; i++) {
		fprintf(stdout, "msg[%d]=%02x\n", i, msg[2+i]);
	}

	m_h2m_if.at(m_h2m_if_id)->send(msg, 2+byte_len);

	uint8_t resp = wait_resp();

	fprintf(stdout, "resp=%02x\n", resp);

	return (resp == VMON_MSG_RSP_OK);
}

uint8_t vmon_client::wait_resp() {
	uint8_t resp;

	while ((resp = getb()) != VMON_MSG_RSP_OK && resp != VMON_MSG_RSP_ERR) {
		switch (resp) {
		case VMON_MSG_FIXLEN_REQ: {
			fprintf(stdout, "TODO: process async message 0x%02x\n", resp);
		} break;

		case VMON_MSG_VARLEN_REQ: {
			fprintf(stdout, "TODO: process async message 0x%02x\n", resp);
		} break;

		default:
			fprintf(stdout, "Error: Unknown async-message %02x\n", resp);
			break;
		}
	}

	return resp;
}

uint8_t vmon_client::getb(int32_t timeout) {
	uint8_t data = 0xFF;
	m_m2h_if.at(m_m2h_if_id)->recv(&data, 1, timeout);
	return data;
}

void vmon_client::outb(uint8_t v) {
	m_h2m_if.at(m_h2m_if_id)->send(&v, 1);
}
