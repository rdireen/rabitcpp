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

/* Auto-generated, do not modify directly */
@{
def include_messages():
    if msg_data["topics"]:
        for topic in msg_data["topics"]:
            print("#include \"{}.h\"".format(topic["msg"]))

def define_topics():
    if msg_data["topics"]:
        for topic in msg_data["topics"]:
            print("        std::shared_ptr<{}> {};".format(topic["msg"], topic["name"]))

}@

#pragma once
#include <memory>
#include "RabitManager.h"
#include "RabitMessage.h"
#include "RabitWorkspace.h"

@include_messages()
namespace @(nm_space)
{

class @(class_name)WorkSpace{
    private:
    bool _initialized = false;

    public:
@define_topics()

    bool is_initialized(){return _initialized;}
    void Initialize();
};

class @(class_name) : public Rabit::RabitManager{

    private:
        @(class_name)WorkSpace _ws;

    private:
        void InitializeWorkSpace(void);

    public:
        @(class_name)(std::string name);


        @(class_name)WorkSpace * WorkSpace(){
            return &_ws;
        }

        ~@(class_name)(){}

};
}
