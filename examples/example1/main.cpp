/* 5/12/2016
 * Randy Direen
 *
 * Just a quick example to startup a couple managers and spit information to
 * the console. No information is being passed between the two managers yet.
 *
 */

#include <iostream>
#include <memory>
#include "RabitReactor.h"
#include "RabitManager.h"

using namespace std;
using namespace Rabit;


class Manager1 : public RabitManager{

private:
    int _counter = 0;
public:

  Manager1(std::string name ): RabitManager(name){
    this->SetWakeupTimeDelayMSec(1000);
  }


  void ExecuteUnitOfWork() final {
      cout << "From Manager1" << endl;

      _counter++;
      if( _counter > 10 )
          this->ShutdownManager();
  }

};

class Manager2 : public RabitManager{
private:
    int _counter = 0;

public:

  Manager2(std::string name ) : RabitManager(name){
    this->SetWakeupTimeDelayMSec(500);

  }

  void ExecuteUnitOfWork() final {

    cout << "From Manager2" << endl;

    _counter++;
    if( _counter > 10 )
        this->ShutdownManager();
  }

};


typedef std::unique_ptr<Rabit::RabitManager> ManagerPtr;

int main(int argc, char* argv[]) {

  std::cout << "***************************************************" << std::endl;
  std::cout << "*              Quick Example                      *" << std::endl;
  std::cout << "***************************************************" << std::endl;
  std::cout << std::endl;

  auto m1 = ManagerPtr(new Manager1("Manager1"));
  auto m2 = ManagerPtr(new Manager2("Manater2"));


  auto reactor = Rabit::RabitReactor();

  reactor.AddManager(std::move(m1));
  reactor.AddManager(std::move(m2));

  reactor.Run();

}
