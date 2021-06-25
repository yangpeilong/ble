/*
 * qd_access_protocol_helper_private.c
 *
 *  Created on: Feb 16, 2016
 *      Author: hutao
 */

#include "qdkey.h"
#include "qd_access_protocol_define.h"
#include <stdarg.h>
#include "common_struct.h"

static const unsigned short crc_ta[16] = {0x0000, 0x1021, 0x2042, 0x3063, 0x4084, 0x50a5, 0x60c6, 0x70e7, 0x8108,
                                          0x9129, 0xa14a, 0xb16b, 0xc18c, 0xd1ad, 0xe1ce, 0xf1ef};

//crc校验函数
uint16_t Crc16_DATAs(uint8_t *ptr, uint16_t len) {

    if (ptr == NULL) {
        return 0;
    }
    unsigned char da;
    unsigned short CRC_16_Data = 0;
    while (len-- != 0) {
        da = CRC_16_Data >> 12;
        CRC_16_Data <<= 4;
        CRC_16_Data ^= crc_ta[da ^ (*ptr / 16)];
        da = CRC_16_Data >> 12;
        CRC_16_Data <<= 4;
        CRC_16_Data ^= crc_ta[da ^ (*ptr & 0x0f)];
        ptr++;
    }
    return CRC_16_Data;
}

//加密数据
int encrypt_content(const char *const p_content, int content_len, char *const p_out_holder, int holder_max_len,
                    uint32_t *p_dec_key) {
    if (p_content == NULL || p_out_holder == NULL || p_dec_key == NULL) {
        printf("parameter error\n");
        return -1;
    }

    uint32_t enc_key;
    int rc = qd_generate_key_pair("qdingnet", &enc_key, p_dec_key);
    if (rc == 0) {
        rc = qdmenc((unsigned char *) p_content, content_len, (unsigned char *) p_out_holder, holder_max_len, enc_key);
    }
    return rc;
}

//解密数据
int decrypt_content(const char *const p_content, int content_len, char *const p_out_holder, int holder_max_len,
                    uint32_t dec_key) {
    if (p_content == NULL || p_out_holder == NULL) {
        printf("parameter error\n");
        return -1;
    }

    int rc = qdmdec((unsigned char *) p_content, content_len, (unsigned char *) p_out_holder, holder_max_len, dec_key);

    return rc;
}

//构建消息
t_buffer build_msg(int sum_of_content, ...) {

    int size_of_contents = 0;
    //统计content的总长度
    va_list args_ptr;
    va_start(args_ptr, sum_of_content);
    for (int i = 0; i < sum_of_content; i++) {
        t_buffer *p_tmp_content = va_arg(args_ptr, p_t_buffer);
        //确保传递进来的content都是合法的
        if (p_tmp_content->p_buffer != NULL) {
            size_of_contents += p_tmp_content->len;
//			printf("sum = %d, current = %d\n", size_of_contents, p_tmp_content->len);
        }
    }
    va_end(args_ptr);

    t_buffer msg;
    msg.len = 0;
    msg.p_buffer = NULL;
    msg.len = size_of_contents + sizeof(t_qd_gatectrl_msg) - sizeof(uint8_t);
    msg.p_buffer = malloc(msg.len);

    if (msg.p_buffer == NULL) {
        printf("%s:Memory malloc failed!\n", __func__);
        msg.len = 0;
        return msg;
    }

    t_qd_gatectrl_msg *p_msg_body = (t_qd_gatectrl_msg *) msg.p_buffer;
    va_start(args_ptr, sum_of_content);
    size_of_contents = 0;
    for (int i = 0; i < sum_of_content; i++) {
        t_buffer *p_tmp_content = va_arg(args_ptr, p_t_buffer);
        //确保传递进来的content都是合法的
        if (p_tmp_content->p_buffer != NULL) {
            memcpy(&(p_msg_body->content_start) + size_of_contents, p_tmp_content->p_buffer, p_tmp_content->len);
            size_of_contents += p_tmp_content->len;
        }
    }
    va_end(args_ptr);

    p_msg_body->frame_head = QD_MSG_FRAMEHEAD;
    p_msg_body->protocol_version = QD_BLE_GATE_CTRL_VERSION;
    p_msg_body->total_length = msg.len;
    p_msg_body->crc16_check = Crc16_DATAs((uint8_t *) &(p_msg_body->protocol_version),
                                          msg.len - sizeof(p_msg_body->frame_head) - sizeof(p_msg_body->crc16_check));

    return msg;
}

