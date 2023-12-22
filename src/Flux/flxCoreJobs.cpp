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

//////////////////////////////////////////
// Our FreeRTOS entities

// "Command Queue"
static QueueHandle_t hWorkQueue = NULL;
const uint16_t kWorkQueueSize = 20;

// task handle
static TaskHandle_t hJobQTask = NULL;

#define kStackSize 1400
const uint32_t kDefaultWaitDelayMS = 100;

// Use a semaphore to manage access to the shared jobQueue and _skip flag
// These are shared between the main task and our timer task
SemaphoreHandle_t xSemaphore = NULL;
StaticSemaphore_t xSemaphoreBuffer;

//------------------------------------------------------------------
// Global object - we only have one queue

_flxJobQueue &flxJobQueue = _flxJobQueue::get();

//------------------------------------------------------------------
// Callback for the FreeRTOS task
//

static void _fluxJobQ_TaskProcessing(void *parameter)
{
    // no queue, no dice
    if (hWorkQueue == NULL)
        return;

    // start up delay
    vTaskDelay(50);

    uint32_t theDelay = 0;

    // just spin - waiting for delays to expire;
    while (true)
    {
        // Get the next delay
        theDelay = flxJobQueue._timerCB();
        if (theDelay == 0)
            theDelay = kDefaultWaitDelayMS;

        vTaskDelay(theDelay / portTICK_RATE_MS);
    }
}
//------------------------------------------------------------------
// overall job queue object
//
_flxJobQueue::_flxJobQueue() : _running{false}, _skip{false}
{
    xSemaphore = xSemaphoreCreateBinaryStatic(&xSemaphoreBuffer);
    if (!xSemaphore)
    {
        flxLog_E("Job Queue - Initialization failed");
        return;
    }
    // Init the semaphore - give it
    xSemaphoreGive(xSemaphore);
}

//------------------------------------------------------------------
// start
//
bool _flxJobQueue::start(void)
{
    if (_running)
        return true;

    // if we don't have a semaphore, we can't continue
    if (xSemaphore == NULL)
        return false;

    // okay, we need to transition from an idle state to a running state

    // order the jobs based on delta time needs.
    std ::vector<flxJob *> theJobs;

    for (auto aJob : _jobQueue)
        theJobs.push_back(aJob.second);

    _jobQueue.clear();
    // now clear out the map, and add everything back with proper timing

    for (auto aJob : theJobs)
        addJob(*aJob);

    flxLog_I("STarting the job queue: %d", _jobQueue.size());

    // Create our task used to dispatch jobs to work based on time interval needs
    hWorkQueue = xQueueCreate(kWorkQueueSize, sizeof(flxJob *));

    if (hWorkQueue == NULL)
    {
        flxLog_E("Job Queue - Failed to create queue");
        vSemaphoreDelete(xSemaphore);
        xSemaphore = NULL;
        return false;
    }

    // Event processing task
    BaseType_t xReturnValue = xTaskCreate(_fluxJobQ_TaskProcessing, // Event processing task function
                                          "job_q_proc",             // String with name of task.
                                          kStackSize,               // Stack size in 32 bit words.
                                          NULL,                     // Parameter passed as input of the task
                                          1,                        // Priority of the task.
                                          &hJobQTask);              // Task handle.

    if (xReturnValue != pdPASS)
    {
        hJobQTask = NULL;
        flxLog_E("Job Queue - Failed to start task");
        vQueueDelete(hWorkQueue);
        hWorkQueue = NULL;
        vSemaphoreDelete(xSemaphore);
        xSemaphore = NULL;
        return false;
    }

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

    if (hJobQTask)
    {
        vTaskDelete(hJobQTask);
        hJobQTask = NULL;
    }

    if (hWorkQueue)
    {
        vQueueDelete(hWorkQueue);
        hWorkQueue = NULL;
    }

    if (xSemaphore)
    {
        vSemaphoreDelete(xSemaphore);
        xSemaphore = NULL;
    }
}

//------------------------------------------------------------------
// Callback for the FreeRTOS timer
//
uint32_t _flxJobQueue::_timerCB(void)
{
    checkJobQueue();
    return updateTimer();
}

//------------------------------------------------------------------
// update the timer
uint32_t _flxJobQueue::updateTimer(void)
{

    uint32_t retval = 0;

    if (xSemaphoreTake(xSemaphore, (TickType_t)0))
    {
        // valid state?
        if (_running && _jobQueue.size() > 0)
        {

            // Return the delta from the head of the job queue to our current time - millis().
            // Note the period is the key for  the job map, which are sorted in ascending order.

            uint32_t deltaT = (_jobQueue.begin()->first - millis()) / portTICK_RATE_MS;

            // make sure the deltaT isn't near 0 - which causes an assert error - during high speed ops, it can hit 0;

            retval = deltaT < 10 ? 10 : deltaT;
        }
        xSemaphoreGive(xSemaphore);
    }

    return retval;
}
//------------------------------------------------------------------
//
void _flxJobQueue::checkJobQueue(void)
{

    // not running, no dice
    if (!_running)
        return;

    if (xSemaphoreTake(xSemaphore, (TickType_t)20))
    {
        // if skip is set, we just need to exit
        if (_skip)
        {
            _skip = false;
            xSemaphoreGive(xSemaphore);
            return;
        }

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
        xSemaphoreGive(xSemaphore);
    }
}

//------------------------------------------------------------------
void _flxJobQueue::dispatchJobs(void)
{

    if (!_running)
        return;

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
    // if (nItems > 0 && hJobQTask != NULL)
    //     flxLog_I("HIGH WATER MARK HEAP %u words", uxTaskGetStackHighWaterMark(hJobQTask));
}
//------------------------------------------------------------------
//
auto _flxJobQueue::findJob(flxJob &theJob) -> decltype(_jobQueue.end())
{

    // assuming the caller has grabbed the semaphore

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
    if (xSemaphoreTake(xSemaphore, (TickType_t)0))
    {
        // Is this job in our queue already
        if (_jobQueue.size() == 0 || findJob(theJob) == _jobQueue.end())
        {
            // Add this job to the job queue (map)
            _jobQueue[millis() + theJob.period()] = &theJob;
        }
        xSemaphoreGive(xSemaphore);
    }
}
//------------------------------------------------------------------
// remove a job
//
void _flxJobQueue::removeJob(flxJob &theJob)
{
    if (xSemaphoreTake(xSemaphore, (TickType_t)0))
    {
        auto itJob = findJob(theJob);

        // do we know of this job?
        if (itJob != _jobQueue.end())
        {
            // If the job is at the head of the queue, and a timer is set,
            // the job needs to be skipped

            _skip = itJob == _jobQueue.begin();

            _jobQueue.erase(itJob);

            // if we need to skip, but the task is blocked, wake it up
            if (_skip && hJobQTask != NULL && eTaskGetState(hJobQTask) == eBlocked)
                xTaskAbortDelay(hJobQTask);
        }
        xSemaphoreGive(xSemaphore);
    }
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
    // not setup
    if (!_running)
        return false;

    // any jobs in the work queue to execute? Check
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