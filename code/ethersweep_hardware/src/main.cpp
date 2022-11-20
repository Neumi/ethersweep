// FOR HARDWARE 3.0.3 and 3.0.4 and above
#include <Arduino.h>
#include <Ethernet.h>
#include <EthernetUdp.h>
#include <IPAddress.h>
#include <ArduinoJson.h>
#include "SSD1306Ascii.h"
#include "SSD1306AsciiAvrI2c.h"
#include <Wire.h>
#include <AS5600.h>
#include "Configuration.h"
#include "Connection.h"
#include "SensorManager.h"
#include "Motor.h"
#include "Display.h"
#include "Messenger.h"

SSD1306AsciiAvrI2c oled;
AMS_5600 ams5600;

#if STATIC_IP
String connectionMode = "STAT";
IPAddress ip(192, 168, 1, 111); // static IP
#else
String connectionMode = "DHCP";
IPAddress ip(0, 0, 0, 0);
#endif

SensorManager sensorManager(&ams5600, E_STOP_PIN, END_STOP_PIN, DIAG_PIN, FAULT_PIN, VOLT_DETECT_PIN);
Connection connection(RANDOM_SEED_PIN);
Display display(&sensorManager, &oled, &connection, connectionMode);
Motor motor(&sensorManager, &display, STEP_PIN, DIR_PIN, ENABLE_PIN, M0_PIN, M1_PIN, LED_PIN);
EthernetUDP Udp;

int driveMode = 0;
int motorSpeed = 0;
int motorSlope = 0;
int motorSteps = 0;
boolean motorDirection = 0;
byte motorStepMode = 0;
boolean motorHold = 0;

boolean endStopped = false;

boolean eStopActive = false;
boolean endStopPressed = false;
boolean motorEnabled = false;

boolean eStopActiveLast = false;
boolean endStopPressedLast = false;
boolean usbActive = false;


unsigned long previousMillis = 0;

const byte macEepromStartAddress = 1; // has to be one, because first MAC address element is not to be changed
const byte macEepromEndAddress = 5;
boolean macUnwritten = true;

/*
#if DEBUG_MODE
#define debugBegin(...) Serial.begin(__VA_ARGS__)
#define debugPrint(...) Serial.print(__VA_ARGS__)
#define debugPrintln(...) Serial.println(__VA_ARGS__)
#else
#define debugBegin(...)
#define debugPrint(...)
#define debugPrintln(...)
#endif
*/

byte mac[] = {
    0xDE, 0x00, 0x00, 0x00, 0x00, 0x00 // first element is not to be changed as it is defined by MAC protocol
};

unsigned long currentMillis;

char packetBuffer[BUFFER_SIZE]; // buffer to hold incoming packet,
StaticJsonDocument<BUFFER_SIZE> doc;
Messenger messenger(&Serial);

void setup()
{

  messenger.init(BAUD_SPEED);
  messenger.sendInfo("Ethersweep " + version);

  pinMode(STEP_PIN, OUTPUT);
  pinMode(DIR_PIN, OUTPUT);
  pinMode(ENABLE_PIN, OUTPUT);

  pinMode(M0_PIN, OUTPUT);
  pinMode(M1_PIN, OUTPUT);
  pinMode(LED_PIN, OUTPUT);

  pinMode(DIAG_PIN, INPUT);
  pinMode(END_STOP_PIN, INPUT);
  pinMode(E_STOP_PIN, INPUT);

  digitalWrite(STEP_PIN, LOW);

  if (!sensorManager.startUpCheck())
  {
    messenger.sendError(F("Sensors fail"));
  }
  else
  {
    messenger.sendInfo(F("Sensors OK"));
  }

  connection.getMac();
  display.setupDisplay();
  motor.disableMotor();

  Serial.setTimeout(10);
  if (Serial.available() > 0)
  {
    usbActive = true;
    connectionMode = "USB ";
  }
  else
  {
    usbActive = false;
  }

  if (!usbActive)
  {
#if STATIC_IP
    Ethernet.begin(mac, ip);
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
    if (Ethernet.begin(mac) == 0)
    {
      if (Ethernet.hardwareStatus() == EthernetNoHardware)
      {
        messenger.sendError(F("Ethernet error"));
      }
      if (Ethernet.linkStatus() == LinkOFF)
      {
        messenger.sendError(F("Ethernet no link"));
      }
      Ethernet.begin(mac, ip);
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

  currentMillis = millis();
  if (currentMillis - previousMillis >= DISPLAY_REFRESH)
  {
    display.drawDisplay();
    previousMillis = currentMillis;
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
