#include <TMCStepper.h> // TMCstepper - https://github.com/teemuatlut/TMCStepper
// FOR ETHERSWEEP Hardware v4.5.0 with STM32 controller and TMC2209
// running this code should spin the stepper motor on Ethersweep in different speeds and directions.
// the interface happens through serial2 and does not need STOP DIR pins
// this only works on Ethersweep v4.5.0

#define MS1_PIN  PB4
#define MS2_PIN  PB3
#define EN_PIN PB8
HardwareSerial Serial2(PB11, PB10);

#define DRIVER_ADDRESS 0x00 // TMC2209 driver address when MS1 LOW and MS2 LOW
#define R_SENSE 0.1f // TMC2209 with 100mOhm R shunt
bool dir = false;

// SoftwareSerial SoftSerial(PB11, PB10);
TMC2209Stepper TMCdriverX(&Serial2, R_SENSE, DRIVER_ADDRESS);

void setup() {
  Serial2.begin(115200);
  TMCdriverX.beginSerial(115200);

  pinMode(EN_PIN, OUTPUT);
  pinMode(MS2_PIN, OUTPUT);
  pinMode(MS1_PIN, OUTPUT);

  digitalWrite(EN_PIN, LOW);
  digitalWrite(MS1_PIN, LOW);
  digitalWrite(MS2_PIN, LOW);


  TMCdriverX.begin();
  TMCdriverX.toff(5);
  TMCdriverX.rms_current(500); // motor current in mA
  TMCdriverX.microsteps(64); // microsteps in 1/x
  TMCdriverX.en_spreadCycle(false);
  TMCdriverX.pwm_autoscale(true); // needed for stealthChop
  TMCdriverX.VACTUAL(12000); // motor speed
  TMCdriverX.step();
}

void loop() {
  delay(100);
  dir = !dir;
  TMCdriverX.shaft(dir);

  delay(500);
  TMCdriverX.VACTUAL(50000);

  delay(500);
  dir = !dir;
  TMCdriverX.shaft(dir);
  delay(500);

  TMCdriverX.VACTUAL(12000);
  delay(200);
  for (int i = 0; i <= 20; i++) {
    dir = !dir;
    TMCdriverX.shaft(dir);
    delay(50);
  }
}
