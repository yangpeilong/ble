//
// Created by 胡涛 on 2017/5/11.
//

#include <jni.h>

#include <list>

#import "qd_access_protocol_v2_define.h"
#include "qd_access_protocol_define.h"
#include "meshsub.h"

#include "debugLog.h"
#include "CMsgProcessor.h"
#include "CJavaMethodManager.h"
#include "QDPassRecordEntity.h"

CMsgProcessor::CMsgProcessor(JNIEnv *env, jobject obj) {
    p_javaMethodManger = new CJavaMethodManager(env, obj);
}

CMsgProcessor::~CMsgProcessor(){
    delete p_javaMethodManger;
}

void CMsgProcessor::update_jni_env(JNIEnv *env) {
    if (p_javaMethodManger) {
        p_javaMethodManger->update_jni_env(env);
    }
}

int CMsgProcessor::handle_msg(void *pBuf, int len, std::string target_device_mac) {
    LOGD("=============== handle_msg start ===========");
    if (_target_dev_mac == "") {
        _target_dev_mac = target_device_mac;
    }
    // 如果数据包发生变更异常处理
    if (_target_dev_mac != target_device_mac) {
        _is_received_frame_head = 0;
        _assemble_buf.len = 0;
        _target_dev_mac = target_device_mac;
    }

    if (pBuf == NULL || len <= 0) {
        return -1;
    }

    assemble_data(pBuf, len);
    LOGD("=============== handle_msg end ===========");
    return 0;
}

void CMsgProcessor::assemble_data(void *const p_data, int data_len) {
    //打印数据
    char test_buf[data_len * 3 + 1];
    for (int i = 0; i < data_len; i++) {
        sprintf(test_buf + i * 3, "%02X ", ((uint8_t *) p_data)[i]);
    }
    test_buf[data_len * 3] = 0;
    LOGD("==== data = %s", test_buf);

    //计算接收二帧数据间隔时间
    struct timeval current_time_val;
    gettimeofday(&current_time_val, NULL);
    long current_time = current_time_val.tv_sec * 1000 + current_time_val.tv_usec / 1000;
    long diff = current_time - _pre_received_time;
    LOGD("diff ==  %ld", diff);
    if (diff > 1000)
    {
        _is_received_frame_head = false;
    }
    _pre_received_time = current_time;
    LOGD("_is_received_frame_head %d ", _is_received_frame_head);

    //检测数据长度最大值
    if (data_len > QD_MAX_BUFFER_SIZE)
    {
        LOGE("单次输入数据超过64K，无法处理");
        return;
    }

    //已经收到帧头，就直接接收数据
    if (_is_received_frame_head)
    {
        memcpy(&_assemble_buf.buf[_assemble_buf.len], p_data, (size_t) data_len);
        _assemble_buf.len += data_len;
        LOGD("_total_msg_len = %d, _assemble_buf.len = %d\n", _total_msg_len, _assemble_buf.len);
        if (_assemble_buf.len >= _total_msg_len) {
            process_msg(_assemble_buf.buf, _total_msg_len);
            int unproc_len = _assemble_buf.len - _total_msg_len;
            int processed_len = _total_msg_len;
            _is_received_frame_head = false;
            _total_msg_len = 0;
            if (unproc_len > 0) {
                uint8_t tmp_buf[unproc_len];
                memcpy(tmp_buf, &_assemble_buf.buf[processed_len], (size_t) unproc_len);
                assemble_data(tmp_buf, unproc_len);
            }
        }
        return;
    }
    //检测是否为帧头
    uint8_t * p_head = (uint8_t *)p_data;
    LOGD("p_msg->frame_head = %02x", p_head[0]);
    if (p_head[0] == QD_MSG_FRAMEHEAD)
    {
        // Step_0 拷贝数据
        LOGD("Receive frame_head");
        _is_received_frame_head = true;
        _assemble_buf.len = 0;
        memcpy(&_assemble_buf.buf, p_data, (size_t) data_len);
        _assemble_buf.len = data_len;

        // Step_1 取出总长度
        if (data_len >= (int) sizeof(t_qd_access_msg))
        {
            t_qd_access_msg *p_msg = (t_qd_access_msg *) p_data;
            //二代四代协议处理
            if (p_msg->protocol_version == QD_BLE_GATE_CTRL_VERSION_V2
                || p_msg->protocol_version == QD_BLE_GATE_CTRL_VERSION_V4)
            {
                LOGD("QD_BLE_GATE_CTRL_VERSION_V2");
                _total_msg_len = p_msg->total_length;
            }
            //一代协议处理
            else if (p_msg->protocol_version == QD_BLE_GATE_CTRL_VERSION)
            {
                LOGD("QD_BLE_GATE_CTRL_VERSION");
                t_qd_gatectrl_msg *p_qd_gatectrl_msg = (t_qd_gatectrl_msg *) p_data;
                process_msg_v1(p_qd_gatectrl_msg, data_len, true);
                _total_msg_len = p_qd_gatectrl_msg->total_length;
            }
            else
            {
                LOGE("unknown version ！！");
                return;
            }
            LOGD("_total_msg_len = %d, _assemble_buf.len = %d\n", _total_msg_len, _assemble_buf.len);
            if (_total_msg_len == 0) {
                LOGE("_total_msg_len == 0");
                return;
            }
            if (_assemble_buf.len >= _total_msg_len)
            {
                process_msg(_assemble_buf.buf, _total_msg_len);
                int unproc_len = _assemble_buf.len - _total_msg_len;
                int processed_len = _total_msg_len;
                _is_received_frame_head = false;
                _total_msg_len = 0;
                if (unproc_len > 0) {
                    uint8_t tmp_buf[unproc_len];
                    memcpy(tmp_buf, &_assemble_buf.buf[processed_len], (size_t) unproc_len);
                    assemble_data(tmp_buf, unproc_len);
                }
            }
        }
        return;
    }
    LOGE("非法数据，忽略");
    return;
}


