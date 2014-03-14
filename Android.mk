# Android makefile for Yosal
LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

YOSAL_SRC_FILES := src/base64/cdecode.c
YOSAL_SRC_FILES += src/base64/cencode.c
YOSAL_SRC_FILES += src/base64/base64.c
YOSAL_SRC_FILES += src/core/yalloc.c
YOSAL_SRC_FILES += src/core/ybuffer.c
YOSAL_SRC_FILES += src/hash/lookup3.c
YOSAL_SRC_FILES += src/hash/fnv1.c
YOSAL_SRC_FILES += src/struct/array.c
YOSAL_SRC_FILES += src/struct/hashmap.c
YOSAL_SRC_FILES += src/struct/queue.c
YOSAL_SRC_FILES += src/struct/yobject.c
YOSAL_SRC_FILES += src/digest/digest_md5.c
YOSAL_SRC_FILES += src/digest/digest_sha1.c
YOSAL_SRC_FILES += src/digest/digest.c
YOSAL_SRC_FILES += src/io/ychannel.c
YOSAL_SRC_FILES += src/io/engine/fd.c
YOSAL_SRC_FILES += src/io/engine/file.c
YOSAL_SRC_FILES += src/io/engine/javastream.c
YOSAL_SRC_FILES += src/java/jniutils.c
YOSAL_SRC_FILES += src/system/log.c
YOSAL_SRC_FILES += src/system/system.c
YOSAL_SRC_FILES += src/system/time.c
YOSAL_SRC_FILES += src/system/yrandom.c
YOSAL_SRC_FILES += src/test/ytest.c

YOSAL_C_INCLUDES := $(LOCAL_PATH)/include

YOSAL_CFLAGS := -Wall -Werror

LOCAL_MODULE:= libyahoo_yosal
LOCAL_MODULE_TAGS := optional

LOCAL_SRC_FILES := $(YOSAL_SRC_FILES)
LOCAL_CFLAGS := $(YOSAL_CFLAGS)
LOCAL_C_INCLUDES := $(YOSAL_C_INCLUDES)

# Disable prelink if trying to build within AOSP tree
LOCAL_PRELINK_MODULE := false

# Building in ARM32 mode improve performance, while adding very little
# overhead on size of library since this module is very compact
LOCAL_ARM_MODE := arm

# For the static library to be linked within a shared library, need
# to compile it with PIC (Position Independant Code) enabled
LOCAL_CFLAGS += -fPIC -DPIC

ifneq ($(NDK_ROOT),)
# If building with NDK, force using gold linker
LOCAL_LDLIBS += -fuse-ld=gold 
endif

include $(BUILD_STATIC_LIBRARY)

ifeq ($(BUILD_SUPPORT_EXECUTABLE),true)
include $(call first-makefiles-under,$(LOCAL_PATH))
endif
