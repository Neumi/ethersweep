#pragma once

const String version = "3.5.2"; // software version

#define STATIC_IP 0 // when set 1, DHCP is enabled, when 0, statis IP is used

#define DEBUG_MODE 1 // when set 1, debugging output is enabled

#define BAUD_SPEED 9600 // debug and message baud speed

#define SYS_VOL 3.3 // system voltage in volts

#define OLED_I2C_ADDRESS 0x3C // i2c address of oled display

#define BUFFER_SIZE 256 // communication buffer size

#define LOCAL_PORT 8888 // local UDP port

#define STANDARD_REMOTE_SENSOR_PORT 5000 // remote machines receiving UDP port

#define DEFAULT_DISPLAY_REFRESH_TIME 100 // milliseconds until new data is shown
#define DEFAULT_FEEDBACK_TIME 300 // milliseconds until hearbeat


// action definitions
#define STEPS 0      // runs certain ammounts of steps
#define HOME 1       // homes until endstop is reached
#define RAMP 2       // ramps motor with certain slope
#define POWERCYCLE 3 // power cycles motor controller
#define POSITION 4   // encoder feedback position loop
#define SENSORFEEDBACK 5 // sends sensor values to IP and PORT
#define HEARTBEAT 6 // sends heartbeat answer values to IP and PORT
#define IDENTIFY 7 // sends IDENTIFY answer values to IP and PORT
#define CONFIGURE 8 // receives and saves configuration data

// scheduler modes
#define standby 0 // waiting for instructions
#define drive 1 // running drive loop
#define feedback 2 // feedback and hearbeat to host

// W5500 ethernet chip 
#define ETHERNET_INTER_PIN PB0 // ethernet int
#define RESET_PIN PA1 // ethernet reset
#define CS_PIN PB9 // ethernet chip select

// TMC2209 motor controller chip
#define ENABLE_PIN PB8
#define STEP_PIN PB15
#define DIR_PIN PA8
#define M0_PIN PB4 // MS1
#define M1_PIN PB3 // MS2
#define DIAG_PIN PA15
#define FAULT_PIN PA14

// sensor inputs
#define VOLT_DETECT_PIN PB1
#define END_STOP_PIN PA3
#define E_STOP_PIN PA4
#define RANDOM_SEED_PIN PA2

// outputs
#define LED_PIN PC13

// connection modes
#define CONNECTION_STAT "STAT"
#define CONNECTION_DHCP "DHCP"
#define CONNECTION_USB "USB "
 
