#include "Display.h"
#include "Configuration.h"

Display::Display(SensorManager *sensor, SSD1306AsciiWire *oled, Connection *connection)
{
    this->sensor = sensor;
    this->oled = oled;
    this->connection = connection;
}

// reads and gets data from sensor
void Display::getData()
{
    this->emergencyStopState = this->sensor->getEmergencyStopState();
    this->endStopState = this->sensor->getEndStopState();
    this->motorDriverFailure = this->sensor->getMotorDriverFailure();
    this->encoderAngle = this->sensor->getAngle();
    this->voltage = this->sensor->getVoltage();
    this->jobDone = this->sensor->getJobState();
}

// refresh and draw all data on display
void Display::drawDisplay()
{
    this->getData();

    // voltage display
    if (voltage != this->lastVoltage)
    {
        this->oled->clearField(0, 1, 4);
        // if (voltage < 10.0) oled.print("");
        this->oled->print(voltage / 1000, 1);
    }

    // encoder angle display
    if (encoderAngle != this->lastEncoderAngle)
    {
        this->oled->clearField(90, 1, 6);
        this->oled->print(encoderAngle, 1);
    }

    // end stop display
    if (this->endStopState != this->lastEndStopState)
    {
        this->oled->clearField(0, 2, 4);
        if (this->endStopState)
        {
            this->oled->setInvertMode(1);
        }
        this->oled->print("END");
        this->oled->setInvertMode(0);
    }

    // emergency stop display
    if (this->emergencyStopState != this->lastEmergencyStopState)
    {
        this->oled->clearField(47, 2, 4);
        if (this->emergencyStopState)
        {
            this->oled->setInvertMode(1);
        }
        this->oled->print("STOP");
        this->oled->setInvertMode(0);
    }

    // active state display
    if (this->jobDone != this->lastJobDone)
    {
        this->oled->clearField(96, 2, 4);
        if (!this->jobDone)
        {
            this->oled->setInvertMode(1);
        }
        this->oled->print("ACT");
        this->oled->setInvertMode(0);
    }

    // writes memory data to only refresh new data on display
    this->lastVoltage = this->voltage;
    this->lastEncoderAngle = this->encoderAngle;
    this->lastEndStopState = this->endStopState;
    this->lastEmergencyStopState = this->emergencyStopState;
    this->lastJobDone = this->jobDone;
}

// shows display at setup time (startup)
void Display::setupDisplay()
{
    this->oled->begin(&Adafruit128x32, OLED_I2C_ADDRESS);
    this->oled->setFont(System5x7);
    this->oled->set2X();
    this->oled->clear();
    this->oled->println(F("ethersweep"));
    this->oled->set1X();
    this->oled->println(F(" "));
    this->oled->println("       v" + version);
}

// formats IP to a human readable string
String Display::formatAddress(IPAddress address)
{
    if (address[0] == 255 && address[1] == 255 && address[2] == 255 && address[3] == 255)
    {
        return "none";
    }
    return String(address[0]) + "." +
           String(address[1]) + "." +
           String(address[2]) + "." +
           String(address[3]);
}

// initializes display that has to happen once on startup. Screen is saved in memory of display.
void Display::initializeDisplay(IPAddress address)
{
    this->oled->setFont(System5x7);
    this->oled->clear();
    this->oled->println("ethersweep    v" + version);
    this->oled->println("00.0V | " + this->connection->connectionMode + " | 000.0°");
    this->oled->println(F("END   | STOP |  ACT"));
    this->oled->println("IP: " + this->formatAddress(address));
}
