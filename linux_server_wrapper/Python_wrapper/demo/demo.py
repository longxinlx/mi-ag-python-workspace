# python3.6

#  PYTHONPATH=../../agora_sdk/  LD_LIBRARY_PATH=../../agora_sdk/   python3.6 demo.py
import time
import ctypes
import libagora_rtc_python

file = open('demo.pcm', 'rb')

videoFile = open("demo.yuv","rb")
# file1 = open('3.pcm', 'wb')


def agora_callback_fun(buf,num):
    print("call back num is:",num,type(buf),len(buf),str(buf))

def agora_audio_frame_handler(audiobuf,uid):
    pass
    # print("call back num is:",uid,type(audiobuf),len(audiobuf))
    # file1.write(audiobuf)

libagora_rtc_python.init_agora_engine("../../config/rtc_config.json")

libagora_rtc_python.registerAgoraCallback(agora_callback_fun)

libagora_rtc_python.registerAgoraAudioFrameHandler(agora_audio_frame_handler)


libagora_rtc_python.joinAgoraChannel()


buffer_size = 320
buffer = ctypes.create_string_buffer(buffer_size)

i = 0
while i < 100:
    
    AudioData = file.read(320)
    libagora_rtc_python.send_pcm_audio(AudioData,320,0)
    AudioData = file.read(320)
    libagora_rtc_python.send_pcm_audio(AudioData,320,0)
    AudioData = file.read(320)
    libagora_rtc_python.send_pcm_audio(AudioData,320,0)
    AudioData = file.read(320)
    libagora_rtc_python.send_pcm_audio(AudioData,320,0)
    VideoData = videoFile.read(352*144*3)
    libagora_rtc_python.send_yuv_video(VideoData,352,288,0)
    time.sleep(0.04)
    i = i + 1

libagora_rtc_python.leaveAgoraChannel()

time.sleep(1)


libagora_rtc_python.destroy_agora_engine()





