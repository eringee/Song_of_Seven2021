# BioSynth

## Description
The BioSynth is an affective biofeedback device that turns physiological signals —heart rate, skin conductance, and respiration—into sound, light, and other expressive media. 
Created by Erin Gee and collaborators, it uses Gee's [BioData](https://github.com/eringee/BioData.git) library to convert raw bioignals into meaningful indicators for artistic use. The system is built with creative technologists, performers, and researchers in mind, to facilitate integration between body data and interactive media environments.

BioSynth is modular, extensible, and designed for embedded systems using [PlatformIO](https://platformio.org/) for development. It supports features like OSC communication and real-time data logging. This repository serves as a central hub for all related projects and code associated with the BioSynth ecosystem.

## Music pieces
This repository contains the pieces Song Of Seven (2021) and Affect Flow (2022).

## Features 
Features include :
- Sending biodata through OSC with ESP32
- Logging biodata on an SD card
- Using an external ADC for 16-bit data resolution

## PlatformIO Environments
Each show or project is encapsulated in a PlatformIO environment that can be independently built and uploaded. 

Environments included are : 
- cinexmedia (to record biodata on an SD card)
- teensy_OSC (to send biodata through OSC)
- tinypico_OSC (to send biodata through OSC)
- calgary_affect_flow (latest version of Affect Flow show with OSC light control)
- basic_song_of_seven (latest version of Song Of Seven)

## More
2021 schematic : https://oshwlab.com/siri87/biosynth2021 

 
