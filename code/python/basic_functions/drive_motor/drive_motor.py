import socket
import json

UDP_SEND_IP_MOTOR = "192.168.1.176" # change to Etehrsweep IP
UDP_SEND_PORT = 8888


message = json.dumps(
        {
            'mode': 0, # drivemode 0 = drive steps
            'steps': 1000, # steps
            'speed': 100, # delay in microseconds between steps
            'dir': 1, # direction 0 or 1
            'stepmode': 64, # microsepping divider
            'hold': 0 # hold 0 or 1
        }
    ).encode()

sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

sock.sendto(message, (UDP_SEND_IP_MOTOR, UDP_SEND_PORT))
