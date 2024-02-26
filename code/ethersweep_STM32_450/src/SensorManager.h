#pragma once
#include <Arduino.h>
#include <AS5600.h>
#include "Messenger.h"


class SensorManager
{
private:
    float absoluteAngle;
    int absoluteRotations;
    byte eStopPin;
    byte endStopPin;
    byte diagPin;
    byte faultPin;
    byte voltDetectPin;
    bool jobDone;
    AMS_5600 *ams5600;

    void init();

public:
    int voltage;
    float angle;
    bool magnet;
    bool emergencyStop;
    bool endStop;
    bool motorDriverDiagnose;
    bool motorDriverFailure;
    

    SensorManager(AMS_5600 *ams5600, byte eStopPin, byte endStopPin, byte diagPin, byte faultPin, byte voltDetectPin);
    void readSensorValues();
    int getVoltage();
    float getAngle();
    void refreshAngle();
    bool startUpCheck(Messenger messenger);
    bool checkMagnet();
    bool getEmergencyStopState();
    bool getEndStopState();
    bool getMotorDriverDiagnose();
    bool getMotorDriverFailure();
    bool getJobState();
    bool setJobState(bool state);
    
};
