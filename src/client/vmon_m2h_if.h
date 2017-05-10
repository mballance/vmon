
#ifndef INCLUDED_VMON_M2H_IF_H
#define INCLUDED_VMON_M2H_IF_H

class vmon_m2h_if {
public:
	virtual int32_t recv(
			void 		*data,
			uint32_t 	sz,
			int32_t		timeout=-1) = 0;
};

#endif /* INCLUDED_VMON_M2H_IF_H */
