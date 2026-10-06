#ifndef Task_Manager_H
#define Task_Manager_H

#include <cstdint>
#include <functional>
#include <Arduino.h>
#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>

#define MAX_TASKS 10
#define MAX_TIMER_TASKS 10
#define NETWORK_RX_QUEUE_SIZE 10

struct ThreadTaskinfo
{
    const char* name;
    std::function<void()> callback;
    uint32_t period;
    uint32_t stacksize;
    UBaseType_t priority;
    TaskHandle_t TaskHandle;
};

struct TimerTaskinfo
{
    const char* name;
    std::function<void()> callback;
    uint32_t period;
    uint32_t lastExecution;
    bool active;
};

class TaskManager
{
    public:
        void begin();
        void addThreadTask(const char* name, std::function<void()> callback, uint32_t period, uint32_t stacksize, UBaseType_t priority);
        void addTimerTask(const char* name, std::function<void()> callback, uint32_t period);
        void startThreadTask();
        void updateTimerTask();
        void removeTimerTask(const char* name);
        
    private:
        ThreadTaskinfo tasks[MAX_TASKS];
        TimerTaskinfo timerTasks[MAX_TIMER_TASKS];
        int ThreadTaskCount=0;
        int TimerTaskCount=0;
        static void taskWrapper(void* pvParameters);
};
#endif