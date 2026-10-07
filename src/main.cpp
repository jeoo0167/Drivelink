#include <Arduino.h>

#include <Hardware/IMU.h>
#include <TaskManager.h>
#include <Hardware/Sounds.h>
#include <Data_processing/AIMP.h>
//#include <Services/DataManager.h>
#include <Services/Logger.h>
#include <Services/Supervisor.h>
#include <Networking/NetworkManager.h>


// ==================================================
// OBJETOS
// ==================================================

IMU imu;
TaskManager taskManager;

//FileManager f_manager;
//JsonManager json_manager;

NetworkManager network_manager;

Logger logger(__FILE__);

Sounds sounds(taskManager);

PositionModel positionModel(imu);

Supervisor supervisor(positionModel);


// ==================================================
// MAC DEL RECEPTOR
// ==================================================

uint8_t recvMac[] =
{
    0x34,0x98,0x7A,0xA6,0x8C,0xF4

};


// ==================================================
// SETUP
// ==================================================

void setup()
{
    // ----------------------------------------------
    // Inicialización
    // ----------------------------------------------

    logger.init();
    logger.setOutput(OutMode::both);

    imu.begin();
    sounds.begin();


    // ----------------------------------------------
    // NetworkManager
    // ----------------------------------------------

    if (!network_manager.begin(recvMac))
    {
        logger.msg(
            MsgType::CRITICAL,
            "NetworkManager failed"
        );

        while (true)
        {
            delay(1000);
        }
    }

    logger.msg(
        MsgType::INFO,
        "NetworkManager initialized"
    );


    // ----------------------------------------------
    // FileManager
    // ----------------------------------------------

    /**  if (!f_manager.begin() ||
        !json_manager.load(FileID::Settings))
    {
        logger.msg(
            MsgType::CRITICAL,
            "FileManager WRONG\nABORTING"
        );

        while (true);
    }
*/
    logger.msg(
        MsgType::INFO,
        "FileManager Success"
    );


    // ==================================================
    // TASK: IMU
    // ==================================================

    taskManager.addThreadTask(
        "IMU Update",
        []()
        {
            imu.update();
        },
        100,
        2048,
        1
    );


    // ==================================================
    // TASK: POSITION MODEL
    // ==================================================

    taskManager.addThreadTask(
        "Position Prediction",
        []()
        {
            positionModel.predictPosition();
           // positionModel.showClases();
        },
        100,
        2048,
        1
    );


    // ==================================================
    // TASK: SUPERVISOR
    // ==================================================

    taskManager.addThreadTask(
        "Supervisor",
        []()
        {
            supervisor.update();

            if (!supervisor.hasChanged())
            {
                return;
            }


            // ------------------------------------------
            // Mostrar estado
            // ------------------------------------------

            supervisor.showStatus();


            // ------------------------------------------
            // Obtener posición
            // ------------------------------------------

            Supervisor::Position position =
                supervisor.getCurrentPosition();


            // ------------------------------------------
            // Enviar comando
            // ------------------------------------------

            bool success = true;

            switch (position)
            {
                case Supervisor::Position::STOP:
                    success = network_manager.send("S");
                    Serial.println("[TX] S");
                    break;

                case Supervisor::Position::FORWARD:
                    success = network_manager.send("F");
                    Serial.println("[TX] F");
                    break;

                case Supervisor::Position::BACKWARD:
                    success = network_manager.send("B");
                    Serial.println("[TX] B");
                    break;

                case Supervisor::Position::LEFT:
                    success = network_manager.send("L");
                    Serial.println("[TX] L");
                    break;

                case Supervisor::Position::RIGHT:
                    success = network_manager.send("R");
                    Serial.println("[TX] R");
                    break;

                default:
                    break;
            }


            // ------------------------------------------
            // Error TX
            // ------------------------------------------

            if (!success)
            {
                logger.msg(
                    MsgType::WARN,
                    "Error al enviar paquete"
                );
            }


            // ------------------------------------------
            // Feedback
            // ------------------------------------------

            sounds.playSound(2);


            // ------------------------------------------
            // Consumir cambio
            // ------------------------------------------

            supervisor.clearChanged();
        },
        20,
        2048,
        1
    );


    // ----------------------------------------------
    // Sonido de inicio
    // ----------------------------------------------

    sounds.playSound(1);


    // ==================================================
    // TASK: NETWORK HEARTBEAT
    // ==================================================

    taskManager.addThreadTask(
        "Network Heartbeat",
        []()
        {
            network_manager.sendHeartbeat();
            network_manager.updateConnection();
        },
        HEARTBEAT_INTERVAL,
        2048,
        1
    );


    // ==================================================
    // INICIAR TASKS
    // ==================================================

    taskManager.startThreadTask();
}


// ==================================================
// LOOP
// ==================================================

void loop()
{
    taskManager.updateTimerTask();


    // ==================================================
    // RECIBIR PAQUETES
    // ==================================================

    NetworkPacket packet{};

    while (network_manager.receive(packet))
    {
        // ----------------------------------------------
        // HEARTBEAT ACK
        // ----------------------------------------------

        if (packet.type ==
            static_cast<uint8_t>(MessageType::HEARTBEAT_ACK))
        {
            network_manager.processHeartbeatAck(packet);

            Serial.printf(
                "[NET] HEARTBEAT ACK SEQ=%lu\n",
                (unsigned long)packet.sequence
            );
        }


        // ----------------------------------------------
        // ACK
        // ----------------------------------------------

        else if (packet.type ==
                 static_cast<uint8_t>(MessageType::ACK))
        {
            Serial.printf(
                "[RX] ACK SEQ=%lu DATA=%s\n",
                (unsigned long)packet.sequence,
                packet.data
            );
        }


        // ----------------------------------------------
        // DATA
        // ----------------------------------------------

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


    // ==================================================
    // ESTADO DE CONEXIÓN
    // ==================================================

    static bool lastConnectionState = false;

    bool currentConnectionState =
        network_manager.isConnected();


    if (currentConnectionState != lastConnectionState)
    {
        lastConnectionState = currentConnectionState;

        if (currentConnectionState)
        {
            Serial.println(
                "[NET] CONNECTION: CONNECTED"
            );
        }
        else
        {
            Serial.println(
                "[NET] CONNECTION: LOST"
            );
        }
    }
}