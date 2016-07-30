//
// Created by wavelet on 7/29/16.
//

#include "RabitStopWatch.h"

namespace Rabit
{
    RabitStopWatch::RabitStopWatch()
    {
        oldTime = std::chrono::high_resolution_clock::now();
        newTime = oldTime;
    }

    void RabitStopWatch::reset()
    {
        oldTime = std::chrono::high_resolution_clock::now();
        newTime = oldTime;
    }

    void RabitStopWatch::tick()
    {
        oldTime = newTime;
        newTime = std::chrono::high_resolution_clock::now();
    }

    void RabitStopWatch::start()
    {
        oldTime = std::chrono::high_resolution_clock::now();
    }

    void RabitStopWatch::stop()
    {
        newTime = std::chrono::high_resolution_clock::now();
    }

    void RabitStopWatch::captureTime()
    {
        newTime = std::chrono::high_resolution_clock::now();
    }

    double RabitStopWatch::getTimeElapsed() const
    {
        using std::chrono::duration_cast;
        using std::chrono::duration;
        return (duration_cast<duration<float>>(newTime - oldTime)).count();
    }

}
