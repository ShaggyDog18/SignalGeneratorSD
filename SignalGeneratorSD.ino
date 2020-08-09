/*=================== Signal Generator -SD mod- based on AD9833 Module
# SignalGeneratorSD based on AD9833 Module

by **ShaggyDog18@gmail.com**, June-July 2020

github: https://github.com/ShaggyDog18/SignalGeneratorSD

License: [GNU GPLv3](https://choosealicense.com/licenses/gpl-3.0/)

Original firmware and hardware solution/schematic by: **Cezar Chirila**; 
URL: https://www.allaboutcircuits.com/projects/how-to-DIY-waveform-generator-analog-devices-ad9833-ATmega328p/

Also, the project was promoted by **GreatScott** with some simplified schematic: https://www.instructables.com/id/DIY-FunctionWaveform-Generator/; 
https://www.youtube.com/watch?v=Y1KE8eAC9Bk 

**If you like the new look and feel of SignalGeneratorSD, please, consider making a small donation using [PayPal](https://paypal.me/shaggyDog18/4USD)**

## Change Log:

- Use **MD_AD9833**(modified) library to control the AD9833 Module: compact and bug-free library with great functions.
- Improved, simplified, optimized, fixed bugs, used better/"standard" libraries for all components: the display, rotary encoder, button.
- Improved navigation, essentially, coded from scratch (see Navigation section below).
- Improved the way frequency value is displayed (coded from scratch): 
  - option to hide leading zeros in frequency value (toggled by triple click of encoder button).
  - option to delimit thousands by a separation sign (toggled by triple click to encoder button). 
  - a selected option (one out of four possible combinations) is stored to EEPROM (if enabled) and is set at start up.
- Use EEPROM to store and recover settings at switch on (to disable comment `#define ENABLE_EEPROM`).  
- Added graphic icons for signal form representation on the display (if you still like the old way, comment `#define GRAPH_ICONS`). 
- Tied a signal mode to a Channel; so, now you may change signal form along with its frequency by selecting a channel.
- Added a new signal mode: square/meander signal wave at 1/2 frequency (for more accuracy of the output signal frequency). This is a standard feature of AD9833 module. Comment `#define ENABLE_MEANDRE05F_SIGMODE` if you do not need it.
- More convenient and fast way of input frequency value by rotary encoder (if you still like the old way, comment `#define NEW_WAY_INPUT_FREQ`): 
  - continuous input: if reach either '9' or '0' in a digit position, then it jumps over to the senior digit and decreases/increases it.
  - fast input: if fast encoder rotation is detected, then it increases/decreases ten times of a current digit position
  - **NEW** "Running" frequency - the value of frequency is applied "on a fly" with a small 0.5 sec delay, so that you keep adjusting the frequency by encoder and the value is applied in 0.5 sec after your input is complete.
- **NEW** - `Stepped Sweep Generator`: the frequency is varied in a range defined by values set in Ch#0 (start of the range) and Ch#1 (end of the range) with signal settings of Ch#0 and discrete steps of 0,1 of a current running frequency (kind of logarithmical steps).
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

- **MD_A9833** (modified):  https://github.com/ShaggyDog18/MD_AD9833  (modification allows right functioning  of ON / OFF functions)
- **RotaryEncoder** (modified): https://github.com/ShaggyDog18/RotaryEncoder
- **GyverButton**: https://github.com/AlexGyver/GyverLibs/tree/master/GyverButton
- **LCD1602 I2C**: https://github.com/fdebrabander/Arduino-LiquidCrystal-I2C-library

## Compile Options/Firmware Configuration:

- `#define GRAPH_ICONS` - use graphic icons for signal representation on the display; Still, the original text labels can be used if commented.
- `#define ENABLE_EEPROM`- save settings to EEPROM, recover them at startup.
- `#define ENABLE_MEANDRE05F_SIGMODE` - extra signal mode: squarewave signal at 0.5 frequency. This is one of the AD9833 module's features, used for more precise frequency setting. 
- `#define NEW_WAY_INPUT_FREQ` - new faster and more convenient  way of input frequency by encoder; if you like the old way - comment it!
- **NEW** `#define RUNNING_FREQUENCY` - the value of frequency is applied "on the fly" with a small 0.5 sec delay so that you keep adjusting the frequency by encoder and the value is applied in 0.5 sec after your input is complete.
- **NEW** `#define STEPPED_SWEEP_GENERATOR` - the value of frequency is varied in a range defined by frequency values set in Ch#0 and Ch#1 with signal settings of Ch#0 and a discrete step of 0,1 of a current frequency. 
- `#define SWAP_ENCODER_DIRECTION` - swap encoder pins if encoder is detecting rotation incorrectly.
- `#define ENABLE_VOUT_SWITCH` - developed an extra output circuit that switch meander logic level to either 3.3v or 5v. Switched from menu by pin 6. See explanation and EasyEDA link below in the **Squarewave Signal Amplitude Feature** chapter below. 
- `#define USE_PHASE` - use Phase instead of the FREQ register; never used nor tested :-) Sorry, no guarantee it works...
- `#define LCD_I2C_ADDRESS 0x3f` - may need to change I2C address of the display module.
- `#define EEPROM_ADDRESS_SHIFT` - start address in EEPROM to store settings; if EEPROM resource is vanished and you start getting `"EEPROM CRC Error"` at launch, change the settings block start address shifting it to the other unused EEPROM area. The entire settings block takes 14 bytes only.

## Improved Navigation:

- Single button click at the default operation screen -> go to SETTING_MODE.
- Input frequency value:
  - Single click -> jump to the left to more significant  number.
  - Double click -> jump to the right to less significant  number.
  - Encoder rotation -> change value of the current digit (underlined by a cursor) of the frequency value.
  - Fast encoder rotation -> change value of more significant digit rather than the current digit position (if `NEW_WAY_INPUT_FREQ` is defined).
  - **NEW** "running" frequency  ->  the value of frequency  is applied "on the fly" with a small 0.5 sec delay; keep adjusting the frequency by encoder and the set value is applied in 0.5 sec after your input is completet.
- Encoder rotation any direction -> switch from one input parameter to another in a loop; a current input parament is highlighted by underline cursor.
- Single click at active input parameter -> change parameter's value. The new value is immediately applied.
- Long button press anywhere in settings mode -> save and apply the current value of a parameter and jump to operation screen (blinking cursor at the "f=" letter).
- Triple click anywhere -> change the way the frequency value is displayed: with/without leading zeros; with/without thousands separation sign. All four possible combinations are toggled in a loop. Default set: no leading zeros with a separation apostrophe. 
The thousands delimiter is different from country to country. In the United States, this character is a comma (,) in Germany, a period (.), in Sweden, a space. 
So, you may re-define `DELIMITER` sign to one you аrе accustomed to: comma, period, space, astrisk, etc... Just search for `DELIMITER` definition.
- **NEW** `Stepped Sweep Generator` feature:
  - Double click in SETTING_MODE while in channel Ch#0 position -> initiate the `Stepped Sweep Generator` feature; runs one cycle of a frequency variation.
  - Single click or click and hold -> cancel a `Stepped Sweep Generator` sweep cycle.  

#### If EEPROM is enabled:

- Press and hold button during start up -> reset settings to default (just for the current session, does not write default settings to EEPROM).
Hold the button until display's backlight starts blinking. Backlight will blink 3 times to confirm the reset.   
- Double click anywhere except input frequency and Ch#0 -> save settings to EEPROM. Display backlight will blink 2 times to confirm.
- At the first launch of the firmware `Error:CRC EEPROM` will be shown because no settings are in the EEPROM yet. Settings will be automatically set to default and saved to EEPROM. The error will not appear any more.

## Squarewave Signal Amplitude Feature

AD98333 module generates meandre (squarewave signal) at its VCC level. So, if VCC bus is +5v, then the amplitude of the squarewave sugnal is +5v. 
In some cases a signal of +3.3v TTL may be required. 

There are several solutions:
1. use +3.3v power bus for entire solution (will not have +5v output squarewave signal then).
2. add +3.3v voltage regulator and switch between +5v and +3.3v power bus for entire setup including both uController and A9833 module (plain-rough solution).
3. add +3.3v voltage regulator and an output buffer; flip power bus between +5v and +3.3v for the output buffer only (deployed, switching from menu by flipping pin#6). 

**Note:** The +5v/+3.3v power buss switch may also be a simple mechanical 2-position toggle switch!

So, I deployed the option#3: added an output buffer for meander/squarewave signal only based on Schmitt-trigger (for example, 74LVC1G14) which is connected right to the AD9833 out pin. By flipping the Schmitt-trigger's power bus between +5v and +3.3v, the output signal amplitude is switched accordingly. The switching is done from firmware (menu) by pin#6. 
To activate the feature in the firmware uncomment: `#define ENABLE_VOUT_SWITCH`

Schematic of the "ouput buffer" based on the Schmitt-trigger 74LVC1G14 is awailable at [EasyEDA](https://easyeda.com/Sergiy/switch-5-3-3v-power-bus)

Inhale new life into your Signal Generator! Enjoy!

**If you like the new look and feel of SignalGeneratorSD, please, consider making a small "cup of coffee" donation using [PayPal](https://paypal.me/shaggyDog18/4USD)**

*/
// --- COMPILER CONFIG ----
#define GRAPH_ICONS     // use graphical icons for sign representation on display
#define ENABLE_EEPROM   // sacve settings to EEPROM, recover them at startup
#define ENABLE_MEANDRE05F_SIGMODE   // compatible with the new MD_AD9833 library only; if you do not need it - comment!
#define NEW_WAY_INPUT_FREQ  // input frequency with jumping to the next digit position; Fast rotation adds 10 times more
#define RUNNING_FREQUENCY   // The value of frequency is applied "on the fly" with a small 0.4 sec delay. The new frequency value is applied in 0.4 sec after your input is complete.
#define ENABLE_VOUT_SWITCH  // developped an extra output circuit that switch meander logic level of eather 3.3v or 5v; switched from menu by pin 6
#define EEPROM_ADDRESS_SHIFT 0  // start address in EEPROM to store settings; if EEPROM is vanished and you start getting "EEPROM CRC Error" at launch, change the start address to shift to the other unused EEPROM area
#define STEPPED_SWEEP_GENERATOR  // veries frequency in a range defined by frequency values set in Ch#0 and Ch#1 with a signal settings of Ch#0 and a discrete step of 0,1 of a current frequency. RUNNING_FREQUENCY should be defined.
//#define SWAP_ENCODER_DIRECTION  // swap if encoder is rotating in the wrong direction
//#define USE_PHASE    //Uncomment the line below if you want to change the Phase instead of the FREQ register // never use or tested
// ------------------------

