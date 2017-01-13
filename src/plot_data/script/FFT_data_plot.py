#!/usr/bin/env python
#-*- coding: utf-8 -*-

import rospy,os
from audio_msgs.msg import AudioData
import pylab as pl
import matplotlib.animation as animation 
import matplotlib.lines as line
from multiprocessing import Queue
import numpy as np


block_size = 512
fig = pl.figure()

rt_ax = pl.subplot(211,xlim=(0,1),ylim=(-5000,5000))
fft_ax = pl.subplot(212)
fft_ax.set_xlim(0,8000)
fft_ax.set_ylim(-50,50)
rt_ax.set_title('Real Time')
fft_ax.set_title('FFT')

rt_line = line.Line2D([], [])
fft_line = line.Line2D([], [])

rt_x_data = np.linspace(0,1,block_size)
fft_x_data = np.linspace(0,8000,block_size/2+1)


def init():
    rt_ax.add_line(rt_line)
    fft_ax.add_line(fft_line)
    return rt_line,fft_line
    
def animate(i): 
    data = q.get()

    rt_data = data
    fft_data = np.fft.rfft(data)/block_size
    fft_data = 20*np.log10(np.clip(np.abs(fft_data), 1e-20, 1e100))

    rt_line.set_xdata(rt_x_data)
    fft_line.set_xdata(fft_x_data)
    rt_line.set_ydata(rt_data) 
    fft_line.set_ydata(fft_data)

    return rt_line, fft_line

def callback(value):
    q.put(value.data)
        
def listener():
    rospy.init_node('captrue_plot', anonymous=True)
    rospy.Subscriber('audio_data',AudioData, callback)
    rospy.spin()

if __name__ == '__main__':
    
    q = Queue()
    pid = os.fork()
    if pid:
        listener()
    else:
        ani = animation.FuncAnimation(fig, animate, init_func=init, interval=1)
        pl.show()
    
    
   
