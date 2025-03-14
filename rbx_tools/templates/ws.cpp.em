@###############################################
@#
@# Rabit message generation for C++
@#
@# EmPy template for generating <msg>.cpp
@#
@###############################################
@# Start of Template
@###############################################

/* ****************************************************************
  *******************************************************************/

/* Auto-generated do not modify directly */

#include "@(class_name).h"
@{
def init_topics():
    if msg_data["topics"]:
        for topic in msg_data["topics"]:
            print("        {} = std::make_shared<{}>();".format(topic["name"], topic["msg"]))

def pubsub_topics():
    if msg_data["topics"]:
        for topic in msg_data["topics"]:
            print("        AddPublishSubscribeMessage(\"{}\", WorkSpace()->{});".format(topic["name"], topic["name"]))
            if(topic["wakeup"]):
                print("        WakeUpManagerOnMessagePost(WorkSpace()->{});".format(topic["name"]))
            print("")

}@

namespace @(nm_space)
{

    void @(class_name)WorkSpace::Initialize()
    {
@init_topics()
        _initialized=true;
    }


    @(class_name)::@(class_name)(std::string name) : Rabit::RabitManager(name){
            InitializeWorkSpace();
    }

    void @(class_name)::InitializeWorkSpace(void)
    {
        WorkSpace()->Initialize();
@pubsub_topics()

    }

}