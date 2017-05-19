#ifndef INCLUDED_VMON_CLIENT_H
#define INCLUDED_VMON_CLIENT_H
#include <vector>
#include <string>
#include <stdint.h>
#include "vmon_h2m_if.h"
#include "vmon_m2h_if.h"
#include "vmon_client_ep0_if.h"

class vmon_client : protected vmon_client_ep0_if {
public:

	vmon_client();

	virtual ~vmon_client();

	void add_m2h_if(vmon_m2h_if *i);

	void add_h2m_if(vmon_h2m_if *i);

	bool connect();

	bool ping();

	void add_ep0_listener(vmon_client_ep0_if *ep0_if);

	/**
	 * Loads an ELF file
	 */
	uint64_t load(const std::string &path);

	bool exec(uint64_t addr);

	bool read(uint64_t addr, uint8_t *data, uint16_t len);

	bool write(uint64_t addr, uint8_t *data, uint16_t len);

	bool wait_endtest(int32_t *status);

	bool set_m2h_path(uint8_t p);

	bool set_h2m_path(uint8_t p);

protected:

	virtual void msg(const char *msg);

	virtual void endtest(int32_t status);

private:

	enum len_t {
		LEN_2,
		LEN_4,
		LEN_8,
		LEN_16
	};

	static uint8_t parity(uint8_t v);

	void send(uint8_t *msg, uint32_t len);

	bool send_fixedlen_msg(
			uint8_t		ep,
			len_t		len,
			uint64_t	data1,
			uint64_t	data2
			);

	/**
	 * Process messages, while waiting for a response
	 */
	uint8_t wait_resp();

	uint8_t getb(int32_t timeout=-1);

	void outb(uint8_t v);

	void read_fixedlen_msg(
			uint8_t		*ep,
			uint8_t		*len,
			uint8_t		*buf);

	// Returns a path to m_varlen
	uint8_t *read_varlen_msg(
			uint8_t		*ep,
			uint16_t	*len);

	void process_async_msg(uint8_t cmd);

	void process_async_ep0_fixedlen(
			uint32_t			len,
			uint8_t				*buf);

	void process_async_ep0_varlen(
			uint32_t			len,
			uint8_t				*buf);

private:
	bool								m_connected;
	std::vector<vmon_m2h_if *>			m_m2h_if;
	uint32_t							m_m2h_if_id;
	std::vector<vmon_h2m_if *>			m_h2m_if;
	uint32_t							m_h2m_if_id;

	bool								m_endtest;
	int32_t								m_teststatus;

	uint32_t							m_varlenmax;
	uint8_t								*m_varlen;

	std::vector<vmon_client_ep0_if *>	m_ep0_listeners;

};

#endif /* INCLUDED_VMON_CLIENT_H */
