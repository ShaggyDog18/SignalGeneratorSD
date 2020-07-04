/*=================== Signal Generator -SD mod- based on AD9833 Module
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
- More convinient and fast way of input frequency by rotary encoder (if you still like the old way, comment `#define NEW_WAY_INPUT_FREQ`): 
  - continuous input: if reach either '9' or '0' in a digit position, then it jumps over to the senior digit and decreases/encreases it.
  - fast input: if fast encoder rotation is detected, then it increases/decreases ten times of a current digit position
  
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
- `#define NEW_WAY_INPUT_FREQ` - new faster and more convinient way of input frequency by encoder; if you like the old way - comment it!
- `#define SWAP_ENCODER_DIRECTION` - swap encoder pins if encoder is detecting rotation incorrectly
- `#define LCD_I2C_ADDRESS 0x3f` - may need to change I2C address of the display module
- `#define USE_PHASE` - use Phase instead of the FREQ register; never used nor tested :-) Sorry, no guarantee it works...

## Navigation:

- Single button click at the default screen -> go to SETTING_MODE.
- Encoder rotation any direction -> switch from one input parameter to another in a loop; a current input parament is highlighted by underline cursor.
- Encoder rotation at input frequency -> change value of the current digit (underlined by a cursor) of the frequency value.
- Fast encoder rotartion at input frequency -> change value of more significant digit than current digit position (only if `NEW_WAY_INPUT_FREQ` is defined).
- Single button click at active input parameter -> change parameter value. The new value is immediately applied.
- Long button press anywhere in settings -> save and apply the current value of a parameter and jump to operation screen (blinking cursor at the "f=" letter).

#### If EEPROM is enabled:

- Press and hold button during start up -> reset settings to default (just for the current session, does not write default settings to EEPROM).
Hold the button until display's backlight starts blinking. Backlight will blink 3 times to confirm the reset.   
- Double click anywhere -> save settings to EEPROM. Display backlight will blink 2 times to confirm.
- At the first launch of the firmware `Error:CRC EEPROM` will be shown because no settings are in the EEPROM yet. Settings will be automatically set to default and saved to EEPROM. The error will not appear any more.

## Known Feature

AD98333 module generates meandre (square signal) at its VCC level. So, if VCC bus is +5V, then amplitude of the square sugnal is 5V. 
In some cases a signal of 3.3v may be required. 
There are several solution:
1. use 3.3v power bus for entire solutoin.
2. add 3.3v voltage regulator and switch between 5v and 3.3v power bus for entire setup (plain rough solution).
3. add 3.3v voltage regulator and switch power bus of an output buffer (deployed). 

So, I deployed the option#3: added an output cascade/buffer for meander signal only based on Schmitt-trigger (for example, 74LVC1G14) which is connected right to the AD9833 out pin, and flip its power bus between 5v and 3.3v from firmware (menu). 
To activate the feature in the firmware uncomment: **#define ENABLE_VOUT_SWITCH**

**Note:** The switch may also be  a simple mechanical 2-position toggle switch!

Schematic of the "ouput buffer" based on the Schmitt-trigger 74LVC1G14 at [EasyEDA](https://easyeda.com/Sergiy/switch-5-3-3v-power-bus)

*/
// --- COMPILER CONFIG ----
#define USE_MD_LIB      // new bug-free library
#define GRAPH_ICONS     // use graphical icons for sign representation on display
#define ENABLE_EEPROM   // sacve settings to EEPROM, recover them at startup
#define ENABLE_MEANDRE05F_SIGMODE   // compatible with the new MD_AD9833 library only
//#define ENABLE_VOUT_SWITCH  // developped an extra output circuit that switch meander logic level of eather 3.3v or 5v; switched from menu by pin 6
#define NEW_WAY_INPUT_FREQ  // input frequency with jumping to the next digit position; Fast rotation adds 10 times more
//#define SWAP_ENCODER_DIRECTION  // swap if encoder is rotating in the wrong direction
//#define USE_PHASE    //Uncomment the line below if you want to change the Phase instead of the FREQ register // never use or tested
//---------------- Config Checkup --------
#ifndef USE_MD_LIB 
  #undef ENABLE_MEANDRE05F_SIGMODE // compatible with the new MD_AD9833 library only
