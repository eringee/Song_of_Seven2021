/**
 * @file OSC.h
 * @author Erin Gee & Etienne Montenegro & Luana Belinsky
 * @brief WaW project class. USE THIS FILE AS TEMPLATE FOR OTHER PROJECTS. DO NOT FORGET TO OVERRIDE ALL THE Projects.h methods
 * @version 1.0
 * @date 2022-11-02
 * @copyright Copyright (c) 2022
 * 
 */
#pragma once
#include "configuration.h"
#include "Projects.h"
#include <Arduino.h>
#include <Audio.h>
#include <Chrono.h>
#include "PlaquetteLib.h"
//include library needed for the project here

#define PRINTSERIAL Serial
#define SENDSERIAL Serial2
#define RXD2 9
#define TXD2 10
#define DEBUG_PRINT

using namespace pq;

static Metronome sendMetro(0.02);

//create a class that inherit Projet class and modify ist member for the project
class Osc :public Project{
    
    private:
    const char* name{"  OSC"};
    static const int number_of_boards{1};
    static const int number_of_sections{20};
    int current_section{0};

    Heart *heart;
    SkinConductance *sc1;
    Respiration *resp;
    SkinConductance *sc2;

    sample processed_for_leds{};

// Packet structure
    enum PacketType {
        PACKET_UNKNOWN,
        PACKET_HEART,
        PACKET_SKIN,
        PACKET_RESPIRATION
    };

    struct heart_packet {
        int32_t magic_in = 0x11223344;
        int32_t checksum = 0;
        struct data {
            int16_t heart_raw = 0;
            bool heart_beat_detected =0;
            char padding = 0;
            float heart_normalized = 0;
            float heart_bpm = 0;
            float heart_bpm_change =0;
            float heart_amplitude_change = 0;
            int32_t checksum() {
                uint8_t* begin = reinterpret_cast<uint8_t*>(this);
                uint8_t* end = begin + sizeof(*this);
                int32_t sum = 0;
                for(auto it = begin ; it != end; ++it)   sum += *it; 
                return sum;
            }
        } data;
        int32_t magic_out = 0x44332211;
    } heartPacket;
    static_assert(sizeof(heart_packet) == 4+4+2+4+4+4+4+1+1+4, "Heart packet size should be 32 bytes");

    struct skin_packet {
        int32_t magic_in = 0x22334455;
        int32_t checksum =0;
        struct data {
            int32_t skin_raw =0;
            float skin_scr =0;
            float skin_scl =0;
            int32_t checksum() {
                uint8_t* begin = reinterpret_cast<uint8_t*>(this);
                uint8_t* end = begin + sizeof(*this);
                int32_t sum = 0;
                for(auto it = begin ; it != end; ++it)   sum += *it; 
                return sum;
            }
        } data;
        int32_t magic_out = 0x55443322;
    } skinPacket;
    static_assert(sizeof(skin_packet) == 4+4+4+4+4+4, "Skin packet size should be 24 bytes");

    struct resp_packet {
        int32_t magic_in = 0x33445566;
        int32_t checksum =0;

        struct data{
            int16_t resp_raw =0;
            bool resp_is_exhaling =0;
            char padding =0;
            float resp_normalized =0;
            float resp_scaled =0;
            float resp_raw_amplitude =0;
            float resp_normalized_amplitude =0;
            float resp_scaled_amplitude =0;
            float resp_amplitude_level =0;
            float resp_amplitude_change =0;
            float resp_amplitude_variability =0;
            float resp_interval =0;
            float resp_rpm =0;
            float resp_normalized_rpm =0;
            float resp_scaled_rpm =0;
            float resp_rpm_level =0;
            float resp_rpm_change =0;
            float resp_rpm_variability =0;
            int32_t checksum() {
                uint8_t* begin = reinterpret_cast<uint8_t*>(this);
                uint8_t* end = begin + sizeof(*this);
                int32_t sum = 0;
                for(auto it = begin ; it != end; ++it)   sum += *it; 
                return sum;
            }
        } data;
        int32_t magic_out = 0x66554433;
    } respPacket;
    static_assert(sizeof(resp_packet) == 4+4+2+1+1+4+4+4+4+4+4+4+4+4+4+4+4+4+4+4+4, "Resp packet size should be 76 bytes");

    const char sections_title[number_of_sections][17] = {"    Send OSC    "};

    public:

    Osc(Heart *_heart = nullptr,SkinConductance *_sc1 = nullptr, Respiration *_resp = nullptr,SkinConductance *_sc2 = nullptr): heart{_heart},sc1{_sc1},resp{_resp},sc2{_sc2}
    {};

    void debugPrint(const char* format, ...) {
            #ifdef DEBUG_PRINT
            va_list args;
            va_start(args, format);
            char buffer[100]; // allocate a buffer to hold the converted string
            vsnprintf(buffer, 100, format, args); // convert the format string to a buffer
            PRINTSERIAL.print(buffer);
            va_end(args);
            PRINTSERIAL.println();
            #endif
        }

