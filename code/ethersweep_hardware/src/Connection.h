#pragma once

class Connection
{
private:
    int mac[6] = {222, 0, 0, 0, 0, 0}; // first number is fixed in mac address space

    bool macUnwritten = true;
    String connectionMode = "none";
public:
    
    Connection(byte randomSeedPin);
    bool checkMacAddress();
    void generateNewMacEeprom();
    int *getMac();
    void clearMac();
    void setConnectionMode(String connectionMode);
};
