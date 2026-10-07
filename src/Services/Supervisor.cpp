#include "Supervisor.h"

Supervisor::Supervisor(PositionModel& model)
    : positionModel(model),
      currentPosition(STOP),
      candidatePosition(STOP),
      currentConfidence(0.0),
      candidateConfidence(0.0),
      changed(false),
      candidateSince(0),
      logger(__FILE__)
{
}

void Supervisor::update()
{
    int prediction = positionModel.getPredict();
    double confidence = positionModel.getConfidence();

    Position detectedPosition = static_cast<Position>(prediction);

    currentConfidence = confidence;

    /*
     * ---------------------------------------------------------
     * El estado actual es STOP
     * ---------------------------------------------------------
     */

    if (currentPosition == STOP)
    {
        if (confidence >= ENTER_THRESHOLD &&
            detectedPosition != STOP)
        {
            if (candidatePosition != detectedPosition)
            {
                candidatePosition = detectedPosition;
                candidateSince = millis();
            }

            if (millis() - candidateSince >= CONFIRM_TIME)
            {
                currentPosition = candidatePosition;
                changed = true;

                candidatePosition = STOP;
                candidateSince = 0;
            }
        }
        else
        {
            candidatePosition = STOP;
            candidateSince = 0;
        }

        return;
    }


    /*
     * ---------------------------------------------------------
     * Ya estamos ejecutando una posición
     * ---------------------------------------------------------
     */

    // Si la confianza cae demasiado, regresamos a STOP.
    if (confidence < EXIT_THRESHOLD)
    {
        currentPosition = STOP;
        changed = true;

        candidatePosition = STOP;
        candidateSince = 0;

        return;
    }


    /*
     * ---------------------------------------------------------
     * La predicción sigue siendo válida
     * ---------------------------------------------------------
     */

    if (detectedPosition == currentPosition)
    {
        // Todo estable
        candidatePosition = STOP;
        candidateSince = 0;

        return;
    }


    /*
     * ---------------------------------------------------------
     * Apareció una nueva dirección
     * ---------------------------------------------------------
     */

    if (confidence >= ENTER_THRESHOLD)
    {
        if (candidatePosition != detectedPosition)
        {
            candidatePosition = detectedPosition;
            candidateConfidence = confidence;
            candidateSince = millis();
        }

        if (millis() - candidateSince >= CONFIRM_TIME)
        {
            currentPosition = candidatePosition;
            changed = true;

            candidatePosition = STOP;
            candidateSince = 0;
        }
    }
    else
    {
        candidatePosition = STOP;
        candidateSince = 0;
    }
}


Supervisor::Position Supervisor::getCurrentPosition() const
{
    return currentPosition;
}


double Supervisor::getCurrentConfidence() const
{
    return currentConfidence;
}


bool Supervisor::hasChanged()
{
    return changed;
}


void Supervisor::clearChanged()
{
    changed = false;
}

void Supervisor::showStatus()
{
   logger.msg(MsgType::INFO,"Supervisor | Position: ");

    switch (currentPosition)
    {
        case STOP:     logger.msg(MsgType::INFO,"STOP");     break;
        case FORWARD:  logger.msg(MsgType::INFO,"FORWARD");  break;
        case BACKWARD: logger.msg(MsgType::INFO,"BACKWARD"); break;
        case LEFT:     logger.msg(MsgType::INFO,"LEFT");     break;
        case RIGHT:    logger.msg(MsgType::INFO,"RIGHT");    break;
    }

    logger.msg(MsgType::INFO,String(" | Confidence: ") + currentConfidence + 2);
    logger.msg(MsgType::INFO,String(" | Changed: ")+String(changed ? "YES" : "NO"));
}