#endif
//----------------

#include <avr/delay.h>
#include <LiquidCrystal_I2C.h>
#include <RotaryEncoder.h>
#include <GyverButton.h>

#ifdef ENABLE_EEPROM
  #include <avr/eeprom.h>
#endif

#ifdef USE_MD_LIB
  #include <MD_AD9833.h>  // modified library. Midifications are commented with ShaggyDog keyword
#else
  #include "AD9833.h"
#endif

#ifdef SWAP_ENCODER_DIRECTION
  #define DT 2  // encoder
  #define CLK 3 // encder
#else
  #define DT 3  // encoder
  #define CLK 2 // encder
#endif

#define BUTTON_OK  4
#define TOGGLE_OUT 5      // switch the out off if toggle to Meandre/CLK which is 5V out to protect the output from saturation (under development)
#define TOGGLE_CLK_VOUT 6 // switch output sine voltage between 3.3v (default) and 5v 
#define FSYNC_PIN  10

// LCD Settings
#define LCD_I2C_ADDRESS 0x3f	// 0x27 - alternative i2c address
#define LCD_DISP_COLS   16
#define LCD_DISP_ROWS   2

#define FREQ_N_DIGITS 8  // number of digits for frequency to display

LiquidCrystal_I2C lcd( LCD_I2C_ADDRESS, LCD_DISP_COLS, LCD_DISP_ROWS ); // LCD Initialise
RotaryEncoder encoder( DT, CLK );     // Initialise the encoder on pins 2 and 3 (interrupt pins)
GButton buttonOK( BUTTON_OK );

#ifdef USE_MD_LIB
  MD_AD9833 sigGen( FSYNC_PIN );  // Hardware SPI
#else  
  AD9833 sigGen( FSYNC_PIN, 25000000 ); // Initialise our AD9833 with FSYNC pin and a master clock frequency of 25MHz
#endif

// Variables used to input data and walk through menu
#ifdef ENABLE_VOUT_SWITCH
  unsigned char settingInputPos[] = {0, 14, 20, 23, 29};
#else
  unsigned char settingInputPos[] = {0, 14, 20, 29};
#endif

// Define data types
enum menuState_t : uint8_t {
  DEFAULT_SCREEN = 0,
  SETTING_MENU,
  FREQUENCY_SETTING, 
#ifdef USE_PHASE  
  PHASE_SETTING,
#endif
} menuState = DEFAULT_SCREEN;

enum sigmode_t : uint8_t {
  SIGMODE_SINE = 0,
  SIGMODE_TRIANGLE,
  SIGMODE_MEANDRE,
#ifdef ENABLE_MEANDRE05F_SIGMODE
  SIGMODE_MEANDRE05F,
#endif
  NUMBER_SIGMODES
};

enum inputPositions_t : uint8_t {
  IP_FREQUENCY = 0,
  IP_ONOFF,
  IP_CHANNEL,
  #ifdef ENABLE_VOUT_SWITCH
    IP_VOUT_SWITCH,
  #endif
  IP_MODE,
  NUMBER_INPUT_POSITIONS
};

struct settings_t {
  unsigned long frequency[2]; 
  sigmode_t currentMode[2]; // default value = SIGMODE_SINE
  bool currentChannel;      // Default FREQ register/Channel#0 <> false); TRUE for FREQReg/Channel#1
  bool toggleCLKoutVolt;    // 3.3v CLK signal by default
  uint8_t checkSum;
#ifdef ENABLE_EEPROM
  } settings;
#else
  } settings = { 1000UL, 1000UL, SIGMODE_SINE, SIGMODE_SINE, false, false, 0 }; // dafault values
#endif

int digitPos = 0;
const unsigned long maxFrequency = 12500000; // according to AS9833 Datasheet the max frequency is 12.5 MHz
#ifdef USE_PHASE
const unsigned int maxPhase = 4095; // Only used if you enable PHASE setting instead of FREQ register
uint16_t phase = 0;
#endif

