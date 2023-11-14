#pragma once
#include <EEPROM.h>

class Configurator {
private:
    int configurationStartAddress;

    bool isConfigured;
    int displayRefreshTimeAddress;
    int feedbackTimeAddress;

    int displayRefreshTime;

    int feedbackTime;
    
    void readData();
    void saveData();
    void getFullEEPROM();
    void factoryConfiguration();

    void setDisplayRefreshTime(int value);
    void setFeedbackTime(int value);
    
public:
    Configurator();

    void loadData();
    
    void processNewConfiguration();
    
    int getDisplayRefreshTime();
    int getFeedbackTime();
    
};