//Check up and correct Compiler Configuration
#if defined( STEPPED_SWEEP_GENERATOR ) && !defined( RUNNING_FREQUENCY )
  #define RUNNING_FREQUENCY
#endif
#if defined( RUNNING_FREQUENCY ) && !defined( NEW_WAY_INPUT_FREQ )
  #define NEW_WAY_INPUT_FREQ  // Runing frequency works in the NEW_WAY_INPUT_FREQ only!
#endif

//-------------------------

// --- INCLUDE SECTION ---
#include <avr/delay.h>
#include <LiquidCrystal_I2C.h>
#include <RotaryEncoder.h>
#include <GyverButton.h>
#include <MD_AD9833.h>  // modified library. Midifications are commented with ShaggyDog keyword
#ifdef ENABLE_EEPROM
  #include <avr/eeprom.h>
#endif
// ------------------------

#ifdef SWAP_ENCODER_DIRECTION
  #define DT  2  // encoder pin
  #define CLK 3  // encoder pin
#else
  #define DT  3  // encoder pin
  #define CLK 2  // encoder pin
#endif

#define BUTTON_OK  4
#define TOGGLE_OUT 5      // switch the out off if toggle to Meandre/CLK which is 5V out to protect the output from saturation (under development)
#define TOGGLE_CLK_VOUT 6 // switch output sine voltage between 3.3v (default) and 5v 
#define FSYNC_PIN  10

// LCD Settings
#define LCD_I2C_ADDRESS 0x3f	// 0x27  // 0x3f - alternative i2c address (blue screen module)
#define LCD_DISP_COLS   16
#define LCD_DISP_ROWS   2

#define FREQ_N_DIGITS  8  // number of digits for frequency to display
#define PHASE_N_DIGITS 4  // number of digits for phase to display

// --- Initialize hardware ---
LiquidCrystal_I2C lcd( LCD_I2C_ADDRESS, LCD_DISP_COLS, LCD_DISP_ROWS ); // LCD Initialise
RotaryEncoder encoder( DT, CLK );  // initialise the encoder on pins 2 and 3 (interrupt pins)
GButton buttonOK( BUTTON_OK );     // initialize Encoder Button
MD_AD9833 sigGen( FSYNC_PIN );     // initialize AS9833 module, connected to hardware SPI

