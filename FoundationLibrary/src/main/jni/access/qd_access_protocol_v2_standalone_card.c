
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "qd_access_protocol_v2_helper_private.h"
#include "qd_access_protocol_v2_standalone_card.h"

t_buffer build_standalone_cardno_read_msg_v2(const char *const p_target_device_mac, const uint8_t time_out) {

    t_buffer content = build_content_v2(QD_CMD_STANDALONE_CARDNO_READ, QD_REQUEST, &time_out, sizeof(time_out));
    if (content.p_buffer == NULL) {
        printf("%s:Build content failed!\n", __func__);
        content.len = 0;
        return content;
    }

    t_buffer msg = build_msg_v2(p_target_device_mac, 1, &content);
    free(content.p_buffer); // 释放content分配的内存
    if (msg.p_buffer == NULL) {
        printf("%s:Build msg failed!\n", __func__);
        msg.len = 0;
    }
    return msg;
}

t_buffer build_standalone_cardno_add_msg_v2(const char *const p_target_device_mac, uint32_t card_no, uint32_t time_validity, uint64_t list_floors) {

    t_qd_standalone_cardno_item item = {card_no, time_validity, list_floors};

    t_buffer content = build_content_v2(QD_CMD_STANDALONE_CARDNO_ADD, QD_REQUEST, &item, sizeof(t_qd_standalone_cardno_item));
    if (content.p_buffer == NULL) {
        printf("%s:Build content failed!\n", __func__);
        content.len = 0;
        return content;
    }

    t_buffer msg = build_msg_v2(p_target_device_mac, 1, &content);
    free(content.p_buffer); // 释放content分配的内存
    if (msg.p_buffer == NULL) {
        printf("%s:Build msg failed!\n", __func__);
        msg.len = 0;
    }
    return msg;
}


t_buffer build_standalone_cardno_del_msg_v2(const char *const p_target_device_mac, uint32_t *list_cardno, uint8_t count_cardno) {

    t_buffer content = build_content_v2(QD_CMD_STANDALONE_CARDNO_DEL, QD_REQUEST, list_cardno, count_cardno * sizeof(uint32_t));
    if (content.p_buffer == NULL) {
        printf("%s:Build content failed!\n", __func__);
        content.len = 0;
        return content;
    }

    t_buffer msg = build_msg_v2(p_target_device_mac, 1, &content);
    free(content.p_buffer); // 释放content分配的内存
    if (msg.p_buffer == NULL) {
        printf("%s:Build msg failed!\n", __func__);
        msg.len = 0;
    }
    return msg;
}

t_buffer build_standalone_cardno_ack_v2(const char *const p_target_device_mac, uint8_t operate_type, int operate_result, uint32_t data_result) {
    uint32_t result_data[2] = {operate_result, data_result};

    t_buffer content = build_content_v2(operate_type, QD_ACK_OK, result_data, sizeof(result_data));

    if (content.p_buffer == NULL) {
        printf("%s:Build content failed!\n", __func__);
        content.len = 0;
        return content;
    }

    t_buffer msg = build_msg_v2(p_target_device_mac, 1, &content);
    free(content.p_buffer); // 释放content分配的内存
    if (msg.p_buffer == NULL) {
        printf("%s:Build msg failed!\n", __func__);
        msg.len = 0;
    }
    return msg;
}

t_buffer build_standalone_cardno_read_ack_v2(const char *const p_target_device_mac, uint32_t result) {
    return build_standalone_cardno_ack_v2(p_target_device_mac, QD_CMD_STANDALONE_CARDNO_READ, !result, result);
}

t_buffer build_standalone_cardno_add_ack_v2(const char *const p_target_device_mac, int result) {
    return build_standalone_cardno_ack_v2(p_target_device_mac, QD_CMD_STANDALONE_CARDNO_ADD, result, 0);
}

t_buffer build_standalone_cardno_del_ack_v2(const char *const p_target_device_mac, int result) {
    return build_standalone_cardno_ack_v2(p_target_device_mac, QD_CMD_STANDALONE_CARDNO_DEL, result, 0);
}
