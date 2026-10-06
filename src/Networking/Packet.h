#ifndef NETWORK_PACKET_H
#define NETWORK_PACKET_H

#include <Arduino.h>

enum class MessageType : uint8_t
{
    DATA = 0,
    ACK  = 1,
    HEARTBEAT,
    HEARTBEAT_ACK
};

struct NetworkPacket
{
    uint8_t type;
    uint32_t sequence;
    char data[200];
};

#endif