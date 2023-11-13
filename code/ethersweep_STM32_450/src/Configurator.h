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

public:
    Configurator();
    void readData();
    void loadData();
    void saveData();
    void factoryConfiguration();
    void getFullEEPROM();

    int getDisplayRefreshTime();
    void setDisplayRefreshTime(int value);

    int getFeedbackTime();
    void setFeedbackTime(int value);
};