//构建数据项
t_buffer build_content(uint8_t cmd, uint8_t ack_type, uint8_t *const p_data, uint32_t data_len) {
    t_buffer content;
    content.len = 0;
    content.p_buffer = NULL;
//	printf("%s %p %d %02X \n", __func__, p_data, data_len, *p_data);
    //参数检查
    if (p_data == NULL || data_len < 0) {
        printf("%s:Paramater error!\n", __func__);
        return content;
    }

    content.len = sizeof(t_qd_gatectrl_contentbody) - sizeof(uint8_t) + data_len;
    content.p_buffer = malloc(content.len);
    //内存分配检查

    if (content.p_buffer == NULL) {
        printf("%s:Memory malloc failed!\n", __func__);
        content.len = 0;
        return content;
    }

    t_qd_gatectrl_contentbody *p_content = (t_qd_gatectrl_contentbody *) content.p_buffer;
    p_content->content_length = (uint16_t) content.len;
    p_content->command_type = cmd;
    p_content->ack_type = ack_type;
    memcpy(&(p_content->data_start), p_data, data_len);
    return content;
}

//开门命令数据
t_buffer build_open_door_data(uint32_t reported_record_index, uint32_t cached_record_index,
                              const char *const p_mac_list, int keep_time, const char *p_app_user_id,
                              const char *p_room_id,
                              const char *p_server_id, uint8_t phone_type) {

    t_buffer data;
    data.len = 0;
    data.p_buffer = NULL;
    //参数检查
    if (p_mac_list == NULL || p_app_user_id == NULL || p_server_id == NULL || p_room_id == NULL) {
        printf("%s in %s at %d:Paramater error.\n", __FILE__, __func__, __LINE__);
        return data;
    }
    uint32_t dec_key;
    char p_holder[strlen(p_mac_list) * 2 + 1024 * 16];
    if (p_holder == NULL) {
        printf("%s in %s at %d:Memory malloc failed.\n", __FILE__, __func__, __LINE__);
        return data;
    }
    int enc_content_len = encrypt_content(p_mac_list, (int) strlen(p_mac_list), p_holder,
                                          strlen(p_mac_list) * 2 + 1024 * 16, &dec_key);
    if (enc_content_len <= 0) {
        printf("%s in %s at %d:Encrypt content error.\n", __FILE__, __func__, __LINE__);
        return data;
    }

    data.len = sizeof(t_qd_opendoor_data) + enc_content_len;
    data.p_buffer = malloc(data.len);

    //内存分配检查
    if (data.p_buffer == NULL) {
        printf("%s in %s at %d:Memory malloc failed.\n", __FILE__, __func__, __LINE__);
        data.len = 0;
        return data;
    }

    t_qd_opendoor_data *p_tmp_data = (t_qd_opendoor_data *) data.p_buffer;
    memset(data.p_buffer, 0, data.len);
    //参数有问题，直接释放内存，报错
    if (strlen(p_app_user_id) > (sizeof(p_tmp_data->app_user_id) - 1)) {
        printf("%s:Parameter error, the app_user_id is over than 32 byte string\n", __func__);
        free(data.p_buffer);
        data.p_buffer = NULL;
        data.len = 0;
        return data;
    }
    //参数有问题，直接释放内存，报错
    if (strlen(p_server_id) > (sizeof(p_tmp_data->server_id) - 1)) {
        printf("%s:Parameter error, the p_server_id string is over than 32 byte\n", __func__);
        free(data.p_buffer);
        data.p_buffer = NULL;
        data.len = 0;
        return data;
    }
    //参数有问题，直接释放内存，报错
    if (strlen(p_room_id) > (sizeof(p_tmp_data->room_id) - 1)) {
        printf("%s:Parameter error, the p_room_id string is over than 32 byte\n", __func__);
        free(data.p_buffer);
        data.p_buffer = NULL;
        data.len = 0;
        return data;
    }

    p_tmp_data->reported_record_index = reported_record_index;
    p_tmp_data->cached_record_index = cached_record_index;
    memcpy(p_tmp_data->app_user_id, p_app_user_id, strlen(p_app_user_id));
    memcpy(p_tmp_data->server_id, p_server_id, strlen(p_server_id));
    memcpy(p_tmp_data->room_id, p_room_id, strlen(p_room_id));
    p_tmp_data->create_time = (uint32_t) time(NULL);
    p_tmp_data->keep_time = keep_time;
    p_tmp_data->decrypt_key = dec_key;
    p_tmp_data->phone_type = phone_type;
    memcpy(&(p_tmp_data->encrypted_mac_list_start), p_holder, enc_content_len);
    return data;
}

