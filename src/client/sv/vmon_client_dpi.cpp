/****************************************************************************
 * vmon_client_dpi.cpp
 ****************************************************************************/
#include "vmon_client_dpi.h"
#include "vmon_dpi_client.h"
#include "vmon_write_if.h"
#include <string.h>
#include <stdexcept>
#include <stdio.h>

class vmon_client_dpi_m2h : public virtual vmon_m2h_if {
public:

	virtual void init(vmon_write_if *write_if, uint32_t ep) {
		m_write_if = write_if;
		m_ep = ep;
	}

	virtual void write(uint8_t *data, uint32_t sz) {
		m_write_if->write(data, sz);
	}

	virtual int32_t recv(
			void 		*data,
			uint32_t 	sz,
			int32_t		timeout) {
		uint8_t tmp[64];
		int32_t ret;

		if (_vmon_m2h_if_recv(this, tmp,
				(sz>64)?64:sz, timeout, &ret)) {
				throw std::runtime_error("DPI");
		} else if (ret > 0) {
				memcpy(data, tmp, ret);
		}

		return ret;
	}

private:
	vmon_write_if					*m_write_if;
	uint32_t						m_ep;
};

class vmon_client_dpi_h2m : public virtual vmon_h2m_if {
public:

	virtual int32_t send(void *data, uint32_t len) {
		uint8_t tmp[64];
		int32_t ret=0;

		memset(tmp, 0, sizeof(tmp));
		memcpy(tmp, data, (len>64)?64:len);

		if (_vmon_h2m_if_send(this, tmp,
				(len>64)?64:len, &ret)) {
			throw std::runtime_error("vmon_client_dpi_h2m::send");
		}

		return ret;
	}
};

class vmon_client_dpi_ep_listener : public virtual vmon_client_ep_if {
public:
	vmon_client_dpi_ep_listener(vmon_client *client) {
		m_client = client;
	}

	virtual void process_msg(uint8_t ep, const vmon_databuf &data) {
		_vmon_client_ep_process_msg(m_client, ep, (void *)&data);
	}

private:
	vmon_client				*m_client;

};

void *_vmon_client_new() {
	return new vmon_dpi_client();
}

void _vmon_client_set_ep_listener(void *client_h, uint32_t ep) {
	vmon_client *client = static_cast<vmon_client *>(client_h);
	vmon_client_dpi_ep_listener *l = new vmon_client_dpi_ep_listener(client);
	client->set_ep_listener(ep, l);
}

uint32_t _vmon_databuf_sz(void *databuf_h) {
	return static_cast<const vmon_databuf *>(databuf_h)->sz();
}

uint8_t _vmon_databuf_at(void *databuf_h, uint32_t idx) {
	return static_cast<const vmon_databuf *>(databuf_h)->at(idx);
}

void *_vmon_client_add_m2h_if(void *client_p) {
	vmon_client_dpi_m2h *ret = new vmon_client_dpi_m2h();
	static_cast<vmon_client *>(client_p)->add_m2h_if(ret);
	return ret;
}

int _vmon_m2h_client_ep_write(
		void			*m2h_h,
		uint8_t			*data,
		uint32_t		sz) {
	try {
		static_cast<vmon_client_dpi_m2h *>(m2h_h)->write(data, sz);
	} catch (std::runtime_error &e) {
		return 1;
	}
	return 0;
}

void *_vmon_client_add_h2m_if(void *client_p) {
	vmon_client_dpi_h2m *ret = new vmon_client_dpi_h2m();
	static_cast<vmon_client *>(client_p)->add_h2m_if(ret);
	return ret;
}

int _vmon_client_connect(void *client_p, int *ok) {
	try {
		*ok = static_cast<vmon_client *>(client_p)->connect();
	} catch (std::runtime_error &e) {
		return 1;
	}
	return 0;
}

uint64_t _vmon_client_get_entry_addr(void *client_p, const char *path) {
	return static_cast<vmon_client *>(client_p)->get_entry_addr(path);
}

int _vmon_client_exec(void *client_p, uint64_t addr) {
	try {
		static_cast<vmon_client *>(client_p)->exec(addr);
	} catch (std::runtime_error &e) {
		return 1;
	}
	return 0;
}

int _vmon_client_exit(void *client_p) {
	try {
		static_cast<vmon_client *>(client_p)->exit();
	} catch (std::runtime_error &e) {
		return 1;
	}
	return 0;
}

int _vmon_client_wait_endtest(void *client_p, int *status) {
	try {
		static_cast<vmon_client *>(client_p)->wait_endtest(status);
	} catch (std::runtime_error &e) {
		return 1;
	}
	return 0;
}

int _vmon_client_read(
		void 		*client_p,
		uint64_t 	addr,
		uint8_t 	*data,
		uint32_t 	len) {
	try {
		static_cast<vmon_client *>(client_p)->read(addr, data, len);
	} catch (std::runtime_error &e) {
		return 1;
	}
	return 0;
}
