/* ****************************************************************
 * Rabit Multi-Threaded Management System
 * Athrs: Randal Direen PhD
 *        Harry Direen PhD
 * www.direentech.com
 * Date: June 2016
 *
 *******************************************************************/

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
#include "ManagerControlMessage.h"
#include "ManagerStatusMessage.h"
#include "ManagerStats.h"
#include "RabitStopWatch.h"

namespace Rabit
{

    class RabitManager
    {

    private:
        int _wakeupTimeDelayMSec = 1000;
        int _wakeupNTimes = 0;
        std::thread _managerThread;
        std::mutex _mutexWaitFor;
        bool _mgrWakeUpHit = false;
        bool _mgrIsAsleep = false;
        std::condition_variable _cvar;
        std::string _managerName = "None";
        bool _shutdownManager;
        typedef RabitMessageQueue<std::shared_ptr<RabitMessage>> RabitQueue;
        std::shared_ptr<RabitQueue> _mgrMessageQueue_sptr;

        //Timers for operational Statistics
        RabitStopWatch _mgrTotalTimeSec;
        RabitStopWatch _execUnitOfWorkTime;
        RabitStopWatch _sleepTime;

        //A Manager control message that applies to all managers.
        std::shared_ptr<ManagerControlMessage> _mgrControlAllMgrs_sptr;

        //A Manager control message that applies to this manager only.
        std::shared_ptr<ManagerControlMessage> _mgrControlThisMgr_sptr;

        //The Status and Stats of this manager.
        std::shared_ptr<ManagerStatusMessage> _mgrStatus_sptr;

        //Accessors
    public:

        //A Manager control message that applies to all managers.
	//This message is automatically keep up-to-date in each manager.
	//Message:  Rabit::ManagerControlMessage
	//Publish Subscribe Message Name:  "ManagerControlMessage"
        std::shared_ptr<ManagerControlMessage> GetControlAllManagersMessage()
	{
	    return _mgrControlAllMgrs_sptr;
	}
	
        //A Manager control message that applies to all managers.
	//This message is automatically keep up-to-date in each manager.
	//Message:  Rabit::ManagerControlMessage
	//Publish Subscribe Message Name:  "ManagerName:ManagerControlMessage"
        std::shared_ptr<ManagerControlMessage> GetControlThisManagerMessage()
	{
	    return _mgrControlThisMgr_sptr;
	}

        //A Manager control message that applies to all managers.
	//This message is automatically keep up-to-date in each manager.
	//Message:  Rabit::ManagerControlMessage
	//Publish Subscribe Message Name:  "ManagerName:ManagerStatusMessage"
        std::shared_ptr<ManagerStatusMessage> GetManagerStatusMessage()
	{
	    return _mgrStatus_sptr;
	}


        int GetWakeupTimeDelayMSec() const
        {
            return _wakeupTimeDelayMSec;
        }

        double GetWakeupTimeDelaySec() const
        {
            return 0.001 * (double) _wakeupTimeDelayMSec;
        }

        void SetWakeupTimeDelayMSec(int value)
        {
            _wakeupTimeDelayMSec = value < 10 ? 10 : value;
        }

        void SetWakeupTimeDelaySec(double value)
        {
            _wakeupTimeDelayMSec = 1000 * (int) value;
        }

        std::string GetManagerName()
        {
            if (_managerName == "None")
            {
                _managerName = _managerName;
            }
            return _managerName;
        }

    public:

        RabitManager(std::string name);

        template<typename T>
        bool AddPublishSubscribeMessage(std::string name, T msg)
        {
            auto msgr = std::static_pointer_cast<RabitMessage>(msg);
            RabitWorkspace::GetWorkspace()->AddPublishSubscribeMessage(name, msgr);
        }

        //Fetch a Mesage from the Publish Subscribe Message List.
        //Returns true if the message was found and copied, false otherwise;
        //Will only copy the messag if the timestamps are different.
        bool FetchMessage(std::string name, RabitMessage *msgPtr)
        {
            return RabitWorkspace::GetWorkspace()->FetchMessage(name, msgPtr);
        }

        //Fetch a Mesage from the Publish Subscribe Message List.
        //Returns true if the message was found and copied, false otherwise;
        bool ForceFetchMessage(std::string name, RabitMessage *msgPtr)
        {
            return RabitWorkspace::GetWorkspace()->ForceFetchMessage(name, msgPtr);
        }

        template<typename T>
        void WakeUpManagerOnMessagePost(T msg)
        {
            msg->Register_SomethingPublished(boost::bind(&RabitManager::WakeUpManagerEH, this));
        }

        template<typename T>
        void AddManagerMessageQueue(std::string name, T rabitMessageQueue)
        {
            RabitWorkspace::GetWorkspace()->AddManagerMessageQueue(name, rabitMessageQueue);
        }

        template<typename T>
        T GetManagerMessageQueue(std::string name)
        {
            return RabitWorkspace::GetWorkspace()->GetManagerMessageQueue<T>(name);
        }

        template<typename T>
        void WakeUpManagerOnEnqueue(T queue)
        {
            queue->Register_SomethingEnqueued(boost::bind(&RabitManager::WakeUpManagerEH, this));
        }

        template<typename T>
        void AddMessageToQueue(std::string name, T msg)
        {
            RabitWorkspace::GetWorkspace()->AddMessageToQueue(name, msg);
        }

        void Run()
        {
            _managerThread = std::thread(&RabitManager::managerMain, this);
        }

        void ShutdownManager()
        {
            _shutdownManager = true;
        }

        void ShutdownAllManagers(bool value)
        {
            _mgrControlAllMgrs_sptr->ShutdownManager(value);
            _mgrControlAllMgrs_sptr->PostMessage();
        }


        void Join()
        {
            _managerThread.join();
        }

        //The Startup method is called once when the manager thread is first
        //started.  It can be used for any necessary initialization processess
        //that have to be done after the manager's constructor.
        void virtual Startup()
        {

        }

        void virtual ExecuteUnitOfWork() = 0;

        void WakeUpManagerEH();

        //The Shutdown method is called when the Manager loop is terminated.
        //So it is called to allow any clean-up operations necessary before shutting
        //down the manager.
        void virtual Shutdown()
        {

        }

        /**
         * If this isn't declared as virtual the destructor of the Super Managers wouldn't
         * get called and I would get memory leaks.
         */
        //Use the Manager's Destructor to clean up any allocated memory and other
        //system resources.
        virtual ~RabitManager()
        {

        }

    private:

        //The Manager Main is an infinite loop which continuously calls the
        //ExecuteUnitOfWork() method.  After calling the ExecuteUnitOfWork() method
        //the manager will go to sleep for the established number of milliseconds provided
        //in the _wakeupTimeDelayMSec parameter.
        //Events can be used to wake up the manager earlier than the given _wakeupTimeDelayMSec.
        void managerMain();

        //Helper Methods
        float GetStatsPublishTime();
    };
}

#endif //RABIT_MANAGER
