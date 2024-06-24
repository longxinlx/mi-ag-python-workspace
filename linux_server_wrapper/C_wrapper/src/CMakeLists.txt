cmake_minimum_required(VERSION 2.4)

project(AgoraSDK_C_wrapper)

set(CMAKE_CXX_FLAGS "-g")

SET(AgoraSDK_C_WRAPPER_SRC
"agora_rtc_proxy.cpp"
"agora_rtc_wrapper.cpp"
"agora_rtc_callback_wrapper.hpp"
"callback.pb-c.c")

include_directories("./../../agora_sdk/include")
include_directories("./../../agora_sdk/include/protobuf-c")



find_library(LIB_RTC_SDK NAMES agora_rtc_sdk PATHS "./../../agora_sdk/" NO_DEFAULT_PATH REQUIRED)
link_libraries(${LIB_RTC_SDK})

find_library(LIB_PROTO_LIB NAMES protobuf-c PATHS "./../../agora_sdk/" NO_DEFAULT_PATH REQUIRED)
link_libraries(${LIB_PROTO_LIB})

find_library(LIB_JSON_LIB NAMES jsoncpp PATHS "./../../agora_sdk/" NO_DEFAULT_PATH REQUIRED)
link_libraries(${LIB_JSON_LIB})

ADD_LIBRARY(agora_rtc_c SHARED ${AgoraSDK_C_WRAPPER_SRC})