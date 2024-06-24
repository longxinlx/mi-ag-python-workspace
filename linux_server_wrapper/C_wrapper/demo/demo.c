#include "agora_rtc_wrapper.h"

#include <stdio.h>
#include <unistd.h>

// gcc demo.c  -I ../ -lagora_rtc_c  -L ../agora_sdk/ -g
void callback_printf(void *protobuf, int size, int fun_num){
    printf("fun_num is %d \n",fun_num);
}


int main() {

  init("../../config/rtc_config.json");
  registerCallback(callback_printf);

  joinChannel();

  static FILE* pcmFIle = NULL;
  pcmFIle = fopen("demo.pcm", "rb");

  char frameBuf[320];

  static FILE* yuvFIle = NULL;
  yuvFIle = fopen("demo.yuv", "rb");
  char yuvBuf[352*144*3];

for(int i = 0 ;i<100;i++){
  fread(frameBuf, 1, sizeof(frameBuf), pcmFIle);

   SendPcmAudio(frameBuf, 320 , 0);
  fread(frameBuf, 1, sizeof(frameBuf), pcmFIle);

   SendPcmAudio(frameBuf, 320 , 0);
  fread(frameBuf, 1, sizeof(frameBuf), pcmFIle);

   SendPcmAudio(frameBuf, 320 , 0);
  fread(frameBuf, 1, sizeof(frameBuf), pcmFIle);

   SendPcmAudio(frameBuf, 320 , 0);

  fread(yuvBuf, 1, sizeof(yuvBuf), yuvFIle);


  SendVideo(yuvBuf,352,288,0);

   usleep(40*1000);
}
  sleep(1);

 leaveChannel();
  sleep(1);

  destroy();
//  leaveChannel();

  return 0;
}