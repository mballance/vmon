
#ifndef INCLUDED_VMON_MSGS_H
#define INCLUDED_VMON_MSGS_H

typedef enum {
	VMON_MSG_PING_REQ=0xEA,
	VMON_MSG_PING_ACK=0xE5,

	/**
	 * [0][7:3] EP ID
	 * [0][2:1] len (0=2, 1=4, 2=8, 3=16)
	 * [0][0]   header parity
	 * [1..1+len] data
	 */
	VMON_MSG_FIXLEN_REQ=0xA5,

	/**
	 * [0][7:3] EP ID
	 * [0]
	 * [1..2]   length
	 * [3..length+3] n bytes
	 * [length] checksum
	 */
	VMON_MSG_VARLEN_REQ=0xA6,

	VMON_MSG_RSP_OK=0xE0,
	VMON_MSG_RSP_ERR=0xE1
} vmon_msgs_e;


#endif /* INCLUDED_VMON_MSGS_H */
