//
// Created by 胡涛 on 2017/5/11.
//

#include "CJavaMethodManager.h"
#include "debugLog.h"

#ifndef QD_ACCESS_MSG_HANDLER_CLASS_PATH_NAME
#define QD_ACCESS_MSG_HANDLER_CLASS_PATH_NAME "com/qdingnet/qdaccess/QDAccessMsgHandler"
#endif

#ifndef QD_CONFIG_HANDLER_CLASS_PATH_NAME
#define QD_CONFIG_HANDLER_CLASS_PATH_NAME "com/qdingnet/qdaccess/QDConfigMsgHandler"
#endif

#ifndef QD_FACTORY_TEST_HANDLER_CLASS_PATH_NAME
#define QD_FACTORY_TEST_HANDLER_CLASS_PATH_NAME "com/qdingnet/qdaccess/QDFactoryTestMsgHandler"
#endif

#ifndef QD_DEVICE_SDK_HANDLER_CLASS_PATH_NAME
#define QD_DEVICE_SDK_HANDLER_CLASS_PATH_NAME "com/qdingnet/qdaccess/QDDeviceSdkMsgHandler"
#endif

#include <jni.h>
#include <stdio.h>
#include <stdint.h>

#define min(X, Y) \
(__extension__ \
    ({ \
        typeof(X) __x=(X), __y=(Y); \
        (void) (&__x == &__y); \
        (__x<__y)?__x:__y;  \
    }) \
)


CJavaMethodManager::CJavaMethodManager(JNIEnv *env, jobject obj) {
    jniEnv = env;
}

CJavaMethodManager::~CJavaMethodManager() {

}


void CJavaMethodManager::update_jni_env(JNIEnv *env) {
    jniEnv = env;
}

int CJavaMethodManager::receive_ack(int result_no, uint8_t cmd_type) {
    if (jniEnv == NULL) {
        return -1;
    }
    LOGI("receive_ack result = %d, cmd_type == %02X", result_no, cmd_type);
    jclass callBackClass = jniEnv->FindClass(QD_ACCESS_MSG_HANDLER_CLASS_PATH_NAME);
    jmethodID on_open_door_ack_method_id = jniEnv->GetStaticMethodID(callBackClass, "onReceiveAck", "(IB)V");
    if (on_open_door_ack_method_id) {
        jniEnv->CallStaticVoidMethod(callBackClass, on_open_door_ack_method_id, result_no, cmd_type);
        //释放本地引用
        jniEnv->DeleteLocalRef(callBackClass);
        return 0;
    }
    else{
        return -1;
    }
}


int CJavaMethodManager::open_door_ack(int result_no) {
    if (jniEnv == NULL) {
        return -1;
    }
    LOGI("open_door_ack result = %d", result_no);
    jclass callBackClass = jniEnv->FindClass(QD_ACCESS_MSG_HANDLER_CLASS_PATH_NAME);
    jmethodID on_open_door_ack_method_id = jniEnv->GetStaticMethodID(callBackClass, "onOpenDoorAck", "(I)V");
    if (on_open_door_ack_method_id) {
        jniEnv->CallStaticVoidMethod(callBackClass, on_open_door_ack_method_id, result_no);
        //释放本地引用
        jniEnv->DeleteLocalRef(callBackClass);
        return 0;
    }
    else{
        return -1;
    }

}

