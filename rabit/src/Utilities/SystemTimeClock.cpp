/* ****************************************************************
 * Athr: Harry Direen, PhD
 * Date: Dec. 2016
 *
 *******************************************************************/

#include "SystemTimeClock.h"
#include <time.h>                   // needed for struct tm

using namespace std;
using namespace chrono;


namespace Rabit
{

    //Declare the static SystemTimeClock
    SystemTimeClock *SystemTimeClock::_systemTimeClockPtr;

    //Private Constructor
    SystemTimeClock::SystemTimeClock()
    {
        _leapSecondOffset = chrono::seconds(17);
        SetGpsTimeOffset();
    }


    /// <summary>
    /// Gets the current estimated GPS Time in seconds.
    /// Note that the return value is not actually from a GPS.
    /// </summary>
    /// <returns></returns>
    double SystemTimeClock::GetCurrentGpsTimeInSeconds()
    {
        double gpsTimeSec = 0;
        system_clock::time_point tp;
        tp = system_clock::now();
        tp = tp - _delSysClockEpocGPSEpocSec + _leapSecondOffset;
        std::chrono::duration<double> tdsec = tp.time_since_epoch();
        gpsTimeSec = tdsec.count() + _TimeSyncOffset;
        return gpsTimeSec;
    }

    //The UTC Time is the same as the GPS time... only the
    //Leap-seconds has not been removed.
    //The time is still corrected based upon a GPS clock if available.
    //Seconds since Jan 6, 1980
    double SystemTimeClock::GetCurrentUTCTimeInSeconds()
    {
        double utcTimeSec = 0;
        system_clock::time_point tp;
        tp = system_clock::now();
        tp = tp - _delSysClockEpocGPSEpocSec;
        std::chrono::duration<double> tdsec = tp.time_since_epoch();
        utcTimeSec = tdsec.count() + _TimeSyncOffset;
        return utcTimeSec;
    }

    //This routine converts a captured std::chrono::system_clock time point
    //to the corrected GPS time.  This routine is primarily used for test
    //purposes.
    double SystemTimeClock::ConvertTimePointToGpsTimeInSeconds(system_clock::time_point tp)
    {
        double gpsTimeSec;
        tp = tp - _delSysClockEpocGPSEpocSec + _leapSecondOffset;
        std::chrono::duration<double> tdsec = tp.time_since_epoch();
        gpsTimeSec = tdsec.count() + _TimeSyncOffset;
        return gpsTimeSec;
    }

    //This method is a second approach to geting the GPS time... is is used
    //for test purposes to verify the standard method:  ConvertTimePointToGpsTimeInSeconds()
    //The time resolution of this method is only 1 second while the time resolution
    //of ConvertTimePointToGpsTimeInSeconds() is microseconds.
    double SystemTimeClock::ConvertTimePointToGpsTimeInSecondsMethod2Resolution1Second(std::chrono::system_clock::time_point tp)
    {
        double gpsTimeSec;

        //Method one calcualtion... looses sub-seconds.
        std::time_t sct = system_clock::to_time_t(tp);
        std::tm *sctm = gmtime(&sct);
        chrono::seconds utcTsec = ToUTCSeconds(sctm);
        chrono::seconds gpsTsec = utcTsec - _delSysClockEpocGPSEpocSec + _leapSecondOffset;
        std::chrono::duration<double> gpsTdsec = gpsTsec;
        gpsTimeSec = gpsTdsec.count() + _TimeSyncOffset;
        return gpsTimeSec;
    }


    //This routine converts a gpsTime in seconds back to a system clock timepoint.
    //The GPS correction factor is left in the time, so this should represent
    //corrected or synchronized time.
    std::chrono::system_clock::time_point
    SystemTimeClock::GpsTimeSecondsToSystemClockTimepoint(double gpsTimeSec)
    {
        system_clock::time_point tp;
        //The time in seconds has a resolution down to the microsecond level
        //on linux and windows based computers.
        chrono::microseconds gpsTusec((int64_t)(gpsTimeSec * 1e6));
        tp = tp + gpsTusec;
        return tp;
    }


    //Get a std::tm struct with the UTC based time from
    //a GPS Time in seconds.
    std::tm SystemTimeClock::GetUTCTimeStructFromGpsTimeStamp(double gpsTimeSec)
    {
        chrono::seconds gpsDsec((long)gpsTimeSec);
        chrono::seconds sysDtime = gpsDsec - _leapSecondOffset + _delSysClockEpocGPSEpocSec;
        time_t tsec(sysDtime.count());
        std::tm *tms = gmtime(&tsec);
        return *tms;
    }

