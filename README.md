# SignalGeneratorSD

 SignalGeneratorSD based on AD9833 ModuleSignal Generator (SD mod) based on AD9833 Module
===

Author: Cezar Chirila
URL: https://www.allaboutcircuits.com/projects/how-to-DIY-waveform-generator-analog-devices-ad9833-ATmega328p/

Re-coded/improved by ShaggyDog18@gmail.com
JUNE 2020
github: https://github.com/ShaggyDog18/SignalGeneratorSD

License: GNU GPLv3: https://choosealicense.com/licenses/gpl-3.0/

## Change Log:

- Improved, simplified, optimized, fixed bugs, used better/"standard" libraries for all components: the display, rotary encoder, button and, most important, for AD9833 module (I’ve never used a PHASE option... so, did not test it)
- Added graphic icons for signal representation on the display
- Renamed FREQuency register on the display to CHANnel: so, now it looks like CHAN0 and CHAN1
- Tied a signal mode to CHANnel; so, now you may change signal form along with the frequency
- Slightly changed navigation (see below)
- use EEPROM to store and recover settings

## Libraries:

- RotaryEncoder, modified: https://github.com/ShaggyDog18/RotaryEncoder
- MD_A9833, modified:  https://github.com/ShaggyDog18/MD_AD9833
- GyverButton: https://github.com/AlexGyver/GyverLibs/tree/master/GyverButton
- LCD I2C display (standard): https://github.com/fdebrabander/Arduino-LiquidCrystal-I2C-library

## Compile Options/Firmware Configuration:

- #define USE_MD_LIB – use a new MD_AD9833 library: smaller, no bugs, trust more.  Still may compile with the old and  fixed AD9833 library by commenting. Strongly suggest using the new one.
- #define GRAPH_ICONS - use graphical icons for signal representation on the display; Original Text labels can be used if commented
- #define SWAP_ENCODER_DIRECTION  - swap encoder pins if encoder is detecting rotation incorrectly
- #define ENABLE_EEPROM - save settings to EEPROM, recover them at startup  
- #define LCD_I2C_ADDRESS 0x3f - may need to change I2C address of the display module
- #define ENABLE_VOUT_SWITCH - developed an extra output circuit that switch meander logic level of either 3.3v or 5v; switched from menu by pin 6. EasyEDA link: 

At first start EEPROM: CRC Error will be shown. Will automatically reset settings to default and write them to EEPROM.

## Navigation:

- Single button click at the default screen -> go to SETTING_MODE.
- Encoder rotation any direction -> switch from one input parameter to another in a loop; a current input parament is highlighted by underline cursor.
- Encoder rotation at input frequency -> change value of the current digit (underlined by a cursor) of the frequency value.
- Single button click at active input parameter -> change parameter value. The new value is immediately applied.
- Long button press anywhere in settings -> save and apply the current value of a parameter and jump to operation screen (blinking cursor at the "f=" letter).

If EEPROM is enabled:

- Press and hold button during start up -> reset settings to default (just for the current session, does not write default settings to EEPROM).
Hold the button until display's backlight starts blinking. Backlight will blink 3 times to confirm the reset.   
- Double click anywhere -> save settings to EEPROM. Display backlight will blink 2 times to confirm.