int CJavaMethodManager::pass_record_ack(std::list<QDPassRecordEntity> &record_list) {
    LOGI("pass_record_ack size == %d", record_list.size());
    if (jniEnv == NULL) {
        return -1;
    }
    jclass callBackClass = jniEnv->FindClass(QD_ACCESS_MSG_HANDLER_CLASS_PATH_NAME);
    jmethodID on_get_open_door_record_method_id = jniEnv->GetStaticMethodID(callBackClass,
                                                                            "onPassRecordsAck",
                                                                            "(Ljava/util/ArrayList;)V");
    if (on_get_open_door_record_method_id) {

        // 构建java层 list对象
        jclass list_cls = jniEnv->FindClass("java/util/ArrayList"); //获得ArrayList类引用
        jmethodID list_constructor = jniEnv->GetMethodID(list_cls, "<init>", "()V");
        jmethodID list_add = jniEnv->GetMethodID(list_cls, "add", "(Ljava/lang/Object;)Z");
        jobject record_list_object = jniEnv->NewObject(list_cls, list_constructor);


        // 构建java 通行记录对象
        jclass record_entity_class = jniEnv->FindClass("com/qdingnet/qdaccess/QDPassRecordEntity");
        jmethodID record_constructor = jniEnv->GetMethodID(record_entity_class,
                                                           "<init>",
                                                           "(Ljava/lang/String;"
                                                           "Ljava/lang/String;"
                                                           "Ljava/lang/String;"
                                                           "Ljava/lang/String;"
                                                           "Ljava/lang/String;"
                                                           "Ljava/lang/String;"
                                                           "Ljava/lang/String;"
                                                           "Ljava/lang/String;)V");

        for (QDPassRecordEntity &item:record_list) {
            jstring app_user_id = jniEnv->NewStringUTF(item.app_user_id.c_str());
            jstring create_time = jniEnv->NewStringUTF(item.create_time.c_str());
            jstring passby_method = jniEnv->NewStringUTF(item.pass_type.c_str());
            jstring pass_result = jniEnv->NewStringUTF(item.pass_result.c_str());
            jstring dev_mac = jniEnv->NewStringUTF(item.dev_mac.c_str());
            jstring record_index = jniEnv->NewStringUTF(item.record_index.c_str());
            jstring room_id = jniEnv->NewStringUTF(item.room_id.c_str());
            jstring server_id = jniEnv->NewStringUTF(item.server_id.c_str());
            // 构建java层通行记录对象
            jobject record_obj = jniEnv->NewObject(record_entity_class, record_constructor,
                                                   record_index,
                                                   app_user_id,
                                                   server_id,
                                                   room_id,
                                                   dev_mac,
                                                   create_time,
                                                   passby_method,
                                                   pass_result);

            jniEnv->CallBooleanMethod(record_list_object, list_add, record_obj);
            //释放局部引用
            jniEnv->DeleteLocalRef(app_user_id);
            jniEnv->DeleteLocalRef(create_time);
            jniEnv->DeleteLocalRef(passby_method);
            jniEnv->DeleteLocalRef(dev_mac);
            jniEnv->DeleteLocalRef(record_index);
            jniEnv->DeleteLocalRef(room_id);
            jniEnv->DeleteLocalRef(server_id);
            jniEnv->DeleteLocalRef(record_obj);
        }

        jniEnv->CallStaticVoidMethod(callBackClass, on_get_open_door_record_method_id,record_list_object);
        //释放本地引用
        jniEnv->DeleteLocalRef(list_cls);
        jniEnv->DeleteLocalRef(record_list_object);
        jniEnv->DeleteLocalRef(record_entity_class);
        jniEnv->DeleteLocalRef(callBackClass);
        return 0;
    }
    else
    {
        return -1;
    }
}

int CJavaMethodManager::distribute_card_ack(int result_no, std::string card_no) {
    LOGI("distribute_card_ack %s ", card_no.c_str());
    if (jniEnv == NULL) {
        return -1;
    }
    jclass callBackClass = jniEnv->FindClass(QD_ACCESS_MSG_HANDLER_CLASS_PATH_NAME);
    jmethodID on_distribute_card_ack_method_id = jniEnv->GetStaticMethodID(callBackClass,
                                                                           "onDistributeCardAck",
                                                                           "(ILjava/lang/String;)V");
    if (on_distribute_card_ack_method_id)
    {
        jstring card_no_string = jniEnv->NewStringUTF(card_no.c_str());
        jniEnv->CallStaticVoidMethod(callBackClass, on_distribute_card_ack_method_id, result_no, card_no_string);
        //释放本地引用
        jniEnv->DeleteLocalRef(card_no_string);
        jniEnv->DeleteLocalRef(callBackClass);
        return 0;
    }
    else
    {
        return -1;
    }
}

