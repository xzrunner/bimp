INNER_SAVED_LOCAL_PATH := $(LOCAL_PATH)

LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := bimp

LOCAL_C_INCLUDES := \
	${BIMP_SRC_PATH}/include \
	${BIMP_SRC_PATH}/include/bimp \
	${CU_SRC_PATH} \
	${LOGGER_SRC_PATH} \
	${FS_SRC_PATH} \
	${LZMA_SRC_PATH} \

LOCAL_SRC_FILES := \
	$(subst $(LOCAL_PATH)/,,$(shell find $(LOCAL_PATH) -name "*.cpp" -print)) \
	
include $(BUILD_STATIC_LIBRARY)	

LOCAL_PATH := $(INNER_SAVED_LOCAL_PATH)