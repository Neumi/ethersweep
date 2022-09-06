// FOR HARDWARE 3.0.3 ... 3.0.6
#include <Ethernet.h>
#include <EthernetUdp.h>
#include <IPAddress.h>
#include <ArduinoJson.h>
#include <EEPROM.h>
#include "SSD1306Ascii.h"
#include "SSD1306AsciiAvrI2c.h"
#include <Wire.h>
#include <AS5600.h>
#include <EEPROM.h>

#define SYS_VOL   3.3

AMS_5600 ams5600;

// motor driver
const int stepPin = 7;
const int dirPin = 6;
const int enablePin = 5;
// const  int sleepPin = 8; // ethernet INT
// const  int resetPin = 9; // ethernet RST

const int m0Pin = A1;
const int m1Pin = A0;
const int diagPin = A2;
const int faultPin = A3;
const int voltDetectPin = A6;
const int endStopPin = 3;
const int eStopPin = 4;
const int ledPin = 2;
const int randomSeedPin = A7;

int driveMode = 0;
int motorSpeed = 0;
int motorSteps = 0;
int motorDirection = 0;
int motorStepMode = 0;
int motorHold = 0;

boolean endStopped = false;

boolean eStopActive = false;
boolean endStopPressed = false;
boolean motorEnabled = false;
boolean jobDone = true;

boolean eStopActiveLast = false;
boolean endStopPressedLast = false;
boolean jobDoneLast = true;

float voltage = 0.0;
float encoderAngle = 0.0;

float voltageLast = 0.0;
float encoderAngleLast = 0.0;


unsigned long previousMillis = 0;
const long sensorRefresh = 50;

const byte macEepromStartAddress = 1; // has to be one, because first MAC address element is not to be changed
const byte macEepromEndAddress = 5;
boolean macUnwritten = true;

#define I2C_ADDRESS 0x3C
SSD1306AsciiAvrI2c oled;




#define DEBUG  //comment line to disable debugging mode

#ifdef DEBUG
#define debugBegin(...) Serial.begin(__VA_ARGS__)
#define debugPrint(...) Serial.print(__VA_ARGS__)
#define debugPrintln(...) Serial.println(__VA_ARGS__)
#else
#define debugBegin(...)
#define debugPrint(...)
#define debugPrintln(...)
#endif

#define STATIC 0
#if STATIC
static String ip_mode = "STAT";
IPAddress ip(192, 168, 2, 111); // static IP
#else
static String ip_mode = "DHCP";
IPAddress ip(0, 0, 0, 0);
#endif



byte mac[] = {
  0xDE, 0x00, 0x00, 0x00, 0x00, 0x00 // first element is not to be changed
};


unsigned int localPort = 8888;      // local port to listen on
unsigned int remotePort = 8889;

// buffers for receiving and sending data

const int packetBufferSize = 128;
char packetBuffer[packetBufferSize];  // buffer to hold incoming packet,

// An EthernetUDP instance to let us send and receive packets over UDP
EthernetUDP Udp;

String received_data = "0";

static uint32_t timer;

unsigned long currentMillis;

void setup() {
  debugBegin(9600);

  pinMode(stepPin, OUTPUT);
  pinMode(dirPin, OUTPUT);
  pinMode(enablePin, OUTPUT);

  // pinMode(sleepPin, OUTPUT);
  // pinMode(resetPin, OUTPUT);

  pinMode(m0Pin, OUTPUT);
  pinMode(m1Pin, OUTPUT);
  pinMode(ledPin, OUTPUT);

  pinMode(diagPin, INPUT);
  pinMode(endStopPin, INPUT);
  pinMode(eStopPin, INPUT);

  digitalWrite(dirPin, LOW);

  randomSeed(analogRead(randomSeedPin));

  disableMotor();

  setupDisplay();
  initializeDisplay();
  drawDisplay();

}

void loop() {
  // put your main code here, to run repeatedly:

  drawDisplay();
  currentMillis = millis();
  if (currentMillis - previousMillis >= sensorRefresh) {
    //drawDisplay(); // time killer!
    previousMillis = currentMillis;
  }

  driveServo(180.0, 16);
  //driveMotor(int motorSteps, int motorSpeed, bool motorDirection, int motorStepMode, int hold)

}

void driveServo(float desiredAngle, byte stepMode) {
  float currAngle = getEncoderAngle();
  float errorAngle = currAngle - desiredAngle;

  // 16 * 200 steps / rev = 8,888 steps / deg

  if (abs(errorAngle) > 0.1) {

    int correctionSteps = round(abs(errorAngle) * 8.8888);
    int correctionSpeed = 10000 - correctionSteps;
    debugPrint(correctionSpeed);
    debugPrint(",");
    debugPrintln(correctionSteps);

    if (errorAngle > 0) { // run left
      driveMotor(correctionSteps, 100, 0, stepMode, 0);
    }
    else if (errorAngle < 0) { // run right
      driveMotor(correctionSteps, 100, 1, stepMode, 0);
    }
  }



  /*
    debugPrint(currAngle);
    debugPrint(",");
    debugPrintln(errorAngle);
    debugPrint(",");
    debugPrint(desiredAngle);
    debugPrintln("");
  */
}


