#include "InstructionProcessor.h"
#include "ArduinoJson.h"
#include "Configuration.h"

InstructionProcessor::InstructionProcessor(StaticJsonDocument<BUFFER_SIZE> *doc)
{
    this->doc = doc;
}

DeserializationError InstructionProcessor::processInstruction(char packetBuffer[BUFFER_SIZE])
{
    DeserializationError error = deserializeJson(*doc, packetBuffer, BUFFER_SIZE);
    if(!error ) {
        
    }
    return error;
}