    //Get a time string with the UTC based time from
    //a GPS Time in seconds.
    string SystemTimeClock::GetUTCTimeStringFromGpsTimeStamp(double gpsTimeSec)
    {
        char buf[80];
        std::tm tms = GetUTCTimeStructFromGpsTimeStamp(gpsTimeSec);
#ifdef WINDOWS
        sprintf(buf, "%d-%02d-%02dT%02d:%02d:%02d", (tms.tm_year - 100 + 2000), (tms.tm_mon + 1), (tms.tm_mday), tms.tm_hour, tms.tm_min, tms.tm_sec);
#else
        //For some stupid reasons the Windows compiler is throwing an error on this.
        strftime (buf,80,"%Y-%m-%dT%0H:%M:%S", &tms);
#endif
        string tstr(buf);
        return tstr;
    }

    //Get a std::tm struct with the local time (corrected for timezone) from
    //a GPS Time in seconds.
    std::tm SystemTimeClock::GetLocalTimeStructFromGpsTimeStamp(double gpsTimeSec)
    {
        chrono::seconds gpsDsec((long) gpsTimeSec);
        chrono::seconds sysDtime = gpsDsec - _leapSecondOffset + _delSysClockEpocGPSEpocSec;
        time_t tsec(sysDtime.count());
        std::tm tms;
#ifdef WINDOWS
        std::tm *tmsptr;
        tmsptr = localtime(&tsec);
        tms = *tmsptr;
#else   //Linux
        localtime_r(&tsec, &tms);
#endif
        return tms;
    }

        //Get a time string with the Local time (corrected for timezone) from
    //a GPS Time in seconds.
    std::string SystemTimeClock::GetLocalTimeStringFromGpsTimeStamp(double gpsTimeSec)
    {
        char buf[80];
        std::tm tms = GetLocalTimeStructFromGpsTimeStamp(gpsTimeSec);
#ifdef WINDOWS
        sprintf(buf, "%d-%02d-%02dT%02d:%02d:%02d", (tms.tm_year - 100 + 2000), (tms.tm_mon + 1), (tms.tm_mday), tms.tm_hour, tms.tm_min, tms.tm_sec);
#else
        //For some stupid reasons the Windows compiler is throwing an error on this.
        strftime (buf,80,"%Y-%m-%dT%0H:%M:%S", &tms);
#endif
        string tstr(buf);
        return tstr;
    }

    void SystemTimeClock::SetGpsTimeOffset()
    {
        system_clock::time_point tp;
        //_systemClockEpocSec = system_clock::to_time_t(tp);
        std::time_t sct = system_clock::to_time_t(tp);
        std::tm *sctm = gmtime(&sct);
        _systemClockEpocSec = ToUTCSeconds(sctm);

        //GPS Time is in seconds relative to Jan. 6, 1980
        _GPSEpocSec = DurationSecondsFromUTCEpoc(1980, 1, 6, 0, 0, 0);
        _delSysClockEpocGPSEpocSec = _GPSEpocSec - _systemClockEpocSec;
    }

    // convert calendar time to timepoint of system clock
    chrono::seconds
    SystemTimeClock::DurationSecondsFromUTCEpoc (int year, int mon, int day,
                                                int hour, int min, int sec)
    {
        std::tm t;
        t.tm_sec = sec;        // second of minute (0 .. 59 and 60 for leap seconds)
        t.tm_min = min;        // minute of hour (0 .. 59)
        t.tm_hour = hour;      // hour of day (0 .. 23)
        t.tm_mday = day;       // day of month (0 .. 31)
        t.tm_mon = mon-1;      // month of year (0 .. 11)
        t.tm_year = year-1900; // year since 1900
        t.tm_isdst = -1;       // determine whether daylight saving time
        //std::time_t tt = std::mktime(&t);
        return ToUTCSeconds(&t);
    }



