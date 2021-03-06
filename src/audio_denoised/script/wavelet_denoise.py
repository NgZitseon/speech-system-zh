#!/usr/bin/env python
#-*- coding:utf-8 -*-


import rospy,pywt
import numpy as np
from audio_msgs.msg import AudioData

__channels = 1
__framerate = 16000
__nframes = 512
__sampwidth = 16

Lambda = ''
threshold_value = ''

def read_audiodata(data):
    audio_data_array = np.array(data)
    return audio_data_array

#小波分解，小波sym8，层数3
def wavelet_dec(data):
    wd_list = pywt.wavedec(data, 'sym8', level=3)
    return wd_list

#小波重构，小波sym8
def wavelet_rec(data):
    wr_list = pywt.waverec(data, 'sym8')
    return wr_list

#小波降噪阈值
def threshold(data_len, data):
    Lambda = np.std(data) * (np.sqrt(2 * np.log10(data_len)))
    return Lambda


########################################
#  软阈值函数                           #
#      | sign(w)(|w| - λ) , |w| >= λ   #
#  W = |                               #
#      | 0 , |w| < λ                   #
########################################
def threshold_func(value, Lambda):
    if abs(value) < Lambda:
            threshold_value = 0
    elif abs(value) >= Lambda:
            threshold_value = np.sign(value) * (abs(value) - Lambda)
    return threshold_value


def denoise(data):
    z = 0
    for i in data:
        k = 0

        if z == 0:
            z += 1
            continue


        Lambda = threshold(len(i), i)

        for ii in i:
            i[k] = threshold_func(ii, Lambda)
            k += 1
        data[z] = i
        z += 1
    return data

def ros_pub(data):
    pub = rospy.Publisher('denoised_data', AudioData, queue_size=50)
    pub.publish(data.astype(np.short), len(data))

def callback(data):
    dec_data = wavelet_dec(data.data)
    dn_data = denoise(dec_data)
    rec_data = wavelet_rec(dn_data)
    ros_pub(rec_data)

def ros_sub():
    rospy.Subscriber('audio_data', AudioData, callback)
    rospy.spin()


if __name__ == '__main__':
    rospy.init_node('audio_denoise', anonymous=True)
    ros_sub()
