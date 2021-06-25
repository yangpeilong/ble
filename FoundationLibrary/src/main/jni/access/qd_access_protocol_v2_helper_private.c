//
// Created by 胡涛 on 16/6/11.
//

#include "meshsub.h"
#include "qd_access_protocol_v2_define.h"
#include "qd_access_protocol_v2_helper_private.h"
#include "crc32.h"
#include "debugLog.h"
#include <stdarg.h>
#include <time.h>
#include <stdbool.h>


//构建消息
t_buffer build_msg_v2(const char *const p_target_device_mac, int sum_of_content, ...) {

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
    msg.len = size_of_contents + sizeof(t_qd_access_msg);
    msg.p_buffer = (uint8_t *) malloc(msg.len);

    if (msg.p_buffer == NULL) {
        printf("%s:Memory malloc failed!\n", __func__);
        msg.len = 0;
        return msg;
    }

    t_qd_access_msg *p_msg_body = (t_qd_access_msg *) msg.p_buffer;
    va_start(args_ptr, sum_of_content);
    size_of_contents = 0;
    for (int i = 0; i < sum_of_content; i++) {
        t_buffer *p_tmp_content = va_arg(args_ptr, p_t_buffer);
        //确保传递进来的content都是合法的
        if (p_tmp_content->p_buffer != NULL) {
            memcpy(p_msg_body->content_start + size_of_contents, p_tmp_content->p_buffer,
                   p_tmp_content->len);
            size_of_contents += p_tmp_content->len;
        }
    }
    va_end(args_ptr);

    p_msg_body->frame_head = QD_MSG_FRAMEHEAD;
    p_msg_body->protocol_version = QD_BLE_GATE_CTRL_VERSION_V2;
    p_msg_body->total_length = (uint16_t) msg.len;


#if 1
    printf("readonly = %s\r\n", p_target_device_mac);
    printf("encrypt data:\r\n");
    int data_len = msg.len - sizeof(t_qd_access_msg);
    char test_buf[data_len * 5 + 1];
    for (int i = 0; i < msg.len - sizeof(t_qd_access_msg); i++) {
        printf("0x%02X,", p_msg_body->content_start[i]);
        sprintf(test_buf + i * 5, "0x%02X,", p_msg_body->content_start[i]);
        if ((i + 1) % 16 == 0) {
            printf("\r\n");
        }
    }
    printf("\r\n");
    test_buf[data_len * 5] = 0;
    LOGD("orgin data = %s", test_buf);
#endif

    p_msg_body->decrypt_key =
            ble_encrypt((uint8_t *) p_target_device_mac,
                        (uint16_t) strlen(p_target_device_mac),
                        p_msg_body->content_start,
                        msg.len - sizeof(t_qd_access_msg));

    printf("key= %04X\r\n", p_msg_body->decrypt_key);
    LOGI("key= %04X\r\n", p_msg_body->decrypt_key);


#if 1
    data_len = msg.len - sizeof(t_qd_access_msg);
    char encrypt_buf[data_len * 5 + 1];
    for (int i = 0; i < data_len; i++) {
        sprintf(encrypt_buf + i * 5, "0x%02X,", p_msg_body->content_start[i]);
    }
    encrypt_buf[data_len * 5] = 0;
    LOGD("encrypt data = %s", encrypt_buf);

    char clone_buffer[data_len];
    uint32_t decrypt_key = p_msg_body->decrypt_key;
    memcpy(clone_buffer, p_msg_body->content_start, data_len);
    uint16_t rc = ble_decrypt((uint8_t *) p_target_device_mac,
                              (uint16_t) strlen(p_target_device_mac),
                              clone_buffer,
                              data_len,
                              decrypt_key);

    if (rc == 0) {
        LOGE("解密失败");
    } else {
        LOGE("解密成功");
    }
#endif

    return msg;
}

