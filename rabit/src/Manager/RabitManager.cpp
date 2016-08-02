/* ****************************************************************
 * Rabit Multi-Threaded Management System
 * Athrs: Randal Direen PhD
 *        Harry Direen PhD
 * www.direentech.com
 * Date: June 2016
 *
 *******************************************************************/
#include "RabitManager.h"

namespace Rabit
{

    RabitManager::RabitManager(std::string name)
    {
        std::string tmpStr;

        _managerName = name;
        _mgrMessageQueue_sptr = std::make_shared<RabitQueue>(1000, name);
        _shutdownManager = false;

        _mgrControlAllMgrs_sptr = std::make_shared<ManagerControlMessage>("ManagerControlMessage");
        this->AddPublishSubscribeMessage(_mgrControlAllMgrs_sptr->GetMessageTypeName(), _mgrControlAllMgrs_sptr);
	_mgrControlAllMgrs_sptr->Register_SomethingPublished(boost::bind(&RabitManager::WakeUpManagerEH, this));

        //This Manager may be controlled with a Manager Control Message with Name:  "MngName:ManagerControlMessage"
        tmpStr = _managerName + ":ManagerControlMessage";
        _mgrControlThisMgr_sptr = std::make_shared<ManagerControlMessage>(tmpStr);
        this->AddPublishSubscribeMessage(_mgrControlThisMgr_sptr->GetMessageTypeName(), _mgrControlThisMgr_sptr);
	_mgrControlThisMgr_sptr->Register_SomethingPublished(boost::bind(&RabitManager::WakeUpManagerEH, this));

        //This Manager Status and Stats Message Name:  "MngName:ManagerStatusMessage"
        tmpStr = _managerName + ":ManagerStatusMessage";
        _mgrStatus_sptr = std::make_shared<ManagerStatusMessage>(tmpStr);
        this->AddPublishSubscribeMessage(_mgrStatus_sptr->GetMessageTypeName(), _mgrStatus_sptr);

        _mgrStatus_sptr->ManagerName = _managerName;
        _mgrStatus_sptr->RunningState = ManagerStatusMessage::MgrState_Startup;
        _mgrStatus_sptr->PostMessage();

    }

    void RabitManager::WakeUpManagerEH()
    {
        //If the Manager is in the process of going to sleep at the
        //end of ExecuteUnitOfWork... we want to wait until that process
        //occurs before waking the Manager up.
        //If the manager is not sleeping... we set a flag letting the manager
        //know there is more to process... so don't go to sleep.

        std::unique_lock<std::mutex> wakeUpLock(_mutexWaitFor);
        _mgrWakeUpHit = true;
        if (_mgrIsAsleep)
        {
            ++_mgrStatus_sptr->ManagerStats.NumberOfWakeUpCallsWhileAsleep;
            _cvar.notify_one();
        }
        else
        {
            ++_mgrStatus_sptr->ManagerStats.NumberOfWakeUpCallsWhileAwake;
        }
    }

    //Get the lower value of between the two Manager Control Messages.
    float RabitManager::GetStatsPublishTime()
    {
        float time = 1000000.0;
        time = _mgrControlAllMgrs_sptr->PulishMgrStatsTime_Sec < time ? _mgrControlAllMgrs_sptr->PulishMgrStatsTime_Sec : time;
        time = _mgrControlThisMgr_sptr->PulishMgrStatsTime_Sec < time ? _mgrControlThisMgr_sptr->PulishMgrStatsTime_Sec : time;
        return time;
    }

