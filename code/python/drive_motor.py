import socket
import json

'''
If you want to run multiple ethersweeps in one network, 
you have to change the ip adress of each ethersweep.

You can enable #define STATIC 1 in the arduino code and set a static ip.
OR you can enable DHCP #define STATIC 0 and let your DHCP server handle the IP adresses.
In both situatiuons it is super important to give each ethersweep a unique MAC adress!!!
'''


UDP_SEND_IP_MOTOR = "192.168.1.169"  # ethersweep IP (depends on DHCP)
UDP_SEND_PORT = 1337

'''
steps: the steps the steppermotor should do
speed: time delay between individual steps in microseconds
direction: 1 or 0 -> depends on connection (just try it out)
stepmode: 1,2,4,8,16,32 -> fullstep, halfstep, quaterstep ...
motor_ip: ip of your ethersweep device. (depends on DHCP and MAC adress)
'''
def drive_motor(steps, speed, direction, stepmode, motor_ip):
    json_data = json.dumps({'steps': steps, 'speed': speed, 'direction': direction, 'stepmode': stepmode})
    message = json_data.encode()
    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    sock.sendto(message, (motor_ip, UDP_SEND_PORT))


drive_motor(2000, 9900, 1, 32, UDP_SEND_IP_MOTOR)
drive_motor(2000, 9900, 0, 32, UDP_SEND_IP_MOTOR)
