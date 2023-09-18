#pragma once
#include <Arduino.h>
#include <IPAddress.h>
#include <Ethernet.h>
#include <EthernetUdp.h>


class Messenger
{

private:
    HardwareSerial *serialInterface;

public:
    Messenger(HardwareSerial *serial);
    void init(int baud);
    void sendError(String message);
    void sendInfo(String message);
    void sendUDPMessage(IPAddress ip, int port, String message, EthernetUDP udp);

};
