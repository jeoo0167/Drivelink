#include "TaskManager.h"

void TaskManager::addThreadTask(const char* name, std::function<void()> callback, uint32_t period, uint32_t stacksize, UBaseType_t priority)
{
    if (ThreadTaskCount >= MAX_TASKS)
    {
        Serial.println("Max tasks reached");
        return;
    }
    tasks[ThreadTaskCount].name = name;
    tasks[ThreadTaskCount].callback = callback;
    tasks[ThreadTaskCount].period = period;
    tasks[ThreadTaskCount].stacksize = stacksize;
    tasks[ThreadTaskCount].priority = priority;
    tasks[ThreadTaskCount].TaskHandle = nullptr;
    ThreadTaskCount++;
}

void TaskManager::taskWrapper(void* pvParameters)
{
    ThreadTaskinfo* task = static_cast<ThreadTaskinfo*>(pvParameters);

    while (true)
    {
        task->callback();

        vTaskDelay(pdMS_TO_TICKS(task->period));
    }
}

void TaskManager::startThreadTask()
{
    for(uint8_t i=0;i<ThreadTaskCount;i++)
    {
        xTaskCreate(
            taskWrapper,
            tasks[i].name,
            tasks[i].stacksize,
            &tasks[i],
            tasks[i].priority,
            &tasks[i].TaskHandle
        );
    }
}

void TaskManager::addTimerTask(const char* name, std::function<void()> callback, uint32_t period)
{
    if (TimerTaskCount >= MAX_TIMER_TASKS)
    {
        Serial.println("Max timer tasks reached");
        return;
    }
    timerTasks[TimerTaskCount].name = name;
    timerTasks[TimerTaskCount].callback = callback;
    timerTasks[TimerTaskCount].period = period;
    timerTasks[TimerTaskCount].lastExecution = millis();
    timerTasks[TimerTaskCount].active = true;
    TimerTaskCount++;
}

void TaskManager::updateTimerTask()
{
    uint32_t currentTime = millis();

    for (uint8_t i = 0; i < TimerTaskCount; i++)
    {
        if (!timerTasks[i].active)
            continue;

        if (currentTime - timerTasks[i].lastExecution >=
            timerTasks[i].period)
        {
            timerTasks[i].lastExecution = currentTime;

            timerTasks[i].callback();
        }
    }

    // Limpieza de tareas inactivas
    for (uint8_t i = 0; i < TimerTaskCount;)
    {
        if (!timerTasks[i].active)
        {
            for (uint8_t j = i; j < TimerTaskCount - 1; j++)
            {
                timerTasks[j] = timerTasks[j + 1];
            }

            TimerTaskCount--;
        }
        else
        {
            i++;
        }
    }
}

void TaskManager::removeTimerTask(const char* name)
{
    for (uint8_t i = 0; i < TimerTaskCount; i++)
    {
        if (strcmp(timerTasks[i].name, name) == 0)
        {
            timerTasks[i].active = false;
            return;
        }
    }
}