//构建消息
t_buffer build_msg_v4(const char *const p_target_device_mac, int sum_of_content, ...) {

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
    msg.len = size_of_contents + sizeof(t_qd_access_msg);
    msg.p_buffer = (uint8_t *) malloc(msg.len);

    if (msg.p_buffer == NULL) {
        printf("%s:Memory malloc failed!\n", __func__);
        msg.len = 0;
        return msg;
    }

    t_qd_access_msg *p_msg_body = (t_qd_access_msg *) msg.p_buffer;
    va_start(args_ptr, sum_of_content);
    size_of_contents = 0;
    for (int i = 0; i < sum_of_content; i++) {
        t_buffer *p_tmp_content = va_arg(args_ptr, p_t_buffer);
        //确保传递进来的content都是合法的
        if (p_tmp_content->p_buffer != NULL) {
            memcpy(p_msg_body->content_start + size_of_contents, p_tmp_content->p_buffer, p_tmp_content->len);
            size_of_contents += p_tmp_content->len;
        }
    }
    va_end(args_ptr);

    p_msg_body->frame_head = QD_MSG_FRAMEHEAD;
    p_msg_body->protocol_version = QD_BLE_GATE_CTRL_VERSION_V4;
    p_msg_body->total_length = (uint16_t) msg.len;



#if 1
    printf("readonly = %s\r\n",p_target_device_mac);
    printf("encrypt data:\r\n");
    int data_len = msg.len - sizeof(t_qd_access_msg);
    char test_buf[data_len * 5 +1];
    for (int i =0; i < msg.len - sizeof(t_qd_access_msg);i++ ) {
        printf("0x%02X,",p_msg_body->content_start[i]);
        sprintf(test_buf + i * 5, "0x%02X,",p_msg_body->content_start[i]);
        if ((i+1)%16 ==0 ){
            printf("\r\n");
        }
    }
    printf("\r\n");
    test_buf[data_len * 5] = 0;
    LOGD("orgin data = %s", test_buf);
#endif

    p_msg_body->decrypt_key =
            ble_encrypt((uint8_t *) p_target_device_mac,
                        (uint16_t) strlen(p_target_device_mac),
                        p_msg_body->content_start,
                        msg.len - sizeof(t_qd_access_msg));

    printf("key= %04X\r\n",p_msg_body->decrypt_key);
    LOGI("key= %04X\r\n",p_msg_body->decrypt_key);


#if 1
    data_len = msg.len - sizeof(t_qd_access_msg);
    char encrypt_buf[data_len * 5 +1];
    for (int i = 0; i < data_len ;i++ ) {
        sprintf(encrypt_buf + i * 5, "0x%02X,",p_msg_body->content_start[i]);
    }
    encrypt_buf[data_len * 5] = 0;
    LOGD("encrypt data = %s", encrypt_buf);

    char clone_buffer [data_len];
    uint32_t decrypt_key = p_msg_body->decrypt_key;
    memcpy(clone_buffer,p_msg_body->content_start,data_len);
    uint16_t rc = ble_decrypt((uint8_t *) p_target_device_mac,
                              (uint16_t) strlen(p_target_device_mac),
                              clone_buffer,
                              data_len,
                              decrypt_key);

    if (rc == 0) {
        LOGE("解密失败");
    }else{
        LOGE("解密成功");
    }
#endif

    return msg;
}