void CMsgProcessor::process_msg(void *p_data, int data_len) {
    if ((uint32_t) data_len < (sizeof(t_qd_access_msg) + sizeof(t_qd_access_content))) {
        LOGE(" data len = %d, is too small\n", data_len);
        return;
    }
    LOGD("process_msg data_len = %d", data_len);
    t_qd_access_msg *p_msg = (t_qd_access_msg *) p_data;
    // 第二版协议，因为大部分是这种所以放在前面判断
    if (p_msg->protocol_version == QD_BLE_GATE_CTRL_VERSION_V2
        || p_msg->protocol_version == QD_BLE_GATE_CTRL_VERSION_V4) {
        if (p_msg->decrypt_key != 0) {
            LOGD("密匙不为0, 需要解密 p_msg->decrypt_key == %u ",p_msg->decrypt_key);
            uint16_t rc = ble_decrypt((uint8_t *) _target_dev_mac.c_str(),
                                      (uint16_t) _target_dev_mac.length(),
                                      p_msg->content_start,
                                      data_len - sizeof(t_qd_access_msg),
                                      p_msg->decrypt_key);

            if (rc == 0) {
                LOGE("解密失败");
                return;
            }
        }

        uint16_t processed_buf_index = sizeof(t_qd_access_msg);
        for (; processed_buf_index < (uint16_t) data_len;) {
            t_qd_access_content *p_qd_content = (t_qd_access_content *) ((char *) p_data +
                                                                         processed_buf_index);
            LOGD("processed_buf_index =%u ,p_qd_content->content_length = %u , data_len =%u\n",
                 processed_buf_index, p_qd_content->content_length, data_len);

            if (p_qd_content->content_length == 0) {
                break;
            }
            //保证content的数据完整性
            if (processed_buf_index + p_qd_content->content_length <= (uint16_t) data_len) {

                char test_buf[data_len * 3 + 1];
                for (int i = 0; i < data_len; i++) {
                    sprintf(test_buf + i * 3, "%02X ", ((uint8_t *) p_data)[i]);
                }
                test_buf[data_len * 3] = 0;
                LOGD("==== data_kb_test= %s", test_buf);

                process_msg_content_v2((void *) p_qd_content);
                processed_buf_index += p_qd_content->content_length;
            }
            else {
                break;
            }
        }

        return;
    }

    // 第一版协议解析
    if (p_msg->protocol_version == QD_BLE_GATE_CTRL_VERSION) {
        t_qd_gatectrl_msg *p_qd_gatectrl_msg = (t_qd_gatectrl_msg *) p_data;
        process_msg_v1(p_qd_gatectrl_msg, data_len, false);
        return;
    };

    return;
}

/****************************************一代协议处理****************************************************/
void CMsgProcessor::process_msg_v1(t_qd_gatectrl_msg *p_qd_gatectrl_msg, int data_len, bool checkOpenDoorAck) {
    LOGD("%s data_len %d", __FUNCTION__, data_len);
    //获取数据内容实际长度
    int size_qd_gatectrl_msg = sizeof(t_qd_gatectrl_msg);
    int size_qd_gatectrl_contentbody = sizeof(t_qd_gatectrl_contentbody);
    int content_data_len = data_len - (size_qd_gatectrl_msg - 1);
    if(content_data_len < (size_qd_gatectrl_contentbody-1))
    {
        LOGD("%s...content_data_len < size_qd_gatectrl_contentbody-1", __FUNCTION__);
        return;
    }
    t_qd_gatectrl_contentbody *p_qd_opendoor_ack_content = (t_qd_gatectrl_contentbody *) &p_qd_gatectrl_msg->content_start;
    //获取命令类型
    uint8_t cmd = p_qd_opendoor_ack_content->command_type;
    //只处理开门结果操作
    if(checkOpenDoorAck)
    {
        if(cmd == QD_CMD_OPEN_DOOR)
        {
            int size = (size_qd_gatectrl_contentbody-1) + sizeof(t_qd_opendoor_ack_data);
            size = content_data_len > size ? size : content_data_len;
            process_open_door_ack_content_v1(p_qd_opendoor_ack_content, size);
        }
    }
    //其他设备操作
    else
    {
        switch (cmd)
        {
            case QD_CMD_OPEN_DOOR: {
                process_open_door_ack_content_v1(p_qd_opendoor_ack_content, content_data_len);
                break;
            }
            case QD_CMD_CONFIG_DEVICE: {
                process_config_device_ack_content_v1(p_qd_opendoor_ack_content, content_data_len);
                break;
            }
            case QD_CMD_UPDATE_SYSTEM: {
                process_update_device_ack_content_v1(p_qd_opendoor_ack_content, content_data_len);
                break;
            }
            case QD_CMD_GET_VERSION: {
                process_get_device_version_ack_content_v1(p_qd_opendoor_ack_content, content_data_len);
                break;
            }
            case QD_CMD_GET_CONFIG: {
                process_get_device_config_ack_content_v1(p_qd_opendoor_ack_content, content_data_len);
                break;
            }
            default:break;
        }
    }
    LOGD("%s checkOpenDoorAck:%d finish", __FUNCTION__, checkOpenDoorAck);
}

