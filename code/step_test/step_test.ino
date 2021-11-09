
int ang = 0;
int lang = 0;

int stepPin = 6;
int dirPin = 7;
int enablePin = 5;

int m0Pin = A0;
int m1Pin = A1;
int m2Pin = A2;
int faultPin = A3;
int estopPin = 4;
int homePin = 3;



void setup() {
  pinMode(m0Pin, OUTPUT);
  pinMode(m1Pin, OUTPUT);
  pinMode(m2Pin, OUTPUT);

  pinMode(stepPin, OUTPUT);
  pinMode(dirPin, OUTPUT);
  pinMode(enablePin, OUTPUT);

  digitalWrite(m0Pin, LOW);
  digitalWrite(m1Pin, LOW);
  digitalWrite(m2Pin, LOW);

  digitalWrite(stepPin, LOW);
  digitalWrite(dirPin, LOW);
  digitalWrite(enablePin, LOW);

}

void loop() {

  setStepMode(1);
  runMotor(200, 0);
  runMotor(200, 1);
  setStepMode(2);
  runMotor(200, 0);
  runMotor(200, 1);
  setStepMode(4);
  runMotor(200, 0);
  runMotor(200, 1);
  setStepMode(8);
  runMotor(200, 0);
  runMotor(200, 1);
  setStepMode(16);
  runMotor(200, 0);
  runMotor(200, 1);
  setStepMode(32);
  runMotor(200, 0);
  runMotor(200, 1);

  delay(1000);

}

void runMotor(int steps, int dir) {
  if (dir) {
    digitalWrite(dirPin, HIGH);
  }
  if (!dir) {
    digitalWrite(dirPin, LOW);
  }

  for (int i = 0; i <= steps; i++) {
    digitalWrite(stepPin, LOW);
    delay(2);
    digitalWrite(stepPin, HIGH);
    delay(2);
  }

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
