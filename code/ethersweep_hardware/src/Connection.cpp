#include <Arduino.h>
#include <EEPROM.h>
#include "Connection.h"


Connection::Connection(byte randomSeedPin)
{
    randomSeed(analogRead(randomSeedPin));
}

// checks if MAC address is already written in EEPROM
bool Connection::checkMacAddress()
{
    for (int i = 1; i <= 5; i++)
    {
        int EEPROMvalue = EEPROM.read(i);
        this->mac[i] = EEPROMvalue;
        if (EEPROMvalue != 255)
        {
            this->macUnwritten = false;
        }
    }

    return this->macUnwritten;
}

// generates randomized MAC address and writes to EEPROM
void Connection::generateNewMacEeprom()
{
    for (int i = 1; i <= 5; i++)
    {
        EEPROM.write(i, random(255));
    }
}

// clears EEPROM, only used for debugging
void Connection::clearMac()
{
    for (int i = 1; i <= 5; i++)
    {
        EEPROM.write(i, 255);
    }
    this->macUnwritten = true;
}

// returns MAC address, generates new if not existing
int *Connection::getMac()
{
    if (checkMacAddress())
    {
        generateNewMacEeprom();
    }
    return this->mac;
}

// sets connection mode, DHCP, STATIC or USB
void Connection::setConnectionMode(String connectionMode)
{
    this->connectionMode = connectionMode;
}