float getEncoderAngle() {
  float normalAngle;
  float rawAngle = ams5600.getRawAngle();
  /* Raw data reports 0 - 4095 segments, which is 0.087 of a degree */
  normalAngle = rawAngle * 0.087;
  return normalAngle;
}


void disableMotor() {
  digitalWrite(enablePin, HIGH); // motor disabled
  motorEnabled = false;

}

void enableMotor() {
  if (!eStopActive) {
    digitalWrite(enablePin, LOW); // motor enabled
    motorEnabled = true;
  }
  else {
    disableMotor();
  }
}

void setStepMode(int mode) {
  // sets TMC2208 step modes: full step to 1/32 step mode
  switch (mode) {
    // MS2, MS1: 00: 1/8, 11: 1/16
    case 2:
      digitalWrite(m0Pin, HIGH);
      digitalWrite(m1Pin, LOW);
      //digitalWrite(m2Pin, LOW);
      break;
    case 4:
      digitalWrite(m0Pin, LOW);
      digitalWrite(m1Pin, HIGH);
      //digitalWrite(m2Pin, LOW);
      break;
    case 8:
      digitalWrite(m0Pin, LOW);
      digitalWrite(m1Pin, LOW);
      //digitalWrite(m2Pin, LOW);
      break;
    case 16:
      digitalWrite(m0Pin, HIGH);
      digitalWrite(m1Pin, HIGH);
      //digitalWrite(m2Pin, HIGH);
      break;
    default:
      digitalWrite(m0Pin, HIGH);
      digitalWrite(m1Pin, HIGH);
      //digitalWrite(m2Pin, HIGH);
      break;
  }
}


void driveMotor(int motorSteps, int motorSpeed, bool motorDirection, int motorStepMode, int hold) {
  drawDisplay();
  digitalWrite(ledPin, HIGH);
  if (!eStopActive) {
    enableMotor();
    setStepMode(motorStepMode);
    if (motorDirection == 1) {
      digitalWrite(dirPin, HIGH);
    }
    else if (motorDirection == 0) {
      digitalWrite(dirPin, LOW);
    }
    motorSpeed = constrain(motorSpeed, 0, 10000);
    for (int i = 0; i < motorSteps; i++) {
      digitalWrite(stepPin, LOW);
      delayMicroseconds(motorSpeed);
      digitalWrite(stepPin, HIGH);
      delayMicroseconds(motorSpeed);
    }
    if (hold == 0) {
      disableMotor();
    }
  }
  digitalWrite(ledPin, LOW);
}

void drawDisplay() {
  getButtonStates();
  encoderAngle = getEncoderAngle();
  voltage = getVoltage();

  if (voltage != voltageLast ) {
    oled.clearField(0, 1, 3);
    //if (voltage < 10.0) oled.print("");
    oled.print(voltage, 1);
  }

  if (encoderAngle != encoderAngleLast ) {
    oled.clearField(90, 1, 3);
    oled.print(encoderAngle, 1);
  }

  if (endStopPressed != endStopPressedLast) {
    oled.clearField(0, 2, 4);
    if (endStopPressed) oled.setInvertMode(1);
    oled.print("END");
  }

  if (eStopActive != eStopActiveLast) {
    oled.clearField(47, 2, 4);
    if (eStopActive) oled.setInvertMode(1);
    oled.print("STOP");
    oled.setInvertMode(0);
  }

  if (jobDone != jobDoneLast) {
    oled.clearField(96, 2, 4);
    if (!jobDone) oled.setInvertMode(1);
    oled.print("ACT");
    oled.setInvertMode(0);
  }

  voltageLast = voltage;
  encoderAngleLast = encoderAngle;
  endStopPressedLast = endStopPressed;
  eStopActiveLast = eStopActive;
  jobDoneLast = jobDone;
}

void getButtonStates() {
  endStopPressed = !digitalRead(endStopPin); // END Stop pressed
  eStopActive = !digitalRead(eStopPin);
}

float getVoltage() {
  float vin = (analogRead(A6) * 3.3) / 1024.0 / (1000.0 / (10000.0 + 1000.0)) ;
  return vin;
}

void setupDisplay() {
  oled.begin(&Adafruit128x32, I2C_ADDRESS);
  oled.setFont(System5x7);
  oled.set2X();
  oled.clear();
  oled.println("ethersweep");
  oled.set1X();
  oled.println(" ");
  oled.println("       v3.0.6");
  // rows = oled.fontRows();
}


void initializeDisplay() {

  oled.setFont(System5x7);
  oled.clear();
  oled.println("ethersweep    v3.0.6");
  oled.println("00.0V | DHCP | 000.0°");
  oled.println("END   | STOP |  ACT");
  oled.println("USB MODE");

  //oled.println("IP: " + displayAddress(Ethernet.localIP()));
  // rows = oled.fontRows();
}
