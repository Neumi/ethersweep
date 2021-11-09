#include <IPAddress.h>
#include <Adafruit_NeoPixel.h>
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


static byte myip[] = { 192, 168, 1, 110 };

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(1, 4, NEO_GRB + NEO_KHZ800);

int stopPin = 8;
boolean stopState = false;

String lastIp;
boolean lastDhcp;
boolean lastStop;
boolean lastHome;

void setup() {
  Serial.begin(57600);
  oled.begin(&Adafruit128x32, I2C_ADDRESS);
  oled.displayRemap(true);
  oled.setFont(Adafruit5x7);

  oled.set2X();
}

void loop() {
  stopState = digitalRead(stopPin);

  drawDisplayState("192.168.131.123", true, stopState, true);
  /*
    drawDisplayState("192.168.131.123", false, true, true);
    delay(1000);

    drawDisplayState("192.168.131.123", true, true, true);
    delay(1000);

    drawDisplayState("192.168.131.123", true, false, true);
    delay(1000);

    drawDisplayState("192.168.131.123", false, false, false);
    delay(1000);
  */
}


void drawDisplayIp(String ip) {
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

void drawDisplayState(String ip, boolean dhcp, boolean stop, boolean home) {
  if (lastIp == ip) {
    if (lastDhcp == dhcp) {
      if (lastStop == stop) {
        if (lastHome == home) {
          return;
        }
      }
    }
  }

  oled.setFont(Adafruit5x7);
  oled.set2X();
  oled.clear();
  oled.println("ethersweep");
  oled.set1X();
  oled.print("Mode: ");
  if (dhcp == true) {
    oled.print("DHCP ");
  } else {
    oled.print("STAT ");
  }
  if (stop == true) {
    oled.setInvertMode(true);
  }
  oled.print("STOP");
  oled.setInvertMode(false);
  oled.print(" ");
  if (home == true) {
    oled.setInvertMode(true);
  }
  oled.print("HOME");
  oled.setInvertMode(false);
  

  oled.println();
  oled.print("IP: ");
  oled.print(ip);

  lastIp = ip;
  lastDhcp = dhcp;
  lastStop = stop;
  lastHome = home;
}
