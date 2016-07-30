/* ****************************************************************
 * Rabit Multi-Threaded Management System
 * Athrs: Randal Direen PhD
 *        Harry Direen PhD
 * www.direentech.com
 * Date: June 2016
 *
 *******************************************************************/

#include "ManagerStats.h"

namespace Rabit
{

    void ManagerStats_t::Clear()
    {
        NumberOfExecuteCycles = 0;
        TimeSinceLastStatsReset_Sec = 0.0;
        MinExecuteUnitOfWorkTime_Sec = 0.0;
        MaxExecuteUnitOfWorkTime_Sec = 0.0;
        AveExecuteUnitOfWorkTime_Sec = 0.0;
        TotalExecuteUnitOfWorkTime_Sec = 0.0;
        MinSleepTime_Sec = 0.0;
        MaxSleepTime_Sec = 0.0;
        AveSleepTime_Sec = 0.0;
        TotalSleepTime_Sec = 0.0;
        NumberOfWakeUpCallsWhileAsleep = 0;
        NumberOfWakeUpCallsWhileAwake = 0;

    }

    void ManagerStats_t::UpdateExeUnitOfWorkTimeStats(double euwTimeSec)
    {
        if( MinExecuteUnitOfWorkTime_Sec <= 0.0)
        {
            MinExecuteUnitOfWorkTime_Sec = euwTimeSec;
        }
        else if(euwTimeSec < MinExecuteUnitOfWorkTime_Sec)
        {
            MinExecuteUnitOfWorkTime_Sec = euwTimeSec;
        }

        if( euwTimeSec > MaxExecuteUnitOfWorkTime_Sec )
        {
            MaxExecuteUnitOfWorkTime_Sec = euwTimeSec;
        }

        TotalExecuteUnitOfWorkTime_Sec += euwTimeSec;
        if( NumberOfExecuteCycles > 1 )
        {
            AveExecuteUnitOfWorkTime_Sec = TotalExecuteUnitOfWorkTime_Sec / (float)NumberOfExecuteCycles;
        }
    }

    void ManagerStats_t::UpdateSleepTimeStats(double sleepTimeSec)
    {
        if( MinSleepTime_Sec <= 0.0)
        {
            MinSleepTime_Sec = sleepTimeSec;
        }
        else if(sleepTimeSec < MinSleepTime_Sec)
        {
            MinSleepTime_Sec = sleepTimeSec;
        }

        if( sleepTimeSec > MaxSleepTime_Sec )
        {
            MaxSleepTime_Sec = sleepTimeSec;
        }

        TotalSleepTime_Sec += sleepTimeSec;
        if( NumberOfExecuteCycles > 1 )
        {
            AveSleepTime_Sec = TotalSleepTime_Sec / (double)NumberOfExecuteCycles;
        }
    }


    std::string ManagerStats_t::ToString() const
    {
        std::ostringstream os;
        os << "NumberNumberOfExecuteCycles=" << NumberOfExecuteCycles
           << ", TimeSinceLastStatsReset_Sec=" << TimeSinceLastStatsReset_Sec
           << ", MinExecuteUnitOfWorkTime_Sec=" << MinExecuteUnitOfWorkTime_Sec
           << ", MaxExecuteUnitOfWorkTime_Sec=" << MaxExecuteUnitOfWorkTime_Sec
           << ", AveExecuteUnitOfWorkTime_Sec=" << AveExecuteUnitOfWorkTime_Sec
           << ", MinSleepTime_Sec=" << MinSleepTime_Sec
           << ", MaxSleepTime_Sec=" << MaxSleepTime_Sec
           << ", AveSleepTime_Sec=" << AveSleepTime_Sec
           << ", NumberOfWakeUpCallsWhileAsleep=" << NumberOfWakeUpCallsWhileAsleep
           << ", NumberOfWakeUpCallsWhileAwake=" << NumberOfWakeUpCallsWhileAwake
           << std::endl;
        return os.str();
    }


}
