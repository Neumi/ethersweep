#include <EtherCard.h>
#include <IPAddress.h>
#include <ArduinoJson.h>
#include <Adafruit_NeoPixel.h>
#include <EEPROM.h>
#include "SSD1306Ascii.h"
#include "SSD1306AsciiAvrI2c.h"


int stepPin = 6;
int dirPin = 5;
int enablePin = 7;
int sleepPin = 8;
int resetPin = 9;

int m0Pin = A3;
int m1Pin = A2;
int m2Pin = A1;
int faultPin = A0;
int voltDetectPin = A7;

int motorSpeed = 0;
int motorSteps = 0;
int motorDirection = 0;
int motorStepMode = 0;

boolean jobDone = true;
String ip_mode = "DHCP";

#define I2C_ADDRESS 0x3C
SSD1306AsciiAvrI2c oled;

#define STATIC 0

#if STATIC
String ip_mode = "STATIC";
static byte myip[] = { 192, 168, 1, 110 };
static byte gwip[] = { 192, 168, 1, 1 };
#endif

// ethersweeps mac address has to be unique on local network
static byte mymac[] = { 0x70, 0x69, 0x69, 0x2D, 0x30, 0x21 };

byte Ethernet::buffer[500];
String received_data = "0";

const char website[] PROGMEM = "192.168.1.101"; // client ip adress (not this device)
const int dstPort PROGMEM = 1234;

const int srcPort PROGMEM = 4321;
static uint32_t timer;

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(1, 4, NEO_GRB + NEO_KHZ800);

void udpSerialPrint(uint16_t dest_port, uint8_t src_ip[IP_LEN], uint16_t src_port, const char *data, uint16_t len) {
  drawState("note");

  IPAddress src(src_ip[0], src_ip[1], src_ip[2], src_ip[3]);

  // Serial.print("dest_port: ");
  // Serial.println(dest_port);
  // Serial.print("src_port: ");
  // Serial.println(src_port);
  // Serial.print("src_port: ");
  ether.printIp(src_ip);
  Serial.println("data: ");
  Serial.println(data);

  received_data = data;
  char udpChars[received_data.length() + 1];
  received_data.toCharArray(udpChars, received_data.length() + 1);
  ether.sendUdp(udpChars, sizeof(udpChars) - 1, srcPort, ether.hisip, dstPort );
  jobDone = false;
}

void setup() {
  Serial.begin(57600);
  pixels.begin();
  drawState("init");

  Serial.println(F("\n[backSoon]"));
  drawDisplay("unknown");

  pinMode(stepPin, OUTPUT);
  pinMode(dirPin, OUTPUT);
  pinMode(enablePin, OUTPUT);
  pinMode(sleepPin, OUTPUT);
  pinMode(resetPin, OUTPUT);
  pinMode(m0Pin, OUTPUT);
  pinMode(m1Pin, OUTPUT);
  pinMode(m2Pin, OUTPUT);

  digitalWrite(dirPin, LOW);
  digitalWrite(enablePin, LOW);
  digitalWrite(sleepPin, HIGH);
  digitalWrite(resetPin, HIGH);

  setStepMode(32);

  if (ether.begin(sizeof Ethernet::buffer, mymac) == 0)
    Serial.println(F("Failed to access Ethernet controller"));
#if STATIC
  ether.staticSetup(myip, gwip);
#else
  if (!ether.dhcpSetup())
    Serial.println(F("DHCP failed"));
#endif

  drawDisplay(String(ether.myip[0]) + "." + String(ether.myip[1]) + "." + String(ether.myip[2]) + "." + String(ether.myip[3]));
  ether.printIp("IP:  ", ether.myip);
  ether.printIp("GW:  ", ether.gwip);
  ether.printIp("DNS: ", ether.dnsip);
  if (!ether.dnsLookup(website))
    Serial.println("DNS failed");

  ether.printIp("SRV: ", ether.hisip);
  ether.udpServerListenOnPort(&udpSerialPrint, 1337);
  ether.udpServerListenOnPort(&udpSerialPrint, 42);
  drawState("ok");
}

void loop() {
  ether.packetLoop(ether.packetReceive());

  char udpChars[received_data.length() + 1];
  received_data.toCharArray(udpChars, received_data.length() + 1);

  if (jobDone == false) {
    DynamicJsonBuffer jsonBuffer;
    JsonObject& root = jsonBuffer.parseObject(received_data);

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

    driveMotor(motorSteps, motorSpeed, motorDirection, motorStepMode);
    jobDone = true;
  }


}

void driveMotor(int motorSteps, int motorSpeed, bool motorDirection, int motorStepMode) {
  drawState("warning");
  setStepMode(motorStepMode);
  if (motorDirection == 1) {
    digitalWrite(dirPin, HIGH);
  }
  else if (motorDirection == 0) {
    digitalWrite(dirPin, LOW);
  }
  motorSpeed = constrain(motorSpeed, 0, 10000);
  for (int i = 0; i <= motorSteps; i++) {
    digitalWrite(stepPin, LOW);
    delayMicroseconds(motorSpeed);
    digitalWrite(stepPin, HIGH);
    delayMicroseconds(motorSpeed);
  }
  drawState("ok");
}


void setStepMode(int mode) {
  // sets DRV8825 step modes: full step to 1/32 step mode
  switch (mode) {
    case 1:
      digitalWrite(m0Pin, LOW);
      digitalWrite(m1Pin, LOW);
      digitalWrite(m2Pin, LOW);
      break;
    case 2:
      digitalWrite(m0Pin, HIGH);
      digitalWrite(m1Pin, LOW);
      digitalWrite(m2Pin, LOW);
      break;
    case 4:
      digitalWrite(m0Pin, LOW);
      digitalWrite(m1Pin, HIGH);
      digitalWrite(m2Pin, LOW);
      break;
    case 8:
      digitalWrite(m0Pin, HIGH);
      digitalWrite(m1Pin, HIGH);
      digitalWrite(m2Pin, LOW);
      break;
    case 16:
      digitalWrite(m0Pin, LOW);
      digitalWrite(m1Pin, LOW);
      digitalWrite(m2Pin, HIGH);
      break;
    case 32:
      digitalWrite(m0Pin, HIGH);
      digitalWrite(m1Pin, HIGH);
      digitalWrite(m2Pin, HIGH);
      break;
  }
}

void drawState(String state) {
  if (state == "init") {
    pixels.setPixelColor(0, pixels.Color(0, 0, 120));
  }
  if (state == "danger") {
    pixels.setPixelColor(0, pixels.Color(120, 0, 0));
  }
  if (state == "ok") {
    pixels.setPixelColor(0, pixels.Color(0, 120, 0));
  }
  if (state == "warning") {
    pixels.setPixelColor(0, pixels.Color(120, 80, 0));
  }
  if (state == "note") {
    pixels.setPixelColor(0, pixels.Color(255, 255, 255));
  }
  pixels.show();
}

void drawDisplay(String ip) {
  oled.begin(&Adafruit128x32, I2C_ADDRESS);
  oled.setFont(Adafruit5x7);
  oled.clear();
  oled.set2X();
  oled.println("ethersweep");
  oled.set1X();
  oled.print("Mode: ");
  oled.println(ip_mode);
  oled.print("IP: ");
  oled.print(ip);
}
