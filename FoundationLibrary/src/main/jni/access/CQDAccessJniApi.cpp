//
// Created by 胡涛 on 2017/5/11.
//

#include <jni.h>
#include <stdint.h>
#include <malloc.h>
#include <string>

#include "CMsgProcessor.h"
#include "debugLog.h"


static CMsgProcessor *p_msgProcessor = NULL;
static CMsgProcessor *broadcast_msg_processor = NULL;

std::string jstring2string(JNIEnv *env, jstring jStr) {
    std::string str;
    if(jStr){
        jboolean copy = 1;
        const char *cstr = env->GetStringUTFChars(jStr, &copy);
        str = std::string(cstr);
        env->ReleaseStringUTFChars(jStr, cstr);
    }
    return str;
}

int QDAccessJNIModelInit(JNIEnv *env, jobject obj) {
    if (!p_msgProcessor) {
        p_msgProcessor = new CMsgProcessor(env, obj);
    }
    if (!broadcast_msg_processor) {
        broadcast_msg_processor = new CMsgProcessor(env, obj);
    }
    return 0;
}

int QDAccessJNIModelExit(JNIEnv *env, jobject obj) {
    if(p_msgProcessor){
        delete p_msgProcessor;
        p_msgProcessor = NULL;
    }
    if(broadcast_msg_processor){
        delete broadcast_msg_processor;
        broadcast_msg_processor = NULL;
    }
    return 0;
}

int QDAccessJNIPushRawData(JNIEnv *env, jobject obj, jstring targetDeviceMac, jbyteArray packet) {
    std::string mac_str = jstring2string(env, targetDeviceMac);
    LOGI("%s mac_str == %s", __FUNCTION__, mac_str.c_str());
    if (!p_msgProcessor) {
        p_msgProcessor = new CMsgProcessor(env, obj);
    }
    if (p_msgProcessor) {
        jboolean is_copy = 1;
        jbyte *p_data = env->GetByteArrayElements(packet, &is_copy);
        int data_len = env->GetArrayLength(packet);
        p_msgProcessor->update_jni_env(env);
        p_msgProcessor->handle_msg((uint8_t *) p_data, data_len, mac_str);
        if(p_data){
            env->ReleaseByteArrayElements(packet, p_data, JNI_ABORT);
        }
        return 0;
    } else {
        return -1;
    }
}

int QDAccessJNIPushBroadcastData(JNIEnv *env, jobject obj, jstring targetDeviceMac, jbyteArray packet){
    std::string mac_str = jstring2string(env, targetDeviceMac);
    LOGI("%s mac_str == %s", __FUNCTION__, mac_str.c_str());
    if (!broadcast_msg_processor) {
        broadcast_msg_processor = new CMsgProcessor(env, obj);
    }
    if (broadcast_msg_processor) {
        jboolean is_copy = 1;
        jbyte *p_data = env->GetByteArrayElements(packet, &is_copy);
        int data_len = env->GetArrayLength(packet);
        broadcast_msg_processor->update_jni_env(env);
        broadcast_msg_processor->process_broadcast_open_door_v2((uint8_t *)p_data, data_len, mac_str);
        if(p_data){
            env->ReleaseByteArrayElements(packet, p_data, JNI_ABORT);
        }
        return 0;
    } else {
        return -1;
    }
}

int QDAccessJNITest(JNIEnv *env, jobject obj) {
    LOGI("QDAccessJNITest");
    long long card_uid;
    card_uid = 12345678;
    LOGI("process_write_card_content_ack_v2 ack card_uid ==  %lld", card_uid);
    char buff[16];
    sprintf(buff, "%lld", card_uid);
    char *str = buff;
    LOGI("process_write_card_content_ack_v2 ack card_uid ==  %s", str);

    jclass callBackClass = env->FindClass("com/qdingnet/qdaccess/QDAccessMsgHandler");
    jmethodID on_open_door_ack_method_id = env->GetStaticMethodID(callBackClass, "onOpenDoorAck",
                                                                  "(I)V");
    if (on_open_door_ack_method_id) {
        env->CallStaticVoidMethod(callBackClass, on_open_door_ack_method_id, 1);
    }
    return 0;
}

