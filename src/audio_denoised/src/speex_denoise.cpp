#include <ros/ros.h>
#include <speex/speex_preprocess.h>
#include <audio_msgs/AudioData.h>

#define FRAME_SIZE 512
#define SAMPLING_RATE 16000
#define DB (-50) //maximun attenuation of the noise in db

class Speex
{
public:
  Speex()
  {
    _denoiseDB = DB;
    _denoise_state = 1;
    _agc_state = 0;
    _agcLevel = 24000;
    speex_preprocess_ctl(st, SPEEX_PREPROCESS_GET_DENOISE,&_denoise_state);
    speex_preprocess_ctl(st, SPEEX_PREPROCESS_SET_NOISE_SUPPRESS, &_denoiseDB);
    speex_preprocess_ctl(st, SPEEX_PREPROCESS_SET_AGC, &_agc_state);
    //speex_preprocess_ctl(st, SPEEX_PREPROCESS_GET_DENOISE, &j);
    speex_preprocess_ctl(st, SPEEX_PREPROCESS_SET_AGC_LEVEL, &_agcLevel);
    _pub = _nh.advertise<audio_msgs::AudioData>("denoised_data",1000);
  }


  static void denoiseCallback(const audio_msgs::AudioData &audio_data)
  {
    audio_msgs::AudioData denoise_data = audio_data;

    speex_preprocess_run(st,(int16_t*)&denoise_data.data[0]);
    _pub.publish(denoise_data);

  }

  void run_denoise()
  {
    _sb = _nh.subscribe("audio_data", 1000, denoiseCallback);

    ros::spin();
  }

private:
  int _denoise_state;
  int _denoiseDB;
  int _agcLevel;
  int _agc_state;
  ros::NodeHandle _nh;
  ros::Subscriber _sb;
  static ros::Publisher _pub;
  static SpeexPreprocessState *st;
};

ros::Publisher Speex::_pub;
SpeexPreprocessState *Speex::st=speex_preprocess_state_init(FRAME_SIZE, SAMPLING_RATE);


int main(int argc, char **argv)
{
  ros::init(argc, argv, "speex_denoise");
  Speex sp;
  sp.run_denoise();
}
