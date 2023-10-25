import asyncio
import socket
import json

UDP_SEND_PORT = 8888

server_ip = "0.0.0.0"  # Replace with the appropriate IP address
server_port = 5000  # Replace with the appropriate port


async def task1():
    server = socket.socket(socket.AF_INET, socket.SOCK_DGRAM, socket.IPPROTO_UDP)
    server.setsockopt(socket.SOL_SOCKET, socket.SO_BROADCAST, 1)

    server.bind(("", UDP_SEND_PORT))
    message = json.dumps({'mode': 5}).encode()
    server.sendto(message, ('<broadcast>', UDP_SEND_PORT))
    server.close()


async def task2():
    udp_socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    udp_socket.bind((server_ip, server_port))
    print(f"Searching for Ethersweep controllers...")

    while True:
        loop = asyncio.get_event_loop()
        data, addr = await loop.run_in_executor(None, udp_socket.recvfrom, 256)
        decoded_data = data.decode('ascii', errors='ignore')

        if decoded_data != "":
            version = json.loads(decoded_data).get("version")
            print("Ethersweep controller: version " + version + " address: " + addr[0])


async def main():
    task1_coroutine = task1()
    task2_coroutine = task2()

    await asyncio.gather(task1_coroutine, task2_coroutine)


if __name__ == "__main__":
    asyncio.run(main())
