

#include <Arduino.h>
#ifndef ESP32
#include "Respiration.h"
#include "SkinConductance.h"
#include "Heart.h"
#endif

#ifndef PACKETS_H
#define PACKETS_H
// Packet structure definitions
enum PacketType {
    PACKET_UNKNOWN,
    PACKET_HEART,
    PACKET_SKIN,
    PACKET_RESPIRATION
};

struct heart_packet {
    int32_t magic_in;
    int32_t checksum;
    struct data {
        int16_t heart_raw;
        bool heart_beat_detected;
        char padding;
        float heart_normalized;
        float heart_bpm;
        float heart_bpm_change;
        float heart_amplitude_change;
        int32_t checksum();
    } data;
    int32_t magic_out;
};

struct skin_packet {
    int32_t magic_in;
    int32_t checksum;
    struct data {
        int32_t skin_raw;
        float skin_scr;
        float skin_scl;
        int32_t checksum();
    } data;
    int32_t magic_out;
};

struct resp_packet {
    int32_t magic_in;
    int32_t checksum;
    struct data {
        int16_t resp_raw;
        bool resp_is_exhaling;
        char padding;
        float resp_normalized;
        float resp_scaled;
        float resp_raw_amplitude;
        float resp_normalized_amplitude;
        float resp_scaled_amplitude;
        float resp_amplitude_level;
        float resp_amplitude_change;
        float resp_amplitude_variability;
        float resp_interval;
        float resp_rpm;
        float resp_normalized_rpm;
        float resp_scaled_rpm;
        float resp_rpm_level;
        float resp_rpm_change;
        float resp_rpm_variability;
        int32_t checksum();
    } data;
    int32_t magic_out;
};

extern heart_packet heartPacket;
extern skin_packet skinPacket;
extern resp_packet respPacket;

// Function declarations
void checksum(PacketType packetType);
#ifndef ESP32
void fillPacket(Heart heart);
void fillPacket(SkinConductance skin);
void fillPacket(Respiration resp);
void sendPackets();
void sendBoardID(int board_id);
#endif

void debugPrint(const char *format, ...);

#endif  // PACKETS_H