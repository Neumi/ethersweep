# ethersweep3

The third version of ethersweep. Now with W5500 ethernet controller, trinamic TMC2208 stepper motor driver, improved high frequency circuitry, XT30 connector and many more improvements.

Ethernet + UDP + Json motor controller. Based on open source soft- and hardware.
Version3 has an built in AS5600 magnetic encoder for position feedback, the option for an END-stop for homing and an E-stop -just in case.
The controller uses 8.5V to 32V (?) from a standard XT30 connector. Ethernet connectivity via W5500 chip, stepper motor driver TMC2208 and ATmega328.

All built in a form factor that fits on the backside of a NEMA 17 stepper motor.


## What it does
Ethersweep is a simple to use ethernet connected stepper motor driver. It lets you use JSON formatted string to controll stpper motors via Ethernet. 

## How to get started:
Connect ethersweep to your network via ethernet and to a +12v power source.
Once connected it serarches for an IP address on the network using DHCP. When an IP is found, the controller shows it on the display and its ready to use.

Now you can use the Python script to control the motor.

(images to be updated)

![PCB Photo](/img/animation.gif)
![PCB raw B](/img/layout303.png)
![PCB raw B](/img/prototype201.jpg)
![PCB raw B](/img/side303.png)
![PCB raw B](/img/top303.png)
![PCB raw B](/img/back303.png)

## Using Python to controll a stepper motor:

```python
import socket
import json

IP_MOTOR = "192.168.1.185" #local motor controller IP
UDP_PORT = 1337


def drive_motor(steps, speed, direction, stepmode, motor_ip):
    json_data = json.dumps({'steps': steps, 'speed': speed, 'direction': direction, 'stepmode': stepmode})
    message = json_data.encode()
    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    sock.sendto(message, (motor_ip, UDP_PORT))


drive_motor(100, 9500, 1, 32, IP_MOTOR)
```

## Video:
[![LINK TO VIDEO](https://img.youtube.com/vi/CZqzoTy67dk/0.jpg)](https://www.youtube.com/watch?v=CZqzoTy67dk)

