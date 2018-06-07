

#ifndef INCLUDED_VMON_CLIENT_EP0_IF_H
#define INCLUDED_VMON_CLIENT_EP0_IF_H

class vmon_client_ep0_if {
public:

	virtual ~vmon_client_ep0_if() { }

	virtual void msg(const char *msg) = 0;

	virtual void tracepoint(int unsigned tp) = 0;

	virtual void endtest(int32_t status) = 0;

};
#endif /* INCLUDED_VMON_CLIENT_EP0_IF_H */
