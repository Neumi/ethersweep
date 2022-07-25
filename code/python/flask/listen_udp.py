import socket

UDP_IP = "192.168.1.155"
UDP_PORT = 8889

sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

sock.bind((UDP_IP, UDP_PORT))

while True:
    data, addr = sock.recvfrom(128)
    print(addr)
    print(data)
