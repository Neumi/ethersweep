![Ethersweep Logo](/img/logo.png)

# Ethersweep
### An Open Source Ethernet controllable stepper motor. Easy interfacing with JSON and UDP from any device on the network in any language you choose.

Want to stay updated or participate? Join my [Discord](https://discord.com/invite/rerCyqAcrw)!

*As there is quite some interest in buying a controller, please fill out this form to help me better understand what Ethersweep would be used for https://forms.gle/FLgfvCP1hjCCp2636*

More media/press material:
https://drive.google.com/drive/folders/1qrk8QeEkzJ8hkr-H0x3y7xxhLTKhMb_F?usp=share_link

    

> *NEWS NOTE:*  Ethersweep is a project still in development. If you plan to order boards for yourself, you can, but expect it to take some work to get everything running! If you have any questions, please write me on Discord: https://discord.com/invite/rerCyqAcrw or
LinkedIn: https://www.linkedin.com/in/neumi/
If you need a custom assembly code or have a special use case in mind, contact me.
The latest running version is in the [production](electronic_design/production) directory. It is now an STM32f103 version with TMC2209 stepper driver. The assembly files contain LCSC part numbers and can be directly ordered at JLCPCB including assembly. Only some THT parts and SMD AS5600 encoder must be soldered by hand.


<img alt="linear axis" src="/img/linear_axis.png">


## What it does
Ethersweep is a motor controller for NEMA17 stepper motors that can be controlled from normal Ethernet networks. This means that you will be able to control motion from normal computers (laptop/server/raspberrypi), mobile devices, microcontrollers, PLCs and any other machine that is capable of sending UDP messages. On top you can control as many motors as you like because every motor gets an IP address from the DHCP server on your network.
You can also connect End and E-Stops to the controller. Sensor data like encoder feedback, voltage, running and button states can also be requested via network requests.

All you need is a network and power between 5v and 30v.
<img alt="PCB Photo" src="/img/pcb_top_v4.png">



## What it needs
Every motor has to be connected to an Ethernet network (optimally with DHCP) and a power supply between 5...30V. USB can be used for programming the controller with new firmware. End and E-Stops can be connected with JST3-pin connectors. To send instructions to Ethersweep you just send a JSON-encoded message via UDP to the controller's IP and port.


## Features:
- W5500 Ethernet controller
- STM32f103 Microcontroller @ 72MHz
- Trinamic TMC2209 Stepper motor driver
- AS5600 magnetic rotary encoder
- XT30 power connector
- Oled 128x32 Display
- USB programmable
- Inputs for E-Stop and End-Stop
- Reset button
- Fits on a NEMA17 stepper motor
- standard RJ45 network connector

<img alt="Display content" src="/img/display.png" width="100%">


## How to get started:
Build an (or multiple) Ethersweep controller. All manufacturing/assembly files work with JLCPCB and only some easy THT soldering is needed when the boards arrive. The code is available in the repository.

Connect your Ethersweep controller to your network via ethernet and connect to a 5.5...30V power source. Stick an encoder magnet (radial magnetized) on a NEMA17 motor shaft and connect the controller to the four M3 screws of the motor.
Once connected to power and Ethernet, it searches for an IP address using DHCP. When an IP is found, the controller shows it on the display and it's ready to use. When no IP is found, it defaults to 192.168.1.111 if available. (It is recommended to use DHCP)

Now you can use the Python script to control the motor.

![PCB raw B](/img/ETHERSWEEP4.png)
![cad rotation](/img/rotation/animation.gif)
![pcb layout](/img/component_overview.png)

## Using Python to control a stepper motor:

```python
import socket
import json

IP_MOTOR = "192.168.1.185" # ethersweep IP (change to motors IP)
UDP_PORT = 8888 # ethersweep port


def drive_motor(steps, speed, direction, mode, motor_ip):
    json_data = json.dumps({'steps': steps, 'speed': speed, 'direction': direction, 'mode': mode})
    message = json_data.encode()
    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    sock.sendto(message, (motor_ip, UDP_PORT))


drive_motor(100, 9500, 1, 32, IP_MOTOR) # this will spin the motor 100 steps
```

## Video:
[![LINK TO VIDEO](https://img.youtube.com/vi/80j7L1H61Hw/0.jpg)](https://www.youtube.com/watch?v=80j7L1H61Hw)

Follow the project on hackaday.io: https://hackaday.io/project/187187-ethersweep

## Upcoming Features:
- [ ] improved Broadcast mode + motor synchronization for multiple motors
- [ ] motor servo mode
- [ ] TMC2209 software interface
- [ ] scheduler

## Big thanks to the sponsors of this project!
![sponsor_banner](/img/sponsoring_banner.png)
