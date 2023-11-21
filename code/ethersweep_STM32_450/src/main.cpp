// FOR HARDWARE 4.5.0 for STM32F103C8T6 and TMC2209 controller
#include <Arduino.h>
#include <SPI.h>
#include <Ethernet.h>
#include <EthernetUdp.h>
#include <IPAddress.h>
#include <ArduinoJson.h>
#include <Wire.h>
#include <EEPROM.h>
#include "Configurator.h"
#include "SSD1306Ascii.h"
#include "SSD1306AsciiWire.h"
#include "Configuration.h"
#include "Connection.h"
#include "SensorManager.h"
#include "Motor.h"
#include "Display.h"
#include "Messenger.h"

#include "w5100int_datasheet.h"


#if STATIC_IP
String connectionMode = CONNECTION_STAT;
IPAddress ip(192, 168, 1, 111); // static IP
#else
String connectionMode = CONNECTION_DHCP;
IPAddress ip(0, 0, 0, 0);
#endif

SSD1306AsciiWire oled;
AMS_5600 ams5600;
EthernetUDP Udp;

SensorManager sensorManager(&ams5600, E_STOP_PIN, END_STOP_PIN, DIAG_PIN, FAULT_PIN, VOLT_DETECT_PIN);
Connection connection(RANDOM_SEED_PIN);
Display display(&sensorManager, &oled, &connection);
Motor motor(&sensorManager, &display, STEP_PIN, DIR_PIN, ENABLE_PIN, M0_PIN, M1_PIN, LED_PIN);
Messenger messenger(&Serial1);
Configurator configurator;

int action = 0;
int motorSpeed = 0;
int motorSlope = 0;
int motorSteps = 0;
byte motorStepMode = 0;
bool motorDirection = false;
bool motorHold = false;
double degrees = 0.0;

IPAddress sensorDestinationIp(0, 0, 0, 0);     // IP to send sensor data to
int sensorPort = STANDARD_REMOTE_SENSOR_PORT;

bool usbActive = false;

unsigned long previousDisplayMillis;
unsigned long previousNetworkMillis;
unsigned long previousSensorMillis;
unsigned long previousMotorMillis;
unsigned long previousFeedbackMillis;

int displayRefreshTime;
int feedbackTime;

// ETHERNET & UDP
char packetBuffer[BUFFER_SIZE]; // buffer to hold incoming UDP packet
StaticJsonDocument<BUFFER_SIZE> doc;

int schedulerMode = 0;



// w5500 interrupt
volatile int SIRflag = 0;

void clearSIRs() { // After a socket IR, SnIR and SIR need to be reset
  for (int i = 0; i < 8; i++) {
    delay(1);
    W5100.writeSnIR(i, 0xFF); // Clear socket i interrupt
  }
  delay(1);
  W5100.writeSIR(0xFF); // Clear SIR
}

// disable interrupts for all sockets
inline void disableSIRs() {
  delay(1);
  W5100.writeSIMR(0x00);
}

// enable interrupts for all sockets
inline void enableSIRs() {
  delay(1);
  W5100.writeSIMR(0xFF);
}

// Interrupt service routine
void socketISR()
{
  SIRflag++;
}

void printIRstate() {
  // Conflict/Unreach/PPPoE/MP interrupt register (not used here):
  //  Serial.print("IR:");
  //  Serial.print(W5100.readIR(),HEX);
  //  Serial.print(" IMR:");
  //  Serial.print(W5100.readIMR(),HEX);
  // Socket IR registers:
  Serial1.print("SIR:");
  Serial1.print(W5100.readSIR(), HEX);
  delay(1);
  Serial1.print(" SIMR:");
  Serial1.print(W5100.readSIMR(), HEX);
  delay(1);
  Serial1.print(" SnIR:");
  for (int i = 0; i < 7; i++) {
    delay(1);
    Serial1.print(W5100.readSnIR(i), HEX);
    Serial1.print(",");
  }
  delay(1);
  Serial1.print(W5100.readSnIR(7), HEX);
  Serial1.print(" SnIMR:");
  for (int i = 0; i < 7; i++) {
    delay(1);
    Serial1.print(W5100.readSnIMR(i), HEX);
    Serial1.print(",");
  }
  delay(1);
  Serial1.print(W5100.readSnIMR(7), HEX);
  // State of the INTn line:
  Serial1.print(" INTn:");
  Serial1.println(digitalRead(ETHERNET_INTER_PIN));
}

