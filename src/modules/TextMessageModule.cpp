#include "TextMessageModule.h"
#include "MeshService.h"
#include "NodeDB.h"
#include "PowerFSM.h"
#include "buzz.h"
#include "configuration.h"
#include "mqtt/MQTT.h"
TextMessageModule *textMessageModule;

ProcessMessage TextMessageModule::handleReceived(const meshtastic_MeshPacket &mp)
{
#ifdef DEBUG_PORT
    auto &p = mp.decoded;
    LOG_INFO("Received text msg from=0x%0x, id=0x%x, msg=%.*s", mp.from, mp.id, p.payload.size, p.payload.bytes);
    const char* topic = "sensor/data";
    // Verifica se mqtt está inicializado
    if (mqtt) {
        // Cria um buffer para garantir que a string termine em '\0'
        char payload_buffer[p.payload.size + 1];
        memcpy(payload_buffer, p.payload.bytes, p.payload.size);
        payload_buffer[p.payload.size] = '\0'; // Garante terminação de string

        mqtt->publish(topic, payload_buffer, false);
    } else {
        LOG_ERROR("MQTT not initialized!");
    }
    
#endif
    // We only store/display messages destined for us.
    // Keep a copy of the most recent text message.
    devicestate.rx_text_message = mp;
    devicestate.has_rx_text_message = true;

    powerFSM.trigger(EVENT_RECEIVED_MSG);
    notifyObservers(&mp);

    return ProcessMessage::CONTINUE; // Let others look at this message also if they want
}

bool TextMessageModule::wantPacket(const meshtastic_MeshPacket *p)
{
    return MeshService::isTextPayload(p);
}