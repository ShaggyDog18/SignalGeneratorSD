#include <SPI.h>
#include "AD9833.h"

AD9833::AD9833(int _FSYNC, unsigned long _mclk) {
  FSYNC = _FSYNC;
  pinMode(FSYNC, OUTPUT);     // Set FSYNC pin as input
  mclk = _mclk;               // Set the oscillator used, for example 24MHz
  controlRegister = 0x2000;   // Default control register : FREQ0, PHASE0, Sine
  fqRegister = 0x4000;        // Default frequency register is 0
  pRegister = 0xC000;         // Default phase register is 0
  SPI.begin();                // Initialise the SPI BUS
  SPI.setDataMode(SPI_MODE2); // Set SPI in mode2, this should be moved in
  // methods when SPI.transfer is called in case you
  // have multiple devices using the SPI bus
}

void AD9833::writeData(int data) {
  // FSYNC pin must be pulled low when new data is received by AD9833
  digitalWrite(FSYNC, LOW);
  // Send the first 8 MSBs of data
  SPI.transfer(highByte(data));
  // Send the last 8 LSBs of data
  SPI.transfer(lowByte(data));
  // Set the FSYNC pin to high then end SPI transaction
  digitalWrite(FSYNC, HIGH);
  // debugging
  if (debugging) {
    lcdDebug(String(data, HEX));
  }
}

void AD9833::setFreq(unsigned long _freq) {
  // First check that the data received is fine
  unsigned long freqReg;
  // Frequency cannot be negative
  if (_freq < 0) {
    freqReg = 0;
    freq = 0;
  }
  // If the frequency is more than maximum frequency, just set it to maximum
  else if (_freq > mclk) {
    freqReg = pow(2, 28) - 1;
    freq = mclk / 2;
  }
  // If all is good, compute the freqReg knowhing that the analog output is
  // (mclk/2^28) * freqReg
  else {
    freq = _freq;
    freqReg = (unsigned long) ((freq * pow(2, 28)) / mclk + 0.5);
  }
  // Initialise two variables that are 16bit long which we use to divide the
  // freqReg in two words
  // set D15 to 0 and D14 to 1 to put data in FREQ0/1 register
  int MSW = ((int)(freqReg >> 14)) | fqRegister; // Take out the first 14bits
  // and set D15 to 0 and D14 to
  // 1 or viceversa depending on
  // FREQ reg
  int LSW = ((int)(freqReg & 0x3FFF)) |
            fqRegister; // Take only the last 14bits using a mask and set D15 to
  // 0 and D14 to 1 or viceversa depending on FREQ reg
  // Send the data, most significant word first
  writeData(LSW);
  writeData(MSW);
}
unsigned long AD9833::getFreq() {
  return freq;
}
void AD9833::setPhase(int _phase) {
  // Phase cannot be negative
  if (_phase < 0) {
    phase = 0;
  }
  // Phase maximum is 2^12
  else if (_phase >= 4096) {
    phase = 4096 - 1;
  }
  // If all is good, set the new phase value
  else {
    phase = _phase;
  }
  // Extract the 12 bits from the freqReg and set D15-1, D14-1, D13-0, D12-X to
  // put data in PHASE0/1 register
  int phaseData = phase | pRegister;
  int LSW = (phase & 0x3FFF) | pRegister;
  writeData(phaseData);
}
int AD9833::getPhase() {
  return phase;
}

void AD9833::setCtrlReg(unsigned long _controlRegister) {
  // Just make sure that the first two bits are set to 0
  controlRegister = _controlRegister & 0x3FFF;
  writeCtrlReg();
}

int AD9833::getCtrlReg() {
  return controlRegister;
}

void AD9833::writeCtrlReg() {
  writeData(controlRegister);
}

void AD9833::sleep(int s) {
  switch (s) {

    case 0: {
        controlRegister &= 0xFF3F; // No power-down: D7-0 and D6-0
      } break;

    case 1: {
        controlRegister &= 0xFF7F; // DAC powered down: D7-0 and D6-1
        controlRegister |= 0x0040;
      } break;

    case 2: {
        controlRegister &= 0xFFBF; // Internal clock disabled: D7-1 and D6-0
        controlRegister |= 0x0080;
      } break;

    case 3: {
        controlRegister |= 0x00C0; // Both DAC powered down and internal clock disabled
      } break;
  }
  // Update the control register
  writeCtrlReg();
}

void AD9833::reset(int r) {
  if (r == 0) {
    controlRegister &= 0xFEFF; // Set D8 to 0
  }

  else if (r == 1) {
    controlRegister |= 0x0100; // Set D8 to 1
  }
  writeCtrlReg();
}

void AD9833::mode(int m) {
  switch (m) {
    case 0: {
        controlRegister &= 0xFFDD; // Output sine: D5-0 and D1-0
      } break;
    case 1: {
        controlRegister &= 0xFFDF; // Output triangle: D5-0 and D1-1
        controlRegister |= 0x0002;
      } break;
    case 2: {
        controlRegister &= 0xFFFD; // Output clock (rectangle): D5-1 and D1-0
        controlRegister |= 0x0028;  // set D5-1, D3-1: ShaggyDog: corrected value to fix the bug 1/2 frequency for CLK signal
      } break;
  }
  writeCtrlReg();
}

void AD9833::setFPRegister(int r) {
  if (r == 0) {
    controlRegister &= 0xF3FF; // Set D11 and D10 in control register to 0
    fqRegister = 0x4000; // Set D15 to 0 and D14 to 1 in a variable that will
    // later choose the FREQ0 register
    pRegister =
      0xC000; // Set D15 to 1 and D14 to 1 and D13 to 0 for the PHASE register
  } else if (r == 1) {
    controlRegister |= 0x0C00; // Set D11 and D10 in control register to 1
    fqRegister = 0x8000; // Set D15 to 1 and D14 to 0 in a variable that will
    // later choose the FREQ1 register
    pRegister =
      0xD000; // Set D15 to 1 and D14 to 1 and D13 to 1 for the PHASE register
  }
  writeCtrlReg();
}

void AD9833::lcdDebugInit(LiquidCrystal_I2C *_debugger) {
  debugger = _debugger;
  debugging = true;
}

void AD9833::lcdDebug(String message) {
  if (debugging) {
    debugger->setCursor(0, 1);
    debugger->print(message);
  }
}
