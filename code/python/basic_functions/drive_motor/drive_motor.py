import socket
import json

UDP_SEND_IP_MOTOR = "192.168.1.176"
UDP_SEND_PORT = 8888


message = json.dumps(
        {
            'mode': 0,
            'steps': 1000,
            'speed': 100,
            'dir': 1,
            'stepmode': 64,
            'hold': 0
        }
    ).encode()

sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

sock.sendto(message, (UDP_SEND_IP_MOTOR, UDP_SEND_PORT))
