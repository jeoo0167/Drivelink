#include <Arduino.h>
#include <Hardware/IMU.h>
#include <TaskManager.h>
#include <Hardware/Sounds.h>
#include <Data_processing/AIMP.h>
#include <Services/DataManager.h>
IMU imu;
TaskManager taskManager;
FileManager f_manager;

Sounds sounds(taskManager);
PositionModel positionModel(imu);

void setup()
{
  Serial.begin(115200);
  imu.begin();
  sounds.begin();
  
  if(!f_manager.begin()) while (true);
  

  taskManager.addThreadTask("IMU Update", []()
  {
    imu.update();
  }, 100, 2048, 1);
  
  taskManager.addThreadTask("Position Prediction", []() 
  {
    positionModel.predictPosition();
    positionModel.showClases();
  }, 100, 2048, 1);

  sounds.playSound(1);


  
  taskManager.startThreadTask();
}

void loop()
{
  taskManager.updateTimerTask();
}