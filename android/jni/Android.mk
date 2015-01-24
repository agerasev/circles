LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

include $(LOCAL_PATH)/../../build-info.mk

LOCAL_MODULE    := native-activity

LOCAL_CFLAGS    := $(_CFLAGS) $(_ANDROID_CFLAGS)
LOCAL_CPPFLAGS  := $(_CXXFLAGS) $(_ANDROID_CXXFLAGS)

LOCAL_LDLIBS    := $(_ANDROID_LIBS)
LOCAL_STATIC_LIBRARIES := android_native_app_glue

LOCAL_SRC_FILES := $(_ANDROID_SOURCES) $(_SOURCES)

LOCAL_C_INCLUDES += $(LOCAL_PATH)/source/
LOCAL_C_INCLUDES += ${ANDROID_NDK}/sources/cxx-stl/gnu-libstdc++/4.8/include

include $(BUILD_SHARED_LIBRARY)

$(call import-module,android/native_app_glue)
