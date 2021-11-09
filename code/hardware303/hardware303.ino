#include <Ethernet.h>
#include <EthernetUdp.h>
#include <IPAddress.h>
#include <ArduinoJson.h>
#include <EEPROM.h>
#include "SSD1306Ascii.h"
#include "SSD1306AsciiAvrI2c.h"
#include <Wire.h>
#include <AS5600.h>


#define SYS_VOL   3.3

AMS_5600 ams5600;


int stepPin = 7;
int dirPin = 6;
int enablePin = 5;
// int sleepPin = 8;
// int resetPin = 9;

int m0Pin = A1;
int m1Pin = A0;
int diagPin = A2;
int faultPin = A3;
int voltDetectPin = A6;

int motorSpeed = 0;
int motorSteps = 0;
int motorDirection = 0;
int motorStepMode = 0;


boolean motorEnabled = false;


boolean jobDone = true;
String ip_mode = "DHCP";

#define I2C_ADDRESS 0x3C
SSD1306AsciiAvrI2c oled;

/*
  #define STATIC 0

  #if STATIC
  String ip_mode = "STATIC";
  static byte myip[] = { 192, 168, 1, 110 };
  static byte gwip[] = { 192, 168, 1, 1 };
  #endif

*/



byte mac[] = {
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xEE
};
IPAddress ip(192, 168, 1, 178);

unsigned int localPort = 8888;      // local port to listen on

// buffers for receiving and sending data
char packetBuffer[64];  // buffer to hold incoming packet,

// An EthernetUDP instance to let us send and receive packets over UDP
EthernetUDP Udp;

String received_data = "0";

static uint32_t timer;
unsigned long previousMillis = 0;

void setup() {
  Serial.begin(9600);
  pinMode(stepPin, OUTPUT);
  pinMode(dirPin, OUTPUT);
  pinMode(enablePin, OUTPUT);
  // pinMode(sleepPin, OUTPUT);
  // pinMode(resetPin, OUTPUT);
  pinMode(m0Pin, OUTPUT);
  pinMode(m1Pin, OUTPUT);
  pinMode(diagPin, INPUT);
  digitalWrite(dirPin, LOW);

  oled.begin(&Adafruit128x32, I2C_ADDRESS);
  oled.setFont(Adafruit5x7);
  oled.clear();

  if (ams5600.detectMagnet() == 0 ) {
    while (1) {
      if (ams5600.detectMagnet() == 1 ) {
        break;
      }
      else {
        Serial.println("ERROR not magent detected!");
      }
      delay(1000);
    }
  }

  drawDisplay();


  


  // digitalWrite(sleepPin, HIGH);
  // digitalWrite(resetPin, HIGH);
  disableMotor();
  // setStepMode(2);

  Ethernet.begin(mac, ip);

  // Check for Ethernet hardware present
  if (Ethernet.hardwareStatus() == EthernetNoHardware) {
    Serial.println("Ethernet shield was not found.  Sorry, can't run without hardware. :(");
    while (true) {
      delay(1); // do nothing, no point running without Ethernet hardware
    }
  }
  if (Ethernet.linkStatus() == LinkOFF) {
    Serial.println("Ethernet cable is not connected.");
  }

  // start UDP
  // Serial.println(ip);
  drawDisplay();
  Udp.begin(localPort);
}

void loop() {
  // ether.packetLoop(ether.packetReceive());

  // char udpChars[received_data.length() + 1];
  // received_data.toCharArray(udpChars, received_data.length() + 1);
  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= 100) {
    drawDisplay();
    previousMillis = currentMillis;
  }
  
  int packetSize = Udp.parsePacket();
  if (packetSize) {
    // Serial.print("Received packet of size ");
    // Serial.println(packetSize);
    // Serial.print("From ");
    IPAddress remote = Udp.remoteIP();
    // Serial.print(", port ");
    // Serial.println(Udp.remotePort());

    // read the packet into packetBufffer
    Udp.read(packetBuffer, 64);
    // Serial.println("Contents:");
    // Serial.println(packetBuffer);

    // send a reply to the IP address and port that sent us the packet we received
    //Udp.beginPacket(Udp.remoteIP(), Udp.remotePort());
    //Udp.write(ReplyBuffer);
    //Udp.endPacket();

    jobDone = false;
  }



  if (jobDone == false) {
    DynamicJsonBuffer jsonBuffer;
    JsonObject& root = jsonBuffer.parseObject(packetBuffer);

    String raw_steps = root["steps"];
    raw_steps = root["steps"].as<String>();
    String raw_speed = root["speed"];
    raw_speed = root["speed"].as<String>();
    String raw_direction = root["direction"];
    raw_direction = root["direction"].as<String>();
    String raw_step_mode = root["stepmode"];
    raw_step_mode = root["stepmode"].as<String>();

    motorSteps = raw_steps.toInt();
    motorSpeed = raw_speed.toInt();
    motorDirection = raw_direction.toInt();
    motorStepMode = raw_step_mode.toInt();

    /*
      Serial.print("raw_data: ");
      Serial.println(received_data);
      Serial.print("steps: ");
      Serial.println(raw_steps);
      Serial.print("speed: ");
      Serial.println(raw_speed);
      Serial.print("dir: ");
      Serial.println(raw_direction);
      Serial.print("stepMode: ");
      Serial.println(raw_step_mode);
    */

    driveMotor(motorSteps, motorSpeed, motorDirection, motorStepMode);
    jobDone = true;
  }


}

void driveMotor(int motorSteps, int motorSpeed, bool motorDirection, int motorStepMode) {
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
  // disableMotor();
}


void setStepMode(int mode) {
  // sets DRV8825 step modes: full step to 1/32 step mode
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
 


void disableMotor() {
  digitalWrite(enablePin, HIGH); // motor disabled
  motorEnabled = false;

}

void enableMotor() {
  digitalWrite(enablePin, LOW); // motor enabled
  motorEnabled = true;

}

boolean checkMotorDriverFailure() {
  return digitalRead(diagPin);
}

void drawDisplay() {
  oled.clear();
  oled.set1X();
  oled.println("ethersweep");
  oled.print("Mode: ");
  oled.println(ip_mode);
  oled.print(getVoltage());
  oled.print("V | ");
  oled.println(String(round(convertRawAngleToDegrees(ams5600.getRawAngle())),DEC) + "°");
  oled.print("IP: ");
  oled.print(displayAddress(ip));
}

float getVoltage() {
  float vin = (analogRead(A6) * 3.3) / 1024.0 / (1000.0 / (10000.0 + 1000.0));
  return vin;
}

String displayAddress(IPAddress address)
{
  return String(address[0]) + "." +
         String(address[1]) + "." +
         String(address[2]) + "." +
         String(address[3]);
}

float convertRawAngleToDegrees(word newAngle)
{
  /* Raw data reports 0 - 4095 segments, which is 0.087 of a degree */
  float retVal = newAngle * 0.087;
  float ang = retVal;
  return ang;
}
