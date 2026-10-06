#include "TaskManager.h"
#include "Sounds.h"

Sounds::Sounds(TaskManager& task_manager)
    : task_manager(task_manager), logger(__FILE__)
{
}
void Sounds::begin()
{
    pinMode(buzzerPin, OUTPUT);
    ledcSetup(channel, baseFrequency, resolution);
    ledcAttachPin(buzzerPin, channel);
    logger.msg(MsgType::INFO,"SOUNDS SUCCESS INITED");
}


void Sounds::playSound(int soundIndex)
{
    switch (soundIndex)
    {
        case 1:
            task_manager.addTimerTask("Sound1", [this]() {sound1(); }, 25);
            break;
        case 2:
            beep();
            break;
        default:
            logger.msg(MsgType::WARN,"Invalid Sound index");
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

void Sounds::beep(uint16_t duration)
{
    ledcWriteTone(channel, 2000);

    task_manager.addTimerTask("Beep", [this]() {
        ledcWriteTone(channel, 0);
        task_manager.removeTimerTask("Beep");
    }, duration);
}