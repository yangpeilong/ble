#include <jni.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "common_struct.h"
#include "qd_access_protocol_define.h"
#include "qd_access_protocol_helper.h"
#include "qd_access_protocol_v2_define.h"
#include "qd_access_protocol_v2_helper.h"
#include "qd_access_protocol_v2_standalone_card.h"
#include "qd_common_utils.h"
#include "debugLog.h"
#include "meshsub.h"
#include <time.h>
#include "../secure/config.h"

/////////////////////////////////// V1.0 function ///////////////////////////////////////////////////

//jstring to char*
char *jstring_to_string(JNIEnv *env, jstring jstr) {
    char *rtn = NULL;
    if(jstr){
        jclass clsstring = env->FindClass("java/lang/String");
        jstring strencode = env->NewStringUTF("utf-8");
        jmethodID mid = env->GetMethodID(clsstring, "getBytes", "(Ljava/lang/String;)[B");
        jbyteArray barr = (jbyteArray) env->CallObjectMethod(jstr, mid, strencode);
        jsize alen = env->GetArrayLength(barr);
        jbyte *ba = env->GetByteArrayElements(barr, JNI_FALSE);
        if (alen > 0) {
            rtn = (char *) malloc(alen + 1);
            memcpy(rtn, ba, alen);
            rtn[alen] = 0;
        }
        env->ReleaseByteArrayElements(barr, ba, 0);
    }
    return rtn;
}

jbyteArray JNI_build_open_door_msg(JNIEnv *env,
                                   jobject obj,
                                   jint reported_record_index,
                                   jint cacahed_record_index,
                                   jstring mac_list,
                                   jint keep_seconds,
                                   jstring app_user_id,
                                   jstring room_id,
                                   jstring server_id)
{
    //授权校验
    if(!isAuthorized)
    {
        return env->NewByteArray(0);
    }
    LOGI("JNI_build_open_door_msg");
    char *p_mac_list = jstring_to_string(env, mac_list);
    char *p_app_user_id = jstring_to_string(env, app_user_id);
    char *p_room_id = jstring_to_string(env, room_id);
    char *p_sever_id = jstring_to_string(env, server_id);

    t_buffer msg = build_open_door_msg((uint32_t) reported_record_index, cacahed_record_index,
                                       p_mac_list, keep_seconds,
                                       p_app_user_id, p_room_id, p_sever_id, OS_ANDROID);

    LOGI("get here %s\n", __func__);

    if (p_mac_list) {
        free(p_mac_list);
    }

    if (p_app_user_id) {
        free(p_app_user_id);
    }
    if (p_room_id) {
        free(p_room_id);
    }

    if (p_sever_id) {
        free(p_sever_id);
    }

    if (msg.p_buffer != NULL) {
        jbyteArray packet = env->NewByteArray(msg.len);
        env->SetByteArrayRegion(packet, 0, msg.len, (jbyte *) msg.p_buffer);
        free(msg.p_buffer);
        return packet;
    } else {
        LOGI("%s %d msg buffer is NULL\n", __func__, __LINE__);
        jbyteArray packet = env->NewByteArray(0);
        return packet;
    }
}

jbyteArray JNI_build_open_door_msg_with_locations(JNIEnv *env,
                                                  jobject obj,
                                                  jint reported_record_index,
                                                  jint cacahed_record_index,
                                                  jstring mac_list,
                                                  jint keep_seconds,
                                                  jstring app_user_id,
                                                  jstring room_id,
                                                  jstring server_id,
                                                  jstring locations)
{
    //授权校验
    if(!isAuthorized)
    {
        return env->NewByteArray(0);
    }
    LOGI("JNI_build_open_door_msg");
    char *p_mac_list = jstring_to_string(env, mac_list);
    char *p_app_user_id = jstring_to_string(env, app_user_id);
    char *p_room_id = jstring_to_string(env, room_id);
    char *p_sever_id = jstring_to_string(env, server_id);
    char *p_location = jstring_to_string(env, locations);

    t_buffer msg = build_open_door_msg_with_locations((uint32_t) reported_record_index,
                                                      cacahed_record_index,
                                                      p_mac_list, keep_seconds, p_app_user_id,
                                                      p_room_id, p_sever_id, OS_ANDROID,
                                                      p_location);

    LOGI("get here %s\n", __func__);

    if (p_mac_list) {
        free(p_mac_list);
    }

    if (p_app_user_id) {
        free(p_app_user_id);
    }
    if (p_room_id) {
        free(p_room_id);
    }

    if (p_sever_id) {
        free(p_sever_id);
    }

    if (msg.p_buffer != NULL) {
        jbyteArray packet = env->NewByteArray(msg.len);
        env->SetByteArrayRegion(packet, 0, msg.len, (jbyte *) msg.p_buffer);
        free(msg.p_buffer);
        return packet;
    } else {
        LOGI("%s %d msg buffer is NULL\n", __func__, __LINE__);
        jbyteArray packet = env->NewByteArray(0);
        return packet;
    }
}

jbyteArray JNI_build_config_device_msg(JNIEnv *env,
                                       jobject obj,
                                       jint keep_time,
                                       jstring device_list,
                                       jint max_doors_size,
                                       jint max_counter_size)
{
    //授权校验
    if(!isAuthorized)
    {
        return env->NewByteArray(0);
    }
    LOGI("JNI_build_config_device_msg");
    char *p_dev_list = jstring_to_string(env, device_list);
//	LOGE("%s\n", p_dev_list);
    t_buffer msg = build_config_device_msg(p_dev_list, keep_time, max_doors_size, max_counter_size);

    if (p_dev_list) {
        free(p_dev_list);
    }

    if (msg.p_buffer != NULL) {
        jbyteArray packet = env->NewByteArray(msg.len);
        env->SetByteArrayRegion(packet, 0, msg.len, (jbyte *) msg.p_buffer);
        free(msg.p_buffer);
        return packet;
    } else {
        LOGI("%s %d msg buffer is NULL\n", __func__, __LINE__);
        jbyteArray packet = env->NewByteArray(0);
        return packet;
    }
}

jbyteArray JNI_build_update_device_msg(JNIEnv *env,
                                       jobject obj,
                                       jstring package_download_path)
{
    //授权校验
    if(!isAuthorized)
    {
        return env->NewByteArray(0);
    }
    LOGD("%s", __FUNCTION__);
    char *p_package_download_path = jstring_to_string(env, package_download_path);
    t_buffer msg = build_update_device_msg(0, p_package_download_path);

    if (p_package_download_path) {
        free(p_package_download_path);
    }
    if (msg.p_buffer != NULL) {
        jbyteArray packet = env->NewByteArray(msg.len);
        env->SetByteArrayRegion(packet, 0, msg.len, (jbyte *) msg.p_buffer);
        free(msg.p_buffer);
        return packet;
    } else {
        LOGI("%s %d msg buffer is NULL\n", __func__, __LINE__);
        jbyteArray packet = env->NewByteArray(0);
        return packet;
    }
}

jbyteArray JNI_build_get_device_version_msg(JNIEnv *env,
                                            jobject obj,
                                            jstring target_mac)
{
    //授权校验
    if(!isAuthorized)
    {
        return env->NewByteArray(0);
    }
    LOGI("JNI_build_get_device_version_msg");
    char *p_target_mac = jstring_to_string(env, target_mac);
    t_buffer msg = build_get_device_version_msg(p_target_mac);

    if (p_target_mac) {
        free(p_target_mac);
    }
    if (msg.p_buffer != NULL) {
        jbyteArray packet = env->NewByteArray(msg.len);
        env->SetByteArrayRegion(packet, 0, msg.len, (jbyte *) msg.p_buffer);
        free(msg.p_buffer);
        return packet;
    } else {
        LOGI("%s %d msg buffer is NULL\n", __func__, __LINE__);
        jbyteArray packet = env->NewByteArray(0);
        return packet;
    }
}

jbyteArray JNI_build_get_device_config_msg(JNIEnv *env,
                                           jobject obj,
                                           jstring target_mac)
{
    //授权校验
    if(!isAuthorized)
    {
        return env->NewByteArray(0);
    }
    LOGI("JNI_build_get_device_config_msg");
    char *p_target_mac = jstring_to_string(env, target_mac);
    t_buffer msg = build_get_device_config_msg(p_target_mac);

    if (p_target_mac) {
        free(p_target_mac);
    }
    if (msg.p_buffer != NULL) {
        jbyteArray packet = env->NewByteArray(msg.len);
        env->SetByteArrayRegion(packet, 0, msg.len, (jbyte *) msg.p_buffer);
        free(msg.p_buffer);
        return packet;
    } else {
        LOGI("%s %d msg buffer is NULL\n", __func__, __LINE__);
        jbyteArray packet = env->NewByteArray(0);
        return packet;
    }

}

