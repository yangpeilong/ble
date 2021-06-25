//
// Created by 胡涛 on 2017/5/11.
//

#ifndef QRCODE_CMSGPROCESSER_H
#define QRCODE_CMSGPROCESSER_H


#include <stdint.h>
#include <string>
#include "qd_access_protocol_v2_define.h"
#include "qd_access_protocol_define.h"
#include "qd_access_protocol_v2_standalone_card.h"
#include "qd_common_utils.h"
#include "qd_ble_hook_msg_define.h"

#define QD_MAX_BUFFER_SIZE          1024 * 64

class CJavaMethodManager;

class CMsgProcessor {

public:
    CMsgProcessor(JNIEnv *env, jobject obj);

    virtual ~CMsgProcessor();

    void update_jni_env(JNIEnv *env);

    int handle_msg(void *pBuf, int len, std::string targe_device_mac);

    //广播开门数据处理
    void process_broadcast_open_door_v2(void *p_data, int data_len, std::string targe_device_mac);

private:
    void assemble_data(void *const p_data, int data_len);

    void process_msg(void *p_data, int data_len);

    void process_msg_v1(t_qd_gatectrl_msg *, int data_len, bool checkOpenDoorAck);

    void process_msg_content_v2(void *const p_data);

    //一代协议处理
    void process_open_door_ack_content_v1(t_qd_gatectrl_contentbody *p_data, int data_len);

    void process_config_device_ack_content_v1(t_qd_gatectrl_contentbody *p_data, int content_data_len);

    void process_update_device_ack_content_v1(t_qd_gatectrl_contentbody *p_data, int content_data_len);

    void process_get_device_version_ack_content_v1(t_qd_gatectrl_contentbody *p_data, int content_data_len);

    void process_get_device_config_ack_content_v1(t_qd_gatectrl_contentbody *p_data, int content_data_len);

    //二代协议处理
    void process_open_door_ack_content_v2(t_qd_access_content *p_content);

    void process_update_state_report_ack_v2(t_qd_access_content *p_content);

    void process_config_device_ack_content_v2(t_qd_access_content *p_content);

    void process_update_device_ack_content_v2(t_qd_access_content *p_content);

    void process_get_device_version_ack_content_v2(t_qd_access_content *p_content);

    void process_get_device_config_ack_content_v2(t_qd_access_content *p_content);

    void process_set_relay_state_content_v2(t_qd_access_content *p_content);

    void process_set_buzzer_mode_content_v2(t_qd_access_content *p_content);

    void process_write_card_content_ack_v2(t_qd_access_content *p_content);

    void process_read_card_content_ack_v2(t_qd_access_content *p_content);

    void process_standalone_card_read_ack_v2(t_qd_access_content *p_content);

    void process_standalone_card_add_ack_v2(t_qd_access_content *p_content);

    void process_standalone_card_del_ack_v2(t_qd_access_content *p_content);

    void process_enable_433_test_ack_v2(t_qd_access_content *p_content);

    void process_get_hal_state_ack_v2(t_qd_access_content *p_content);

    //四代协议处理
    void process_config_device_ack_content_v4(t_qd_access_content *p_content);

    void process_test_get_device_info_v4(t_qd_access_content *p_content);

    void process_test_notify_event_v4(t_qd_access_content *p_content);

    //五代协议处理
    void process_get_gate_magnetism_state_ack_v5(t_qd_access_content *p_content);

    void process_open_door_ack_content_v5(t_qd_access_content *p_content);
private:
    CJavaMethodManager *p_javaMethodManger;
    t_payload_buf _assemble_buf;
    bool _is_received_frame_head = false;
    long _pre_received_time = 0;
    int _total_msg_len;
    std::string _target_dev_mac;
};


#endif //QRCODE_CMSGPROCESSER_H