//构建消息
t_buffer build_msg_v5(const char *const p_target_device_mac, int sum_of_content, ...) {

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
    msg.len = size_of_contents + sizeof(t_qd_access_msg);
    msg.p_buffer = (uint8_t *) malloc(msg.len);

    if (msg.p_buffer == NULL) {
        printf("%s:Memory malloc failed!\n", __func__);
        msg.len = 0;
        return msg;
    }

    t_qd_access_msg *p_msg_body = (t_qd_access_msg *) msg.p_buffer;
    va_start(args_ptr, sum_of_content);
    size_of_contents = 0;
    for (int i = 0; i < sum_of_content; i++) {
        t_buffer *p_tmp_content = va_arg(args_ptr, p_t_buffer);
        //确保传递进来的content都是合法的
        if (p_tmp_content->p_buffer != NULL) {
            memcpy(p_msg_body->content_start + size_of_contents, p_tmp_content->p_buffer,
                   p_tmp_content->len);
            size_of_contents += p_tmp_content->len;
        }
    }
    va_end(args_ptr);

    p_msg_body->frame_head = QD_MSG_FRAMEHEAD;
    //由于使用相同蓝牙芯片TI2541,协议版本还是保持为v2
    p_msg_body->protocol_version = QD_BLE_GATE_CTRL_VERSION_V2;
    p_msg_body->total_length = (uint16_t) msg.len;

#if 1
    printf("readonly = %s\r\n", p_target_device_mac);
    printf("encrypt data:\r\n");
    int data_len = msg.len - sizeof(t_qd_access_msg);
    char test_buf[data_len * 5 + 1];
    for (int i = 0; i < msg.len - sizeof(t_qd_access_msg); i++) {
        printf("0x%02X,", p_msg_body->content_start[i]);
        sprintf(test_buf + i * 5, "0x%02X,", p_msg_body->content_start[i]);
        if ((i + 1) % 16 == 0) {
            printf("\r\n");
        }
    }
    printf("\r\n");
    test_buf[data_len * 5] = 0;
    LOGD("orgin data = %s", test_buf);
#endif

    p_msg_body->decrypt_key =
            ble_encrypt((uint8_t *) p_target_device_mac,
                        (uint16_t) strlen(p_target_device_mac),
                        p_msg_body->content_start,
                        msg.len - sizeof(t_qd_access_msg));

    printf("key= %04X\r\n", p_msg_body->decrypt_key);
    LOGI("key= %04X\r\n", p_msg_body->decrypt_key);


#if 1
    data_len = msg.len - sizeof(t_qd_access_msg);
    char encrypt_buf[data_len * 5 + 1];
    for (int i = 0; i < data_len; i++) {
        sprintf(encrypt_buf + i * 5, "0x%02X,", p_msg_body->content_start[i]);
    }
    encrypt_buf[data_len * 5] = 0;
    LOGD("encrypt data = %s", encrypt_buf);

    char clone_buffer[data_len];
    uint32_t decrypt_key = p_msg_body->decrypt_key;
    memcpy(clone_buffer, p_msg_body->content_start, data_len);
    uint16_t rc = ble_decrypt((uint8_t *) p_target_device_mac,
                              (uint16_t) strlen(p_target_device_mac),
                              clone_buffer,
                              data_len,
                              decrypt_key);

    if (rc == 0) {
        LOGE("解密失败");
    } else {
        LOGE("解密成功");
    }
#endif
    return msg;
}

//构建数据项
t_buffer build_content_v2(const uint8_t cmd, const uint8_t ack_type, const uint8_t *const p_data,
                          const uint32_t data_len) {
    t_buffer content;
    content.len = 0;
    content.p_buffer = NULL;

    // 如果有附加数据则分配内存
    if (p_data != NULL && data_len > 0) {
        content.len = sizeof(t_qd_access_content) + data_len;
    }
    else {
        content.len = sizeof(t_qd_access_content);
    }

    content.p_buffer = (uint8_t *) malloc(content.len);
    //内存分配检查

    if (content.p_buffer == NULL) {
        printf("%s:Memory malloc failed, content.len = %d!\n", __func__, content.len);
        content.len = 0;
        return content;
    }

    t_qd_access_content *p_content = (t_qd_access_content *) content.p_buffer;
    p_content->content_length = (uint16_t) content.len;
    p_content->command_type = cmd;
    p_content->ack_type = ack_type;
    if (p_data != NULL && data_len > 0) {
        memcpy(p_content->data_start, p_data, data_len);
    }

    return content;
}