jint JNI_analyse_opendoor_ack_packet(JNIEnv *env, jobject obj, jbyteArray packet,
                                     jobjectArray record_array) {
    jboolean is_copy = 1;
    uint8_t *p_data = (uint8_t *) env->GetByteArrayElements(packet, &is_copy);
    int data_len = env->GetArrayLength(packet);

    if ((uint32_t) data_len < (sizeof(t_qd_gatectrl_msg) + sizeof(t_qd_gatectrl_contentbody))) {
        LOGE(" data len = %d, is too small\n", data_len);
        return -1;
    }

    t_qd_gatectrl_msg *p_qd_gatectrl_msg = (t_qd_gatectrl_msg *) p_data;

    if (p_qd_gatectrl_msg->frame_head == QD_MSG_FRAMEHEAD) {

        t_qd_gatectrl_contentbody *p_qd_opendoor_ack_content =
                (t_qd_gatectrl_contentbody *) &p_qd_gatectrl_msg->content_start;

        t_qd_opendoor_ack_data *p_qd_opendoor_ack_data =
                (t_qd_opendoor_ack_data *) &(p_qd_opendoor_ack_content->data_start);

        int record_item_len =
                data_len - (sizeof(t_qd_gatectrl_msg) - 1) - (sizeof(t_qd_gatectrl_contentbody) - 1)
                - sizeof(t_qd_opendoor_ack_data);

        if (record_item_len >= sizeof(t_qd_passby_record_item)) {

            jclass record_entity_class = env->FindClass("com/qdingnet/qdaccess/QDPassRecordEntity");

            jmethodID constrocMID =
                    env->GetMethodID(record_entity_class, "<init>",
                                     "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)V");

            jclass list_cls = env->FindClass("java/util/ArrayList"); //获得ArrayList类引用
            jmethodID list_add = env->GetMethodID(list_cls, "add", "(Ljava/lang/Object;)Z");

            //预加上一个item的长度，防止数据不够的情况
            for (int i = 0; (i + sizeof(t_qd_passby_record_item)) <= record_item_len; i +=
                                                                                              sizeof(t_qd_passby_record_item)) {
                t_qd_passby_record_item *p_record_item =
                        (t_qd_passby_record_item *) (&(p_qd_opendoor_ack_data->record_item_start) +
                                                     i);

                char tmp_string[256];

                sprintf(tmp_string, "%u", p_record_item->record_index);
                jstring record_index = env->NewStringUTF((const char *) tmp_string);

                jstring app_user_id = env->NewStringUTF((const char *) p_record_item->app_user_id);

                jstring room_id = env->NewStringUTF((const char *) p_record_item->room_id);

                jstring server_id = env->NewStringUTF((const char *) p_record_item->server_id);

                jstring dev_mac = env->NewStringUTF((const char *) p_record_item->mac_addr);

                sprintf(tmp_string, "%u", p_record_item->create_time);
                jstring create_time = env->NewStringUTF((const char *) tmp_string);

                sprintf(tmp_string, "%d", p_record_item->passby_method);
                jstring passby_method = env->NewStringUTF((const char *) tmp_string);

                jobject record_obj = env->NewObject(record_entity_class, constrocMID, record_index,
                                                    app_user_id,
                                                    room_id, server_id, dev_mac, create_time,
                                                    passby_method);

                env->CallBooleanMethod(record_array, list_add, record_obj);

            }

        }

        return p_qd_opendoor_ack_data->open_result;
    } else {
        return -1;
    }
}

jint JNI_analyse_config_ack_packet(JNIEnv *env, jobject obj, jbyteArray packet) {
    jboolean is_copy = 1;
    uint8_t *p_data = (uint8_t *) env->GetByteArrayElements(packet, &is_copy);
    int data_len = env->GetArrayLength(packet);

    if ((uint32_t) data_len < (sizeof(t_qd_gatectrl_msg) - 1 + sizeof(t_qd_gatectrl_contentbody))) {
        LOGE(" data len = %d, is too small\n", data_len);
        return -1;
    }

    t_qd_gatectrl_msg *p_qd_gatectrl_msg = (t_qd_gatectrl_msg *) p_data;

    if (p_qd_gatectrl_msg->frame_head == QD_MSG_FRAMEHEAD) {

        t_qd_gatectrl_contentbody *p_qd_config_ack_content =
                (t_qd_gatectrl_contentbody *) &p_qd_gatectrl_msg->content_start;

        t_qd_config_device_ack_data *p_qd_config_ack_data =
                (t_qd_config_device_ack_data *) &(p_qd_config_ack_content->data_start);

        return p_qd_config_ack_data->config_result;
    } else {
        return -1;
    }
}

jint JNI_analyse_update_ack_packet(JNIEnv *env, jobject obj, jbyteArray packet) {
    jboolean is_copy = 1;
    uint8_t *p_data = (uint8_t *) env->GetByteArrayElements(packet, &is_copy);
    int data_len = env->GetArrayLength(packet);

    if ((uint32_t) data_len < (sizeof(t_qd_gatectrl_msg) - 1 + sizeof(t_qd_gatectrl_contentbody))) {
        LOGE(" data len = %d, is too small\n", data_len);
        return -1;
    }

    t_qd_gatectrl_msg *p_qd_gatectrl_msg = (t_qd_gatectrl_msg *) p_data;

    if (p_qd_gatectrl_msg->frame_head == QD_MSG_FRAMEHEAD) {

        t_qd_gatectrl_contentbody *p_qd_config_ack_content =
                (t_qd_gatectrl_contentbody *) &p_qd_gatectrl_msg->content_start;

        t_qd_update_device_ack_data *p_qd_update_ack_data =
                (t_qd_update_device_ack_data *) &(p_qd_config_ack_content->data_start);

        return p_qd_update_ack_data->update_result;
    } else {
        return -1;
    }
}

jobjectArray JNI_analyse_get_device_version_ack_packet(JNIEnv *env, jobject obj,
                                                       jbyteArray packet) {
    jboolean is_copy = 1;

    uint8_t *p_data = (uint8_t *) env->GetByteArrayElements(packet, &is_copy);
    int data_len = env->GetArrayLength(packet);
    jobjectArray versions = env->NewObjectArray(2, (env)->FindClass("java/lang/String"), 0);
    t_qd_gatectrl_msg *p_qd_gatectrl_msg = (t_qd_gatectrl_msg *) p_data;
    LOGI("JNI_analyse_get_device_version_ack_packet data len =%d", data_len);
    if (p_qd_gatectrl_msg->frame_head == QD_MSG_FRAMEHEAD) {

        t_qd_gatectrl_contentbody *p_qd_get_device_version_content =
                (t_qd_gatectrl_contentbody *) &p_qd_gatectrl_msg->content_start;

        t_qd_get_device_version_ack_data *p_qd_get_device_version_ack_data =
                (t_qd_get_device_version_ack_data *) &(p_qd_get_device_version_content->data_start);

        jstring software_version = env->NewStringUTF(
                (const char *) p_qd_get_device_version_ack_data->software_version);
        env->SetObjectArrayElement(versions, 0, software_version);
        LOGI("%s %s", p_qd_get_device_version_ack_data->software_version,
             p_qd_get_device_version_ack_data->hardware_version);
        jstring hardware_version = env->NewStringUTF(
                (const char *) p_qd_get_device_version_ack_data->hardware_version);
        env->SetObjectArrayElement(versions, 1, hardware_version);

    } else {
        jstring software_version = env->NewStringUTF("UNKNOWN");
        env->SetObjectArrayElement(versions, 0, software_version);
        jstring hardware_version = env->NewStringUTF("UNKNOWN");
        env->SetObjectArrayElement(versions, 1, hardware_version);
    }

    return versions;
}

jint JNI_analyse_get_device_config_ack_packet(JNIEnv *env, jobject obj, jbyteArray packet,
                                              jobject related_doors_entity) {
    jboolean is_copy = 1;

    uint8_t *p_data = (uint8_t *) env->GetByteArrayElements(packet, &is_copy);
    int data_len = env->GetArrayLength(packet);
    jobjectArray versions = env->NewObjectArray(2, (env)->FindClass("java/lang/String"), 0);
    t_qd_gatectrl_msg *p_qd_gatectrl_msg = (t_qd_gatectrl_msg *) p_data;

    if (p_qd_gatectrl_msg->frame_head == QD_MSG_FRAMEHEAD) {

        t_qd_gatectrl_contentbody *p_qd_get_device_config_content =
                (t_qd_gatectrl_contentbody *) &p_qd_gatectrl_msg->content_start;

        t_qd_get_device_config_ack_data *p_get_device_config_ack_data =
                (t_qd_get_device_config_ack_data *) &(p_qd_get_device_config_content->data_start);

        jclass related_doors_cls = env->GetObjectClass(related_doors_entity);
        jfieldID open_keep_time = env->GetFieldID(related_doors_cls, "open_keep_time", "I");
        jfieldID max_related_doors_size = env->GetFieldID(related_doors_cls,
                                                          "max_related_doors_size", "I");
        jfieldID max_single_uinit_counter_size = env->GetFieldID(related_doors_cls,
                                                                 "max_single_uinit_counter_size",
                                                                 "I");
        jfieldID device_time = env->GetFieldID(related_doors_cls, "device_time", "J");
        jfieldID related_doors = env->GetFieldID(related_doors_cls, "related_doors",
                                                 "Ljava/lang/String;");
        jfieldID software_version = env->GetFieldID(related_doors_cls, "software_version",
                                                    "Ljava/lang/String;");
        jfieldID hardware_version = env->GetFieldID(related_doors_cls, "hardware_version",
                                                    "Ljava/lang/String;");

        env->SetIntField(related_doors_entity, open_keep_time,
                         p_get_device_config_ack_data->open_keep_time);
        env->SetIntField(related_doors_entity, max_related_doors_size,
                         p_get_device_config_ack_data->max_doors_size);
        env->SetIntField(related_doors_entity, max_single_uinit_counter_size,
                         p_get_device_config_ack_data->max_counter_size);
        env->SetLongField(related_doors_entity, device_time,
                          p_get_device_config_ack_data->device_time);
        jstring tmp_doors = env->NewStringUTF(
                (const char *) &(p_get_device_config_ack_data->related_doors_start));
        env->SetObjectField(related_doors_entity, related_doors, tmp_doors);

        jstring tmp_software_version = env->NewStringUTF(
                (const char *) p_get_device_config_ack_data->software_version);
        env->SetObjectField(related_doors_entity, software_version, tmp_software_version);

        jstring tmp_hardware_version = env->NewStringUTF(
                (const char *) p_get_device_config_ack_data->hardware_version);
        env->SetObjectField(related_doors_entity, hardware_version, tmp_hardware_version);

        return 0;
    } else {
        return -1;
    }

}

