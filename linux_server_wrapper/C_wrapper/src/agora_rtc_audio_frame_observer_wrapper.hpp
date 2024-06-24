
#include "NGIAgoraMediaNode.h"

#include "agora_rtc_wrapper.h"

class AudioFrameObserverCWrapper : public agora::media::IAudioFrameObserverBase {
 public:
  AudioFrameObserverCWrapper() {}

  bool onPlaybackAudioFrame(const char* channelId,
                            AudioFrame& audioFrame) override {
    return true;
  };

  bool onRecordAudioFrame(const char* channelId,
                          AudioFrame& audioFrame) override {
    return true;
  };

  bool onMixedAudioFrame(const char* channelId,
                         AudioFrame& audioFrame) override {
    return true;
  };

  bool onPlaybackAudioFrameBeforeMixing(const char* channelId,
                                        agora::media::base::user_id_t userId,
                                        AudioFrame& audioFrame) override {
    if (handler_) {
      handler_(userId, audioFrame.samplesPerSec, audioFrame.channels,
               (char*)audioFrame.buffer,
               audioFrame.samplesPerSec * audioFrame.channels / 100 * 2);
    }
  }

  int getObservedAudioFramePosition() override { return 0; };

  bool onEarMonitoringAudioFrame(AudioFrame& audioFrame) override {
    return true;
  };

  AudioParams getEarMonitoringAudioParams() override { return AudioParams(); };

  AudioParams getPlaybackAudioParams() override { return AudioParams(); };

  AudioParams getRecordAudioParams() override { return AudioParams(); };

  AudioParams getMixedAudioParams() override { return AudioParams(); };

  void setAudioFrameHandler(rtc_proxy_receive_pcm_handler handler){
      handler_ = handler;};

 private:
  rtc_proxy_receive_pcm_handler handler_ = nullptr;
};