// flags
bool defaultScreenFirstPass = true;
bool updateDisplay = true;
bool signalOn = true;

// Variables used to store phase, frequency
unsigned long frequency = settings.frequency[0];
uint8_t cursorInputPos = IP_FREQUENCY;

#ifndef GRAPH_ICONS
// LCD mode depicting constants
  #ifdef ENABLE_MEANDRE05F_SIGMODE
    const String mode[] = {"SIN", "TRI", "CLK", "C/2"};
  #else
    const String mode[] = {"SIN", "TRI", "CLK"};
  #endif
#endif

// Greek PHI symbol for phase shift
// Only used if you enable PHASE setting instead of FREQ register
#ifdef USE_PHASE
const uint8_t phi[] = {
0b01110,0b00100,0b01110,0b10101,0b10101,0b01110,0b00100,0b01110 };
#else
const uint8_t ff[] = { // "FF" sign of indication of "OFF" state 
0b11100,0b10000,0b11000,0b10111,0b00100,0b00110,0b00100,0b00000 };
#define FF (lcd.write( 0 ))
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

const uint8_t triangle[3][8] = {
0b00000,0b00000,0b00000,0b10000,0b01000,0b00100,0b00010,0b00001,
0b00000,0b00000,0b00001,0b00010,0b00100,0b01000,0b10000,0b00000,
0b00000,0b10000,0b01000,0b00100,0b00010,0b00001,0b00000,0b00000 };
#endif


void setup() {
  // Initialise the LCD, start the backlight and print a "bootup" message for two seconds
  lcd.begin();
  
#ifdef USE_PHASE  
  lcd.createChar(0, phi); // Custom PHI char for LCD
#else
   lcd.createChar(0, ff); // Custom FF char for LCD
#endif

#if defined( GRAPH_ICONS ) or defined( ENABLE_VOUT_SWITCH )
  lcd.createChar(1, meander); // Custom meandre char for LCD
#endif
#ifdef GRAPH_ICONS
  lcd.createChar(2, sine[0]); // Custom sine 1/3 char for LCD
  lcd.createChar(3, sine[1]); // Custom sine 2/3 char for LCD
  lcd.createChar(4, sine[2]); // Custom sine 3/3 char for LCD
  lcd.createChar(5, triangle[0]); // Custom triangle 1/3 char for LCD
  lcd.createChar(6, triangle[1]); // Custom triangle 2/3 char for LCD
  lcd.createChar(7, triangle[2]); // Custom triangle 3/3 char for LCD
#endif  

  // Launch Screen
  lcd.home();
  lcd.backlight();
  lcd.print(F("Signal Generator"));
  lcd.setCursor(0, 1);
  lcd.print(F("-ShaggyDog 2020-"));
  _delay_ms(1500);

#ifdef USE_MD_LIB
  sigGen.begin(); // it also set up all default values
#endif

#ifdef ENABLE_EEPROM
  bool resetSettings = false;
  if( readSettingsFromEEPROM() ) {
    applyCurrentSettings();// set settings to the AD module that were read from EEPROM
  } else {
    lcd.setCursor(0, 0);
    lcd.print(F("Error:CRC EEPROM"));
    lcd.setCursor(0, 1);
    lcd.print(F("Reset to default"));  // default values will be used
    _delay_ms(3000);
    resetSettings = true;
    lcd.clear();
  }

  if( ! digitalRead( BUTTON_OK ) || resetSettings ) { // if button is pressed at start up then settings will be reset to defaul
    lcd.setCursor(0, 0);
    lcd.print(F("Reset to default"));
    settings.frequency[0] = settings.frequency[1] = 1000UL;
    settings.currentMode[0] = settings.currentMode[1] = SIGMODE_SINE;
    settings.toggleCLKoutVolt = false;
    settings.currentChannel = false; 
    blinkDisplayBacklight(3);
    applyCurrentSettings();
    while( !digitalRead( BUTTON_OK ) ) ; // wait for button to be released
    lcd.clear();
  }
  if( resetSettings) writeSettingsToEEPROM();
#endif
  
#ifndef USE_MD_LIB
  // Initialise the AD9833 with 1KHz sine output, no phase shift for both
  // registers and remain on the FREQ0 register/channel
  // sigGen.lcdDebugInit(&lcd);
  sigGen.reset(1);
  sigGen.setFPRegister(0);
  sigGen.setFreq(settings.frequency[0]);
  sigGen.setPhase(0);
  sigGen.setFPRegister(1);
  sigGen.setFreq(settings.frequency[1]);
  sigGen.setPhase(0);
  // set current channel and mode
  sigGen.setFPRegister((uint8_t)settings.currentChannel);
  sigGen.mode( settings.currentMode[(uint8_t)settings.currentChannel] );
  sigGen.reset(0);
#endif

  // Set pins A and B for encoder as interrupts
  attachInterrupt(digitalPinToInterrupt(DT),  encoderTickISR, CHANGE);
  attachInterrupt(digitalPinToInterrupt(CLK), encoderTickISR, CHANGE);
  // Initialise pin as input with pull-up enabled and debounce variable for

  #ifdef ENABLE_VOUT_SWITCH
    pinMode(TOGGLE_CLK_VOUT, OUTPUT);
    toggleCLKvolt( settings.toggleCLKoutVolt );  // 3.3v by default (LOW signal output)
    
    pinMode(TOGGLE_OUT, OUTPUT);  // decrease amplitude of CLK signal to 650 mV by toggling an output divider (under development)
    toggleOut( settings.currentMode[(uint8_t)settings.currentChannel] );     // under development
  #endif

  // Set Cursor to initial possition, reset the screen
  lcd.clear();
} // setup()
//---------------------------