int CJavaMethodManager::read_card_ack(int result_no, std::string card_no) {
    LOGI("read_card_ack %s ", card_no.c_str());
    if (jniEnv == NULL) {
        return -1;
    }
    jclass callBackClass = jniEnv->FindClass(QD_ACCESS_MSG_HANDLER_CLASS_PATH_NAME);
    jmethodID on_read_card_ack_method_id = jniEnv->GetStaticMethodID(callBackClass,
                                                                     "onReadCardAck",
                                                                     "(ILjava/lang/String;)V");
    if (on_read_card_ack_method_id)
    {
        jstring card_no_string = jniEnv->NewStringUTF(card_no.c_str());
        jniEnv->CallStaticVoidMethod(callBackClass, on_read_card_ack_method_id, result_no, card_no_string);
        //释放本地引用
        jniEnv->DeleteLocalRef(card_no_string);
        jniEnv->DeleteLocalRef(callBackClass);
        return 0;
    }
    else
    {
        return -1;
    }
}

int CJavaMethodManager::update_state_report_ack(
        t_qd_update_file_state_data_v2 *update_file_state) {
    if (jniEnv == NULL) {
        return -1;
    }
    jclass callBackClass = jniEnv->FindClass(QD_ACCESS_MSG_HANDLER_CLASS_PATH_NAME);
    jmethodID on_update_state_report_ack_id = jniEnv->GetStaticMethodID(callBackClass,
                                                                        "onUpdateStateReportAck",
                                                                        "(Ljava/lang/String;Ljava/lang/String;III)V");
    if(on_update_state_report_ack_id)
    {
        //软件版本
        const char *p_software_version = (const char *) update_file_state->software_version_name;
        const int software_version_size = min(strlen(p_software_version), sizeof(update_file_state->software_version_name));
        jstring software_version = transformCStringToJString(software_version_size, p_software_version);
        //硬件版本
        const char *p_hardware_version = (const char *) update_file_state->hardware_version_name;
        const int hardware_version_size = min(strlen(p_hardware_version), sizeof(update_file_state->hardware_version_name));
        jstring hardware_version = transformCStringToJString(hardware_version_size, p_hardware_version);
        //LOGI("update_state_report_ack...software_version_size:%d, hardware_version_size:%d", software_version_size, hardware_version_size);
        jint software_version_num = update_file_state->software_version_num;
        jint current_update_file_pos = update_file_state->current_update_file_pos;
        jint current_update_version_num = update_file_state->current_update_version_num;
        jniEnv->CallStaticVoidMethod(callBackClass, on_update_state_report_ack_id, software_version,
                                     hardware_version,
                                     software_version_num, current_update_file_pos,
                                     current_update_version_num);
        //释放本地引用
        jniEnv->DeleteLocalRef(software_version);
        jniEnv->DeleteLocalRef(hardware_version);
        jniEnv->DeleteLocalRef(callBackClass);
        return 0;
    }
    else
    {
        return -1;
    }
}

/**
 * 将c端字符串转为java端字符串
 * @param size
 * @param data
 * @return
 */
jstring CJavaMethodManager::transformCStringToJString(const int size, const char *data) {
    jbyteArray array = jniEnv->NewByteArray(size);
    jniEnv->SetByteArrayRegion(array, 0, size, (jbyte *) data);
    //调用java端字符串构造方法
    jstring strEncode = jniEnv->NewStringUTF("UTF-8");
    jclass cls = jniEnv->FindClass("java/lang/String");
    jmethodID ctor = jniEnv->GetMethodID(cls, "<init>", "([BLjava/lang/String;)V");
    jstring result = (jstring) jniEnv->NewObject(cls, ctor, array, strEncode);
    //释放本地资源
    jniEnv->DeleteLocalRef(array);
    jniEnv->DeleteLocalRef(strEncode);
    jniEnv->DeleteLocalRef(cls);
    return result;
}

