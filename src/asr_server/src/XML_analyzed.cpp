#include <ros/ros.h>
#include <stdio.h>
#include "../../lib/tinyxml2/tinyxml2.h"
#include "../../msg_manager/include/msg_manager/msg_type.h"
#include "std_msgs/String.h"
#include "std_msgs/Int8.h"

using namespace std;
using namespace tinyxml2;
ros::Publisher pub;
ros::Publisher pub1;
ros::Publisher pub2;

string XMLanalyzed(const char* XMLdata)
{
  XMLDocument awk;
  awk.Parse(XMLdata,strlen(XMLdata));
  string id;
  XMLElement *root=awk.RootElement();
  XMLElement *result=root->FirstChildElement("result");
  XMLElement *object=result->FirstChildElement("object");
  XMLElement *rawtext=root->FirstChildElement("rawtext");
  XMLElement *child1=object->FirstChildElement();

  const char * value = rawtext->GetText();
  cout<<"Command : "<<value<<endl;
  //cout<<XMLdata<<endl;
  for(;child1;child1=child1->NextSiblingElement())
  {
    const XMLAttribute *attribute=child1->FirstAttribute();
    id.append(attribute->Value());
  //  cout<<*id<<endl;
  }
  return id;
}

void callback(const std_msgs::String &msg)
{
  if(strcmp(msg.data.c_str(),"Null")!=0)
  {
    std_msgs::Int8 status;
    std_msgs::Int8 sys_free;
    string buffer=XMLanalyzed(msg.data.c_str());

    if(!strcmp(buffer.c_str(),"10110001"))
    {
      std_msgs::String wakeup;
      wakeup.data = "wake_up";
      pub.publish(wakeup);
      status.data = SYSTEM_STATUS_LEARNING_ON;
      sys_free.data = SYSTEM_STATUS_NOT_FREE;
      pub1.publish(status);
      pub2.publish(sys_free);
    }

    if(!strcmp(buffer.c_str(),"10000"))
    {
      std_msgs::String sleep;
      sleep.data = "sleep";
      pub.publish(sleep);
      status.data = SYSTEM_STATUS_WAKEUP_NO;
      sys_free.data = SYSTEM_STATUS_WAKEUP_NO;
      pub1.publish(status);
      pub2.publish(sys_free);
    }

  }
}

int main(int argc, char **argv)
{
  int state = 0;
  ros::init(argc, argv, "XML_analyzed");
  ros::NodeHandle n;
  pub=n.advertise<std_msgs::String>("learning_on",10);
  pub1=n.advertise<std_msgs::Int8>("local_asr_result_msg",10);
  pub2=n.advertise<std_msgs::Int8>("system_status",10);
  ros::Subscriber sub=n.subscribe("asr_server/xf/local_s_res",1000,callback);
  ros::spin();
  return 0;
}