// ISR routine for Encoder
void encoderTickISR( void ){
  encoder.tick(); 
}


void loop() {
  buttonOK.tick();   // Check if encoder button has been pressed
  // process button clicks
  if( buttonOK.isSingle() ) processSingleClick(); // single click
  if( buttonOK.isHolded() ) processLongPress();   // long press
  
  #ifdef ENABLE_EEPROM
    if( buttonOK.isDouble() ) { // double click -> write settings to EEPROM
      writeSettingsToEEPROM();
      blinkDisplayBacklight(2); // double blink to confirm operation of writing to EEPROM
    }
  #endif

  processEncoder( encoder.getDirection() );  

  if( updateDisplay ) {   // Update display if needed
    displayFrequency( frequency );
    displayPower( signalOn );

    #ifdef USE_PHASE
      displayPhase( phase );
    #else
      displayCurrentChannel( settings.currentChannel );
    #endif
    
    #ifdef ENABLE_VOUT_SWITCH
      displayCLKoutVolt( settings.toggleCLKoutVolt, settings.currentMode[(uint8_t)settings.currentChannel] );
    #endif
    
    displayMode( settings.currentMode[(uint8_t)settings.currentChannel] );
    setCursor2inputPosition( cursorInputPos );
    updateDisplay = false;
  }
  menuPostProcessing();
  
} // End loop()
//---------------------


void menuPostProcessing(void) {
  switch( menuState ) {
    case DEFAULT_SCREEN:     // Default state
      if( defaultScreenFirstPass ) {
        lcd.setCursor(0, 0);
        lcd.noCursor();
        lcd.blink();
        defaultScreenFirstPass = false;
      }
      break;

    case FREQUENCY_SETTING:    // Frequency setting
      // Set the blinking cursor on the digit you currently modify
      lcd.setCursor(9 - digitPos, 0);
      break;

#ifdef  USE_PHASE // never tested
    case PHASE_SETTING:    // Phase setting
      lcd.setCursor(5 - digitPos, 1);
      break;
#endif      
  } // switch( menuState )
} // menuPostProcessing()


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
            #ifdef USE_MD_LIB
              sigGen.setModeSD(signalOn ? MD_AD9833::MODE_ON : MD_AD9833::MODE_OFF); 
            #else
              sigGen.sleep( signalOn ? 3 : 0); // Both DAC and clock turned ON
                                             // DAC and clock are turned OFF
            #endif
            break;
            
