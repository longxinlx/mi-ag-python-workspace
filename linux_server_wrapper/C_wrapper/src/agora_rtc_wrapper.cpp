#include "agora_rtc_wrapper.h"
#include "agora_rtc_proxy.h"

int init(const char* config_path){
    int r  = AgoraRtcProxy::getAgoraRtcProxy().init(config_path);
    if(r) {
      return r;
    }
}

int destroy(){
    return AgoraRtcProxy::getAgoraRtcProxy().destroy();
}

int joinChannel() {
  return AgoraRtcProxy::getAgoraRtcProxy().joinChannel();
}

int leaveChannel() { return AgoraRtcProxy::getAgoraRtcProxy().leaveChannel(); }

int SendPcmAudio(const char *buffer10ms, int bufferSize,int pts) {
  return AgoraRtcProxy::getAgoraRtcProxy().SendPcmAudio(buffer10ms, bufferSize,pts);
}

void registerCallback(c_callback f){
   if(AgoraRtcProxy::getAgoraRtcProxy().getConnectObserver()){
     AgoraRtcProxy::getAgoraRtcProxy().getConnectObserver()->setCallbackFun(f);
   }
}

void registerAudioFrameCallback(rtc_proxy_receive_pcm_handler f){
 if(AgoraRtcProxy::getAgoraRtcProxy().getAudioFrameObserver()){
     AgoraRtcProxy::getAgoraRtcProxy().getAudioFrameObserver()->setAudioFrameHandler(f);
   }
}

int SendVideo(const char *yuvbuffer, int width,int height , int pts){
  return AgoraRtcProxy::getAgoraRtcProxy().SendVideo(yuvbuffer, width,height,pts);
}