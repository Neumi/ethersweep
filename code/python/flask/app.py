

from flask import Flask, render_template, request
import socket
import json

UDP_SEND_IP_MOTOR = "192.168.1.177"
UDP_SEND_PORT = 8888
app = Flask(__name__)


'''
steps: the steps the steppermotor should do
speed: time delay between individual steps in microseconds
direction: 1 or 0 -> depends on connection (just try it out)
stepmode: 2,4,8,16,32 -> halfstep, quarterstep ...
motor_ip: ip of your ethersweep device. (depends on DHCP and MAC adress)
'''
def drive_motor(drivemode, steps, speed, slope, direction, stepmode, hold, motor_ip):
    json_data = json.dumps(
        {'drivemode': drivemode, 'steps': steps, 'speed': speed, 'slope': slope, 'direction': direction, 'stepmode': stepmode,
         'hold': hold})
    message = json_data.encode()
    print(message)
    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    sock.sendto(message, (motor_ip, UDP_SEND_PORT))


@app.route('/')
def json_route():
    return render_template('json.html')


@app.route('/run_motor/drive', methods=['GET', 'POST'])
def run_motor_drive():
    if request.method == 'POST':
        # print(data.to_dict())
        drive_data = request.form.to_dict()

        try:
            drive_motor(
                int(drive_data['drivemode']),
                int(drive_data['steps']),
                int(drive_data['speed']),
                int(drive_data['slope']),
                int(drive_data['direction']),
                int(drive_data['stepmode']),
                int(drive_data['hold']),
                drive_data['ip_address'])
        except:
            return "failed to reach motor"

        del drive_data['ip_address']
        print(drive_data)
        return str(drive_data)



