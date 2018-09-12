/*
 * vmon_databuf.h
 *
 *  Created on: Aug 24, 2018
 *      Author: ballance
 */

#ifndef INCLUDED_VMON_DATABUF_H
#define INCLUDED_VMON_DATABUF_H
#include <stdint.h>

class vmon_databuf {
public:
	vmon_databuf(uint8_t *data, uint32_t sz);

	virtual ~vmon_databuf();

	uint32_t sz() const { return m_sz; }

	uint8_t at(uint32_t i) const {
		if (i < m_sz) {
			return m_data[i];
		} else {
			return 0;
		}
	}

	uint8_t get8();

	uint16_t get16();

	uint32_t get32();

	uint8_t operator [] (uint32_t i) const {
		return at(i);
	}

	const uint8_t *data() const {
		return m_data;
	}

private:
	uint8_t				*m_data;
	uint32_t			m_sz;
	uint32_t			m_idx;

};

#endif /* SRC_CLIENT_VMON_DATABUF_H_ */
