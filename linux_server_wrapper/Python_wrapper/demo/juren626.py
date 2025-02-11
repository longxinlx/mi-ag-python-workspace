# python3.6

#  PYTHONPATH=../../agora_sdk/  LD_LIBRARY_PATH=../../agora_sdk/   python3.6 demo.py

import os
import queue
import subprocess
import threading
import time
import ctypes
import libagora_rtc_python
import cv2, librosa
import soundfile as sf
import numpy as np

file = open('test_16k1.wav', 'rb')
# file = open('demo.pcm', 'rb')
if os.path.exists('output_16k2.pcm'):
    print('存在')
else:
    print('不存在')
file = open('output_16k2.pcm', 'rb')
# videoFile = open("demo.yuv","rb")
# file1 = open('3.pcm', 'wb')
import re


def agora_callback_fun(buf, num):
    print("call back num is:", num, type(buf), len(buf), str(buf))


def agora_audio_frame_handler(audiobuf, uid):
    pass
    # print("call back num is:",uid,type(audiobuf),len(audiobuf))
    # file1.write(audiobuf)


# speak_voice = queue.Queue()
# def write_audio_data_to_pipe(audio_data, chunk_size):
#     start_idx = 0
#     while start_idx < audio_data.shape[0]:
#         if start_idx + chunk_size > audio_data.shape[0]:
#             af = audio_data[start_idx:audio_data.shape[0]]
#             start_idx = audio_data.shape[0]
#         else:
#             af = audio_data[start_idx:start_idx + chunk_size]
#             start_idx = start_idx + chunk_size
#         speak_voice.put(af.tobytes())


libagora_rtc_python.init_agora_engine("../../config/rtc_config.json")

libagora_rtc_python.registerAgoraCallback(agora_callback_fun)

libagora_rtc_python.registerAgoraAudioFrameHandler(agora_audio_frame_handler)

libagora_rtc_python.joinAgoraChannel()

buffer_size = 320
buffer = ctypes.create_string_buffer(buffer_size)
# stream, sr = sf.read("test_16k(1).wav", dtype='float32')

# stream, sr = librosa.load("test_16k(1).wav", sr=16000)


images = [os.path.join("./test_25fps", f)
          for f in
          os.listdir("./test_25fps") if
          f.endswith(('.bmp', '.jpg', '.jpeg', '.png'))]
# sorted_files =
# sorted(images, key=sort_key)
# print(images)
# data, samplerate = sf.read('test_16k(1).wav', dtype='int16')
data, samplerate = librosa.load("test_16k1.wav", sr=16000)
data = (data * 32768).astype(np.int16)


# padded_data = np.concatenate((data, np.zeros(len(images) * 4 - len(data) , dtype=data.dtype)))
# write_audio_data_to_pipe(data,160)
# print("=======================")
# print(speak_voice.qsize())
# print(len(images))
# print(len(data))
class Pacer:
    def __init__(self, interval):
        self.last_call_time = time.time()
        self.interval = interval

    def pace(self):
        current_time = time.time()
        elapsed_time = current_time - self.last_call_time
        if elapsed_time < self.interval:
            time.sleep(self.interval - elapsed_time)
        self.last_call_time = time.time()


# def send_video():
# while True:
# step = 19000 / 486  # 计算每次递增的值
# increment = 0  # 初始增量值为0
# pacer = Pacer(0.04)
# for i in range(486):
#     print(i)
#     image = cv2.imread("./test_25fps/{}.bmp".format(i + 1))
#     yuv = cv2.cvtColor(image, cv2.COLOR_BGR2YUV_I420)
#     yuv = yuv.reshape(720 * 1280 * 3 // 2)
#     libagora_rtc_python.send_yuv_video(yuv, 720, 1280, int(increment))
#     print(f"Increment: {increment}")
#     increment += step  # 每次递增
#     pacer.pace()
# time.sleep(0.01)  # 确保视频流的时间间隔

