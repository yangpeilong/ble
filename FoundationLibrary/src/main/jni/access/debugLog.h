#ifndef DEBUGLOG_H
#define DEBUGLOG_H

#define PLATEFORM LINUX

#if 1
#include <android/log.h>

#ifndef DBG_LEVEL
#define DBG_LEVEL 4
#endif

#define LOG    "QD-NDK" // 这个是自定义的LOG的标识
#if DBG_LEVEL > 5
#define LOGD(...)  __android_log_print(ANDROID_LOG_DEBUG,LOG,__VA_ARGS__) // 定义LOGD类型
#else
#define LOGD(msg, ...) do{}while(0)
#endif
#if DBG_LEVEL > 4
#define LOGI(...)  __android_log_print(ANDROID_LOG_INFO,LOG,__VA_ARGS__) // 定义LOGI类型
#else
#define LOGI(msg, ...) do{}while(0)
#endif
#if DBG_LEVEL > 3
#define LOGW(...)  __android_log_print(ANDROID_LOG_WARN,LOG,__VA_ARGS__) // 定义LOGW类型
#else
#define LOGW(msg, ...) do{}while(0)
#endif
#if DBG_LEVEL > 2
#define LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG,__VA_ARGS__) // 定义LOGE类型
#else
#define LOGE(msg, ...) do{}while(0)
#endif
#if DBG_LEVEL > 1
#define LOGF(...)  __android_log_print(ANDROID_LOG_FATAL,LOG,__VA_ARGS__) // 定义LOGF类型
#else
#define LOGF(msg, ...) do{}while(0)
#endif

#else

#include <stdio.h>
#include <stdarg.h>
#ifndef DBG_LEVEL
#define DBG_LEVEL 5
#endif

#define LOG    "[QDKEY::" // 这个是自定义的LOG的标识
#if DBG_LEVEL > 2
#define LOGI(fmt, ...) printf(LOG"INFO] "fmt"\n",##__VA_ARGS__)
#else
#define LOGI(fmt, ...) do{}while(0)
#endif

#if DBG_LEVEL > 1
#define LOGW(fmt, ...) printf(LOG"WARN] "fmt"\n", ##__VA_ARGS__)
#else
#define LOGW(msg, ...) do{}while(0)
#endif

#if DBG_LEVEL > 0
#define LOGE(fmt, ...) printf(LOG" ERR] "fmt"\n", ##__VA_ARGS__)
#else
#define LOGE(fmt, ...) do{}while(0)
#endif

#define LOGD(fmt, ...) do{}while(0)
#define LOGF(fmt, ...) do{}while(0)

#endif


#endif
