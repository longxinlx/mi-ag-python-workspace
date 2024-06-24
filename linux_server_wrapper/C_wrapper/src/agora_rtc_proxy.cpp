#include "agora_rtc_proxy.h"
#include <fstream>
#include <iostream>
#include "json/json.h"

#define DEFAULT_LOG_SIZE (512 * 1024)  // default log size is 512 kb

AgoraRtcProxy::AgoraRtcProxy() {}
AgoraRtcProxy::~AgoraRtcProxy() {}

int AgoraRtcProxy::init(const char *config_path) {
  config_path_ = std::string(config_path);
  int r = parser_json_config();
  if (r) {
    return r;
  }
  service_ = createAgoraService();
  agora::base::AgoraServiceConfiguration scfg;
  scfg.appId = token_.c_str();
  scfg.enableAudioProcessor = true;
  scfg.enableAudioDevice = false;
  scfg.enableVideo = false;
  scfg.useStringUid = use_string_uid_;

  if (service_->initialize(scfg) != agora::ERR_OK) {
    return -1;
  }

  if (service_->setLogFile(log_path_.c_str(), DEFAULT_LOG_SIZE) != 0) {
    return -1;
  }

  agora::rtc::RtcConnectionConfiguration ccfg;
  ccfg.autoSubscribeAudio = subscribe_all_audio_;
  ccfg.autoSubscribeVideo = subscribe_all_video_;
  ccfg.clientRoleType = agora::rtc::CLIENT_ROLE_BROADCASTER;
  connection_ = service_->createRtcConnection(ccfg);

  if (!connection_) {
    return -1;
  }

  factory_ = service_->createMediaNodeFactory();

  if (!factory_) {
    return -1;
  }
  audioPcmDataSender_ = factory_->createAudioPcmDataSender();
  customAudioTrack_ = service_->createCustomAudioTrack(audioPcmDataSender_);

  videoFrameSender_ = factory_->createVideoFrameSender();
  customVideoTrack_ = service_->createCustomVideoTrack(videoFrameSender_);

  connection_observer = std::make_shared<ConnectionObserverCWrapper>();
  connection_->registerObserver(connection_observer.get());
  audio_frame_observer = std::make_shared<AudioFrameObserverCWrapper>();
}

int AgoraRtcProxy::destroy() {
  connection_->unregisterObserver(connection_observer.get());

  connection_observer.reset();

  audioPcmDataSender_ = nullptr;
  videoFrameSender_ = nullptr;
  customAudioTrack_ = nullptr;
  customVideoTrack_ = nullptr;
  factory_ = nullptr;
  connection_ = nullptr;

  // Destroy Agora Service
  service_->release();
  service_ = nullptr;
}

int AgoraRtcProxy::joinChannel() {
  if (connection_->connect(token_.c_str(), channel_name_.c_str(),
                           user_id_.c_str())) {
    return -1;
  }
  connection_->getLocalUser()->setPlaybackAudioFrameBeforeMixingParameters(
      audio_rev_channel_nums_, audio_rev_sample_rate_);
  connection_->getLocalUser()->registerAudioFrameObserver(
      audio_frame_observer.get());
    // Configure video encoder
  agora::rtc::VideoEncoderConfiguration encoderConfig;
  encoderConfig.codecType = agora::rtc::VIDEO_CODEC_H264;
  encoderConfig.dimensions.width = width_;
  encoderConfig.dimensions.height = height_;
  encoderConfig.frameRate = fps_;
  encoderConfig.bitrate = targetBitrate_;

  customVideoTrack_->setVideoEncoderConfiguration(encoderConfig);

  customAudioTrack_->setEnabled(true);
  connection_->getLocalUser()->publishAudio(customAudioTrack_);
  customVideoTrack_->setEnabled(true);
  connection_->getLocalUser()->publishVideo(customVideoTrack_);
}

int AgoraRtcProxy::SendPcmAudio(const char *buffer10ms, int bufferSize,
                                int pts) {
  int samplesPer10ms = audio_send_sample_rate_ / 100;

  int r = audioPcmDataSender_->sendAudioPcmData(
      buffer10ms, covert_ts_for_agora(pts), 0, samplesPer10ms, agora::rtc::TWO_BYTES_PER_SAMPLE,
      audio_send_channel_nums_, audio_send_sample_rate_);
  // printf("SendPcmAudio %d \n",r);
  return r;
}

int AgoraRtcProxy::SendVideo(const char *buffer, int width, int height,
                             int pts) {
  agora::media::base::ExternalVideoFrame videoFrame;
  videoFrame.type =
      agora::media::base::ExternalVideoFrame::VIDEO_BUFFER_RAW_DATA;
  videoFrame.format = agora::media::base::VIDEO_PIXEL_I420;
  videoFrame.buffer = (void *)buffer;
  videoFrame.stride = width;
  videoFrame.height = height;
  videoFrame.cropLeft = 0;
  videoFrame.cropTop = 0;
  videoFrame.cropRight = 0;
  videoFrame.cropBottom = 0;
  videoFrame.rotation = 0;
  videoFrame.timestamp = covert_ts_for_agora(pts);
  videoFrameSender_->sendVideoFrame(videoFrame);
}

int AgoraRtcProxy::leaveChannel() {
  connection_->getLocalUser()->unpublishAudio(customAudioTrack_);
  connection_->getLocalUser()->unregisterAudioFrameObserver(
      audio_frame_observer.get());
  if (connection_->disconnect()) {
    return -1;
  }
}

int AgoraRtcProxy::parser_json_config() {
  Json::Reader reader;
  Json::Value root;
  std::ifstream ifs;
  ifs.open(config_path_.c_str());
  if (!ifs.is_open()) {
    std::cout << "open file error. " << config_path_ << std::endl;
    return -1;
  }

  if (!reader.parse(ifs, root)) {
    std::cout << "parse error" << std::endl;
    return -1;
  }

  //  rtc config
  log_path_ = root["LogPath"].asString();
  token_ = root["Token"].asString();
  channel_name_ = root["ChannelId"].asString();
  user_id_ = root["UserId"].asString();
  subscribe_all_video_ = false;
  subscribe_all_audio_ = root["AuidoConfig"].get("SubAullAuido", "").asBool();
  use_string_uid_ = false;

  audio_send_sample_rate_ =
      root["AuidoConfig"].get("AudioSendSampleRate", "").asInt();
  audio_send_channel_nums_ =
      root["AuidoConfig"].get("AudioSendChannelNum", "").asInt();
  audio_rev_sample_rate_ =
      root["AuidoConfig"].get("AudioRevSampleRate", "").asInt();
  audio_rev_channel_nums_ =
      root["AuidoConfig"].get("AudioRevChannelNum", "").asInt();
  width_ = root["VideoConfig"].get("width", "").asInt();
  height_ = root["VideoConfig"].get("height", "").asInt();
  fps_ = root["VideoConfig"].get("fps", "").asInt();
  targetBitrate_ = root["VideoConfig"].get("bitrate", "").asInt();
}

int64_t AgoraRtcProxy::covert_ts_for_agora(int ts){
  if(first_ts == -1){
    first_ts = ts;
    offset_ts = getAgoraCurrentMonotonicTimeInMs() - ts;
  }
  return ts + offset_ts;
}
