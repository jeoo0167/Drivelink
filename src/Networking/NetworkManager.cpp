#include "NetworkManager.h"

NetworkManager* NetworkManager::_instance = nullptr;

NetworkManager::NetworkManager() : logger(__FILE__)
{
    memset(_peerAddress, 0, sizeof(_peerAddress));

    _sequence = 0;

    _instance = this;
}


bool NetworkManager::begin(const uint8_t* peerAddress)
{
    // --------------------------------
    // WiFi en modo estación
    // --------------------------------

    WiFi.mode(WIFI_STA);

    // --------------------------------
    // Inicializar ESP-NOW
    // --------------------------------

    if (esp_now_init() != ESP_OK)
    {
        return false;
    }

    // --------------------------------
    // Guardar MAC del receptor
    // --------------------------------

    memcpy(
        _peerAddress,
        peerAddress,
        6
    );

    // --------------------------------
    // Registrar callbacks
    // --------------------------------

    rxQueue = xQueueCreate(NETWORK_MAX_QUEUE_SIZE,sizeof(NetworkPacket));


    if(rxQueue == nullptr)
    {
        logger.msg(MsgType::CRITICAL, "falied to create rx queue");
        return false;
    }

    esp_now_register_send_cb(onDataSent);

    esp_now_register_recv_cb(onDataReceive);

    // --------------------------------
    // Registrar peer
    // --------------------------------

    esp_now_peer_info_t peerInfo = {};

    memcpy(
        peerInfo.peer_addr,
        _peerAddress,
        6
    );

    peerInfo.channel = 0;
    peerInfo.encrypt = false;

    if (esp_now_add_peer(&peerInfo) != ESP_OK)
    {
        return false;
    }

    return true;
}


bool NetworkManager::send(const char* message)
{
    NetworkPacket packet = {};

    packet.type = static_cast<uint8_t>(
        MessageType::DATA
    );

    packet.sequence = _sequence++;

    strncpy(
        packet.data,
        message,
        sizeof(packet.data) - 1
    );

    return send(packet);
}


bool NetworkManager::send(
    const NetworkPacket& packet
)
{
    esp_err_t result = esp_now_send(
        _peerAddress,
        reinterpret_cast<const uint8_t*>(&packet),
        sizeof(NetworkPacket)
    );

    return result == ESP_OK;
}


bool NetworkManager::available() const
{
    return rxQueue != nullptr &&
           uxQueueMessagesWaiting(rxQueue) > 0;
}

bool NetworkManager::receive(NetworkPacket& packet)
{
    if (rxQueue == nullptr)
        return false;

    return xQueueReceive(
        rxQueue,
        &packet,
        0
    ) == pdPASS;
}


uint32_t NetworkManager::getSequence() const
{
    return _sequence;
}


void NetworkManager::onDataReceive(
    const uint8_t* mac,
    const uint8_t* data,
    int len)
{
    if (mac == nullptr || data == nullptr)
        return;

    if (len != sizeof(NetworkPacket))
        return;

    NetworkPacket packet{};
    memcpy(&packet, data, sizeof(NetworkPacket));

    // Garantizar terminación de la cadena.
    packet.data[sizeof(packet.data) - 1] = '\0';

    if (_instance->rxQueue == nullptr)
        return;

    // Operación no bloqueante: el callback no espera
    // a que la aplicación consuma el paquete.
    if (xQueueSend(_instance->rxQueue, &packet, 0) != pdPASS)
    {
        ++_instance->droppedPackets;
    }
}

void NetworkManager::onDataSent(
    const uint8_t* mac,
    esp_now_send_status_t status
)
{
    if (_instance == nullptr)
        return;

    if (status == ESP_NOW_SEND_SUCCESS)
    {
        _instance->logger.msg(
            MsgType::INFO,
            "ESP-NOW TX: SUCCESS"
        );
    }
    else
    {
        _instance->logger.msg(
            MsgType::ERROR,
            "ESP-NOW TX: FAILED"
        );
    }
}

void NetworkManager::sendHeartbeat()
{
    NetworkPacket packet = {};

    packet.type = static_cast<uint8_t>(MessageType::HEARTBEAT);
    packet.sequence = _sequence++;

    strncpy(
        packet.data,
        "HEARTBEAT",
        sizeof(packet.data) - 1
    );

    packet.data[sizeof(packet.data) - 1] = '\0';

    if (send(packet))
    {
        lastHeartbeat = millis();
    }
}

bool NetworkManager::isConnected() const
{
    return connected;
}

void NetworkManager::updateConnection()
{
    uint32_t now = millis();

    if (now - lastHeartbeatAck > HEARTBEAT_TIMEOUT)
    {
        connected = false;
    }
}

void NetworkManager::processHeartbeatAck(
    const NetworkPacket& packet)
{
    lastHeartbeatAck = millis();
    connected = true;
}