// Variables used to position cursor for data input and walk through menu
#ifdef ENABLE_VOUT_SWITCH  // on/off signal state is represented by one symbol/char
  const uint8_t settingInputPos[] = {0, 15, 19, 22, 29};// used to be {0, 14, 20, 23, 29}; for CHAN
#else
  const uint8_t settingInputPos[] = {0, 15, 19, 29};    // used to be {0, 14, 20, 29};  
#endif
// grapic single chars to represent ON and OFF state of the generator's out signal, taken from LCD1602's Font Table, datasheet, page 14 
const char ONchar  = 0b01111110;  // looks like '->'
const char OFFchar = 0b11011011;  // looks like empty square '|_|'. Also can use ']' or any other symbol...

// --- Define data types/structures ---
enum menuState_t : uint8_t {
  DEFAULT_SCREEN = 0,
  SETTING_MENU,
  FREQUENCY_SETTING, 
  #ifdef USE_PHASE  
    PHASE_SETTING,
  #endif
} menuState = DEFAULT_SCREEN;

enum sigmode_t : uint8_t {  // SigMode <=> Signal Mode
  SIGMODE_SINE = 0,
  SIGMODE_TRIANGLE,
  SIGMODE_MEANDRE,
  #ifdef ENABLE_MEANDRE05F_SIGMODE
    SIGMODE_MEANDRE05F,
  #endif
  NUMBER_SIGMODES
};

enum inputPositions_t : uint8_t {
  IP_FREQUENCY = 0, // IP means Input Position
  IP_ONOFF,
  IP_CHANNEL,
  #ifdef ENABLE_VOUT_SWITCH
    IP_VOUT_SWITCH,
  #endif
  IP_SIGNAL_MODE,
  NUMBER_INPUT_POSITIONS
};

enum frequencyValuePresentation_t : uint8_t {
  LEAD_ZERO_and_NO_DELIMITER = 0, // 0b00
  LEAD_ZERO_and_DELIMITER,        // 0b01
  NO_LEAD_ZERO_and_NO_DELIMITER,  // 0b10
  NO_LEAD_ZERO_and_DELIMITER,     // 0b11
  NUMBER_FREQUENCY_DISPLAY_MODES   
};

// masks for fast if() with binary '&' to be applied to displayFrequencyMode
const uint8_t NO_LEAD_ZERO_MASK        = 0b00000010;  
const uint8_t THOUSANDS_DELIMITER_MASK = 0b00000001;

// thousands delimiter sign for frequency value presentation
const char DELIMITER = 0b00100111; // regular apostrophe sign '
//const char DELIMITER = '`';      // another "angled" apostrophe sign;
//const char DELIMITER = ',';      // comma for the United States
//const char DELIMITER = '.';      // period for Germany
//const char DELIMITER = ' ';      // space for Sweeden

// Settings structure to keep all settings together in one structure and store it in EEPROM "at once", size of 14 bytes
struct settings_t {
  unsigned long frequency[2]; 
  sigmode_t currentMode[2]; // uint8_t type
  bool currentChannel;      // current FREQ register/Channel#0; FALSE for Ch#0, TRUE for Ch#1
  bool toggleCLKoutVolt;    // 3.3v CLK signal by default
  uint8_t displayFrequencyMode;  // 4 ways to display frequency value: with/without leading zeros, with/without separation delimiter 
  uint8_t checkSum;
#ifdef ENABLE_EEPROM
  } settings;
#else
  } settings = { 1000UL, 1000UL, SIGMODE_SINE, SIGMODE_SINE, false, false, NO_LEAD_ZERO_and_DELIMITER, 0 }; // dafault values
#endif


int8_t digitPos = 0;  // current input position for frequency value
const unsigned long maxFrequency = 12500000UL; // according to AS9833 Datasheet the max frequency is 12.5 MHz
#ifdef USE_PHASE
const unsigned int maxPhase = 4095; // Only used if you enable PHASE setting instead of FREQ register
uint16_t phase = 0;
#endif

// flags
bool defaultScreenUpdateFlag = true;
bool updateDisplayFlag = true;
bool signalOn = true;
#ifdef RUNNING_FREQUENCY 
  bool frequencyUpdatedFlag = false;
#endif

// Variables used to temporary store frequency value
unsigned long frequency;
uint8_t cursorInputPos = IP_FREQUENCY;

#ifdef RUNNING_FREQUENCY 
const unsigned long frequencyUpdateApplyDelay = 400UL;  // 0.4 sec delay between change in frequency value and its communication/application to AD9833 module; 
                                                        // use delay to avoid every small freq value change to be communicated to the module; allow fast change in freq value without application
unsigned long lastFrequencyUpdate;  // the most recent time in millis when the frequency value was updated
#endif

#ifdef STEPPED_SWEEP_GENERATOR  // very frequency in a range defined by frequencies set in Ch#0 and Ch#1 with a step of 0,1 of current frequency with signal settings in Ch#0
const unsigned long STEPPED_SWEEP_DELAY = 250UL;
#endif

#ifndef GRAPH_ICONS
// signal mode depicting constants
  #ifdef ENABLE_MEANDRE05F_SIGMODE
    const String mode[] = {"SIN", "TRI", "CLK", "C/2"};
  #else
    const String mode[] = {"SIN", "TRI", "CLK"};
  #endif
#endif

// Greek PHI symbol for phase shift
// Only used if you enable PHASE setting instead of FREQ register
#ifdef USE_PHASE
const uint8_t phi[] = { // greek 'ф' sign for phase
0b01110,0b00100,0b01110,0b10101,0b10101,0b01110,0b00100,0b01110 };
#else
const uint8_t ff[] = { // "FF" sign to indicate "OFF" state 
0b11100,0b10000,0b11000,0b10111,0b00100,0b00110,0b00100,0b00000 };
#endif

#if defined( GRAPH_ICONS ) or defined( ENABLE_VOUT_SWITCH )
const uint8_t meander[] = {  // meander sign _||_
0b00000,0b01110,0b01010,0b01010,0b01010,0b01010,0b11011,0b00000 };
#endif

#ifdef GRAPH_ICONS
// LCD mode depicting icons
const uint8_t sine[3][8] = {
0b00000,0b00000,0b00000,0b00000,0b10000,0b01000,0b00100,0b00011,
0b00000,0b00001,0b00010,0b00100,0b00100,0b01000,0b10000,0b00000,
0b11000,0b00100,0b00010,0b00001,0b00000,0b00000,0b00000,0b00000 };

const uint8_t triangle[2][8] = {
0b00000,0b00000,0b10000,0b01000,0b00100,0b00010,0b00001,0b00000,  // '\'
// use a standard '/' char
0b10000,0b01000,0b00100,0b00010,0b00001,0b00000,0b00000,0b00000 }; //'\'
#endif