//开门命令数据
t_buffer build_open_door_data_v2(uint8_t keep_time, uint32_t app_usr_id, uint8_t phone_type,const char *const agcy) {

    t_buffer data;
    data.len = 0;
    data.p_buffer = NULL;

    //是否有设备识别码
    bool need_agcy = agcy && strlen(agcy) >= sizeof(t_qd_agcy_data);
    data.len = sizeof(t_qd_opendoor_data_v2) + (need_agcy ? sizeof(t_qd_agcy_data) : 0);
    data.p_buffer = (uint8_t *) malloc(data.len);

    //内存分配检查
    if (data.p_buffer == NULL) {
        printf("%s in %s at %d:Memory malloc failed.\n", __FILE__, __func__, __LINE__);
        data.len = 0;
        return data;
    }

    t_qd_opendoor_data_v2 *p_tmp_data = (t_qd_opendoor_data_v2 *) data.p_buffer;
    memset(data.p_buffer, 0, data.len);

    p_tmp_data->create_time = (uint32_t) time(NULL);
    p_tmp_data->keep_time = keep_time;
    p_tmp_data->CA_flag = 0;
    p_tmp_data->app_user_id = app_usr_id;

    if (need_agcy) {
        t_qd_agcy_data *p_agcy_data = (t_qd_agcy_data *) (data.p_buffer +
                                                          sizeof(t_qd_opendoor_data_v2));
        memcpy(p_agcy_data->code, agcy, sizeof(p_agcy_data->code));
    }
    return data;
}

t_buffer build_open_door_content_v2(const uint8_t keep_time, const uint32_t app_user_id,
                                    const uint8_t phone_type, const char *const agcy) {
    t_buffer data = build_open_door_data_v2(keep_time, app_user_id, phone_type, agcy);
    if (data.p_buffer == NULL) {
        printf("%s:Build data failed!\n", __func__);
        data.len = 0;
        return data;
    }

    t_buffer content = build_content_v2(QD_CMD_OPEN_DOOR, QD_REQUEST, data.p_buffer, data.len);
    free(data.p_buffer); //释放data分配的内存
    if (content.p_buffer == NULL) {
        printf("%s:Build content failed!\n", __func__);
        content.len = 0;
        return content;
    }
    return content;
}


//开门命令数据
t_buffer build_open_door_data_v4(uint8_t keep_time, uint32_t app_usr_id, uint8_t phone_type,
                                 uint32_t pass_id, const char *const agcy) {

    t_buffer data;
    data.len = 0;
    data.p_buffer = NULL;
    //是否有设备识别码
    bool need_agcy = agcy && strlen(agcy) >= sizeof(t_qd_agcy_data);
    data.len = sizeof(t_qd_opendoor_data_v4) + (need_agcy ? sizeof(t_qd_agcy_data) : 0);
    data.p_buffer = (uint8_t *) malloc(data.len);

    //内存分配检查
    if (data.p_buffer == NULL) {
        printf("%s in %s at %d:Memory malloc failed.\n", __FILE__, __func__, __LINE__);
        data.len = 0;
        return data;
    }

    t_qd_opendoor_data_v4 *p_tmp_data = (t_qd_opendoor_data_v4 *) data.p_buffer;
    memset(data.p_buffer, 0, data.len);

    p_tmp_data->create_time = (uint32_t) time(NULL);
    p_tmp_data->keep_time = keep_time;
    p_tmp_data->CA_flag = 0;
    p_tmp_data->app_user_id = app_usr_id;
    p_tmp_data->pass_id = pass_id;

    if(need_agcy){
        t_qd_agcy_data *p_agcy_data = (t_qd_agcy_data *)(data.p_buffer + sizeof(t_qd_opendoor_data_v4));
        memcpy(p_agcy_data->code, agcy, sizeof(p_agcy_data->code));
    }

    return data;
}

