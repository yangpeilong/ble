/*
 * common_struct.h
 *
 *  Created on: Apr 17, 2015
 *      Author: fizt
 */

#ifndef SRC_COMMON_STRUCT_H_
#define SRC_COMMON_STRUCT_H_
#include <stdint.h>

#define QD_MAX_BUFFER_SIZE          1024 * 64

typedef struct t_payload_buf {
    uint8_t buf[QD_MAX_BUFFER_SIZE];
    int32_t len;
}__attribute__ ((packed)) t_payload_buf, *p_t_payload_buf;

typedef struct t_buffer {
    uint8_t * p_buffer;
    int32_t len;
}__attribute__ ((packed)) t_buffer, *p_t_buffer;

typedef struct t_Element {
    uint32_t msg_code;
    int32_t msg_len;
    void *p_message;
}__attribute__ ((packed)) t_Element, *p_t_Element;



#endif /* SRC_COMMON_STRUCT_H_ */