//开门应答数据
t_buffer build_open_door_ack_data(uint8_t open_result, void *const p_data, int data_len) {

    t_buffer data;
    data.len = 0;
    data.p_buffer = NULL;
    //参数检查
    if (p_data == NULL) {
        printf("%s in %s at %d:Paramater error.\n", __FILE__, __func__, __LINE__);
        return data;
    }

    data.len = sizeof(t_qd_opendoor_ack_data) - sizeof(uint8_t) + data_len;
    data.p_buffer = malloc(sizeof(uint8_t) * data.len);

    //内存分配检查
    if (data.p_buffer == NULL) {
        printf("%s in %s at %d:Memory malloc failed.\n", __FILE__, __func__, __LINE__);
        data.len = 0;
        return data;
    }

    memset(data.p_buffer, 0, data.len);
    t_qd_opendoor_ack_data *p_tmp_data = (t_qd_opendoor_ack_data *) data.p_buffer;
    p_tmp_data->open_result = open_result;
    memcpy(&(p_tmp_data->record_item_start), p_data, data_len);

    return data;
}

//配置命令数据
t_buffer build_config_data(const char *const p_related_doors, int open_time, int max_doors_size, int max_counter_size) {
    t_buffer data;
    data.len = 0;
    data.p_buffer = NULL;
    //参数检查
    if (p_related_doors == NULL) {
        printf("%s in %s at %d:Paramater error.\n", __FILE__, __func__, __LINE__);
        return data;
    }
    data.len = sizeof(t_qd_config_device_data) + strlen(p_related_doors);
    data.p_buffer = malloc(sizeof(uint8_t) * data.len);

    //内存分配检查
    if (data.p_buffer == NULL) {
        printf("%s in %s at %d:Memory malloc failed.\n", __FILE__, __func__, __LINE__);
        data.len = 0;
        return data;
    }

    memset(data.p_buffer, 0, data.len);
    t_qd_config_device_data *p_config_device_data = (t_qd_config_device_data *) data.p_buffer;
    p_config_device_data->keep_time = open_time;
    p_config_device_data->setting_time = (uint32_t) time(NULL);
    p_config_device_data->max_doors_size = max_doors_size;
    p_config_device_data->max_counter_size = max_counter_size;
    memcpy(&(p_config_device_data->encrypted_related_devices_list_start), p_related_doors, strlen(p_related_doors));

    return data;
}

//设备升级命令
t_buffer build_update_device_data(uint8_t update_type, const char *const p_download_addr) {
    t_buffer data;
    data.len = 0;
    data.p_buffer = NULL;
    //参数检查
    if (p_download_addr == NULL) {
        printf("%s in %s at %d:Paramater error.\n", __FILE__, __func__, __LINE__);
        return data;
    }
#if 0
    uint32_t dec_key;
    char p_holder[strlen(p_download_addr) * 2 + 1024 * 16];
    if (p_holder == NULL) {
        printf("%s in %s at %d:Memory malloc failed.\n", __FILE__, __func__, __LINE__);
        return data;
    }
    int enc_content_len = encrypt_content(p_download_addr, (int) strlen(p_download_addr), p_holder,
            strlen(p_download_addr) * 2 + 1024 *16 , &dec_key);
    if (enc_content_len <= 0) {
        printf("%s in %s at %d:Encrypt content error.\n", __FILE__, __func__, __LINE__);
        return data;
    }

    data.len = sizeof(t_qd_update_device_data) - sizeof(uint8_t) + enc_content_len;
    data.p_buffer = malloc(sizeof(uint8_t) * data.len);

    //内存分配检查
    if (data.p_buffer == NULL) {
        printf("%s in %s at %d:Memory malloc failed.\n", __FILE__, __func__, __LINE__);
        data.len = 0;
        return data;
    }

    memset(data.p_buffer, 0, data.len);
    t_qd_update_device_data *p_tmp_data = (t_qd_update_device_data *) data.p_buffer;

    p_tmp_data->update_type = update_type;
    p_tmp_data->decrypt_key = dec_key;
    memcpy(&(p_tmp_data->encrypted_download_addr_start), p_holder, enc_content_len);
#endif
    //为解决早期版本设备读取下载路径没添加字符串结束符bug,多增加一个字节'\0'
    data.len = sizeof(t_qd_update_device_data) + strlen(p_download_addr) + 1;
    data.p_buffer = malloc(sizeof(uint8_t) * data.len);

    //内存分配检查
    if (data.p_buffer == NULL) {
        printf("%s in %s at %d:Memory malloc failed.\n", __FILE__, __func__, __LINE__);
        data.len = 0;
        return data;
    }

    memset(data.p_buffer, 0, data.len);
    t_qd_update_device_data *p_tmp_data = (t_qd_update_device_data *) data.p_buffer;

    p_tmp_data->update_type = update_type;
    p_tmp_data->decrypt_key = 0x12345678;
    memcpy(&(p_tmp_data->encrypted_download_addr_start), p_download_addr, strlen(p_download_addr));
    return data;
}

