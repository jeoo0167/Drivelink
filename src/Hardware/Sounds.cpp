#include "TaskManager.h"
#include "Sounds.h"

Sounds::Sounds(TaskManager& task_manager)
    : task_manager(task_manager)
{
}
void Sounds::begin()
{
    pinMode(buzzerPin, OUTPUT);
    ledcSetup(channel, baseFrequency, resolution);
    ledcAttachPin(buzzerPin, channel);
    Serial.println("Sounds initialized");
}


void Sounds::playSound(int soundIndex)
{
    switch (soundIndex)
    {
        case 1:
            task_manager.addTimerTask("Sound1", [this]() {sound1(); }, 25);
            break;
        default:
            Serial.println("Invalid sound index");
            break;
    }
}

void Sounds::sound1()
{
    float exp_tone = 100 +pow(SoundStep,2);
    ledcWriteTone(channel, exp_tone);
    SoundStep++;
    if (SoundStep > 35)
    {
        SoundStep = 0;
        ledcWriteTone(channel, 0);
        task_manager.removeTimerTask("Sound1");
    }
}
