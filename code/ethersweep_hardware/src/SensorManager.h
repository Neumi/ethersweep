#pragma once
#include <Arduino.h>
#include <AS5600.h>

class SensorManager
{
private:
    byte eStopPin;
    byte endStopPin;
    byte diagPin;
    byte faultPin;
    byte voltDetectPin;
    boolean jobDone;
    AMS_5600 *ams5600;

public:
    float voltage;
    float angle;
    bool magnet;
    bool emergencyStop;
    bool endStop;
    bool motorDriverDiagnose;
    bool motorDriverFailure;
    

    SensorManager(AMS_5600 *ams5600, byte eStopPin, byte endStopPin, byte diagPin, byte faultPin, byte voltDetectPin);
    void readSensorValues();
    float getVoltage();
    float getAngle();
    bool startUpCheck();
    bool checkMagnet();
    bool getEmergencyStopState();
    bool getEndStopState();
    bool getMotorDriverDiagnose();
    bool getMotorDriverFailure();
    bool getJobState();
    bool setJobState(boolean state);
    
};
