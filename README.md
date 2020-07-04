# SignalGeneratorSD (SD mod) based on AD9833 Module

by **ShaggyDog18@gmail.com**, JUNE 2020

github: https://github.com/ShaggyDog18/SignalGeneratorSD

License: [GNU GPLv3](https://choosealicense.com/licenses/gpl-3.0/)

Original firmware and hardware solution/schematic by: **Cezar Chirila**; 
URL: https://www.allaboutcircuits.com/projects/how-to-DIY-waveform-generator-analog-devices-ad9833-ATmega328p/

Also, the project was promoted by **GreatScott** with some simplified schematic: https://www.instructables.com/id/DIY-FunctionWaveform-Generator/; 
https://www.youtube.com/watch?v=Y1KE8eAC9Bk 

**If you like the new look and feel of SignalGeneratorSD, please, consider making a small donation using [PayPal](https://paypal.me/shaggyDog18/4USD)**

## Change Log:

- Improved, simplified, optimized, fixed bugs, used better/"standard" libraries for all components: the display, rotary encoder, button and, most important, for AD9833 module (I’ve never used a PHASE option... so, did not test it)
- Added graphic icons for signal representation on the display.
- Slightly changed navigation (see Navigation section below).
- Renamed FREQuency register on the display to CHANnel: so, now it looks like CHAN0 and CHAN1.
- Tied a signal mode to a CHANnel; so, now you may change signal form along with its frequency.
- Used EEPROM to store and recover settings.
- Added a new signal mode: square/meander signal wave at 1/2 frequency (for more accuracy of the output signal frequency). This is a standard feature of AD9833 module.

## Hardware:

- Any ATMega328P or 168P chip based Arduino board (UNO, Nano, Pro Mini)
- AD9833 Programmable Waveform Generator Module
- Schmitt-trigger 74LVC1G14 (optional)

## Libraries:

Download and install all below libraries as regular libraries in your Arduino IDE:

- **MD_A9833**, modified:  https://github.com/ShaggyDog18/MD_AD9833  (modification allows right functionning of ON / OFF feature)
- **RotaryEncoder**, modified: https://github.com/ShaggyDog18/RotaryEncoder
- **GyverButton**: https://github.com/AlexGyver/GyverLibs/tree/master/GyverButton
- **LCD1602 I2C** display: https://github.com/fdebrabander/Arduino-LiquidCrystal-I2C-library

## Compile Options/Firmware Configuration:

- `#define USE_MD_LIB` – use a new MD_AD9833 library: smaller, no bugs, trust more.  Still may compile with the old and  fixed AD9833 library by commenting. Strongly suggest using the new one.
- `#define GRAPH_ICONS` - use graphical icons for signal representation on the display; Original Text labels can be used if commented
- `#define ENABLE_EEPROM`- save settings to EEPROM, recover them at startup  
- `#define ENABLE_MEANDRE05F_SIGMODE` - extra signal mode: square wave out signal at 0.5 frequency. This is one of the AD9833 module's features, used for more precise frequency setting. 
	**Note:** Compatible with the new MD_AD9833 library only!
- `#define ENABLE_VOUT_SWITCH` - developed an extra output circuit that switch meander logic level to either 3.3v or 5v. Switched from menu by pin 6. See explanation and EasyEDA link below.  
- `#define SWAP_ENCODER_DIRECTION` - swap encoder pins if encoder is detecting rotation incorrectly
- `#define LCD_I2C_ADDRESS 0x3f` - may need to change I2C address of the display module
- `#define USE_PHASE` - use Phase instead of the FREQ register; never use nor tested

At the first start EEPROM: CRC Error will be shown. Will automatically reset settings to default and write them to EEPROM.

## Navigation:

- Single button click at the default screen -> go to SETTING_MODE.
- Encoder rotation any direction -> switch from one input parameter to another in a loop; a current input parament is highlighted by underline cursor.
- Encoder rotation at input frequency -> change value of the current digit (underlined by a cursor) of the frequency value.
- Single button click at active input parameter -> change parameter value. The new value is immediately applied.
- Long button press anywhere in settings -> save and apply the current value of a parameter and jump to operation screen (blinking cursor at the "f=" letter).

#### If EEPROM is enabled:

- Press and hold button during start up -> reset settings to default (just for the current session, does not write default settings to EEPROM).
Hold the button until display's backlight starts blinking. Backlight will blink 3 times to confirm the reset.   
- Double click anywhere -> save settings to EEPROM. Display backlight will blink 2 times to confirm.

## Known Feature

AD98333 module generates meandre (squarewave signal) at its VCC level. So, if VCC bus is +5v, then the amplitude of the squarewave output sugnal is +5v. 
In some cases a signal of +3.3v TTL may be required. 

There are several solution:
1. use 3.3v power bus for entire solution.
2. add 3.3v voltage regulator and switch between 5v and 3.3v power bus for entire setup including both uController and A9833 module (plain-rough solution).
3. add 3.3v voltage regulator and switch power bus of an output buffer (deployed). 

**Note:** The switch may also be a simple mechanical 2-position toggle switch!

So, I deployed the option#3: added an output buffer for meander/squarewave signal only based on Schmitt-trigger (for example, 74LVC1G14) which is connected right to the AD9833 out pin. By flipping the Schmitt-trigger's power bus between +5v and +3.3v, the output signal amplitude is switched accordingly. The switching is done from firmware (menu) by pin 6. 
To activate the feature in the firmware uncomment: `#define ENABLE_VOUT_SWITCH`

Schematic of the "ouput buffer" based on the Schmitt-trigger 74LVC1G14 is awailable at [EasyEDA](https://easyeda.com/Sergiy/switch-5-3-3v-power-bus)

Enjoy!

**If you like the new look and feel of SignalGeneratorSD, please, consider making a small donation using [PayPal](https://paypal.me/shaggyDog18/4USD)**