    // Conversion from UTC date to second, unsigned 32-bit Unix epoch version.
    // Written by François Grieu, 2015-07-21; public domain.
    //
    // my_mktime  converts from  struct tm  UTC to non-leap seconds since
    // 00:00:00 on the first UTC day of year 1970 (fixed).
    // It works from 1970 to 2105 inclusive. It strives to be compatible
    // with C compilers supporting // comments and claiming C89 conformance.
    //
    // input:   Pointer to a  struct tm  with field tm_year, tm_mon, tm_mday,
    //          tm_hour, tm_min, tm_sec set per  mktime  convention; thus
    //          - tm_year is year minus 1900
    //          - tm_mon is [0..11] for January to December, but [-2..14]
    //            works for November of previous year to February of next year
    //          - tm_mday, tm_hour, tm_min, tm_sec similarly can be offset to
    //            the full range [-32767 to 32768], as long as the combination
    //            with tm_year gives a result within years [1970..2105], and
    //            tm_year>0.
    // output:  Number of non-leap seconds since beginning of the first UTC
    //          day of year 1970, as an unsigned at-least-32-bit integer.
    //          The input is not changed (in particular, fields tm_wday,
    //          tm_yday, and tm_isdst are unchanged and ignored).
    chrono::seconds SystemTimeClock::ToUTCSeconds(const tm *ptm)
    {
        int m, y = ptm->tm_year;
        if ((m = ptm->tm_mon) < 2)
        {
            m += 12;
            --y;
        }
        int64_t ts = ((((int64_t) (y - 69) * 365u + y / 4 - y / 100 * 3 / 4 + (m + 2) * 153 / 5 - 446 +
                  ptm->tm_mday) * 24u + ptm->tm_hour) * 60u + ptm->tm_min) * 60u + ptm->tm_sec;
        return chrono::seconds(ts);
    }



    std::string SystemTimeClock::clockTimepointAsString(const std::chrono::system_clock::time_point& tp)
    {
        // convert to system time:
        std::string ts = "";
#ifndef WINDOWS
        std::time_t t = system_clock::to_time_t(tp);
        if(t >= 0)
        {
            //std::string ts = std::ctime(&t);// convert to calendar time
            std::string ts = std::asctime(gmtime(&t));
            ts.resize(ts.size()-1);         // skip trailing newline
        }
#endif
        return ts;
    }


    void SystemTimeClock::printSystemClockCharacteristics()
    {
        using namespace std;

        std::cout << "***************************************************" << std::endl;
        cout << "System Clock Characteristics:" << endl;

        cout << "- precision: ";
        // if time unit is less or equal one millisecond
        typedef typename system_clock::period P;// type of time unit

        if (ratio_less_equal<P, milli>::value)
        {
            // convert to and print as milliseconds
            typedef typename ratio_multiply<P, kilo>::type TT;

            cout << fixed << double(TT::num) / TT::den
                 << " milliseconds" << endl;
        }
        else
        {
            // print as seconds
            cout << fixed << double(P::num) / P::den << " seconds" << endl;
        }

        chrono::system_clock::time_point tp;
        std::cout << "Clock epoch: " << clockTimepointAsString(tp) << std::endl;

        // print minimum time of this system clock:
        tp = chrono::system_clock::time_point::min();
        std::cout << "min:   " << clockTimepointAsString(tp) << std::endl;

        // print maximum time of this system clock:
        tp = chrono::system_clock::time_point::max();
        std::cout << "max:   " << clockTimepointAsString(tp) << std::endl;

        std::chrono::duration<double> tsec0 = tp.time_since_epoch();
        std::cout << "Seconds Sence Epoc: " << tsec0.count() << std::endl;

        //From Unix Epoch of Jan. 1, 1970 to GPS Epoch there is 315964800 seconds:
        std::cout << "GPS Epoch minus System Clock Epoch: " << _delSysClockEpocGPSEpocSec.count() << std::endl;
        std::cout << "On Unix based system this difference shoud be: 315964800 seconds" << std::endl;

        double currGPSTime = GetCurrentGpsTimeInSeconds();
        std::string lclTimeStamp = GetLocalTimeStringFromGpsTimeStamp(currGPSTime);
        std::string utcTimeStamp = GetUTCTimeStringFromGpsTimeStamp(currGPSTime);

        std::cout << "Current GPS Time Seconds: " << currGPSTime << std::endl;
        std::cout << "Current Local Time and Date: " << lclTimeStamp << std::endl;
        std::cout << "Current UTC Time and Date: " << utcTimeStamp << std::endl;
        std::cout << "***************************************************" << std::endl << std::endl;

    }

}