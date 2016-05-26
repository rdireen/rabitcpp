/* 5/12/2016
 * Randy Direen
 *
 * Integration test to exercise the various parts of the code.
 *
 */

#include <iostream>
#include <memory>
#include "RabitReactor.h"
#include "RabitManager.h"
#include "Manager0.h"
#include "Manager1.h"
#include "Manager2.h"
#include "Manager3.h"

using namespace std;
using namespace Rabit;


typedef std::unique_ptr<Rabit::RabitManager> ManagerPtr;

int main(int argc, char* argv[]) {

  std::cout << "***************************************************" << std::endl;
  std::cout << "*              Integration1                       *" << std::endl;
  std::cout << "***************************************************" << std::endl;
  std::cout << std::endl;

  auto m0 = ManagerPtr(new Manager0("Manager0"));
  auto m1 = ManagerPtr(new Manager1("Manager1"));
  auto m2 = ManagerPtr(new Manager2("Manager2"));
  auto m3 = ManagerPtr(new Manager3("Manager3"));

  auto reactor = Rabit::RabitReactor();

  reactor.AddManager(std::move(m0));
  reactor.AddManager(std::move(m1));
  reactor.AddManager(std::move(m2));
  reactor.AddManager(std::move(m3));

  reactor.Run();

}
