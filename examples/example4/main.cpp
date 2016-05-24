/* 5/14/2016
 * Randy Direen
 * 
 * Demonstrates PublishSubscribeMessages
 * 
 */

#include <iostream>
#include <sstream>
#include <vector>
#include <memory>
#include "RabitReactor.h"
#include "RabitManager.h"
#include "RabitMessageQueue.h"
#include "RabitWorkspace.h"

using namespace std;
using namespace Rabit;

/*****************************************************************************
 *                            Messages
 ****************************************************************************/

class MessageA : public Rabit::RabitMessage{

public:
  int a;
  int b;

public:

  MessageA(std::string name) : Rabit::RabitMessage(name){
    a = 0;
    b = 1;
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


/****************************************************************************
 *                                Managers
 ****************************************************************************/

/**
 * This manager posts numbers to the simple message
 */
class NumberManager : public RabitManager{

private:
    shared_ptr<MessageA> _messageA_sptr;
public:

  NumberManager(string name ) : RabitManager(name){

    // This is the amount of time the manager waits before waking up. If an event
    // has been setup, it will wakeup before the timeout.
    this->SetWakeupTimeDelayMSec(500);
    _messageA_sptr = make_shared<MessageA>("MessageA");
    this->AddPublishSubscribeMessage(_messageA_sptr->GetMessageTypeName(), _messageA_sptr);
  }

  void ExecuteUnitOfWork() final {
    if(_messageA_sptr->a < 20){
      _messageA_sptr->a++;
      _messageA_sptr->b++;
       _messageA_sptr->PostMessage();
    } else {
      this->ShutdownManager();
    }
  }
};

/**
 * This manager prints the messages.
 */
class ConsoleManager : public Rabit::RabitManager{
private:
    shared_ptr<MessageA> _messageA_sptr;
public:

  ConsoleManager(string name ) : RabitManager(name){

    // Change this above and below 500 to see if Fetching Works properly.
    this->SetWakeupTimeDelayMSec(200);
    _messageA_sptr = make_shared<MessageA>("MessageA");
    this->AddPublishSubscribeMessage(_messageA_sptr->GetMessageTypeName(), _messageA_sptr);
  }

  void ExecuteUnitOfWork() final {
    // See if a new message is there and get it.

    if(_messageA_sptr->FetchMessage()){
      cout << "::Got new message::" << endl;
      if( _messageA_sptr->a < 20){
        cout << "  Currently we have: " << _messageA_sptr->a << endl;
      }else{
        this->ShutdownManager();
      }
    } else {
      cout << "::NO NEW MESSAGE::" << endl;
    }
  }
};

typedef std::unique_ptr<Rabit::RabitManager> ManagerPtr;
int main(int argc, char* argv[]) {
  
  std::cout << "***************************************************" << std::endl;
  std::cout << "*              Updated Number                     *" << std::endl;
  std::cout << "***************************************************" << std::endl;
  std::cout << std::endl;
  
  auto numberM = ManagerPtr(new NumberManager("NumberManager"));
  auto consoleM = ManagerPtr(new ConsoleManager("ConsoleManager"));

  auto reactor = Rabit::RabitReactor();
  
  reactor.AddManager(std::move(numberM));
  reactor.AddManager(std::move(consoleM));

  reactor.Run();
}