//离线读取卡号响应
int CJavaMethodManager::standalone_read_card_ack(int result, uint32_t cardNo){
    LOGI("%s...result:%d, cardNo:%u ", __FUNCTION__, result, cardNo);
    if (jniEnv == NULL) {
        return -1;
    }
    jclass callBackClass = jniEnv->FindClass(QD_ACCESS_MSG_HANDLER_CLASS_PATH_NAME);
    jmethodID methodId = jniEnv->GetStaticMethodID(callBackClass,
                                                   "onStandaloneReadCardAck",
                                                   "(IJ)V");
    if (methodId) {
        jlong jCardNo = 0l + cardNo;
        jniEnv->CallStaticVoidMethod(callBackClass, methodId, result, jCardNo);
        //释放本地引用
        jniEnv->DeleteLocalRef(callBackClass);
        return 0;
    }
    else
    {
        return -1;
    }
}
//离线添加卡号响应
int CJavaMethodManager::standalone_add_card_ack(int result){
    LOGI("%s...result:%d", __FUNCTION__, result);
    if (jniEnv == NULL) {
        return -1;
    }
    jclass callBackClass = jniEnv->FindClass(QD_ACCESS_MSG_HANDLER_CLASS_PATH_NAME);
    jmethodID methodId = jniEnv->GetStaticMethodID(callBackClass,
                                                   "onStandaloneAddCardAck",
                                                   "(I)V");
    if (methodId) {
        jniEnv->CallStaticVoidMethod(callBackClass, methodId, result);
        //释放本地引用
        jniEnv->DeleteLocalRef(callBackClass);
        return 0;
    }
    else
    {
        return -1;
    }
}
//离线删除卡号响应
int CJavaMethodManager::standalone_del_card_ack(int result){
    LOGI("%s...result:%d", __FUNCTION__, result);
    if (jniEnv == NULL) {
        return -1;
    }
    jclass callBackClass = jniEnv->FindClass(QD_ACCESS_MSG_HANDLER_CLASS_PATH_NAME);
    jmethodID methodId = jniEnv->GetStaticMethodID(callBackClass,
                                                   "onStandaloneDelCardAck",
                                                   "(I)V");
    if (methodId) {
        jniEnv->CallStaticVoidMethod(callBackClass, methodId, result);
        //释放本地引用
        jniEnv->DeleteLocalRef(callBackClass);
        return 0;
    }
    else
    {
        return -1;
    }
}

////////////////////////////////////////设备配置功能/////////////////////////////////////////////////
/**
 * 配置设备响应
 * @param result_no
 * @return
 */
int CJavaMethodManager::config_device_ack(int result_no){
    LOGI("%s...result:%d", __FUNCTION__, result_no);
    if (jniEnv == NULL) {
        return -1;
    }
    jclass callBackClass = jniEnv->FindClass(QD_CONFIG_HANDLER_CLASS_PATH_NAME);
    jmethodID methodId = jniEnv->GetStaticMethodID(callBackClass,
                                                   "onWriteConfigAck",
                                                   "(I)V");
    if (methodId) {
        jniEnv->CallStaticVoidMethod(callBackClass, methodId, result_no);
        //释放本地引用
        jniEnv->DeleteLocalRef(callBackClass);
        return 0;
    }
    else
    {
        return -1;
    }
}

/**
 * 获取设备配置响应(v1版本)
 * @param p_config_ack
 * @param config_length
 * @return
 */
