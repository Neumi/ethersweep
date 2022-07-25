
import socket

server = socket.socket(socket.AF_INET, socket.SOCK_DGRAM, socket.IPPROTO_UDP)
server.setsockopt(socket.SOL_SOCKET, socket.SO_BROADCAST, 1)

server.bind(("", 8888))
message = b"{'drivemode': '0', 'speed': '240', 'slope': '100', 'steps': '1600', 'stepmode': '8', 'direction': '0', 'hold': '1'}"
server.sendto(message, ('<broadcast>', 8888))

server.close()
