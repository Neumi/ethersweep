#pragma once

const String version = "3.0.9"; // software version

#define STATIC_IP 0 // when set 1, DHCP is enabled, when 0, statis IP is used

#define DEBUG_MODE 1 // when set 1, debugging output is enabled

#define BAUD_SPEED 9600 // debug and message baud speed

#define SYS_VOL 3.3 // system voltage in volts

#define OLED_I2C_ADDRESS 0x3C // i2c address of oled display

#define BUFFER_SIZE 128 // communication buffer size

#define DISPLAY_REFRESH 100 // milliseconds until new data is shown

#define LOCAL_PORT 8888 // local UDP port

#define REMOTE_PORT 8889 // remote machines receiving UDP port

// drivemode definitions
#define STEPS 0      // runs certain ammounts of steps
#define HOME 1       // homes until endstop is reached
#define RAMP 2       // ramps motor with certain slope
#define POWERCYCLE 3 // power cycles motor controller


// W5500 ethernet chip
#define STEP_PIN 7
#define DIR_PIN 6
#define ENABLE_PIN 5
#define SLEEP_PIN 8
#define RESET_PIN 9

// TMC2208 motor controller chip
#define M0_PIN A1
#define M1_PIN A0
#define DIAG_PIN A2
#define FAULT_PIN A3

// sensor inputs
#define VOLT_DETECT_PIN A6
#define END_STOP_PIN 3
#define E_STOP_PIN 4
#define RANDOM_SEED_PIN A7

// outputs
#define LED_PIN 2