int CJavaMethodManager::get_config_ack_v1(t_qd_get_device_config_ack_data *p_config_ack, int config_length){
    LOGI("%s...", __FUNCTION__);
    if (jniEnv == NULL) {
        return -1;
    }
    jclass callBackClass = jniEnv->FindClass(QD_CONFIG_HANDLER_CLASS_PATH_NAME);
    jmethodID on_get_config_method_id = jniEnv->GetStaticMethodID(callBackClass,
                                                                  "onReadConfigAck",
                                                                  "(Lcom/qdingnet/qdaccess/QDDeviceConfigEntity;)V");
    if (on_get_config_method_id)
    {
        jclass device_config_entity_cls = jniEnv->FindClass("com/qdingnet/qdaccess/QDDeviceConfigEntity");
        //设备开门时长
        jint open_keep_time = p_config_ack->open_keep_time;
        //设备当前时间
        jlong device_time = p_config_ack->device_time;
        //最大单元门数量
        jint max_doors_size = p_config_ack->max_doors_size;
        //单个单元最大有效访客数量计数
        jint max_counter_size = p_config_ack->max_counter_size;
        //设备固件版本
        jstring software_version = jniEnv->NewStringUTF((const char *) p_config_ack->software_version);
        //设备硬件版本
        jstring hardware_version = jniEnv->NewStringUTF((const char *) p_config_ack->hardware_version);
        //设备配置的单元数据
        jstring related_doors = NULL;
        if(config_length - (int)sizeof(t_qd_get_device_config_ack_data) > 0){
            related_doors = jniEnv->NewStringUTF((const char *)&(p_config_ack->related_doors_start));
        }
        //创建设备配置信息实体
        jmethodID record_constructor = jniEnv->GetMethodID(device_config_entity_cls, "<init>", "(IIIJLjava/lang/String;Ljava/lang/String;Ljava/lang/String;)V");
        jobject device_config_entity = jniEnv->NewObject(device_config_entity_cls,
                                                         record_constructor,
                                                         open_keep_time,
                                                         max_doors_size,
                                                         max_counter_size,
                                                         device_time,
                                                         software_version,
                                                         hardware_version,
                                                         related_doors);
        //回调java接口
        jniEnv->CallStaticVoidMethod(callBackClass, on_get_config_method_id, device_config_entity);
        //释放java本地引用
        jniEnv->DeleteLocalRef(callBackClass);
        jniEnv->DeleteLocalRef(device_config_entity_cls);
        jniEnv->DeleteLocalRef(device_config_entity);
        jniEnv->DeleteLocalRef(software_version);
        jniEnv->DeleteLocalRef(hardware_version);
        if(related_doors){
            jniEnv->DeleteLocalRef(related_doors);
        }
        return 0;
    }
    else
    {
        return -1;
    }
}

/**
 * 获取设备配置响应(v2版本)
 * @param p_config_ack
 * @return
 */