jint JNI_genarate_key(JNIEnv *env,
                      jobject obj,
                      jstring commnity,
                      jstring bulid_uinit,
                      jint max_doors_size,
                      jint max_counter_size)
{
    //授权校验
    if(!isAuthorized)
    {
        return 0;
    }
    LOGD("%s", __FUNCTION__);
    char *p_commnity = jstring_to_string(env, commnity);
    char *p_bulid_uinit = jstring_to_string(env, bulid_uinit);

    time_t current_time = time(NULL);
    struct tm *p_tm;
    p_tm = localtime(&current_time); /*获取本地时区时间*/
    char tmp_day[10];
    sprintf(tmp_day, "%4d%02d%02d", p_tm->tm_year + 1900, p_tm->tm_mon + 1, p_tm->tm_mday);
    mesh_preset(max_doors_size, max_counter_size, max_counter_size, 0, 2);
    uint32_t passwrod = mesh_generate((uint8_t *) p_commnity, (uint8_t *) p_bulid_uinit,
                                      (uint8_t *) tmp_day, 0, 0, 1,
                                      0x12345678);
    return passwrod;

}

/////////////////////////////////// V2.0 function ///////////////////////////////////////////////////

int decrypt_data(JNIEnv *env,
                 jobject obj,
                 jstring target_mac,
                 uint8_t *wbinfo,
                 uint16_t wblen,
                 uint16_t enc)
{
    char *p_target_mac = jstring_to_string(env, target_mac);
    int target_mac_len = strlen(p_target_mac);
    int suc = ble_decrypt((uint8_t *) p_target_mac, target_mac_len, wbinfo, wblen, enc);
    LOGI("decrypt_data...suc:%d, mac:%s data_len:%d decrpty_key:%04X", suc, p_target_mac, wblen,
         enc);
    if (p_target_mac) {
        free(p_target_mac);
    }
    return suc;
}

jbyteArray JNI_build_open_door_msg_v2(JNIEnv *env,
                                      jobject obj,
                                      jstring target_mac,
                                      jbyte keep_time,
                                      jlong app_user_id,
                                      jstring auth_code)
{
    //授权校验
    if(!isAuthorized)
    {
        return env->NewByteArray(0);
    }
    LOGI("JNI_build_open_door_msg_v2...");
    char *p_target_mac = jstring_to_string(env, target_mac);
    char *p_auth_code = jstring_to_string(env, auth_code);

    const char *t_p_auth_code = p_auth_code && strlen(p_auth_code) > 0 ? p_auth_code : AUTH_CODE;
    LOGD("%s...auth_code:%s", __FUNCTION__, t_p_auth_code ? t_p_auth_code : "");
    t_buffer msg = build_open_door_msg_v2(p_target_mac, keep_time, app_user_id, OS_ANDROID, t_p_auth_code);

    LOGI("get here %s\n", __func__);

    if (p_target_mac) {
        free(p_target_mac);
    }
    if (p_auth_code) {
        free(p_auth_code);
    }

    if (msg.p_buffer != NULL) {
        jbyteArray packet = env->NewByteArray(msg.len);
        env->SetByteArrayRegion(packet, 0, msg.len, (jbyte *) msg.p_buffer);
        free(msg.p_buffer);
        return packet;
    } else {
        LOGI("%s %d msg buffer is NULL\n", __func__, __LINE__);
        jbyteArray packet = env->NewByteArray(0);
        return packet;
    }
}

jbyteArray JNI_build_open_door_msg_with_locations_v2(JNIEnv *env,
                                                     jobject obj,
                                                     jstring target_mac,
                                                     jbyte keep_time,
                                                     jlong app_user_id,
                                                     jstring auth_code,
                                                     jobjectArray related_units)
{
    //授权校验
    if(!isAuthorized)
    {
        return env->NewByteArray(0);
    }
    LOGI("JNI_build_open_door_msg_with_locations_v2...");
    char *p_target_mac = jstring_to_string(env, target_mac);
    char *p_auth_code = jstring_to_string(env, auth_code);

    jclass qd_unit_item_class = env->FindClass("com/qdingnet/qdaccess/QDUnitItem");
    jfieldID unit_num_id = env->GetFieldID(qd_unit_item_class, "unit_id", "J");
    jfieldID multi_time_counter_id = env->GetFieldID(qd_unit_item_class, "multi_time_counter", "I");
    jfieldID single_time_counter_id = env->GetFieldID(qd_unit_item_class, "single_time_counter",
                                                      "I");

    jsize related_units_size = env->GetArrayLength(related_units);
    t_qd_unit_item tmp_unit_items[related_units_size];

    for (int i = 0; i < related_units_size; i++) {
        jobject unit_item_obj = env->GetObjectArrayElement(related_units, i);
        tmp_unit_items[i].unit_num = env->GetLongField(unit_item_obj, unit_num_id);
        tmp_unit_items[i].multi_time_counter = env->GetIntField(unit_item_obj,
                                                                multi_time_counter_id);
        tmp_unit_items[i].single_time_counter = env->GetIntField(unit_item_obj,
                                                                 single_time_counter_id);
    }
    const char *t_p_auth_code = p_auth_code && strlen(p_auth_code) > 0 ? p_auth_code : AUTH_CODE;
    LOGD("%s...auth_code:%s", __FUNCTION__, t_p_auth_code ? t_p_auth_code : "");

    t_buffer msg = build_open_door_msg_with_locations_v2(p_target_mac,
                                                         keep_time,
                                                         app_user_id,
                                                         (uint8_t) OS_ANDROID,
                                                         t_p_auth_code,
                                                         (uint8_t *) tmp_unit_items,
                                                         related_units_size);

    LOGI("get here %s\n", __func__);

    if (p_target_mac) {
        free(p_target_mac);
    }
    if (p_auth_code) {
        free(p_auth_code);
    }

    if (msg.p_buffer != NULL) {
        jbyteArray packet = env->NewByteArray(msg.len);
        env->SetByteArrayRegion(packet, 0, msg.len, (jbyte *) msg.p_buffer);
        free(msg.p_buffer);
        return packet;
    } else {
        LOGI("%s %d msg buffer is NULL\n", __func__, __LINE__);
        jbyteArray packet = env->NewByteArray(0);
        return packet;
    }
}

jbyteArray JNI_build_open_door_msg_with_cards_v2(JNIEnv *env,
                                                 jobject obj,
                                                 jstring target_mac,
                                                 jbyte keep_time,
                                                 jlong app_user_id,
                                                 jstring auth_code,
                                                 jobjectArray cards)
{
    //授权校验
    if(!isAuthorized)
    {
        return env->NewByteArray(0);
    }
    LOGI("JNI_build_open_door_msg_with_cards_v2...");
    char *p_target_mac = jstring_to_string(env, target_mac);
    char *p_auth_code = jstring_to_string(env, auth_code);

    jsize cards_size = env->GetArrayLength(cards);
    t_qd_card_item *local_cards = (t_qd_card_item *) malloc(sizeof(t_qd_card_item) * cards_size);
    if (cards_size > 0 && local_cards == NULL) {
        LOGI("%s %d malloc cards buffer is NULL\n", __func__, __LINE__);
        jbyteArray packet = env->NewByteArray(0);
        return packet;
    }

    jclass cardItemClass = env->FindClass("com/qdingnet/qdaccess/QDRFCardItem");
    jfieldID cardNoField = env->GetFieldID(cardItemClass, "card_no", "J");
    jfieldID cardTimestamp = env->GetFieldID(cardItemClass, "timestamp", "J");
    jfieldID cardOperateType = env->GetFieldID(cardItemClass, "operate_type", "B");
    for (jsize i = 0; i < cards_size; i++) {
        jobject item = env->GetObjectArrayElement(cards, i);
        local_cards[i].card_uid = (uint32_t) env->GetLongField(item, cardNoField);
        local_cards[i].time_stamp = (uint32_t) env->GetLongField(item, cardTimestamp);
        jbyte type = env->GetByteField(item, cardOperateType);
        if (type == 1) {
            local_cards[i].operate_type = CARD_NEED_DISABLE;
        } else if (type == 2) {
            local_cards[i].operate_type = CARD_NEED_ACTIVATE;
        }
        LOGI("local_cards...card_no:%u,%u,%u\n", local_cards[i].card_uid, local_cards[i].time_stamp,
             local_cards[i].operate_type);
    }

    const char *t_p_auth_code = p_auth_code && strlen(p_auth_code) > 0 ? p_auth_code : AUTH_CODE;
    LOGD("%s...auth_code:%s", __FUNCTION__, t_p_auth_code ? t_p_auth_code : "");

    t_buffer msg = build_open_door_msg_with_black_cards_v2(p_target_mac, (uint8_t) keep_time,
                                                           (uint32_t) app_user_id,
                                                           (uint8_t) OS_ANDROID,
                                                           t_p_auth_code,
                                                           (uint8_t *) local_cards, cards_size);

    if (p_target_mac) {
        free(p_target_mac);
    }
    if (p_auth_code) {
        free(p_auth_code);
    }
    if (local_cards) {
        free(local_cards);
    }

    if (msg.p_buffer != NULL) {
        jbyteArray packet = env->NewByteArray(msg.len);
        env->SetByteArrayRegion(packet, 0, msg.len, (jbyte *) msg.p_buffer);
        free(msg.p_buffer);
        return packet;
    } else {
        LOGI("%s %d msg buffer is NULL\n", __func__, __LINE__);
        jbyteArray packet = env->NewByteArray(0);
        return packet;
    }

}

