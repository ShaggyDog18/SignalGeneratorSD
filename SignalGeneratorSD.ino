/*=================== Signal Generator-SD mod- based on AD9833 Module
Author: Cezar Chirila
URL: https://www.allaboutcircuits.com/projects/how-to-DIY-waveform-generator-analog-devices-ad9833-ATmega328p/

Re-coded by ShaggyDog18@gmail.com
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
- #define ENABLE_VOUT_SWITCH - developed an extra output circuit that switch meander logic level of either 3.3v or 5v; switched from menu by pin 6

At first start EEPROM: RC Error will be shown. Will automatically reset settings to default and write them to EEPROM.

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

*/
// --- COMPILER CONFIG ----
#define USE_MD_LIB      // new bug-free library
#define GRAPH_ICONS     // use graphical icons for sign representation on display
#define ENABLE_EEPROM   // sacve settings to EEPROM, recover them at startup
//#define SWAP_ENCODER_DIRECTION  // swap if encoder is rotating in the wrong direction
//#define ENABLE_VOUT_SWITCH  // developped an extra output circuit that switch meander logic level of eather 3.3v or 5v; switched from menu by pin 6
//#define USE_PHASE    //Uncomment the line below if you want to change the Phase instead of the FREQ register // never use or tested
//----------------

#include <avr/delay.h>
#include <LiquidCrystal_I2C.h>
#include <RotaryEncoder.h>
#include <GyverButton.h>

#ifdef ENABLE_EEPROM
  #include <avr/eeprom.h>
#endif

#ifdef USE_MD_LIB
  #include <MD_AD9833.h>  // modified library, do not update!!! Midifications are commented with ShaggyDog keyword
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
#define TOGGLE_OUT 5      // switch out off is toggle to Meandre/CLK which is 5V out
#define TOGGLE_CLK_VOUT 6 // switch output sine voltage between 3.3v (default) / 5v 
#define FSYNC_PIN  10

// LCD Settings
#define LCD_I2C_ADDRESS 0x3f
#define LCD_DISP_COLS   16
#define LCD_DISP_ROWS   2

LiquidCrystal_I2C lcd( LCD_I2C_ADDRESS, LCD_DISP_COLS, LCD_DISP_ROWS ); // LCD Initialise
RotaryEncoder encoder( DT, CLK );     // Initialise the encoder on pins 2 and 3 (interrupt pins)
GButton buttOK( BUTTON_OK );

#ifdef USE_MD_LIB
  MD_AD9833 sigGen( FSYNC_PIN );  // Hardware SPI
#else  
  AD9833 sigGen( FSYNC_PIN, 25000000 ); // Initialise our AD9833 with FSYNC pin = 10 and a master clock frequency of 24MHz
#endif

// Variables used to input data and walk through menu
#ifdef ENABLE_VOUT_SWITCH
  unsigned char settingInputPos[] = {0, 13, 20, 23, 29};
#else
  unsigned char settingInputPos[] = {0, 13, 20, 29};
#endif

// Define data types
enum menuState_t : uint8_t {
  DEFAULT_SCREEN = 0,
  SETTING_MODE,
  FREQUENCY_SETTING, 
#ifdef USE_PHASE  
  PHASE_SETTING,
#endif
} menuState = DEFAULT_SCREEN;

enum outmode_t : uint8_t {
  OUTMODE_SINE = 0,
  OUTMODE_TRIANGLE,
  OUTMODE_MEANDRE
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
  outmode_t currentMode[2]; // default value = OUTMODE_SINE
  bool currentChannel;      // Default FREQ register/Channel#0 <> false); TRUE for FREQReg/Channel#1
  bool toggleCLKoutVolt;    // 3.3v CLK signal by default
  uint8_t checkSum;
#ifdef ENABLE_EEPROM
  } settings;
#else
  } settings = { 1000UL, 1000UL, OUTMODE_SINE, OUTMODE_MEANDRE, false, false, 0 }; // dafault values
#endif

int digitPos = 0;
const unsigned long maxFrequency = 12500000; // according to AS9833 Datasheet the max frequency is 12.5 MHz
const unsigned int maxPhase = 4095; // Only used if you enable PHASE setting instead of FREQ register

// button activity flags
bool buttonPressed = false;
bool buttonLongPress = false;

// flags
bool defaultScreenFirstPass = true;
bool updateDisplay = true;
bool signalOn = true;

