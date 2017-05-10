#ifndef INCLUDED_VMON_SOCKET_PATH_H
#define INCLUDED_VMON_SOCKET_PATH_H
#include <stdint.h>
#include "vmon_h2m_if.h"
#include "vmon_m2h_if.h"

class vmon_socket_path :
		public virtual vmon_h2m_if,
		public virtual vmon_m2h_if {
public:

	vmon_socket_path(bool h2m);

	virtual int32_t recv(
			void 		*data,
			uint32_t 	sz,
			int32_t		timeout=-1);

	virtual int32_t send(void *data, uint32_t len);

	static int32_t data(void *, uint8_t *, uint32_t);

private:
	bool					m_h2m;
	int						m_harness_fd;
	int						m_monitor_fd;



};

#endif /* INCLUDED_VMON_SOCKET_PATH_H */
