#pragma once
#include "SensorManager.h"
#include "SSD1306AsciiWire.h"
#include "Connection.h"
#include <Ethernet.h>
#include <EthernetUdp.h>

class Display
{
private:
    bool emergencyStopState;
    bool endStopState;
    bool jobDone;
    bool motorDriverFailure;
    float encoderAngle;
    float voltage;

    bool lastEmergencyStopState;
    bool lastEndStopState;
    bool lastJobDone;
    bool lastMotorDriverFailure;
    float lastEncoderAngle;
    float lastVoltage;

    SensorManager *sensor;
    SSD1306AsciiWire *oled;
    Connection *connection;

public:
    Display(SensorManager *sensor, SSD1306AsciiWire *oled, Connection *connection);
    void getData();
    void drawDisplay();
    void setupDisplay();
    String formatAddress(IPAddress address);
    void initializeDisplay(IPAddress address);
};