void CMsgProcessor::process_open_door_ack_content_v1(t_qd_gatectrl_contentbody *p_data, int content_data_len){
    LOGD("%s", __FUNCTION__);
    //验证开门结果数据
    int data_len = content_data_len - (sizeof(t_qd_gatectrl_contentbody) - 1);
    if(data_len < (int) (sizeof(t_qd_opendoor_ack_data)-1)){
        LOGD("%s...data_len < 0", __FUNCTION__);
        return;
    }
    t_qd_opendoor_ack_data *p_qd_opendoor_ack_data = (t_qd_opendoor_ack_data *) &(p_data->data_start);

    //计算通行记录数据
    int record_item_len = data_len - (sizeof(t_qd_opendoor_ack_data) - 1);
    if (record_item_len >= (int) sizeof(t_qd_passby_record_item)) {

        std::list<QDPassRecordEntity> record_list;
        char tmp_string[64];
        QDPassRecordEntity record_item;
        //预加上一个item的长度，防止数据不够的情况
        for (int i = 0; (i + sizeof(t_qd_passby_record_item)) <= record_item_len; i += sizeof(t_qd_passby_record_item)) {
            t_qd_passby_record_item *p_record_item = (t_qd_passby_record_item *) (&(p_qd_opendoor_ack_data->record_item_start) + i);

            sprintf(tmp_string, "%u", p_record_item->record_index);
            record_item.record_index = std::string(tmp_string);

            sprintf(tmp_string, "%u", p_record_item->app_user_id);
            record_item.app_user_id = std::string(tmp_string);

            sprintf(tmp_string, "%u", p_record_item->room_id);
            record_item.room_id = std::string(tmp_string);

            sprintf(tmp_string, "%u", p_record_item->server_id);
            record_item.server_id = std::string(tmp_string);

            sprintf(tmp_string, "%u", p_record_item->mac_addr);
            record_item.dev_mac = std::string(tmp_string);

            sprintf(tmp_string, "%u", p_record_item->passby_method);
            record_item.pass_type = std::string(tmp_string);

            sprintf(tmp_string, "%u", p_record_item->create_time);
            record_item.create_time = std::string(tmp_string);
            record_list.push_back(record_item);
        }
        LOGD("process_msg_v1 record_list =  %d  ", record_list.size());
        if (p_javaMethodManger) {
            p_javaMethodManger->pass_record_ack(record_list);
        }
    } else {
        if (p_javaMethodManger) {
            p_javaMethodManger->open_door_ack(p_qd_opendoor_ack_data->open_result);
        }
    }
    LOGD("%s finish", __FUNCTION__);
}

void CMsgProcessor::process_config_device_ack_content_v1(t_qd_gatectrl_contentbody *p_data, int content_data_len){
    int ack_type = p_data->ack_type;
    LOGD("%s...ack_type:%02X", __FUNCTION__, ack_type);
    //验证配置结果数据
    int data_len = content_data_len - (sizeof(t_qd_gatectrl_contentbody) - 1);
    if(data_len >= (int) sizeof(t_qd_config_device_ack_data)){
        t_qd_config_device_ack_data *p_qd_config_ack_data = (t_qd_config_device_ack_data *) &(p_data->data_start);
        if (p_javaMethodManger) {
            p_javaMethodManger->config_device_ack(p_qd_config_ack_data->config_result);
        }
    } else {
        if (p_javaMethodManger) {
            p_javaMethodManger->receive_ack(ack_type, p_data->command_type);
        }
    }
}

