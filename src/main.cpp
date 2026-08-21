#include <Arduino.h>
#include <Hardware/IMU.h>
#include <TaskManager.h>
#include <Hardware/Sounds.h>
#include <Data_processing/AIMP.h>

IMU imu;
TaskManager taskManager;

Sounds sounds(taskManager);
PositionModel positionModel(imu);

void setup()
{
  Serial.begin(115200);
  imu.begin();
  
  taskManager.addThreadTask("IMU Update", []()
  {
    imu.update();
  }, 100, 2048, 1);
  
  taskManager.addThreadTask("Position Prediction", []() 
  {
    positionModel.predictPosition();
    positionModel.showClases();
  }, 100, 2048, 1);

  
  taskManager.startThreadTask();
  sounds.begin();
  sounds.playSound(1);
}

void loop()
{
  taskManager.updateTimerTask();
}