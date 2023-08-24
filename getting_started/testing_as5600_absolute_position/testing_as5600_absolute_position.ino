#include <Wire.h>
#include <AS5600.h>

HardwareSerial Serial1(PA10, PA9);

#define SERIAL Serial1
#define SYS_VOL   3.3

AMS_5600 ams5600;

double totalRotation = 0.0;
float previousAngle = 0.0;

void setup()
{
  SERIAL.begin(115200);
  Wire.begin();

  if (ams5600.detectMagnet() == 0) {
    while (1) {
      if (ams5600.detectMagnet() == 1) {
        SERIAL.print("Current Magnitude: ");
        SERIAL.println(ams5600.getMagnitude());
        break;
      } else {
        SERIAL.println("Can not detect magnet");
      }
      delay(1000);
    }
  }
}

float convertRawAngleToDegrees(word newAngle)
{
  float retVal = newAngle * 0.087890625;
  return retVal;
}

void loop()
{
  int rawAngle = ams5600.getRawAngle();
  float currentAngle = convertRawAngleToDegrees(rawAngle);

  // Calculate the change in angle from the previous reading
  float angleChange = currentAngle - previousAngle;

  // Check for angle transitions near 360° to 0° or vice versa
  if (angleChange > 180.0) {
    totalRotation -= 360.0; // Subtract 360° for clockwise transition
  } else if (angleChange < -180.0) {
    totalRotation += 360.0; // Add 360° for counterclockwise transition
  }

  // Update the total rotation based on the angle change
  totalRotation += angleChange;

  // Update the previous angle value
  previousAngle = currentAngle;

  // Print the updated absolute angle, which can be positive or negative
  SERIAL.println(String(totalRotation, DEC));
}
