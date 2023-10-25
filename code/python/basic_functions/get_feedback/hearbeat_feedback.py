import asyncio
import socket
import json
import os

UDP_SEND_IP_MOTOR = "192.168.1.176"
UDP_SEND_PORT = 8888

server_ip = "0.0.0.0"  # Replace with the appropriate IP address
server_port = 5000  # Replace with the appropriate port

async def task1():
    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

    while True:
        try:
            message = json.dumps({'mode': 5}).encode()
            sock.sendto(message, (UDP_SEND_IP_MOTOR, UDP_SEND_PORT))
            await asyncio.sleep(0.01)
        except:
            print("something is irregular")


async def task2():
    udp_socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    udp_socket.bind((server_ip, server_port))
    print(f"Listening for UDP messages on port {server_port}...")

    while True:
        loop = asyncio.get_event_loop()
        data, addr = await loop.run_in_executor(None, udp_socket.recvfrom, 1024)
        decoded_data = data.decode('ascii', errors='ignore')
        print(decoded_data)

async def main():
    task1_coroutine = task1()
    task2_coroutine = task2()

    await asyncio.gather(task1_coroutine, task2_coroutine)

if __name__ == "__main__":
    asyncio.run(main())
