
#ifndef INCLUDED_VMON_EP0_MSGS
#define INCLUDED_VMON_EP0_MSGS

typedef enum {
	/**
	 * 16-byte message
	 * [0] 		- command
	 * [8..15] 	- address
	 *
	 * Response:
	 * OK
	 */
	VMON_EP0_EXEC=1,

	/**
	 * 2-byte message
	 * [0]		- command
	 * [1]		- m2h_ep_id
	 */
	VMON_EP0_SET_M2H_EP,

	/**
	 * 2-byte message
	 * [0]		- command
	 * [1]		- h2m_ep_id
	 */
	VMON_EP0_SET_H2M_EP,

	/**
	 * Variable-length message
	 * [0]      - command
	 * [1..N]	- string
	 */
	VMON_EP0_MSG,

	/**
	 * Variable-length message
	 * [0]      - command
	 * [1..8]   - address
	 * [9..10]  - bytes
	 * [11..N]  - data
	 */
	VMON_EP0_WRITE,

	/**
	 * 16-byte message
	 * [0]      - command
	 * [1..8]   - address
	 * [9..10]  - bytes
	 *
	 * Response: variable-length message
	 * [0]      - OK
	 * [1]      -
	 */
	VMON_EP0_READ,

} vmon_ep0_msgs_e;

#endif /* INCLUDED_VMON_EP0_MSGS */
