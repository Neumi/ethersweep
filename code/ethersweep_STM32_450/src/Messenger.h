#pragma once
#include <Arduino.h>

class Messenger
{

private:
    HardwareSerial *serialInterface;

public:
    Messenger(HardwareSerial *serial);
    void init(int baud);
    void sendError(String message);
    void sendInfo(String message);
};
