#include "Configurator.h"
#include "Configuration.h"
#include <Arduino.h>


Configurator::Configurator() {
    configurationStartAddress = 10;

    // bools and bytes need one address space, ints need two
    isConfigured = false;
    displayRefreshTimeAddress = configurationStartAddress + 1;  // Address for displayRefreshTime
    feedbackTimeAddress = configurationStartAddress + 3;        // Address for feedbackTime

    // Initialize default values
    displayRefreshTime = 0;
    feedbackTime = 0;

}

void Configurator::processNewConfiguration() {
    
}

void Configurator::readData() {
    isConfigured = EEPROM.read(configurationStartAddress) == 1;
    displayRefreshTime = EEPROM.read(displayRefreshTimeAddress) | (EEPROM.read(displayRefreshTimeAddress + 1) << 8);
    feedbackTime = EEPROM.read(feedbackTimeAddress) | (EEPROM.read(feedbackTimeAddress + 1) << 8);

}


void Configurator::loadData() {
    readData();

    if(isConfigured) {
        // Read displayRefreshTime from EEPROM
        displayRefreshTime = EEPROM.read(displayRefreshTimeAddress) | (EEPROM.read(displayRefreshTimeAddress + 1) << 8);

        // Read feedbackTime from EEPROM
        feedbackTime = EEPROM.read(feedbackTimeAddress) | (EEPROM.read(feedbackTimeAddress + 1) << 8);
    }
    else {
        factoryConfiguration();
    }

}

void Configurator::saveData() {
    // Write isConfigured to EEPROM
    EEPROM.write(configurationStartAddress, isConfigured ? 1 : 0);
    
    // Write displayRefreshTime to EEPROM
    EEPROM.write(displayRefreshTimeAddress, displayRefreshTime & 0xFF);
    EEPROM.write(displayRefreshTimeAddress + 1, (displayRefreshTime >> 8) & 0xFF);

    // Write feedbackTime to EEPROM
    EEPROM.write(feedbackTimeAddress, feedbackTime & 0xFF);
    EEPROM.write(feedbackTimeAddress + 1, (feedbackTime >> 8) & 0xFF);

}

void Configurator::factoryConfiguration() {
    // Set default values and mark as configured
    isConfigured = true;
    displayRefreshTime = DEFAULT_DISPLAY_REFRESH_TIME;  // Updated variable name
    feedbackTime = DEFAULT_FEEDBACK_TIME;

    saveData();
}

// for testing only
void Configurator::getFullEEPROM() {
    // Loop through the flash memory and print its contents
    for (int address = 0; address < 16; address++) {
        byte value = EEPROM.read(address);
        Serial1.println("Address: ");
        Serial1.println(address);
        Serial1.println(" Value: ");
        Serial1.println(value);
    }
}

// Updated method names
int Configurator::getDisplayRefreshTime() {
    return displayRefreshTime;
}

void Configurator::setDisplayRefreshTime(int value) {
    displayRefreshTime = value;
}

int Configurator::getFeedbackTime() {
    return feedbackTime;
}

void Configurator::setFeedbackTime(int value) {
    feedbackTime = value;
}
