#pragma once

#include "SensorManager.h"

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

public:

    Motor(SensorManager *sensor, byte stepPin, byte dirPin, byte enablePin, byte m0Pin, byte m1Pin, byte ledPin);
    void setStepMode(int mode);
    void driveMotor(int motorSteps, int motorSpeed, bool motorDirection, int motorStepMode, bool hold);
    void rampMotor(int motorSteps, int motorSpeed, int motorSlope, bool motorDirection, int motorStepMode, bool hold);
    void homeMotor(int motorSteps, int motorSpeed, bool motorDirection, int motorStepMode, bool hold);
    void disableMotor();
    void enableMotor();
    void powerCycleMotor();
};