//获取设备版本信息
t_buffer build_get_device_version_data(const char *const p_target_mac_addr) {

    t_buffer data;
    data.len = 0;
    data.p_buffer = NULL;
    //参数检查
    if (p_target_mac_addr == NULL) {
        printf("%s in %s at %d:Paramater error.\n", __FILE__, __func__, __LINE__);
        return data;
    }

    uint32_t dec_key;
    char p_holder[strlen(p_target_mac_addr) * 2 + 1024 * 16];
    if (p_holder == NULL) {
        printf("%s in %s at %d:Memory malloc failed.\n", __FILE__, __func__, __LINE__);
        return data;
    }
    int enc_content_len = encrypt_content(p_target_mac_addr, (int) strlen(p_target_mac_addr), p_holder,
                                          strlen(p_target_mac_addr) * 2 + 1024 * 16, &dec_key);
    if (enc_content_len <= 0) {
        printf("%s in %s at %d:Encrypt content error.\n", __FILE__, __func__, __LINE__);
        return data;
    }

    data.len = sizeof(t_qd_get_device_version_data) - sizeof(uint8_t) + enc_content_len;
    data.p_buffer = malloc(sizeof(uint8_t) * data.len);

    //内存分配检查
    if (data.p_buffer == NULL) {
        printf("%s in %s at %d:Memory malloc failed.\n", __FILE__, __func__, __LINE__);
        data.len = 0;
        return data;
    }

    memset(data.p_buffer, 0, data.len);
    t_qd_get_device_version_data *p_tmp_data = (t_qd_get_device_version_data *) data.p_buffer;

    p_tmp_data->decrypt_key = dec_key;
    memcpy(&(p_tmp_data->encrypted_mac_addr_start), p_holder, enc_content_len);

    return data;
}

t_buffer build_get_device_version_ack_data(const char *const p_software_version, const char *const p_hardware_version) {
    t_buffer data;
    data.len = 0;
    data.p_buffer = NULL;
    //参数检查
    if (p_software_version == NULL && p_hardware_version == NULL) {
        printf("%s in %s at %d:Paramater error.\n", __FILE__, __func__, __LINE__);
        return data;
    }

    data.len = sizeof(t_qd_get_device_version_ack_data);
    data.p_buffer = malloc(sizeof(uint8_t) * data.len);

    //内存分配检查
    if (data.p_buffer == NULL) {
        printf("%s in %s at %d:Memory malloc failed.\n", __FILE__, __func__, __LINE__);
        data.len = 0;
        return data;
    }

    memset(data.p_buffer, 0, data.len);
    t_qd_get_device_version_ack_data *p_tmp_data = (t_qd_get_device_version_ack_data *) data.p_buffer;
    //参数有问题，直接释放内存，报错
    if (strlen(p_software_version) > (sizeof(p_tmp_data->software_version) - 1)) {
        printf("%s:Parameter error, the software_version is over than 30 byte string\n", __func__);
        free(data.p_buffer);
        data.p_buffer = NULL;
        data.len = 0;
        return data;

    }
    //参数有问题，直接释放内存，报错
    if (strlen(p_hardware_version) > (sizeof(p_tmp_data->hardware_version) - 1)) {
        printf("%s:Parameter error, the hardware_version string is over than 50 byte\n", __func__);
        free(data.p_buffer);
        data.p_buffer = NULL;
        data.len = 0;
        return data;
    }

    memcpy(p_tmp_data->software_version, p_software_version, strlen(p_software_version));
    memcpy(p_tmp_data->hardware_version, p_hardware_version, strlen(p_hardware_version));
    return data;
}

