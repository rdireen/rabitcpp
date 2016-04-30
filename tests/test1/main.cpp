

#include <iostream>
#include <memory>
#include "RabitReactor.h"
#include "RabitManager.h"


class TestManager : public Rabit::RabitManager{

private:
    int _counter = 0;
public:
  void ExecuteUnitOfWork() final {
      std::cout << "All men are enveloped in whale lines... " << std::endl;
      std::cout << GetManagerName() << std::endl;
      _counter++;
      if( _counter > 5 )
          this->ShutdownManager();
  }
  
  std::string ReturnManagerName() final {
      return std::string("TestManager");
  }
  
};

class AnotherTestManager : public Rabit::RabitManager{  
private:
    int _counter = 0;
public:
  void ExecuteUnitOfWork() final {
      std::cout << "Ahab beware of Ahab. " << std::endl;
      std::cout << GetManagerName() << std::endl;
      _counter++;
      if( _counter > 5 )
          this->ShutdownManager();
  }
  
  std::string ReturnManagerName() final {
      return std::string("AnotherTestManager");
  }
  
};

typedef std::unique_ptr<Rabit::RabitManager> ManagerPtr;

int main(int argc, char* argv[]) {
  
  std::cout << "Oh sweet" << std::endl;
  
  auto manager = ManagerPtr(new TestManager());
  auto amanager = ManagerPtr(new AnotherTestManager());
  
  auto reactor = Rabit::RabitReactor();
  
  reactor.AddManager(std::move(manager));
  reactor.AddManager(std::move(amanager));
  
  reactor.Run();
  
}
