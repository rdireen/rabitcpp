/* ****************************************************************
 * Rabit Multi-Threaded Management System
 * Athrs: Randal Direen PhD
 *        Harry Direen PhD
 * www.direentech.com
 * Date: June 2016
 *
 *******************************************************************/

#ifndef RABIT_DEV_MANAGERSTATS_H
#define RABIT_DEV_MANAGERSTATS_H

#include <string>
#include <memory>
#include <sstream>

namespace Rabit
{
    struct ManagerStats_t
    {
    public:
        //Total number of Execution cycles since the start of the
        //manager... this value is not reset or cleared.
        int TotalNumberOfExecuteCycles = 0;

        //Number Of Execute Cycles since the last time the Stats were
        //cleared or reset.
        int NumberOfExecuteCycles = 0;

        double TimeSinceLastStatsReset_Sec = 0.0;

        //Min, Max and Average ExecuteUnitOfWorkTime
        //since the last time the Stats were cleared
        double MinExecuteUnitOfWorkTime_Sec = 0.0;
        double MaxExecuteUnitOfWorkTime_Sec = 0.0;
        double AveExecuteUnitOfWorkTime_Sec = 0.0;
        double TotalExecuteUnitOfWorkTime_Sec = 0.0;

        //Min, Max and Average Sleep time... Time the
        //manager loop is in the sleep state.
        //since the last time the Stats were cleared
        double MinSleepTime_Sec = 0.0;
        double MaxSleepTime_Sec = 0.0;
        double AveSleepTime_Sec = 0.0;
        double TotalSleepTime_Sec = 0.0;

        //The number of times the WakeUpManagerEH is called
        //since the last time the Stats were cleared
        int NumberOfWakeUpCallsWhileAsleep = 0;
        int NumberOfWakeUpCallsWhileAwake = 0;


        void Clear();

        void UpdateExeUnitOfWorkTimeStats(double euwTimeSec);

        void UpdateSleepTimeStats(double sleepTimeSec);

        std::string ToString() const;

    };
}

#endif //RABIT_DEV_MANAGERSTATS_H