#ifndef USE_PHASE          // If USE_PHASE has not been set
          case IP_CHANNEL: // select channel 0 or 1
            settings.currentChannel = !settings.currentChannel; // flip bool value
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
          case IP_MODE: // Change the mode (sine/triangle/clock)
            ++(*((uint8_t*)&(settings.currentMode[(uint8_t)settings.currentChannel])));  // increment currentMode value :-)
            if( settings.currentMode[(uint8_t)settings.currentChannel] == NUMBER_SIGMODES ) settings.currentMode[(uint8_t)settings.currentChannel] = SIGMODE_SINE;
            // switch output to direct out
            toggleOut( settings.currentMode[(uint8_t)settings.currentChannel] );
            setADsignalMode( settings.currentMode[(uint8_t)settings.currentChannel] );
            break;
      } // switch( cursorInputPos )
      updateDisplay = true;  
      break;
             
    case FREQUENCY_SETTING:  
      // Each button press will either enable to change the value of another digit
      // or if all digits have been changed, to apply the setting
      if (digitPos < FREQ_N_DIGITS-1) {
        digitPos++;
      } else {
        digitPos = 0;
        setADfrequency( settings.currentChannel, settings.frequency[(uint8_t)settings.currentChannel] );
        jump2settingMenu();
      }        
      break;
        
#ifdef  USE_PHASE // never tested
    case PHASE_SETTING:    // Phase setting
      if (digitPos < 3) { 
        digitPos++;
      } else {
        digitPos = 0;
        #ifdef USE_MD_LIB
          sigGen.setPhase( settings.currentChannel ? MD_AD9833::CHAN_1 : MD_AD9833::CHAN_0, phase );
        #else
          sigGen.setPhase(phase);
        #endif
        jump2settingMenu();
      }
      break;      
#endif    
  }  // switch( menuState )
}  // processSingleClick(void)


void processLongPress(void) {
  switch( menuState ) {
  case DEFAULT_SCREEN: 
    // do nothing
    break;

  case SETTING_MENU:
    menuState = DEFAULT_SCREEN;
    cursorInputPos = IP_FREQUENCY;
    defaultScreenFirstPass = true;
    break;

#ifdef  USE_PHASE // never tested
  case PHASE_SETTING: 
    // setPhase here
    #ifdef USE_MD_LIB
      sigGen.setPhase( settings.currentChannel ? MD_AD9833::CHAN_1 : MD_AD9833::CHAN_0, phase );
    #else
      sigGen.setPhase(phase);
    #endif
    jump2settingMenu();
    break;
#endif

  case FREQUENCY_SETTING: 
    setADfrequency( settings.currentChannel, settings.frequency[(uint8_t)settings.currentChannel] );
    jump2settingMenu();
    break;
  } // switch( menuState )
} //  processLongPress(void)


void setCursor2inputPosition( uint8_t _cursorPosition ) {
static uint8_t lastCursorPos = 0;
  // Move the cursor to display position in case it changed
    unsigned char realPosR = 0;
    unsigned char realPosC;
    if (settingInputPos[_cursorPosition] < LCD_DISP_COLS) {
      realPosC = settingInputPos[_cursorPosition];
    } else {
      realPosC = settingInputPos[_cursorPosition] - LCD_DISP_COLS;
      realPosR = 1;
    }
    lastCursorPos = _cursorPosition;
    lcd.setCursor(realPosC, realPosR);
    lcd.cursor();
} // setCursor2inputPosition()


void processEncoder( RotaryEncoder::Direction _rotaryDirection ) {
  // Depending in which menu state you are the encoder will either change the value of a setting:
  //-+ frequency, change between FREQ0 and FREQ1 register (or -+ phase), 
  // or it will change the cursor position
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
          updateDisplay = true;
          settings.frequency[(uint8_t)settings.currentChannel] = frequency;
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
          if (newFrequency <= maxFrequency && dispDigit < 9) {
            frequency += power(10, digitPos);
            updateDisplay = true;
            settings.frequency[(uint8_t)settings.currentChannel] = frequency;
          }
        } break; 
        #endif

        
