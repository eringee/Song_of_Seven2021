#include "Arduino.h"
#include "puara.h"
#include <WiFiUdp.h>
#include <OSCMessage.h>
#include <OSCBundle.h>
#include "Packets.h"

#define RXD2 4
#define TXD2 5
#define PRINTSERIAL Serial
#define TEENSYSERIAL Serial2
#define DEBUG_PRINT

Puara puara;
WiFiUDP Udp;
OSCBundle bundle;

#define BUFFER_SIZE 1024
uint8_t packetBuffer[BUFFER_SIZE];  // maximum packet size
uint8_t bytesRead = 0;

struct BioData {
    struct Heart {
        float Raw = 0;
        float Normalized = 0;
        float BeatDetected = 0;
        float BPM = 0;
        float BPMChange = 0;
        float AmplitudeChange = 0;
    } heart;

    struct Skin {
        float Raw = 0;
        float SCR = 0;
        float SCL = 0;
    } skin;

    struct Respiration {
        float Raw = 0;
        float Normalized = 0;
        float Scaled = 0;
        float IsExhaling = 0;
        float RawAmplitude = 0;
        float NormalizedAmplitude = 0;
        float ScaledAmplitude = 0;
        float AmplitudeLevel = 0;
        float AmplitudeChange = 0;
        float AmplitudeVariability = 0;
        float Interval = 0;
        float RPM = 0;
        float NormalizedRPM = 0;
        float ScaledRPM = 0;
        float RPMLevel = 0;
        float RPMChange = 0;
        float RPMVariability = 0;
    } respiration;
} bioData;

// Variables for packet verification
bool confirmationSent = false;

// Functions for packet sending, parsing, and verification
void prepPuaraOSCMessage(float message, const char* oscNamespace) {
    if (puara.IP1_ready()) {
        std::string msg1_path = "/biosynth.";
        msg1_path.append(std::to_string(puara.get_dmi_id()));
        msg1_path.append(oscNamespace);
        OSCMessage msg1(msg1_path.c_str());
        msg1.add(message);
        bundle.add(msg1);  
        msg1.empty();
    }
}

bool dataReceived() {
    bytesRead = 0;
    while (!TEENSYSERIAL.available()) {
        vTaskDelay(1 / portTICK_PERIOD_MS);
    }
    
    while (TEENSYSERIAL.available()) {
        uint8_t byte = TEENSYSERIAL.read();
        packetBuffer[bytesRead++] = byte;
    }
    debugPrint("Data received");
    return (bytesRead > 0);
}

PacketType findMagicIn(uint32_t value) {
    // Check the magic bytes at a given index
    if (value == heartPacket.magic_in) {
        debugPrint("Heart packet detected");
        return PACKET_HEART;
    } else if (value == skinPacket.magic_in) {
        debugPrint("Skin packet detected");
        return PACKET_SKIN;
    } else if (value == respPacket.magic_in) {
        debugPrint("Respiration packet detected");
        return PACKET_RESPIRATION;
    } else 
    return PACKET_UNKNOWN;
}

bool verifyMagicOut(int index, PacketType packetType) {
    int32_t magicOut;
    switch (packetType) {
        case PACKET_HEART:
            magicOut = heartPacket.magic_out;
            break;
        case PACKET_SKIN:
            magicOut = skinPacket.magic_out;
            break;
        case PACKET_RESPIRATION:
            magicOut = respPacket.magic_out;
            break;
        default:
            return false;
    }

    uint8_t* magicOutPos = packetBuffer + index + 4;
    for (; magicOutPos < packetBuffer + bytesRead - 3; magicOutPos += 4) {
        if (*(uint32_t*)magicOutPos == magicOut) {
            debugPrint("Magic out found");
            return true;
        }
    }
    debugPrint("Magic out not found");
    return false;
}

