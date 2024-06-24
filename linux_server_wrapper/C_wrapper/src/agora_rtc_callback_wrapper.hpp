#include "NGIAgoraRtcConnection.h"
#include "callback.pb-c.h"

static Agoracallback__TConnectionInfo proto_info;
typedef void (*c_callback)(void *protobuf, int size, int fun_num);

static void TConnectionInfoToProtobuf(
    const agora::rtc::TConnectionInfo &connectionInfo) {
  proto_info.id = connectionInfo.id;
  proto_info.channelid = (char *)connectionInfo.channelId.get()->c_str();
  proto_info.state = connectionInfo.state;
  proto_info.localuserid = (char *)connectionInfo.localUserId.get()->c_str();
  proto_info.internaluid = connectionInfo.internalUid;
  proto_info.proxytype = connectionInfo.proxyType;
  proto_info.connectionip = (char *)connectionInfo.connectionIp.get()->c_str();
}

class ConnectionObserverCWrapper : public agora::rtc::IRtcConnectionObserver,
                                   public agora::rtc::INetworkObserver {
 public:
  ConnectionObserverCWrapper() {
    agoracallback__tconnection_info__init(&proto_info);
  }

 public:  // IRtcConnectionObserver
  void onConnected(const agora::rtc::TConnectionInfo &connectionInfo,
                   agora::rtc::CONNECTION_CHANGED_REASON_TYPE reason) override {
    TConnectionInfoToProtobuf(connectionInfo);
    size_t length =
        agoracallback__tconnection_info__get_packed_size(&proto_info);
    void *buffer = malloc(length);
    agoracallback__tconnection_info__pack(&proto_info, (uint8_t *)buffer);
    if (fun) {
      fun(buffer, length, 1);
    }
    free(buffer);
  };
  void onDisconnected(
      const agora::rtc::TConnectionInfo &connectionInfo,
      agora::rtc::CONNECTION_CHANGED_REASON_TYPE reason) override {
    TConnectionInfoToProtobuf(connectionInfo);
    size_t length =
        agoracallback__tconnection_info__get_packed_size(&proto_info);
    void *buffer = malloc(length);
    agoracallback__tconnection_info__pack(&proto_info, (uint8_t *)buffer);
    if (fun) {
      fun(buffer, length, 2);
    }
    free(buffer);
  };
  void onConnecting(
      const agora::rtc::TConnectionInfo &connectionInfo,
      agora::rtc::CONNECTION_CHANGED_REASON_TYPE reason) override {
    TConnectionInfoToProtobuf(connectionInfo);
    size_t length =
        agoracallback__tconnection_info__get_packed_size(&proto_info);
    void *buffer = malloc(length);
    agoracallback__tconnection_info__pack(&proto_info, (uint8_t *)buffer);
    if (fun) {
      fun(buffer, length, 3);
    }
    free(buffer);
  };
  void onReconnecting(
      const agora::rtc::TConnectionInfo &connectionInfo,
      agora::rtc::CONNECTION_CHANGED_REASON_TYPE reason) override {
    TConnectionInfoToProtobuf(connectionInfo);
    size_t length =
        agoracallback__tconnection_info__get_packed_size(&proto_info);
    void *buffer = malloc(length);
    agoracallback__tconnection_info__pack(&proto_info, (uint8_t *)buffer);
    if (fun) {
      fun(buffer, length, 4);
    }
    free(buffer);
  };
  void onReconnected(
      const agora::rtc::TConnectionInfo &connectionInfo,
      agora::rtc::CONNECTION_CHANGED_REASON_TYPE reason) override {
    TConnectionInfoToProtobuf(connectionInfo);
    size_t length =
        agoracallback__tconnection_info__get_packed_size(&proto_info);
    void *buffer = malloc(length);
    agoracallback__tconnection_info__pack(&proto_info, (uint8_t *)buffer);
    if (fun) {
      fun(buffer, length, 5);
    }
    free(buffer);
  };
  void onConnectionLost(
      const agora::rtc::TConnectionInfo &connectionInfo) override {
    TConnectionInfoToProtobuf(connectionInfo);
    size_t length =
        agoracallback__tconnection_info__get_packed_size(&proto_info);
    void *buffer = malloc(length);
    agoracallback__tconnection_info__pack(&proto_info, (uint8_t *)buffer);
    if (fun) {
      fun(buffer, length, 6);
    }
    free(buffer);
  };
  void onLastmileQuality(const agora::rtc::QUALITY_TYPE quality) override {}
  void onTokenPrivilegeWillExpire(const char *token) override {}
  void onTokenPrivilegeDidExpire() override {}
  void onConnectionFailure(
      const agora::rtc::TConnectionInfo &connectionInfo,
      agora::rtc::CONNECTION_CHANGED_REASON_TYPE reason) override {}
  void onUserJoined(agora::user_id_t userId) override{};
  void onUserLeft(agora::user_id_t userId,
                  agora::rtc::USER_OFFLINE_REASON_TYPE reason) override{};
  void onTransportStats(const agora::rtc::RtcStats &stats) override {}
  void onLastmileProbeResult(
      const agora::rtc::LastmileProbeResult &result) override {}
  void onChannelMediaRelayStateChanged(int state, int code) override {}

 public:  // INetworkObserver
  void onUplinkNetworkInfoUpdated(
      const agora::rtc::UplinkNetworkInfo &info) override{};

  void setCallbackFun(c_callback f){
    fun = f;
  }

  private:
    c_callback fun = nullptr;

};