//---------------------
//   setup()
//---------------------
void setup() {
  // Initialise the LCD, start the backlight and print a "bootup" message
  lcd.begin();

   // create graphic signs
#ifdef USE_PHASE  
  lcd.createChar(0, phi); // Custom graphic PHI char for LCD
#else
  lcd.createChar(0, ff); // Custom graphic FF char for LCD to indicate "Signal OFF" state 
#endif

#if defined( GRAPH_ICONS ) or defined( ENABLE_VOUT_SWITCH )
  lcd.createChar(1, meander); // Custom graphic meandre char for LCD
#endif
#ifdef GRAPH_ICONS
  lcd.createChar(2, sine[0]); // Custom graphic sine 1/3 char for LCD
  lcd.createChar(3, sine[1]); // Custom graphic sine 2/3 char for LCD
  lcd.createChar(4, sine[2]); // Custom graphic sine 3/3 char for LCD
  lcd.createChar(5, triangle[0]); // Custom graphic triangle 1/3 char for LCD
  lcd.createChar(6, triangle[1]); // Custom graphic triangle 3/3 char for LCD
//  lcd.createChar(7, vacant); // Custom graphic - vacant position
#endif  

  // Launch Screen
  lcd.home();
  lcd.backlight();
  lcd.print(F("Signal Generator"));
  lcd.setCursor(0, 1);
  lcd.print(F("-ShaggyDog 2020-"));
  _delay_ms(1500);

  // Initite AD9833 Module
  sigGen.begin(); // it also set up all default values

#ifdef ENABLE_EEPROM
  bool resetSettings = false;
  if( readSettingsFromEEPROM() ) {
    applyCurrentSettings();// set settings to the AD module that were read from EEPROM
  } else {
    lcd.setCursor(0, 0);
    lcd.print(F("Error:CRC EEPROM"));
    lcdPrintResetMsg( 1 );  // print Reset message in line # 1
    _delay_ms(3000);
    resetSettings = true;
    lcd.clear();
  }

  if( !digitalRead( BUTTON_OK ) || resetSettings ) { // if button is pressed at start up then all settings are reset to defaul
    lcdPrintResetMsg( 0 ); // print Reset message in line # 0
    settings.frequency[0] = settings.frequency[1] = 1000UL;
    settings.currentMode[0] = settings.currentMode[1] = SIGMODE_SINE;
    settings.toggleCLKoutVolt = false;
    settings.currentChannel = false; 
    settings.displayFrequencyMode = NO_LEAD_ZERO_and_DELIMITER;
    blinkDisplayBacklight(3);
    applyCurrentSettings();
    while( !digitalRead( BUTTON_OK ) ) ; // wait for button to be released
    lcd.clear();
  }
  if( resetSettings) writeSettingsToEEPROM();
#endif
  
  // Set encoder pins as interrupts
  attachInterrupt( digitalPinToInterrupt(DT),  encoderTickISR, CHANGE );
  attachInterrupt( digitalPinToInterrupt(CLK), encoderTickISR, CHANGE );

  #ifdef ENABLE_VOUT_SWITCH
    // init pins and set them to default state 
    pinMode( TOGGLE_CLK_VOUT, OUTPUT );
    toggleCLKvolt( settings.toggleCLKoutVolt );  // 3.3v by default (LOW signal output)
    
    pinMode( TOGGLE_OUT, OUTPUT );  // decrease amplitude of CLK signal to 650 mV by toggling an output divider (under development)
    toggleOut( settings.currentMode[(uint8_t)settings.currentChannel] );     // under development
  #endif

  // Clear the screen
  lcd.clear();
} // end of setup()
//---------------------------


// print settings Reset message: called twice, defined as a separate function to optimize the code
void lcdPrintResetMsg( const uint8_t _line ) {  // print Reset message in a line # x
  lcd.setCursor( 0, _line );
  lcd.print( F("Reset to default") );  
}  // lcdPrintResetMsg()
//---------------------


// ISR routine for Encoder
void encoderTickISR(void) {
  encoder.tick(); 
}
//---------------------


//---------------------
//   loop()
//---------------------
void loop() {
  buttonOK.tick();   // Check if encoder button has been pressed
  // process button clicks
  if( buttonOK.isSingle() ) processSingleClick(); // single click
  if( buttonOK.isHolded() ) processLongPress();   // long press
  if( buttonOK.isDouble() ) processDoubleClick(); // double click
  if( buttonOK.isTriple() ) processTripleClick(); // tripple click
  
  processEncoder( encoder.getDirection() );  

  #ifdef RUNNING_FREQUENCY 
    if( frequencyUpdatedFlag ) {  // frequency value was changed
      if( millis() - lastFrequencyUpdate > frequencyUpdateApplyDelay ) {  // apply new frequency value after a delay
        settings.frequency[(uint8_t)settings.currentChannel] = frequency;
        setADfrequency( settings.currentChannel, settings.frequency[(uint8_t)settings.currentChannel] );
        frequencyUpdatedFlag = false;
      }
    }  
  #endif

  if( updateDisplayFlag ) {   // Update display if needed
  // Display: 1st line
    displayFrequency( frequency );  
    displaySignalONOFF( signalOn );

  // Display: 2nd line
    #ifdef USE_PHASE
      displayPhase( phase );
    #else
      displayCurrentChannel( settings.currentChannel );
    #endif
    
    #ifdef ENABLE_VOUT_SWITCH
      displayCLKoutVolt( settings.toggleCLKoutVolt, settings.currentMode[(uint8_t)settings.currentChannel] );
    #endif
    
    displaySignalMode( settings.currentMode[(uint8_t)settings.currentChannel] );
    
    setCursor2inputPosition( cursorInputPos );
    updateDisplayFlag = false;
  }
  
  cursorPositionPostProcessing();  // set cursor to the right position after display update
} // end of loop()
//---------------------


void cursorPositionPostProcessing(void) { // set cursor to the right position after display update
  switch( menuState ) {
    case DEFAULT_SCREEN:     // Default state
      if( defaultScreenUpdateFlag ) {
        lcd.setCursor(0, 0);
        lcd.noCursor();
        lcd.blink();
        defaultScreenUpdateFlag = false;
      }
      break;

    case FREQUENCY_SETTING:    // Frequency setting
      uint8_t pos;
      if( settings.displayFrequencyMode & THOUSANDS_DELIMITER_MASK ) {  // binary '&' - that's correct!!! // thousands delimiter sign is ON
        pos = FREQ_N_DIGITS+3 - digitPos;
        if( digitPos > 2 ) pos--;  // skip place for separation point#1
        if( digitPos > 5 ) pos--;  // skip place for separation point#2
      } else {  // no separation points
        pos = FREQ_N_DIGITS+1 - digitPos;
      }
      lcd.setCursor( pos, 0 );
      break;

#ifdef  USE_PHASE // never tested
    case PHASE_SETTING:    // Phase setting
      lcd.setCursor( PHASE_N_DIGITS+1 - digitPos, 1 );
      break;
#endif      
  } // switch( menuState )
} // menuPostProcessing()
//---------------------


