/*
 * qd_access_protocol_helper.c
 *
 *  Created on: Feb 2, 2016
 *      Author: hutao
 */

#include "qd_common_utils.h"
#include "qdkey.h"
#include "qd_access_protocol_helper_private.h"
#include "qd_access_protocol_define.h"

t_buffer build_open_door_msg(uint32_t reported_record_index,
                             uint32_t cached_record_index,
                             const char *const p_mac_list,
                             int keep_time,
                             const char *p_app_user_id,
                             const char *p_room_id,
                             const char *p_server_id,
                             uint8_t phone_type) {
    t_buffer data = build_open_door_data(reported_record_index, cached_record_index, p_mac_list, keep_time,
                                         p_app_user_id, p_server_id, p_room_id, phone_type);

    if (data.p_buffer == NULL) {
        printf("%s:Build data failed!\n", __func__);
        data.len = 0;
        return data;
    }

    t_buffer content = build_content(QD_CMD_OPEN_DOOR, QD_REQUEST, data.p_buffer, data.len);
    free(data.p_buffer); //释放data分配的内存
    if (content.p_buffer == NULL) {
        printf("%s:Build content failed!\n", __func__);
        content.len = 0;
        return content;
    }

    t_buffer msg = build_msg(1, &content);
    free(content.p_buffer); // 释放content分配的内存
    if (msg.p_buffer == NULL) {
        printf("%s:Build msg failed!\n", __func__);
        msg.len = 0;
    }
    return msg;
}

t_buffer build_open_door_msg_with_locations(uint32_t reported_record_index,
                                            uint32_t cached_record_index,
                                            const char *const p_mac_list,
                                            int keep_time,
                                            const char *p_app_user_id,
                                            const char *p_room_id,
                                            const char *p_server_id,
                                            uint8_t phone_type,
                                            const char *p_location) {

    t_buffer data = build_open_door_data(reported_record_index, cached_record_index, p_mac_list, keep_time,
                                         p_app_user_id, p_room_id, p_server_id, phone_type);

    if (data.p_buffer == NULL) {
        printf("%s:Build data failed!\n", __func__);
        data.len = 0;
        return data;
    }

    t_buffer content = build_content(QD_CMD_OPEN_DOOR, QD_REQUEST, data.p_buffer, data.len);
    free(data.p_buffer); //释放data分配的内存
    if (content.p_buffer == NULL) {
        printf("%s:Build content failed!\n", __func__);
        content.len = 0;
        return content;
    }
    if (p_location != NULL) {
        t_buffer location_content = build_content(QD_CMD_PUSH_LOCATION, QD_REQUEST, (uint8_t *const) p_location,
                                                  strlen(p_location) + 1);
        if (location_content.p_buffer == NULL) {
            printf("%s:Build location_content failed!\n", __func__);
            location_content.len = 0;
            free(content.p_buffer); // 释放content分配的内存
            return location_content;
        }

        t_buffer msg = build_msg(2, &content, &location_content);
        free(content.p_buffer); // 释放content分配的内存
        free(location_content.p_buffer);
        if (msg.p_buffer == NULL) {
            printf("%s:Build msg failed!\n", __func__);
            msg.len = 0;
        }
        return msg;
    }
    else {
        t_buffer msg = build_msg(1, &content);
        free(content.p_buffer); // 释放content分配的内存
        if (msg.p_buffer == NULL) {
            printf("%s:Build msg failed!\n", __func__);
            msg.len = 0;
        }
        return msg;
    }
}

t_buffer build_open_door_ack(uint8_t open_result, void *const p_data, int data_len) {

    t_buffer data = build_open_door_ack_data(open_result, p_data, data_len);

    if (data.p_buffer == NULL) {
        printf("%s:Build data failed!\n", __func__);
        data.len = 0;
        return data;
    }

    t_buffer content = build_content(QD_CMD_OPEN_DOOR, QD_ACK_OK, data.p_buffer, data.len);
    free(data.p_buffer); //释放data分配的内存
    if (content.p_buffer == NULL) {
        printf("%s:Build content failed!\n", __func__);
        content.len = 0;
        return content;
    }

    t_buffer msg = build_msg(1, &content);
    free(content.p_buffer); // 释放content分配的内存
    if (msg.p_buffer == NULL) {
        printf("%s:Build msg failed!\n", __func__);
        msg.len = 0;
    }
    return msg;
}