int CJavaMethodManager::get_config_ack_v2(t_qd_config_ack_data_v2 *p_config_ack, int config_length){
    LOGI("%s...", __FUNCTION__);
    if (jniEnv == NULL) {
        return -1;
    }
    jclass callBackClass = jniEnv->FindClass(QD_CONFIG_HANDLER_CLASS_PATH_NAME);
    jmethodID on_get_config_method_id = jniEnv->GetStaticMethodID(callBackClass,
                                                                            "onReadConfigAck",
                                                                            "(Lcom/qdingnet/qdaccess/QDDeviceConfigEntity;)V");
    if (on_get_config_method_id)
    {
        jclass device_config_entity_cls = jniEnv->FindClass("com/qdingnet/qdaccess/QDDeviceConfigEntity");
        //设备开门时长
        jint open_keep_time = p_config_ack->open_keep_time;
        //设备当前时间
        jlong device_time = p_config_ack->device_time;
        //设备固件版本
        jstring software_version = jniEnv->NewStringUTF((const char *) p_config_ack->software_version);
        //设备硬件版本
        jstring hardware_version = jniEnv->NewStringUTF((const char *) p_config_ack->hardware_version);

        //设备配置的单元信息
        jobject unitItemList = NULL;
        int record_item_len = config_length - sizeof(t_qd_config_ack_data_v2);
        if (record_item_len >= (int) sizeof(t_qd_unit_item)) {
            jclass unit_item_class = jniEnv->FindClass("com/qdingnet/qdaccess/QDUnitItem");
            jmethodID constructor_unit_item = jniEnv->GetMethodID(unit_item_class, "<init>", "(JII)V");
            //获得ArrayList类引用
            jclass list_cls = jniEnv->FindClass("java/util/ArrayList");
            jmethodID list_constructor = jniEnv->GetMethodID(list_cls, "<init>", "()V");
            unitItemList = jniEnv->NewObject(list_cls, list_constructor);
            jmethodID list_add = jniEnv->GetMethodID(list_cls, "add", "(Ljava/lang/Object;)Z");

            for (int i = 0; i < record_item_len; i += sizeof(t_qd_unit_item)) {
                t_qd_unit_item *p_unit_item = (t_qd_unit_item *) (p_config_ack->related_doors_start + i);
                jobject record_obj = jniEnv->NewObject(unit_item_class,
                                                       constructor_unit_item,
                                                       (jlong) p_unit_item->unit_num,
                                                       p_unit_item->multi_time_counter,
                                                       p_unit_item->single_time_counter);
                jniEnv->CallBooleanMethod(unitItemList, list_add, record_obj);
                //释放java本地引用
                jniEnv->DeleteLocalRef(record_obj);
            }
            //释放java本地引用
            jniEnv->DeleteLocalRef(unit_item_class);
            jniEnv->DeleteLocalRef(list_cls);
        }
        //创建设备配置信息实体
        jmethodID record_constructor = jniEnv->GetMethodID(device_config_entity_cls, "<init>", "(IJLjava/lang/String;Ljava/lang/String;Ljava/util/ArrayList;)V");
        jobject device_config_entity = jniEnv->NewObject(device_config_entity_cls,
                                                         record_constructor,
                                                         open_keep_time,
                                                         device_time,
                                                         software_version,
                                                         hardware_version,
                                                         unitItemList);
        //回调java接口
        jniEnv->CallStaticVoidMethod(callBackClass, on_get_config_method_id, device_config_entity);
        //释放java本地引用
        jniEnv->DeleteLocalRef(callBackClass);
        jniEnv->DeleteLocalRef(device_config_entity_cls);
        jniEnv->DeleteLocalRef(software_version);
        jniEnv->DeleteLocalRef(hardware_version);
        if(unitItemList){
            jniEnv->DeleteLocalRef(unitItemList);
        }
        jniEnv->DeleteLocalRef(device_config_entity);
        return 0;
    }
    else
    {
        return -1;
    }
}

int CJavaMethodManager::get_version_ack(std::string software_version, std::string hardware_version){
    LOGI("%s...", __FUNCTION__);
    if (jniEnv == NULL) {
        return -1;
    }
    jclass callBackClass = NULL;
#if CONFIG
    callBackClass = jniEnv->FindClass(QD_CONFIG_HANDLER_CLASS_PATH_NAME);
#endif

#if FACTORY
    callBackClass = jniEnv->FindClass(QD_FACTORY_TEST_HANDLER_CLASS_PATH_NAME);
#endif

    jmethodID on_get_version_method_id = callBackClass != NULL ? jniEnv->GetStaticMethodID(callBackClass,
                                                                  "onGetVersionAck",
                                                                  "(Ljava/lang/String;Ljava/lang/String;)V")
                                                               : NULL;
    if (on_get_version_method_id)
    {
        jstring software = jniEnv->NewStringUTF(software_version.c_str());
        jstring hardware = jniEnv->NewStringUTF(hardware_version.c_str());
        jniEnv->CallStaticVoidMethod(callBackClass, on_get_version_method_id, software, hardware);
        //释放本地引用
        jniEnv->DeleteLocalRef(callBackClass);
        jniEnv->DeleteLocalRef(software);
        jniEnv->DeleteLocalRef(hardware);
        return 0;
    }
    else
    {
        return -1;
    }
}

int CJavaMethodManager::update_device_ack(int result_no){
    LOGI("%s...", __FUNCTION__);
    if (jniEnv == NULL) {
        return -1;
    }
    jclass callBackClass = jniEnv->FindClass(QD_CONFIG_HANDLER_CLASS_PATH_NAME);
    jmethodID on_update_device_method_id = jniEnv->GetStaticMethodID(callBackClass,
                                                   "onUpdateDeviceAck",
                                                   "(I)V");
    if (on_update_device_method_id) {
        jniEnv->CallStaticVoidMethod(callBackClass, on_update_device_method_id, result_no);
        //释放本地引用
        jniEnv->DeleteLocalRef(callBackClass);
        return 0;
    }
    else
    {
        return -1;
    }
}

