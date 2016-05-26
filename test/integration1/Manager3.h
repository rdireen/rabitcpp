#ifndef MANAGER3
#define MANAGER3

#include <iostream>
#include <string>
#include "RabitManager.h"
#include "RabitMessageQueue.h"

class Manager3 : public Rabit::RabitManager {

private:
  typedef Rabit::RabitMessageQueue<std::string> StringMessageQueue;
  std::shared_ptr<StringMessageQueue> _squeue_sptr;

public:
  Manager3(std::string name ) : RabitManager(name){

    this->SetWakeupTimeDelayMSec(20);
    _squeue_sptr = std::make_shared<StringMessageQueue>(10, "printstrings");
    this->AddManagerMessageQueue(_squeue_sptr->GetMessageQueueName(), _squeue_sptr);
  }

  void ExecuteUnitOfWork() final {
    for(int n = 0; n < _squeue_sptr->NoMessagesInQueue(); n++){
        auto val = _squeue_sptr->GetMessage();
        std::cout << this->GetManagerName() << ": " << val << std::endl;
    }
  }

};

#endif //MANAGER3
