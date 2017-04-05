#include "../../lib/xunfei/include/xfApiHead/msp_cmn.h"
#include "../../lib/xunfei/include/xfApiHead/msp_errors.h"
#include "../../lib/xunfei/include/xfApiHead/qivw.h"
#include "../../msg_manager/include/msg_manager/msg_type.h"
#include "audio_msgs/AudioData.h"
#include <std_msgs/String.h>
#include <std_msgs/Int8.h>
#include <stdio.h>
#include <iostream>
#include <vector>
#include <ros/ros.h>

using namespace std;

class awaken
{

public:

  awaken()
  {
    Login();
    session_id =  QIVWSessionBegin(NULL,ssb_param,&err_code);
    if(err_code != MSP_SUCCESS)
    {
      cout<<"QIVSessionBegin failed, error code is: "<<err_code<<endl;
    }
    _pub1 = _nh.advertise<audio_msgs::AudioData>("asr_awaken/audio_data",1000);
    _pub2 = _nh.advertise<std_msgs::Int8>("local_asr_result_msg",10);
    //_pub3 = _nh.advertise<std_msgs::String>("learning_on",10);
  }


//  static void remote_awake()
//  {
//    std_msgs::String wakeup;
//    wakeup.data = "wake_up";
//    ros::Rate loop_rate(1);
//    while(_status==SYSTEM_STATUS_WAKEUP_YES)
//    {
//      _pub2.publish(wakeup);
//     // _pub3.publish(wakeup);
//      loop_rate.sleep();
//      ros::spinOnce();
//    }
//  }

  void run()
  {
    err_code = QIVWRegisterNotify(session_id, cb_ivw_msg_proc, NULL);

    if(err_code != MSP_SUCCESS)
    {
      cout<<"QIVRegisterNotify failed! error code: "<<err_code<<endl;
    }

    _sb = _nh.subscribe("denoised_data",1000,awakecallback);
    _msg_sb = _nh.subscribe("/msg_manager/activity",50,msgManagerCallback);
    ros::spin();
  }

 static void msgManagerCallback(const std_msgs::Int8 &msg)
 {
   if(msg.data == SYSTEM_STATUS_IS_FREE)
   {
     _sysfree = SYSTEM_STATUS_IS_FREE;
   }
   else if(msg.data == SYSTEM_STATUS_NOT_FREE)
   {
     _sysfree = SYSTEM_STATUS_NOT_FREE;
   }
   else if(msg.data == SYSTEM_STATUS_WAKEUP_NO)
   {
     _status = SYSTEM_STATUS_WAKEUP_NO;
   }
 }

 static void awakecallback(const audio_msgs::AudioData &data)
  { 


    if(_status == SYSTEM_STATUS_WAKEUP_NO)
    {
      vector<int16_t> audio_vec(data.data);
      run_ivw(NULL,audio_vec);
    }
    else if((_status == SYSTEM_STATUS_WAKEUP_YES)&&(_sysfree==SYSTEM_STATUS_IS_FREE))
    {
      _pub1.publish(data);
    }
//    cout<<countNum++<<endl;

  }

  static int cb_ivw_msg_proc( const char *sessionID, int msg, int param1, int param2, const void *info, void *userData )
  {
    char* rinfo = (char*)info;
    cout<<"cb_ivw_msg_proc"<<endl;
    if(MSP_IVW_MSG_ERROR == msg)
    {

       cout<<"MSP_IVW_MSG_ERROR errCode = "<<param1<<endl;
    }
    else if(MSP_IVW_MSG_WAKEUP == msg)
    {
      cout<<"MSP_IVW_MSG_WAKEUP result = "<<rinfo<<endl;
      _status = SYSTEM_STATUS_WAKEUP_YES;
      std_msgs::Int8 camera_status;
      camera_status.data = SYSTEM_STATUS_CAMERA_ON;
      //remote_awake();
      _pub2.publish(camera_status);

    }
    return 0;
  }

  static void run_ivw(const char *grammar_list, vector<int16_t>& audio_data)
  {

    int audio_status = 2;

    err_code = QIVWAudioWrite(session_id,(const void*)&audio_data[0],audio_data.size()*2,audio_status);
    if(err_code != MSP_SUCCESS)
    {
      cout<<"QIVWAudioWrite failed, error code is: "<<err_code<<endl;

    }

  }


  void Login()
  {
    const char* params = "appid=58b3c9de, engine_start=ivw, ivw_res_path=fo|/home/saki/.SpeechSystem/xf-source/asr-local-model/asr/wakeupresource.jet";

    int ret = MSPLogin(NULL,NULL,params);
    if(ret != MSP_SUCCESS)
    {
      cout<<"MSPLogin failed, error code is: "<<ret<<endl;
    }
  }



private:
  static int _status;
  static int _sysfree;
  ros::NodeHandle _nh;
  ros::Subscriber _sb;
  ros::Subscriber _msg_sb;
  static ros::Publisher _pub1;
  static ros::Publisher _pub2;
  //static ros::Publisher _pub3;
  static int audio_count; 
  static int err_code;
  static const char* session_id;
  static const char *ssb_param;
  static int countNum;
};
int awaken::_sysfree = SYSTEM_STATUS_IS_FREE;
int awaken::_status = 0;
int awaken::countNum = 0;
int awaken::err_code = 0;
int awaken::audio_count = 0;
const char* awaken::session_id = NULL;
const char* awaken::ssb_param = "ivw_threshold=0:-20,sst=wakeup";
ros::Publisher awaken::_pub1;
ros::Publisher awaken::_pub2;
//ros::Publisher awaken::_pub3;


int main(int argc, char *argv[])
{
  ros::init(argc, argv, "xf_local_awaken");
  awaken awake;
  awake.run();

  return 0;
}

