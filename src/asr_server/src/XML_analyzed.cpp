#include <ros/ros.h>
#include <stdio.h>
#include "../../lib/tinyxml2/tinyxml2.h"
#include "std_msgs/String.h"

using namespace tinyxml2;
ros::Publisher pub;


char* XMLanalyzed(const char* XMLdata)
{
  XMLDocument awk;
  awk.Parse(XMLdata,strlen(XMLdata));

  XMLElement *root=awk.RootElement();
  XMLElement *result=root->FirstChildElement("result");
  XMLElement *object=result->FirstChildElement("object");
  XMLElement *rawtext=root->FirstChildElement("rawtext");
  XMLElement *child1=object->FirstChildElement();

  const char * value = rawtext->GetText();
  std::cout<<"Command : "<<value<<std::endl;
  const XMLAttribute *attribute1=child1->FirstAttribute();
  //std::cout<<attribute1->Value()<<std::endl;

  XMLElement *child2=child1->NextSiblingElement();
  const XMLAttribute *attribute2=child2->FirstAttribute();
  //std::cout<<attribute2->Value()<<std::endl;
  char *buffer = new char[7];
  sprintf(buffer,"%s%s",attribute1->Value(),attribute2->Value());
  //std::cout<<buffer<<std::endl;
  return buffer;
}

void callback(const std_msgs::String &msg)
{
  if(strcmp(msg.data.c_str(),"Null")!=0)
  {

    char *buffer=XMLanalyzed(msg.data.c_str());

    if(!strcmp(buffer,"10110001"));
    {
        std_msgs::String wakeup;
        wakeup.data = "wake_up";
        pub.publish(wakeup);
    }
    delete []buffer;
  }
}

int main(int argc, char **argv)
{
  int state = 0;
  ros::init(argc, argv, "XML_analyzed");
  ros::NodeHandle n;
  pub=n.advertise<std_msgs::String>("learning_on",10);
  ros::Subscriber sub=n.subscribe("asr_server/xf/local_s_res",1000,callback);
  ros::spin();
  return 0;
}