t_buffer build_config_device_msg(const char *const p_related_doors, int open_time, int max_doors_size,
                                 int max_counter_size) {
    t_buffer data = build_config_data(p_related_doors, open_time, max_doors_size, max_counter_size);

    if (data.p_buffer == NULL) {
        printf("%s:Build data failed!\n", __func__);
        data.len = 0;
        return data;
    }

    t_buffer content = build_content(QD_CMD_CONFIG_DEVICE, QD_REQUEST, data.p_buffer, data.len);
    free(data.p_buffer); //释放data分配的内存
    if (content.p_buffer == NULL) {
        printf("%s:Build content failed!\n", __func__);
        content.len = 0;
        return content;
    }

    t_buffer msg = build_msg(1, &content);
    free(content.p_buffer); // 释放content分配的内存
    if (msg.p_buffer == NULL) {
        printf("%s:Build msg failed!\n", __func__);
        msg.len = 0;
    }
    return msg;

}

t_buffer build_config_device_ack(uint8_t config_result) {

    t_buffer content = build_content(QD_CMD_CONFIG_DEVICE, QD_ACK_OK, &config_result, sizeof(config_result));

    if (content.p_buffer == NULL) {
        printf("%s:Build content failed!\n", __func__);
        content.len = 0;
        return content;
    }

    t_buffer msg = build_msg(1, &content);
    free(content.p_buffer); // 释放content分配的内存
    if (msg.p_buffer == NULL) {
        printf("%s:Build msg failed!\n", __func__);
        msg.len = 0;
    }
    return msg;
}

t_buffer build_update_device_msg(uint8_t update_type, const char *const p_download_addr) {
    t_buffer data = build_update_device_data(update_type, p_download_addr);

    if (data.p_buffer == NULL) {
        printf("%s:Build data failed!\n", __func__);
        data.len = 0;
        return data;
    }

    t_buffer content = build_content(QD_CMD_UPDATE_SYSTEM, QD_REQUEST, data.p_buffer, data.len);
    free(data.p_buffer); //释放data分配的内存
    if (content.p_buffer == NULL) {
        printf("%s:Build content failed!\n", __func__);
        content.len = 0;
        return content;
    }

    t_buffer msg = build_msg(1, &content);
    free(content.p_buffer); // 释放content分配的内存
    if (msg.p_buffer == NULL) {
        printf("%s:Build msg failed!\n", __func__);
        msg.len = 0;
    }
    return msg;
}

t_buffer build_update_device_ack(uint8_t update_result) {
    t_buffer content = build_content(QD_CMD_UPDATE_SYSTEM, QD_ACK_OK, &update_result, sizeof(update_result));

    if (content.p_buffer == NULL) {
        printf("%s:Build content failed!\n", __func__);
        content.len = 0;
        return content;
    }

    t_buffer msg = build_msg(1, &content);
    free(content.p_buffer); // 释放content分配的内存
    if (msg.p_buffer == NULL) {
        printf("%s:Build msg failed!\n", __func__);
        msg.len = 0;
    }
    return msg;

}

t_buffer build_get_device_version_msg(const char *const p_target_mac_addr) {
    t_buffer data = build_get_device_version_data(p_target_mac_addr);

    if (data.p_buffer == NULL) {
        printf("%s:Build data failed!\n", __func__);
        data.len = 0;
        return data;
    }

    t_buffer content = build_content(QD_CMD_GET_VERSION, QD_REQUEST, data.p_buffer, data.len);
    free(data.p_buffer); //释放data分配的内存
    if (content.p_buffer == NULL) {
        printf("%s:Build content failed!\n", __func__);
        content.len = 0;
        return content;
    }

    t_buffer msg = build_msg(1, &content);
    free(content.p_buffer); // 释放content分配的内存
    if (msg.p_buffer == NULL) {
        printf("%s:Build msg failed!\n", __func__);
        msg.len = 0;
    }
    return msg;

}