void processSingleClick(void) {
  switch( menuState ) {
    case DEFAULT_SCREEN:
      jump2settingMenu();
      break;
      
    case SETTING_MENU:
      switch( cursorInputPos ) {
          case IP_FREQUENCY: // go to frequesncy setting
             menuState = FREQUENCY_SETTING; 
             lcd.cursor();
             break;
             
          case IP_ONOFF: // switch an output generator signal on / off
            signalOn = ! signalOn;
            sigGen.setModeSD(signalOn ? MD_AD9833::MODE_ON : MD_AD9833::MODE_OFF); 
            break;
            
#ifndef USE_PHASE          // If USE_PHASE has not been set
          case IP_CHANNEL: // select channel 0 or 1
            settings.currentChannel = ! settings.currentChannel; // flip bool value
            setADchannel( settings.currentChannel );
            setADsignalMode( settings.currentMode[(uint8_t)settings.currentChannel] ); 
            break;
#endif

#ifdef ENABLE_VOUT_SWITCH
          case IP_VOUT_SWITCH: // switch meandre out voltage 5v vs. 3v
            settings.toggleCLKoutVolt = ! settings.toggleCLKoutVolt;  // flip bool value
            toggleCLKvolt( settings.toggleCLKoutVolt );
            break;
#endif            
          case IP_SIGNAL_MODE: // Change the signal mode (sine/triangle/clock)
            ++(*((uint8_t*)&(settings.currentMode[(uint8_t)settings.currentChannel])));  // increment currentMode value :-)
            if( settings.currentMode[(uint8_t)settings.currentChannel] == NUMBER_SIGMODES ) 
              settings.currentMode[(uint8_t)settings.currentChannel] = SIGMODE_SINE;
            // switch output to direct out
            #ifdef ENABLE_VOUT_SWITCH
              toggleOut( settings.currentMode[(uint8_t)settings.currentChannel] );
            #endif
            setADsignalMode( settings.currentMode[(uint8_t)settings.currentChannel] );
            break;
      } // switch( cursorInputPos )
      updateDisplayFlag = true;
      break;
             
    case FREQUENCY_SETTING:  
      // Each button press will either enable to change the value of another digit
      // or if all digits have been changed, to apply the setting
      if( digitPos < FREQ_N_DIGITS-1 ) {
        digitPos++;
      } else {
        digitPos = 0;
        #ifndef RUNNING_FREQUENCY 
          setADfrequency( settings.currentChannel, settings.frequency[(uint8_t)settings.currentChannel] );
        #endif
        jump2settingMenu();
      }        
      break;
        
#ifdef  USE_PHASE // never tested
    case PHASE_SETTING:    // Phase setting
      if( digitPos < PHASE_N_DIGITS-1 ) { 
        digitPos++;
      } else {
        digitPos = 0;
        sigGen.setPhase( settings.currentChannel ? MD_AD9833::CHAN_1 : MD_AD9833::CHAN_0, phase );
        jump2settingMenu();
      }
      break;      
#endif    
  }  // switch( menuState )
}  // processSingleClick(void)
//---------------------


void processLongPress(void) {
  switch( menuState ) {
  case SETTING_MENU:
    menuState = DEFAULT_SCREEN;
    cursorInputPos = IP_FREQUENCY;
    defaultScreenUpdateFlag = true;
    break;

#ifdef  USE_PHASE // never tested
  case PHASE_SETTING: 
    // setPhase here
    sigGen.setPhase( settings.currentChannel ? MD_AD9833::CHAN_1 : MD_AD9833::CHAN_0, phase );
    jump2settingMenu();
    break;
#endif

  case FREQUENCY_SETTING: 
    #ifndef RUNNING_FREQUENCY 
      setADfrequency( settings.currentChannel, settings.frequency[(uint8_t)settings.currentChannel] );
    #endif
    jump2settingMenu();
    break;
  } // switch( menuState )
} //  processLongPress(void)
//---------------------


void processDoubleClick(void) {
  switch( menuState ) {
  case FREQUENCY_SETTING: 
    if( digitPos > 0 ) digitPos--;
    break;

#ifdef STEPPED_SWEEP_GENERATOR
  case SETTING_MENU:
    if( cursorInputPos == IP_CHANNEL && !settings.currentChannel ) { // for Ch#0 only;
      steppedSweepGenerator();
    }
    break;
#endif
      
#ifdef ENABLE_EEPROM     
  default: 
    writeSettingsToEEPROM();
    blinkDisplayBacklight(2); // double blink to confirm operation of writing to EEPROM
#endif
  }
}  // processDoubleClick(void)
//---------------------


void processTripleClick(void) {
  settings.displayFrequencyMode++;
  if( settings.displayFrequencyMode == NUMBER_FREQUENCY_DISPLAY_MODES ) 
    settings.displayFrequencyMode = 0;  // LEAD_ZERO_and_NO_DELIMITER
  updateDisplayFlag = true;
  lcd.clear();
}  // processTripleClick()
//---------------------


void setCursor2inputPosition( const uint8_t _cursorPosition ) {
  // Move the cursor to display position in case it changed
    uint8_t realPosR = 0;
    uint8_t realPosC = settingInputPos[_cursorPosition];
    if( realPosC >= LCD_DISP_COLS ) {
      realPosC -= LCD_DISP_COLS;
      realPosR = 1;
    }
    lcd.setCursor( realPosC, realPosR );
    lcd.cursor();
} // setCursor2inputPosition()
//---------------------


