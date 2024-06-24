#!/bin/bash

rm -rf build
mkdir ./build
cd build
cmake ../src
make
cp libagora_rtc_c.so ../../agora_sdk/
cd ..
echo -e "\033[32m build libagora_rtc_c.so success\033[0m"

cd ./demo
gcc demo.c  -I ../src -lagora_rtc_c -lagora-fdkaac -lpthread -L ../../agora_sdk/ -o c_demo
echo -e "\033[32m build c_demo success\033[0m"
