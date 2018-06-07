
#ifndef INCLUDED_VMON_M2H_IF_H
#define INCLUDED_VMON_M2H_IF_H
#include <stdint.h>

class vmon_write_if;
class vmon_m2h_if {
public:
	virtual ~vmon_m2h_if() { }

	/**
	 * Initialization call made by client when an
	 * endpoint is registered
	 */
	virtual void init(vmon_write_if *c, uint32_t ep);

	/**
	 * Blocking call made by the client for a primary endpoint
	 */
	virtual int32_t recv(
			void 		*data,
			uint32_t 	sz,
			int32_t		timeout=-1) = 0;
};

#endif /* INCLUDED_VMON_M2H_IF_H */