void processEncoder( const RotaryEncoder::Direction _rotaryDirection ) {
  // Depending in which menu state you are the encoder will either change the value of a setting:
  //-+ frequency or it will change the cursor position
  switch( _rotaryDirection ) {
  case RotaryEncoder::Direction::CLOCKWISE:
  case RotaryEncoder::Direction::FAST_CW:
    switch( menuState ) {
      case DEFAULT_SCREEN: 
          jump2settingMenu();
          break;
          
      case SETTING_MENU: 
          cursorInputPos++;  	// changing input positions in a loop
          #ifdef ENABLE_VOUT_SWITCH		// jump over the VOUT_SWITCH input position in case of sine or triangle signal modes
          #ifdef ENABLE_MEANDRE05F_SIGMODE
            if( (settings.currentMode[(uint8_t)settings.currentChannel] != SIGMODE_MEANDRE && 
                 settings.currentMode[(uint8_t)settings.currentChannel] != SIGMODE_MEANDRE05F) && cursorInputPos == IP_VOUT_SWITCH ) cursorInputPos++;
          #else
            if( settings.currentMode[(uint8_t)settings.currentChannel] != SIGMODE_MEANDRE && cursorInputPos == IP_VOUT_SWITCH ) cursorInputPos++;
          #endif
          #endif
          if( cursorInputPos == NUMBER_INPUT_POSITIONS ) cursorInputPos = IP_FREQUENCY;
          setCursor2inputPosition( cursorInputPos );
          break;

      case FREQUENCY_SETTING:
        #ifdef NEW_WAY_INPUT_FREQ
          if( _rotaryDirection == RotaryEncoder::Direction::FAST_CW )
            frequency += power(10, digitPos+1);
          else  // slow encoder rotation
            frequency += power(10, digitPos);
           
          if( frequency > maxFrequency ) frequency = maxFrequency;
          #ifdef RUNNING_FREQUENCY
            lastFrequencyUpdate = millis();
            frequencyUpdatedFlag = true;
          #else
            settings.frequency[(uint8_t)settings.currentChannel] = frequency;
          #endif
          updateDisplayFlag = true;
          break;
          
        #else // old way of input frequency
        {
          // Here we initialise two variables.
          // newFrequency is the value of the frequency after we increment it
          // dispDigit is the digit that we are currently modifing, and we obtain it
          // by a neat little trick, using operator % in conjunction with division
          // We then compare these variables with the maximum value for our
          // frequency, if all is good, make the change        
          unsigned long newFrequency = frequency + power(10, digitPos);
          unsigned char dispDigit = frequency % power(10, digitPos + 1) / power(10, digitPos);
          if (newFrequency <= maxFrequency && dispDigit < FREQ_N_DIGITS+1) {
            frequency += power(10, digitPos);
            settings.frequency[(uint8_t)settings.currentChannel] = frequency;
            updateDisplayFlag = true;
          }
        } break; 
        #endif

        
#ifdef USE_PHASE
      case PHASE_SETTING: {
          // if USE_PHASE has been defined, changes in the encoder will vary the phase value (upto 4096)
          // A better implementation would be to use increment of pi/4 or submultiples of pi where 2pi = 4096
          unsigned long newPhase  = phase + power(10, digitPos);
          unsigned char dispDigit = phase % power(10, digitPos + 1) / power(10, digitPos);
          if( newPhase < maxPhase && dispDigit < PHASE_N_DIGITS+1 ) {
            phase += power(10, digitPos);
            updateDisplayFlag = true;
          }
        } break;
#endif        
      }  // switch( menuState )
      break;

    case RotaryEncoder::Direction::COUNTERCLOCKWISE:
    case RotaryEncoder::Direction::FAST_CCW:
      switch( menuState ) {
      case SETTING_MENU:
          if( cursorInputPos == IP_FREQUENCY ) {		// changing input positions in a loop
            cursorInputPos = NUMBER_INPUT_POSITIONS-1;
          } else {
            cursorInputPos--;
            #ifdef ENABLE_VOUT_SWITCH  // jump over the VOUT_SWITCH input position in case of sine or triangle signal modes
            #ifdef ENABLE_MEANDRE05F_SIGMODE
              if( (settings.currentMode[(uint8_t)settings.currentChannel] != SIGMODE_MEANDRE && 
                   settings.currentMode[(uint8_t)settings.currentChannel] != SIGMODE_MEANDRE05F) && cursorInputPos == IP_VOUT_SWITCH ) cursorInputPos--;
            #else
              if( settings.currentMode[(uint8_t)settings.currentChannel] != SIGMODE_MEANDRE && cursorInputPos == IP_VOUT_SWITCH ) cursorInputPos--;
            #endif
            #endif
          }  
          setCursor2inputPosition( cursorInputPos );
          break;

      case FREQUENCY_SETTING:
        #ifdef NEW_WAY_INPUT_FREQ
          if( _rotaryDirection == RotaryEncoder::Direction::FAST_CCW ) {
            if( frequency > power(10, digitPos+1) ) 
              frequency -= power(10, digitPos+1);
            else 
              break;  
          } else {    // slow encoder rotation
            if( frequency > power(10, digitPos) ) 
              frequency -= power(10, digitPos);
            else 
              break;
          }
          #ifdef RUNNING_FREQUENCY
            lastFrequencyUpdate = millis();
            frequencyUpdatedFlag = true;
          #else
            settings.frequency[(uint8_t)settings.currentChannel] = frequency;
          #endif
          updateDisplayFlag = true;
          break;
          
        #else // old way of input frequency
        {
          unsigned long newFrequency = frequency + power(10, digitPos);
          unsigned char dispDigit = frequency % power(10, digitPos + 1) / power(10, digitPos);
          if (newFrequency > 0 && dispDigit > 0) {
            frequency -= power(10, digitPos);
            settings.frequency[(uint8_t)settings.currentChannel] = frequency;
            updateDisplayFlag = true;
          }         
        } break; 
        #endif
     
#ifdef USE_PHASE
      case PHASE_SETTING: {
          // if USE_PHASE has been defined, changes in the encoder will vary the phase value (upto 4096)
          // A better implementation would be to use increment of pi/4 or submultiples of pi where 2pi = 4096
          unsigned long newPhase = phase + power(10, digitPos);
          unsigned char dispDigit = phase % power(10, digitPos + 1) / power(10, digitPos);
          if(newPhase > 0 && dispDigit > 0) {
            phase -= power(10, digitPos);
            updateDisplayFlag = true;
          }
        } break;
#endif        
      }  // switch( menuState )
      break;
  }  // switch( rotaryDirection )
}  // processingEncoder()
//---------------------


void jump2settingMenu(void) {
  menuState = SETTING_MENU;
  cursorInputPos = IP_FREQUENCY;
  lcd.noBlink();
  lcd.setCursor(0,0);
  lcd.cursor();
} // jump2settingMenu()
//---------------------


