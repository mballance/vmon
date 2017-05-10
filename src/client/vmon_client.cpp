
#include "vmon_client.h"

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
		outb(0xEA);

		// Now, wait for some data back
		//
		data = getb(10);
	} while (data != 0xE5); // ping response

	return (data == 0xE5);
}

bool vmon_client::ping() {
	uint8_t data;

	outb(0xEA);

	// Now, wait for some data back
	//
	data = getb(10);

	return (data == 0xE5);
}

bool vmon_client::exec(uint64_t addr) {
	return send_fixedlen_msg(0, LEN_16, 0x5, addr);
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

	for (uint32_t i=0; i<byte_len; i++) {
		msg[2+i] = (data1 >> 8*i);
	}
	for (uint32_t i=8; i<byte_len; i++) {
		msg[2+i] = (data2 >> 8*(i-8));
	}

	m_h2m_if.at(m_h2m_if_id)->send(msg, 2+byte_len);

	uint8_t resp = wait_resp();

	fprintf(stdout, "resp=%02x\n", resp);

	return (resp == 0xE0);
}

uint8_t vmon_client::wait_resp() {
	uint8_t resp;

	while ((resp = getb()) != 0xE0 && resp != 0xE1) {
		fprintf(stdout, "TODO: process async message 0x%02x\n", resp);
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
