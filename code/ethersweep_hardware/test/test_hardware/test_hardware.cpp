#pragma once

#include <Arduino.h>
#include <unity.h>
#include <SPI.h>
#include <Wire.h>
#include <Ethernet.h>
#include <AS5600.h>
#include "SSD1306Ascii.h"
#include "SSD1306AsciiAvrI2c.h"
#include "Configuration.h"

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32

SSD1306AsciiAvrI2c oled;
AMS_5600 ams5600;

void test_status_led()
{
  pinMode(2, OUTPUT);
  digitalWrite(2, LOW);
  delay(100);
  digitalWrite(2, HIGH);
}

void test_display_interaction()
{
#define OLED_I2C_ADDRESS 0x3C
  oled.begin(&Adafruit128x32, OLED_I2C_ADDRESS);
  oled.setFont(Adafruit5x7);
  oled.println("set2wX");
  int returnValue = oled.println("display test");
  delay(100);
  oled.clear();
  TEST_ASSERT_EQUAL(14, returnValue);
}

void test_i2c_addresses()
{
  byte error, address;
  int nDevices;

  nDevices = 0;
  for (address = 1; address < 127; address++)
  {
    Wire.beginTransmission(address);
    error = Wire.endTransmission();

    if (error == 0)
    {
      // Serial.print("I2C device found at address 0x");
      // Serial.print(address, HEX);
      nDevices++;
    }
    else if (error == 4)
    {
      // Serial.print("Unknown error at address 0x");
      // Serial.println(address, HEX);
    }
  }

  TEST_ASSERT_EQUAL(2, nDevices);
}

void test_encoder_i2c()
{
  byte error, address;
  int result = 0;

#define ENCODER_ADDRESS 0x36

  for (address = 1; address < 127; address++)
  {
    Wire.beginTransmission(address);
    error = Wire.endTransmission();
    if (error == 0)
    {
      if (address == ENCODER_ADDRESS)
      {
        result = 1;
      }
    }
  }

  TEST_ASSERT_EQUAL(1, result);
}

void test_display_i2c()
{
  byte error, address;
  int result = 0;

#define DISPLAY_ADDRESS 0x3C

  for (address = 1; address < 127; address++)
  {
    Wire.beginTransmission(address);
    error = Wire.endTransmission();
    if (error == 0)
    {
      if (address == DISPLAY_ADDRESS)
      {
        result = 1;
      }
    }
  }

  TEST_ASSERT_EQUAL(1, result);
}

void test_encoder_presence()
{
  int result = ams5600.detectMagnet();

  TEST_ASSERT_EQUAL(1, result);
}

void test_encoder_magent_strengh()
{
  int result = ams5600.getMagnitude();

  TEST_ASSERT_GREATER_OR_EQUAL(1500, result);
  TEST_ASSERT_LESS_OR_EQUAL(2000, result);
}

void test_encoder_sanity()
{
  int angle = ams5600.getRawAngle();

  TEST_ASSERT_GREATER_OR_EQUAL(0, angle);
  TEST_ASSERT_LESS_OR_EQUAL(4096, angle);
}

void test_voltage_sanity()
{
  float voltage = (analogRead(A6) * 3.3) / 1024.0 / (1.0 / 11.0);

  TEST_ASSERT_GREATER_OR_EQUAL(5.5, voltage);
  TEST_ASSERT_LESS_OR_EQUAL(30.0, voltage);
}

void test_end_stop_pin()
{
  int pin = 3;
  pinMode(pin, INPUT);
  boolean endStopState = digitalRead(pin);

  TEST_ASSERT_EQUAL(1, endStopState);
}

void test_emergency_stop_pin()
{
  int pin = 4;
  pinMode(pin, INPUT);
  boolean emergencyStopState = digitalRead(pin);

  TEST_ASSERT_EQUAL(1, emergencyStopState);
}

void test_ethernet_hardware_check()
{
  byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};
  int timeout = 1000;
  int result = Ethernet.begin(mac, timeout);

  // 0 = no cable
  // 1 = functional hardware + connected cable
  // 2 == no hardware
  TEST_ASSERT_EQUAL(1, result);
}

void test_motor_driver_quater_turn_left()
{
  int enablePin = 5;
  int stepPin = 7;
  int dirPin = 6;
  int m0Pin = A1;
  int m1Pin = A0;
  int diagPin = A2;
  int faultPin = A3;

  pinMode(enablePin, OUTPUT);
  pinMode(stepPin, OUTPUT);
  pinMode(dirPin, OUTPUT);
  pinMode(m0Pin, OUTPUT);
  pinMode(m1Pin, OUTPUT);
  pinMode(diagPin, INPUT);
  pinMode(faultPin, INPUT);

  // set 16th stepmode
  digitalWrite(m0Pin, HIGH);
  digitalWrite(m1Pin, HIGH);

  digitalWrite(enablePin, LOW); // enables motor

  double startNormalizedAngle = (ams5600.getRawAngle() * 360.0) / 4096.0;

  for (int i = 0; i < 800; i++)
  {
    digitalWrite(stepPin, LOW);
    delayMicroseconds(100);
    digitalWrite(stepPin, HIGH);
    delayMicroseconds(100);
  }
  delay(100);

  double endNormalizedAngle = (ams5600.getRawAngle() * 360.0) / 4096.0;
  double angleOffset = 0.0;

  if (startNormalizedAngle < 100)
  {
    angleOffset = 90.0;
  }
  double angleDifference = abs((endNormalizedAngle - angleOffset) - (startNormalizedAngle + angleOffset));

  Serial.println(angleDifference);
  Serial.println(startNormalizedAngle);
  Serial.println(endNormalizedAngle);
  TEST_ASSERT_GREATER_OR_EQUAL(89.0, angleDifference);
  TEST_ASSERT_LESS_OR_EQUAL(91.0, angleDifference);
}

void setup()
{
  delay(200);

  pinMode(LED_BUILTIN, OUTPUT);

  UNITY_BEGIN();

  RUN_TEST(test_status_led);
  RUN_TEST(test_i2c_addresses);
  RUN_TEST(test_encoder_i2c);
  RUN_TEST(test_display_i2c);
  RUN_TEST(test_display_interaction);
  RUN_TEST(test_encoder_presence);
  RUN_TEST(test_encoder_magent_strengh);
  RUN_TEST(test_encoder_sanity);
  RUN_TEST(test_voltage_sanity);
  RUN_TEST(test_end_stop_pin);

  RUN_TEST(test_ethernet_hardware_check);

  RUN_TEST(test_motor_driver_quater_turn_left);

  UNITY_END();
}

void loop()
{
}