// Variables used to store phase, frequency
unsigned long frequency = settings.frequency[0];
uint16_t phase = 0; // Only used if you enable PHASE setting instead of FREQ register
uint8_t cursorInputPos = IP_FREQUENCY;

#ifndef GRAPH_ICONS
// LCD mode depicting constants
const String mode[] = {"SIN", "TRI", "CLK"};
#endif

// Greek PHI symbol for phase shift
// Only used if you enable PHASE setting instead of FREQ register
#ifdef USE_PHASE
const uint8_t phi[8] = {
0b01110,0b00100,0b01110,0b10101,0b10101,0b01110,0b00100,0b01110 };
#endif

#if defined( GRAPH_ICONS ) or defined( ENABLE_VOUT_SWITCH )
const uint8_t meander[8] = {
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
  // Define Encoder button
  // pinMode(BUTTON_OK, INPUT_PULLUP);
  
  // Initialise the LCD, start the backlight and print a "bootup" message for two seconds
  lcd.begin();
  lcd.backlight();
#ifdef USE_PHASE  
  lcd.createChar(0, phi ); // Custom PHI char for LCD
#endif
#if defined( GRAPH_ICONS ) or defined( ENABLE_VOUT_SWITCH )
  lcd.createChar(1, meander ); // Custom meandre char for LCD
#endif
#ifdef GRAPH_ICONS
  lcd.createChar(2, sine[0] ); // Custom sine 1/3 char for LCD
  lcd.createChar(3, sine[1] ); // Custom sine 2/3 char for LCD
  lcd.createChar(4, sine[2] ); // Custom sine 3/3 char for LCD
  lcd.createChar(5, triangle[0] ); // Custom triangle 1/3 char for LCD
  lcd.createChar(6, triangle[1] ); // Custom triangle 2/3 char for LCD
  lcd.createChar(7, triangle[2] ); // Custom triangle 3/3 char for LCD
#endif  

  // Launch Screen
  lcd.home();
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
    settings.currentMode[0] = settings.currentMode[1] = OUTMODE_SINE;
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
  sigGen.setPhase(phase);
  sigGen.setFPRegister(1);
  sigGen.setFreq(settings.frequency[1]);
  sigGen.setPhase(phase);
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
  lcd.setCursor(0, 0);
  lcd.blink();
  //updateDisplay = true;
} // setup()
//---------------------------



// ISR routine for Encoder
void encoderTickISR( void ){
  encoder.tick(); 
}


void loop() {
  buttOK.tick();   // Check to see if encoder button has been pressed
  if( buttOK.isSingle() ) buttonPressed = true;
  if( buttOK.isHolded() ) buttonLongPress = true;
  
  #ifdef ENABLE_EEPROM
    if( buttOK.isDouble() ) { // write settings to EEPROM
      writeSettingsToEEPROM();
      blinkDisplayBacklight(2);
    }
  #endif
  
  // We are using the variable menuState to know where we are in the menu and
  // what to do in case the button is pressed or increment/drecrement via the encoder
  // Enter setting mode if the button has been pressed and display underline 
  // cursor under input options (menuState 0)
  // Pick a setting (menuState 1); Change that particular setting and save settings 

  menuProcessing();
  buttonPressed = buttonLongPress = false;

  encoderProcessing( encoder.getDirection() );

  if( updateDisplay ) {   // Update display if needed
    displayFrequency( frequency );
    displayPower( signalOn );

    #ifdef USE_PHASE
      displayPhase( phase );
    #else
      displaycurrentChannel( settings.currentChannel );
    #endif
    #ifdef ENABLE_VOUT_SWITCH
      displayCLKoutVolt( settings.toggleCLKoutVolt, settings.currentMode[(uint8_t)settings.currentChannel] );
    #endif
    displayMode( settings.currentMode[(uint8_t)settings.currentChannel] );
    updateDisplay = false;
  }
} // End loop()
//---------------------