    //The Manager Main is an infinite loop which continuously calls the
    //ExecuteUnitOfWork() method.  After calling the ExecuteUnitOfWork() method
    //the manager will go to sleep for the established number of milliseconds provided
    //in the _wakeupTimeDelayMSec parameter.
    //Events can be used to wake up the manager earlier than the given _wakeupTimeDelayMSec.
    void RabitManager::managerMain()
    {
        bool exceptionOccurred = false;
        bool mcAllResetStats = false;
        bool mcThisResetStats = false;
        bool msgChanged = false;

        double publishStatsTimeSec = GetStatsPublishTime();
        double currentMgrTimeSec = 0;
        double lastPublishStatsTimeSec = 0;

        publishStatsTimeSec = GetStatsPublishTime();

        Startup();
        _mgrStatus_sptr->RunningState = ManagerStatusMessage::MgrState_Running;
        _mgrStatus_sptr->PostMessage();

        _mgrTotalTimeSec.reset();

        while (!_shutdownManager 
		and !_mgrControlAllMgrs_sptr->GetShutdownManager()
		and !_mgrControlThisMgr_sptr->GetShutdownManager() )
        {
            exceptionOccurred = false;
            try
            {
                msgChanged = _mgrControlAllMgrs_sptr->FetchMessage();
                msgChanged |= _mgrControlThisMgr_sptr->FetchMessage();
                if (msgChanged)
                {
                    if (_mgrControlAllMgrs_sptr->ResetMgrStatsToggle != mcAllResetStats
                        || _mgrControlThisMgr_sptr->ResetMgrStatsToggle != mcThisResetStats)
                    {
                        _mgrStatus_sptr->ManagerStats.Clear();
                        mcAllResetStats = _mgrControlAllMgrs_sptr->ResetMgrStatsToggle;
                        mcThisResetStats = _mgrControlThisMgr_sptr->ResetMgrStatsToggle;
                        //Reset the Manager Run-Time clock;
                        _mgrTotalTimeSec.reset();
                    }
                    publishStatsTimeSec = GetStatsPublishTime();
                }

                ++_mgrStatus_sptr->ManagerStats.TotalNumberOfExecuteCycles;
                ++_mgrStatus_sptr->ManagerStats.NumberOfExecuteCycles;

                _execUnitOfWorkTime.tick();
                ExecuteUnitOfWork();
                _execUnitOfWorkTime.tick();

                _sleepTime.tick();
                if (_wakeupTimeDelayMSec > 0 && !_mgrWakeUpHit)
                {
                    std::unique_lock<std::mutex> wakeUpLock(_mutexWaitFor);
                    if (!_mgrWakeUpHit)
                    {
                        //Only go to sleep if we have not had a _mgrWakeUpHit
                        _mgrIsAsleep = true;
                        _cvar.wait_for(wakeUpLock, std::chrono::milliseconds(_wakeupTimeDelayMSec));
                        _mgrIsAsleep = false;
                    }
                }
                _sleepTime.tick();
                //Alway clear the Wakeup Hit Flag.
                _mgrWakeUpHit = false;

                //Update Manager Stats
                _mgrTotalTimeSec.captureTime();
                currentMgrTimeSec = _mgrTotalTimeSec.getTimeElapsed();
                _mgrStatus_sptr->ManagerStats.TimeSinceLastStatsReset_Sec = currentMgrTimeSec;
                _mgrStatus_sptr->ManagerStats.UpdateExeUnitOfWorkTimeStats(_execUnitOfWorkTime.getTimeElapsed());
                _mgrStatus_sptr->ManagerStats.UpdateSleepTimeStats(_sleepTime.getTimeElapsed());

                if( (currentMgrTimeSec - lastPublishStatsTimeSec) >  publishStatsTimeSec)
                {
                    lastPublishStatsTimeSec = currentMgrTimeSec;
                    _mgrStatus_sptr->PostMessage();
                    std::cout << _managerName << "_MgrStats:" << std::endl
                              << _mgrStatus_sptr->ManagerStats.ToString() << std::endl;
                }

            }
            catch (std::exception &e)
            {
                std::cout << "Manager: " << _managerName
                          << " with exception: " << e.what();

                exceptionOccurred = true;
            }
            if(exceptionOccurred)
            {
                if (!_mgrStatus_sptr->ErrorCondition)
                {
                _mgrStatus_sptr->ErrorCondition = true;
                _mgrStatus_sptr->ErrorCode = 1;
                _mgrStatus_sptr->PostMessage();
                }
            }
            else
            {
                if (_mgrStatus_sptr->ErrorCondition)
                {
                    _mgrStatus_sptr->ErrorCondition = false;
                    _mgrStatus_sptr->ErrorCode = 0;
                    _mgrStatus_sptr->PostMessage();
                }
            }

        }

        _mgrStatus_sptr->RunningState = ManagerStatusMessage::MgrState_Shutdown;
        _mgrStatus_sptr->PostMessage();

        Shutdown();

    }

}
