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

SensorManager sensorManager(&ams5600, eStopPin, endStopPin, diagPin, faultPin, voltDetectPin);
Motor motor(&sensorManager, stepPin, dirPin, enablePin, m0Pin, m1Pin, ledPin);
Connection connection(randomSeedPin);
Display display(&sensorManager, &oled, &connection, connectionMode);
EthernetUDP Udp;

int driveMode = 0;
int motorSpeed = 0;
int motorSlope = 0;
int motorSteps = 0;
boolean motorDirection = 0;
int motorStepMode = 0;
boolean motorHold = 0;

boolean endStopped = false;

boolean eStopActive = false;
boolean endStopPressed = false;
boolean motorEnabled = false;
boolean jobDone = true;

boolean eStopActiveLast = false;
boolean endStopPressedLast = false;
boolean jobDoneLast = true;
boolean usbActive = false;

float voltage = 0.0;
float encoderAngle = 0.0;

float voltageLast = 0.0;
float encoderAngleLast = 0.0;

unsigned long previousMillis = 0;
const long sensorRefresh = 100;

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

unsigned int localPort = 8888;  // port to listen on
unsigned int remotePort = 8889; // port to send feedback messages

unsigned long currentMillis;

char packetBuffer[BUFFER_SIZE]; // buffer to hold incoming packet,
StaticJsonDocument<BUFFER_SIZE> doc;

void setup()
{
  Messenger messenger(&Serial);
  messenger.init(BAUD_SPEED);
  messenger.sendInfo("Ethersweep " + version);

  pinMode(stepPin, OUTPUT);
  pinMode(dirPin, OUTPUT);
  pinMode(enablePin, OUTPUT);

  pinMode(m0Pin, OUTPUT);
  pinMode(m1Pin, OUTPUT);
  pinMode(ledPin, OUTPUT);

  pinMode(diagPin, INPUT);
  pinMode(endStopPin, INPUT);
  pinMode(eStopPin, INPUT);

  digitalWrite(dirPin, LOW);

  if (!sensorManager.startUpCheck())
  {
    messenger.sendError("Sensors fail");
  }
  else
  {
    messenger.sendInfo("Sensors OK");
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
      messenger.sendError("Ethernet error");
    }
    if (Ethernet.linkStatus() == LinkOFF)
    {
      messenger.sendError("Ethernet no link");
    }
    messenger.sendInfo("Ethernet OK");
#else
    if (Ethernet.begin(mac) == 0)
    {
      if (Ethernet.hardwareStatus() == EthernetNoHardware)
      {
        messenger.sendError("Ethernet error");
      }
      if (Ethernet.linkStatus() == LinkOFF)
      {
        messenger.sendError("Ethernet no link");
      }
      Ethernet.begin(mac, ip);
    }
    else
    {
      messenger.sendInfo("Ethernet OK");
    }
#endif
    Udp.begin(localPort);
  }

  display.initializeDisplay(Ethernet.localIP());

  messenger.sendInfo("IP: " + display.formatAddress(Ethernet.localIP()));
  messenger.sendInfo("done");
}

void loop()
{
  sensorManager.getEmergencyStopState();
  sensorManager.getEndStopState();

  if (!usbActive)
  {
    int packetSize = Udp.parsePacket();
    if (packetSize)
    {
      // debugPrintln("loop");
      // debugPrint("Received packet of size ");
      // debugPrintln(packetSize);
      // debugPrint("From ");
      IPAddress remoteIP = Udp.remoteIP();
      // debugPrint(remoteIP);
      // debugPrint(", port ");
      // debugPrintln(Udp.remotePort());

      // read the packet into packetBufffer
      Udp.read(packetBuffer, BUFFER_SIZE);
      // debugPrintln("Contents:");
      // debugPrintln(packetBuffer);

      /*
        // reply receive message
        // send a reply to the IP address and port that sent us the packet we received
        Udp.beginPacket(Udp.remoteIP(), remotePort);
        Udp.write(packetBuffer);
        Udp.endPacket();
      */
      jobDone = false;
    }
  }
  else
  {
    if (Serial.available() > 0)
    {
      Serial.readStringUntil('\n').toCharArray(packetBuffer, BUFFER_SIZE);
      jobDone = false;
    }
  }

  currentMillis = millis();
  if (currentMillis - previousMillis >= sensorRefresh)
  {
    display.drawDisplay(); // time killer!
    previousMillis = currentMillis;
  }

  if (jobDone == false)
  {
    DeserializationError error = deserializeJson(doc, packetBuffer, BUFFER_SIZE);
    if (error)
    {
      // debugPrint(F("deserializeJson() failed: "));
      // debugPrintln(error.f_str());
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
      case STEPS: // steps
        motor.driveMotor(motorSteps, motorSpeed, motorDirection, motorStepMode, motorHold);
        // debugPrintln("motor driven");
        break;
      case HOME: // home
        motor.homeMotor(motorSteps, motorSpeed, motorDirection, motorStepMode, motorHold);
        // debugPrintln("motor homed");
        break;
      case RAMP: // ramp
        motor.rampMotor(motorSteps, motorSpeed, motorSlope, motorDirection, motorStepMode, motorHold);
        break;
      case POWERCYCLE: // power cycle
        motor.powerCycleMotor();
        break;
      }
    }

    jobDone = true;
  }
}
