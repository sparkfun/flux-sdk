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

// What this system does
//
// This maintains a list of "jobs" that are ordered based on time when they should
// execute - soonest to latest.
//
// Each job contains a period to wait, a callback handler that should be called
// when the time for the job has expired.
//
// Jobs can be *loops* - re-queue's itself once executed, or one-shot - runs once.
//
// A module/subsystem that wants to use the job queue, creates a job object and
// adds it to the queue. The job-queue adds the job at the correct entry point based
// on it's period and when it was added (add time + job period).
//
// The system loop - framework loop - calls the loop method on the job queue. In
// the loop the following happens:
//
//    - If a job's time has expired, the job's handler method is called
//    - If the job isn't a one-shot job, it is re-added to the queue using
//      the jobs previous time + the job period.
//
// The original system of providing a timing/update call to a subsystem/device
// was based on just calling a loop() method on each Action and Device in the
// system. This was actually wasteful since:
//
//     - A majority of items didn't implement a loop() method, so a no-op
//       method from the base class was called
//     - Each system loop would call everything, so even if a object was
//       monitoring for a timed event, most times it wasn't required and
//       the call was returned.
//
// Moving to the job queue, object timed update method are only called when
// desired. If no updates are needed in the system, only one loop() method is called
// - the loop method on the job queue.
//
// Helper Functions
//
//    The following functions are provided to simplify use of the job queue
//
//        flxAddJobToQueue(flxJob &theJob)
//           Adds the provided job to the job queue. If the job exists, its not added.
//
//        flxUpdateJobInQueue(flxJob &theJob)
//           Updates the position in the job queue if the period changed. Basically the job is
//           removed from the queue and re-added. If the job wasn't in the job queue, it's just added.
//
//        flxRemoveJobFromQueue(flxJob &theJob)
//           If the job is in the job queue, it is removed.

#include "flxCoreJobs.h"
#include <Arduino.h>
#include <vector>

//------------------------------------------------------------------
// Global object - we only have one queue - it's a singleton

_flxJobQueue &flxJobQueue = _flxJobQueue::get();

//------------------------------------------------------------------
// overall job queue object
//
_flxJobQueue::_flxJobQueue() : _running{false}
{
}

//------------------------------------------------------------------
// start
//
bool _flxJobQueue::start(void)
{
    if (_running)
        return true;

    // okay, we need to transition from an idle state to a running state
    //
    // Any added jobs were added to the internal queue with no ordering
    // based on operational timing.
    //
    // Now order the jobs based on delta time needs based on *start* time
    std::vector<flxJob *> theJobs;

    // stash our jobs
    for (auto aJob : _jobQueue)
        theJobs.push_back(aJob.second);

    // now clear out the map, and add everything back with proper timing
    _jobQueue.clear();

    for (auto aJob : theJobs)
        addJob(*aJob);

    _running = true;

    return true;
}
//------------------------------------------------------------------
// stop
//
void _flxJobQueue::stop()
{
    // stop the job queue
    _running = false;
}

//------------------------------------------------------------------
// Any jobs to dispatch?
//
void _flxJobQueue::dispatchJobs(void)
{
    if (!_running)
        return;

    // The Plan
    //
    // The job queue is sorted based on time to call/dispatch a job handler.
    // Loop over the queue and dispatch handlers to all events that time values (keys)
    // are less than ticks.

    flxJob *theJob;
    uint32_t tNext;

    // our time cutoff
    uint32_t ticks = millis();

    for (auto it = _jobQueue.begin(); it != _jobQueue.end(); /*nothing*/)
    {
        // past what is available?
        if (it->first > ticks)
            break;

        theJob = it->second;

        // call the job's handler. Doing this here, allows the target to modify job period if needed
        theJob->callHandler();

        // normally the base of the next period timeout is the current event timeout - this
        // keeps timed sequences on a predicable schedule - absorbing small delays
        // that occur during operation by the event delta.
        //
        // However, if the system is paused (menu in use), or the time period between an event is
        // less than the minimum loop interval timeout (high-speed logging), the next time period
        // end is less than current ticks. If this is the case, fast forward the base by N * period()
        //
        // For a majority of jobs, the next event tick number is this event tick number + job period.
        tNext = it->first + theJob->period();

        // high-speed or timed out system (next is <= current ticks) - re-base the period to next valid time
        if (tNext <= ticks)
            tNext = it->first + (((ticks - it->first) / theJob->period()) + 1) * theJob->period();

        // remove this item from the job queue head,
        // Note - erase returns the iterator to next item in container
        it = _jobQueue.erase(it);

        // add back if not a one shot job
        if (!theJob->oneShot())
            _jobQueue.insert(std::pair<uint32_t, flxJob *>(tNext, theJob));
    }
}
//------------------------------------------------------------------
// Find a job in the job queue
//
auto _flxJobQueue::findJob(flxJob &theJob) -> decltype(_jobQueue.end())
{
    auto itJob = _jobQueue.begin();

    while (itJob != _jobQueue.end())
    {
        if (itJob->second == &theJob)
            break;

        itJob++;
    }
    return itJob;
}
//------------------------------------------------------------------
//
void _flxJobQueue::addJob(flxJob &theJob)
{
    // Is this job in our queue already
    if (_jobQueue.size() == 0 || findJob(theJob) == _jobQueue.end())
    {
        // Add this job to the job queue (map) - make sure it has some sane period
        if (theJob.period() > 0)
            _jobQueue.insert(std::pair<uint32_t, flxJob *>(millis() + theJob.period(), &theJob));
    }
}
//------------------------------------------------------------------
// remove a job
//
void _flxJobQueue::removeJob(flxJob &theJob)
{
    auto itJob = findJob(theJob);

    // do we know of this job?
    if (itJob != _jobQueue.end())
        _jobQueue.erase(itJob);
}

//------------------------------------------------------------------
// update a job
//
void _flxJobQueue::updateJob(flxJob &theJob)
{
    // the job needs to be reset it in the current job (priority) queue.
    //
    // To do this - just remove it and the add it back.

    removeJob(theJob);
    addJob(theJob);
}
//------------------------------------------------------------------
// dump out the contents of the queue
//
void _flxJobQueue::dump(void)
{
    for (auto aJob : _jobQueue)
        flxLog_I("\t %u\t%s", aJob.first, aJob.second->name());
}
//------------------------------------------------------------------
//  loop()
//
//  Called by the system -- from `loop`
//
bool _flxJobQueue::loop(void)
{
    // if running, dispatch jobs
    if (_running)
        dispatchJobs();

    return false;
}
//------------------------------------------------------------------
// Easy to use functions
//------------------------------------------------------------------
//
// Add to the queue
void flxAddJobToQueue(flxJob &theJob)
{
    flxJobQueue.addJob(theJob);
}
//------------------------------------------------------------------
// Update a job in the queue
//
void flxUpdateJobInQueue(flxJob &theJob)
{
    flxJobQueue.updateJob(theJob);
}
//------------------------------------------------------------------
// Remove a job
//
void flxRemoveJobFromQueue(flxJob &theJob)
{
    flxJobQueue.removeJob(theJob);
}