#pragma once
#include "SensorManager.h"
#include "Display.h"

class Motor
{
private:
    bool motorEnabled = false;
    bool eStopActive = false;
    bool endStopPressed = false;

    byte stepPin;
    byte dirPin;
    byte enablePin;
    byte m0Pin;
    byte m1Pin;
    byte ledPin;

    SensorManager *sensor;
    Display *display;


public:
    Motor(SensorManager *sensor, Display *display, byte stepPin, byte dirPin, byte enablePin, byte m0Pin, byte m1Pin, byte ledPin);
    void setStepMode(byte mode);
    void driveMotor(int motorSteps, int motorSpeed, bool motorDirection, byte motorStepMode, bool hold);
    void rampMotor(int motorSteps, int motorSpeed, int motorSlope, bool motorDirection, byte motorStepMode, bool hold);
    void homeMotor(int motorSteps, int motorSpeed, bool motorDirection, byte motorStepMode, bool hold);
    void disableMotor();
    void enableMotor();
    void powerCycleMotor();
};