jbyteArray JNI_build_open_door_msg_with_distribute_cards_info_v2(JNIEnv *env,
                                                                 jobject obj,
                                                                 jstring target_mac,
                                                                 jbyte keep_time,
                                                                 jlong app_user_id,
                                                                 jstring auth_code,
                                                                 jint wait_time_pre_card,
                                                                 jstring cards_info)
{
    //授权校验
    if(!isAuthorized)
    {
        return env->NewByteArray(0);
    }
    LOGI("JNI_build_open_door_msg_with_distribute_cards_info_v2...");
    char *p_target_mac = jstring_to_string(env, target_mac);
    char *p_auth_code = jstring_to_string(env, auth_code);
    char *p_cards_info = jstring_to_string(env, cards_info);

    const char *t_p_auth_code = p_auth_code && strlen(p_auth_code) > 0 ? p_auth_code : AUTH_CODE;
    LOGD("%s...auth_code:%s", __FUNCTION__, t_p_auth_code ? t_p_auth_code : "");

    t_buffer msg = build_open_door_msg_with_distribute_cards_info_v2(p_target_mac, keep_time,
                                                                     app_user_id, OS_ANDROID,
                                                                     t_p_auth_code,
                                                                     wait_time_pre_card,
                                                                     p_cards_info);
    if (p_target_mac) {
        free(p_target_mac);
    }
    if (p_auth_code) {
        free(p_auth_code);
    }
    if (p_cards_info) {
        free(p_cards_info);
    }

    if (msg.p_buffer != NULL) {
        jbyteArray packet = env->NewByteArray(msg.len);
        env->SetByteArrayRegion(packet, 0, msg.len, (jbyte *) msg.p_buffer);
        free(msg.p_buffer);
        return packet;
    } else {
        LOGI("%s %d msg buffer is NULL\n", __func__, __LINE__);
        jbyteArray packet = env->NewByteArray(0);
        return packet;
    }
}

jint JNI_analyse_opendoor_ack_packet_v2(JNIEnv *env, jobject obj, jstring target_mac,
                                        jbyteArray packet, jobjectArray record_array) {

    jboolean is_copy = 1;
    uint8_t *p_data = (uint8_t *) env->GetByteArrayElements(packet, &is_copy);
    int data_len = env->GetArrayLength(packet);

    if ((uint32_t) data_len < (sizeof(t_qd_access_msg) - 1 + sizeof(t_qd_access_content))) {
        LOGE(" data len = %d, is too small\n", data_len);
        return -1;
    }

    int result = -1;
    t_qd_access_msg *p_qd_access_msg = (t_qd_access_msg *) p_data;

    if (p_qd_access_msg->frame_head == QD_MSG_FRAMEHEAD) {

        int suc = decrypt_data(env, obj, target_mac, p_qd_access_msg->content_start,
                               p_qd_access_msg->total_length - sizeof(t_qd_access_msg),
                               p_qd_access_msg->decrypt_key);

        if (suc == 1) {
            t_qd_access_content *p_qd_opendoor_ack_content = (t_qd_access_content *) p_qd_access_msg->content_start;
            LOGI("command_type == %d ", p_qd_opendoor_ack_content->command_type);
            switch (p_qd_opendoor_ack_content->command_type) {
                case QD_CMD_OPEN_DOOR:
                    LOGI("QD_CMD_OPEN_DOOR");
                    break;
                case QD_CMD_DISTRIBUTE_CARD:
                    LOGI("QD_CMD_DISTRIBUTE_CARD");
                    break;
                case QD_CMD_READ_CARD_UID:
                    LOGI("QD_CMD_READ_CARD_UID");
                default:
                    break;
            }
            int record_item_len = data_len - sizeof(t_qd_access_msg) - sizeof(t_qd_access_content);

            if (record_item_len >= sizeof(t_qd_passby_record_item_v2)) {

                jclass record_entity_class = env->FindClass(
                        "com/qdingnet/qdaccess/QDPassRecordEntity");

                jmethodID constrocMID =
                        env->GetMethodID(record_entity_class, "<init>",
                                         "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)V");

                jclass list_cls = env->FindClass("java/util/ArrayList"); //获得ArrayList类引用
                jmethodID list_add = env->GetMethodID(list_cls, "add", "(Ljava/lang/Object;)Z");

                //预加上一个item的长度，防止数据不够的情况
                for (int i = 0; (i + sizeof(t_qd_passby_record_item_v2)) <= record_item_len; i +=
                                                                                                     sizeof(t_qd_passby_record_item_v2)) {
                    t_qd_passby_record_item_v2 *p_record_item =
                            (t_qd_passby_record_item_v2 *) (p_qd_opendoor_ack_content->data_start +
                                                            i);

                    char tmp_string[20];
                    sprintf(tmp_string, "%u", p_record_item->app_user_id);
                    jstring app_user_id = env->NewStringUTF((const char *) tmp_string);

                    uint32_t passby_method_create_time = p_record_item->passby_method_create_time;
                    sprintf(tmp_string, "%u", passby_method_create_time >> 28);
                    jstring passby_method = env->NewStringUTF((const char *) tmp_string);

                    time_t current_time = time(NULL);
                    long long_create_time = current_time - (current_time & 0x0fffffff) +
                                            (passby_method_create_time & 0x0fffffff);
                    sprintf(tmp_string, "%ld", long_create_time);
                    jstring create_time = env->NewStringUTF((const char *) tmp_string);

                    jobject record_obj = env->NewObject(record_entity_class, constrocMID, NULL,
                                                        app_user_id,
                                                        NULL, NULL, NULL, create_time,
                                                        passby_method);

                    env->CallBooleanMethod(record_array, list_add, record_obj);
                }
            }
            result = p_qd_opendoor_ack_content->ack_type;
        } else {
            LOGI("JNI_analyse_opendoor_ack_packet_v2...decrypt...fail");
        }
    } else {
        LOGI("JNI_analyse_opendoor_ack_packet_v2...frame_head is wrong");
    }
    return result;
}

jbyteArray JNI_build_get_device_version_msg_v2(JNIEnv *env,
                                               jobject obj,
                                               jstring target_mac)
{
    //授权校验
    if(!isAuthorized)
    {
        return env->NewByteArray(0);
    }
    LOGI("JNI_build_get_device_version_msg_v2...");
    char *p_target_mac = jstring_to_string(env, target_mac);
    t_buffer msg = build_get_device_version_msg_v2(p_target_mac);

    if (p_target_mac) {
        free(p_target_mac);
    }
    if (msg.p_buffer != NULL) {
        jbyteArray packet = env->NewByteArray(msg.len);
        env->SetByteArrayRegion(packet, 0, msg.len, (jbyte *) msg.p_buffer);
        free(msg.p_buffer);
        return packet;
    } else {
        LOGI("%s %d msg buffer is NULL\n", __func__, __LINE__);
        jbyteArray packet = env->NewByteArray(0);
        return packet;
    }
}

jobjectArray JNI_analyse_get_device_version_ack_packet_v2(JNIEnv *env, jobject obj,
                                                          jstring target_mac, jbyteArray packet) {

    jboolean is_copy = 1;
    uint8_t *p_data = (uint8_t *) env->GetByteArrayElements(packet, &is_copy);
    int data_len = env->GetArrayLength(packet);

    jobjectArray versions = env->NewObjectArray(2, (env)->FindClass("java/lang/String"), 0);
    t_qd_access_msg *p_qd_access_msg = (t_qd_access_msg *) p_data;

    LOGI("JNI_analyse_get_device_version_ack_packet_v2 data len =%d", data_len);


    jstring software_version = env->NewStringUTF("UNKNOWN");
    env->SetObjectArrayElement(versions, 0, software_version);
    jstring hardware_version = env->NewStringUTF("UNKNOWN");
    env->SetObjectArrayElement(versions, 1, hardware_version);
    if (p_qd_access_msg->frame_head == QD_MSG_FRAMEHEAD) {

        int suc = decrypt_data(env, obj, target_mac, p_qd_access_msg->content_start,
                               p_qd_access_msg->total_length - sizeof(t_qd_access_msg),
                               p_qd_access_msg->decrypt_key);

        if (suc == 1) {
            LOGI("JNI_analyse_get_device_version_ack_packet_v2...decrypt success");
            t_qd_access_content *p_qd_get_device_version_content =
                    (t_qd_access_content *) p_qd_access_msg->content_start;

            t_qd_version_ack_data_v2 *p_qd_get_device_version_ack_data =
                    (t_qd_version_ack_data_v2 *) p_qd_get_device_version_content->data_start;

            jstring software_version = env->NewStringUTF(
                    (const char *) p_qd_get_device_version_ack_data->software_version);
            env->SetObjectArrayElement(versions, 0, software_version);
            LOGI("%s %s", p_qd_get_device_version_ack_data->software_version,
                 p_qd_get_device_version_ack_data->hardware_version);
            jstring hardware_version = env->NewStringUTF(
                    (const char *) p_qd_get_device_version_ack_data->hardware_version);
            env->SetObjectArrayElement(versions, 1, hardware_version);
        } else {
            LOGI("JNI_analyse_get_device_version_ack_packet_v2...decrypt fail");
        }
    } else {
        LOGI("JNI_analyse_get_device_version_ack_packet_v2...frame_head is wrong");
    }
    return versions;
}

