#ifndef INCLUDED_VMON_CLIENT_H
#define INCLUDED_VMON_CLIENT_H
#include <vector>
#include <string>
#include <stdint.h>
#include "vmon_h2m_if.h"
#include "vmon_m2h_if.h"

class vmon_client {
public:

	vmon_client();

	void add_m2h_if(vmon_m2h_if *i);

	void add_h2m_if(vmon_h2m_if *i);

	bool connect();

	bool ping();

	/**
	 * Loads an ELF file
	 */
	uint64_t load(const std::string &path);

	bool exec(uint64_t addr);

	bool read(uint64_t addr, uint8_t *data, uint16_t len);

	bool write(uint64_t addr, uint8_t *data, uint16_t len);

	bool set_m2h_path(uint8_t p);

	bool set_h2m_path(uint8_t p);

private:

	enum len_t {
		LEN_2,
		LEN_4,
		LEN_8,
		LEN_16
	};

	static uint8_t parity(uint8_t v);

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

private:
	bool							m_connected;
	std::vector<vmon_m2h_if *>		m_m2h_if;
	uint32_t						m_m2h_if_id;
	std::vector<vmon_h2m_if *>		m_h2m_if;
	uint32_t						m_h2m_if_id;

};

#endif /* INCLUDED_VMON_CLIENT_H */
