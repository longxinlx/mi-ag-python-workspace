#include <string>

#include "agora_rtc_audio_frame_observer_wrapper.hpp"
#include "agora_rtc_callback_wrapper.hpp"

#include "AgoraRefPtr.h"
#include "IAgoraService.h"
#include "NGIAgoraAudioTrack.h"
#include "NGIAgoraLocalUser.h"
#include "NGIAgoraMediaNode.h"
#include "NGIAgoraMediaNodeFactory.h"
#include "NGIAgoraRtcConnection.h"
#include "NGIAgoraVideoTrack.h"

class AgoraRtcProxy {
 public:
  static AgoraRtcProxy& getAgoraRtcProxy() {
    static AgoraRtcProxy proxy_;
    return proxy_;
  }

  AgoraRtcProxy(const AgoraRtcProxy&) = delete;
  AgoraRtcProxy& operator=(const AgoraRtcProxy&) = delete;

  int init(const char* config_path);
  int joinChannel();
  int leaveChannel();
  int SendPcmAudio(const char* buffer10ms, int bufferSize, int pts);
  int SendVideo(const char* buffer, int width, int height, int pts);

  std::shared_ptr<ConnectionObserverCWrapper> getConnectObserver() {
    return connection_observer;
  }
  std::shared_ptr<AudioFrameObserverCWrapper> getAudioFrameObserver() {
    return audio_frame_observer;
  }

  int destroy();

 private:
  AgoraRtcProxy();
  ~AgoraRtcProxy();

  std::string config_path_;
  int parser_json_config();
  int64_t covert_ts_for_agora(int ts);
  //  rtc config
  std::string log_path_;
  std::string token_;
  std::string channel_name_;
  std::string user_id_;
  bool subscribe_all_video_;
  bool subscribe_all_audio_;
  bool use_string_uid_;

  //  audio  config
  int audio_send_sample_rate_;
  int audio_send_channel_nums_;
  int audio_rev_sample_rate_;
  int audio_rev_channel_nums_;

  //  video  config
  int width_;
  int height_;
  int fps_;
  int targetBitrate_;


  //
  int first_ts = -1;
  int64_t offset_ts;
  //   agora class
  agora::base::IAgoraService* service_;
  agora::agora_refptr<agora::rtc::IRtcConnection> connection_;
  agora::agora_refptr<agora::rtc::IMediaNodeFactory> factory_;
  agora::agora_refptr<agora::rtc::IAudioPcmDataSender> audioPcmDataSender_;
  agora::agora_refptr<agora::rtc::IVideoFrameSender> videoFrameSender_;
  agora::agora_refptr<agora::rtc::ILocalVideoTrack> customVideoTrack_;
  agora::agora_refptr<agora::rtc::ILocalAudioTrack> customAudioTrack_;
  std::shared_ptr<ConnectionObserverCWrapper> connection_observer;
  std::shared_ptr<AudioFrameObserverCWrapper> audio_frame_observer;

  //   agora::agora_refptr<agora::rtc::ILocalVideoTrack> customVideoTrack_;
};