void receiveAll() { // Receive ethernet data and print it to serial
  /*
  EthernetClient client = server.available();
  while (client) {
    Serial.println("Client available");
    while (client.available() > 0) {
      Serial.print(client.read(), HEX);
    }
    client = server.available();
  }
  */
  Serial1.println("interrupt triggered from W5500!!!");
}



void setup()
{
  Wire.begin();

  connection.setConnectionMode(connectionMode);
  messenger.init(BAUD_SPEED);
  configurator.loadData();
  connection.getMac();
  display.setupDisplay();
  motor.disableMotor();
  
  Serial1.setTimeout(10);
  

  displayRefreshTime = configurator.getDisplayRefreshTime();
  feedbackTime = configurator.getFeedbackTime();
  Serial1.println("Ethersweep " + version);

  Serial1.println("init start");
  pinMode(RESET_PIN, OUTPUT);
  digitalWrite(RESET_PIN, LOW);
  delay(2);
  digitalWrite(RESET_PIN, HIGH);
  
  // w5500 interrupt
  pinMode(ETHERNET_INTER_PIN, INPUT);
  Serial1.print("Read INTn pin:");
  Serial1.println(digitalRead(ETHERNET_INTER_PIN));
  Serial1.print("Register states before enabling IRs: "); printIRstate();
  
  for (int i = 0; i < 8; i++) {
    W5100.writeSnIMR(i, 0x04); // Socket IR mask: RECV for all sockets
  }
  enableSIRs();

  Serial1.print("Register states after  enabling IRs: "); printIRstate();

  attachInterrupt(digitalPinToInterrupt(ETHERNET_INTER_PIN), socketISR, FALLING); // For some reason, SocketISR gets called by this line...
  SIRflag = 0; // ...therefore, we reset it

  
  
  

  if (sensorManager.startUpCheck(messenger))
  {
    messenger.sendInfo(F("Sensors OK"));
  }
  else
  {
    messenger.sendError(F("Sensor fail"));
  }

  

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
  if(schedulerMode == standby) 
  {
  }
  else if (schedulerMode == drive)
  {
  }
  else if (schedulerMode == feedback)
  {
  }

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

  if (millis() - previousDisplayMillis >= displayRefreshTime)
  {
    display.drawDisplay();
    previousDisplayMillis = millis();
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
        

      action = doc["mode"];
      motorSteps = doc["steps"];
      motorSpeed = doc["speed"];
      motorSlope = doc["slope"];
      motorDirection = doc["direction"];
      motorStepMode = doc["stepmode"];
      motorHold = doc["hold"];

      if (doc.containsKey("degrees")) {
        degrees = doc["degrees"];
      }

      if (doc.containsKey("port")) {
        sensorPort = doc["port"];
      }
      
      if (doc.containsKey("ip")) {
        const char* ipAddressStr = doc["ip"];
        sensorDestinationIp.fromString(ipAddressStr);
      }
      else {
        sensorDestinationIp = Udp.remoteIP();
      }

      switch (action)
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
      case POSITION:
        motor.positionMotor(degrees);
        break;
      case SENSORFEEDBACK:
        motor.sensorFeedback(sensorDestinationIp, sensorPort, messenger, Udp);
        break;
      case HEARTBEAT:
        motor.sendHeartbeat(sensorDestinationIp, sensorPort, messenger, Udp);
        break;
      case IDENTIFY:
        motor.identify(sensorDestinationIp, sensorPort, messenger, Udp);
        break;
      case CONFIGURE:
        configurator.processNewConfiguration();
        break;
      case POWERCYCLE:
        motor.powerCycleMotor();
        break;
      }
    }

    sensorManager.setJobState(true);
  }
}

