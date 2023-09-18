import socket

server_ip = "0.0.0.0"  # Listen on all available network interfaces
server_port = 5000 # ethersweep standard port for sensor data feedback

udp_socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

udp_socket.bind((server_ip, server_port))

print(f"Listening for UDP messages on port {server_port}...")

while True:
    data, addr = udp_socket.recvfrom(1024)

    decoded_data = data.decode('ascii', errors='ignore')  # Ignore non-ASCII characters
    print(decoded_data)
    print(data)

udp_socket.close()

