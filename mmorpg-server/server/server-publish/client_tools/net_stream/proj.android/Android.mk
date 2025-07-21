LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
   
LOCAL_MODULE := net_stream
LOCAL_C_INCLUDES += external/stlport/stlport
LOCAL_C_INCLUDES += bionic
LOCAL_C_INCLUDES += bionic/libstdc++/include
LOCAL_SRC_FILES := 
	../src/net_stream.c

LOCAL_SHARED_LIBRARIES := libstlport
include $(BUILD_SHARED_LIBRARY)