jbyteArray JNI_build_get_device_config_msg_v2(JNIEnv *env,
                                              jobject obj,
                                              jstring target_mac)
{
    //授权校验
    if(!isAuthorized)
    {
        return env->NewByteArray(0);
    }
    LOGI("JNI_build_get_device_config_msg_v2...");
    char *p_target_mac = jstring_to_string(env, target_mac);
    t_buffer msg = build_get_device_config_msg_v2(p_target_mac);

    if (p_target_mac) {
        free(p_target_mac);
    }
    if (msg.p_buffer != NULL) {
        jbyteArray packet = env->NewByteArray(msg.len);
        env->SetByteArrayRegion(packet, 0, msg.len, (jbyte *) msg.p_buffer);
        free(msg.p_buffer);
        return packet;
    } else {
        LOGI("%s %d msg buffer is NULL\n", __func__, __LINE__);
        jbyteArray packet = env->NewByteArray(0);
        return packet;
    }
}

jint JNI_analyse_get_device_config_ack_packet_v2(JNIEnv *env, jobject obj, jstring target_mac,
                                                 jbyteArray packet, jobject related_doors_entity) {
    LOGI("JNI_analyse_get_device_config_ack_packet_v2...");
    jboolean is_copy = 1;
    uint8_t *p_data = (uint8_t *) env->GetByteArrayElements(packet, &is_copy);
    int data_len = env->GetArrayLength(packet);
    jobjectArray versions = env->NewObjectArray(2, (env)->FindClass("java/lang/String"), 0);
    t_qd_access_msg *p_qd_access_msg = (t_qd_access_msg *) p_data;

    int result = -1;
    if (p_qd_access_msg->frame_head == QD_MSG_FRAMEHEAD) {
        int suc = decrypt_data(env, obj, target_mac, p_qd_access_msg->content_start,
                               p_qd_access_msg->total_length - sizeof(t_qd_access_msg),
                               p_qd_access_msg->decrypt_key);
        if (suc == 1) {
            t_qd_access_content *p_qd_get_device_config_content =
                    (t_qd_access_content *) p_qd_access_msg->content_start;

            t_qd_config_ack_data_v2 *p_t_qd_config_ack_data = (t_qd_config_ack_data_v2 *) p_qd_get_device_config_content->data_start;
            jclass related_doors_cls = env->GetObjectClass(related_doors_entity);
            jfieldID open_keep_time = env->GetFieldID(related_doors_cls, "open_keep_time", "I");
            jfieldID max_single_uinit_counter_size = env->GetFieldID(related_doors_cls,
                                                                     "max_single_uinit_counter_size",
                                                                     "I");
            jfieldID device_time = env->GetFieldID(related_doors_cls, "device_time", "J");
            jfieldID related_doors = env->GetFieldID(related_doors_cls, "related_doors",
                                                     "Ljava/lang/String;");
            jfieldID software_version = env->GetFieldID(related_doors_cls, "software_version",
                                                        "Ljava/lang/String;");
            jfieldID hardware_version = env->GetFieldID(related_doors_cls, "hardware_version",
                                                        "Ljava/lang/String;");
            jfieldID related_units = env->GetFieldID(related_doors_cls, "related_units",
                                                     "Ljava/util/ArrayList;");

            env->SetIntField(related_doors_entity, open_keep_time,
                             p_t_qd_config_ack_data->open_keep_time);
            env->SetLongField(related_doors_entity, device_time,
                              p_t_qd_config_ack_data->device_time);

            jstring tmp_software_version = env->NewStringUTF(
                    (const char *) p_t_qd_config_ack_data->software_version);
            env->SetObjectField(related_doors_entity, software_version, tmp_software_version);

            jstring tmp_hardware_version = env->NewStringUTF(
                    (const char *) p_t_qd_config_ack_data->hardware_version);
            env->SetObjectField(related_doors_entity, hardware_version, tmp_hardware_version);


            jsize record_item_len =
                    p_qd_get_device_config_content->content_length - sizeof(t_qd_access_content) -
                    sizeof(t_qd_config_ack_data_v2);
            LOGI("analyse_get_device_config_ack_packet_v2...related_doors_entity...record_item_len:%d",
                 record_item_len);

            if (record_item_len >= sizeof(t_qd_unit_item)) {
                jclass unit_item_class = env->FindClass("com/qdingnet/qdaccess/QDUnitItem");
                jmethodID contructor_unit_item = env->GetMethodID(unit_item_class, "<init>",
                                                                  "(JII)V");

                jclass list_cls = env->FindClass("java/util/ArrayList"); //获得ArrayList类引用
                jmethodID list_contructor = env->GetMethodID(list_cls, "<init>", "()V");
                jobject list_obj = env->NewObject(list_cls, list_contructor);
                jmethodID list_add = env->GetMethodID(list_cls, "add", "(Ljava/lang/Object;)Z");

                for (int i = 0;
                     (i + sizeof(t_qd_unit_item)) <= record_item_len; i += sizeof(t_qd_unit_item)) {
                    t_qd_unit_item *p_unit_item = (t_qd_unit_item *) (
                            p_t_qd_config_ack_data->related_doors_start + i);
                    jlong units_id = (p_unit_item->unit_num & 0x80000000) +
                                     (p_unit_item->unit_num & 0x7fffffff);
                    jobject record_obj = env->NewObject(unit_item_class, contructor_unit_item,
                                                        units_id, p_unit_item->multi_time_counter,
                                                        p_unit_item->single_time_counter);
                    LOGI("analyse_get_device_config_ack_packet_v2...unit_num:%u", units_id);
                    env->CallBooleanMethod(list_obj, list_add, record_obj);
                }
                //set related units value
                env->SetObjectField(related_doors_entity, related_units, list_obj);
            }
            result = 0;
            LOGI("JNI_analyse_get_device_config_ack_packet_v2...suc");
        } else {
            LOGI("JNI_analyse_get_device_config_ack_packet_v2...FAIL");
        }
    } else {
        LOGI("JNI_analyse_get_device_config_ack_packet_v2...frame_head is wrong");
    }
    return result;
}

jbyteArray JNI_build_update_device_msg_v2(JNIEnv *env,
                                          jobject obj,
                                          jstring target_mac,
                                          jstring package_download_path)
{
    //授权校验
    if(!isAuthorized)
    {
        return env->NewByteArray(0);
    }
    LOGI("JNI_build_update_device_msg_v2...");
    char *p_target_mac = jstring_to_string(env, target_mac);
    char *p_package_download_path = jstring_to_string(env, package_download_path);
    t_buffer msg = build_update_device_msg_v2(p_target_mac, (uint8_t) UPDATE_NORMAL,
                                              p_package_download_path);

    if (p_target_mac) {
        free(p_target_mac);
    }
    if (p_package_download_path) {
        free(p_package_download_path);
    }
    if (msg.p_buffer != NULL) {
        jbyteArray packet = env->NewByteArray(msg.len);
        env->SetByteArrayRegion(packet, 0, msg.len, (jbyte *) msg.p_buffer);
        free(msg.p_buffer);
        return packet;
    } else {
        LOGI("%s %d msg buffer is NULL\n", __func__, __LINE__);
        jbyteArray packet = env->NewByteArray(0);
        return packet;
    }
}

jint JNI_analyse_update_ack_packet_v2(JNIEnv *env, jobject obj, jstring target_mac,
                                      jbyteArray packet) {
    LOGI("JNI_analyse_update_ack_packet_v2...");
    jboolean is_copy = 1;
    uint8_t *p_data = (uint8_t *) env->GetByteArrayElements(packet, &is_copy);
    int data_len = env->GetArrayLength(packet);

    if ((uint32_t) data_len < (sizeof(t_qd_access_msg) - 1 + sizeof(t_qd_access_content))) {
        LOGE(" data len = %d, is too small\n", data_len);
        return -1;
    }

    t_qd_access_msg *p_qd_access_msg = (t_qd_access_msg *) p_data;

    if (p_qd_access_msg->frame_head == QD_MSG_FRAMEHEAD) {

        int suc = decrypt_data(env, obj, target_mac, p_qd_access_msg->content_start,
                               p_qd_access_msg->total_length - sizeof(t_qd_access_msg),
                               p_qd_access_msg->decrypt_key);
        if (suc == 1) {
            t_qd_access_content *p_qd_config_ack_content =
                    (t_qd_access_content *) p_qd_access_msg->content_start;
            return p_qd_config_ack_content->ack_type;
        } else {
            LOGI("JNI_analyse_update_ack_packet_v2...FAILED");
        }
    } else {
        LOGI("JNI_analyse_update_ack_packet_v2...frame_head is wrong");
    }
    return -1;
}

