/*
 * vmon_write_if.h
 *
 *  Created on: Jun 5, 2018
 *      Author: ballance
 */

#ifndef SRC_CLIENT_VMON_WRITE_IF_H_
#define SRC_CLIENT_VMON_WRITE_IF_H_
#include <stdint.h>

class vmon_write_if {
public:

	virtual ~vmon_write_if() { }

	virtual int32_t write(
			void 			*data,
			uint32_t		sz) = 0;
};





#endif /* SRC_CLIENT_VMON_WRITE_IF_H_ */
