#include <Arduino.h>
#include <Hardware/IMU.h>
#include <TaskManager.h>
#include <Hardware/Sounds.h>
#include <Data_processing/AIMP.h>
#include <Services/DataManager.h>
#include <Services/Logger.h>
#include <Services/Supervisor.h>

IMU imu;
TaskManager taskManager;
FileManager f_manager;
JsonManager json_manager;
Logger logger(__FILE__);
Sounds sounds(taskManager);
PositionModel positionModel(imu);
Supervisor supervisor(positionModel);
void setup()
{
  logger.init();
  logger.setOutput(OutMode::both); 
  imu.begin();
  sounds.begin();
  
  if(!f_manager.begin() || !json_manager.load(FileID::Settings))
  { 
    logger.msg(MsgType::CRITICAL,"FileManager WRONG \n ABORTING");
    while(true);
  }
  else
  {
    //Serial.println("FileManager and file load success");
    logger.msg(MsgType::INFO,"FileManager Success");
  }
  
  taskManager.addThreadTask("IMU Update", []()
  {
    imu.update();
  }, 100, 2048, 1);
  
  taskManager.addThreadTask("Position Prediction", []() 
  {
    positionModel.predictPosition();
    //positionModel.showClases();
  }, 100, 2048, 1);

  taskManager.addThreadTask("Supervisor", []()
  {
      supervisor.update();
      if(supervisor.hasChanged())
      {
        supervisor.showStatus();
        supervisor.clearChanged();
      }
  }, 20, 2048, 1);
  //sounds.playSound(1);


  taskManager.startThreadTask();
}

void loop()
{
  taskManager.updateTimerTask();
}