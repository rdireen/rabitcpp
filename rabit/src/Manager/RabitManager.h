#ifndef RABIT_MANAGER
#define RABIT_MANAGER

#include <thread>
#include <chrono>
#include <exception>
#include <iostream>

namespace Rabit{
  
  class RabitManager{
    
  private:
    int _wakeupTimeDelayMSec = 1000;
    std::thread  _managerThread;
    std::string _managerName = "None";
    bool _shutdownManager;
    
  //Accessors  
  public:

    int GetWakeupTimeDelayMSec(){
      return _wakeupTimeDelayMSec;
    }

    double GetWakeupTimeDelaySec(){
      return 0.001 * (double)_wakeupTimeDelayMSec;
    }

    void SetWakeupTimeDelayMSec(int value){
      _wakeupTimeDelayMSec = value < 10 ? 10 : value;
    }

    void SetWakeupTimeDelaySec(double value){
      _wakeupTimeDelayMSec = 1000 * (int)value;
    }
    
    std::string GetManagerName(){
      if(_managerName == "None"){
         _managerName = ReturnManagerName();
      }
      return _managerName;
    }
    
  public:
    
    RabitManager(){
      _shutdownManager = false;
    }
    
    void Run(){
      _managerThread = std::thread(&RabitManager::managerMain, this);
    }
    
    void ShutdownManager(){
      _shutdownManager = true;
    }
    
    void Join(){
      _managerThread.join();	
    }
    
    std::string virtual ReturnManagerName() = 0;
    
    void virtual Startup(){
      
    }
    
    void virtual ExecuteUnitOfWork() = 0;
    
    void virtual Shutdown(){
      
    }
    
  private:
    
    void managerMain(){
      
      Startup();
      
      while(!_shutdownManager and true) // need to add to MgrControl here
      {
	try{
	  ExecuteUnitOfWork();
	  if ( _wakeupTimeDelayMSec > 0 ) {
	     std::this_thread::sleep_for(std::chrono::milliseconds(_wakeupTimeDelayMSec));
	  }
	  
	  //MgrControl.FetchMessage();
	  
	}catch(std::exception& e){
	  std::cout << "Manager: " << _managerName 
	            << " with exception: " << e.what();
	  
	}    
	
      }

      Shutdown();

    }
  };
}

#endif //RABIT_MANAGER