// Function to display the current frequency in the top left corner
void displayFrequency( unsigned long _frequencyToDisplay ) {
  lcd.setCursor(0,0); 
  lcd.print( F("f=") );
  
  uint8_t dispBuffer[FREQ_N_DIGITS]; // digits order is from the least significant to the most significant digit of frequency value
  uint8_t i;
  
  for( i=0; i<FREQ_N_DIGITS; i++ ) {
    dispBuffer[i] = _frequencyToDisplay % 10;
    _frequencyToDisplay /= 10;
    if( _frequencyToDisplay == 0 ) break;
  }
  // after performing the `break` command above i indicates the place of the last most significant meaningful digit in the freq value
  
  // correct cursor input position in freq value to the most significant digit if it's happen to orfan at the left 
  if( digitPos > i ) digitPos = i; 

  // print freq value from left to right starting from the most significant digit
  for( int8_t j=FREQ_N_DIGITS-1; j >= 0; j-- ) {
    if( settings.displayFrequencyMode & NO_LEAD_ZERO_MASK ) { // hide leaading Zeros
      if( j>i ) {  // digits (zeros) before most significant digit in frequency value
        if( settings.displayFrequencyMode & THOUSANDS_DELIMITER_MASK ) {  // delimiter sign is ON
          if( j == 5 || j == 2 ) lcd.print( ' ' );  // print space
        } 
        lcd.print( ' ' );
      } else {  // digits after (including) most significant digit in frequency value
        if( settings.displayFrequencyMode & THOUSANDS_DELIMITER_MASK ) {  // delimiter sign is ON
          if( j == 5 ) {
            if( i == 5 ) lcd.print( ' ' ); else lcd.print( DELIMITER );  // print either space or delimiter sign
          }  
          if( j == 2 ) {
            if( i == 2 ) lcd.print( ' ' ); else lcd.print( DELIMITER );   // print either space or delimiter sign
          }
        }
        lcd.print( dispBuffer[j] );
      }
    } else {  // show leading zeros
      if( settings.displayFrequencyMode & THOUSANDS_DELIMITER_MASK ) {  //  delimiter sign is ON
        if( j == 5 || j == 2 ) lcd.print( DELIMITER );
      }
      if( j>i ) lcd.print( '0' ); else lcd.print( dispBuffer[j] );   //lcd.print( dispBuffer[j] );  //try
    } // if() hide leaading Zeros
  } // for()
  lcd.print( F("Hz") );
} // displayFrequency()
//---------------------


// Function to display power state (ON/OFF) in the top right corner
void displaySignalONOFF( const bool _signalOn ) {
  #ifdef USE_PHASE
    lcd.setCursor(13, 0); 
    lcd.print( _signalOn ? F("ON ") : F("OFF") );
  #else
    if( settings.displayFrequencyMode & THOUSANDS_DELIMITER_MASK ) {  // separation sign ON
      lcd.setCursor(15, 0);
      //lcd.print( _signalOn ? ONchar : OFFchar );  // same as below
      if( _signalOn ) lcd.print( ONchar ); else lcd.print( OFFchar );  // more compact than the above line
    } else {  // separation sign OFF
      lcd.setCursor(14, 0);
      lcd.print('O');
      if( _signalOn )
        lcd.print('N');  // ON
      else
        lcd.write( 0 ); // OFF sign
    }
  #endif
}
//---------------------


// Function to display the mode in the bottom right corner
void displaySignalMode( const sigmode_t _currentMode ) {
  lcd.setCursor(13, 1);
#ifdef GRAPH_ICONS
  switch( _currentMode ) {
  case SIGMODE_SINE:     lcd.write( 2 );lcd.write( 3 );lcd.write( 4 );break;
  case SIGMODE_TRIANGLE: lcd.write( 5 );lcd.print('/');lcd.write( 6 );break;
  case SIGMODE_MEANDRE:  lcd.write( 1 );lcd.write( 1 );lcd.write( 1 );break;
  #ifdef ENABLE_MEANDRE05F_SIGMODE
    case SIGMODE_MEANDRE05F: lcd.write( 1 );lcd.print( F("/2") );break;
  #endif
  }
#else
  lcd.print( mode[(uint8_t)_currentMode ] );
#endif
} // displaySignalMode()
//---------------------


// Only used if you enable PHASE setting instead of FREQ register
void displayPhase( unsigned int _phaseToDisplay ) {
  lcd.setCursor(0, 1);
  lcd.write(0); // phase sign
  lcd.print('=');

  uint8_t dispBuffer[PHASE_N_DIGITS] = {0,0,0,0};
  for( uint8_t i=0; i<PHASE_N_DIGITS; i++ ) {
    dispBuffer[i] = _phaseToDisplay % 10;
    _phaseToDisplay /= 10;
    if( _phaseToDisplay == 0 ) break;
  }
  for( int8_t j=PHASE_N_DIGITS-1; j >= 0; j-- ) lcd.print( dispBuffer[j] );
} // displayPhase()
//---------------------


// Function to display the FREQ register/Channel (either 0 or 1) in the bottom left corner
void displayCurrentChannel( const bool _channel ) {
  lcd.setCursor(0, 1);
  lcd.print( F("Ch#") );  // display channel as Ch#1 Ch#0 vs. old style CHAN1 CHAN0  
  lcd.print( (uint8_t)_channel );
} // displayCurrentChannel()
//---------------------


