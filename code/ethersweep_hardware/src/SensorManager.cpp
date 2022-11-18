#include <Arduino.h>
#include "SensorManager.h"

SensorManager::SensorManager(AMS_5600 *ams5600, byte eStopPin, byte endStopPin, byte diagPin, byte faultPin, byte voltDetectPin)
{
    this->eStopPin = eStopPin;
    this->endStopPin = endStopPin;
    this->voltDetectPin = voltDetectPin;
    this->diagPin = diagPin;
    this->faultPin = faultPin;
    this->jobDone = true;

    this->ams5600 = ams5600;
}

void SensorManager::readSensorValues()
{
    this->voltage = getVoltage();
    this->angle = getAngle();
    this->magnet = checkMagnet();
    this->emergencyStop = getEmergencyStopState();
    this->endStop = getEndStopState();
    this->motorDriverDiagnose = getMotorDriverDiagnose();
    this->motorDriverFailure = getMotorDriverFailure();
    this->jobState = getJobState();
}

bool SensorManager::startUpCheck()
{
    this->readSensorValues();

    if (motorDriverDiagnose == 1)
        return false;
    if (motorDriverFailure == 1)
        return false;
    if (magnet == 0)
        return false;
    if (voltage < 5.0)
        return false;

    return true;
}

float SensorManager::getVoltage()
{
    float vin = (analogRead(this->voltDetectPin) * 3.3) / 1024.0 / (1.0 / 11.0);
    return vin;
}

float SensorManager::getAngle()
{
    float rawAngle = this->ams5600->getRawAngle();
    /* Raw data reports 0 - 4095 segments, which is 0.087890625 of a degree */
    float normalizedAngle = (rawAngle * 360.0) / 4096.0;

    return normalizedAngle;
}

bool SensorManager::checkMagnet()
{
    if (this->ams5600->detectMagnet() == 0)
    {
        while (1)
        {
            if (this->ams5600->detectMagnet() == 1)
            {
                return true;
            }
            else
            {
                // debugPrintln("magnet err");
            }
            delay(100);
        }
    }
    return true;
}

bool SensorManager::getEmergencyStopState()
{
    return !digitalRead(this->eStopPin); // Emergency Stop pressed if HIGH
}

bool SensorManager::getEndStopState()
{
    return !digitalRead(this->endStopPin); // END Stop pressed if HIGH
}

bool SensorManager::getMotorDriverDiagnose()
{
    return digitalRead(this->diagPin); // Motor driver diagnose if HIGH
}

bool SensorManager::getMotorDriverFailure()
{
    return digitalRead(this->faultPin); // Motor driver failure if HIGH
}

bool SensorManager::getJobState() // Running job state reuturns TRUE when in idle
{
    return this->jobDone;
}

bool SensorManager::setJobState(boolean state) // sets job state running 0 or idle 1
{
    return this->jobDone = state;
}
