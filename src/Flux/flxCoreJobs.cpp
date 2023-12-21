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

//////////////////////////////////////////
// Our FreeRTOS entities

// "Command Queue"
static QueueHandle_t hCmdQueue = NULL;
const uint16_t kCmdQueueSize = 12;
// Timer for job queue
static xTimerHandle hTimer;
const uint16_t kTimerPeriod = 100;

//------------------------------------------------------------------
// Global object - we only have one queue

_flxJobQueue &flxJobQueue = _flxJobQueue::get();

//------------------------------------------------------------------
// Callback for the FreeRTOS timer
//
static void _fluxJobQ_TimerCallback(xTimerHandle pxTimer)
{
    flxJobQueue._timerCB();
}
//------------------------------------------------------------------
// overall job queue object
//
_flxJobQueue::_flxJobQueue()
{
    // Create a timer, used to manage when to dispatch jobs
    hTimer = xTimerCreate("flux_job_q", kTimerPeriod / portTICK_RATE_MS, pdFALSE, (void *)0, _fluxJobQ_TimerCallback);
    if (hTimer == NULL)
    {
        // no timer - whoa
        flxLog_E("Job Queue - Failed to create timer");
        return;
    }

    hCmdQueue = xQueueCreate(kCmdQueueSize, sizeof(flxJob *));

    if (hCmdQueue == NULL)
    {
        flxLog_E("Job Queue - Failed to create queue");
        xTimerDelete(hTimer, 100);
        hTimer = NULL;
        return;
    }
}

//------------------------------------------------------------------
// Callback for the FreeRTOS timer
//
void _flxJobQueue::_timerCB(void)
{
    checkJobQueue();
    updateTimer();
}

//------------------------------------------------------------------
// update the timer
void _flxJobQueue::updateTimer(void)
{
    if (!hTimer || _jobQueue.size() == 0)
        return;

    // flxLog_I("JOBS: Updating Timer: %u", _jobQueue.begin()->second->period());
    // Set the timer to the period in the queue - millis(). Note the period is the key, which
    // are sorted in ascending order.
    xTimerChangePeriod(hTimer, (_jobQueue.begin()->first - millis()) / portTICK_RATE_MS, 10);
    xTimerReset(hTimer, 10);
}
//------------------------------------------------------------------
//
void _flxJobQueue::checkJobQueue(void)
{

    uint32_t ticks = millis();

    for (auto it = _jobQueue.begin(); it != _jobQueue.end(); /*nothing*/)
    {
        // flxLog_I("check JOBS: p1: %u, ticks: %u", it->first, ticks);
        // clear anything out within 10 ms
        if (it->first - ticks < 10)
        {
            // add job to
            if (xQueueSend(hCmdQueue, (void *)&it->second, 5 / portTICK_RATE_MS) != pdPASS)
                flxLog_W("Job Queue Overflow");

            flxJob *pJob = it->second;
            it = _jobQueue.erase(it);

            // re-queue
            _jobQueue[ticks + pJob->period()] = pJob;
        }
        else // since the map is sorted, no need to traverse the entire list..
            break;
    }
}

//------------------------------------------------------------------
void _flxJobQueue::dispatchJobs(void)
{
    flxJob *theJob;

    // Pull jobs in the queue and call the handlers
    while (xQueueReceive(hCmdQueue, &theJob, (TickType_t)10) == pdPASS)
    {
        // flxLog_I("JOBS: dispatching a Job ptr: 0x%x", (uint)theJob);
        theJob->callHandler();
        // flxLog_I("dispatched");
    }
}
//------------------------------------------------------------------
//
auto _flxJobQueue::findJob(flxJob &theJob) -> decltype(_jobQueue.end())
{
    for (auto itJob = _jobQueue.begin(); itJob != _jobQueue.end(); itJob++)
    {
        if (itJob->second == &theJob)
            return itJob;
    }
    return _jobQueue.end();
}
//------------------------------------------------------------------
//
void _flxJobQueue::addJob(flxJob &theJob)
{
    // Is this job in our queue already
    if (_jobQueue.size() > 0 && findJob(theJob) != _jobQueue.end())
        return;

    // Add this job to the job queue (map)
    _jobQueue[millis() + theJob.period()] = &theJob;

    if (_jobQueue.size() == 1)
        updateTimer();
}
//------------------------------------------------------------------
//
void _flxJobQueue::removeJob(flxJob &theJob)
{
    auto itJob = findJob(theJob);

    if (itJob == _jobQueue.end())
        return;

    _jobQueue.erase(itJob);
}
//------------------------------------------------------------------
//
bool _flxJobQueue::loop(void)
{
    dispatchJobs();

    return false;
}
