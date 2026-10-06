#ifndef NETWORK_MANAGER_H
#define NETWORK_MANAGER_H

#include <Arduino.h>
#include <esp_now.h>
#include <WiFi.h>
#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>
#include "Packet.h"
#include "Services/Logger.h"

#define NETWORK_MAX_QUEUE_SIZE 10

static constexpr uint32_t HEARTBEAT_INTERVAL = 500;
static constexpr uint32_t HEARTBEAT_TIMEOUT = 1500;

class NetworkManager
{
public:

    NetworkManager();

    bool begin(const uint8_t* peerAddress);

    bool send(const char* message);

    bool send(const NetworkPacket& packet);

    bool available();

    NetworkPacket receive();

    uint32_t getSequence() const;
    
    bool isConnected() const;

    void sendHeartbeat();

    void updateConnection();

    void processHeartbeatAck(const NetworkPacket& packet);
private:

    uint8_t _peerAddress[6];

    volatile bool _packetAvailable;
    NetworkPacket _receivedPacket;

    uint32_t _sequence;

    static NetworkManager* _instance;

    static void onDataReceive(
        const uint8_t* mac,
        const uint8_t* data,
        int len
    );

    static void onDataSent(
        const uint8_t* mac,
        esp_now_send_status_t status
    );

    void handleReceive(
        const uint8_t* data,
        int len
    );

    Logger logger;

    QueueHandle_t rxQueue = nullptr;
    uint32_t droppedPackets = 0;
    
    uint32_t lastHeartbeat = 0;
    uint32_t lastHeartbeatAck = 0;

    bool connected = false;
};

#endif