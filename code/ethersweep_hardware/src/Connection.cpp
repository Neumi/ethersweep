#include <Arduino.h>
#include <EEPROM.h>
#include "Connection.h"


Connection::Connection(byte randomSeedPin)
{
    randomSeed(analogRead(randomSeedPin));
}

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

void Connection::generateNewMacEeprom()
{
    for (int i = 1; i <= 5; i++)
    {
        EEPROM.write(i, random(255));
    }
}

int *Connection::getMac()
{
    if (checkMacAddress())
    {
        generateNewMacEeprom();
    }
    return this->mac;
}

void Connection::clearMac()
{
    for (int i = 1; i <= 5; i++)
    {
        EEPROM.write(i, 255);
    }
    this->macUnwritten = true;
}

void Connection::setConnectionMode(String connectionMode)
{
    this->connectionMode = connectionMode;
}