t_buffer build_open_door_content_v4(const uint8_t keep_time, const uint32_t app_user_id,
                                    const uint8_t phone_type, const uint32_t pass_id,
                                    const char *const agcy) {
    t_buffer data = build_open_door_data_v4(keep_time, app_user_id, phone_type, pass_id, agcy);
    if (data.p_buffer == NULL) {
        printf("%s:Build data failed!\n", __func__);
        data.len = 0;
        return data;
    }
    t_buffer content = build_content_v2(QD_CMD_OPEN_DOOR, QD_REQUEST, data.p_buffer, data.len);

    free(data.p_buffer); //释放data分配的内存
    if (content.p_buffer == NULL) {
        printf("%s:Build content failed!\n", __func__);
        content.len = 0;
        return content;
    }
    return content;
}

t_buffer build_open_door_data_v5(uint8_t keep_time, uint64_t app_usr_id, uint8_t phone_type) {
    t_buffer data;
    data.len = 0;
    data.p_buffer = NULL;

    data.len = sizeof(t_qd_opendoor_data_v5);
    data.p_buffer = (uint8_t *) malloc(data.len);

    //内存分配检查
    if (data.p_buffer == NULL) {
        printf("%s in %s at %d:Memory malloc failed.\n", __FILE__, __func__, __LINE__);
        data.len = 0;
        return data;
    }

    t_qd_opendoor_data_v5 *p_tmp_data = (t_qd_opendoor_data_v5 *) data.p_buffer;
    memset(data.p_buffer, 0, data.len);

    p_tmp_data->create_time = (uint32_t) time(NULL);
    p_tmp_data->keep_time = keep_time;
    p_tmp_data->CA_flag = 0;
    p_tmp_data->app_user_id = app_usr_id;
    return data;
}

t_buffer build_open_door_content_v5(const uint8_t keep_time, const uint64_t app_user_id,
                                    const uint8_t phone_type) {
    t_buffer data = build_open_door_data_v5(keep_time, app_user_id, phone_type);
    if (data.p_buffer == NULL) {
        printf("%s:Build data failed!\n", __func__);
        data.len = 0;
        return data;
    }

    t_buffer content = build_content_v2(QD_CMD_OPEN_DOOR_V5, QD_REQUEST, data.p_buffer, data.len);
    free(data.p_buffer); //释放data分配的内存
    if (content.p_buffer == NULL) {
        printf("%s:Build content failed!\n", __func__);
        content.len = 0;
        return content;
    }
    return content;
}

//配置命令数据
t_buffer build_config_data_v2(const uint8_t open_time, const uint8_t *const p_related_unit_items,
                              const int sum_of_unit_items) {
    t_buffer data;
    data.len = 0;
    data.p_buffer = NULL;
    //参数检查
    if (p_related_unit_items == NULL || sum_of_unit_items == 0) {
        printf("%s in %s at %d:Paramater error.\n", __FILE__, __func__, __LINE__);
        return data;
    }
    data.len = sizeof(t_qd_config_device_data_v2) + sizeof(t_qd_unit_item) * sum_of_unit_items;
    data.p_buffer = (uint8_t *) malloc((size_t) data.len);

    //内存分配检查
    if (data.p_buffer == NULL) {
        printf("%s in %s at %d:Memory malloc failed.\n", __FILE__, __func__, __LINE__);
        data.len = 0;
        return data;
    }

    memset(data.p_buffer, 0, (size_t) data.len);
    t_qd_config_device_data_v2 *p_config_device_data = (t_qd_config_device_data_v2 *) data.p_buffer;
    p_config_device_data->keep_time = open_time;
    p_config_device_data->setting_time = (uint32_t) time(NULL);
    memcpy(p_config_device_data->related_unit_items, p_related_unit_items, sizeof(t_qd_unit_item) * sum_of_unit_items);

    return data;
}