bool verifyChecksum(int index, PacketType packetType) {
uint32_t expectedChecksum = 0;
uint32_t calculatedChecksum = 0;

switch (packetType) {
    case PACKET_HEART:
        expectedChecksum = ((heart_packet*)(packetBuffer + index))->checksum;
        calculatedChecksum =((heart_packet*)(packetBuffer + index))->data.checksum();
        break;
    case PACKET_SKIN:
        expectedChecksum = ((skin_packet*)(packetBuffer + index))->checksum;
        calculatedChecksum =((skin_packet*)(packetBuffer + index))->data.checksum();
        break;
    case PACKET_RESPIRATION:
        expectedChecksum = ((resp_packet*)(packetBuffer + index))->checksum;
        calculatedChecksum =((resp_packet*)(packetBuffer + index))->data.checksum();
        break;
    default:
        return false;
    }

    if (calculatedChecksum == expectedChecksum) {
        debugPrint("Checksum ok");
        return true;
    } else {
        debugPrint("Checksum mismatch");
        return false;
    }
}

bool verifyPacket(int index, PacketType packetType) {
    return verifyMagicOut(index, packetType) && verifyChecksum(index, packetType);
}

void processPacket(int index, PacketType packetType) {
    heart_packet* heartPacket;
    skin_packet* skinPacket;
    resp_packet* respPacket;

    switch (packetType) {
        case PACKET_HEART:
            heartPacket = (heart_packet*)(packetBuffer + index);
            bioData.heart.Raw = heartPacket->data.heart_raw;
            bioData.heart.Normalized = heartPacket->data.heart_normalized;
            bioData.heart.BeatDetected = heartPacket->data.heart_beat_detected;
            bioData.heart.BPM = heartPacket->data.heart_bpm;
            bioData.heart.BPMChange = heartPacket->data.heart_bpm_change;
            bioData.heart.AmplitudeChange = heartPacket->data.heart_amplitude_change;
            
            // Sending the Heart data
            prepPuaraOSCMessage(bioData.heart.Raw, "/heart/raw");
            prepPuaraOSCMessage(bioData.heart.Normalized, "/heart/normalized");
            prepPuaraOSCMessage(bioData.heart.BeatDetected, "/heart/beat_detected");
            prepPuaraOSCMessage(bioData.heart.BPM, "/heart/bpm");
            prepPuaraOSCMessage(bioData.heart.BPMChange, "/heart/bpm_change");
            prepPuaraOSCMessage(bioData.heart.AmplitudeChange, "/heart/amplitude_change");

            debugPrint("bioData.heart.Raw: %.0f", bioData.heart.Raw);

            break;
        case PACKET_SKIN:
            skinPacket = (skin_packet*)(packetBuffer + index);
            bioData.skin.Raw = skinPacket->data.skin_raw;
            bioData.skin.SCR = skinPacket->data.skin_scr;
            bioData.skin.SCL = skinPacket->data.skin_scl;
            
            // Sending the Skin data
            prepPuaraOSCMessage(bioData.skin.Raw, "/skin/raw");
            prepPuaraOSCMessage(bioData.skin.SCR, "/skin/scr");
            prepPuaraOSCMessage(bioData.skin.SCL, "/skin/scl");

            debugPrint("bioData.skin.Raw: %.0f", bioData.skin.Raw);
            break;
        case PACKET_RESPIRATION:
            respPacket = (resp_packet*)(packetBuffer + index);
            bioData.respiration.Raw = respPacket->data.resp_raw;
            bioData.respiration.Normalized = respPacket->data.resp_normalized;
            bioData.respiration.Scaled = respPacket->data.resp_scaled;
            bioData.respiration.IsExhaling = respPacket->data.resp_is_exhaling;
            bioData.respiration.RawAmplitude = respPacket->data.resp_raw_amplitude;
            bioData.respiration.NormalizedAmplitude = respPacket->data.resp_normalized_amplitude;
            bioData.respiration.ScaledAmplitude = respPacket->data.resp_scaled_amplitude;
            bioData.respiration.AmplitudeLevel = respPacket->data.resp_amplitude_level;
            bioData.respiration.AmplitudeChange = respPacket->data.resp_amplitude_change;
            bioData.respiration.AmplitudeVariability = respPacket->data.resp_amplitude_variability;
            bioData.respiration.Interval = respPacket->data.resp_interval;
            bioData.respiration.RPM = respPacket->data.resp_rpm;
            bioData.respiration.NormalizedRPM = respPacket->data.resp_normalized_rpm;
            bioData.respiration.ScaledRPM = respPacket->data.resp_scaled_rpm;
            bioData.respiration.RPMLevel = respPacket->data.resp_rpm_level;
            bioData.respiration.RPMChange = respPacket->data.resp_rpm_change;
            bioData.respiration.RPMVariability = respPacket->data.resp_rpm_variability;
            
            // Sending the Respiration data
            prepPuaraOSCMessage(bioData.respiration.Raw, "/respiration/raw");
            prepPuaraOSCMessage(bioData.respiration.Normalized, "/respiration/normalized");
            prepPuaraOSCMessage(bioData.respiration.Scaled, "/respiration/scaled");
            prepPuaraOSCMessage(bioData.respiration.IsExhaling, "/respiration/is_exhaling");
            prepPuaraOSCMessage(bioData.respiration.RawAmplitude, "/respiration/raw_amplitude");
            prepPuaraOSCMessage(bioData.respiration.NormalizedAmplitude, "/respiration/normalized_amplitude");
            prepPuaraOSCMessage(bioData.respiration.ScaledAmplitude, "/respiration/scaled_amplitude");
            prepPuaraOSCMessage(bioData.respiration.AmplitudeLevel, "/respiration/amplitude_level");
            prepPuaraOSCMessage(bioData.respiration.AmplitudeChange, "/respiration/amplitude_change");
            prepPuaraOSCMessage(bioData.respiration.AmplitudeVariability, "/respiration/amplitude_variability");
            prepPuaraOSCMessage(bioData.respiration.Interval, "/respiration/interval");
            prepPuaraOSCMessage(bioData.respiration.RPM, "/respiration/rpm");
            prepPuaraOSCMessage(bioData.respiration.NormalizedRPM, "/respiration/normalized_rpm");
            prepPuaraOSCMessage(bioData.respiration.ScaledRPM, "/respiration/scaled_rpm");
            prepPuaraOSCMessage(bioData.respiration.RPMLevel, "/respiration/rpm_level");
            prepPuaraOSCMessage(bioData.respiration.RPMChange, "/respiration/rpm_change");
            prepPuaraOSCMessage(bioData.respiration.RPMVariability, "/respiration/rpm_variability");

            debugPrint("bioData.respiration.Raw: %.0f", bioData.respiration.Raw);
            break;
        default:
            debugPrint("Unknown packet type");
            break;
    }
    if (puara.IP1_ready()) {
    Udp.beginPacket(puara.getIP1().c_str(), puara.getPORT1());
    bundle.send(Udp);
    Udp.endPacket();
    bundle.empty();
    }
}

void setup() {
    // Initialize serial for debugging
    PRINTSERIAL.begin(115200);
    TEENSYSERIAL.begin(115200, SERIAL_8N1, RXD2, TXD2);
    
    //Initialize Puara
    puara.start();
    
    // Initialize UDP
    Udp.begin(puara.getLocalPORT());
}

void loop() {
    // prepPuaraOSCMessage(10, "/puara/skin/Raw");

if (dataReceived()) {
    for (int i = 0; i < bytesRead - 3; i++) {
            uint32_t value = *(uint32_t*)(packetBuffer + i);
            PacketType packetType = findMagicIn(value);
            if (packetType != PACKET_UNKNOWN){
                if(verifyPacket(i, packetType)){
                    processPacket(i, packetType);
                }
            } 
    }
}
}

#ifndef Arduino_h
    extern "C" {
        void app_main(void);
    }

    void app_main() {
        setup();
        while(1) {
            loop();
        }
    }
#endif

