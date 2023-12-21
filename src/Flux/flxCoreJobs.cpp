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
static QueueHandle_t hWorkQueue = NULL;
const uint16_t kWorkQueueSize = 20;
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

    hWorkQueue = xQueueCreate(kWorkQueueSize, sizeof(flxJob *));

    if (hWorkQueue == NULL)
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
    {
        return;
    }

    // flxLog_I("JOBS: Updating Timer: %u", _jobQueue.begin()->second->period());
    // Set the timer to the period in the queue - millis(). Note the period is the key, which
    // are sorted in ascending order.

    uint32_t deltaT = (_jobQueue.begin()->first - millis()) / portTICK_RATE_MS;

    // make sure the deltaT isn't near 0 - which causes an assert error - during high speed ops, it can hit 0;

    xTimerChangePeriod(hTimer, deltaT < 10 ? 10 : deltaT, 10);
    xTimerReset(hTimer, 10);
}
//------------------------------------------------------------------
//
void _flxJobQueue::checkJobQueue(void)
{

    uint32_t ticks = millis();

    for (auto it = _jobQueue.begin(); it != _jobQueue.end(); /*nothing*/)
    {
        // clear anything out within 10 ms
        if (it->first - ticks < 10)
        {
            flxJob *theJob = it->second;

            // remove this item from the job queue head, and since the time
            // is recurring, put the job at the end of the queue
            it = _jobQueue.erase(it);

            // re-queue our job in the job task timer list
            _jobQueue[ticks + theJob->period()] = theJob;

            bool bAddToQ = true;

            // Can we compress this job -- skip adding to work queue if already there
            if (theJob->compress())
            {
                // To see if an item is in the do work queue, we need to walk the queue
                // and check if it exists. The only way to do this is to dequeue an
                // item, the put it back on the queue.
                flxJob *qJob;
                int nItems = uxQueueMessagesWaiting(hWorkQueue);
                for (int i = 0; i < nItems; i++)
                {
                    if (xQueueReceive(hWorkQueue, &qJob, (TickType_t)10) != pdPASS)
                        break; // something else is wrong

                    // First, put this item back at the end of the queue
                    if (xQueueSend(hWorkQueue, (void *)&qJob, 10 / portTICK_RATE_MS) != pdPASS)
                        flxLog_W("Queue compress error");

                    // is the job equal to the job we want to add? If so, don't add it
                    if (qJob == theJob)
                        bAddToQ = false;
                }
            }
            // add job to the do work queue?
            if (bAddToQ)
            {
                if (xQueueSend(hWorkQueue, (void *)&theJob, 10 / portTICK_RATE_MS) != pdPASS)
                    flxLog_W("Job Queue - work overflow");
            }
        }
        else // since the map is sorted, no need to traverse the entire list..
            break;
    }
}

//------------------------------------------------------------------
void _flxJobQueue::dispatchJobs(void)
{
    flxJob *theJob;

    // loop over the current items in the work queue. If the time interval is fast,
    // AND the work called is slow, the queue could continue to be filled and never
    // be emptied unless we restrict to items at hand
    int nItems = uxQueueMessagesWaiting(hWorkQueue);
    for (int i = 0; i < nItems; i++)
    {
        // Pull jobs in the queue and call the handlers
        if (xQueueReceive(hWorkQueue, &theJob, (TickType_t)50) == pdPASS)
            theJob->callHandler();
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

    // If the job is at the head of the queue, and a timer is set,
    // we need to stop that timer.
    //
    if (itJob == _jobQueue.begin() && xTimerIsTimerActive(hTimer) != pdFALSE)
        xTimerStop(hTimer, 0);

    _jobQueue.erase(itJob);
}

//------------------------------------------------------------------
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
//
bool _flxJobQueue::loop(void)
{
    // not setup
    if (hTimer == NULL)
        return false;

    dispatchJobs();

    return false;
}

// Easy to use functions
void flxAddJobToQueue(flxJob &theJob)
{
    flxJobQueue.addJob(theJob);
}

void flxUpdateJobInQueue(flxJob &theJob)
{
    flxJobQueue.updateJob(theJob);
}