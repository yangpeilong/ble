//
// Created by 胡涛 on 2017/5/11.
//

#include <jni.h>

#ifndef QRCODE_CQDACCESSJNIAPI_H
#define QRCODE_CQDACCESSJNIAPI_H


JNIEXPORT jint JNICALL QDAccessJNIModelInit(JNIEnv *env, jobject obj);

JNIEXPORT jint JNICALL QDAccessJNIModelExit(JNIEnv *env, jobject obj);

JNIEXPORT jint JNICALL QDAccessJNITest(JNIEnv *env, jobject obj);

JNIEXPORT jint JNICALL QDAccessJNIPushRawData(JNIEnv *env, jobject obj, jstring targetDeviceMac,
                                              jbyteArray pData);

JNIEXPORT jint JNICALL QDAccessJNIPushBroadcastData(JNIEnv *env, jobject obj, jstring targetDeviceMac,
                                                    jbyteArray pData);


JNINativeMethod data_parse_methods[] = {
        {"QDAccessJNIModelInit", "()I",                     (void *) QDAccessJNIModelInit},
        {"QDAccessJNIModelExit", "()I",                     (void *) QDAccessJNIModelExit},
        {"QDAccessJNIModelTest", "()I",                     (void *) QDAccessJNITest},
        {"QDAccessJNIPushData",  "(Ljava/lang/String;[B)I", (void *) QDAccessJNIPushRawData},
        {"QDAccessJNIPushBroadcastData",  "(Ljava/lang/String;[B)I", (void *) QDAccessJNIPushBroadcastData}
};

#endif //QRCODE_CQDACCESSJNIAPI_H
