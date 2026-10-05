#ifndef SUPERVISOR_H
#define SUPERVISOR_H

#include <Arduino.h>
#include "Data_processing/AIMP.h"
#include "Services/Logger.h"
class Supervisor
{
public:

    enum Position
    {
        STOP = 0,
        FORWARD,
        BACKWARD,
        LEFT,
        RIGHT
    };

    Supervisor(PositionModel& model);

    void update();

    Position getCurrentPosition() const;
    double getCurrentConfidence() const;

    bool hasChanged();
    void clearChanged();

    void showStatus();
private:
    Logger logger;
    PositionModel& positionModel;

    Position currentPosition;
    Position candidatePosition;

    double currentConfidence;
    double candidateConfidence;

    bool changed;

    unsigned long candidateSince;

    // Parámetros del filtro
    static constexpr double ENTER_THRESHOLD = 0.75;
    static constexpr double EXIT_THRESHOLD  = 0.55;

    static constexpr unsigned long CONFIRM_TIME = 150;
};

#endif