LOCAL_PATH:=$(call my-dir)
include $(CLEAR_VARS)

YOSAL_ROOT := $(LOCAL_PATH)/../../..

LOCAL_C_INCLUDES += $(YOSAL_ROOT)/include
LOCAL_C_INCLUDES += $(LOCAL_PATH)/include

LOCAL_CFLAGS := -Wall -Werror

LOCAL_SRC_FILES += test-yosal.c

LOCAL_STATIC_LIBRARIES += libyahoo_yosal

ifeq ($(BUILD_ANDROID),true)
LOCAL_LDLIBS += -llog
endif

# LOCAL_STATIC_LIBRARIES += cpufeatures
ifeq ($(NDK_ROOT),)
LOCAL_SHARED_LIBRARIES  += libcutils libutils
endif

LOCAL_MODULE := test-yosal
LOCAL_MODULE_TAGS := optional

include $(BUILD_EXECUTABLE)