////////////////////////////////////////设备产测功能/////////////////////////////////////////////////

int CJavaMethodManager::enable_433_test_ack(int result_no){
    LOGI("%s...", __FUNCTION__);
    if (jniEnv == NULL) {
        return -1;
    }
    jclass callBackClass = jniEnv->FindClass(QD_FACTORY_TEST_HANDLER_CLASS_PATH_NAME);
    jmethodID on_enable_433_test_method_id = jniEnv->GetStaticMethodID(callBackClass,
                                                                     "onEnable433TestAck",
                                                                     "(I)V");
    if (on_enable_433_test_method_id) {
        jniEnv->CallStaticVoidMethod(callBackClass, on_enable_433_test_method_id, result_no);
        //释放本地引用
        jniEnv->DeleteLocalRef(callBackClass);
        return 0;
    }
    else
    {
        return -1;
    }
}

int CJavaMethodManager::get_hal_state_ack(int state){
    LOGI("%s...", __FUNCTION__);
    if (jniEnv == NULL) {
        return -1;
    }
    jclass callBackClass = jniEnv->FindClass(QD_FACTORY_TEST_HANDLER_CLASS_PATH_NAME);
    jmethodID on_get_hal_state_method_id = jniEnv->GetStaticMethodID(callBackClass,
                                                                     "onGetHalStateAck",
                                                                     "(I)V");
    if (on_get_hal_state_method_id) {
        jniEnv->CallStaticVoidMethod(callBackClass, on_get_hal_state_method_id, state);
        //释放本地引用
        jniEnv->DeleteLocalRef(callBackClass);
        return 0;
    }
    else
    {
        return -1;
    }
}

int CJavaMethodManager::get_device_info_ack(t_qd_devinfo_ack_data_v4 *p_devinfo_ack){
    LOGI("%s...", __FUNCTION__);
    if (jniEnv == NULL) {
        return -1;
    }
    jclass callBackClass = jniEnv->FindClass(QD_FACTORY_TEST_HANDLER_CLASS_PATH_NAME);
    jmethodID on_get_device_info_method_id = jniEnv->GetStaticMethodID(callBackClass,
                                                                     "onReceiveDeviceInfoAck",
                                                                     "(Lcom/qdingnet/qdaccess/QDDeviceTestInfo;)V");
    if (on_get_device_info_method_id) {
        //回调参数值
        jstring software_version_str = jniEnv->NewStringUTF((char*)p_devinfo_ack->sw_ver);
        jstring hardware_version_str = jniEnv->NewStringUTF((char*)p_devinfo_ack->hw_ver);
        jstring serial_no_str = jniEnv->NewStringUTF((char*)p_devinfo_ack->serial_no);
        jstring imei_no_str = jniEnv->NewStringUTF((char*)p_devinfo_ack->imei);
        jstring mac_str = jniEnv->NewStringUTF((char*)p_devinfo_ack->mac);
        jint networkStatus = p_devinfo_ack->network_status & 0xff;
        //回调参数封装类
        jclass device_info_class = jniEnv->FindClass("com/qdingnet/qdaccess/QDDeviceTestInfo");
        jmethodID device_info_constructor = jniEnv->GetMethodID(device_info_class, "<init>","(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;I)V");
        jobject device_info = jniEnv->NewObject(device_info_class,
                                                device_info_constructor,
                                                software_version_str,
                                                hardware_version_str,
                                                serial_no_str,
                                                imei_no_str,
                                                mac_str,
                                                networkStatus);
        //回调接口
        jniEnv->CallStaticVoidMethod(callBackClass,on_get_device_info_method_id,device_info);
        //释放资源
        jniEnv->DeleteLocalRef(callBackClass);
        jniEnv->DeleteLocalRef(device_info_class);
        jniEnv->DeleteLocalRef(device_info);
        jniEnv->DeleteLocalRef(software_version_str);
        jniEnv->DeleteLocalRef(hardware_version_str);
        jniEnv->DeleteLocalRef(serial_no_str);
        jniEnv->DeleteLocalRef(imei_no_str);
        jniEnv->DeleteLocalRef(mac_str);
        return 0;
    }
    else
    {
        return -1;
    }
}

