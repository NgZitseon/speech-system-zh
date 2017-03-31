#include <ros/ros.h>
#include <std_msgs/Bool.h>
class msg_manager
{
public:

  static void waitforcommand(const std_msgs::Bool &msg1)
  {
    if(!msg1.data)
    {
      _if_command = false;
    }
    else
    {
      _if_command = true;
    }
  }

  static void if_activity(const std_msgs::Bool &msg2)
  {
    if(!msg2.data||!_if_command)
    {
      _if_activity = false;
    }
    if(msg2.data&&_if_command)
    {
      _if_activity = true;
    }
  }

  void camera_ready()
  {

  }

  void robot_ready()
  {

  }

  void run()
  {
    _sub1 = _nh.subscribe("/asr_awaken/message",50,if_activity);
    _sub2 = _nh.subscribe("/xf_local_asr/message",50,waitforcommand);
  }

private:
  static bool _if_command;
  static bool _if_activity;
  ros::NodeHandle _nh;
  ros::Subscriber _sub1;
  ros::Subscriber _sub2;
};

bool msg_manager::_if_command = false;
bool msg_manager::_if_activity = true;
int main(int argc, char **argv)
{
  // Set up ROS.
  ros::init(argc, argv, "msg_manager");
  msg_manager msg;
  msg.run();

}