jbyteArray JNI_build_config_device_msg_v2(JNIEnv *env,
                                          jobject obj,
                                          jstring target_mac,
                                          jbyte keep_time,
                                          jobjectArray related_units,
                                          jbyte relay_state,
                                          jlong phone,
                                          jstring auth_code)
{
    //授权校验
    if(!isAuthorized)
    {
        return env->NewByteArray(0);
    }
    LOGI("JNI_build_config_device_msg_v2...");
    char *p_dev_mac = jstring_to_string(env, target_mac);
    char *p_auth_code = jstring_to_string(env, auth_code);

    jclass qd_unit_item_class = env->FindClass("com/qdingnet/qdaccess/QDUnitItem");
    jfieldID unit_num_id = env->GetFieldID(qd_unit_item_class, "unit_id", "J");
    jfieldID multi_time_counter_id = env->GetFieldID(qd_unit_item_class, "multi_time_counter", "I");
    jfieldID single_time_counter_id = env->GetFieldID(qd_unit_item_class, "single_time_counter", "I");

    jsize related_units_size = env->GetArrayLength(related_units);
    t_qd_unit_item tmp_unit_items[related_units_size];
    for (int i = 0; i < related_units_size; i++) {
        jobject unit_item_obj = env->GetObjectArrayElement(related_units, i);
        tmp_unit_items[i].unit_num = (uint32_t) env->GetLongField(unit_item_obj, unit_num_id);
        tmp_unit_items[i].multi_time_counter = (uint16_t) env->GetIntField(unit_item_obj,
                                                                           multi_time_counter_id);
        tmp_unit_items[i].single_time_counter = (uint16_t) env->GetIntField(unit_item_obj,
                                                                            single_time_counter_id);
        //释放java本地引用
        env->DeleteLocalRef(unit_item_obj);
    }
    //释放java本地引用
    env->DeleteLocalRef(qd_unit_item_class);

    const char *t_p_auth_code = p_auth_code && strlen(p_auth_code) > 0 ? p_auth_code : AUTH_CODE;
    LOGI("%s...units_size:%d, relay_state:%d, phone:%d, auth_code:%s", __FUNCTION__, related_units_size, relay_state, (uint32_t)phone, t_p_auth_code ? t_p_auth_code : "");

    t_buffer msg = build_config_device_msg_with_relay_state_and_phone_v2(p_dev_mac, keep_time,
                                                               (uint8_t *) tmp_unit_items,
                                                               related_units_size,
                                                               (uint8_t) relay_state,
                                                               (uint32_t) phone,
                                                               t_p_auth_code);

    if (p_dev_mac) {
        free(p_dev_mac);
    }
    if (p_auth_code) {
        free(p_auth_code);
    }

    if (msg.p_buffer != NULL) {
        jbyteArray packet = env->NewByteArray(msg.len);
        env->SetByteArrayRegion(packet, 0, msg.len, (jbyte *) msg.p_buffer);
        free(msg.p_buffer);
        return packet;
    } else {
        LOGI("%s %d msg buffer is NULL\n", __func__, __LINE__);
        jbyteArray packet = env->NewByteArray(0);
        return packet;
    }
}

jint JNI_analyse_config_ack_packet_v2(JNIEnv *env, jobject obj, jstring target_mac,
                                      jbyteArray packet) {
    LOGI("JNI_analyse_config_ack_packet_v2...");
    jboolean is_copy = 1;
    uint8_t *p_data = (uint8_t *) env->GetByteArrayElements(packet, &is_copy);
    int data_len = env->GetArrayLength(packet);

    if ((uint32_t) data_len < (sizeof(t_qd_access_msg) - 1 + sizeof(t_qd_access_content))) {
        LOGE(" data len = %d, is too small\n", data_len);
        return -1;
    }

    t_qd_access_msg *p_qd_access_msg = (t_qd_access_msg *) p_data;

    if (p_qd_access_msg->frame_head == QD_MSG_FRAMEHEAD) {
        int suc = decrypt_data(env, obj, target_mac, p_qd_access_msg->content_start,
                               p_qd_access_msg->total_length - sizeof(t_qd_access_msg),
                               p_qd_access_msg->decrypt_key);
        if (suc == 1) {
            t_qd_access_content *p_qd_config_ack_content =
                    (t_qd_access_content *) p_qd_access_msg->content_start;

            return p_qd_config_ack_content->ack_type;
        } else {
            LOGI("JNI_analyse_config_ack_packet_v2...FAILED");
        }
    } else {
        LOGI("JNI_analyse_config_ack_packet_v2...frame_head is wrong");
    }
    return -1;
}

jint JNI_genarate_key_v2(JNIEnv *env,
                         jobject obj,
                         jlong unit_id)
{
    //授权校验
    if(!isAuthorized)
    {
        return 0;
    }
    LOGD("%s", __FUNCTION__);
    time_t current_time = time(NULL);
    struct tm *p_tm;
    p_tm = localtime(&current_time); /*获取本地时区时间*/
    char salt_unit[64];
    sprintf(salt_unit, "%u%4d%02d%02d", (uint32_t) unit_id, p_tm->tm_year + 1900, p_tm->tm_mon + 1,
            p_tm->tm_mday);
    LOGI("JNI_genarate_key_v2...salt_unit:%s,%d", salt_unit, strlen(salt_unit));
    uint32_t passwrod = mesh_generate2((uint8_t *) salt_unit, strlen(salt_unit), 0, 0, 0x12345678);
    return passwrod;
}

jstring JNI_build_phone_advertise_msg_v2(JNIEnv *env,
                                         jobject obj,
                                         jstring target_mac,
                                         jlong app_user_id,
                                         jbyteArray floors,
                                         jstring auth_code)
{
    LOGI("JNI_build_phone_advertise_msg_v2...");
    //授权校验
    if(!isAuthorized)
    {
        return env->NewStringUTF("");
    }
    char *p_device_mac = jstring_to_string(env, target_mac);
    //提取楼层
    int floors_num = floors ? env->GetArrayLength(floors) : 0;
    jbyte * p_floors = NULL;
    if(floors_num > 0){
        jboolean copy = JNI_TRUE;
        p_floors = env->GetByteArrayElements(floors, &copy);
    }
    //提取并验证设备验证码
    char *p_auth_code = jstring_to_string(env, auth_code);
    const char *t_p_auth_code = p_auth_code && strlen(p_auth_code) > 0 ? p_auth_code : AUTH_CODE;
    LOGD("%s...auth_code:%s", __FUNCTION__, t_p_auth_code ? t_p_auth_code : "");
    //构造广播消息
    t_buffer msg = build_phone_advertise_msg_with_floors_v2(p_device_mac, QD_BLE_GATE_CTRL_VERSION_V2, (uint32_t) app_user_id, (uint8_t*)p_floors, floors_num, t_p_auth_code);

    //释放内存
    if(p_device_mac){
        free(p_device_mac);
    }
    if(p_floors){
        env->ReleaseByteArrayElements(floors, p_floors, JNI_ABORT);
    }
    if(p_auth_code){
        free(p_auth_code);
    }

    if (msg.p_buffer != NULL) {
        jstring strMsg = env->NewStringUTF((const char *) msg.p_buffer);
        free(msg.p_buffer);
        return strMsg;
    } else {
        LOGI("%s %d msg buffer is NULL\n", __func__, __LINE__);
        return env->NewStringUTF("");
    }
}


jbyteArray JNI_build_protocol_get_card_uid_msg_v2(JNIEnv *env,
                                                  jobject obj,
                                                  jstring target_mac)
{
    //授权校验
    if(!isAuthorized)
    {
        return env->NewByteArray(0);
    }
    LOGI("JNI_build_protocol_get_card_uid_msg_v2");
    char *p_device_mac = jstring_to_string(env, target_mac);
    LOGI("p_device_mac = %s", p_device_mac);
    t_buffer msg = build_protocol_get_card_uid_msg_v2(p_device_mac);
    if (p_device_mac) {
        free(p_device_mac);
    }
    if (msg.p_buffer != NULL) {
        jbyteArray packet = env->NewByteArray(msg.len);
        env->SetByteArrayRegion(packet, 0, msg.len, (jbyte *) msg.p_buffer);
        free(msg.p_buffer);
        return packet;
    } else {
        LOGI("%s %d msg buffer is NULL\n", __func__, __LINE__);
        jbyteArray packet = env->NewByteArray(0);
        return packet;
    }
}


jbyteArray JNI_build_protocol_write_card_data_msg_v2(JNIEnv *env,
                                                     jobject obj,
                                                     jstring target_mac,
                                                     jint wait_time_pre_card,
                                                     jstring write_data)
{
    //授权校验
    if(!isAuthorized)
    {
        return env->NewByteArray(0);
    }
    LOGI("build_protocol_write_card_data_msg_v2");
    char *p_device_mac = jstring_to_string(env, target_mac);
    char *p_write_data = jstring_to_string(env, write_data);
    LOGI("%s p_device_mac = %s", __func__, p_device_mac);
    LOGI("%s p_write_data = %s", __func__, p_write_data);
    LOGI("%s wait_time_pre_card = %d ", __func__, wait_time_pre_card);
    t_buffer msg = build_protocol_write_card_data_msg_v2(p_device_mac, wait_time_pre_card,
                                                         p_write_data);
    if (p_device_mac) {
        free(p_device_mac);
    }
    if (p_write_data) {
        free(p_write_data);
    }
    if (msg.p_buffer != NULL) {
        jbyteArray packet = env->NewByteArray(msg.len);
        env->SetByteArrayRegion(packet, 0, msg.len, (jbyte *) msg.p_buffer);
        free(msg.p_buffer);
        return packet;
    } else {
        LOGI("%s %d msg buffer is NULL\n", __func__, __LINE__);
        jbyteArray packet = env->NewByteArray(0);
        return packet;
    }
}

jbyteArray JNI_build_protocol_cancel_write_card_data_msg_v2(JNIEnv *env,
                                                            jobject obj,
                                                            jstring target_device_mac)
{
    //授权校验
    if(!isAuthorized)
    {
        return env->NewByteArray(0);
    }
    LOGD("%s", __FUNCTION__);
    char *p_device_mac = jstring_to_string(env, target_device_mac);
    t_buffer msg = build_protocol_cancel_write_card_data_msg_v2(p_device_mac);
    if (p_device_mac) {
        free(p_device_mac);
    }
    if (msg.p_buffer != NULL) {
        jbyteArray packet = env->NewByteArray(msg.len);
        env->SetByteArrayRegion(packet, 0, msg.len, (jbyte *) msg.p_buffer);
        free(msg.p_buffer);
        return packet;
    } else {
        LOGI("%s %d msg buffer is NULL\n", __func__, __LINE__);
        jbyteArray packet = env->NewByteArray(0);
        return packet;
    }
}

