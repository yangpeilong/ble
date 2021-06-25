LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

$(shell touch $(LOCAL_PATH)/JNI_OnLoad.cpp)

JAVA_LIB_NAME = \"qdaccess\"
JAVA_LIB_VERSION = \"V2.0.5\"
JAVA_CLASS_PATH_NAME = \"com/qdingnet/qdaccess/ProtocolHelper\"

#JAVA_CLASS_PATH_NAME = \"com/qdingnet/push/EncryptEngine\"

LOCAL_MODULE    := qdaccess
LOCAL_SRC_FILES := JNI_OnLoad.cpp \
				   QDAccess.cpp \
				   qd_access_protocol_helper.c \
				   qd_common_utils.c \
				   qd_access_protocol_helper_private.c \
				   qdkey.c \
				   meshsub.c \
				   qd_access_protocol_v2_helper_private.c \
				   qd_access_protocol_v2_helper.c 
				   

LOCAL_CXXFLAGS += -DCLASS_PATH_NAME=$(JAVA_CLASS_PATH_NAME) -DLIB_NAME=$(JAVA_LIB_NAME) \
               -DLIB_VERSION=$(JAVA_LIB_VERSION) -std=c++11
               
LOCAL_CFLAGS += -std=c99

LOCAL_LDLIBS := -llog

include $(BUILD_SHARED_LIBRARY)