#ifdef USE_PHASE
      case PHASE_SETTING: {
          // if USE_PHASE has been defined, changes in the encoder will vary the phase
          // value (upto 4096)
          // A better implementation would be to use increment of pi/4 or submultiples of
          // pi where 2pi = 4096
          unsigned long newPhase = phase + power(10, digitPos);
          unsigned char dispDigit =
            phase % power(10, digitPos + 1) / power(10, digitPos);
          if (newPhase < maxPhase && dispDigit < 9) {
            phase += power(10, digitPos);
            updateDisplay = true;
          }
        } break;
#endif        
      }  // switch( menuState )
      break;

    case RotaryEncoder::Direction::COUNTERCLOCKWISE:
    case RotaryEncoder::Direction::FAST_CCW:
      switch( menuState ) {
      case SETTING_MENU:
          if( cursorInputPos == IP_FREQUENCY )		// changing input positions in a loop
            cursorInputPos = NUMBER_INPUT_POSITIONS-1;
          else {
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
          updateDisplay = true; 
          settings.frequency[(uint8_t)settings.currentChannel] = frequency;
          break;
          
        #else // old way of input frequency
        {
          unsigned long newFrequency = frequency + power(10, digitPos);
          unsigned char dispDigit = frequency % power(10, digitPos + 1) / power(10, digitPos);
          if (newFrequency > 0 && dispDigit > 0) {
            frequency -= power(10, digitPos);
            updateDisplay = true;
            settings.frequency[(uint8_t)settings.currentChannel] = frequency;
          }         
        } break; 
        #endif
          

          
#ifdef USE_PHASE
      case PHASE_SETTING: {
          // if USE_PHASE has been defined, changes in the encoder will vary the phase
          // value (upto 4096)
          // A better implementation would be to use increment of pi/4 or submultiples of
          // pi where 2pi = 4096
          unsigned long newPhase = phase + power(10, digitPos);
          unsigned char dispDigit = phase % power(10, digitPos + 1) / power(10, digitPos);
          if (newPhase > 0 && dispDigit > 0) {
            phase -= power(10, digitPos);
            updateDisplay = true;
          }
        } break;
#endif        
      }  // switch( menuState )
      break;
  }  // switch( rotaryDirection )
}  // processingEncoder()


void jump2settingMenu(void) {
  menuState = SETTING_MENU;
  cursorInputPos = IP_FREQUENCY;
  lcd.noBlink();
  lcd.setCursor(0,0);
  lcd.cursor();
} // jump2settingMenu()



// old way (power and multiplications=10102/449 : new=10076/449
// Function to display the current frequency in the top left corner
void displayFrequency( unsigned long _frequencyToDisplay ) {
  lcd.setCursor(0,0); 
  lcd.print( F("f=") );
  
  uint8_t dispBuffer[FREQ_N_DIGITS] = {0,0,0,0,0,0,0,0};
  for( uint8_t i=0; i<FREQ_N_DIGITS; i++ ) {
    dispBuffer[i] = _frequencyToDisplay % 10;
    _frequencyToDisplay /= 10;
    if( _frequencyToDisplay == 0 ) break;
  }
  for( int8_t i=FREQ_N_DIGITS-1; i >= 0; i-- ) lcd.print(dispBuffer[i]);
/*
 *
// old way - too many power 10X operations
  for (int i = 7; i >= 0; i--) {
    unsigned char dispDigit = _frequencyToDisplay / power(10, i);
    lcd.print(dispDigit);
    _frequencyToDisplay -= dispDigit * power(10, i);
  }
*/
  lcd.print( F("Hz") );
} // displayFrequency()


