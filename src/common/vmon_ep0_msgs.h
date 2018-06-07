
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
	 * Fixed-length message
	 * [1..N]   - trace-point
	 */
	VMON_EP0_TP,

	/**
	 * Variable-length message
	 * [0]      - command
	 * [1..8]   - address
	 * [9..N]  - data
	 */
	VMON_EP0_WRITE,

	/**
	 * 16-byte message
	 * [0]      - command
	 * [1..8]   - address
	 * [9..10]  - n_bytes
	 *
	 * Response: variable-length message
	 * [0]      - OK
	 * [1]      -
	 */
	VMON_EP0_READ,

	/**
	 * 0-byte message
	 *
	 * Response: OK
	 */
	VMON_EP0_EXIT,

	/**
	 * 8-byte message
	 * [0]     - command
	 * [4..7]  - status
	 */
	VMON_EP0_ENDTEST


} vmon_ep0_msgs_e;


// 0x02A5 (LE)
#define VMON_EP0_TP24_HDR (0x02 << 8 | VMON_MSG_FIXLEN_REQ)
#define VMON_EP0_TP24_VAL(x) (((x) << 8) + VMON_EP0_TP)

#endif /* INCLUDED_VMON_EP0_MSGS */
