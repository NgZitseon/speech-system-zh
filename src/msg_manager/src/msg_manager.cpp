#include <ros/ros.h>
#include <std_msgs/Int8.h>
#include <std_msgs/String.h>
#include "./../include/msg_manager/msg_type.h"

class msg_manager
{
public:
  msg_manager()
  {
    sys_status.data = SYSTEM_STATUS_IS_FREE;
    _pub1 = _nh.advertise<std_msgs::Int8>("/msg_manager/activity",10);
    _pub2 = _nh.advertise<std_msgs::String>("/tts_server/baidu_tts",10);
    _pub3 = _nh.advertise<std_msgs::String>("camera_on",10);
  }

  static void system_status(const std_msgs::Int8 &msg1)
  {
    if(msg1.data == SYSTEM_STATUS_IS_FREE)
    {
      sys_status.data = SYSTEM_STATUS_IS_FREE;
      _pub1.publish(sys_status);
    }
    else if(msg1.data == SYSTEM_STATUS_NOT_FREE)
    {
      sys_status.data = SYSTEM_STATUS_NOT_FREE;
      _pub1.publish(sys_status);
    }
    else if (msg1.data == SYSTEM_STATUS_WAKEUP_NO)
    {
      sys_status.data = SYSTEM_STATUS_IS_FREE;
      _pub1.publish(msg1);
    }
  }

  static void asr_result_msg(const std_msgs::Int8 &msg2)
  {
    std_msgs::String files;
    if((msg2.data == SYSTEM_STATUS_LEARNING_ON)&&(sys_status.data==SYSTEM_STATUS_IS_FREE))
    {
      files.data = AUDIO_OF_LEARNING_ON;
      _pub2.publish(files);
      sys_status.data==SYSTEM_STATUS_NOT_FREE;
    }
    if((msg2.data == SYSTEM_STATUS_CAMERA_ON)&&(sys_status.data==SYSTEM_STATUS_IS_FREE))
    {
      std_msgs::String wakeup;
      wakeup.data = "y";
      files.data = AUDIO_OF_CAMERA_ON;
      _pub2.publish(files);
      _pub3.publish(wakeup);
      sys_status.data==SYSTEM_STATUS_NOT_FREE;
    }
    if((msg2.data == SYSTEM_STATUS_WAKEUP_NO)&(sys_status.data==SYSTEM_STATUS_IS_FREE))
    {
      std_msgs::String sleep;
      sleep.data = "n";
      files.data = AUDIO_OF_SLEEP;
      _pub2.publish(files);
      _pub3.publish(sleep);
      sys_status.data==SYSTEM_STATUS_NOT_FREE;
    }
  }

  void run()
  {
    _sub1 = _nh.subscribe("local_asr_result_msg",50,asr_result_msg);
    _sub2 = _nh.subscribe("system_status",50,system_status);
    ros::spin();
  }

private:
  static std_msgs::Int8 sys_status;
  static bool _if_activity;
  ros::NodeHandle _nh;
  ros::Subscriber _sub1;
  ros::Subscriber _sub2;
  static ros::Publisher _pub1;
  static ros::Publisher _pub2;
  static ros::Publisher _pub3;
};
std_msgs::Int8 msg_manager::sys_status;
ros::Publisher msg_manager::_pub1;
ros::Publisher msg_manager::_pub2;
ros::Publisher msg_manager::_pub3;
int main(int argc, char **argv)
{
  // Set up ROS.
  ros::init(argc, argv, "msg_manager");
  msg_manager msg;
  msg.run();

}
