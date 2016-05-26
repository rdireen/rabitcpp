#ifndef MANAGER1
#define MANAGER1

#include <string>
#include <string>
#include "RabitManager.h"

class Manager1 : public Rabit::RabitManager {

private:
  int _count = 0;

public:
  Manager1(std::string name ) : RabitManager(name){

    this->SetWakeupTimeDelayMSec(100);

  }

  void ExecuteUnitOfWork() final {

    for (int n = 0; n < 10; n++){
      std::ostringstream os;
      _count+=7;
      os << this->GetManagerName() << ": " << _count;
      this->AddMessageToQueue("procstrings", os.str());
    }
  }

};

#endif //MANAGER1