    void checksum(PacketType packetType) {
    uint32_t checksum = 0;
    switch (packetType) {
        case PACKET_HEART: {
        checksum = 0;
        checksum = heartPacket.data.checksum();
        heartPacket.checksum = checksum;
        debugPrint("Sent Heart Checksum: %d", checksum);
        break;
        }
        case PACKET_SKIN: {
        checksum = 0;
        checksum = skinPacket.data.checksum();
        skinPacket.checksum = checksum;
        debugPrint("Sent Skin Checksum: %d", checksum);
        break;
        }
        case PACKET_RESPIRATION: {
        checksum = 0;
        checksum = respPacket.data.checksum();
        respPacket.checksum = checksum;
        debugPrint("Sent Resp Checksum: %d", checksum);
        break;
        }
        default:
        debugPrint("Unknown packet type: %d", packetType);
        break;
    }
    }

    void fillPacket(PacketType packetType) {
    switch (packetType) {
        case PACKET_HEART:
        heartPacket.data.heart_raw = heart->getRaw();
        heartPacket.data.heart_normalized = heart->getNormalized();
        heartPacket.data.heart_beat_detected = heart->beatDetected();
        heartPacket.data.heart_bpm = heart->getBPM();
        heartPacket.data.heart_bpm_change = heart->bpmChange();
        heartPacket.data.heart_amplitude_change = heart->amplitudeChange();
        debugPrint("Heart packet filled");
        break;
        case PACKET_SKIN:
        skinPacket.data.skin_raw = sc1->getRaw();
        skinPacket.data.skin_scr = sc1->getSCR();
        skinPacket.data.skin_scl = sc1->getSCL();
        debugPrint("Skin packet filled");
        break;
        case PACKET_RESPIRATION:
        respPacket.data.resp_raw = resp->getRaw();
        respPacket.data.resp_normalized = resp->getNormalized();
        respPacket.data.resp_scaled = resp->getScaled();
        respPacket.data.resp_is_exhaling = resp->isExhaling();
        respPacket.data.resp_raw_amplitude = resp->getRawAmplitude();
        respPacket.data.resp_normalized_amplitude = resp->getNormalizedAmplitude();
        respPacket.data.resp_scaled_amplitude = resp->getScaledAmplitude();
        respPacket.data.resp_amplitude_level = resp->getAmplitudeLevel();
        respPacket.data.resp_amplitude_change = resp->getAmplitudeChange();
        respPacket.data.resp_amplitude_variability = resp->getAmplitudeVariability();
        respPacket.data.resp_interval = resp->getInterval();
        respPacket.data.resp_rpm = resp->getRpm();
        respPacket.data.resp_normalized_rpm = resp->getNormalizedRpm();
        respPacket.data.resp_scaled_rpm = resp->getScaledRpm();
        respPacket.data.resp_rpm_level = resp->getRpmLevel();
        respPacket.data.resp_rpm_change = resp->getRpmChange();
        respPacket.data.resp_rpm_variability = resp->getRpmVariability();
        debugPrint("Resp packet filled");
        break;
    }
    checksum(packetType);
    }

    void sendPackets() {
    SENDSERIAL.write((uint8_t*)&heartPacket, sizeof(heartPacket));
    SENDSERIAL.write((uint8_t*)&skinPacket, sizeof(skinPacket));
    SENDSERIAL.write((uint8_t*)&respPacket, sizeof(respPacket));
    }

    void setup() override {
        // Setup Plaquette
        Plaquette.begin();
        // Initialize Serial port
        PRINTSERIAL.begin(115200);
        SENDSERIAL.begin(115200, SERIAL_8N1);
    }

    //Project update loop. Access the  biosensors from here, process the data and modify audio objects
    //The biosensor need to be accessed with arrow "->" instead of dots "." because we are dealing with pointers and not objects
    void update() override{
        if(sendMetro){
        fillPacket(PACKET_HEART);
        fillPacket(PACKET_SKIN);
        fillPacket(PACKET_RESPIRATION);
        sendPackets();
        }

       //LEDs
        processed_for_leds.heart.sig =  biosensors::heart.getNormalized();
        processed_for_leds.gsr.scr = biosensors::sc1.getSCR();
        processed_for_leds.resp.sig = biosensors::resp.getScaled();
    }

    //Project update volume loop. Modify here if project needs special volume clamping
    void updateVolume(float vol) override{

    }

    //This is where you set the sound difference for every sections

    void changeSection(const int currentSection) override //this is where we change sections AND frequencies...
    {
    Serial.println(currentSection);
    }

    const char* getName() override { //Do not modify, just copy paste to new project
        return name;
    };

    const int getNumberOfSection() override { //Do not modify, just copy paste to new project
        return number_of_sections;
    }

    const char* getSectionTitle(const int section_index) override { //Do not modify, just copy paste to new project
        return sections_title[section_index];
    }

    sample getLedProcessed() override{ //Do not modify, just copy paste to new project
        return processed_for_leds;
    }
    };
