#pragma once

const String version = "3.0.9"; // software version

#define STATIC_IP 0 // when set 1, DHCP is enabled, when 0, statis IP is used

#define DEBUG_MODE 1 // when set 1, debugging output is enabled

#define BAUD_SPEED 9600 // debug and message baud speed

#define SYS_VOL 3.3 // system voltage in volts

#define OLED_I2C_ADDRESS 0x3C // i2c address of oled display

#define BUFFER_SIZE 128 // communication buffer size

// W5500 ethernet chip
const byte stepPin = 7;
const byte dirPin = 6;
const byte enablePin = 5;
// const  int sleepPin = 8; // ethernet INT
// const  int resetPin = 9; // ethernet RST

// TMC2208 motor controller chip
const byte m0Pin = A1;
const byte m1Pin = A0;
const byte diagPin = A2;
const byte faultPin = A3;

// inputs
const byte voltDetectPin = A6;
const byte endStopPin = 3;
const byte eStopPin = 4;
const byte randomSeedPin = A7;

// outputs
const byte ledPin = 2;

// drivemode definitions
#define STEPS 0 // runs certain ammounts of steps
#define HOME 1 // homes until endstop is reached
#define RAMP 2 // ramps motor with certain slope
#define POWERCYCLE 3 // power cycles motor controller
