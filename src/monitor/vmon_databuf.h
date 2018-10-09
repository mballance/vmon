/*
 * vmon_databuf.h
 *
 *  Created on: Oct 8, 2018
 *      Author: ballance
 */

#ifndef INCLUDED_VMON_DATABUF_H
#define INCLUDED_VMON_DATABUF_H
#include <stdint.h>

typedef struct vmon_databuf_s {
	uint32_t		idx;
	uint8_t			data[1];
} vmon_databuf_t;

typedef struct vmon_databuf16_s {
	vmon_databuf_t	base;
	uint8_t			__reserved[16];
} vmon_databuf16_t;

static inline void vmon_databuf_init(vmon_databuf_t *buf) {
	buf->idx = 0;
}

static inline void vmon_databuf_put32(vmon_databuf_t *buf, uint32_t data) {
	buf->data[buf->idx++] = (data >> 0);
	buf->data[buf->idx++] = (data >> 8);
	buf->data[buf->idx++] = (data >> 16);
	buf->data[buf->idx++] = (data >> 24);
}

static inline void vmon_databuf_put16(vmon_databuf_t *buf, uint16_t data) {
	buf->data[buf->idx++] = (data >> 0);
	buf->data[buf->idx++] = (data >> 8);
}

static inline void vmon_databuf_put8(vmon_databuf_t *buf, uint8_t data) {
	buf->data[buf->idx++] = data;
}


#endif /* INCLUDED_VMON_DATABUF_H */
