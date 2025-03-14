/* ****************************************************************
 * Athr: Harry Direen, PhD
 * Date: Dec. 2016
 *
 *
 * SystemTime:  a GPS synchronized time.
 * GPS synchronized time provides a standard, synchronized time between
 * UAVs and other systems.  UAVs typically have a GPS system which
 * can be used to synchronized the system clock.
 * It is difficult to ensure the local processor clock is tightly
 * synchronized with GPS time... so an offset is used in this clock
 * which can help synchronize the processor clock with GPS Time.
 *
 * Note:  One of the primary reasons for using GPS time in seconds is that
 * UAV position information (Lat/Lon, speeds, attitude) typically are
 * come from an IMU or autopilot that timestamps the data with GPS time
 * in seconds.  Image processing and Mission Control uses the position information
 * along with current times to handler their processing.  So it is impartive
 * that Image processing and Mission control have access to a GPS synchronized
 * clock.  Since all UXVs in the system typically use GPS for position information,
 * this becomes a good standard time to use within the system and between
 * the UXVs.
 *
 * A typical UAV in mission is not connected to the internet and
 * cannot synchronize its clock to UTC time... so synchronizing to the GPS
 * time makes the most since.
 *
 * UTC and GPS clocks only differ by Leap-Seconds since Jan. 6, 1980, which
 * as of July 1, 2015 is 17 seconds where:
 * GPS seconds = UTC + leap seconds.
 *
 * A Ground station computer can synchonize its clock to UTC time and compute
 * GPS time base on the algorithms in this class.  The TimeSyncOffset will simply
 * be set to zero... which it is by default.
 *
 * Times are represented in Universal Time Coordinated (UTC) in seconds
 * since Jan. 6, 1980 (the GPS reference start point) using
 * IEEE double precision floating point numbers.
 * Time in secondes is what is most important to a UAV in flight cooridated
 * with other UAVs and ground resources.
 *
 * The Unix Epoc and time is typically seconds relative to Jan. 1, 1970
 *
 * GPS Time is in seconds relative to Jan. 6, 1980
 * https://confluence.qps.nl/display/KBE/UTC+to+GPS+Time+Correction
 *
 * Leap-Second offset is 17seconds, as of July 1, 2015
 * GPS seconds = UTC + leap seconds.
 *
 * C++ chrono tutorial reference:
 * http://www.informit.com/articles/article.aspx?p=1881386&seqNum=2
 *
 *******************************************************************/

#ifndef VIDERE_DEV_SYSTEMTIME_H
#define VIDERE_DEV_SYSTEMTIME_H

#include <chrono>
#include <ctime>
#include <iostream>
#include <string>

namespace Rabit
{

    //This is a Singleton Class as there should be only one System Time Clock.
    class SystemTimeClock
    {

    private:
        static SystemTimeClock *_systemTimeClockPtr;

        //The time in seconds between the system clock and
        //the actual GPS time.
        //This time is added to the system clock to get the corrected
        //Time.  This value can be positive or negative.
        //The default value is zero.
        double _TimeSyncOffset = 0.0;

        //UInt32 _SyncCounter = 0;
        bool _IsTimeInSyncWithGps = false;

        std::chrono::seconds _leapSecondOffset;
        std::chrono::seconds _systemClockEpocSec;
        std::chrono::seconds _GPSEpocSec;
        std::chrono::seconds _delSysClockEpocGPSEpocSec;

        //Private Constructor
        SystemTimeClock();


    public:
        static SystemTimeClock *GetSystemTimeClock()
        {
            if(!_systemTimeClockPtr)
            {
                _systemTimeClockPtr = new SystemTimeClock();
            }
            return _systemTimeClockPtr;
        }

        void SetGpsTimeOffset();

        double GetLeapSecondOffset()
        {
            return (double)_leapSecondOffset.count();
        }

        void SetLeapSecondOffset(double leapSec)
        {
            leapSec = leapSec < 0 ? 0 : leapSec > 100.0 ? 100.0 : leapSec;
            _leapSecondOffset = std::chrono::seconds((int)leapSec);
        }

        //Offset between the processor clock and the GPS time.
        //If the processor clock is set properly, this should be
        //only a few seconds.
        double GetTimeSyncOffset()
        {
            return _TimeSyncOffset;
        }

        //Offset between the processor clock and the GPS time.
        //If the processor clock is set properly, this should be
        //only a few seconds.
        //This time is added to the system clock to get the corrected
        //time.  The value can be positive or negative.
        //If the system clock is running slow, this offset will be positive.
        //If the system clock is ahead of the actual time, this value will be
        //negative.
        void SetTimeSyncOffset(double offsetSec)
        {
            _TimeSyncOffset = offsetSec;
        }

        /// <summary>
        /// Gets the current estimated GPS Time in seconds.
        /// Note that the return value is not actually from a GPS.
        /// </summary>
        /// <returns></returns>
        double GetCurrentGpsTimeInSeconds();


        //The UTC Time is the same as the GPS time... only the
        //Leap-seconds has not been removed.
        //The time is still corrected based upon a GPS clock if available.
        double GetCurrentUTCTimeInSeconds();

        //This routine converts a captured std::chrono::system_clock time point
        //to the corrected GPS time.  This routine is primarily used for test
        //purposes.
        double ConvertTimePointToGpsTimeInSeconds(std::chrono::system_clock::time_point tp);

        //This method is a second approach to geting the GPS time... is is used
        //for test purposes to verify the standard method:  ConvertTimePointToGpsTimeInSeconds()
        //The time resolution of this method is only 1 second while the time resolution
        //of ConvertTimePointToGpsTimeInSeconds() is microseconds.
        double ConvertTimePointToGpsTimeInSecondsMethod2Resolution1Second(std::chrono::system_clock::time_point tp);

        //This routine converts a gpsTime in seconds back to a system clock timepoint.
        //The GPS correction factor is left in the time, so this should represent
        //corrected or synchronized time.
        std::chrono::system_clock::time_point
        GpsTimeSecondsToSystemClockTimepoint(double gpsTimeSec);

        //Get a std::tm struct with the UTC based time from
        //a GPS Time in seconds.
        std::tm GetUTCTimeStructFromGpsTimeStamp(double gpsTimeSec);

        //Get a time string with the UTC based time from
        //a GPS Time in seconds.
        std::string GetUTCTimeStringFromGpsTimeStamp(double gpsTimeSec);

        //Get a std::tm struct with the local time (corrected for timezone) from
        //a GPS Time in seconds.
        std::tm GetLocalTimeStructFromGpsTimeStamp(double gpsTimeSec);

        //Get a time string with the Local time (corrected for timezone) from
        //a GPS Time in seconds.
        std::string GetLocalTimeStringFromGpsTimeStamp(double gpsTimeSec);


        // convert calendar time to timepoint of system clock
        std::chrono::seconds
        DurationSecondsFromUTCEpoc(int year, int mon, int day,
                                   int hour, int min, int sec=0);

        std::chrono::seconds ToUTCSeconds(const tm *ptm);

        std::string clockTimepointAsString (const std::chrono::system_clock::time_point& tp);


        void printSystemClockCharacteristics();

    };




}
#endif //VIDERE_DEV_SYSTEMTIME_H