jbyteArray JNI_build_protocol_update_file_desc_msg_v2(JNIEnv *env,
                                                      jobject obj,
                                                      jstring target_device_mac,
                                                      jint version_num,
                                                      jint file_len,
                                                      jstring md5)
{
    //授权校验
    if(!isAuthorized)
    {
        return env->NewByteArray(0);
    }
    LOGI("JNI_build_protocol_update_file_desc_msg_v2  version_num == %d ", version_num);
    char *p_device_mac = jstring_to_string(env, target_device_mac);
    char *p_md5 = jstring_to_string(env, md5);
    t_buffer msg = build_protocol_update_file_desc_msg_v2(p_device_mac, version_num, file_len,
                                                          p_md5);
    if (p_device_mac) {
        free(p_device_mac);
    }
    if (p_md5) {
        free(p_md5);
    }
    if (msg.p_buffer != NULL) {
        jbyteArray packet = env->NewByteArray(msg.len);
        env->SetByteArrayRegion(packet, 0, msg.len, (jbyte *) msg.p_buffer);
        free(msg.p_buffer);
        return packet;
    } else {
        LOGI("%s %d msg buffer is NULL\n", __func__, __LINE__);
        jbyteArray packet = env->NewByteArray(0);
        return packet;
    }
}

jbyteArray JNI_build_protocol_update_file_desc_msg_v3(JNIEnv *env,
                                                      jobject obj,
                                                      jstring target_device_mac,
                                                      jint version_num,
                                                      jint file_len,
                                                      jint crc)
{
    //授权校验
    if(!isAuthorized)
    {
        return env->NewByteArray(0);
    }
    LOGI("%s version_num == %d ", __func__,version_num);
    char *p_device_mac = jstring_to_string(env, target_device_mac);
   // char *p_md5 = jstring_to_string(env, md5);
    t_buffer msg = build_protocol_update_file_desc_msg_v3(p_device_mac, version_num, file_len,
                                                          crc);
    if (p_device_mac) {
        free(p_device_mac);
    }
//    if (p_md5) {
//        free(p_md5);
//    }
    if (msg.p_buffer != NULL) {
        jbyteArray packet = env->NewByteArray(msg.len);
        env->SetByteArrayRegion(packet, 0, msg.len, (jbyte *) msg.p_buffer);
        free(msg.p_buffer);
        return packet;
    } else {
        LOGI("%s %d msg buffer is NULL\n", __func__, __LINE__);
        jbyteArray packet = env->NewByteArray(0);
        return packet;
    }
}

jbyteArray JNI_build_protocol_transport_update_file_msg_v2(JNIEnv *env,
                                                           jobject obj,
                                                           jstring target_mac,
                                                           jint seek_index,
                                                           jbyteArray data,
                                                           jint data_len)
{
    //授权校验
    if(!isAuthorized)
    {
        return env->NewByteArray(0);
    }
    LOGI("JNI_build_protocol_transport_update_file_msg_v2 data_len == %d", data_len);
    char *p_device_mac = jstring_to_string(env, target_mac);
    jsize len = env->GetArrayLength(data);
    jbyte *jbarray = (jbyte *) malloc(len * sizeof(jbyte));
    env->GetByteArrayRegion(data, 0, len, jbarray);
    uint8_t *p_data = (uint8_t *) jbarray;
    t_buffer msg = build_protocol_transport_update_file_msg_v2(p_device_mac, seek_index, p_data,
                                                               data_len);
    if (p_device_mac) {
        free(p_device_mac);
    }
    if (p_data) {
        free(p_data);
    }
    if (msg.p_buffer != NULL) {
        jbyteArray packet = env->NewByteArray(msg.len);
        env->SetByteArrayRegion(packet, 0, msg.len, (jbyte *) msg.p_buffer);
        free(msg.p_buffer);
        return packet;
    } else {
        LOGI("%s %d msg buffer is NULL\n", __func__, __LINE__);
        jbyteArray packet = env->NewByteArray(0);
        return packet;
    }
}

jbyteArray  JNI_build_update_state_request_msg_v3(JNIEnv *env,
                                                  jobject obj,
                                                  jstring target_mac)
{
    //授权校验
    if(!isAuthorized)
    {
        return env->NewByteArray(0);
    }
    LOGI("%s start",__func__);
    char *p_device_mac = jstring_to_string(env, target_mac);
    LOGI("p_device_mac = %s", p_device_mac);
    t_buffer msg = build_update_state_request_msg_v3(p_device_mac);
    if (p_device_mac) {
        free(p_device_mac);
    }
    if (msg.p_buffer != NULL) {
        jbyteArray packet = env->NewByteArray(msg.len);
        env->SetByteArrayRegion(packet, 0, msg.len, (jbyte *) msg.p_buffer);
        free(msg.p_buffer);
        return packet;
    } else {
        LOGI("%s %d msg buffer is NULL\n", __func__, __LINE__);
        jbyteArray packet = env->NewByteArray(0);
        return packet;
    }
}

/*********************离线发卡模块***********************/
jbyteArray JNI_build_protocol_standalone_read_cardno_msg_v2(JNIEnv *env,
                                                            jobject obj,
                                                            jstring target_mac,
                                                            jint timeout)
{
    //授权校验
    if(!isAuthorized)
    {
        return env->NewByteArray(0);
    }
    LOGI("%s start", __func__);
    char *p_device_mac = jstring_to_string(env, target_mac);
    LOGI("p_device_mac = %s", p_device_mac);
    t_buffer msg = build_standalone_cardno_read_msg_v2(p_device_mac, (uint8_t)timeout);
    if (p_device_mac) {
        free(p_device_mac);
    }
    if (msg.p_buffer != NULL) {
        jbyteArray packet = env->NewByteArray(msg.len);
        env->SetByteArrayRegion(packet, 0, msg.len, (jbyte *) msg.p_buffer);
        free(msg.p_buffer);
        return packet;
    } else {
        LOGI("%s %d msg buffer is NULL\n", __func__, __LINE__);
        jbyteArray packet = env->NewByteArray(0);
        return packet;
    }
}

jbyteArray JNI_build_protocol_standalone_add_cardno_msg_v2(JNIEnv *env,
                                                           jobject obj,
                                                           jstring target_mac,
                                                           jlong cardNo,
                                                           jlong validity,
                                                           jbyteArray floors)
{
    //授权校验
    if(!isAuthorized)
    {
        return env->NewByteArray(0);
    }
    LOGI("%s start", __func__);
    char *p_device_mac = jstring_to_string(env, target_mac);
    LOGI("p_device_mac = %s", p_device_mac);
    jsize len = env->GetArrayLength(floors);
    jboolean is_copy = 0;
    uint8_t *pFloors = (uint8_t *)env->GetByteArrayElements(floors, &is_copy);
    uint64_t floorsMask = 0;
    for(int i=0; i<len; i++){
        floorsMask |= pFloors[i];
    }
    t_buffer msg = build_standalone_cardno_add_msg_v2(p_device_mac, cardNo, validity, floorsMask);
    if (p_device_mac) {
        free(p_device_mac);
    }
    if (msg.p_buffer != NULL) {
        jbyteArray packet = env->NewByteArray(msg.len);
        env->SetByteArrayRegion(packet, 0, msg.len, (jbyte *) msg.p_buffer);
        free(msg.p_buffer);
        return packet;
    } else {
        LOGI("%s %d msg buffer is NULL\n", __func__, __LINE__);
        jbyteArray packet = env->NewByteArray(0);
        return packet;
    }
}

jbyteArray JNI_build_protocol_standalone_delete_cardno_msg_v2(JNIEnv *env,
                                                              jobject obj,
                                                              jstring target_mac,
                                                              jlongArray cardNos)
{
    //授权校验
    if(!isAuthorized)
    {
        return env->NewByteArray(0);
    }
    LOGI("%s start", __func__);
    char *p_device_mac = jstring_to_string(env, target_mac);
    LOGI("p_device_mac = %s", p_device_mac);
    jsize len = env->GetArrayLength(cardNos);
    jboolean is_copy = 0;
    jlong *jCardNos = env->GetLongArrayElements(cardNos, &is_copy);
    uint32_t *pCardNos = (uint32_t *)malloc(len * sizeof(uint32_t));
    for(int i=0; i<len; i++){
        pCardNos[i] = jCardNos[i];
    }
    t_buffer msg = build_standalone_cardno_del_msg_v2(p_device_mac, pCardNos, len);
    if (p_device_mac) {
        free(p_device_mac);
    }
    if(pCardNos != NULL){
        free(pCardNos);
    }
    if (msg.p_buffer != NULL) {
        jbyteArray packet = env->NewByteArray(msg.len);
        env->SetByteArrayRegion(packet, 0, msg.len, (jbyte *) msg.p_buffer);
        free(msg.p_buffer);
        return packet;
    } else {
        LOGI("%s %d msg buffer is NULL\n", __func__, __LINE__);
        jbyteArray packet = env->NewByteArray(0);
        return packet;
    }
}

/////////////////////////////////// V4.0 /////////////////////////////////////////////////
jbyteArray JNI_build_open_door_msg_v4(JNIEnv *env,
                                      jobject obj,
                                      jstring target_mac,
                                      jbyte keep_time,
                                      jlong app_user_id,
                                      jint pass_id,
                                      jstring auth_code)
{
    //授权校验
    if(!isAuthorized)
    {
        return env->NewByteArray(0);
    }
    char *p_target_mac = jstring_to_string(env, target_mac);
    char *p_auth_code = jstring_to_string(env, auth_code);

    const char *t_p_auth_code = p_auth_code && strlen(p_auth_code) > 0 ? p_auth_code : AUTH_CODE;
    LOGD("%s...auth_code:%s", __FUNCTION__, t_p_auth_code ? t_p_auth_code : "");

    t_buffer msg = build_open_door_msg_v4(p_target_mac, keep_time, app_user_id, OS_ANDROID, pass_id, t_p_auth_code);

    LOGI("get here %s\n", __func__);

    if (p_target_mac) {
        free(p_target_mac);
    }
    if (p_auth_code) {
        free(p_auth_code);
    }
    if (msg.p_buffer != NULL) {
        jbyteArray packet = env->NewByteArray(msg.len);
        env->SetByteArrayRegion(packet, 0, msg.len, (jbyte *) msg.p_buffer);
        free(msg.p_buffer);
        return packet;
    } else {
        LOGI("%s %d msg buffer is NULL\n", __func__, __LINE__);
        jbyteArray packet = env->NewByteArray(0);
        return packet;
    }
}

