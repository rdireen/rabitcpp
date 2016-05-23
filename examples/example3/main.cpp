/* 5/14/2016
 * Randy Direen
 * 
 * ::NOTE::
 * This is similar to example2 except that this example uses events to 
 * immediately do processing once data is received. There really is only one
 * change from example2 and that is registering the WakeUpManagerEH handler
 * with the RabitMessageQueue.
 * 
 * Three managers are made to send information back and forth to each other
 * using RabbitMessageQueues. The first manager sends a list of strings to
 * a censoring manager, which in turn sends censored strings to a console
 * printing manager. 
 * 
 */

#include <iostream>
#include <vector>
#include <memory>
#include "RabitReactor.h"
#include "RabitManager.h"
#include "RabitMessageQueue.h"
#include "RabitWorkspace.h"

using namespace std;
using namespace Rabit;

/****************************************************************************
 * This manager creates strings to be printed in the console. But before the
 * strings are sent to the ConsoleManager they are first sent to the
 * CensorManager (using a RabitMessageQueue) to be censored for sensitive
 * information.
 ****************************************************************************/
class StringManager : public RabitManager{

private:
    int _counter = 0;
    shared_ptr<RabitMessageQueue<std::string>> _inqueue_sptr;
public:

  StringManager(string name ) : RabitManager(name){

    // This is the amount of time the manager waits before waking up. If an event
    // has been setup, it will wakeup before the timeout.
    this->SetWakeupTimeDelayMSec(30);
    _inqueue_sptr = make_shared<RabitMessageQueue<std::string>>(100, "instrings");
    this->AddManagerMessageQueue(_inqueue_sptr->GetMessageQueueName(), _inqueue_sptr);

    _inqueue_sptr->AddMessage("Dear Mom,");
    _inqueue_sptr->AddMessage("Today I walked around downtown with a dollar");
    _inqueue_sptr->AddMessage("in my pocket and was stopped by a homeless man.");
    _inqueue_sptr->AddMessage("He asked me if I knew of any good pawn stores");
    _inqueue_sptr->AddMessage("he might have luck selling his teeth too. Being ");
    _inqueue_sptr->AddMessage("the guy I am, I remembered the dollar in my pocket");
    _inqueue_sptr->AddMessage("and then punched that man in the face. I had to use");
    _inqueue_sptr->AddMessage("the dollar to buy a bandaid because his teeth weren't ");
    _inqueue_sptr->AddMessage("worth too much.");
    _inqueue_sptr->AddMessage("end");
  }

  void ExecuteUnitOfWork() final {
      if(_inqueue_sptr->NoMessagesInQueue() > 0){
        string next = _inqueue_sptr->GetMessage();

        // This is how we communicate with the CensorManager. The CensorManager has
        // added a RabitMessageQueue to the RabitWorkspace
        this->AddMessageToQueue("rawstrings", next);
      } else {
        this->ShutdownManager();
      }
  }
  
};

/****************************************************************************
 * This manager receives strings via a RabitMessageQueue and censors it for
 * sensitive information. Once the strings have been censored, they are then
 * sent to the ConsoleManager so that they can be displayed on the screen.
 ****************************************************************************/
class CensorManager : public Rabit::RabitManager{
private:
    int _counter = 0;
    shared_ptr<RabitMessageQueue<std::string>> _rqueue_sptr;
    vector<string> _cstrings;
public:

  CensorManager(string name) : RabitManager(name){

    // This is the amount of time the manager waits before waking up. If an event
    // has been setup, it will wakeup before the timeout.
    this->SetWakeupTimeDelayMSec(1000);

    // This is how we add message queue to the RabitWorkspace
    _rqueue_sptr = make_shared<RabitMessageQueue<std::string>>(100, "rawstrings");
    this->AddManagerMessageQueue(_rqueue_sptr->GetMessageQueueName(), _rqueue_sptr);

    _cstrings = vector<string>();
    _cstrings.push_back("Mom");
    _cstrings.push_back("downtown");
    _cstrings.push_back("dollar");
    _cstrings.push_back("pocket");
    _cstrings.push_back("homeless");
    _cstrings.push_back("pawn");
    _cstrings.push_back("stores");
    _cstrings.push_back("selling");
    _cstrings.push_back("teeth");
    _cstrings.push_back("guy");
    _cstrings.push_back("face");
    _cstrings.push_back("bandaid");
    _cstrings.push_back("worth");
    
    // This is how we know someone has placed an item into the queue we setup.
    _rqueue_sptr->Register_SomethingEnqueued(boost::bind(&CensorManager::WakeUpManagerEH, this));
  }

  void ExecuteUnitOfWork() final {

    for(int n = 0; n < _rqueue_sptr->NoMessagesInQueue(); n++){
      auto val = _rqueue_sptr->GetMessage();

      if(val == "end"){
        this->ShutdownManager();
        this->AddMessageToQueue("printstrings", val);
      }
      else{
        for (int m = 0; m < _cstrings.size(); m++){
            int index = 0;
            index = val.find(_cstrings[m], index);
            if (index != std::string::npos){
              string c = string(_cstrings[m].length(),'*');
              val.replace(index, _cstrings[m].length(), c);
            }
        }
        this->AddMessageToQueue("printstrings", val);
      }
    }

  }
  
};

/****************************************************************************
 * This manager receives strings via a RabitMessageQueue and displays them
 * in the console.
 ****************************************************************************/
class ConsoleManager : public Rabit::RabitManager{
private:
    int _counter = 0;
    shared_ptr<RabitMessageQueue<std::string>> _cqueue_sptr;
public:

  ConsoleManager(string name ) : RabitManager(name){
    this->SetWakeupTimeDelayMSec(1000);
    _cqueue_sptr = make_shared<RabitMessageQueue<std::string>>(100, "printstrings");
    this->AddManagerMessageQueue(_cqueue_sptr->GetMessageQueueName(), _cqueue_sptr);
    _cqueue_sptr->Register_SomethingEnqueued(boost::bind(&ConsoleManager::WakeUpManagerEH, this));
  }

  void ExecuteUnitOfWork() final {
    for(int n = 0; n < _cqueue_sptr->NoMessagesInQueue(); n++){
        auto val = _cqueue_sptr->GetMessage();
        if(val == "end")
          this->ShutdownManager();
        else
          cout << val << endl;
    }

  }

};

typedef std::unique_ptr<Rabit::RabitManager> ManagerPtr;

int main(int argc, char* argv[]) {
  
  std::cout << "***************************************************" << std::endl;
  std::cout << "*              Censored Message                   *" << std::endl;
  std::cout << "***************************************************" << std::endl;
  std::cout << std::endl;
  
  auto stringM = ManagerPtr(new StringManager("StringManager"));
  auto censorM = ManagerPtr(new CensorManager("CensorManager"));
  auto consoleM = ManagerPtr(new ConsoleManager("ConsoleManager"));
  
  auto reactor = Rabit::RabitReactor();
  
  reactor.AddManager(std::move(stringM));
  reactor.AddManager(std::move(censorM));
  reactor.AddManager(std::move(consoleM));
  
  reactor.Run();
  
}