int CJavaMethodManager::notify_test_event_ack(int type, std::string data){
    LOGI("%s...", __FUNCTION__);
    if (jniEnv == NULL) {
        return -1;
    }
    jclass callBackClass = jniEnv->FindClass(QD_FACTORY_TEST_HANDLER_CLASS_PATH_NAME);
    jmethodID on_device_notify_method_id = jniEnv->GetStaticMethodID(callBackClass,
                                                              "onDeviceNotifyEventAck",
                                                              "(Lcom/qdingnet/qdaccess/QDDeviceTestNotify;)V");
    if (on_device_notify_method_id) {
        //回调参数值
        jstring data_str = transformCStringToJString(data.length(), data.c_str());
        //回调参数封装类
        jclass device_notify_class = jniEnv->FindClass("com/qdingnet/qdaccess/QDDeviceTestNotify");
        jmethodID device_notify_constructor = jniEnv->GetMethodID(device_notify_class, "<init>","(ILjava/lang/String;)V");
        jobject device_notify = jniEnv->NewObject(device_notify_class, device_notify_constructor, (jint)type, data_str);
        //回调接口
        jniEnv->CallStaticVoidMethod(callBackClass, on_device_notify_method_id, device_notify);
        //释放资源
        jniEnv->DeleteLocalRef(callBackClass);
        jniEnv->DeleteLocalRef(device_notify_class);
        jniEnv->DeleteLocalRef(device_notify);
        jniEnv->DeleteLocalRef(data_str);
        return 0;
    }
    else
    {
        return -1;
    }
}

int CJavaMethodManager::get_gate_magnetism_state_ack(int state)
{
    LOGI("%s...", __FUNCTION__);
    int result = -1;
    if (jniEnv) {
        jclass callBackClass = jniEnv->FindClass(QD_FACTORY_TEST_HANDLER_CLASS_PATH_NAME);
        jmethodID on_get_gate_magnetism_state_method_id = jniEnv->GetStaticMethodID(callBackClass,
                                                                                    "onGetGateMagnetismStateAck",
                                                                                    "(I)V");
        if (on_get_gate_magnetism_state_method_id) {
            jniEnv->CallStaticVoidMethod(callBackClass, on_get_gate_magnetism_state_method_id, state);
            result =  0;
        }
        //释放本地引用
        jniEnv->DeleteLocalRef(callBackClass);
    }
    return result;
}

////////////////////////////////////////千丁蓝牙门禁设备功能/////////////////////////////////////////////////
/**
 * 开门请求回复
 * @param result
 * @param user_id
 * @param keep_time
 * @param data
 * @return
 */
int CJavaMethodManager::open_door_reply(int operation, bool result, uint32_t user_id, uint32_t keep_time, uint8_t* data, uint8_t size){
    LOGI("%s...", __FUNCTION__);
    if(jniEnv){
        jclass callBackClass = jniEnv->FindClass(QD_DEVICE_SDK_HANDLER_CLASS_PATH_NAME);
        if(callBackClass){
            jmethodID on_open_door_reply_method_id = jniEnv->GetStaticMethodID(callBackClass,
                                                                               "onOpenDoorReply",
                                                                               "(IZJJ[B)V");
            if (on_open_door_reply_method_id) {
                //回调接口
                jbyteArray replayData = jniEnv->NewByteArray(size);
                if(size > 0){
                    jniEnv->SetByteArrayRegion(replayData, 0, size, (jbyte *)data);
                }
                jniEnv->CallStaticVoidMethod(callBackClass,
                                             on_open_door_reply_method_id,
                                             operation,
                                             (jboolean)result,
                                             (jlong)user_id,
                                             (jlong)keep_time,
                                             replayData);
                jniEnv->DeleteLocalRef(replayData);
                return 0;
            }
            jniEnv->DeleteLocalRef(callBackClass);
        }
    }
    return -1;
}