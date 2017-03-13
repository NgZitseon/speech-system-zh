#include <ros/ros.h>
#include "audio_msgs/AudioData.h"
#include "std_msgs/String.h"
#include "std_msgs/Int16.h"
#include <istream>
#include <stdio.h>

using namespace std;


class xf_awake
{
public:
  xf_awake()
  {

    sub = n.subscribe("asr_server/xf/local_s_res", 50 ,awakecallback);
    sub_tts = n.subscribe("tts_server/baidu_tts_pid", 50, ttsPIDcallback);
    sub_nlu = n.subscribe("nlu_server/tuling_pid",50,nluPIDcallback);
    //sub_play = n.subscribe("play_server/play_pid",50,playPIDcallback);
    ros::spin();
  }

  static void awakecallback(const std_msgs::String& msg)
  {
    if((tts_count==0)&&(nlu_count==0)&&(play_count==0)&&(msg.data!="Null"))
    {
      system("rosrun tts_server baidu_tts.py &");
      system("rosrun nlu_server tuling_nlu.py &");
      //system("rosrun audio_play audio_play &");
    }
   }

  static void ttsPIDcallback(const std_msgs::Int16& tts_pid)
  {

    char tts_kill_cmd[30];
    sprintf(tts_kill_cmd, "kill %d", tts_pid);
    tts_count += 1;
    cout<<tts_count<<endl;

    if(tts_count == 20)
    {
      system(tts_kill_cmd);
      tts_count = 0;
    }
  }

  static void nluPIDcallback(const std_msgs::Int16& nlu_pid)
  {
    char nlu_kill_cmd[30];
    sprintf(nlu_kill_cmd, "kill %d", nlu_pid);
    nlu_count += 1;

    if(nlu_count == 20)
    {
      system(nlu_kill_cmd);
      nlu_count = 0;
    }
  }

//  static void playPIDcallback(const std_msgs::Int16& play_pid)
//  {
//   char play_kill_cmd[30];
//   sprintf(play_kill_cmd,"kill %d", play_pid);
//   play_count += 1;

//   if(play_count == 10)
//   {
//     system(play_kill_cmd);
//     play_count = 0;
//   }
//  }

private:

  static int tts_count;
  static int nlu_count;
  static int play_count;
  ros::NodeHandle n;
  ros::Subscriber sub;
  ros::Subscriber sub_tts;
  ros::Subscriber sub_nlu;
  ros::Subscriber sub_play;

};

int xf_awake::tts_count = 0;
int xf_awake::nlu_count = 0;
int xf_awake::play_count = 0;




int main(int argc, char **argv)
{
  ros::init(argc, argv, "xf_awake");
  xf_awake awake;

  return 0;
}
