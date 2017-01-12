/*
 * audio_data_plot.cpp
 *
 *  Created on: 2016年12月7日
 *      Author: hntea
 */
#include <ros/ros.h>
#include "audio_msgs/AudioData.h"
#include "audio_msgs/PlotData.h"
#include <vector>
class RosAudioPlot{
public:
	RosAudioPlot(){
		_sb = _nh.subscribe("denoised_data",50,chapterCallback);
		_pub = _nh.advertise<audio_msgs::PlotData>("denoised_data_plot",5120);
		ros::spin();
	}
	~RosAudioPlot(){}

	static void  chapterCallback(const audio_msgs::AudioData &msgs){
		std::vector<int16_t> vec(msgs.data);
		vec.resize(msgs.data_size);
		std::vector<int16_t>::iterator iter = vec.begin();

		audio_msgs::PlotData plot;
		for(int i=0;i<msgs.data_size;i++){
			plot.src_data = iter[i];
			_pub.publish(plot);
			usleep(62.5);		//接收到的消息以16kHz发布
		}
	}

private:
	ros::NodeHandle _nh;
    ros::Subscriber _sb;
    static ros::Publisher _pub;
};
ros::Publisher RosAudioPlot::_pub;

int main (int argc, char **argv)
{
  ROS_INFO ("Ros Node Name : denoised_data_plot");
  ROS_INFO ("Ros Node Subscribe : denoised_data");
  ROS_INFO ("Ros Node Publish Topic : denoised_data_plot");

  ros::init(argc, argv, "denoised_data_plot");
  RosAudioPlot plot;
}
