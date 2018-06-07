
#ifndef INCLUDED_VMON_H2M_IF_H
#define INCLUDED_VMON_H2M_IF_H
#include <stdint.h>

class vmon_h2m_if {
public:
	virtual ~vmon_h2m_if() { }

	virtual int32_t send(void *data, uint32_t len) = 0;
};

#endif /* INCLUDED_VMON_H2M_IF_H */
