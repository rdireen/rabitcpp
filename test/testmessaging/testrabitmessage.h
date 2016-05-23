#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "RabitMessage.h"
#include <memory>

/*****************************************************************************
 *
 * A couple messages to work with
 *
 * **************************************************************************/
class MessageA : public Rabit::RabitMessage{

public:
  int a;
  int b;

public:

  MessageA(std::string name) : Rabit::RabitMessage(name){
    a = 1;
    b = 2;
  }

  virtual std::unique_ptr<Rabit::RabitMessage> Clone() final{
    std::unique_ptr<MessageA> clone = std::unique_ptr<MessageA>(new MessageA(GetMessageTypeName()));
    clone->CopyBase(this);
    clone->a = this->a;
    clone->b = this->b;
    return std::move(clone);
  }

  virtual bool CopyMessage(Rabit::RabitMessage* msg) final{
    Rabit::RabitMessage::CopyMessage(msg); // call baseclass
    if(msg->GetTypeIndex() == std::type_index(typeid(MessageA))){
      this->a = static_cast<MessageA*>(msg)->a;
      this->b = static_cast<MessageA*>(msg)->b;
      return true;
    }
    return false;
  }

  virtual void Clear() final {
    a = 0;
    b = 0;
  }

  virtual std::string ToString() const final {
    std::ostringstream ss;
    ss << "a = " << a << "   b = " << b;
    return ss.str();
  }


};

class MessageB : public Rabit::RabitMessage{
public:

  MessageB(std::string name) : Rabit::RabitMessage(name){

  }

  virtual std::unique_ptr<Rabit::RabitMessage> Clone() final{}

  virtual bool CopyMessage(Rabit::RabitMessage* msg) final{
    Rabit::RabitMessage::CopyMessage(msg); // call baseclass
    if(msg->GetTypeIndex() == std::type_index(typeid(MessageA))){
      return true;
    }
    return false;
  }

  virtual void Clear() final {

  }

  virtual std::string ToString() const final { }


};

// The fixture for testing class Rabit.
class RabitMessageTest : public ::testing::Test {

public:

    // You can do set-up work for each test here.
    RabitMessageTest(){}

    // You can do clean-up work that doesn't throw exceptions here.
    virtual ~RabitMessageTest(){}

    // If the constructor and destructor are not enough for setting up
    // and cleaning up each test, you can define the following methods:

    // Code here will be called immediately after the constructor (right
    // before each test).
    virtual void SetUp(){

    }

    // Code here will be called immediately after each test (right
    // before the destructor).
    //virtual void TearDown();

};