def send_video():
    step = 19000 / 486  # 计算每次递增的值
    increment = 0  # 初始增量值为0
    pacer = Pacer(0.04)
    for i in range(486):
        print(i)
        image = cv2.imread("./test_25fps/{}.bmp".format(i + 1))
        yuv = cv2.cvtColor(image, cv2.COLOR_BGR2YUV_I420)
        yuv = yuv.reshape(720 * 1280 * 3 // 2)
        libagora_rtc_python.send_yuv_video(yuv, 720, 1280, int(increment))
        print(f"Increment: {increment}")
        increment += step  # 每次递增
        pacer.pace()
        # time.sleep(0.01)  # 确保视频流的时间间隔


# def send_audio():
#     index = 0
#     increment = 10  # 音频帧的时间戳
#     while index * 160 < len(data):
#         print(index)
#         for i in range(4):
#             audio = data[index * 160: (index + 1) * 160]
#             libagora_rtc_python.send_pcm_audio(audio.tobytes(), 320, increment)
#             increment += 10
#             index += 1
#         time.sleep(0.04)  # 根据音频帧率调整

def send_audio():
    index = 0
    increment = 10  # 音频帧的时间戳
    while index * 160 < len(data):
        print(index)
        for i in range(4):
            audio = data[index * 160: (index + 1) * 160]
            libagora_rtc_python.send_pcm_audio(audio.tobytes(), 320, increment)
            increment += 10
            index += 1
        time.sleep(0.04)


video_thread = threading.Thread(target=send_video)
audio_thread = threading.Thread(target=send_audio)

# 启动线程
video_thread.start()
audio_thread.start()

# 等待线程结束
video_thread.join()
audio_thread.join()

# index = 0
# for i in range(486):
# # for image_path in images:
#     image = cv2.imread("./test_25fps/{}.bmp".format(i+1))
#     yuv = cv2.cvtColor(image, cv2.COLOR_BGR2YUV_I420)
#     yuv = yuv.reshape(720 * 1280 * 3 // 2)
#     libagora_rtc_python.send_yuv_video(yuv,720,1280,0)
#     # AudioData = file.read(320*4)
#     # libagora_rtc_python.send_pcm_audio(AudioData,320*4,0)
#     for i in range(4):
#         audio = data[index * 160 : (index + 1) * 160]
#         libagora_rtc_python.send_pcm_audio(audio.tobytes(),320,0)
#         index = index + 1
# libagora_rtc_python.send_pcm_audio(speak_voice.get(),320,0)
# libagora_rtc_python.send_pcm_audio(speak_voice.get(),320,0)
# libagora_rtc_python.send_pcm_audio(speak_voice.get(),320,0)
# AudioData = file.read(320)
# libagora_rtc_python.send_pcm_audio(AudioData,320,0)
# AudioData = file.read(320)
# libagora_rtc_python.send_pcm_audio(AudioData,320,0)
# AudioData = file.read(320)
# libagora_rtc_python.send_pcm_audio(AudioData,320,0)
# AudioData = file.read(320)
# libagora_rtc_python.send_pcm_audio(AudioData,320,0)
# time.sleep(0.04)
# i = 0
# while i < 100:

#     AudioData = file.read(320)
#     libagora_rtc_python.send_pcm_audio(AudioData,320,0)
#     AudioData = file.read(320)
#     libagora_rtc_python.send_pcm_audio(AudioData,320,0)
#     AudioData = file.read(320)
#     libagora_rtc_python.send_pcm_audio(AudioData,320,0)
#     AudioData = file.read(320)
#     libagora_rtc_python.send_pcm_audio(AudioData,320,0)
#     VideoData = videoFile.read(352*144*3)
#     libagora_rtc_python.send_yuv_video(VideoData,352,288,0)
#     time.sleep(0.04)
#     i = i + 1

libagora_rtc_python.leaveAgoraChannel()

time.sleep(1)

libagora_rtc_python.destroy_agora_engine()