t_buffer build_config_data_v4(const char *const agcy){
    t_buffer data;
    uint32_t len = sizeof(t_qd_agcy_data);
    data.len = len;
    data.p_buffer = (uint8_t *)malloc(len);
    if(data.p_buffer == NULL){
        LOGI("%s:Memory malloc failed", __func__);
        data.len = 0;
        return data;
    }
    memset(data.p_buffer, 0, len);
    //设置code值
    t_qd_agcy_data* p_agcy_data = (t_qd_agcy_data*) data.p_buffer;
    int code_length = sizeof(p_agcy_data->code);
    if(agcy && strlen(agcy) >= code_length){
        memcpy(p_agcy_data->code, agcy, code_length);
    }
    return data;
}

//设备升级命令
t_buffer build_update_device_data_v2(uint8_t update_type, const char *const p_download_addr) {
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
    memcpy(p_tmp_data->encrypted_download_addr_start, p_holder, enc_content_len);
#endif
    //为解决早期版本设备读取下载路径没添加字符串结束符bug,多增加一个字节'\0'
    data.len = sizeof(t_qd_update_device_data_v2) + strlen(p_download_addr) + 1;
    data.p_buffer = (uint8_t *) malloc((size_t) data.len);

    //内存分配检查
    if (data.p_buffer == NULL) {
        printf("%s in %s at %d:Memory malloc failed.\n", __FILE__, __func__, __LINE__);
        data.len = 0;
        return data;
    }

    memset(data.p_buffer, 0, (size_t) data.len);
    t_qd_update_device_data_v2 *p_tmp_data = (t_qd_update_device_data_v2 *) data.p_buffer;
    p_tmp_data->update_type = update_type;
    memcpy(p_tmp_data->encrypted_download_addr_start, p_download_addr, strlen(p_download_addr));
    return data;
}

t_buffer build_get_device_version_ack_data_v2(const char *const p_software_version,
                                              const char *const p_hardware_version) {
    t_buffer data;
    data.len = 0;
    data.p_buffer = NULL;
    //参数检查
    if (p_software_version == NULL && p_hardware_version == NULL) {
        printf("%s in %s at %d:Paramater error.\n", __FILE__, __func__, __LINE__);
        return data;
    }

    data.len = sizeof(t_qd_version_ack_data_v2);
    data.p_buffer = (uint8_t *) malloc((size_t) data.len);

    //内存分配检查
    if (data.p_buffer == NULL) {
        printf("%s in %s at %d:Memory malloc failed.\n", __FILE__, __func__, __LINE__);
        data.len = 0;
        return data;
    }

    memset(data.p_buffer, 0, (size_t) data.len);
    t_qd_version_ack_data_v2 *p_tmp_data = (t_qd_version_ack_data_v2 *) data.p_buffer;

    //参数有问题，直接释放内存，报错
    if (strlen(p_software_version) > (sizeof(p_tmp_data->software_version) - 1)) {
        printf("%s:Parameter error, the software_version is over than %d byte string\n", __func__,
               sizeof(p_tmp_data->software_version));
        free(data.p_buffer);
        data.p_buffer = NULL;
        data.len = 0;
        return data;
    }

    //参数有问题，直接释放内存，报错
    if (strlen(p_hardware_version) > (sizeof(p_tmp_data->hardware_version) - 1)) {
        printf("%s:Parameter error, the hardware_version string is over than %d byte\n", __func__,
               sizeof(p_tmp_data->hardware_version));
        free(data.p_buffer);
        data.p_buffer = NULL;
        data.len = 0;
        return data;
    }

    memcpy(p_tmp_data->software_version, p_software_version, strlen(p_software_version));
    memcpy(p_tmp_data->hardware_version, p_hardware_version, strlen(p_hardware_version));
    return data;
}

