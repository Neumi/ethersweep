# ethersweep
Ethernet + UDP + Json motor controller. Based on open source soft- and hardware.



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