void menuProcessing( void ) {
  switch( menuState ) {
    case DEFAULT_SCREEN:     // Default state
        if( defaultScreenFirstPass ) {
          lcd.setCursor(0, 0);
          lcd.noCursor();
          lcd.blink();
          defaultScreenFirstPass = false;
        }
        if( buttonPressed ) jump2settingMenu();
        break;

    case SETTING_MODE: {    // Settings mode
        if( buttonPressed ) { // If the setting in Power just toggle between on and off
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
            setADmode( settings.currentMode[(uint8_t)settings.currentChannel] ); 
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
            if( settings.currentMode[(uint8_t)settings.currentChannel] > OUTMODE_MEANDRE ) settings.currentMode[(uint8_t)settings.currentChannel] = OUTMODE_SINE;
            // switch output to direct out
            toggleOut( settings.currentMode[(uint8_t)settings.currentChannel] );
            setADmode( settings.currentMode[(uint8_t)settings.currentChannel] );
            break;
          } // switch( cursorInputPos )
          updateDisplay = true; 
        } else if( buttonLongPress ) {
            menuState = DEFAULT_SCREEN;
            defaultScreenFirstPass = true;
            cursorInputPos = IP_FREQUENCY;
        }
        setCursor2enterPosition( cursorInputPos );
      } 
      break;

    case FREQUENCY_SETTING:    // Frequency setting
        // Each button press will either enable to change the value of another digit
        // or if all digits have been changed, to apply the setting
        if( buttonPressed ) {
          if (digitPos < 7) {
            digitPos++;
          } else {
            digitPos = 0;
            setADfrequency( settings.currentChannel, settings.frequency[(uint8_t)settings.currentChannel] );
            menuState = SETTING_MODE;
            cursorInputPos = IP_FREQUENCY;
            lcd.setCursor(0,0);
            lcd.cursor();
            break;
          }
          //lcd.setCursor(9 - digitPos, 0);
        } else if( buttonLongPress ) {
          //digitPos = 0;  // preserve last digit input position
          setADfrequency( settings.currentChannel, settings.frequency[(uint8_t)settings.currentChannel] );
          menuState = SETTING_MODE;
          cursorInputPos = IP_FREQUENCY;
          lcd.setCursor(0,0);
          lcd.cursor();
          break;
        }
        // Set the blinking cursor on the digit you can currently modify
        lcd.setCursor(9 - digitPos, 0);
        break;
        
#ifdef  USE_PHASE // never tested
    case PHASE_SETTING:    // Phase setting
        if( buttonPressed ) {
          if (digitPos < 3) { 
            digitPos++;
          } else {
            digitPos = 0;
            menuState = DEFAULT_SCREEN;
            defaultScreenFirstPass = true;
            #ifdef USE_MD_LIB
              sigGen.setPhase( settings.currentChannel ? MD_AD9833::CHAN_1 : MD_AD9833::CHAN_0, phase );
            #else
              sigGen.setPhase(phase);
            #endif
          }
        }
        lcd.setCursor(5 - digitPos, 1);
      break;      
#endif
    
    default: // Just in case we messed something up
      menuState = DEFAULT_SCREEN;
      defaultScreenFirstPass = true;
  }// switch( menuState )
  //lcd.cursor(); 
}// menuProcessing


void setCursor2enterPosition( uint8_t cursotPosition ){
static uint8_t lastCursorPos = 0;
  // Move the cursor position in case it changed
  if(lastCursorPos != cursotPosition) {
    unsigned char realPosR = 0;
    unsigned char realPosC;
    if (settingInputPos[cursotPosition] < LCD_DISP_COLS) {
      realPosC = settingInputPos[cursotPosition];
    } else {
      realPosC = settingInputPos[cursotPosition] - LCD_DISP_COLS;
      realPosR = 1;
    }
    lcd.setCursor(realPosC, realPosR);
    lastCursorPos = cursotPosition;
    lcd.cursor();
  }
}