void CMsgProcessor::process_update_device_ack_content_v1(t_qd_gatectrl_contentbody *p_data, int content_data_len){
    int ack_type = p_data->ack_type;
    LOGD("%s...ack_type:%02X", __FUNCTION__, ack_type);
    //验证配置结果数据
    int data_len = content_data_len - (sizeof(t_qd_gatectrl_contentbody) - 1);
    if(data_len >= (int) sizeof(t_qd_update_device_ack_data)){
        t_qd_update_device_ack_data *p_update_device_ack_data = (t_qd_update_device_ack_data *) &(p_data->data_start);
        if (p_javaMethodManger) {
            p_javaMethodManger->update_device_ack(p_update_device_ack_data->update_result);
        }
    } else{
        if (p_javaMethodManger) {
            p_javaMethodManger->receive_ack(ack_type, p_data->command_type);
        }
    }
}

void CMsgProcessor::process_get_device_version_ack_content_v1(t_qd_gatectrl_contentbody *p_data, int content_data_len){
    int ack_type = p_data->ack_type;
    LOGD("%s...ack_type:%02X", __FUNCTION__, ack_type);
    //验证配置结果数据
    int data_len = content_data_len - (sizeof(t_qd_gatectrl_contentbody) - 1);
    if(data_len >= (int) sizeof(t_qd_get_device_version_ack_data)){
        t_qd_get_device_version_ack_data *p_update_device_ack_data = (t_qd_get_device_version_ack_data *) &(p_data->data_start);
        if (p_javaMethodManger) {
            p_javaMethodManger->get_version_ack(std::string((char*)p_update_device_ack_data->software_version), std::string((char*)p_update_device_ack_data->hardware_version));
        }
    } else {
        if (p_javaMethodManger) {
            p_javaMethodManger->receive_ack(ack_type, p_data->command_type);
        }
    }
}

void CMsgProcessor::process_get_device_config_ack_content_v1(t_qd_gatectrl_contentbody *p_data, int content_data_len){
    int ack_type = p_data->ack_type;
    LOGD("%s...ack_type:%02X", __FUNCTION__, ack_type);
    //验证配置结果数据
    int data_len = content_data_len - (sizeof(t_qd_gatectrl_contentbody) - 1);
    if(data_len >= (int) sizeof(t_qd_get_device_config_ack_data)){
        t_qd_get_device_config_ack_data *p_update_device_ack_data = (t_qd_get_device_config_ack_data *) &(p_data->data_start);
        if (p_javaMethodManger) {
            p_javaMethodManger->get_config_ack_v1(p_update_device_ack_data, data_len);
        }
    } else {
        if (p_javaMethodManger) {
            p_javaMethodManger->receive_ack(ack_type, p_data->command_type);
        }
    }
}

/****************************************二代协议处理****************************************************/

