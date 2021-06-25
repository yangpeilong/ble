/*
 * JNI_OnLoad.cpp
 *
 *  Created on: Apr 1, 2014
 *      Author: china
 */

/*
 * NativeMethods_OnLoad.cpp
 *
 *  Created on: Mar 25, 2014
 *      Author: china
 */
#include <stdio.h>
#include <jni.h>
#include "debugLog.h"
#include "JNI_Methods.h"
#include "CQDAccessJniApi.h"
#include "../secure/JNI_Secure_Methods.h"

#ifndef LIB_VERSION
#define LIB_VERSION "V1.0.1"
#endif

#ifndef LIB_NAME
#define LIB_NAME "qdjni"
#endif

#ifndef CLASS_PATH_NAME
#define CLASS_PATH_NAME "com/qdingnet/qdaccess/ProtocolHelper"
#endif

#ifndef QD_ACCESS_MSG_HANDLER_CLASS_PATH_NAME
#define QD_ACCESS_MSG_HANDLER_CLASS_PATH_NAME "com/qdingnet/qdaccess/QDAccessMsgHandler"
#endif

static const char *version = LIB_VERSION;
static const char *libName = LIB_NAME;
static const char *classPathName = CLASS_PATH_NAME;
static const char *qdaccessMsgHandlerClassPathName = QD_ACCESS_MSG_HANDLER_CLASS_PATH_NAME;

/*
 * Register several native methods for one class.
 */
static int registerNativeMethods(JNIEnv *env, const char *className, JNINativeMethod *gMethods,
                                 int numMethods) {
    jclass clazz;
//	LOGI("RegisterNatives start for '%s'", className);
    clazz = env->FindClass(className);
    if (clazz == NULL) {
        LOGE("Native registration unable to find class '%s'", className);
        return JNI_FALSE;
    }
    if (env->RegisterNatives(clazz, gMethods, numMethods) < 0) {
        LOGE("RegisterNatives failed for '%s'", className);
        return JNI_FALSE;
    }
    return JNI_TRUE;
}

/*
 * Register native methods for all classes we know about.
 *
 * returns JNI_TRUE on success.
 */
static int registerNatives(JNIEnv *env) {
    if (!registerNativeMethods(env, classPathName, methods, sizeof(methods) / sizeof(methods[0]))) {
        return JNI_FALSE;
    }
    if (!registerNativeMethods(env, qdaccessMsgHandlerClassPathName, data_parse_methods, sizeof(data_parse_methods) / sizeof(data_parse_methods[0]))) {
        return JNI_FALSE;
    }
    if (!registerNativeMethods(env, classPathName, secure_methods, sizeof(secure_methods)/sizeof(secure_methods[0]) )){
        return JNI_FALSE;
    }
    return JNI_TRUE;
}

// ----------------------------------------------------------------------------

/*
 * This is called by the VM when the shared library is first loaded.
 */
typedef union {
    JNIEnv *env;
    void *venv;
} UnionJNIEnvToVoid;

jint JNI_OnLoad(JavaVM *vm, void *reserved) {
    UnionJNIEnvToVoid uenv;
    uenv.venv = NULL;
    jint result = -1;
    JNIEnv *env = NULL;
    LOGE("------Load %s Lib , %s------", libName, version);
    LOGE("------Build at %s %s -------", __DATE__, __TIME__);
    if (vm->GetEnv(&uenv.venv, JNI_VERSION_1_4) != JNI_OK) {
        LOGE("GetEnv failed");
        goto bail;
    }

    env = uenv.env;

    if (registerNatives(env) != JNI_TRUE) {
        LOGE("registerNatives failed");
        goto bail;
    }

    result = JNI_VERSION_1_4;

    bail:
    return result;
}

