#pragma once

class Connection
{
private:
    bool macUnwritten = true;

    bool checkMacAddress();
    void generateNewMacEeprom();
    void clearMac();

public:
    String connectionMode = "none";
    byte mac[6] = {222, 0, 0, 0, 0, 0}; // first number is fixed in mac address space

    Connection(byte randomSeedPin);

    byte *getMac();
    void setConnectionMode(String connectionMode);
};
