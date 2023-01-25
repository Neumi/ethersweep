import socket
import json

UDP_SEND_IP_MOTOR = "192.168.1.177"
UDP_SEND_PORT = 8888


def drive_motor(drive_mode, steps, speed, slope, direction, step_mode, hold, motor_ip):
    message = json.dumps(
        {
            'drive_mode': drive_mode,
            'steps': steps,
            'speed': speed,
            'slope': slope,
            'direction': direction,
            'step_mode': step_mode,
            'hold': hold
        }
    ).encode()
    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    sock.sendto(message, (motor_ip, UDP_SEND_PORT))