void CMsgProcessor::process_msg_content_v2(void *const p_data) {
    t_qd_access_content *p_qd_content = (t_qd_access_content *) p_data;

    switch (p_qd_content->command_type) {
        case QD_CMD_OPEN_DOOR: {
            LOGD("Get QD_CMD_OPEN_DOOR ACK\r\n");
            process_open_door_ack_content_v2(p_qd_content);
            break;
        }
        case QD_CMD_OPEN_DOOR_V5:
        case QD_CMD_OPEN_DOOR_AND_RECORDS_V5: {
            LOGD("Get QD_CMD_OPEN_DOOR_V5 ACK\r\n");
            process_open_door_ack_content_v5(p_qd_content);
            break;
        }
        case QD_CMD_DISTRIBUTE_CARD: {
            LOGD("Get QD_CMD_DISTRIBUTE_CARD ACK\r\n");
            process_write_card_content_ack_v2(p_qd_content);
            break;
        }
        case QD_CMD_READ_CARD_UID: {
            LOGD("Get QD_CMD_READ_CARD_UID ACK\r\n");
            process_read_card_content_ack_v2(p_qd_content);
            break;
        }
        case QD_CMD_UPDATE_STATE_REPORT:
            LOGD("Get QD_CMD_UPDATE_STATE_REPORT ACK\r\n");
            process_update_state_report_ack_v2(p_qd_content);
            break;
        case QD_CMD_CONFIG_DEVICE: {
            LOGD("Get QD_CMD_CONFIG_DEVICE ACK\r\n");
            process_config_device_ack_content_v2(p_qd_content);
            break;
        }
        case QD_CMD_UPDATE_SYSTEM: {
            LOGD("get QD_CMD_UPDATE_SYSTEM ACK\r\n");
            process_update_device_ack_content_v2(p_qd_content);
            break;
        }
        case QD_CMD_GET_VERSION: {
            LOGD("Get QD_CMD_GET_VERSION ACK \r\n");
            process_get_device_version_ack_content_v2(p_qd_content);
            break;
        }
        case QD_CMD_GET_CONFIG: {
            LOGD("Get QD_CMD_GET_CONFIG ACK\r\n");
            process_get_device_config_ack_content_v2(p_qd_content);
            break;
        }
        //针对四代设备的配置功能
        case QD_CMD_CONFIG_DEVICE_AGCY: {
            LOGD("Get QD_CMD_CONFIG_DEVICE_AGCY ACK\r\n");
            process_config_device_ack_content_v4(p_qd_content);
            break;
        }
        case QD_CMD_SET_RELAY_STATE: {
            LOGD("Get QD_CMD_SET_RELAY_STATE ACK\r\n");
            process_set_relay_state_content_v2(p_qd_content);
            break;
        }
        case QD_CMD_SET_BUZZER_MODE: {
            LOGD("Get QD_CMD_SET_BUZZER_MODE ACK\r\n");
            process_set_buzzer_mode_content_v2(p_qd_content);
            break;
        }
        case QD_CMD_STANDALONE_CARDNO_READ: {
            LOGD("Get QD_CMD_STANDALONE_CARDNO_READ ACK\r\n");
            process_standalone_card_read_ack_v2(p_qd_content);
            break;
        }
        case QD_CMD_STANDALONE_CARDNO_ADD: {
            LOGD("Get QD_CMD_STANDALONE_CARDNO_ADD ACK\r\n");
            process_standalone_card_add_ack_v2(p_qd_content);
            break;
        }
        case QD_CMD_STANDALONE_CARDNO_DEL: {
            LOGD("Get QD_CMD_STANDALONE_CARDNO_DEL ACK\r\n");
            process_standalone_card_del_ack_v2(p_qd_content);
            break;
        }
        /***********二代设备工厂测试***************/
        case QD_CMD_ENABLE_433_TEST: {
            LOGD("Get QD_CMD_ENABLE_433_TEST ACK\r\n");
            process_enable_433_test_ack_v2(p_qd_content);
            break;
        }
        case QD_CMD_GET_HAL_STATE: {
            LOGD("Get QD_CMD_GET_HAL_STATE ACK\r\n");
            process_get_hal_state_ack_v2(p_qd_content);
            break;
        }
        /***********四代设备工厂测试***************/
        case QD_CMD_GET_DEVINFO: {
            LOGD("Get QD_CMD_GET_DEVINFO ACK\r\n");
            process_test_get_device_info_v4(p_qd_content);
            break;
        }
        case QD_CMD_NOTIFY_EVENT: {
            LOGD("Get QD_CMD_NOTIFY_EVENT ACK\r\n");
            process_test_notify_event_v4(p_qd_content);
            break;
        }
        /***********五代设备工厂测试***************/
        case QD_CMD_GET_DOORSENSOR_STATE: {
            LOGD("Get QD_CMD_GET_DOORSENSOR_STATE ACK\r\n");
            process_get_gate_magnetism_state_ack_v5(p_qd_content);
            break;
        }
        default:
            break;
    }

    LOGD("process_msg_content_v2 finished \n");
}

//添加实现函数
void CMsgProcessor::process_open_door_ack_content_v2(t_qd_access_content *p_content)
{
    //开门请求：由千丁门禁sdk发出, 千丁门禁设备处理
    if(p_content->ack_type == QD_REQUEST)
    {
        LOGD("%s request", __FUNCTION__);
        int open_door_req_len = p_content->content_length - sizeof(t_qd_access_content);
        uint32_t keep_time = 0;
        uint32_t user_id   = 0;
        bool result = false;
        t_buffer msg_record = {.p_buffer=NULL, .len=0};
        if(open_door_req_len >= sizeof(t_qd_opendoor_data_v2)){
            t_qd_opendoor_data_v2* p_qd_opendoor_data_v2 = (t_qd_opendoor_data_v2*)p_content->data_start;
            keep_time = (uint32_t)p_qd_opendoor_data_v2->keep_time * 500;
            user_id = p_qd_opendoor_data_v2->app_user_id;
            result = true;
            msg_record = build_open_door_ack_v2(_target_dev_mac.c_str(), QD_ACCESS_SUCCESS, NULL, 0);
        }
        if (p_javaMethodManger) {
            LOGD("%s result:%d, msg.len:%d", __FUNCTION__, result, msg_record.len);
            p_javaMethodManger->open_door_reply(QD_OPERATION_CONNECT, result, user_id, keep_time, msg_record.p_buffer, (uint8_t)msg_record.len);
        }
        if(msg_record.p_buffer){
            free(msg_record.p_buffer);
        }
    }
    //开门响应：由千丁门禁设备发出，千丁门禁sdk处理
    else
    {
        LOGD("%s response", __FUNCTION__);
        int record_item_len = p_content->content_length - sizeof(t_qd_access_content);
        if (record_item_len >= sizeof(t_qd_passby_record_item_v2)) {
            std::list<QDPassRecordEntity> record_list;
            char tmp_string[20];
            QDPassRecordEntity record_item;
            //预加上一个item的长度，防止数据不够的情况
            for (int i = 0; (i + sizeof(t_qd_passby_record_item_v2)) <=
                            record_item_len; i += sizeof(t_qd_passby_record_item_v2)) {
                t_qd_passby_record_item_v2 *p_record_item = (t_qd_passby_record_item_v2 *) (
                        p_content->data_start + i);

                sprintf(tmp_string, "%u", p_record_item->app_user_id);
                record_item.app_user_id = std::string(tmp_string);

                uint32_t passby_method_create_time = p_record_item->passby_method_create_time;
                sprintf(tmp_string, "%u", passby_method_create_time >> 28);
                record_item.pass_type = std::string(tmp_string);

                time_t current_time = time(NULL);
                long long_create_time = current_time - (current_time & 0x0fffffff) +
                                        (passby_method_create_time & 0x0fffffff);
                sprintf(tmp_string, "%ld", long_create_time);
                record_item.create_time = std::string(tmp_string);

                record_list.push_back(record_item);
            }
            LOGD("%s record_list =  %d  ", __FUNCTION__, record_list.size());
            if (p_javaMethodManger) {
                p_javaMethodManger->pass_record_ack(record_list);
            }
        } else {
            if (p_javaMethodManger) {
                p_javaMethodManger->open_door_ack(p_content->ack_type);
            }
        }
    }
}

