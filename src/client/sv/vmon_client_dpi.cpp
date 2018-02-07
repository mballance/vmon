/****************************************************************************
 * vmon_client_dpi.cpp
 ****************************************************************************/
#include "vmon_client.h"
#include <string.h>
#include <stdexcept>
#include <stdio.h>

extern "C" {
void *_vmon_client_new(void);
int _vmon_m2h_if_recv(
		void			*if_h,
		uint8_t			*data,
		int				size,
		int				timeout,
		int				*ret);

int _vmon_h2m_if_send(
		void			*if_h,
		uint8_t			*data,
		int				size,
		int				*ret);

void *_vmon_client_add_m2h_if(void *);

void *_vmon_client_add_h2m_if(void *);

int _vmon_client_connect(void *, int *);

int _vmon_client_read(void *, uint64_t, uint8_t *, uint32_t);

uint64_t _vmon_client_get_entry_addr(void *, const char *path);

int _vmon_client_exec(void *client_p, uint64_t addr);

int _vmon_client_exit(void *client_p);

int _vmon_client_wait_endtest(void *client_p, int *status);

}

class vmon_client_dpi_m2h : public virtual vmon_m2h_if {
public:

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

void *_vmon_client_new() {
	return new vmon_client();
}

void *_vmon_client_add_m2h_if(void *client_p) {
	vmon_client_dpi_m2h *ret = new vmon_client_dpi_m2h();
	static_cast<vmon_client *>(client_p)->add_m2h_if(ret);
	return ret;
}

void *_vmon_client_add_h2m_if(void *client_p) {
	fprintf(stdout, "client_add_h2m_if: %p\n", client_p);
	vmon_client_dpi_h2m *ret = new vmon_client_dpi_h2m();
	fprintf(stdout, "ret: %p\n", ret);
	static_cast<vmon_client *>(client_p)->add_h2m_if(ret);
	return ret;
}

int _vmon_client_connect(void *client_p, int *ok) {
	try {
		*ok = static_cast<vmon_client *>(client_p)->connect();
	} catch (std::runtime_error) {
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
	} catch (std::runtime_error) {
		return 1;
	}
	return 0;
}

int _vmon_client_exit(void *client_p) {
	try {
		static_cast<vmon_client *>(client_p)->exit();
	} catch (std::runtime_error) {
		return 1;
	}
	return 0;
}

int _vmon_client_wait_endtest(void *client_p, int *status) {
	try {
		static_cast<vmon_client *>(client_p)->wait_endtest(status);
	} catch (std::runtime_error) {
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
	} catch (std::runtime_error) {
		return 1;
	}
	return 0;
}
