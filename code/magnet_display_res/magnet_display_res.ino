#include <IPAddress.h>
#include <Adafruit_NeoPixel.h>
#include "SSD1306Ascii.h"
#include "SSD1306AsciiAvrI2c.h"
#include <Wire.h>
#include <AS5600.h>
#define SYS_VOL   3.3

AMS_5600 ams5600;

int ang = 0;
int lang = 0;

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


int estopPin = 4;
boolean eStopState = false;
int homePin = 3;
boolean homeState = false;

String lastIp;
boolean lastDhcp;
boolean lastStop;
boolean lastHome;

void setup() {
  oled.begin(&Adafruit128x32, I2C_ADDRESS);
  oled.setI2cClock(400000L); // 400khz for oled
  oled.displayRemap(true);
  oled.setFont(Adafruit5x7);

  oled.set2X();

  if (ams5600.detectMagnet() == 0 ) {
    while (1) {
      if (ams5600.detectMagnet() == 1 ) {

        drawDisplayMagnet("Mgntude: " + ams5600.getMagnitude());
        break;
      }
      else {
        drawDisplayMagnet("NO MAGNET");
      }
      delay(1000);
    }
  }





}

void loop() {
  eStopState = digitalRead(estopPin);
  homeState = digitalRead(homePin);

  drawDisplayState("192.168.131.123", true, eStopState, homeState);


  drawDisplayMagnet("Angle: " + String(convertRawAngleToDegrees(ams5600.getRawAngle()), DEC));



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

float convertRawAngleToDegrees(word newAngle)
{
  /* Raw data reports 0 - 4095 segments, which is 0.087 of a degree */
  float retVal = newAngle * 0.087;
  ang = retVal;
  return ang;
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
  oled.print(" ")
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

void drawDisplayMagnet(String message) {
  oled.setFont(Adafruit5x7);
  oled.clear();
  oled.set2X();
  oled.println("ethersweep");
  oled.set1X();
  
  oled.print("Msg: ");
  oled.println(message);

}
