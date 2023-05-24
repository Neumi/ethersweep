# Ethersweep with TMC2209

This versions goal was to fix some issues I had with the Atmega328 / TMC2208 version.
I changed:
  - STM32f103 at 72Mhz instead of ATMega328 at 8Mhz
  - native USB support instead of CP2102 serial/USB converter
  - USB C connector instead of Micro USB
  - TMC2209 instead of TMC2208 (Sensorless homing!!, higher current, faster speeds, software interface)
  - 3-pin (3.3,gnd,sig) HOME and ESTOP connectors instead of 2-pin
  - better power design -> less noise

Unfortunately, I made two pretty annoying mistakes. Therefore Ethersweep was not working (old verion 3 boards could be used though) and the TMC2209 didn't work as well. Accordingly this is not a production version. The issues will be fixed in Version 4.2.0.
