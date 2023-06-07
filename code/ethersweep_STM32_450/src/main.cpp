// FOR HARDWARE 4.5.0 for STM32F103C8T6 and TMC2209 controller
#include <Arduino.h>
#include <SPI.h>
#include <Ethernet.h>
#include <EthernetUdp.h>
#include <IPAddress.h>
#include <ArduinoJson.h>
#include <Wire.h>
#include "SSD1306Ascii.h"
#include "SSD1306AsciiWire.h"
#include "Configuration.h"
#include "Connection.h"
#include "SensorManager.h"
#include "Motor.h"
#include "Display.h"
#include "Messenger.h"


#if STATIC_IP
String connectionMode = CONNECTION_STAT;
IPAddress ip(192, 168, 1, 111); // static IP
#else
String connectionMode = CONNECTION_DHCP;
IPAddress ip(0, 0, 0, 0);
#endif

SSD1306AsciiWire oled;
AMS_5600 ams5600;
SensorManager sensorManager(&ams5600, E_STOP_PIN, END_STOP_PIN, DIAG_PIN, FAULT_PIN, VOLT_DETECT_PIN);
Connection connection(RANDOM_SEED_PIN);
Display display(&sensorManager, &oled, &connection);
Motor motor(&sensorManager, &display, STEP_PIN, DIR_PIN, ENABLE_PIN, M0_PIN, M1_PIN, LED_PIN);
Messenger messenger(&Serial1);
EthernetUDP Udp;

int driveMode = 0;
int motorSpeed = 0;
int motorSlope = 0;
int motorSteps = 0;
byte motorStepMode = 0;
bool motorDirection = false;
bool motorHold = false;

bool usbActive = false;

unsigned long previousMillis;

// ETHERNET & UDP
char packetBuffer[BUFFER_SIZE]; // buffer to hold incoming UDP packet
StaticJsonDocument<BUFFER_SIZE> doc;

void setup()
{
  Wire.begin();

  connection.setConnectionMode(connectionMode);
  messenger.init(BAUD_SPEED);
  messenger.sendInfo("Ethersweep " + version);
  
  if (sensorManager.startUpCheck(messenger))
  {
    messenger.sendInfo(F("Sensors OK"));
  }
  else
  {
    messenger.sendError(F("Sensor fail"));
  }

  connection.getMac();
  display.setupDisplay();
  motor.disableMotor();

  Serial1.setTimeout(10);

  if (Serial1.available() > 0)
  {
    usbActive = true;
    connectionMode = CONNECTION_USB;
  }
  else
  {
    Ethernet.init(CS_PIN);
#if STATIC_IP
    Ethernet.begin(connection.mac, ip);
    if (Ethernet.hardwareStatus() == EthernetNoHardware)
    {
      messenger.sendError(F("Ethernet error"));
    }
    if (Ethernet.linkStatus() == LinkOFF)
    {
      messenger.sendError(F("Ethernet no link"));
    }
    messenger.sendInfo(F("Ethernet OK"));
#else
    if (Ethernet.begin(connection.mac) == 0)
    {
      if (Ethernet.hardwareStatus() == EthernetNoHardware)
      {
        messenger.sendError(F("Ethernet error"));
      }
      if (Ethernet.linkStatus() == LinkOFF)
      {
        messenger.sendError(F("Ethernet no link"));
      }
      Ethernet.begin(connection.mac, ip);
    }
    else
    {
      messenger.sendInfo(F("Ethernet OK"));
    }
#endif
    Udp.begin(LOCAL_PORT);
  }

  display.initializeDisplay(Ethernet.localIP());

  messenger.sendInfo("IP: " + display.formatAddress(Ethernet.localIP()));
  messenger.sendInfo(F("done"));
}

void loop()
{
  sensorManager.getEmergencyStopState();
  sensorManager.getEndStopState();

  if (usbActive)
  {
    if (Serial.available() > 0)
    {
      Serial.readStringUntil('\n').toCharArray(packetBuffer, BUFFER_SIZE);
      sensorManager.setJobState(false);
    }
  }
  else
  {
    int packetSize = Udp.parsePacket();
    if (packetSize)
    {
      Udp.read(packetBuffer, BUFFER_SIZE);
      sensorManager.setJobState(false);
    }
  }

  if (millis() - previousMillis >= DISPLAY_REFRESH_TIME)
  {
    display.drawDisplay();
    previousMillis = millis();
  }

  if (!sensorManager.getJobState())
  {
    DeserializationError error = deserializeJson(doc, packetBuffer, BUFFER_SIZE);
    if (error)
    {
      messenger.sendError(F("JSON error"));
    }
    else
    {
      driveMode = doc["drivemode"];
      motorSteps = doc["steps"];
      motorSpeed = doc["speed"];
      motorSlope = doc["slope"];
      motorDirection = doc["direction"];
      motorStepMode = doc["stepmode"];
      motorHold = doc["hold"];

      switch (driveMode)
      {
      case STEPS:
        motor.driveMotor(motorSteps, motorSpeed, motorDirection, motorStepMode, motorHold);
        break;
      case HOME:
        motor.homeMotor(motorSteps, motorSpeed, motorDirection, motorStepMode, motorHold);
        break;
      case RAMP:
        motor.rampMotor(motorSteps, motorSpeed, motorSlope, motorDirection, motorStepMode, motorHold);
        break;
      case POWERCYCLE:
        motor.powerCycleMotor();
        break;
      }
    }

    sensorManager.setJobState(true);
  }
}