void encoderProcessing( RotaryEncoder::Direction rotaryDirection ) {
  // Depending in which menu state you are
  // the encoder will either change the value of a setting:
  //-+ frequency, change between FREQ0 and FREQ1 register (or -+ phase), On/Off, mode
  // or it will change the cursor position
  switch( rotaryDirection ) {
  case RotaryEncoder::Direction::CLOCKWISE:
  case RotaryEncoder::Direction::FAST_CW:
    switch( menuState ) {
      case DEFAULT_SCREEN: 
          jump2settingMenu();
          break;
          
      case SETTING_MODE: 
          cursorInputPos++;
          #ifdef ENABLE_VOUT_SWITCH
            if( settings.currentMode[(uint8_t)settings.currentChannel] != OUTMODE_MEANDRE && cursorInputPos == IP_VOUT_SWITCH ) cursorInputPos++;
          #endif
          if (cursorInputPos > NUMBER_INPUT_POSITIONS-1 ) cursorInputPos = IP_FREQUENCY;
          setCursor2enterPosition( cursorInputPos );
          break;

      case FREQUENCY_SETTING:{
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
          }
          settings.frequency[(uint8_t)settings.currentChannel] = frequency;
        } break;
        
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
      }
      break;

    case RotaryEncoder::Direction::COUNTERCLOCKWISE:
    case RotaryEncoder::Direction::FAST_CCW:
      switch( menuState ) {
      case SETTING_MODE:
          if( cursorInputPos == IP_FREQUENCY )
            cursorInputPos = NUMBER_INPUT_POSITIONS-1;
          else {
            cursorInputPos--;
            #ifdef ENABLE_VOUT_SWITCH  // skip the input position for switching Vout for meandre(CLK) if the current mode is not meandre...
              if( settings.currentMode[(uint8_t)settings.currentChannel] != OUTMODE_MEANDRE && cursorInputPos == IP_VOUT_SWITCH ) cursorInputPos--;
            #endif
          }  
          setCursor2enterPosition( cursorInputPos );
          break;

      case FREQUENCY_SETTING: {
          unsigned long newFrequency = frequency + power(10, digitPos);
          unsigned char dispDigit = frequency % power(10, digitPos + 1) / power(10, digitPos);
          if (newFrequency > 0 && dispDigit > 0) {
            frequency -= power(10, digitPos);
            updateDisplay = true;
          }
          settings.frequency[(uint8_t)settings.currentChannel] = frequency;
        } break;
          
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
      }
      break;
    }
}  // encoderProcessing()


void jump2settingMenu( void ) {
  lcd.noBlink();
  lcd.setCursor(0, 0);
  lcd.cursor();
  menuState = SETTING_MODE;
  cursorInputPos = IP_FREQUENCY;
}

// Function to display the current frequency in the top left corner
void displayFrequency( unsigned long frequencyToDisplay ) {
  lcd.setCursor(0, 0); 
  lcd.print( F("f=") );
  for (int i = 7; i >= 0; i--) {
    unsigned char dispDigit = frequencyToDisplay / power(10, i);
    lcd.print(dispDigit);
    frequencyToDisplay -= dispDigit * power(10, i);
  }
  lcd.print(F("Hz"));
}


// Function to display power state (ON/OFF) in the top right corner
void displayPower( bool _signalOn ) {
  lcd.setCursor(13, 0);
  lcd.print( _signalOn ? F("ON ") : F("OFF") );
}


// Function to display the mode in the bottom right corner
void displayMode( outmode_t _currentMode ) {
  lcd.setCursor(13, 1);
#ifdef GRAPH_ICONS
  switch( _currentMode ) {
  case OUTMODE_SINE:    lcd.write( 2 );lcd.write( 3 );lcd.write( 4 );break;
  case OUTMODE_TRIANGLE:lcd.write( 5 );lcd.write( 6 );lcd.write( 7 );break;
  case OUTMODE_MEANDRE: lcd.write( 1 );lcd.write( 1 );lcd.write( 1 );break;
  }
#else
  lcd.print(mode[(uint8_t)_currentMode ]);
#endif
  if( menuState == SETTING_MODE ) {
    lcd.cursor();
    lcd.noBlink();
  }
}


// Function to display the mode in the bottom left corner
// Only used if you enable PHASE setting instead of FREQ register
void displayPhase( unsigned int phaseToDisplay ) {
  lcd.setCursor(0, 1);
  lcd.write(0);
  lcd.print("=");
  for( int i = 3; i >= 0; i-- ) {
    unsigned int dispDigit = phaseToDisplay / power(10, i);
    lcd.print(dispDigit);
    phaseToDisplay -= dispDigit * power(10, i);
  }
}


// Function to display the FREQ register (either 0 or 1) in the bottom left
// corner
void displaycurrentChannel( bool _channel ) {
  lcd.setCursor(0, 1);
  lcd.print(F("CHAN"));
  lcd.print((uint8_t)_channel);
}


void displayCLKoutVolt( bool _toggleCLKoutVolt, outmode_t _currentMode ) {
  lcd.setCursor(7, 1);
  if( _currentMode == OUTMODE_MEANDRE ) {
    lcd.write( 1 );  // meandre sign
    lcd.print( _toggleCLKoutVolt ? F("5v  ") : F("3.3v") );
  } else {
    lcd.print( F("     ") );
  }
}

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
void setADmode( outmode_t _currentMode ) {
  switch( _currentMode ) {
  case OUTMODE_SINE:    sigGen.setModeSD( MD_AD9833::MODE_SINE ); break;
  case OUTMODE_TRIANGLE:sigGen.setModeSD( MD_AD9833::MODE_TRIANGLE ); break;
  case OUTMODE_MEANDRE: sigGen.setModeSD( MD_AD9833::MODE_SQUARE1 ); break;
  }
}

