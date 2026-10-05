#include "NetworkManager.h"

NetworkManager* NetworkManager::_instance = nullptr;

NetworkManager::NetworkManager()
{
    memset(_peerAddress, 0, sizeof(_peerAddress));

    _packetAvailable = false;

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


bool NetworkManager::available()
{
    return _packetAvailable;
}


NetworkPacket NetworkManager::receive()
{
    NetworkPacket packet = {};

    if (!_packetAvailable)
    {
        return packet;
    }

    // Evitar leer mientras el callback modifica
    noInterrupts();

    memcpy(
        &packet,
        &_receivedPacket,
        sizeof(NetworkPacket)
    );

    _packetAvailable = false;

    interrupts();

    return packet;
}


uint32_t NetworkManager::getSequence() const
{
    return _sequence;
}


void NetworkManager::onDataReceive(
    const uint8_t* mac,
    const uint8_t* data,
    int len
)
{
    if (_instance == nullptr)
    {
        return;
    }

    _instance->handleReceive(
        data,
        len
    );
}


void NetworkManager::handleReceive(
    const uint8_t* data,
    int len
)
{
    if (len != sizeof(NetworkPacket))
    {
        return;
    }

    memcpy(
        &_receivedPacket,
        data,
        sizeof(NetworkPacket)
    );

    _packetAvailable = true;
}


void NetworkManager::onDataSent(
    const uint8_t* mac,
    esp_now_send_status_t status
)
{
    // Por ahora solamente observamos
    // el resultado mediante Serial.

    Serial.print("ESP-NOW TX: ");

    if (status == ESP_NOW_SEND_SUCCESS)
    {
        Serial.println("OK");
    }
    else
    {
        Serial.println("FAIL");
    }
}