jstring JNI_build_phone_advertise_msg_v4(JNIEnv *env,
                                         jobject obj,
                                         jstring target_mac,
                                         jlong app_user_id,
                                         jint pass_id)
{
    //授权校验
    if(!isAuthorized)
    {
        return env->NewStringUTF("");
    }
    LOGI("JNI_build_phone_advertise_msg_v4...");
    char *p_device_mac = jstring_to_string(env, target_mac);
    t_buffer msg = build_phone_advertise_msg_v4(p_device_mac, QD_BLE_GATE_CTRL_VERSION_V4, (uint32_t) app_user_id, pass_id);

    if (p_device_mac) {
        free(p_device_mac);
    }
    if (msg.p_buffer != NULL) {
        jstring strMsg = env->NewStringUTF((const char *) msg.p_buffer);
        free(msg.p_buffer);
        LOGI("JNI_build_phone_advertise_msg_v4 result : %s", msg.p_buffer);
        return strMsg;
    } else {
        LOGI("%s %d msg buffer is NULL\n", __func__, __LINE__);
        return NULL;
    }
}

jbyteArray JNI_build_config_device_msg_v4(JNIEnv * env, jobject obj, jstring target_mac, jstring auth_code){
    //授权校验
    if(!isAuthorized)
    {
        return env->NewByteArray(0);
    }
    char *p_device_mac = jstring_to_string(env, target_mac);
    char *p_auth_code = jstring_to_string(env, auth_code);
    const char *t_p_auth_code = p_auth_code && strlen(p_auth_code) > 0 ? p_auth_code : AUTH_CODE;
    LOGD("%s...auth_code:%s", __FUNCTION__, t_p_auth_code ? t_p_auth_code : "");

    t_buffer msg = build_config_device_msg_v4(p_device_mac, t_p_auth_code);

    if (p_device_mac) {
        free(p_device_mac);
    }
    if (p_auth_code) {
        free(p_auth_code);
    }

    if (msg.p_buffer != NULL) {
        jbyteArray packet = env->NewByteArray(msg.len);
        env->SetByteArrayRegion(packet, 0, msg.len, (jbyte *) msg.p_buffer);
        free(msg.p_buffer);
        return packet;
    } else {
        LOGI("%s %d msg buffer is NULL\n", __func__, __LINE__);
        jbyteArray packet = env->NewByteArray(0);
        return packet;
    }
}

/////////////////////////////////// V5.0 /////////////////////////////////////////////////
jbyteArray JNI_build_open_door_msg_v5(JNIEnv *env, jobject obj, jstring target_mac, jbyte keep_time,
                                      jlong app_user_id) {
    LOGI("%s...keep_time:%u, app_user_id:%llu", __func__, keep_time, app_user_id);
    //授权校验
    if (!isAuthorized) {
        return env->NewByteArray(0);
    }
    LOGI("get here %s\n", __func__);
    char *p_target_mac = jstring_to_string(env, target_mac);

    t_buffer msg = build_open_door_msg_v5(p_target_mac, keep_time, app_user_id, OS_ANDROID);

    if (p_target_mac) {
        free(p_target_mac);
    }

    if (msg.p_buffer) {
        jbyteArray packet = env->NewByteArray(msg.len);
        env->SetByteArrayRegion(packet, 0, msg.len, (jbyte *) msg.p_buffer);
        free(msg.p_buffer);
        return packet;
    } else {
        LOGI("%s %d msg buffer is NULL\n", __func__, __LINE__);
        jbyteArray packet = env->NewByteArray(0);
        return packet;
    }
}

jstring
JNI_build_phone_advertise_msg_v5(JNIEnv *env, jobject obj, jstring target_mac, jlong app_user_id,
                                 jbyteArray floors) {
    LOGI("%s...app_user_id:%llu", __func__, app_user_id);
    //授权校验
    if (!isAuthorized) {
        return env->NewStringUTF("");
    }
    LOGI("get here %s\n", __func__);
    char *p_device_mac = jstring_to_string(env, target_mac);
    //提取楼层
    int floors_num = floors ? env->GetArrayLength(floors) : 0;
    jbyte *p_floors = NULL;
    if (floors_num > 0) {
        jboolean copy = JNI_TRUE;
        p_floors = env->GetByteArrayElements(floors, &copy);
    }
    //构造广播消息
    t_buffer msg = build_phone_advertise_msg_with_floors_v5(p_device_mac,
                                                            QD_BLE_GATE_CTRL_VERSION_V5,
                                                            app_user_id,
                                                            (uint8_t *) p_floors, floors_num);

    //释放内存
    if (p_device_mac) {
        free(p_device_mac);
    }
    if (p_floors) {
        env->ReleaseByteArrayElements(floors, p_floors, JNI_ABORT);
    }
    if (msg.p_buffer) {
        jstring strMsg = env->NewStringUTF((const char *) msg.p_buffer);
        free(msg.p_buffer);
        return strMsg;
    } else {
        LOGI("%s %d msg buffer is NULL\n", __func__, __LINE__);
        return env->NewStringUTF("");
    }
}


//////////////////////////////////// factory test function ////////////////////////////////////////////
jbyteArray JNI_build_enable_433_test_msg_v2(JNIEnv *env, jobject obj, jstring target_mac) {
    LOGI("%s", __func__);
    char *p_device_mac = jstring_to_string(env, target_mac);
    t_buffer msg = build_protocol_enable_433_test_msg_v2(p_device_mac);

    if (p_device_mac) {
        free(p_device_mac);
    }

    if(msg.p_buffer != NULL){
        jbyteArray packet = env->NewByteArray(msg.len);
        env->SetByteArrayRegion(packet,0,msg.len,(jbyte*)msg.p_buffer);
        return packet;
    }else{
        LOGI("%s %d msg buffer is NULL\n", __func__, __LINE__);
        jbyteArray packet = env->NewByteArray(0);
        return packet;
    }
}

jbyteArray JNI_build_get_hal_state_msg_v2(JNIEnv* env, jobject obj, jstring target_mac){
    LOGI("%s", __func__);
    char *p_device_mac = jstring_to_string(env, target_mac);
    t_buffer msg = build_protocol_get_hal_state_msg_v2(p_device_mac);

    if(p_device_mac){
        free(p_device_mac);
    }

    if(msg.p_buffer != NULL){
        jbyteArray packet = env->NewByteArray(msg.len);
        env->SetByteArrayRegion(packet,0,msg.len,(jbyte*)msg.p_buffer);
        return packet;
    }else{
        LOGI("%s %d msg buffer is NULL\n", __func__, __LINE__);
        jbyteArray packet = env->NewByteArray(0);
        return packet;
    }
}

jbyteArray JNI_build_get_device_info_msg_v4(JNIEnv* env, jobject obj, jstring target_mac){
    LOGI("%s", __func__);
    char *p_device_mac = jstring_to_string(env, target_mac);
    t_buffer msg = build_protocol_get_device_info_msg_v4(p_device_mac);

    if(p_device_mac){
        free(p_device_mac);
    }

    if(msg.p_buffer != NULL){
        jbyteArray packet = env->NewByteArray(msg.len);
        env->SetByteArrayRegion(packet,0,msg.len,(jbyte*)msg.p_buffer);
        return packet;
    }else{
        LOGI("%s %d msg buffer is NULL\n", __func__, __LINE__);
        jbyteArray packet = env->NewByteArray(0);
        return packet;
    }
}

jbyteArray JNI_build_get_gate_magnetism_state_msg_v5(JNIEnv* env, jobject obj, jstring target_mac){
    LOGI("%s", __func__);
    char *p_device_mac = jstring_to_string(env, target_mac);
    t_buffer msg = build_protocol_get_gate_magnetism_state_msg_v5(p_device_mac);

    if(p_device_mac){
        free(p_device_mac);
    }

    if(msg.p_buffer != NULL){
        jbyteArray packet = env->NewByteArray(msg.len);
        env->SetByteArrayRegion(packet,0,msg.len,(jbyte*)msg.p_buffer);
        return packet;
    }else{
        LOGI("%s %d msg buffer is NULL\n", __func__, __LINE__);
        jbyteArray packet = env->NewByteArray(0);
        return packet;
    }
}

/////////////////////////////////// common function /////////////////////////////////////////////////
jint JNI_PJWHash(JNIEnv *env, jobject obj, jbyteArray packet) {
    int data_len = env->GetArrayLength(packet);
    jboolean is_copy = 1;
    const char *data = (char *) env->GetByteArrayElements(packet, &is_copy);
    uint32_t hash = PJWHash(data, data_len);
    return hash;
}

jint JNI_Do_Calc_CheckSum(JNIEnv *env, jobject obj, jbyteArray packet){
    int data_len = env->GetArrayLength(packet);
    jbyte* dataPtr = env->GetByteArrayElements(packet, NULL);
    return Do_Calc_CheckSum((uint8_t*)dataPtr,data_len);
}