void displayCLKoutVolt( const bool _toggleCLKoutVolt, sigmode_t _currentMode ) {
  lcd.setCursor(6, 1);
#ifdef ENABLE_MEANDRE05F_SIGMODE
  if( _currentMode == SIGMODE_MEANDRE || _currentMode == SIGMODE_MEANDRE05F ) {
#else
  if( _currentMode == SIGMODE_MEANDRE ) {
#endif
    lcd.write( 1 );  // meandre sign
    lcd.print( _toggleCLKoutVolt ? F("5.0v") : F("3.3v") );
  } else {
    lcd.print( F("     ") ); // 5 x spaces
  }
} // displayCLKoutVolt()
//---------------------

  
void toggleCLKvolt( const bool _toggleOutVolt ) {
 digitalWrite( TOGGLE_CLK_VOUT, _toggleOutVolt ? HIGH : LOW ); // HIGH = 5v, 3.3v
}
//---------------------


unsigned long power( const uint8_t a, uint8_t b ) {
  unsigned long res = 1;
  if( b > 0 ) {
    res = a; b--;
    for( uint8_t i = 0; i < b; i++ ) res *= a;
  }
  return res;
}
//---------------------


void setADsignalMode( const sigmode_t _currentMode ) {
  switch( _currentMode ) {
  case SIGMODE_SINE:     sigGen.setModeSD( MD_AD9833::MODE_SINE ); break;
  case SIGMODE_TRIANGLE: sigGen.setModeSD( MD_AD9833::MODE_TRIANGLE ); break;
  case SIGMODE_MEANDRE:  sigGen.setModeSD( MD_AD9833::MODE_SQUARE1 ); break;
  #ifdef ENABLE_MEANDRE05F_SIGMODE
    case SIGMODE_MEANDRE05F: sigGen.setModeSD( MD_AD9833::MODE_SQUARE2 ); break;
  #endif
  }
} // setADsignalMode()
//---------------------


void setADfrequency( const bool _channel, const unsigned long _frequency ) {
  sigGen.setFrequency( _channel ? MD_AD9833::CHAN_1 : MD_AD9833::CHAN_0, _frequency );
} // setADfrequency()
//---------------------


void setADchannel( const bool _channel ) {
  if( _channel ) {
    sigGen.setActiveFrequency( MD_AD9833::CHAN_1 );
    frequency = settings.frequency[1];
    //frequency = sigGen.getFrequency( MD_AD9833::CHAN_1 );  // alternative way is to read frequency value from AD9833 module
  } else {
    sigGen.setActiveFrequency( MD_AD9833::CHAN_0 );
    frequency = settings.frequency[0];
  }
} // setADchannel()
//---------------------


void blinkDisplayBacklight( const uint8_t nBlinks ) {
  for( uint8_t i=0; i<nBlinks; i++ ) {
    lcd.noBacklight(); _delay_ms(300);
    lcd.backlight();   _delay_ms(300);
  }
} // blinkDisplayBacklight()
//---------------------


void applyCurrentSettings(void) {
  setADfrequency( 0, settings.frequency[0] );
  setADfrequency( 1, settings.frequency[1] );
  // set current signal mode
  setADsignalMode( settings.currentMode[(uint8_t)settings.currentChannel] ); 
  setADchannel( settings.currentChannel );
} // applyCurrentSettings()
//---------------------


bool readSettingsFromEEPROM(void) { // read settings from EEPROM; return true if CRC check sum is OK
  eeprom_read_block( (void *)&settings, EEPROM_ADDRESS_SHIFT, sizeof(settings_t) );  //eeprom_read_block (void *__dst, const void *__src, size_t __n);
  // check the CRC8 chech sum of the memory block read from EEPROM
  uint8_t checkSum = crc8array( (uint8_t *) &settings, sizeof(settings_t)-1 ); // last array member is a byte of the CRC check sum, so passing one less byte
  if( checkSum == settings.checkSum ) return true;
  return false;
} // readSettingsFromEEPROM()
//---------------------


void writeSettingsToEEPROM(void) { // write settings to EEPROM
  // calculate the CRC8 chech sum of settings structure and write it to EEPROM along with settings
  uint8_t checkSum = crc8array( (uint8_t *)&settings, sizeof(settings_t)-1 ); // last array member is a byte of the CRC check sum, so passing one less byte
  settings.checkSum = checkSum;
  eeprom_update_block( (void *)&settings, EEPROM_ADDRESS_SHIFT, sizeof(settings_t) );   // write settings to EEPROM: eeprom_update_block(const void *__src, void *__dst, size_t __n)
} // writeSettingsToEEPROM()
//---------------------


uint8_t crc8array( const uint8_t data[], uint8_t dataSize ) {   // calculate CRC8 check sum for the memory block
  uint8_t checkSum = 0xFF;  // initial crc check sum value (see crc8 algorythm in Wikipedia)

  dataSize--; // decrease one time so that do not decrease it by 1 in the below for() checking the condition
  for( uint8_t i=0; i < dataSize; i++ ) {  // CRC8 for all bytes in the memory block
    checkSum = crc8update( checkSum, data[i] );  
  }
  return checkSum;
} //crc8array()
//---------------------


uint8_t crc8update( uint8_t crc, const uint8_t data ) {  // standard calculation from Wikipedia
  crc ^= data;
  for( uint8_t i = 0; i < 8; i++ ) 
    crc = (crc & 0x80) ? (crc << 1)^0x31 : crc << 1;  // same as:  if( crc & 0x80 ) crc = (crc << 1)^0x31; else crc = crc << 1;
    
  return crc;
}  // crc8update()
//---------------------


// under development
void toggleOut( const sigmode_t _currentMode ) {
#ifdef ENABLE_MEANDRE05F_SIGMODE
  digitalWrite( TOGGLE_OUT, (_currentMode == SIGMODE_MEANDRE || _currentMode == SIGMODE_MEANDRE05F) ? HIGH : LOW );
#else
  digitalWrite( TOGGLE_OUT, (_currentMode == SIGMODE_MEANDRE) ? HIGH : LOW );
#endif
  _delay_ms(100);
}
//---------------------

void steppedSweepGenerator( void ) {  // for Ch#0 only; uses Ch#0 signal settings: signal mode and amplitude setting for square wave
  lcd.blink();  //blinking cursor to indicate that the steppedSweepGenerator mode is running
    
  bool goUp = false;
  if( settings.frequency[1] >= settings.frequency[0] ) goUp = true;

  uint8_t freqLog10;
  unsigned long _sweepFrequency = settings.frequency[0];  // Ch#0 frequency value = start of the sweep range
  while( (goUp && _sweepFrequency < settings.frequency[1]) || (!goUp && _sweepFrequency > settings.frequency[1] ) ) {  // while in a range
        
    freqLog10 = (uint8_t)log10( _sweepFrequency );  // magnitude/decimal power of the sweep frequency value; used to define logarithmic steps
    if( freqLog10 > 0 ) freqLog10--;  // define the power of 10 for the sweep step to imitate logarithmic increase/decrease as 0.1*sweepFrequency
    
    if( goUp ) 
      _sweepFrequency += power(10, freqLog10 ); // increase frequency value by 0.1*frequency
    else 
      _sweepFrequency -= power(10, freqLog10 ); // decrease frequency value by 0.1*frequency
    
    sigGen.setFrequency( MD_AD9833::CHAN_0, _sweepFrequency ); // set the sweep frequency; frequency values defined in Ch#0 and Ch#1 are not altered
    
    displayFrequency( _sweepFrequency ); // update Frequency value on the display
    
    if( !digitalRead( BUTTON_OK ) ) break;  // press OK button to cancel the sweep cycle
    _delay_ms( STEPPED_SWEEP_DELAY );  // delay between sweep steps
  }
  frequencyUpdatedFlag = true;  // get back fo Ch#0 settings
  updateDisplayFlag = true;  // update the display
  _delay_ms( 3000 );  
  lcd.noBlink();  // stop blinking cursor - end of a sweep cycle
}
