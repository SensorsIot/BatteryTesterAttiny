/*  BatteryTesterAttiny.ino  -
  2015 Copyright (c) Andreas Spiess  All right reserved.
 
  Author: Andreas Spiess
  
  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  */
  
#include <TinyOzOLED.h>
#include <TinyWireM.h>

// ATtiny Pin 5 = SDA                   ATtiny Pin 6 = (D1) to LED2
// ATtiny Pin 7 = SCK                    ATtiny Pin 8 = VCC (2.7-5.5V)

#define FET 1          // ATtiny Pin 6
#define ADCVOLT A2     // ATtiny Pin 3
#define ADCCURRENT A3  // ATtiny Pin 2

const float FACTOR = 2.2;    // Adjust for 2:1 Voltage divider and internal reference


int emptyRaw ;
int emptyOld ;
float empty ;
int loadRaw ;
int currRaw ;
float load ;
float curr ;
int emptySum;
int loadSum;


void setup() {
  analogReference(INTERNAL);      // use precise internal reference

  OzOled.init();  //initialze OLED display

  OzOled.clearDisplay();           //clear the screen and set start position to top left corner
  OzOled.setNormalDisplay();       //Set display to Normal mode
  
  OzOled.sendCommand(0xA1);        // set Orientation
  OzOled.sendCommand(0xC8);

  pinMode(FET, OUTPUT);
}

void loop() {

  digitalWrite(FET, LOW);

  // wait till value stable
  while (abs(emptyRaw - emptyOld) > 3) {
    emptyOld = emptyRaw;
    emptyRaw = analogRead(ADCVOLT);
  }

  // Average over 10 cycles
  emptySum = 0;
  for (int i = 0; i < 10; i++) {
    emptySum = emptySum + analogRead(ADCVOLT);
    delay(1);
  }
  empty = emptySum  / 10230.0 * FACTOR;
  digitalWrite(FET, HIGH);             // switch load on


  while (abs(loadRaw - emptyOld) > 3) {
    emptyOld = loadRaw;
    loadRaw = analogRead(ADCVOLT);
  }

  currRaw = analogRead(ADCCURRENT);
  curr = currRaw * 1.1 / 1023.0;

  // Average over 10 cycles
  loadSum = 0;
  for (int i = 0; i < 10; i++) {
    loadSum = loadSum + analogRead(ADCVOLT);
    delay(1);
  }
  load = loadSum / 10230.0 * FACTOR ;

  digitalWrite(FET, LOW);   // switch load off

  displayOLED();

  delay(500);
  OzOled.clearDisplay();
}

void displayOLED() {
    char tmp[10];
    dtostrf(empty, 1, 2, tmp);
    OzOled.printBigNumber(tmp, 0, 0, 4);
    dtostrf(load, 1, 2, tmp);
    OzOled.printBigNumber(tmp, 0, 4, 4);
}

