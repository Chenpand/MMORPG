LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
   
LOCAL_MODULE := enet
LOCAL_CFLAGS += -std=c99
LOCAL_CFLAGS += -D ENET_DEBUG_LOG
LOCAL_C_INCLUDES += external/stlport/stlport
LOCAL_C_INCLUDES += bionic
LOCAL_C_INCLUDES += bionic/libstdc++/include
LOCAL_C_INCLUDES += ../src/
LOCAL_LDLIBS := -L$(SYSROOT)/usr/lib -llog
LOCAL_SRC_FILES := \
	../src/callbacks.c \
	../src/compress.c \
	../src/host.c \
	../src/list.c \
	../src/packet.c \
	../src/peer.c \
	../src/protocol.c \
	../src/unix.c \
	../src/win32.c \
	../src/utility.c

LOCAL_SHARED_LIBRARIES := libstlport
include $(BUILD_SHARED_LIBRARY)