t_buffer build_get_device_config_ack_data_v2(const uint8_t open_keep_time,
                                             const char *const p_software_version,
                                             const char *const p_hardware_version,
                                             const uint8_t *const p_related_units,
                                             const int sum_of_units) {

    t_buffer data;
    data.len = 0;
    data.p_buffer = NULL;
    //参数检查
    if (sum_of_units < 0) {
        printf("%s in %s at %d:Paramater error.\n", __FILE__, __func__, __LINE__);
        return data;
    }

    if (sum_of_units > 0) {
        data.len = sizeof(t_qd_config_ack_data_v2) + sizeof(t_qd_unit_item) * sum_of_units;
    }
    else {
        data.len = sizeof(t_qd_config_ack_data_v2);
    }
    data.p_buffer = (uint8_t *) malloc((size_t) data.len);

    //内存分配检查
    if (data.p_buffer == NULL) {
        printf("%s in %s at %d:Memory malloc failed.\n", __FILE__, __func__, __LINE__);
        data.len = 0;
        return data;
    }
    memset(data.p_buffer, 0, (size_t) data.len);
    t_qd_config_ack_data_v2 *p_tmp_data = (t_qd_config_ack_data_v2 *) data.p_buffer;

    //参数有问题，直接释放内存，报错
    if (strlen(p_software_version) > (sizeof(p_tmp_data->software_version) - 1)) {
        printf("%s:Parameter error, the software_version is over than %d byte string\n", __func__,
               sizeof(p_tmp_data->software_version));
        free(data.p_buffer);
        data.p_buffer = NULL;
        data.len = 0;
        return data;
    }
    //参数有问题，直接释放内存，报错
    if (strlen(p_hardware_version) > (sizeof(p_tmp_data->hardware_version) - 1)) {
        printf("%s:Parameter error, the hardware_version string is over than %d byte\n", __func__,
               sizeof(p_tmp_data->hardware_version));
        free(data.p_buffer);
        data.p_buffer = NULL;
        data.len = 0;
        return data;
    }

    p_tmp_data->open_keep_time = open_keep_time;
    p_tmp_data->device_time = (uint32_t) time(NULL);
    memcpy(p_tmp_data->software_version, p_software_version, strlen(p_software_version));
    memcpy(p_tmp_data->hardware_version, p_hardware_version, strlen(p_hardware_version));
    if (p_related_units != NULL && sum_of_units > 0) {
        memcpy(p_tmp_data->related_doors_start, p_related_units, sizeof(t_qd_unit_item) * sum_of_units);
    }
    return data;
}