// Function to display power state (ON/OFF) in the top right corner
void displayPower( bool _signalOn ) {
  #ifdef USE_PGHASE
    lcd.setCursor(13, 0);
    lcd.print( _signalOn ? F("ON ") : F("OFF") );
  #else
    lcd.setCursor(14, 0);
    if( _signalOn )
      lcd.print( F("ON") );
    else {
      lcd.print(F("O")); FF;   //lcd.write( 0 ); //FF sign
    }
  #endif
}


// Function to display the mode in the bottom right corner
void displayMode( sigmode_t _currentMode ) {
  lcd.setCursor(13, 1);
#ifdef GRAPH_ICONS
  switch( _currentMode ) {
  case SIGMODE_SINE:     lcd.write( 2 );lcd.write( 3 );lcd.write( 4 );break;
  case SIGMODE_TRIANGLE: lcd.write( 5 );lcd.write( 6 );lcd.write( 7 );break;
  case SIGMODE_MEANDRE:  lcd.write( 1 );lcd.write( 1 );lcd.write( 1 );break;
  #ifdef ENABLE_MEANDRE05F_SIGMODE
    case SIGMODE_MEANDRE05F: lcd.write( 1 );lcd.print(F("/2"));break;
  #endif
  }
#else
  lcd.print(mode[(uint8_t)_currentMode ]);
#endif
} // displayMode()


// Function to display the mode in the bottom left corner
// Only used if you enable PHASE setting instead of FREQ register
void displayPhase( unsigned int _phaseToDisplay ) {
  lcd.setCursor(0, 1);
  lcd.write(0);
  lcd.print( F("=") );
  for( int i = 3; i >= 0; i-- ) {
    unsigned int dispDigit = _phaseToDisplay / power(10, i);
    lcd.print(dispDigit);
    _phaseToDisplay -= dispDigit * power(10, i);
  }
} // displayPhase()


// Function to display the FREQ register (either 0 or 1) in the bottom left
// corner
void displayCurrentChannel( bool _channel ) {
  lcd.setCursor(0, 1);
  lcd.print( F("CHAN") );
  lcd.print((uint8_t)_channel);
} // displayCurrentChannel()


