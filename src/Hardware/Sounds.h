#ifndef Sounds_H
#define Sounds_H


#include <Arduino.h>

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

    private:
        TaskManager& task_manager;
        int buzzerPin = 17;
        int channel = 1;
        int resolution = 8;
        int baseFrequency = 2000;
        int SoundStep = 0;

};

#endif