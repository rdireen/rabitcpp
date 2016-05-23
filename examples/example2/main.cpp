/* 5/12/2016
 * Randy Direen
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
    vector<string> _vstrings;
public:

  StringManager(string name ): RabitManager(name){

    this->SetWakeupTimeDelayMSec(500);
    _vstrings = vector<string>();
    _vstrings.push_back("worth too much.");
    _vstrings.push_back("the dollar to buy a bandaid because his teeth weren't ");
    _vstrings.push_back("and then punched that man in the face. I had to use");
    _vstrings.push_back("the guy I am, I remembered the dollar in my pocket");
    _vstrings.push_back("he might have luck selling his teeth too. Being ");
    _vstrings.push_back("He asked me if I knew of any good pawn stores");
    _vstrings.push_back("in my pocket and was stopped by a homeless man.");
    _vstrings.push_back("Today I walked around downtown with a dollar");
    _vstrings.push_back("Dear Mom,");
  }

  void ExecuteUnitOfWork() final {
      if(!_vstrings.empty()){
        string next = _vstrings.back();
        _vstrings.pop_back();
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
class CensorManager : public RabitManager{
private:
    int _counter = 0;
    shared_ptr<RabitMessageQueue<std::string>> _rqueue_sptr;
    vector<string> _cstrings;
public:

  CensorManager(string name) : RabitManager(name){

    this->SetWakeupTimeDelayMSec(500);

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
  }


  void ExecuteUnitOfWork() final {

    for(int n = 0; n < _rqueue_sptr->NoMessagesInQueue(); n++){
        auto val = _rqueue_sptr->GetMessage();


        for (int m = 0; m < _cstrings.size(); m++){
            int index = 0;
            index = val.find(_cstrings[m], index);
            if (index != std::string::npos){
              string c = string(_cstrings[m].length(),'*');
              //cout << _cstrings[m] << " " << c << endl;
              val.replace(index, _cstrings[m].length(), c);
            }
        }

        this->AddMessageToQueue("printstrings", val);
    }

    _counter++;
    if( _counter > 10 )
        this->ShutdownManager();
  }

  
};

/****************************************************************************
 * This manager receives strings via a RabitMessageQueue and displays them
 * in the console.
 ****************************************************************************/
class ConsoleManager : public RabitManager{
private:
    int _counter = 0;
    shared_ptr<RabitMessageQueue<std::string>> _cqueue_sptr;
public:

  ConsoleManager(string name) : RabitManager(name){
    _cqueue_sptr = make_shared<RabitMessageQueue<std::string>>(100, "printstrings");
    this->AddManagerMessageQueue(_cqueue_sptr->GetMessageQueueName(), _cqueue_sptr);
  }


  void ExecuteUnitOfWork() final {

    for(int n = 0; n < _cqueue_sptr->NoMessagesInQueue(); n++){
        auto val = _cqueue_sptr->GetMessage();
        cout << val << endl;
    }

      _counter++;
      if( _counter > 10 )
          this->ShutdownManager();
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
