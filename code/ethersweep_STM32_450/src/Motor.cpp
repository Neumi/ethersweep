#include "Motor.h"

Motor::Motor(SensorManager *sensor, Display *display, byte stepPin, byte dirPin, byte enablePin, byte m0Pin, byte m1Pin, byte ledPin)
{
    this->sensor = sensor;
    this->display = display;
    this->stepPin = stepPin;
    this->dirPin = dirPin;
    this->enablePin = enablePin;
    this->m0Pin = m0Pin;
    this->m1Pin = m1Pin;
    this->ledPin = ledPin;

    init();
}

// inits all outputs.
void Motor::init()
{
    pinMode(this->stepPin, OUTPUT);
    pinMode(this->dirPin, OUTPUT);
    pinMode(this->enablePin, OUTPUT);
    pinMode(this->m0Pin, OUTPUT);
    pinMode(this->m1Pin, OUTPUT);
    pinMode(this->ledPin, OUTPUT);

    digitalWrite(this->stepPin, LOW);
}

// sets step mode. options are half, quater, eigth and sixteenth step mode.
void Motor::setStepMode(byte mode)
{
    switch (mode)
    {
    // TMC2208 MS0, MS1: 10: 1/2, 01: 1/4, 00: 1/8, 11: 1/16
    // TMC2209 M1, M0: 00: 1/8, 01: 1/32, 10: 1/64 11: 1/16
    case 8:
        digitalWrite(this->m0Pin, LOW);
        digitalWrite(this->m1Pin, LOW);
        break;
    case 16:
        digitalWrite(this->m0Pin, HIGH);
        digitalWrite(this->m1Pin, HIGH);
        break;
    case 32:
        digitalWrite(this->m0Pin, HIGH);
        digitalWrite(this->m1Pin, LOW);
        break;
    case 64:
        digitalWrite(this->m0Pin, LOW);
        digitalWrite(this->m1Pin, HIGH);
        break;
    default: // defaults to 1/64
        digitalWrite(this->m0Pin, LOW);
        digitalWrite(this->m1Pin, HIGH);
        break;
    }
}

// drives motor a certain number of steps.
void Motor::driveMotor(int motorSteps, int motorSpeed, bool motorDirection, byte motorStepMode, bool hold)
{
    this->display->drawDisplay();

    digitalWrite(this->ledPin, HIGH);
    if (!this->sensor->getEmergencyStopState())
    {
        this->enableMotor();
        this->setStepMode(motorStepMode);
        if (motorDirection == 1)
        {
            digitalWrite(this->dirPin, HIGH);
        }
        else if (motorDirection == 0)
        {
            digitalWrite(this->dirPin, LOW);
        }
        motorSpeed = constrain(motorSpeed, 0, 10000);
        for (int i = 0; i < motorSteps; i++)
        {
            if (this->sensor->getEmergencyStopState())
            {
                this->disableMotor();
                break;
            }
            digitalWrite(this->stepPin, LOW);
            delayMicroseconds(motorSpeed);
            digitalWrite(this->stepPin, HIGH);
            delayMicroseconds(motorSpeed);
        }
        if (hold == 0)
        {
            this->disableMotor();
        }
    }
    digitalWrite(this->ledPin, LOW);
}

// ramps motor to speed. Ramp of 10 % means 5% acceleration, 90% running full speed and 5% decelleration.
void Motor::rampMotor(int motorSteps, int motorSpeed, int motorSlope, bool motorDirection, byte motorStepMode, bool hold)
{
    this->display->drawDisplay();
    digitalWrite(this->ledPin, HIGH);

    float speedCorrection = 0.0;

    this->enableMotor();
    this->setStepMode(motorStepMode);

    if (motorDirection == 1)
    {
        digitalWrite(this->dirPin, HIGH);
    }
    else if (motorDirection == 0)
    {
        digitalWrite(this->dirPin, LOW);
    }
    motorSpeed = constrain(motorSpeed, 0, 10000);
    float slopeSteps = motorSteps / (100 / motorSlope) / 2.0;

    for (int i = 0; i < motorSteps; i++)
    {
        if (slopeSteps > i)
        {
            speedCorrection = (slopeSteps - i) / slopeSteps;
        }
        if (i > motorSteps - slopeSteps)
        {
            speedCorrection = 1.0 - ((motorSteps - i) / slopeSteps);
        }
        if (this->sensor->getEmergencyStopState())
        {
            this->disableMotor();
            break;
        }
        digitalWrite(this->stepPin, LOW);
        delayMicroseconds(motorSpeed + round(motorSpeed * speedCorrection));
        digitalWrite(this->stepPin, HIGH);
        delayMicroseconds(motorSpeed + round(motorSpeed * speedCorrection));
    }
    if (hold == 0)
    {
        this->disableMotor();
    }

    digitalWrite(this->ledPin, LOW);
}

// homes motor until endstop is pressed. After first End Stop hit, moves back and tries again slower.
void Motor::homeMotor(int motorSteps, int motorSpeed, bool motorDirection, byte motorStepMode, bool hold)
{
    int homingState = 0;
    const int homing_runs = 3;
    while (true)
    {
        if (homingState == homing_runs)
        {
            break;
        }
        if (this->sensor->getEmergencyStopState())
        {
            this->disableMotor();
            break;
        }

        if (this->sensor->getEndStopState())
        {
            if (homingState < homing_runs - 1)
            {
                driveMotor(motorSteps, motorSpeed * (homingState + 1), !motorDirection, motorStepMode, 1);
            }
            homingState += 1;
        }
        driveMotor(1, motorSpeed * (2 * homingState + 1), motorDirection, motorStepMode, 1);
    }
}

// disbales motor
void Motor::disableMotor()
{
    digitalWrite(this->enablePin, HIGH); // motor disabled
    this->motorEnabled = false;
}

// enables motor
void Motor::enableMotor()
{
    if (!this->eStopActive)
    {
        digitalWrite(this->enablePin, LOW); // motor enabled
        this->motorEnabled = true;
    }
    else
    {
        this->disableMotor();
    }
}

// power cycles motor controller
void Motor::powerCycleMotor()
{
    this->disableMotor();
    delay(20);
    this->enableMotor();
    delay(20);
}
