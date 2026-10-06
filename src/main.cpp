#include <Arduino.h>
#include <Hardware/IMU.h>
#include <TaskManager.h>
#include <Hardware/Sounds.h>
#include <Data_processing/AIMP.h>
#include <Services/DataManager.h>
#include <Services/Logger.h>
#include <Services/Supervisor.h>
#include <Networking/NetworkManager.h>

IMU imu;
TaskManager taskManager;
FileManager f_manager;
JsonManager json_manager;
NetworkManager network_manager;
Logger logger(__FILE__);
Sounds sounds(taskManager);
PositionModel positionModel(imu);
Supervisor supervisor(positionModel);

uint8_t recvMac[] = 
{
  0xE4,
  0x65,
  0xB8,
  0x76,
  0xF8,
  0xD8
};
void setup()
{
  logger.init();
  logger.setOutput(OutMode::both); 
  imu.begin();
  sounds.begin();
  
  if (!network_manager.begin(recvMac)) 
  {
    logger.msg( MsgType::CRITICAL, "NetworkManager failed" ); 
    while (true) { delay(1000); } 
  } 
  logger.msg( MsgType::INFO, "NetworkManager initialized" );


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
        sounds.playSound(2);
      }
  }, 20, 2048, 1);
  //sounds.playSound(1);


  taskManager.startThreadTask();

taskManager.addTimerTask("Networking", []() 
{
   if (!network_manager.send("Hello from DriveLink :)")) 
   { 
    logger.msg( MsgType::WARN, "Network TX request failed" ); 
  } }, 1000);
}


void loop()
{
  taskManager.updateTimerTask();

  static uint32_t lastHeartbeat = 0;

  if (millis() - lastHeartbeat >= 500)
  {
      lastHeartbeat = millis();

      network_manager.sendHeartbeat();

      Serial.println("[NET] HEARTBEAT TX");
  }

  // --------------------------------
  // Procesar paquetes recibidos
  // --------------------------------

  while (network_manager.available())
  {
      NetworkPacket packet = network_manager.receive();

      // HEARTBEAT ACK
      if (packet.type ==
          static_cast<uint8_t>(MessageType::HEARTBEAT_ACK))
      {
          network_manager.processHeartbeatAck(packet);

          Serial.printf(
              "[NET] HEARTBEAT ACK SEQ=%lu\n",
              (unsigned long)packet.sequence
          );
      }

      // ACK normal
      else if (packet.type ==
                static_cast<uint8_t>(MessageType::ACK))
      {
          Serial.printf(
              "[RX] ACK SEQ=%lu DATA=%s\n",
              (unsigned long)packet.sequence,
              packet.data
          );
      }

      // DATA
      else if (packet.type ==
                static_cast<uint8_t>(MessageType::DATA))
      {
          Serial.printf(
              "[RX] DATA SEQ=%lu DATA=%s\n",
              (unsigned long)packet.sequence,
              packet.data
          );
      }
  }

  // --------------------------------
  // Actualizar estado de conexión
  // --------------------------------

  network_manager.updateConnection();

  // --------------------------------
  // Mostrar estado
  // --------------------------------

  static bool lastConnectionState = false;

  bool currentConnectionState =
      network_manager.isConnected();

  if (currentConnectionState != lastConnectionState)
  {
      lastConnectionState = currentConnectionState;

      if (currentConnectionState)
      {
          Serial.println("[NET] CONNECTION: CONNECTED");
      }
      else
      {
          Serial.println("[NET] CONNECTION: LOST");
      }
  }
}

