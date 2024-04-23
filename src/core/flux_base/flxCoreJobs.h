/*
 *---------------------------------------------------------------------------------
 *
 * Copyright (c) 2022-2024, SparkFun Electronics Inc.  All rights reserved.
 * This software includes information which is proprietary to and a
 * trade secret of SparkFun Electronics Inc.  It is not to be disclosed
 * to anyone outside of this organization. Reproduction by any means
 * whatsoever is  prohibited without express written permission.
 *
 *---------------------------------------------------------------------------------
 */

// How this works - SEE the impl/CPP file

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
    flxJob() : _name{nullptr}, _one_shot{false}
    {
    }

    flxJob(const char *name, uint32_t in_period) : _name{name}, _period{in_period}
    {
    }

    template <typename T>
    void setup(const char *name, uint32_t in_period, T *inst, void (T::*func)(), bool bOneShot = false)
    {
        setName(name);
        setPeriod(in_period);
        setHandler(inst, func);
        setOneShot(bOneShot);
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

    inline uint32_t period(void)
    {
        return _period;
    }

    const char *name(void)
    {
        return _name;
    }
    void setName(const char *name)
    {
        _name = name;
    }
    void setOneShot(bool bOneShot)
    {
        _one_shot = bOneShot;
    }

    inline bool oneShot(void)
    {
        return _one_shot;
    }

  private:
    // handler
    std::function<void()> _handler;

    const char *_name;
    // Time delta in MS
    uint32_t _period;

    bool _one_shot;
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

    void addJob(flxJob &);
    void removeJob(flxJob &);
    void updateJob(flxJob &);

    bool loop(void);

    bool start(void);
    void stop(void);

    void dump(void);

  private:
    _flxJobQueue();

    void dispatchJobs(void);

    bool _running; // used to flag if the queue is running

    // use a multi-map for the queue - since entries can have the same key (time value)
    std::multimap<uint32_t, flxJob *> _jobQueue;

    auto findJob(flxJob &theJob) -> decltype(_jobQueue.end());
};
extern _flxJobQueue &flxJobQueue;

//----------------------------------------------------------------------------------------------------
// User exposed convenience function help with jobs
//
void flxAddJobToQueue(flxJob &theJob);

void flxUpdateJobInQueue(flxJob &theJob);

void flxRemoveJobFromQueue(flxJob &theJob);