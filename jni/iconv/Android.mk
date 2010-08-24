LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := iconv

APP_SUBDIRS :=  $(patsubst $(LOCAL_PATH)/%, %, $(shell find $(LOCAL_PATH)/srclib -type d))  $(patsubst $(LOCAL_PATH)/%, %, $(shell find $(LOCAL_PATH)/lib -type d))  $(patsubst $(LOCAL_PATH)/%, %, $(shell find $(LOCAL_PATH)/libcharset/lib -type d))

LOCAL_CFLAGS := $(foreach D, $(APP_SUBDIRS), -I$(LOCAL_PATH)/$(D)) \
				-I$(LOCAL_PATH)/include \
				-I$(LOCAL_PATH)/srclib \
				-I$(LOCAL_PATH)/libcharset/include \
				-DLIBDIR="libdir"
				


LOCAL_CPP_EXTENSION := .cpp

LOCAL_SRC_FILES := $(foreach F, $(APP_SUBDIRS), $(addprefix $(F)/,$(notdir $(wildcard $(LOCAL_PATH)/$(F)/*.cpp))))
LOCAL_SRC_FILES += $(foreach F, $(APP_SUBDIRS), $(addprefix $(F)/,$(notdir $(wildcard $(LOCAL_PATH)/$(F)/*.c))))

LOCAL_SHARED_LIBRARIES := 

LOCAL_STATIC_LIBRARIES := 

LOCAL_LDLIBS :=

include $(BUILD_SHARED_LIBRARY)
