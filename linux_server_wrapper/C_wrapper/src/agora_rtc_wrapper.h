#ifdef __cplusplus
extern "C" {
#endif

typedef void (*c_callback)(void *protobuf, int size, int fun_num);

typedef void (*rtc_proxy_receive_pcm_handler)(const char *userId, int samplesPerSec, int channel_num, const char *buffer10ms, int bufferSize);

int init(const char* config_path);

int destroy();

int joinChannel();

int leaveChannel();

int SendPcmAudio(const char *buffer10ms, int bufferSize,int pts);

void registerCallback(c_callback f);

void registerAudioFrameCallback(rtc_proxy_receive_pcm_handler f);

// int SendEncodedImage(const char *imagebuffer, int bufferSize);

int SendVideo(const char *yuvbuffer, int width,int height , int pts);

// static rtc_proxy_receive_pcm_handler receiveHandler;

#ifdef __cplusplus
}
#endif