void displayCLKoutVolt( bool _toggleCLKoutVolt, sigmode_t _currentMode ) {
  lcd.setCursor(7, 1);
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

void toggleCLKvolt( bool _toggleOutVolt ) {
 digitalWrite( TOGGLE_CLK_VOUT, _toggleOutVolt ? HIGH : LOW ); // HIGH = 5v, 3.3v
}


unsigned long power(int a, int b) {
  unsigned long res;
  if (b == 0) {
    res = 1;
  } else {
    res = a;
    for (int i = 0; i < (b - 1); i++) {
      res *= a;
    }
  }
  return res;
}


#ifdef USE_MD_LIB
void setADsignalMode( sigmode_t _currentMode ) {
  switch( _currentMode ) {
  case SIGMODE_SINE:     sigGen.setModeSD( MD_AD9833::MODE_SINE ); break;
  case SIGMODE_TRIANGLE: sigGen.setModeSD( MD_AD9833::MODE_TRIANGLE ); break;
  case SIGMODE_MEANDRE:  sigGen.setModeSD( MD_AD9833::MODE_SQUARE1 ); break;
  #ifdef ENABLE_MEANDRE05F_SIGMODE
    case SIGMODE_MEANDRE05F: sigGen.setModeSD( MD_AD9833::MODE_SQUARE2 ); break;
  #endif
  }
} // setADsignalMode()

void setADfrequency( bool _channel, unsigned long _frequency ) {
  sigGen.setFrequency( _channel ? MD_AD9833::CHAN_1 : MD_AD9833::CHAN_0, _frequency );
} // setADfrequency()

void setADchannel( bool _channel ) {
  if( _channel ) {
    sigGen.setActiveFrequency( MD_AD9833::CHAN_1 );
    frequency = settings.frequency[1];
    //frequency = sigGen.getFrequency( MD_AD9833::CHAN_1 );  // alternative way is to read frequency value from AD9833 module
  } else {
    sigGen.setActiveFrequency( MD_AD9833::CHAN_0 );
    frequency = settings.frequency[0];
  }
} // setADchannel()

#else // old error-fixed library

void setADsignalMode( sigmode_t _currentMode ) {
  sigGen.mode( _currentMode );
} // setADsignalMode()

void setADfrequency(  bool _channel, unsigned long _frequency ) {
  sigGen.setFPRegister((uint8_t) _channel );
  sigGen.setFreq( _frequency );
} // setADfrequency()

void setADchannel( bool _channel ) {
  if( _channel ) {
    sigGen.setFPRegister(1);
    frequency = settings.frequency[1];
  } else {
    sigGen.setFPRegister(0);
    frequency = settings.frequency[0];
  }
} // setADchannel()
#endif


void blinkDisplayBacklight( uint8_t nBlinks ) {
  for( uint8_t i=0; i<nBlinks; i++ ) {
    lcd.noBacklight(); _delay_ms(300);
    lcd.backlight();   _delay_ms(300);
  }
} // blinkDisplayBacklight()


void applyCurrentSettings(void) {
  setADfrequency( 0, settings.frequency[0] );
  setADfrequency( 1, settings.frequency[1] );
  // set current mode
  setADsignalMode( settings.currentMode[(uint8_t)settings.currentChannel] ); 
  setADchannel( settings.currentChannel );
} // applyCurrentSettings()


#ifdef ENABLE_EEPROM
#define ADDRESS_SHIFT 0
bool readSettingsFromEEPROM(void) { // read settings from EEPROM; return true if CRC check sum is OK
  eeprom_read_block( (void *)&settings, ADDRESS_SHIFT, sizeof(settings_t) );  //eeprom_read_block (void *__dst, const void *__src, size_t __n);
  // check the CRC8 chech sum of the memory block read from EEPROM
  uint8_t checkSum = crc8array( (uint8_t *) &settings, sizeof(settings_t)-1 ); // last array member is a byte of the CRC check sum, so passing one less byte
  if( checkSum == settings.checkSum ) return true;
  return false;
} // readSettingsFromEEPROM()


void writeSettingsToEEPROM(void) { // write settings to EEPROM
  // calculate the CRC8 chech sum of settings structure and write it to EEPROM along with settings
  uint8_t checkSum = crc8array( (uint8_t *)&settings, sizeof(settings_t)-1 ); // last array member is a byte of the CRC check sum, so passing one less byte
  settings.checkSum = checkSum;
  eeprom_update_block( (void *)&settings, ADDRESS_SHIFT, sizeof(settings_t) );   // write settings to EEPROM: eeprom_update_block(const void *__src, void *__dst, size_t __n)
} // writeSettingsToEEPROM()


uint8_t crc8array( uint8_t data[], uint8_t dataSize ) {   // calculate CRC8 check sum for the memory block
  uint8_t checkSum = 0xFF;  // initial crc check sum value (see crc8 algorythm in Wikipedia)

  dataSize--; // decrease one time so that do not decrease it by 1 in the below for() checking the condition
  for( uint8_t i=0; i < dataSize; i++ ) {  // CRC8 for all bytes in the memory block
    checkSum = crc8update( checkSum, data[i] );  
  }
  return checkSum;
} //crc8array


uint8_t crc8update( uint8_t crc, uint8_t data ) {  // standard calculation from Wikipedia
  crc ^= data;
  for( uint8_t i = 0; i < 8; i++ ) crc = (crc & 0x80) ? (crc << 1)^0x31 : crc << 1;
  return crc;
}
#endif


// under development
void toggleOut( sigmode_t _currentMode ) {
#ifdef ENABLE_MEANDRE05F_SIGMODE
  digitalWrite( TOGGLE_OUT, (_currentMode == SIGMODE_MEANDRE || _currentMode == SIGMODE_MEANDRE05F) ? HIGH : LOW );
#else
  digitalWrite( TOGGLE_OUT, (_currentMode == SIGMODE_MEANDRE) ? HIGH : LOW );
#endif
  _delay_ms(100);
}
