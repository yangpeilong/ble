#ifndef _QD_ACCESS_PROTOCOL_V2_STANDALONE_CARD_H_
#define _QD_ACCESS_PROTOCOL_V2_STANDALONE_CARD_H_

#include "common_struct.h"

#ifdef __cplusplus
extern "C" {
#endif

//卡号开卡
#define QD_CMD_STANDALONE_CARDNO_READ       0x30
#define QD_CMD_STANDALONE_CARDNO_ADD        0x31
#define QD_CMD_STANDALONE_CARDNO_DEL        0x32


typedef struct t_qd_standalone_cardno_item {
    uint32_t card_no;       //卡号
    uint32_t time_validity;    // 操作的时间戳
    uint64_t list_floors;   // 操作类型, 包括 注销/激活 两种操作
}__attribute__((packed())) t_qd_standalone_cardno_item, *p_t_qd_standalone_cardno_item;


t_buffer build_standalone_cardno_read_msg_v2(const char *const p_target_device_mac, const uint8_t time_out);

t_buffer build_standalone_cardno_add_msg_v2(const char *const p_target_device_mac, uint32_t card_no, uint32_t time_validity, uint64_t list_floors);

t_buffer build_standalone_cardno_del_msg_v2(const char *const p_target_device_mac, uint32_t *list_cardno, uint8_t count_cardno);

t_buffer build_standalone_cardno_read_ack_v2(const char *const p_target_device_mac, uint32_t result); 

t_buffer build_standalone_cardno_add_ack_v2(const char *const p_target_device_mac, int result);

t_buffer build_standalone_cardno_del_ack_v2(const char *const p_target_device_mac, int result);

#ifdef __cplusplus
}
#endif
#endif
