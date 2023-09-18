#include "Messenger.h"

Messenger::Messenger(HardwareSerial *serial)
{
    this->serialInterface = serial;
}

void Messenger::init(int baud)
{
    this->serialInterface->begin(baud);
}

void Messenger::sendError(String message)
{   
    this->serialInterface->println("\033[1;31m" + message + "\033[0m");
}

void Messenger::sendInfo(String message)
{
    this->serialInterface->println("\033[1;32m" + message + "\033[0m");
}

void Messenger::sendUDPMessage(IPAddress ip, int remotePort, String message, EthernetUDP udp)
{  
  // Send the UDP message to the specified IP and port
  udp.beginPacket(ip, remotePort);
  udp.print(message);
  udp.endPacket();
}
