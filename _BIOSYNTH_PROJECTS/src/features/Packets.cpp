#include "Packets.h"

// Implementation of the checksum function
int32_t heart_packet::data::checksum() {
    uint8_t* begin = reinterpret_cast<uint8_t*>(this);
    uint8_t* end = begin + sizeof(*this);
    int32_t sum = 0;
    for(auto it = begin ; it != end; ++it)   sum += *it; 
    return sum;
}

int32_t skin_packet::data::checksum() {
    uint8_t* begin = reinterpret_cast<uint8_t*>(this);
    uint8_t* end = begin + sizeof(*this);
    int32_t sum = 0;
    for(auto it = begin ; it != end; ++it)   sum += *it; 
    return sum;
}

int32_t resp_packet::data::checksum() {
    uint8_t* begin = reinterpret_cast<uint8_t*>(this);
    uint8_t* end = begin + sizeof(*this);
    int32_t sum = 0;
    for(auto it = begin ; it != end; ++it)   sum += *it; 
    return sum;
}

heart_packet heartPacket;
skin_packet skinPacket;
resp_packet respPacket;

void checksum(PacketType packetType) {
    uint32_t checksum = 0;
    switch (packetType) {
        case PACKET_HEART: {
            checksum = heartPacket.data.checksum();
            heartPacket.checksum = checksum;
            break;
        }
        case PACKET_SKIN: {
            checksum = skinPacket.data.checksum();
            skinPacket.checksum = checksum;
            break;
        }
        case PACKET_RESPIRATION: {
            checksum = respPacket.data.checksum();
            respPacket.checksum = checksum;
            break;
        }
        default:
            // Handle unknown packet type
            break;
    }
}

#ifndef ESP32
// Implementation of the fillPacket function
void fillPacket(Heart heart) {
        heartPacket.data.heart_raw = heart.getRaw();
        heartPacket.data.heart_normalized = heart.getNormalized();
        heartPacket.data.heart_beat_detected = heart.beatDetected();
        heartPacket.data.heart_bpm = heart.getBPM();
        heartPacket.data.heart_bpm_change = heart.bpmChange();
        heartPacket.data.heart_amplitude_change = heart.amplitudeChange();
        checksum(PACKET_HEART); 
}
void fillPacket(SkinConductance skin) {
        skinPacket.data.skin_raw = skin.getRaw();
        skinPacket.data.skin_scl = skin.getSCL();
        skinPacket.data.skin_scr = skin.getSCR();
        checksum(PACKET_SKIN); 
}
void fillPacket(Respiration resp) {
        respPacket.data.resp_raw = resp.getRaw();
        respPacket.data.resp_is_exhaling = resp.isExhaling();
        respPacket.data.resp_normalized = resp.getNormalized();
        respPacket.data.resp_scaled = resp.getScaled();
        respPacket.data.resp_raw_amplitude = resp.getRawAmplitude();
        respPacket.data.resp_normalized_amplitude = resp.getNormalizedAmplitude();
        respPacket.data.resp_scaled_amplitude = resp.getScaledAmplitude();
        respPacket.data.resp_amplitude_level = resp.getAmplitudeLevel();
        respPacket.data.resp_amplitude_change = resp.getAmplitudeChange();
        respPacket.data.resp_amplitude_variability = resp.getAmplitudeVariability();
        respPacket.data.resp_interval = resp.getInterval();
        respPacket.data.resp_rpm = resp.getRpm();
        respPacket.data.resp_normalized_rpm = resp.getNormalizedRpm();
        respPacket.data.resp_scaled_rpm = resp.getScaledRpm();
        respPacket.data.resp_rpm_level = resp.getRpmLevel();
        respPacket.data.resp_rpm_change = resp.getRpmChange();
        respPacket.data.resp_rpm_variability = resp.getRpmVariability();
        checksum(PACKET_RESPIRATION);
}
// Implementation of the sendPackets function
void sendPackets() {
    Serial3.write((uint8_t*)&heartPacket, sizeof(heartPacket));
    Serial3.write((uint8_t*)&skinPacket, sizeof(skinPacket));
    Serial3.write((uint8_t*)&respPacket, sizeof(respPacket));
}   
void sendBoardID(int board_id) {
    Serial3.write("Board_ID: "); 
    Serial3.write(board_id);
    Serial3.write('\n');
}
#endif

void debugPrint(const char* format, ...) {
    #ifdef DEBUG_PRINT
        va_list args;
        va_start(args, format);
        char buffer[100]; // allocate a buffer to hold the converted string
        vsnprintf(buffer, 100, format, args); // convert the format string to a buffer
        Serial.print(buffer);
        va_end(args);
        Serial.println();
    #endif
}