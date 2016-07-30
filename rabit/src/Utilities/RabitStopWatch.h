/********************************************************
 * A high resolution Stopwatch class
 * This code was obtained from:
 *      (https://github.com/st3w/RabitStopWatch)
 *
 *
 *
 * The code has been modified to suit the needs of Rabit
 *
 *
 * Usage:
 *  RabitStopWatch s;
 *      //Other stuff
 *      s.tick();
 *      //Timed stuff here
 *      s.tick();
 *      //Other stuff
 *      float dt = s.getTimeElapsed();
 *******************************************************/

#ifndef RABITSTOPWATCH_H
#define RABITSTOPWATCH_H

#include <chrono>

namespace Rabit
{
    class RabitStopWatch
    {
    public:
        RabitStopWatch();

        //Use tick at the start of a process to be measured
        //and again at the end of the process to be measured.
        void tick();

        //Reset time to zero
        void reset();

        //Start a new Time Period... same as tick().
        void start();

        //Captures the current time
        void stop();

        //Capture the current time without reseting the old time.
        //tick() captures the new time and resets the old time.
        void captureTime();

        //Default:  Floating point time in seconds
        double getTimeElapsed() const;

    private:
        std::chrono::high_resolution_clock::time_point newTime, oldTime;
    };


}

#endif // RABITSTOPWATCH_H