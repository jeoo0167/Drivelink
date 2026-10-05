#ifndef NETWORK_MANAGER_H
#define NETWORK_MANAGER_H

#include <Arduino.h>
#include <esp_now.h>
#include <WiFi.h>

#include "Packet.h"

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
};

#endif