t_buffer build_distribute_cards_data_v2(int wait_time_pre_card, const char *const p_cards_info) {

    t_buffer data = {0};
    if (p_cards_info == NULL) {
        printf("%s: p_write_data is NULL",__func__);
        return data;
    }

    char tmp_str[strlen(p_cards_info) + 1];
    memset(tmp_str,0, sizeof(tmp_str));
    memcpy(tmp_str,p_cards_info,strlen(p_cards_info)+1);
    int split_char_counter = 0;
    char *p_split_char = "@";

    for(int i=0; i <= strlen(tmp_str); i++) {
        if (tmp_str[i] == *p_split_char) {
            ++split_char_counter;
        }
    }

    if ((split_char_counter%2) != 0) {
        printf("%s: card_uid & data not pair",__func__);
        return data;
    }

    int card_counter = split_char_counter / 2;

    if (card_counter > 10) {
        printf("cards is more than 10\r\n");
        return data;
    }
    char card_uid_array[10][20] = {0};
    char card_data_array[10][100] = {0};

    printf("%s: card_counter = %d\r\n",__func__,card_counter);

    strcpy(card_uid_array[0],strtok(tmp_str,p_split_char));
    strcpy(card_data_array[0],strtok(NULL,p_split_char));

    for (int i=1; i < card_counter; ++i) {
        strcpy(card_uid_array[i],strtok(NULL,p_split_char));
        strcpy(card_data_array[i],strtok(NULL,p_split_char));
    }

#if 0

    for (int i=0; i < card_counter; ++i) {
        printf("%s\r\n",card_uid_array[i]);
        printf("%s\r\n",card_data_array[i]);
    }

#endif

    data.len = sizeof(t_qd_write_card_data_v2)  + sizeof(t_qd_write_card_item_v2) * card_counter;
    data.p_buffer = (uint8_t*)malloc(data.len);


    t_qd_write_card_data_v2 *p_card_data = (t_qd_write_card_data_v2 *)data.p_buffer ;
    p_card_data->wait_mseconds_pre_card = wait_time_pre_card;


    unsigned long long tmp_card_uid;

    unsigned long tmp_num;
    char *p_end;
    char sub_str[9]= {0};


    t_qd_write_card_item_v2 *p_card_item = NULL;
    for (int i=0; i < card_counter; i++) {
        p_card_item = (t_qd_write_card_item_v2 *)(p_card_data->card_item + (i * sizeof(t_qd_write_card_item_v2)));
        for (int k =0 ; k < strlen(card_uid_array[i]); k +=2) {
            char card_uid_str[3] = {0};
            memcpy(card_uid_str, card_uid_array[i] +k, 2);
            sscanf(card_uid_str,"%02X",&(p_card_item->target_card_uid[k/2]));
//            printf("card_uid_str= %s, Hex= %02X\r\n",card_uid_str, p_card_item->target_card_uid[k/2]);
        }

//        printf("card_data_str=%s, len = %d\r\n", card_data_array[i],strlen(card_data_array[i]));
        for (int j=0; j < strlen(card_data_array[i]); j = j+8) {

            if ((j+8) < strlen(card_data_array[i])) {
                memcpy(sub_str,card_data_array[i]+j,8);
            } else {
                memcpy(sub_str,card_data_array[i]+j,strlen(card_data_array[i])-j);
            }

            tmp_num = strtoul(sub_str,&p_end,16);

            //前四个字节为密钥,hash计数，版本号，需特殊处理下
            if(j == 0) {
                // 密钥
                memset(sub_str,0,9);
                memcpy(sub_str,card_data_array[i],4);
                tmp_num = strtoul(sub_str,&p_end,16);
                memcpy(p_card_item->write_data,&tmp_num,2);

                // hash counter
                memset(sub_str,0,9);
                memcpy(sub_str,card_data_array[i]+4,2);
                tmp_num = strtoul(sub_str,&p_end,16);
                memcpy(p_card_item->write_data+2,&tmp_num,1);

                //版本
                memset(sub_str,0,9);
                memcpy(sub_str,card_data_array[i]+6,2);
                tmp_num = strtoul(sub_str,&p_end,16);
                memcpy(p_card_item->write_data+3,&tmp_num,1);

            } else {
                memcpy(p_card_item->write_data+(j/2),&tmp_num,4);
            }

//            printf("sub_str=%s, tmp_num=%08X\r\n",sub_str,tmp_num);
        }
        printf("\r\n");
    }
    return data;

}

t_buffer build_update_transport_fragment_data_v2(int seek_index, const uint8_t *const p_data, int data_len) {
    t_buffer data = { 0 };

    if (p_data == NULL || data_len == 0) {
        printf("%s: Parameter error, the p_data is NULL or the date_len is 0\r\n", __FUNCTION__);
        return data;
    }

    data.len = sizeof(t_qd_update_file_transport_data_v2) + data_len;
    data.p_buffer = (uint8_t *)malloc(data.len);
    t_qd_update_file_transport_data_v2 *p_fragment = (t_qd_update_file_transport_data_v2*)data.p_buffer;

    p_fragment->file_pos_index = seek_index;
    p_fragment->crc32_check = crc32(0,p_data,data_len);
    memcpy(p_fragment->file_data,p_data,data_len);
    return data;
}