//获取设备的配置信息
t_buffer build_get_device_config_data(const char *const p_target_mac_addr) {

    t_buffer data;
    data.len = 0;
    data.p_buffer = NULL;
    //参数检查
    if (p_target_mac_addr == NULL) {
        printf("%s in %s at %d:Paramater error.\n", __FILE__, __func__, __LINE__);
        return data;
    }

    uint32_t dec_key;
    char p_holder[strlen(p_target_mac_addr) * 2 + 1024 * 16];
    if (p_holder == NULL) {
        printf("%s in %s at %d:Memory malloc failed.\n", __FILE__, __func__, __LINE__);
        return data;
    }
    int enc_content_len = encrypt_content(p_target_mac_addr, (int) strlen(p_target_mac_addr), p_holder,
                                          strlen(p_target_mac_addr) * 2 + 1024 * 16, &dec_key);
    if (enc_content_len <= 0) {
        printf("%s in %s at %d:Encrypt content error.\n", __FILE__, __func__, __LINE__);
        return data;
    }

    data.len = sizeof(t_qd_get_device_config_data) - sizeof(uint8_t) + enc_content_len;
    data.p_buffer = malloc(sizeof(uint8_t) * data.len);

    //内存分配检查
    if (data.p_buffer == NULL) {
        printf("%s in %s at %d:Memory malloc failed.\n", __FILE__, __func__, __LINE__);
        data.len = 0;
        return data;
    }

    memset(data.p_buffer, 0, data.len);
    t_qd_get_device_config_data *p_tmp_data = (t_qd_get_device_config_data *) data.p_buffer;

    p_tmp_data->decrypt_key = dec_key;
    memcpy(&(p_tmp_data->encrypted_mac_addr_start), p_holder, enc_content_len);

    return data;
}

t_buffer build_get_device_config_ack_data(uint32_t open_keep_time, const char *const p_related_doors,
                                          int max_doors_size, int max_counter_size,
                                          const char *const p_software_version,
                                          const char *const p_hardware_version) {

    t_buffer data;
    data.len = 0;
    data.p_buffer = NULL;
    //参数检查
    if (p_related_doors == NULL || p_software_version == NULL || p_hardware_version == NULL) {
        printf("%s in %s at %d:Paramater error.\n", __FILE__, __func__, __LINE__);
        return data;
    }

    data.len = sizeof(t_qd_get_device_config_ack_data) + strlen(p_related_doors);
    data.p_buffer = malloc(data.len);

    //内存分配检查
    if (data.p_buffer == NULL) {
        printf("%s in %s at %d:Memory malloc failed.\n", __FILE__, __func__, __LINE__);
        data.len = 0;
        return data;
    }
    memset(data.p_buffer, 0, data.len);
    t_qd_get_device_config_ack_data *p_tmp_data = (t_qd_get_device_config_ack_data *) data.p_buffer;

    //参数有问题，直接释放内存，报错
    if (strlen(p_software_version) > (sizeof(p_tmp_data->software_version) - 1)) {
        printf("%s:Parameter error, the software_version is over than 30 byte string\n", __func__);
        free(data.p_buffer);
        data.p_buffer = NULL;
        data.len = 0;
        return data;
    }
    //参数有问题，直接释放内存，报错
    if (strlen(p_hardware_version) > (sizeof(p_tmp_data->hardware_version) - 1)) {
        printf("%s:Parameter error, the hardware_version string is over than 50 byte\n", __func__);
        free(data.p_buffer);
        data.p_buffer = NULL;
        data.len = 0;
        return data;
    }

    p_tmp_data->open_keep_time = open_keep_time;
    p_tmp_data->max_doors_size = max_doors_size;
    p_tmp_data->max_counter_size = max_counter_size;
    p_tmp_data->device_time = time(NULL);
    memcpy(p_tmp_data->software_version, p_software_version, strlen(p_software_version));
    memcpy(p_tmp_data->hardware_version, p_hardware_version, strlen(p_hardware_version));
    memcpy(&(p_tmp_data->related_doors_start), p_related_doors, strlen(p_related_doors));
    return data;
}