void setADfrequency( bool _channel, unsigned long _frequency ) {
  sigGen.setFrequency( _channel ? MD_AD9833::CHAN_1 : MD_AD9833::CHAN_0, _frequency );
}

void setADchannel( bool _channel ) {
  if( _channel ) {
    sigGen.setActiveFrequency( MD_AD9833::CHAN_1 );
    frequency = settings.frequency[1];
    //frequency = sigGen.getFrequency( MD_AD9833::CHAN_1 );
  } else {
    sigGen.setActiveFrequency( MD_AD9833::CHAN_0 );
    frequency = settings.frequency[0];
    //frequency = sigGen.getFrequency(  MD_AD9833::CHAN_0 );
  }
}

#else // old error-fixed library

void setADmode( outmode_t _currentMode ) {
  sigGen.mode( _currentMode );
}

void setADfrequency(  bool _channel, unsigned long _frequency ) {
  sigGen.setFPRegister((uint8_t) _channel );
  sigGen.setFreq( _frequency );
}

void setADchannel( bool _channel ) {
  if( _channel ) {
    sigGen.setFPRegister(1);
    frequency = settings.frequency[1];
  } else {
    sigGen.setFPRegister(0);
    frequency = settings.frequency[0];
  }
}
#endif


void blinkDisplayBacklight( uint8_t nBlinks ) {
  for( uint8_t i=0; i<nBlinks; i++ ) {
    lcd.noBacklight(); _delay_ms(300);
    lcd.backlight();   _delay_ms(300);
  }
}


void applyCurrentSettings(void) {
  setADfrequency( 0, settings.frequency[0] );
  setADfrequency( 1, settings.frequency[1] );
  
  // set current mode
  setADmode( settings.currentMode[(uint8_t)settings.currentChannel] ); 
  setADchannel( settings.currentChannel );
}


#ifdef ENABLE_EEPROM
#define ADDRESS_SHIFT 0
bool readSettingsFromEEPROM( void ) { // read settings from EEPROM; return true if CRC check sum is OK
  eeprom_read_block( (void *)&settings, ADDRESS_SHIFT, sizeof(settings_t) );  //eeprom_read_block (void *__dst, const void *__src, size_t __n);
  // check the CRC8 chech sum of the memory block read from EEPROM
  uint8_t checkSum = crc8checkSum( (uint8_t *) &settings, sizeof(settings_t)-1 ); // last array member is a byte of the CRC check sum, so passing one less byte
  if( checkSum == settings.checkSum ) return true;
  return false;
}

void writeSettingsToEEPROM( void ) { // write settings to EEPROM
  // calculate the CRC8 chech sum of settings structure and write it to EEPROM along with settings
  uint8_t checkSum = crc8checkSum( (uint8_t *)&settings, sizeof(settings_t)-1 ); // last array member is a byte of the CRC check sum, so passing one less byte
  settings.checkSum = checkSum;
  eeprom_update_block( (void *)&settings, ADDRESS_SHIFT, sizeof(settings_t) );   // write settings to EEPROM: eeprom_update_block(const void *__src, void *__dst, size_t __n)
}

uint8_t crc8checkSum( uint8_t data[], uint8_t dataSize ) {   // calculate CRC8 check sum for the memory block
  uint8_t checkSum = 0xFF;  // initial crc check sum value (see crc algorythm in Wikipedia)
  
  for( uint8_t i=0; i < dataSize-1; i++ ) {  // CRC8 for all bytes in the memory block
    checkSum = crc8update( checkSum, data[i] );  
  }
  return checkSum;
}

uint8_t crc8update(uint8_t crc, uint8_t data) {  // standard calculation from Wikipedia
  crc ^= data;
  for( uint8_t i = 0; i < 8; i++ ) crc = crc & 0x80 ? (crc << 1) ^ 0x31 : crc << 1;
  return crc;
}
#endif


void toggleOut( outmode_t _currentMode ) {
  digitalWrite( TOGGLE_OUT, (_currentMode == OUTMODE_MEANDRE) ? HIGH : LOW );
  _delay_ms(100);
}
