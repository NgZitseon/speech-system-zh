#!/usr/bin/env python
#-*- coding:utf-8 -*-
# tuling_nlu.py
# Created on: 2017年1月5日
#     Author: hntea

import TulingNLUClientManager as nlu
import sys
import rospy,os
from std_msgs.msg import String
from std_msgs.msg import Int16

#
# 该模块尚未完整，还需要资源判别请求
#
class TulingNLU:
    def __init__(self):
        
        subtopic = rospy.get_param("~subtopic","/asr_server/xf/online_s_res")
        
        self.man = nlu.TulingNLUClientManager()
           
        rospy.init_node('nlu_tuling', anonymous=True)
        rospy.Subscriber(subtopic,String,self.process)
        self.pub = rospy.Publisher("nlu_server/tuling_res",String,queue_size=10)
        self.processid()
        rospy.spin()
    

    def process(self,data):
        print data.data
        if data.data != "Null":
            result = self.man.runNLU(data.data)
            rospy.loginfo('[TulinNLU: ]'+result)
            self.pub.publish(result)

    def processid(self):

        self.pubpid = rospy.Publisher("nlu_server/tuling_pid", Int16, queue_size=50)
        rate = rospy.Rate(1)
        while not rospy.is_shutdown():
            pid = os.getpid()
            rospy.loginfo("the pid of baidu_tts is %s" % pid)
            self.pubpid.publish(pid)
            rate.sleep()

if __name__ == "__main__":
    TulingNLU()