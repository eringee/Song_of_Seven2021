

#include <Arduino.h>
#include "Respiration.h"
#include "SkinConductance.h"
#include "Heart.h"

#ifndef PACKETS_H
#define PACKETS_H


// Packet structure definitions
enum PacketType {
    PACKET_UNKNOWN,
    PACKET_HEART,
    PACKET_SKIN,
    PACKET_RESPIRATION
};

// Function declarations
void checksum(PacketType packetType);
void fillPacket(Heart *heart);
void fillPacket(SkinConductance *skin);
void fillPacket(Respiration *resp);
void sendPackets();
void debugPrint(const char *format, ...);

#endif  // PACKETS_H