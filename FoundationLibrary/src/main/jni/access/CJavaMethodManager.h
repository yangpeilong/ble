//
// Created by 胡涛 on 2017/5/11.
//

#ifndef QRCODE_CJAVAMETHODMANAGER_H
#define QRCODE_CJAVAMETHODMANAGER_H

#include <jni.h>
#include <pthread.h>
#include <string.h>
#include <list>
#include "QDPassRecordEntity.h"
#include "qd_access_protocol_v2_define.h"
#include "qd_access_protocol_v2_helper.h"
#include "qd_access_protocol_define.h"

#define  QD_OPERATION_BROADCAST 1
#define  QD_OPERATION_CONNECT   2

class CJavaMethodManager {
public:
    CJavaMethodManager(JNIEnv *env, jobject obj);
    void update_jni_env(JNIEnv *env);
    int receive_ack(int result_no, uint8_t cmd_type);
    int open_door_ack(int result_no);
    int pass_record_ack(std::list<QDPassRecordEntity> &record_list);
    int update_state_report_ack(t_qd_update_file_state_data_v2* update_file_state);
    int read_card_ack(int result_no, std::string card_no);
    int distribute_card_ack(int result_no, std::string card_no);
    int get_version_ack(std::string software_version, std::string hardware_version);
    int get_config_ack_v1(t_qd_get_device_config_ack_data *p_config_ack, int config_length);
    int get_config_ack_v2(t_qd_config_ack_data_v2 *p_config_ack, int config_length);
    int config_device_ack(int result_no);
    int update_device_ack(int result_no);
    //离线读取卡号响应
    int standalone_read_card_ack(int result, uint32_t cardNo);
    //离线添加卡号响应
    int standalone_add_card_ack(int result);
    //离线删除卡号响应
    int standalone_del_card_ack(int result);

    /////////////////////////产测接口/////////////////////////////
    int enable_433_test_ack(int result_no);
    int get_hal_state_ack(int state);
    int get_device_info_ack(t_qd_devinfo_ack_data_v4 *p_devinfo_ack);
    int notify_test_event_ack(int type, std::string data);
    int get_gate_magnetism_state_ack(int state);
    /////////////////////////千丁蓝牙门禁设备接口/////////////////////////////
    //开门请求回复
    int open_door_reply(int operation, bool result, uint32_t user_id, uint32_t keep_time, uint8_t* data, uint8_t size);

    virtual ~CJavaMethodManager();

private:
    jstring transformCStringToJString(const int size, const char *data);
private:
    JNIEnv* jniEnv;
};


#endif //QRCODE_CJAVAMETHODMANAGER_H
