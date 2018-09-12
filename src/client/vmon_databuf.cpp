/*
 * vmon_databuf.cpp
 *
 *  Created on: Aug 24, 2018
 *      Author: ballance
 */

#include "vmon_databuf.h"
#include <stdio.h>

vmon_databuf::vmon_databuf(uint8_t *data, uint32_t sz) {
	m_data = data;
	m_sz = sz;
	m_idx = 0;
}

vmon_databuf::~vmon_databuf() {
	// TODO Auto-generated destructor stub
}

uint8_t vmon_databuf::get8() {
	if (m_idx < m_sz) {
		return m_data[m_idx++];
	} else {
		return 0xDE;
	}
}

uint16_t vmon_databuf::get16() {
	if (m_idx+1 < m_sz) {
		uint16_t ret = m_data[m_idx++];
		ret |= (m_data[m_idx++] << 8);
		return ret;
	} else {
		return 0xDEAD;
	}
}

uint32_t vmon_databuf::get32() {
	if (m_idx+3 < m_sz) {
		uint32_t ret = m_data[m_idx++];
		ret |= (m_data[m_idx++] << 8);
		ret |= (m_data[m_idx++] << 16);
		ret |= (m_data[m_idx++] << 24);
		return ret;
	} else {
		return 0xDEADBEEF;
	}
}