void CMsgProcessor::process_open_door_ack_content_v5(t_qd_access_content *p_content) {
    LOGD("%s", __func__);
    int record_item_len = p_content->content_length - sizeof(t_qd_access_content);
    const int one_record_size = sizeof(t_qd_passby_record_item_v5);
    if (record_item_len >= one_record_size) {
        std::list<QDPassRecordEntity> record_list;
        char tmp_string[32];
        QDPassRecordEntity record_item;
        //预加上一个item的长度，防止数据不够的情况
        for (int i = 0; (i + one_record_size) <= record_item_len; i += one_record_size) {
            t_qd_passby_record_item_v5 *p_record_item = (t_qd_passby_record_item_v5 *) (
                    p_content->data_start + i);

            sprintf(tmp_string, "%lu", p_record_item->app_user_id);
            record_item.app_user_id = std::string(tmp_string);

            uint8_t passby_method_result = p_record_item->passby_method_result;
            sprintf(tmp_string, "%u", passby_method_result >> 3);
            record_item.pass_type = std::string(tmp_string);

            sprintf(tmp_string, "%u", passby_method_result & 0x07);
            record_item.pass_result = std::string(tmp_string);

            sprintf(tmp_string, "%u", p_record_item->create_time);
            record_item.create_time = std::string(tmp_string);

            record_list.push_back(record_item);
        }
        LOGD("%s record_list =  %d  ", __func__, record_list.size());
        if (p_javaMethodManger) {
            p_javaMethodManger->pass_record_ack(record_list);
        }
    } else {
        if (p_javaMethodManger) {
            p_javaMethodManger->open_door_ack(p_content->ack_type);
        }
    }
}

void CMsgProcessor::process_update_state_report_ack_v2(t_qd_access_content *p_content) {
    LOGD("%s", __FUNCTION__);
    t_qd_update_file_state_data_v2 *update_file_state = (t_qd_update_file_state_data_v2 *) (
            p_content->data_start);
    if (p_javaMethodManger) {
        p_javaMethodManger->update_state_report_ack(update_file_state);
    }
}

void CMsgProcessor::process_write_card_content_ack_v2(t_qd_access_content *p_content) {
    int result = p_content->ack_type;
    LOGD("%s ack ==  %02X ", __FUNCTION__, result);
    if (p_content->content_length >= 7) {
        char buff[64] = {0};
        for (int i = 0; i < 7; i++) {
            sprintf(buff + (i * 2), "%02X", p_content->data_start[i]);
        }
        std::string str(buff);
        if (p_javaMethodManger) {
            p_javaMethodManger->distribute_card_ack(result, str);
        }
    } else {
        if (p_javaMethodManger) {
            p_javaMethodManger->receive_ack(result, p_content->command_type);
        }
    }
}

void CMsgProcessor::process_read_card_content_ack_v2(t_qd_access_content *p_content) {
    int result = p_content->ack_type;
    LOGD("%s ack ==  %02X ", __FUNCTION__, result);
    if (p_content->content_length >= 7) {
        char buff[64] = {0};
        for (int i = 0; i < 7; i++) {
            sprintf(buff + (i * 2), "%02X", p_content->data_start[i]);
        }
        std::string str(buff);
        if (p_javaMethodManger) {
            p_javaMethodManger->read_card_ack(result, str);
        }
    } else {
        if (p_javaMethodManger) {
            p_javaMethodManger->receive_ack(result, p_content->command_type);
        }
    }
}

void CMsgProcessor::process_standalone_card_read_ack_v2(t_qd_access_content *p_content){
    int ack_type = p_content->ack_type;
    LOGD("%s...ack_type:%02X", __FUNCTION__, ack_type);
    if (p_content->content_length >= (sizeof(t_qd_access_content) + sizeof(uint32_t)*2)) {
        uint32_t * data = (uint32_t *)p_content->data_start;
        LOGD("%s...operate_result:%d", __FUNCTION__, data[0]);
        if (p_javaMethodManger) {
            p_javaMethodManger->standalone_read_card_ack(data[0], data[1]);
        }
    } else {
        if (p_javaMethodManger) {
            p_javaMethodManger->receive_ack(ack_type, p_content->command_type);
        }
    }
}

