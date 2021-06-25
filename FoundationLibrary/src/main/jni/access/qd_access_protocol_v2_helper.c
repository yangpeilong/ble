//
// Created by 胡涛 on 16/6/11.
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <netinet/in.h>
#include "meshsub.h"
#include "qd_common_utils.h"
#include "qd_access_protocol_v2_define.h"
#include "qd_ble_hook_msg_define.h"
#include "qd_access_protocol_v2_helper.h"
#include "qd_access_protocol_v2_helper_private.h"
#include "common_struct.h"
#include "debugLog.h"

//开门指令
t_buffer build_open_door_msg_v2(const char *const p_target_device_mac,
                                const uint8_t keep_time,
                                const uint32_t app_user_id,
                                const uint8_t phone_type,
                                const char *const agcy) {

    t_buffer content = build_open_door_content_v2(keep_time, app_user_id, phone_type, agcy);

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


t_buffer build_open_door_msg_with_locations_v2(const char *const p_target_device_mac,
                                               const uint8_t keep_time,
                                               const uint32_t app_user_id,
                                               const uint8_t phone_type,
                                               const char *const agcy,
                                               const uint8_t *const p_related_unit_items,
                                               const int sum_of_unit_items) {

    t_buffer content = build_open_door_content_v2(keep_time, app_user_id, phone_type, agcy);

    if (content.p_buffer == NULL) {
        printf("%s:Build content failed!\n", __func__);
        content.len = 0;
        return content;
    }

    if (p_related_unit_items != NULL) {
        t_buffer location_content = build_content_v2(QD_CMD_PUSH_LOCATION, QD_REQUEST,
                                                     p_related_unit_items,
                                                     sizeof(t_qd_unit_item) * sum_of_unit_items);
        if (location_content.p_buffer == NULL) {
            printf("%s:Build location_content failed!\n", __func__);
            location_content.len = 0;
            free(content.p_buffer); // 释放content分配的内存
            return location_content;
        }

        t_buffer msg = build_msg_v2(p_target_device_mac, 2, &content, &location_content);
        free(content.p_buffer); // 释放content分配的内存
        free(location_content.p_buffer);
        if (msg.p_buffer == NULL) {
            printf("%s:Build msg failed!\n", __func__);
            msg.len = 0;
        }
        return msg;
    } else {
        t_buffer msg = build_msg_v2(p_target_device_mac, 1, &content);
        free(content.p_buffer); // 释放content分配的内存
        if (msg.p_buffer == NULL) {
            printf("%s:Build msg failed!\n", __func__);
            msg.len = 0;
        }
        return msg;
    }

}

t_buffer build_open_door_msg_with_all_locations_v2(const char *const p_target_device_mac,
                                                   const uint8_t keep_time,
                                                   const uint32_t app_user_id,
                                                   const uint8_t phone_type,
                                                   const char *const agcy,
                                                   const uint8_t *const p_related_unit_items,
                                                   const int sum_of_unit_items,
                                                   const uint32_t *const p_history_units,
                                                   const int sum_of_history_units) {
    t_buffer open_door_content = build_open_door_content_v2(keep_time, app_user_id, phone_type, agcy);

    if (open_door_content.p_buffer == NULL) {
        printf("%s:Build content failed!\n", __func__);
        open_door_content.len = 0;
        return open_door_content;
    }

    if (p_related_unit_items != NULL && p_history_units != NULL) {
        t_buffer location_content = build_content_v2(QD_CMD_PUSH_LOCATION, QD_REQUEST,
                                                     p_related_unit_items,
                                                     sizeof(t_qd_unit_item) * sum_of_unit_items);
        if (location_content.p_buffer == NULL) {
            printf("%s:Build location_content failed!\n", __func__);
            location_content.len = 0;
            free(open_door_content.p_buffer); // 释放content分配的内存
            return location_content;
        }

        t_buffer history_location_content =
                build_content_v2(QD_CMD_PUSH_HISTORY_LOCATION, QD_REQUEST,
                                 (uint8_t *) p_history_units,
                                 sizeof(uint32_t) * sum_of_history_units);
        if (location_content.p_buffer == NULL) {
            printf("%s:Build history_location_content failed!\n", __func__);
            history_location_content.len = 0;
            free(location_content.p_buffer);
            free(open_door_content.p_buffer); // 释放content分配的内存
            return history_location_content;
        }

        t_buffer msg =
                build_msg_v2(p_target_device_mac, 3, &open_door_content, &location_content,
                             &history_location_content);
        free(open_door_content.p_buffer); // 释放content分配的内存
        free(location_content.p_buffer);
        free(history_location_content.p_buffer);
        if (msg.p_buffer == NULL) {
            printf("%s:Build msg failed!\n", __func__);
            msg.len = 0;
        }
        return msg;
    } else {
        t_buffer msg = build_msg_v2(p_target_device_mac, 1, &open_door_content);
        free(open_door_content.p_buffer); // 释放content分配的内存
        if (msg.p_buffer == NULL) {
            printf("%s:Build msg failed!\n", __func__);
            msg.len = 0;
        }
        return msg;
    }

}

t_buffer build_open_door_msg_with_black_cards_v2(const char *const p_target_device_mac,
                                                 const uint8_t keep_time,
                                                 const uint32_t app_user_id,
                                                 const uint8_t phone_type,
                                                 const char *const agcy,
                                                 const uint8_t *const p_black_card_items,
                                                 const int sum_of_card_item) {

    t_buffer open_door_content = build_open_door_content_v2(keep_time, app_user_id, phone_type, agcy);

    if (open_door_content.p_buffer == NULL) {
        printf("%s:Build content failed!\n", __func__);
        open_door_content.len = 0;
        return open_door_content;
    }

    if (p_black_card_items != NULL) {
        t_buffer black_cards_content = build_content_v2(QD_CMD_PUSH_BLACK_CARD, QD_REQUEST,
                                                        p_black_card_items,
                                                        sizeof(t_qd_card_item) * sum_of_card_item);
        if (black_cards_content.p_buffer == NULL) {
            printf("%s:Build location_content failed!\n", __func__);
            black_cards_content.len = 0;
            free(open_door_content.p_buffer); // 释放content分配的内存
            return black_cards_content;
        }

        t_buffer msg = build_msg_v2(p_target_device_mac, 2, &open_door_content,
                                    &black_cards_content);
        free(open_door_content.p_buffer); // 释放content分配的内存
        free(black_cards_content.p_buffer);
        if (msg.p_buffer == NULL) {
            printf("%s:Build msg failed!\n", __func__);
            msg.len = 0;
        }
        return msg;
    } else {
        t_buffer msg = build_msg_v2(p_target_device_mac, 1, &open_door_content);
        free(open_door_content.p_buffer); // 释放content分配的内存
        if (msg.p_buffer == NULL) {
            printf("%s:Build msg failed!\n", __func__);
            msg.len = 0;
        }
        return msg;
    }
}


t_buffer build_open_door_msg_with_distribute_cards_info_v2(const char *const p_target_device_mac,
                                                           const uint8_t keep_time,
                                                           const uint32_t app_user_id,
                                                           const uint8_t phone_type,
                                                           const char *const agcy,
                                                           int wait_time_pre_card,
                                                           const char *const p_cards_info) {

    t_buffer open_door_content = build_open_door_content_v2(keep_time, app_user_id, phone_type, agcy);

    if (open_door_content.p_buffer == NULL) {
        printf("%s:Build content failed!\n", __func__);
        open_door_content.len = 0;
        return open_door_content;
    }

    // 是否需要构建发卡数据
    if (p_cards_info == NULL) {
        t_buffer msg = build_msg_v2(p_target_device_mac, 1, &open_door_content);
        free(open_door_content.p_buffer); // 释放content分配的内存
        if (msg.p_buffer == NULL) {
            printf("%s:Build msg failed!\n", __func__);
            msg.len = 0;
        }
        return msg;
    }

    // 构建发卡数据
    t_buffer cards_data = build_distribute_cards_data_v2(wait_time_pre_card, p_cards_info);
    if (cards_data.p_buffer == NULL) { //构建失败，只构建开门数据
        printf("%s:Build distribute_card_data failed!\n", __func__);
        t_buffer msg = build_msg_v2(p_target_device_mac, 1, &open_door_content);
        free(open_door_content.p_buffer); // 释放content分配的内存
        if (msg.p_buffer == NULL) {
            printf("%s:Build msg failed!\n", __func__);
            msg.len = 0;
        }
        return msg;
    }

    // 构建发卡数据项
    t_buffer distribute_cards_content = build_content_v2(QD_CMD_DISTRIBUTE_CARD, QD_REQUEST,
                                                         cards_data.p_buffer, cards_data.len);
    if (distribute_cards_content.p_buffer == NULL) {
        printf("%s:Build distribute_card_content failed!\n", __func__);
        distribute_cards_content.len = 0;
        free(open_door_content.p_buffer); // 释放content分配的内存
        return distribute_cards_content;
    }

    printf("%s:****** %d %d ******\r\n", __func__, open_door_content.len,
           distribute_cards_content.len);

    // 构建开门消息
    t_buffer msg = build_msg_v2(p_target_device_mac, 2, &open_door_content,
                                &distribute_cards_content);
    free(open_door_content.p_buffer); // 释放content分配的内存
    free(distribute_cards_content.p_buffer);
    if (msg.p_buffer == NULL) {
        printf("%s:Build msg failed!\n", __func__);
        msg.len = 0;
    }
    return msg;

}

t_buffer build_open_door_ack_v2(const char *const p_target_device_mac, const uint8_t open_result,
                                void *const p_data,
                                const int data_len) {

    t_buffer open_door_content = build_content_v2(QD_CMD_OPEN_DOOR, open_result, p_data, data_len);
    if (open_door_content.p_buffer == NULL) {
        printf("%s:Build content failed!\n", __func__);
        open_door_content.len = 0;
        return open_door_content;
    }


    t_buffer msg = build_msg_v2(p_target_device_mac, 1, &open_door_content);
    free(open_door_content.p_buffer); // 释放content分配的内存
    if (msg.p_buffer == NULL) {
        printf("%s:Build msg failed!\n", __func__);
        msg.len = 0;
    }
    return msg;
}

//配置指令
t_buffer build_config_device_msg_v2(const char *const p_target_device_mac, const uint8_t open_time,
                                    const uint8_t *const p_related_unit_items,
                                    const int sum_of_unit_items) {
    t_buffer data = build_config_data_v2(open_time, p_related_unit_items, sum_of_unit_items);

    if (data.p_buffer == NULL) {
        printf("%s:Build data failed!\n", __func__);
        data.len = 0;
        return data;
    }

    t_buffer content = build_content_v2(QD_CMD_CONFIG_DEVICE, QD_REQUEST, data.p_buffer, data.len);
    free(data.p_buffer); //释放data分配的内存
    if (content.p_buffer == NULL) {
        printf("%s:Build content failed!\n", __func__);
        content.len = 0;
        return content;
    }

    t_buffer msg = build_msg_v2(p_target_device_mac, 1,&content);
    free(content.p_buffer); // 释放content分配的内存
    if (msg.p_buffer == NULL) {
        printf("%s:Build msg failed!\n", __func__);
        msg.len = 0;
    }
    return msg;
}

t_buffer build_config_device_msg_with_relay_state_and_phone_v2(const char *const p_target_device_mac,
                                                     const uint8_t open_time,
                                                     const uint8_t *const p_related_unit_items,
                                                     const int sum_of_unit_items,
                                                     const uint8_t relay_state,
                                                     const uint32_t phone,
                                                     const char* const agcy)
{
    //构造配置信息
    t_buffer data = build_config_data_v2(open_time, p_related_unit_items, sum_of_unit_items);

    if (data.p_buffer == NULL) {
        printf("%s:Build data failed!\n", __func__);
        data.len = 0;
        return data;
    }

    //数据集
    t_buffer content_buffers[4] = {[0 ... 3] = {NULL, 0}};
    int content_size = 0;

    t_buffer config_content = build_content_v2(QD_CMD_CONFIG_DEVICE, QD_REQUEST, data.p_buffer, data.len);
    free(data.p_buffer); //释放data分配的内存
    if (config_content.p_buffer == NULL) {
        printf("%s:Build config_content failed!\n", __func__);
        config_content.len = 0;
        return config_content;
    } else {
        content_buffers[content_size++] = config_content;
    }

    //构造继电器状态信息
    t_buffer set_relay_content = build_content_v2(QD_CMD_SET_RELAY_STATE, QD_REQUEST, &relay_state, sizeof(relay_state));
    if(set_relay_content.p_buffer == NULL){
        printf("%s:Build set_relay_content failed!\n", __func__);
    } else {
        content_buffers[content_size++] = set_relay_content;
    }

    //构造手机号信息
    t_buffer phone_content = build_content_v2(QD_CMD_RECORD_CONFIG_PHONENO, QD_REQUEST, &phone, sizeof(phone));
    if(phone_content.p_buffer == NULL){
        printf("%s:Build phone_content failed!\n", __func__);
    } else {
        content_buffers[content_size++] = phone_content;
    }

    //构造设备厂商识别码信息
    if(agcy && strlen(agcy) >= sizeof(t_qd_agcy_data)){
        t_qd_agcy_data* p_agcy_data = (t_qd_agcy_data*)agcy;
        t_buffer agcy_content = build_content_v2(QD_CMD_CONFIG_DEVICE_AGCY, QD_REQUEST, (uint8_t *)agcy, sizeof(p_agcy_data->code));
        if(agcy_content.p_buffer == NULL){
            printf("%s:Build agcy_content failed!\n", __func__);
        } else {
            content_buffers[content_size++] = agcy_content;
        }
    }

    //数据封包
    t_buffer msg = build_msg_v2(p_target_device_mac, 4, &content_buffers[0], &content_buffers[1], &content_buffers[2], &content_buffers[3]);
    //释放资源
    for(int i=0; i<content_size; i++){
        free(content_buffers[i].p_buffer);
    }
    if (msg.p_buffer == NULL) {
        printf("%s:Build msg failed!\n", __func__);
        msg.len = 0;
    }
    return msg;
}

t_buffer build_config_device_msg_with_relay_state_all_locations_v2(
                                                                    const char *const p_target_device_mac,
                                                                    const uint8_t open_time,
                                                                    const uint8_t *const p_related_unit_items,
                                                                    const int sum_of_unit_items,
                                                                    uint8_t relay_state,
                                                                    const uint32_t *const p_history_units,
                                                                    const int sum_of_history_units)
{

    t_buffer data = build_config_data_v2(open_time, p_related_unit_items, sum_of_unit_items);

    if (data.p_buffer == NULL) {
        printf("%s:Build data failed!\n", __func__);
        data.len = 0;
        return data;
    }

    t_buffer config_content = build_content_v2(QD_CMD_CONFIG_DEVICE, QD_REQUEST, data.p_buffer,
                                               data.len);
    free(data.p_buffer); //释放data分配的内存
    if (config_content.p_buffer == NULL) {
        printf("%s:Build config_content failed!\n", __func__);
        config_content.len = 0;
        return config_content;
    }

    t_buffer
            set_relay_content = build_content_v2(QD_CMD_SET_RELAY_STATE, QD_REQUEST, &relay_state,
                                                 sizeof(relay_state));

    t_buffer history_location_content = build_content_v2(QD_CMD_PUSH_HISTORY_LOCATION,
                                                         QD_REQUEST,
                                                         (uint8_t *) p_history_units,
                                                         sizeof(uint32_t) * sum_of_history_units);

    if (set_relay_content.p_buffer == NULL || history_location_content.p_buffer == NULL) {
        printf("%s:Build config_content failed!\n", __func__);
        t_buffer msg = build_msg_v2(p_target_device_mac, 1, &config_content);
        free(config_content.p_buffer); // 释放content分配的内存
        if (msg.p_buffer == NULL) {
            printf("%s:Build msg failed!\n", __func__);
            msg.len = 0;
        }
        return msg;
    } else {
        t_buffer msg = build_msg_v2(p_target_device_mac, 3, &config_content, &set_relay_content,
                                    &history_location_content);
        free(config_content.p_buffer); // 释放content分配的内存
        free(set_relay_content.p_buffer);
        if (msg.p_buffer == NULL) {
            printf("%s:Build msg failed!\n", __func__);
            msg.len = 0;
        }
        return msg;
    }

}

t_buffer build_config_device_ack_v2(const char *const p_target_device_mac,
                                    const uint8_t config_result) {
    t_buffer content = build_content_v2(QD_CMD_CONFIG_DEVICE, config_result, NULL, 0);
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

//升级指令
t_buffer build_update_device_msg_v2(const char *const p_target_device_mac,
                                    const uint8_t update_type,
                                    const char *const p_download_addr) {
    t_buffer data = build_update_device_data_v2(update_type, p_download_addr);

    if (data.p_buffer == NULL) {
        printf("%s:Build data failed!\n", __func__);
        data.len = 0;
        return data;
    }

    t_buffer content = build_content_v2(QD_CMD_UPDATE_SYSTEM, QD_REQUEST, data.p_buffer, data.len);
    free(data.p_buffer); //释放data分配的内存
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

t_buffer build_update_device_ack_v2(const char *const p_target_device_mac,
                                    const uint8_t update_result) {

    t_buffer content = build_content_v2(QD_CMD_UPDATE_SYSTEM, update_result, NULL, 0);
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

//获取版本信息指令
t_buffer build_get_device_version_msg_v2(const char *const p_target_device_mac) {

    t_buffer content = build_content_v2(QD_CMD_GET_VERSION, QD_REQUEST, NULL, 0);
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

t_buffer build_get_device_version_ack_v2(const char *const p_target_device_mac,
                                         const char *const p_software_version,
                                         const char *const p_hardware_version) {
    t_buffer data = build_get_device_version_ack_data_v2(p_software_version, p_hardware_version);

    if (data.p_buffer == NULL) {
        printf("%s:Build data failed!\n", __func__);
        data.len = 0;
        return data;
    }

    t_buffer content = build_content_v2(QD_CMD_GET_VERSION, QD_ACK_OK, data.p_buffer, data.len);
    free(data.p_buffer); //释放data分配的内存
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

//获取配置信息指令
t_buffer build_get_device_config_msg_v2(const char *const p_target_device_mac) {

    t_buffer content = build_content_v2(QD_CMD_GET_CONFIG, QD_REQUEST, NULL, 0);
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

t_buffer build_get_device_config_ack_v2(const char *const p_target_device_mac,
                                        const uint8_t open_keep_time,
                                        const char *const p_software_version,
                                        const char *const p_hardware_version,
                                        const uint8_t *const p_related_units,
                                        const int sum_of_units) {

    t_buffer data = build_get_device_config_ack_data_v2(open_keep_time, p_software_version,
                                                        p_hardware_version,
                                                        p_related_units, sum_of_units);

    if (data.p_buffer == NULL) {
        printf("%s:Build data failed!\n", __func__);
        data.len = 0;
        return data;
    }

    t_buffer content = build_content_v2(QD_CMD_GET_CONFIG, QD_ACK_OK, data.p_buffer, data.len);
    free(data.p_buffer); //释放data分配的内存
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

//协议不支持应答
t_buffer build_protocol_error_ack_v2(const char *const p_target_device_mac) {

    t_buffer content = build_content_v2(QD_CMD_OPEN_DOOR, QD_ACK_ERR, NULL, 0);
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

//设置继电器默认状态指令


t_buffer build_set_relay_state_msg_v2(const char *const p_target_device_mac,
                                      const uint8_t set_state) {

    t_buffer content = build_content_v2(QD_CMD_SET_RELAY_STATE, QD_REQUEST, &set_state,
                                        sizeof(set_state));
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

t_buffer build_set_relay_state_ack_v2(const char *const p_target_device_mac,
                                      const uint8_t set_state) {
    t_buffer content = build_content_v2(QD_CMD_SET_RELAY_STATE, QD_ACK_OK, &set_state,
                                        sizeof(set_state));
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

// 启用433测试模式
t_buffer build_protocol_enable_433_test_msg_v2(const char *const p_target_device_mac) {
    t_buffer content = build_content_v2(QD_CMD_ENABLE_433_TEST, QD_REQUEST, NULL, 0);
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

t_buffer build_protocol_enable_433_test_ack_v2(const char *const p_target_device_mac) {
    t_buffer content = build_content_v2(QD_CMD_ENABLE_433_TEST, QD_ACK_OK, NULL, 0);
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

// 获取霍尔开关的状态
t_buffer build_protocol_get_hal_state_msg_v2(const char *const p_target_device_mac) {
    t_buffer content = build_content_v2(QD_CMD_GET_HAL_STATE, QD_REQUEST, NULL, 0);
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

t_buffer build_protocol_get_hal_state_ack_v2(const char *const p_target_device_mac,
                                             const uint8_t state) {
    t_buffer content = build_content_v2(QD_CMD_GET_HAL_STATE, QD_ACK_OK, &state, sizeof(state));
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

// 读卡UID & 写卡
t_buffer build_protocol_get_card_uid_msg_v2(const char *const p_target_device_mac) {
    t_buffer content = build_content_v2(QD_CMD_READ_CARD_UID, QD_REQUEST, NULL, 0);
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

t_buffer build_protocol_get_card_uid_ack_v2(const char *const p_target_device_mac,
                                            const uint8_t state) {
    t_buffer content = build_content_v2(QD_CMD_READ_CARD_UID, state, NULL, 0);
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


t_buffer build_protocol_report_card_uid_ack_v2(const char *const p_target_device_mac,
                                               const uint8_t state, const uint8_t *const p_data,
                                               uint32_t data_len) {
    t_buffer content = build_content_v2(QD_CMD_READ_CARD_UID, state, p_data, data_len);
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


t_buffer build_protocol_write_card_data_msg_v2(const char *const p_target_device_mac,
                                               int wait_time_pre_card,
                                               const char *const p_write_data) {
    t_buffer data = build_distribute_cards_data_v2(wait_time_pre_card, p_write_data);

    if (data.p_buffer == NULL) {
        printf("%s:Build data failed!\n", __func__);
        return data;
    }

    t_buffer content = build_content_v2(QD_CMD_DISTRIBUTE_CARD, QD_REQUEST, data.p_buffer,
                                        data.len);
    free(data.p_buffer);
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


t_buffer build_protocol_cancel_write_card_data_msg_v2(const char *const p_target_device_mac) {
    LOGI("build_protocol_cancel_write_card_data_msg_v2");
    t_buffer content = build_content_v2(QD_CMD_CANCEL_DISTRIBUTE_CARD, QD_REQUEST, NULL, 0);
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

t_buffer build_protocol_write_card_data_ack_v2(const char *const p_target_device_mac,
                                               const uint8_t state) {
    t_buffer content = build_content_v2(QD_CMD_DISTRIBUTE_CARD, state, NULL, 0);
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

// 发送升级包描述信息
t_buffer build_protocol_update_file_desc_msg_v2(const char *const p_target_device_mac,
                                                int version_num, int file_len, const char md5[33]) {
    t_buffer msg = {0};
    if (md5 == NULL) {
        return msg;
    }

    t_qd_update_file_desc_data_v2 data = {0};
    data.file_len = file_len;
    data.update_file_version_num = version_num;
    memcpy(data.file_md5, md5, 32);

    t_buffer update_file_desc_content = build_content_v2(QD_CMD_UPDATE_FILE_DESC, QD_REQUEST,
                                                         (uint8_t *) &data,
                                                         sizeof(t_qd_update_file_desc_data_v2));
    if (update_file_desc_content.p_buffer == NULL) {
        printf("%s:Build update_file_desc_content failed!\n", __func__);
        update_file_desc_content.len = 0;
        return update_file_desc_content;
    }

    msg = build_msg_v2(p_target_device_mac, 1, &update_file_desc_content);
    free(update_file_desc_content.p_buffer);
    if (msg.p_buffer == NULL) {
        printf("%s:Build msg failed!\n", __func__);
        msg.len = 0;
    }
    LOGI("build_protocol_update_file_desc_msg_v2 len == %d  ", msg.len);
    return msg;
}


// 发送升级包描述信息
t_buffer
build_protocol_update_file_desc_msg_v3(const char *const p_target_device_mac, int version_num,
                                       int file_len, const uint32_t const checksum_crc32) {
    LOGI("build_protocol_update_file_desc_msg_v3 step1");
    t_buffer msg = {0};
    if (checksum_crc32 == 0) {
        return msg;
    }
    LOGI("build_protocol_update_file_desc_msg_v3 step2");
    t_qd_update_file_desc_data_v3 data = {0};
    data.file_len = file_len;
    data.update_file_version_num = version_num;
    data.file_checksum_crc32 = checksum_crc32;
    //memcpy(data.file_md5,md5,32);
    LOGI("build_protocol_update_file_desc_msg_v3 step3");
    // 构建发卡数据项
    t_buffer update_file_desc_content = build_content_v2(QD_CMD_UPDATE_FILE_DESC, QD_REQUEST, &data,
                                                         sizeof(t_qd_update_file_desc_data_v3));
    LOGI("build_protocol_update_file_desc_msg_v3 step4");
    if (update_file_desc_content.p_buffer == NULL) {
        printf("%s:Build update_file_desc_content failed!\n", __func__);

        update_file_desc_content.len = 0;
        return update_file_desc_content;
    }

// 构建开门消息
    msg = build_msg_v2(p_target_device_mac, 1, &update_file_desc_content);
    free(update_file_desc_content.p_buffer);
    if (msg.p_buffer == NULL) {
        printf("%s:Build msg failed!\n", __func__);
        msg.len = 0;
    }
    LOGI("update_file_desc_content.len = %d", update_file_desc_content.len);
    return msg;
}

//构建请求设备当前版本以及当前接收到的升级文件位置信息
t_buffer build_update_state_request_msg_v3(const char *const p_target_device_mac) {
    t_buffer content = build_content_v2(QD_CMD_UPDATE_REQUEST, QD_REQUEST, NULL, 0);
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

// 分片升级
t_buffer build_protocol_transport_update_file_msg_v2(const char *const p_target_device_mac,
                                                     int seek_index, const uint8_t *const p_data,
                                                     int data_len) {


    t_buffer data = build_update_transport_fragment_data_v2(seek_index, p_data, data_len);

    if (data.p_buffer == NULL) {
        printf("%s:Build data failed!\n", __func__);
        data.len = 0;
        return data;
    }


    t_buffer content = build_content_v2(QD_CMD_UPDATE_FILE_TRANSPORT, QD_REQUEST, data.p_buffer,
                                        data.len);
    free(data.p_buffer);

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
    LOGI("build_protocol_transport_update_file_msg_v2 len == %d  ", msg.len);
    return msg;
}


//---------- 分割线 ---------
// 快速开门使用
t_buffer build_phone_advertise_msg_with_floors_v2(const char *p_target_dev_mac, uint8_t version, uint32_t app_user_id, const uint8_t *const p_floors, const uint8_t sum_of_floors, const char *const agcy_code) {
    LOGI("build_phone_advertise_msg_v2 MAC: %s version：%c , app_user_id: %d", p_target_dev_mac, version, app_user_id);
    t_buffer msg = {0};
    // 参数检查
    if (p_target_dev_mac == NULL) {
        msg.p_buffer = NULL;
        msg.len = 0;
        printf("Parameter error\n");
        return msg;
    }

    t_phone_adv_msg tmp_adv_msg;
    tmp_adv_msg.version = version;
    tmp_adv_msg.app_user_id = app_user_id;
    tmp_adv_msg.time = (uint32_t) time(NULL);
    memset(tmp_adv_msg.reserved, 0, 7);
    //添加楼层
    if(sum_of_floors > 0){
        int size = sum_of_floors > 7 ? 7 : sum_of_floors;
        for (int i=0; i<size; i++) {
            if(*(p_floors + i) >= 64)
            {
                msg.p_buffer = NULL;
                msg.len = 0;
                printf("floor data error\n");
                return msg;
            }
            tmp_adv_msg.reserved[i] = p_floors[i];
        }
    }
    //添加设备厂商码
    uint8_t agcy_data[] = {0x00, 0x00};
    size_t agcy_size = sizeof(agcy_data);
    if(agcy_code && strlen(agcy_code) >= agcy_size){
        memcpy(agcy_data, agcy_code, agcy_size);
    }

    tmp_adv_msg.reserved[0] |= ((agcy_data[0] << 1) & 0xC0);
    tmp_adv_msg.reserved[1] |= ((agcy_data[0] << 3) & 0xC0);
    tmp_adv_msg.reserved[2] |= ((agcy_data[0] << 5) & 0xC0);

    tmp_adv_msg.reserved[3] |= ((agcy_data[0] << 7) & 0x80);
    tmp_adv_msg.reserved[3] |= ((agcy_data[1]) & 0x40);

    tmp_adv_msg.reserved[4] |= ((agcy_data[1] << 2) & 0xC0);
    tmp_adv_msg.reserved[5] |= ((agcy_data[1] << 4) & 0xC0);
    tmp_adv_msg.reserved[6] |= ((agcy_data[1] << 6) & 0xC0);


    //数据加密
    int encrypt_len = sizeof(t_phone_adv_msg) - 3;
    tmp_adv_msg.key = ble_encrypt((uint8_t *) p_target_dev_mac, strlen(p_target_dev_mac),
                                  (uint8_t *) &tmp_adv_msg.app_user_id, encrypt_len);

    char buf[64] = {0};
    char *p_str = base64_encode_ht(&tmp_adv_msg.version, buf, sizeof(t_phone_adv_msg));
    if (p_str == NULL) {
        LOGI("Base64_encode failed");
        printf("Base64_encode failed\n");
        return msg;
    }

    msg.len = strlen(p_str) + 1;
    msg.p_buffer = malloc(msg.len);

    if (msg.p_buffer == NULL) {
        LOGI("%s:Memory malloc failed", __func__);
        printf("%s:Memory malloc failed\n", __func__);
        msg.len = 0;
        return msg;
    }
    memset(msg.p_buffer, 0, msg.len);
    memcpy(msg.p_buffer, p_str, msg.len);
    return msg;
}

//-------------------- 四代协议 -------------------
t_buffer build_open_door_msg_v4(const char *const p_target_device_mac,
                                const uint8_t keep_time,
                                const uint32_t app_user_id,
                                const uint8_t phone_type,
                                const uint32_t pass_id,
                                const char *const agcy) {

    t_buffer content = build_open_door_content_v4(keep_time, app_user_id, phone_type, pass_id, agcy);

    if (content.p_buffer == NULL) {
        printf("%s:Build content failed!\n", __func__);
        content.len = 0;
        return content;
    }

    t_buffer msg = build_msg_v4(p_target_device_mac, 1, &content);
    free(content.p_buffer); // 释放content分配的内存
    if (msg.p_buffer == NULL) {
        printf("%s:Build msg failed!\n", __func__);
        msg.len = 0;
    }
    return msg;
}

t_buffer build_phone_advertise_msg_v4(const char *p_target_dev_mac, uint8_t version, uint32_t app_user_id, uint32_t pass_id) {
    LOGI("build_phone_advertise_msg_v4 MAC: %s version：%c , app_user_id: %d , pass_id :%d", p_target_dev_mac, version, app_user_id, pass_id);
    t_buffer msg = {0};
    // 参数检查
    if (p_target_dev_mac == NULL) {
        msg.p_buffer = NULL;
        msg.len = 0;
        printf("Parameter error\n");
        return msg;
    }

    t_phone_adv_msg_v4 tmp_adv_msg;
    tmp_adv_msg.version = version;
    tmp_adv_msg.app_user_id = app_user_id;
    tmp_adv_msg.time = (uint32_t) time(NULL);
    tmp_adv_msg.pass_id = pass_id;
    int encrypt_len = sizeof(t_phone_adv_msg_v4) - 3;

    tmp_adv_msg.key = ble_encrypt((uint8_t *) p_target_dev_mac, strlen(p_target_dev_mac),
                                  (uint8_t *) &tmp_adv_msg.app_user_id, encrypt_len);

    char buf[64] = {0};
    char *p_str = base64_encode_ht(&tmp_adv_msg.version, buf, sizeof(t_phone_adv_msg_v4));
    if (p_str == NULL) {
        LOGI("Base64_encode failed");
        printf("Base64_encode failed\n");
        return msg;
    }

    msg.len = strlen(p_str) + 1;
    msg.p_buffer = malloc(msg.len);

    if (msg.p_buffer == NULL) {
        LOGI("%s:Memory malloc failed", __func__);
        printf("%s:Memory malloc failed\n", __func__);
        msg.len = 0;
        return msg;
    }
    memset(msg.p_buffer, 0, msg.len);
    memcpy(msg.p_buffer, p_str, msg.len);
    return msg;
}

t_buffer build_config_device_msg_v4(const char *const p_target_device_mac, const char* const agcy){
    LOGD("%s", __FUNCTION__);
    t_buffer data = build_config_data_v4(agcy);
    if (data.p_buffer == NULL) {
        printf("%s:Build data failed!\n", __func__);
        data.len = 0;
        return data;
    }
    t_buffer content = build_content_v2(QD_CMD_CONFIG_DEVICE_AGCY, QD_REQUEST, data.p_buffer, data.len);
    free(data.p_buffer);
    if(content.p_buffer == NULL){
        printf("%s:Build content failed!\n", __func__);
        content.len = 0;
        return content;
    }
    t_buffer msg = build_msg_v4(p_target_device_mac, 1, &content);
    free(content.p_buffer); // 释放content分配的内存
    if (msg.p_buffer == NULL) {
        printf("%s:Build msg failed!\n", __func__);
        msg.len = 0;
    }
    return msg;
}

//获取设备信息
t_buffer build_protocol_get_device_info_msg_v4(const char *const p_target_device_mac) {
    t_buffer content = build_content_v2(QD_CMD_GET_DEVINFO, QD_REQUEST, NULL, 0);
    if (content.p_buffer == NULL) {
        printf("%s:Build content failed!\n", __func__);
        content.len = 0;
        return content;
    }

    t_buffer msg = build_msg_v4(p_target_device_mac, 1, &content);
    free(content.p_buffer); // 释放content分配的内存
    if (msg.p_buffer == NULL) {
        printf("%s:Build msg failed!\n", __func__);
        msg.len = 0;
    }
    return msg;
}

t_buffer build_protocol_get_gate_magnetism_state_msg_v5(const char *const p_target_device_mac)
{
    t_buffer content = build_content_v2(QD_CMD_GET_DOORSENSOR_STATE, QD_REQUEST, NULL, 0);
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

t_buffer build_protocol_get_gate_magnetism_state_ack_v5(const char *const p_target_device_mac, const uint8_t state)
{
    t_buffer content = build_content_v2(QD_CMD_GET_DOORSENSOR_STATE, QD_ACK_OK, &state, sizeof(state));
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

t_buffer build_open_door_msg_v5(const char *const p_target_device_mac,
                                const uint8_t keep_time,
                                const uint64_t app_user_id,
                                const uint8_t phone_type) {
    LOGI("%s", __func__);
    t_buffer content = build_open_door_content_v5(keep_time, app_user_id, phone_type);

    if (content.p_buffer == NULL) {
        printf("%s:Build content failed!\n", __func__);
        content.len = 0;
        return content;
    }

    t_buffer msg = build_msg_v5(p_target_device_mac, 1, &content);

    free(content.p_buffer); // 释放content分配的内存
    if (msg.p_buffer == NULL) {
        printf("%s:Build msg failed!\n", __func__);
        msg.len = 0;
    }
    return msg;
}

t_buffer build_phone_advertise_msg_with_floors_v5(const char *p_target_dev_mac,
                                                  uint8_t version,
                                                  uint64_t app_user_id,
                                                  const uint8_t *const p_floors,
                                                  const uint8_t sum_of_floors) {
    LOGI("%s", __func__);
    t_buffer msg = {0};
    // 参数检查
    if (p_target_dev_mac == NULL) {
        msg.p_buffer = NULL;
        msg.len = 0;
        printf("Parameter error\n");
        return msg;
    }

    t_phone_adv_msg_v5 tmp_adv_msg;
    tmp_adv_msg.version = version;
    tmp_adv_msg.app_user_id = app_user_id;
    tmp_adv_msg.time = (uint32_t) time(NULL);
    memset(tmp_adv_msg.floors, 0, 3);
    memcpy(tmp_adv_msg.floors, p_floors, sum_of_floors > 3 ? 3 : sum_of_floors);
    int encrypt_len = sizeof(t_phone_adv_msg_v5) - 3;

    tmp_adv_msg.key = ble_encrypt((uint8_t *) p_target_dev_mac, strlen(p_target_dev_mac),
                                  (uint8_t *) &tmp_adv_msg.app_user_id, encrypt_len);

    char buf[64] = {0};
    char *p_str = base64_encode_ht(&tmp_adv_msg.version, buf, sizeof(t_phone_adv_msg_v5));
    if (p_str == NULL) {
        printf("Base64_encode failed\n");
        return msg;
    }

    msg.len = strlen(p_str) + 1;
    msg.p_buffer = malloc(msg.len);

    if (msg.p_buffer == NULL) {
        printf("%s:Memory malloc failed\n", __func__);
        msg.len = 0;
        return msg;
    }
    memset(msg.p_buffer, 0, msg.len);
    memcpy(msg.p_buffer, p_str, msg.len);
    return msg;
}