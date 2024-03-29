# SignalGeneratorSD based on AD9833 Module

by **ShaggyDog18@gmail.com**, June-July 2020, FEB-2021

github: https://github.com/ShaggyDog18/SignalGeneratorSD

License: [GNU GPLv3](https://choosealicense.com/licenses/gpl-3.0/)

Original firmware and hardware solution/schematic by: **Cezar Chirila**; 
URL: https://www.allaboutcircuits.com/projects/how-to-DIY-waveform-generator-analog-devices-ad9833-ATmega328p/

Also, the project was promoted by **GreatScott** with some simplified schematic: https://www.instructables.com/id/DIY-FunctionWaveform-Generator/; 
https://www.youtube.com/watch?v=Y1KE8eAC9Bk

**If you like the new look and feel of SignalGeneratorSD, please, consider making a small "cup of coffee" donation using [PayPal](https://paypal.me/shaggyDog18/5USD)**

## Change Log:

- Use **MD_AD9833**(modified) library to control the AD9833 Module: compact and bug-free library with great functions.
- Improved, simplified, optimized, fixed bugs, used better/"standard" libraries for all components: the display, rotary encoder, button.
- Improved navigation, essentially, coded from scratch (refer to **Improved Navigation** section below).
- Improved the way frequency value is displayed (coded from scratch): 
  - option to hide leading zeros in frequency value (toggled by triple click of encoder button).
  - option to delimit thousands by a separation sign (toggled by triple click to encoder button). 
  - a selected option (one out of four possible combinations) is stored to EEPROM (if enabled) and is set at start up.
- Use EEPROM to store and recover settings at switch on (to disable comment `#define ENABLE_EEPROM`).  
- Added graphic icons for signal form representation on the display (if you still like the old way, comment `#define GRAPH_ICONS`). 
- Tied a signal mode to a Channel; so, now you may change signal form along with its frequency by selecting a channel.
- Added a new signal mode: square/meander signal wave at 1/2 frequency (for more accuracy of the output signal frequency). This is a standard feature of AD9833 module. Comment `#define ENABLE_MEANDRE05F_SIGMODE` if you do not need it.
- More convenient and fast way of input frequency value by rotary encoder: 
  - continuous input: if reach either '9' or '0' in a digit position, then it jumps over to the senior digit and decreases/increases it.
  - fast input: if fast encoder rotation is detected, then it increases/decreases ten times of a current digit position
  - "Running" frequency - the value of frequency is applied "on a fly" with a small 0.5 sec delay, so that you keep adjusting the frequency by encoder and the value is applied in 0.5 sec after your input is complete.
- `Stepped Sweep Generator`- the frequency is varied in a range defined by values set in Ch#0 (start of the range) and Ch#1 (end of the range) with signal settings of Ch#0 and discrete steps of 0,1 of a current running frequency (kind of logarithmic steps).
Frequency value steps either up or down from the start of the range depends on what channels' frequency is larger. Frequency is changed discretely every 250 mSec (can be changed at compilation). 
The running `Stepped Sweep Generator` cycle is indicated by a blinking cursor at the end of a frequency value. Can be activated for Ch#0 only and uses its signal settings. 
While running, can be cancelled by short press and hold of OK button. When the end of the range is reached, it pauses for 3 sec and switches back to Ch#0 settings. 
Sweep generators are commonly used to test the frequency response of electronic filter circuits. Read more about [Sweep Generator in Wikipedia](https://en.wikipedia.org/wiki/Sweep_generator)

**Note**: I’ve never used/tested a PHASE option... Use it at your risk...

## Hardware:

- Any ATMega328P (tested) or ATMega168P (tested) chip based Arduino board (UNO, Nano, Pro Mini)
- AD9833 Programmable Waveform Generator Module
- LCD1602 Display with I2C Module
- Rotary Encoder
- Schmitt-trigger 74LVC1G14 (optional)

## Libraries

Download and install all below libraries as regular libraries in your Arduino IDE environment (to the `Library` folder):

- **MD_A9833** (modified):  https://github.com/ShaggyDog18/MD_AD9833  (modification allows a right functioning of ON/OFF functions)
- **RotaryEncoder** (modified): https://github.com/ShaggyDog18/RotaryEncoder
- **OneButton** (modified): https://github.com/ShaggyDog18/OneButton 
- **LCD1602 I2C**: https://github.com/fdebrabander/Arduino-LiquidCrystal-I2C-library
- **WatchDog**: https://github.com/GyverLibs/GyverWDT

## Compile Options/Firmware Configuration:

You may pick up a pre-compiled firmware HEX file that includes all the below advanced options (except ENABLE_VOUT_SWITCH and USE_PHASE which are disabled).
- `#define GRAPH_ICONS` - use graphic icons for signal representation on the display; Still, the original text labels can be used if commented.
- `#define ENABLE_EEPROM`- save settings to EEPROM, recover them at startup.
- `#define ENABLE_MEANDRE05F_SIGMODE` - extra signal mode: squarewave signal at 0.5 frequency. This is one of the AD9833 module's features, used for more precise frequency setting. 
- `#define RUNNING_FREQUENCY` - the value of frequency is applied "on the fly" with a small 0.5 sec delay so that you keep adjusting the frequency by encoder and the value is applied in 0.5 sec after your input is complete.
- `#define STEPPED_SWEEP_GENERATOR` - the value of frequency is varied in a range defined by frequency values set in Ch#0 and Ch#1 with signal settings of Ch#0 and a discrete step of 0,1 of a current frequency.
- `#define ENABLE_VOUT_SWITCH` - (disabled by default) developed an extra output circuit that switch meander logic level to either 3.3v or 5v. Switched from menu by pin 6. See explanation and EasyEDA link below in the **Squarewave Signal Amplitude Feature** chapter below. 
- `#define LCD_I2C_ADDRESS 0x3f` - may need to change I2C address of the display module.
- `#define EEPROM_ADDRESS_SHIFT` - start address in EEPROM to store settings; if EEPROM resource is vanished and you start getting `"EEPROM CRC Error"` at launch, change the settings block start address shifting it to the other unused EEPROM area. The entire settings block takes 14 bytes only.
- **NEW** `#define ENABLE_WATCHDOG` - use WatchDog timer to prevent firmware from hanging out. 
- `#define SWAP_ENCODER_DIRECTION` - (disabled by default) swap encoder pins if encoder is detecting rotation incorrectly.
- `#define USE_PHASE` - (disabled by default) use Phase instead of the FREQ register; never used nor tested :-) Sorry, no guarantee it works...

## Improved Navigation:

- Default Operation Screen:
  - Single button click -> go to Settings Mode.
  - Double click -> save settings to EEPROM (in case EEPROM is enabled). Display backlight will blink 2 times to confirm the operation.
  - Encoder rotation -> enter Settings Mode.
- Frequency Input Mode:
  - Single click -> change frequency input mode: input a digit value or change a digit position.
  - Long press -> exit Frequncy Input Mode to Settings Mode.
  - Encoder rotation -> change value of the current digit of the frequency value (digit is ighlighted by a blinking cursor) OR change an input digit postion (flat cursor). (depends of the frequency input mode).
  - Fast encoder rotation -> change value of more significant digit rather than the current digit position.
  - "Running" frequency -> the value of frequency  is applied "on the fly" with a small 0.5 sec delay; keep adjusting the frequency by encoder and the set value is applied in 0.5 sec after your've stopped rotating encoder, so the input is completet.
- Settings Mode:
  - Encoder rotation any direction -> switch from one input parameter to another in a loop; a current input parament is highlighted by underline cursor.
  - Single click at active input parameter -> change parameter's value. The new value is immediately applied (except Frequeency Input mode).
  - Long button press anywhere in settings mode -> save and apply the current value of a parameter and jump to Operation Screen (blinking cursor at the "f=" letter).
  - Triple click anywhere -> change the way the frequency value is displayed: with/without leading zeros; with/without thousands separation sign. All four possible combinations are toggled in a loop. Default set: no leading zeros with a separation apostrophe. 
The thousands delimiter is different from country to country. In the United States, this character is a comma (,) in Germany, a period (.), in Sweden, a space. So, you may re-define `DELIMITER` sign to one you аrе accustomed to: comma, period, space, astrisk, etc... Just search for `DELIMITER` definition.
- `Stepped Sweep Generator` feature:
  - Double click in SETTING_MODE while in channel Ch#0 position -> initiate the `Stepped Sweep Generator` feature; runs one cycle of a frequency variation.
  - Single click or click and hold -> cancel a `Stepped Sweep Generator` sweep cycle.  

#### If EEPROM is enabled:

- Press and hold button during start up -> reset settings to default (just for the current session, does not write default settings to EEPROM).
Hold the button until display's backlight starts blinking. Backlight will blink 3 times to confirm the reset.   
- Double click anywhere except input frequency and Ch#0 -> save settings to EEPROM. Display backlight will blink 2 times to confirm.
- At the first launch of the firmware `Error:CRC EEPROM` will be shown because no settings are in the EEPROM yet. Settings will be automatically set to default and saved to EEPROM. The error will not appear any more.

## Squarewave Signal Amplitude Feature

The AD98333 module generates meandre (squarewave signal) with amplitude equal to its VCC level. So, if VCC bus is +5v, then the amplitude of the squarewave sugnal is +5v. In some cases a signal of +3.3v TTL may be required.

There are several solutions:
1. use +3.3v power bus for entire solution (will not have +5v output squarewave signal then).
2. add +3.3v voltage regulator and switch between +5v and +3.3v power bus for entire setup including both uController and A9833 module (plain-rough solution).
3. add +3.3v voltage regulator and an output buffer; flip power bus between +5v and +3.3v for the output buffer only (deployed, switching from menu by flipping pin#6). 

**Note:** The +5v/+3.3v power buss switch may also be a simple mechanical 2-position toggle switch!

So, I deployed the option#3: added an output buffer for meander/squarewave signal only based on Schmitt-trigger (for example, 74LVC1G14) which is connected right to the AD9833 out pin. By flipping the Schmitt-trigger's power bus between +5v and +3.3v, the output signal amplitude is switched accordingly. The switching is done from firmware (menu) by pin#6. 
To activate the feature in the firmware uncomment: `#define ENABLE_VOUT_SWITCH`

Schematic of the "ouput buffer" based on the Schmitt-trigger 74LVC1G14 is awailable at [EasyEDA](https://easyeda.com/Sergiy/switch-5-3-3v-power-bus)

## Compact Case

Developed a compact case for the generator (see pictures), available at: 
- Thingiverse  : https://www.thingiverse.com/thing:4552227
- MyMiniFactory: https://www.myminifactory.com/object/3d-print-128983 

## Gratitude

**If you like the new look and feel of SignalGeneratorSD, please, consider making a small "cup of coffee" donation using [PayPal](https://paypal.me/shaggyDog18/5USD)**

Inhale a new life into your Signal Generator! Enjoy!
