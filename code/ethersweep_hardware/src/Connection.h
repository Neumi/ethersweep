#pragma once

class Connection
{
private:
    int mac[6] = {222, 0, 0, 0, 0, 0}; // first number is fixed in mac address space

    bool macUnwritten = true;

    bool checkMacAddress();
    void generateNewMacEeprom();
    void clearMac();

public:
    String connectionMode = "none";
    
    Connection(byte randomSeedPin);

    int *getMac();
    void setConnectionMode(String connectionMode);
};
