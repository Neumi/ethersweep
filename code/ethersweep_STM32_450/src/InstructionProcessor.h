#pragma once
#include <Arduino.h>
#include <IPAddress.h>
#include <Ethernet.h>
#include <EthernetUdp.h>
#include <ArduinoJson.h>
#include "Configuration.h"



class InstructionProcessor
{
    private:
        StaticJsonDocument<BUFFER_SIZE> *doc;

    public:
        InstructionProcessor(StaticJsonDocument<BUFFER_SIZE> *doc);
        DeserializationError processInstruction(char packetBuffer[BUFFER_SIZE]);

};
