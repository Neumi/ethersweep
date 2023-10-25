import socket
import json

UDP_SEND_IP_MOTOR = "192.168.1.207" # change to Etehrsweep IP
UDP_SEND_PORT = 8888


message = json.dumps(
        {
            'mode': 7,
            # 'ip': '192.168.1.232', # feedback IP (optional)
            # 'port': 5000, # feedback port (optional)
        }
    ).encode()

sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

sock.sendto(message, (UDP_SEND_IP_MOTOR, UDP_SEND_PORT))
