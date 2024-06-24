#!/bin/bash


gcc -shared -o libagora_rtc_python.so src/agora_cpython.c  -lagora_rtc_c  -L ../agora_sdk/ -lpython3.10 -fPIC

cp libagora_rtc_python.so  ../agora_sdk/

echo -e "\033[32m build libagora_rtc_python.so success\033[0m"

