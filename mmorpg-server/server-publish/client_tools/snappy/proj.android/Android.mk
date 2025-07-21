LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
   
LOCAL_MODULE := snappy
LOCAL_CFLAGS += -std=c99
LOCAL_CFLAGS += -D SNAPPY_ANDROID_LIB
LOCAL_C_INCLUDES += external/stlport/stlport
LOCAL_C_INCLUDES += bionic
LOCAL_C_INCLUDES += bionic/libstdc++/include
LOCAL_C_INCLUDES += ../src/
LOCAL_LDLIBS := -L$(SYSROOT)/usr/lib -llog
LOCAL_SRC_FILES := \
	../src/snappy.cc \
	../src/snappy-c.cc \
	../src/snappy-sinksource.cc \
	../src/snappy-stubs-internal.cc 

LOCAL_SHARED_LIBRARIES := libstlport
include $(BUILD_SHARED_LIBRARY)
