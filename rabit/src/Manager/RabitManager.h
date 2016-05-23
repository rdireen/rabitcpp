#ifndef RABIT_MANAGER
#define RABIT_MANAGER

#include <memory>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <chrono>
#include <exception>
#include <iostream>
#include "RabitWorkspace.h"
#include "RabitMessage.h"

namespace Rabit{
  
  class RabitManager{
    
  private:
    int _wakeupTimeDelayMSec = 1000;
    int _wakeupNTimes = 0;
    std::thread  _managerThread;
    std::mutex _mutexWaitFor;
    std::condition_variable _cvar;
    std::string _managerName = "None";
    bool _shutdownManager;
    typedef RabitMessageQueue<std::shared_ptr<RabitMessage>> RabitQueue;
    std::shared_ptr<RabitQueue> _mgrMessageQueueUptr;
    
  //Accessors  
  public:

    int GetWakeupTimeDelayMSec() const{
      return _wakeupTimeDelayMSec;
    }

    double GetWakeupTimeDelaySec() const{
      return 0.001 * (double)_wakeupTimeDelayMSec;
    }

    void SetWakeupTimeDelayMSec(int value){
      _wakeupTimeDelayMSec = value < 10 ? 10 : value;
    }

    void SetWakeupTimeDelaySec(double value){
      _wakeupTimeDelayMSec = 1000 * (int)value;
    }
    
    std::string GetManagerName() {
      if(_managerName == "None"){
         _managerName = _managerName;
      }
      return _managerName;
    }
    
  public:
    
    RabitManager(std::string name){
      _managerName = name;
      _mgrMessageQueueUptr = std::make_shared<RabitQueue>(1000, name);
      _shutdownManager = false;
    }

    template<typename T>
    bool AddPublishSubscribeMessage(std::string name, T msg)
    {
      RabitWorkspace::GetWorkspace()->AddPublishSubscribeMessage(name, msg);
    }

    template<typename T>
    void AddManagerMessageQueue(std::string name, T rabitMessageQueue){
      RabitWorkspace::GetWorkspace()->AddManagerMessageQueue(name, rabitMessageQueue);
    }

    template<typename T>
    void AddMessageToQueue(std::string name, T msg){
      RabitWorkspace::GetWorkspace()->AddMessageToQueue(name, msg);
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
    
    void virtual Startup(){
      
    }
    
    void virtual ExecuteUnitOfWork() = 0;

    void WakeUpManagerEH(){
      _cvar.notify_one();
    }
    
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
	    std::unique_lock<std::mutex> lk(_mutexWaitFor);
	    _cvar.wait_for(lk, std::chrono::milliseconds(_wakeupTimeDelayMSec));
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
