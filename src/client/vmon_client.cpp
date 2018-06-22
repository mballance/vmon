
#include "vmon_client.h"
#include "vmon_msgs.h"
#include "vmon_ep0_msgs.h"
#include "vmon_m2h_client_ep.h"
#include <stdio.h>
#ifndef _WIN32
#include <arpa/inet.h>
#include <elf.h>
#else
#include <winsock2.h>
#include "elf/elf.h"
#endif

vmon_client::vmon_client() {
	m_m2h_if_id = 0;
	m_h2m_if_id = 0;
	m_connected = false;

	m_varlenmax = 0;
	m_varlen = 0;
	m_debug = false;

//	add_ep0_listener(this);
}

vmon_client::~vmon_client() {

}

void vmon_client::add_m2h_if(vmon_m2h_if *i) {
	vmon_m2h_client_ep *client_ep = new vmon_m2h_client_ep(this);
	m_m2h_client_l.push_back(client_ep);
	i->init(client_ep, m_m2h_if.size());
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

void vmon_client::add_ep0_listener(vmon_client_ep0_if *ep0_if) {
	m_ep0_listeners.push_back(ep0_if);
}

uint64_t vmon_client::load(const std::string &path) {

}

uint64_t vmon_client::get_entry_addr(const std::string &path) {
	FILE *fp = fopen(path.c_str(), "rb");
	bool big_endian;

	if (!fp) {
		return ~0;
	}

	Elf32_Ehdr hdr32;
	fread(&hdr32, sizeof(Elf32_Ehdr), 1, fp);

//	for (int i=0; i<16; i++) {
//		fprintf(stdout, "  e_ident[%d]=%02x\n", i, (int)hdr32.e_ident[i]);
//	}

	big_endian = (hdr32.e_ident[EI_DATA] == ELFDATA2MSB);

	if (big_endian) {
		hdr32.e_entry = ntohl(hdr32.e_entry);
	}

	fclose(fp);

	return hdr32.e_entry;
}

bool vmon_client::exec(uint64_t addr) {
	m_endtest = false;
	return send_fixedlen_msg(0, LEN_16, VMON_EP0_EXEC, addr);
}

bool vmon_client::exit() {
	return send_fixedlen_msg(0, LEN_2, VMON_EP0_EXIT, 0);
}

bool vmon_client::read(uint64_t addr, uint8_t *data, uint16_t len) {
	uint8_t cs_t, cs;
	if (!send_fixedlen_msg(0, LEN_16,
			((addr << 8) | VMON_EP0_READ), ((addr >> 56) | (len << 8)))) {
		fprintf(stdout, "Error: bad response\n");
		return false;
	}

	// Now, expect to receive a variable-length response
	uint8_t c;

	if ((c=getb()) != VMON_MSG_VARLEN_REQ) {
		fprintf(stdout, "Error: expect varlen, receive 0x%02x\n", c);
		return false;
	}

	if ((c=getb()) != 0) {
		fprintf(stdout, "Error: expect 0, receive 0x%02x\n", c);
		return false;
	}

	uint16_t len_r = getb();
	len_r |= (getb() << 8);

	if (len_r != len) {
		return false;
	}

	// Finally, receive bytes
	cs_t = 0;
	for (uint32_t i=0; i<len_r; i++) {
		uint8_t tmp = getb();
		data[i] = tmp;
		cs_t += tmp;
	}

	cs = getb();

	if (cs != cs_t) {
		return false;
	}

	return true;
}

bool vmon_client::write(uint64_t addr, uint8_t *data, uint16_t len) {
	uint8_t cs=VMON_EP0_WRITE;
	uint16_t len_t = len + 1 + 8;

	// First, output VARLEN
	outb(VMON_MSG_VARLEN_REQ);

	// Output EP(0)
	outb(0);
	outb(len_t);
	outb(len_t >> 8);

	outb(VMON_EP0_WRITE); // Command

	// Address next
	for (uint32_t i=0; i<8; i++) {
		cs += (addr & 0xFF);
		outb(addr);
		addr >>= 8;
	}

	// Finally, data
	for (uint32_t i=0; i<len; i++) {
		outb(data[i]);
		cs += data[i];
	}

	// Finally, emit checksum
	outb(cs);

	return (wait_resp() == VMON_MSG_RSP_OK);
}

bool vmon_client::wait_endtest(int32_t *status) {
	uint8_t cmd;

	// Process messages
	while (!m_endtest) {

		cmd = getb();

		if (cmd == VMON_MSG_FIXLEN_REQ || cmd == VMON_MSG_VARLEN_REQ) {
			process_async_msg(cmd);
		} else {
			break;
		}
	}
	*status = m_teststatus;

	return m_endtest;
}

bool vmon_client::set_m2h_path(uint8_t p) {
	if (m_connected) {
		bool ret = send_fixedlen_msg(0, LEN_2,
				(VMON_EP0_SET_M2H_EP | (p << 8)), 0);

		if (ret) {
			m_m2h_if_id = p;
		}

		return ret;
	} else {
		m_m2h_if_id = p;
		return true;
	}
}

bool vmon_client::set_h2m_path(uint8_t p) {
	bool ret = send_fixedlen_msg(0, LEN_2,
			(VMON_EP0_SET_H2M_EP | (p << 8)), 0);

	if (ret) {
		m_h2m_if_id = p;
	}

	return ret;
}

void vmon_client::process_msg(uint8_t cmd, uint8_t ep, uint8_t *data, uint32_t sz) {
	if (m_debug) {
		fprintf(stdout, "process_msg: cmd=%d ep=%d sz=%d\n", cmd, ep, sz);
	}
	switch (cmd) {
		case VMON_MSG_PING_REQ:
		case VMON_MSG_PING_ACK:
		case VMON_MSG_RSP_OK:
		case VMON_MSG_RSP_ERR:
			fprintf(stdout, "TODO: received some sort of response (%d)\n", cmd);
			break;

		case VMON_MSG_FIXLEN_REQ:
		case VMON_MSG_VARLEN_REQ:
			if (ep == 0) {
				process_ep0_msg(cmd, ep, data, sz);
			} else {
				fprintf(stdout, "Note: ep=%d\n", ep);
			}
			break;

	}


}

void vmon_client::process_ep0_msg(uint8_t cmd, uint8_t ep, uint8_t *data, uint32_t sz) {
	uint8_t msg_cmd = data[0];

	switch (msg_cmd) {
	case VMON_EP0_MSG: {
		if (m_ep0_listeners.size() > 0) {
			for (std::vector<vmon_client_ep0_if *>::const_iterator it=m_ep0_listeners.begin();
					it!=m_ep0_listeners.end(); it++) {
				(*it)->msg((const char *)&data[1]);
			}
		} else {
			fprintf(stdout, "Note: %s\n", (const char *)&data[1]);
		}
		break;

	case VMON_EP0_TP: {
		uint32_t tp = 0;

		tp = (data[1] | (data[2] << 8) | (data[3] << 16));

		if (m_ep0_listeners.size() > 0) {
			for (std::vector<vmon_client_ep0_if *>::const_iterator it=m_ep0_listeners.begin();
					it!=m_ep0_listeners.end(); it++) {
				(*it)->tracepoint(tp);
			}
		} else {
			fprintf(stdout, "Note: tracepoint %d\n", tp);
		}

	} break;

	case VMON_EP0_ENDTEST: {
		int32_t status = (int32_t)(data[4] | (data[5] << 8) | (data[6] << 16) | (data[7] << 24));

		if (m_ep0_listeners.size() > 0) {
			for (std::vector<vmon_client_ep0_if *>::const_iterator it=m_ep0_listeners.begin();
					it!=m_ep0_listeners.end(); it++) {
				(*it)->endtest(status);
			}
		} else {
			fprintf(stdout, "Note: endtest %d\n", status);
		}
	} break;

	default:
		fprintf(stdout, "Error: unknown EP0 message %d\n", msg_cmd);
		break;
	}
	}
}

void vmon_client::msg(const char *msg) {

}

void vmon_client::endtest(int32_t status) {
	m_endtest = true;
	m_teststatus = status;
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

void vmon_client::send(uint8_t *msg, uint32_t len) {
	uint32_t idx=0;

	while (len > 0) {
		int32_t ret = m_h2m_if.at(m_h2m_if_id)->send(
				&msg[idx], len);

		if (ret > 0) {
			len -= ret;
			idx += ret;
		}
	}
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

	if (m_debug) {
		fprintf(stdout, "fixedlen: data1=0x%08llx data2=0x%08llx\n", data1, data2);
	}

	for (uint32_t i=0; i<byte_len; i++) {
		msg[2+i] = (data1 >> 8*i);
	}
	for (uint32_t i=8; i<byte_len; i++) {
		msg[2+i] = (data2 >> 8*(i-8));
	}

	send(msg, 2+byte_len);

	uint8_t resp = wait_resp();

	return (resp == VMON_MSG_RSP_OK);
}

uint8_t vmon_client::wait_resp() {
	uint8_t resp;

	while ((resp = getb()) != VMON_MSG_RSP_OK && resp != VMON_MSG_RSP_ERR) {
		process_async_msg(resp);
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

void vmon_client::read_fixedlen_msg(
		uint8_t		*ep,
		uint8_t 	*len,
		uint8_t 	*buf) {
	uint8_t b = getb();
	uint8_t p = parity((b & 0xFE));

	if (p == (b & 1)) {
		*ep = (b >> 3); // 7:3
		*len = 2 << ((b >> 1) & 0x3);

		for (int i=0; i<*len; i++) {
			buf[i] = getb();
		}
	} else {
		// Error
		fprintf(stdout, "fixed-len parity error\n");
	}
}

// Message-processing states
// - Fixed-length
//   - ReadHdr (1)
//   - ReadDat (2, 4, 8, 16)
//   - TODO: do we have checksum over data?
//
// - Variable-length
//   - ReadHdr (1) Header (capture endpoint)
//   - ReadLen (2) Packet length (I thought there was a checksum in here)
//   - ReadDat (N+1) Data and checksum byte
//

uint8_t *vmon_client::read_varlen_msg(
		uint8_t		*ep,
		uint16_t	*len) {
	uint8_t h = getb();
	*ep = (h >> 3);

	*len = 0;
	for (int i=0; i<2; i++) {
		*len |= (getb() << 8*i);
	}

	if (m_varlenmax < *len) {
		uint8_t *tmp = m_varlen;
		m_varlen = new uint8_t[*len];
		if (tmp) {
			delete [] tmp;
		}
	}

	for (int i=0; i<*len; i++) {
		m_varlen[i] = getb();
	}

	// Read checksum
	// TODO: check
	getb();

	return m_varlen;
}

void vmon_client::process_async_msg(uint8_t cmd) {
	switch (cmd) {
	case VMON_MSG_FIXLEN_REQ: {
		uint8_t ep;
		uint8_t len;
		uint8_t buf[16];

		read_fixedlen_msg(&ep, &len, buf);

		if (ep == 0) {
			process_async_ep0_fixedlen(len, buf);
		} else {
			fprintf(stdout, "TODO: process async message 0x%02x on ep %d\n", cmd, ep);
		}
	} break;

	case VMON_MSG_VARLEN_REQ: {
		uint8_t ep;
		uint16_t len;
		uint8_t *data = read_varlen_msg(&ep, &len);

		if (ep == 0) {
			process_async_ep0_varlen(len, data);
		} else {
			fprintf(stdout, "TODO: process async variable-length message 0x%02x\n", cmd);
		}
	} break;

	case VMON_MSG_PING_ACK: {
		// Ignore
	} break;

	default:
		fprintf(stdout, "Error: Unknown async-message %02x\n", cmd);
		break;
	}

}

void vmon_client::process_async_ep0_fixedlen(
		uint32_t			len,
		uint8_t				*buf) {
	switch (buf[0]) {
	case VMON_EP0_ENDTEST: {
		for (std::vector<vmon_client_ep0_if *>::iterator it=m_ep0_listeners.begin();
				it != m_ep0_listeners.end(); it++) {
			uint32_t stat_u = 0;
			for (int i=0; i<4; i++) {
				stat_u |= (buf[4+i] << 8*i);
			}

			// TODO: read status
			(*it)->endtest((int32_t)stat_u);
		}
	} break;

	default: {
		fprintf(stdout, "Error: unknown async ep0 message %d\n", buf[0]);
	} break;
	}
}

void vmon_client::process_async_ep0_varlen(uint32_t len, uint8_t *buf) {
	switch (buf[0]) {
	case VMON_EP0_MSG: {
		fprintf(stdout, "MSG: %s\n", (const char *)&buf[1]);
		for (std::vector<vmon_client_ep0_if *>::iterator it=m_ep0_listeners.begin();
				it != m_ep0_listeners.end(); it++) {
			(*it)->msg((const char *)&buf[1]);
		}
	} break;

	default: {
		fprintf(stdout, "Error: Unknown EP0 variable-length message %d\n", buf[0]);
	} break;
	}
}
