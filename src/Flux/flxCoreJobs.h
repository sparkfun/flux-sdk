/*
 *---------------------------------------------------------------------------------
 *
 * Copyright (c) 2022-2023, SparkFun Electronics Inc.  All rights reserved.
 * This software includes information which is proprietary to and a
 * trade secret of SparkFun Electronics Inc.  It is not to be disclosed
 * to anyone outside of this organization. Reproduction by any means
 * whatsoever is  prohibited without express written permission.
 *
 *---------------------------------------------------------------------------------
 */

#pragma once

#include "flxCoreLog.h"

#include <functional>
#include <map>
#include <stdint.h>
#include <vector>

//-----------------------------------------------------------------
// Define our Job
//
//  A job defines the following:
//     - a time period - this time period is triggered repeatedly
//     - A method to call after the time period
//
class flxJob
{

  public:
    flxJob() : _compress{true}
    {
    }

    flxJob(uint32_t in_period) : _period{in_period}
    {
    }

    template <typename T> void setup(uint32_t in_period, T *inst, void (T::*func)(), bool bCompress = true)
    {
        setPeriod(in_period);
        setHandler(inst, func);
        setCompress(bCompress);
    }

    void callHandler(void)
    {
        if (_handler)
            _handler();
    }
    // what method to call when time expired
    template <typename T> void setHandler(T *inst, void (T::*func)())
    {
        if (!inst || !func)
            return;

        _handler = [=]() { // uses a lambda for the callback
            (inst->*func)();
        };
    }

    void setPeriod(uint32_t in_period)
    {
        if (in_period > 0)
            _period = in_period;
    }

    uint32_t period(void)
    {
        return _period;
    }

    void setCompress(bool bCompress)
    {
        _compress = bCompress;
    }

    bool compress(void)
    {
        return _compress;
    }

  private:
    // handler
    std::function<void()> _handler;

    // Time delta in MS
    uint32_t _period;

    bool _compress;
};

////////////////////////////////////////////////////////////////////////////////
// Job Queue/Timer based
////////////////////////////////////////////////////////////////////////////////
//
//  This takes the next job and adds it to a work queue after the specified time
//  of the job has expired.
//
class _flxJobQueue
{

  public:
    // _flxJobQueue is a singleton
    static _flxJobQueue &get(void)
    {
        static _flxJobQueue instance;
        return instance;
    }
    // This is a singleton class - so delete copy & assignment constructors
    _flxJobQueue(_flxJobQueue const &) = delete;
    void operator=(_flxJobQueue const &) = delete;

    // internal timer callback
    void _timerCB(void);

    void addJob(flxJob &);
    void removeJob(flxJob &);
    void updateJob(flxJob &);

    bool loop(void);

  private:
    _flxJobQueue();

    void checkJobQueue(void);
    void dispatchJobs(void);
    void updateTimer(void);

    std::map<uint32_t, flxJob *> _jobQueue;
    auto findJob(flxJob &theJob) -> decltype(_jobQueue.end());
};
extern _flxJobQueue &flxJobQueue;

//----------------------------------------------------------------------------------------------------
// User exposed convenience function help with jobs
//
void flxAddJobToQueue(flxJob &theJob);

void flxUpdateJobInQueue(flxJob &theJob);