void CMsgProcessor::process_standalone_card_add_ack_v2(t_qd_access_content *p_content){
    int ack_type = p_content->ack_type;
    LOGD("%s...ack_type:%02X", __FUNCTION__, ack_type);
    if (p_content->content_length >= (sizeof(t_qd_access_content) + sizeof(uint32_t)*2)) {
        uint32_t * data = (uint32_t *)p_content->data_start;
        LOGD("%s...operate_result:%d", __FUNCTION__, data[0]);
        if (p_javaMethodManger) {
            p_javaMethodManger->standalone_add_card_ack(data[0]);
        }
    } else {
        if (p_javaMethodManger) {
            p_javaMethodManger->receive_ack(ack_type, p_content->command_type);
        }
    }
}

void CMsgProcessor::process_standalone_card_del_ack_v2(t_qd_access_content *p_content){
    int ack_type = p_content->ack_type;
    LOGD("%s...ack_type:%02X", __FUNCTION__, ack_type);
    if (p_content->content_length >= (sizeof(t_qd_access_content) + sizeof(uint32_t)*2)) {
        uint32_t * data = (uint32_t *)p_content->data_start;
        LOGD("%s...operate_result:%d", __FUNCTION__, data[0]);
        if (p_javaMethodManger) {
            p_javaMethodManger->standalone_del_card_ack(data[0]);
        }
    } else {
        if (p_javaMethodManger) {
            p_javaMethodManger->receive_ack(ack_type, p_content->command_type);
        }
    }
}


void CMsgProcessor::process_config_device_ack_content_v2(t_qd_access_content *p_content) {
    LOGD("%s", __FUNCTION__);
    if (p_javaMethodManger) {
        p_javaMethodManger->config_device_ack(p_content->ack_type);
    }
}

void CMsgProcessor::process_get_device_config_ack_content_v2(t_qd_access_content *p_content) {
    int ack_type = p_content->ack_type;
    LOGD("%s...ack_type:%02X", __FUNCTION__, ack_type);
    int config_len = p_content->content_length - sizeof(t_qd_access_content);
    if(config_len >= (int) sizeof(t_qd_config_ack_data_v2)){
        if (p_javaMethodManger) {
            p_javaMethodManger->get_config_ack_v2((t_qd_config_ack_data_v2 *) p_content->data_start, config_len);
        }
    } else {
        if (p_javaMethodManger) {
            p_javaMethodManger->receive_ack(ack_type, p_content->command_type);
        }
    }
}

void CMsgProcessor::process_get_device_version_ack_content_v2(t_qd_access_content *p_content) {
    int ack_type = p_content->ack_type;
    LOGD("%s...ack_type:%02X", __FUNCTION__, ack_type);
    int version_len = p_content->content_length - sizeof(t_qd_access_content);
    if(version_len >= (int) sizeof(t_qd_version_ack_data_v2)){
        if (p_javaMethodManger) {
            t_qd_version_ack_data_v2* p_version = (t_qd_version_ack_data_v2*)p_content->data_start;
            p_javaMethodManger->get_version_ack(std::string((char*)p_version->software_version), std::string((char*)p_version->hardware_version));
        }
    } else {
        if (p_javaMethodManger) {
            p_javaMethodManger->receive_ack(ack_type, p_content->command_type);
        }
    }
}

void CMsgProcessor::process_update_device_ack_content_v2(t_qd_access_content *p_content) {
    LOGD("%s", __FUNCTION__);
    if (p_javaMethodManger) {
        p_javaMethodManger->update_device_ack(p_content->ack_type);
    }
}

void CMsgProcessor::process_set_buzzer_mode_content_v2(t_qd_access_content *p_content) {

}

void CMsgProcessor::process_set_relay_state_content_v2(t_qd_access_content *p_content) {

}

void CMsgProcessor::process_enable_433_test_ack_v2(t_qd_access_content *p_content) {
    int ack_type = p_content->ack_type;
    LOGD("%s...ack_type:%02X", __FUNCTION__, ack_type);
    if (p_javaMethodManger) {
        p_javaMethodManger->enable_433_test_ack(ack_type);
    }
}

void CMsgProcessor::process_get_hal_state_ack_v2(t_qd_access_content *p_content) {
    int ack_type = p_content->ack_type;
    LOGD("%s...ack_type:%02X", __FUNCTION__, ack_type);
    int config_len = p_content->content_length - sizeof(t_qd_access_content);
    if(config_len >= (int) sizeof(int8_t)){
        if (p_javaMethodManger) {
            p_javaMethodManger->get_hal_state_ack(p_content->data_start[0]);
        }
    } else {
        if (p_javaMethodManger) {
            p_javaMethodManger->receive_ack(ack_type, p_content->command_type);
        }
    }
}


