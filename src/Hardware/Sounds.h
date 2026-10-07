#ifndef Sounds_H
#define Sounds_H

#include <Arduino.h>
#include <Services/Logger.h>

class TaskManager;

class Sounds
{
    public:
        Sounds(TaskManager& taskManager);
        void begin();
        void playSound(int soundIndex);
        void stopSound();
        void changeParameters(int newBaseFrequency, int newResolution, int newChannel);
        void sound1();
        void beep(uint16_t duration = 500);

    private:
        TaskManager& task_manager;
        Logger logger;
        int buzzerPin = 14;
        int channel = 1;
        int resolution = 8;
        int baseFrequency = 2000;
        int SoundStep = 0;
};

#endif