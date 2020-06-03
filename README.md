# ethersweep
Ethernet + UDP + Json motor controller. Based on open source soft- and hardware.


## How to get started:
Connect ethersweep to your network via ethernet and to a 12v power source (Stepper motor driver works from 8-32v). 
Wait for the LED to turn from blue to green. Blue indicates that ethersweep is searching for a DHCP server. Green means everything is normal and you're ready to go!

Then open your routers (or DHCP servers) admin page and find out the IP adress of ethersweep (will change when a display is installed). 

Change the IP in the python script and run it!


## ALL YOU NEED TO DRIVE A STEPPER MOTOR WITH PYTHON:

```python
import socket
import json

UDP_SEND_IP_MOTOR = "192.168.1.185"  # ethersweep IP (depends on DHCP)
UDP_SEND_PORT = 1337


def drive_motor(steps, speed, direction, stepmode, motor_ip):
    json_data = json.dumps({'steps': steps, 'speed': speed, 'direction': direction, 'stepmode': stepmode})
    message = json_data.encode()
    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    sock.sendto(message, (motor_ip, UDP_SEND_PORT))


drive_motor(100, 9500, 1, 32, UDP_SEND_IP_MOTOR)
```