t_buffer build_get_device_version_ack(const char *const p_software_version, const char *const p_hardware_version) {

    t_buffer data = build_get_device_version_ack_data(p_software_version, p_hardware_version);

    if (data.p_buffer == NULL) {
        printf("%s:Build data failed!\n", __func__);
        data.len = 0;
        return data;
    }

    t_buffer content = build_content(QD_CMD_GET_VERSION, QD_ACK_OK, data.p_buffer, data.len);
    free(data.p_buffer); //释放data分配的内存
    if (content.p_buffer == NULL) {
        printf("%s:Build content failed!\n", __func__);
        content.len = 0;
        return content;
    }

    t_buffer msg = build_msg(1, &content);
    free(content.p_buffer); // 释放content分配的内存
    if (msg.p_buffer == NULL) {
        printf("%s:Build msg failed!\n", __func__);
        msg.len = 0;
    }
    return msg;
}

t_buffer build_get_device_config_msg(const char *const p_target_mac_addr) {
    t_buffer data = build_get_device_config_data(p_target_mac_addr);

    if (data.p_buffer == NULL) {
        printf("%s:Build data failed!\n", __func__);
        data.len = 0;
        return data;
    }

    t_buffer content = build_content(QD_CMD_GET_CONFIG, QD_REQUEST, data.p_buffer, data.len);
    free(data.p_buffer); //释放data分配的内存
    if (content.p_buffer == NULL) {
        printf("%s:Build content failed!\n", __func__);
        content.len = 0;
        return content;
    }

    t_buffer msg = build_msg(1, &content);
    free(content.p_buffer); // 释放content分配的内存
    if (msg.p_buffer == NULL) {
        printf("%s:Build msg failed!\n", __func__);
        msg.len = 0;
    }
    return msg;

}

t_buffer build_get_device_config_ack(uint32_t open_keep_time,
                                     const char *const p_related_doors,
                                     int max_doors_size,
                                     int max_counter_size,
                                     const char *const p_software_version,
                                     const char *const p_hardware_version) {

    t_buffer data = build_get_device_config_ack_data(open_keep_time, p_related_doors, max_doors_size, max_counter_size,
                                                     p_software_version, p_hardware_version);

    if (data.p_buffer == NULL) {
        printf("%s:Build data failed!\n", __func__);
        data.len = 0;
        return data;
    }

    t_buffer content = build_content(QD_CMD_GET_CONFIG, QD_ACK_OK, data.p_buffer, data.len);
    free(data.p_buffer); //释放data分配的内存
    if (content.p_buffer == NULL) {
        printf("%s:Build content failed!\n", __func__);
        content.len = 0;
        return content;
    }
    t_buffer msg = build_msg(1, &content);
    free(content.p_buffer); // 释放content分配的内存
    if (msg.p_buffer == NULL) {
        printf("%s:Build msg failed!\n", __func__);
        msg.len = 0;
    }
    return msg;

}

t_buffer build_protocol_error_ack(uint16_t current_protocol_version) {
    t_buffer content = build_content(QD_CMD_OPEN_DOOR, QD_ACK_ERR, (uint8_t *) &current_protocol_version,
                                     sizeof(uint16_t));

    if (content.p_buffer == NULL) {
        printf("%s:Build content failed!\n", __func__);
        content.len = 0;
        return content;
    }

    t_buffer msg = build_msg(1, &content);
    free(content.p_buffer); // 释放content分配的内存
    if (msg.p_buffer == NULL) {
        printf("%s:Build msg failed!\n", __func__);
        msg.len = 0;
    }
    return msg;
}

t_buffer build_protocol_set_relay_state_msg(uint8_t default_state) {
    t_buffer content = build_content(QD_CMD_SET_RELAY_STATE, QD_REQUEST, &default_state, sizeof(default_state));

    if (content.p_buffer == NULL) {
        printf("%s:Build content failed!\n", __func__);
        content.len = 0;
        return content;
    }

    t_buffer msg = build_msg(1, &content);
    free(content.p_buffer); // 释放content分配的内存
    if (msg.p_buffer == NULL) {
        printf("%s:Build msg failed!\n", __func__);
        msg.len = 0;
    }
    return msg;
}
