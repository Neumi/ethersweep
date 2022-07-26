# Ethersweep
### An Open Source Ethernet controllable stepper motor. Easy interfacing with JSON and UDP from any device on the network in any language you choose.

<img alt="linear axis" src="/img/linear_axis.jpg" width="50%">


## What it does
Ethersweep offers easy interfacing from computers/machines/backends to motion control. Every UDP supporting language is able to communicate with it and is able to control as many motors (on your network) as you like. 

It gets an IP address from a DHCP server and shows data like IP, operating voltage, button and operation states on the OLED display.

<img alt="Display content" src="/img/display.png" width="50%">

<img alt="PCB Photo" src="/img/3d_side.png" width="50%">



## What it needs
Every motor has to be connected to an Ethernet network (optimally with DHCP) and a power supply between 5.5...30V.

## How to control




## Features:
- W5500 Ethernet controller
- AtMega328 Microcontroller (with Arduino Bootloader)
- Trinamic TMC2208 Stepper motor driver
- AS5600 magnetic rotary encoder
- XT30 power connector
- Oled 128x32 Display
- USB / ISP programmable
- Inputs for E-Stop and End-Stop
- Reset button
- Motor current setting via potentiometer
- Fits on a NEMA17 stepper motor


## How to get started:
Build an (or multiple) Ethersweep controller. All manufacturing/assembly files work with JLCPCB and only some easy THT soldering and programming when the boards arrive. 

Connect ethersweep to your network via ethernet and to a +12v power source.
Once connected it serarches for an IP address on the network using DHCP. When an IP is found, the controller shows it on the display and its ready to use.

Now you can use the Python script to control the motor.


![PCB raw B](/img/3d_bottom.png)
![PCB raw B](/img/3d_top.png)
![animation](/img/animation.gif)
![cad 2](/img/cad2.png)
![pcb layout](/img/pcb_layout.png)

## Using Python to control a stepper motor:

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
[![LINK TO VIDEO](https://img.youtube.com/vi/JtqH8TxggIc/0.jpg)](https://www.youtube.com/watch?v=JtqH8TxggIc)

