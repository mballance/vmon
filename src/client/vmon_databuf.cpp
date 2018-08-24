/*
 * vmon_databuf.cpp
 *
 *  Created on: Aug 24, 2018
 *      Author: ballance
 */

#include "vmon_databuf.h"

vmon_databuf::vmon_databuf(uint8_t *data, uint32_t sz) {
	m_data = data;
	m_sz = sz;
}

vmon_databuf::~vmon_databuf() {
	// TODO Auto-generated destructor stub
}