void CMsgProcessor::process_config_device_ack_content_v4(t_qd_access_content *p_content){
    LOGD("%s", __FUNCTION__);
    //验证配置结果数据
    if (p_javaMethodManger) {
        p_javaMethodManger->config_device_ack(p_content->ack_type);
    }
}

void CMsgProcessor::process_test_get_device_info_v4(t_qd_access_content *p_content){
    LOGD("%s", __FUNCTION__);
    int size = p_content->content_length - sizeof(t_qd_access_content);
    if(size >= (int)sizeof(t_qd_devinfo_ack_data_v4)){
        t_qd_devinfo_ack_data_v4* devinfo_ack_data = (t_qd_devinfo_ack_data_v4*)p_content->data_start;
        if (p_javaMethodManger) {
            p_javaMethodManger->get_device_info_ack(devinfo_ack_data);
        }
    }else {
        if (p_javaMethodManger) {
            p_javaMethodManger->receive_ack(p_content->ack_type, p_content->command_type);
        }
    }
}

void CMsgProcessor::process_test_notify_event_v4(t_qd_access_content *p_content){
    LOGD("%s", __FUNCTION__);
    int size = p_content->content_length - sizeof(t_qd_access_content);
    if(p_content->command_type == QD_CMD_NOTIFY_EVENT && size >= sizeof(t_qd_notify_data_v4)){
        t_qd_notify_data_v4* notify_data = (t_qd_notify_data_v4*) p_content->data_start;
        //类型
        int type = notify_data->type;
        //数据
        char buffer[36] = {0};
        snprintf(buffer, sizeof(buffer), "%llX", notify_data->data);
        std::string data(buffer);
        LOGD("%s...data:%s, type:%u", __FUNCTION__, data.c_str(), type);
        if (p_javaMethodManger) {
            p_javaMethodManger->notify_test_event_ack(type, data);
        }
    }else {
        if (p_javaMethodManger) {
            p_javaMethodManger->receive_ack(p_content->ack_type, p_content->command_type);
        }
    }
}

void CMsgProcessor::process_broadcast_open_door_v2(void *p_data, int data_len, std::string target_device_mac){
    LOGD("%s target_device_mac:%s", __FUNCTION__, target_device_mac.c_str());
    if (_target_dev_mac != target_device_mac) {
        _target_dev_mac = target_device_mac;
    }
    if(p_data && data_len >= 24){
        char base64_str[25] = {0};
        memcpy(base64_str, p_data, 24);
        uint8_t buf[64] = {0};
        base64_decode_ht((const char *) base64_str, buf);
        LOGI("base64_str == %s", base64_str);
        t_phone_adv_msg *p_adv_msg = (t_phone_adv_msg *) buf;
        LOGI("Version = %02X key = %u\n", p_adv_msg->version, p_adv_msg->key);
        switch (p_adv_msg->version) {
            case QD_BLE_HOOK_PROTOCOL_VER: {
                std::string tmp_mac = target_device_mac;
                LOGD("tmp_mac == %s", (uint8_t *) tmp_mac.c_str());
                int rc = ble_decrypt((uint8_t *) tmp_mac.c_str(),
                                     (uint16_t) tmp_mac.length(),
                                     (uint8_t *) &p_adv_msg->app_user_id,
                                     sizeof(t_phone_adv_msg) - 3,
                                     (uint16_t) p_adv_msg->key);

                LOGI("app_user_id == %u, decrypt == 广播数据解密%s", p_adv_msg->app_user_id, rc==0?"失败":"成功");
                if (p_javaMethodManger) {
                    t_dev_ack_id tmp_ack_byte = {0};
                    tmp_ack_byte.open_reslut = (uint8_t)(rc != 0 ? QD_ACCESS_SUCCESS : QD_ACCESS_FAILED);
                    memcpy(tmp_ack_byte.two_byte_app_user_id, (uint8_t *) &p_adv_msg->app_user_id, 2);
                    char ack_id[8] = {0};
                    base64_encode_ht(&tmp_ack_byte.open_reslut, ack_id, sizeof(tmp_ack_byte));
                    LOGI("ack_id = %s", ack_id);
                    p_javaMethodManger->open_door_reply(QD_OPERATION_BROADCAST,
                                                        rc != 0,
                                                        p_adv_msg->app_user_id,
                                                        5000,
                                                        (uint8_t *)ack_id,
                                                        (uint8_t)strlen(ack_id));
                }
            }
            break;
            default:break;
        }
    }
}

void CMsgProcessor::process_get_gate_magnetism_state_ack_v5(t_qd_access_content *p_content)
{
    int ack_type = p_content->ack_type;
    LOGD("%s...ack_type:%02X", __FUNCTION__, ack_type);
    int config_len = p_content->content_length - sizeof(t_qd_access_content);
    if(config_len >= (int) sizeof(int8_t)){
        if (p_javaMethodManger) {
            p_javaMethodManger->get_gate_magnetism_state_ack(p_content->data_start[0]);
        }
    } else {
        if (p_javaMethodManger) {
            p_javaMethodManger->receive_ack(ack_type, p_content->command_type);
        }
    }
}