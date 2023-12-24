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

#include "flxCoreJobs.h"
#include <Arduino.h>
#include <vector>

const uint32_t kJobDispatchMSBuffer = 2;
//------------------------------------------------------------------
// Global object - we only have one queue

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
    // order the jobs based on delta time needs.
    std ::vector<flxJob *> theJobs;

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
    // are less than ticks + buffer.
    //
    // Jobs are reoccurring, so once a job is dispatched, it's placed at the end of the
    // job list at current tick about + job period

    std ::vector<flxJob *> theJobs;

    // our time cutoff
    uint32_t ticks = millis() + kJobDispatchMSBuffer;

    for (auto it = _jobQueue.begin(); it != _jobQueue.end(); /*nothing*/)
    {
        // past what is available?
        if (it->first > ticks)
            break;

        // save job to our dispatch list
        theJobs.push_back(it->second);

        // remove this item from the job queue head,
        // Note - erase returns the iterator to next item in container
        it = _jobQueue.erase(it);
    }

    // dispatch jobs if needed and then add back to list
    for (auto theJob : theJobs)
    {
        // re-queue our job in the job task timer list if it's not a one-shot
        if (!theJob->oneShot())
            _jobQueue[ticks + theJob->period()] = theJob;

        // dispatch the job
        theJob->callHandler();
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
        // Add this job to the job queue (map)
        if (theJob.period() > 0)
            _jobQueue[millis() + theJob.period()] = &theJob;
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