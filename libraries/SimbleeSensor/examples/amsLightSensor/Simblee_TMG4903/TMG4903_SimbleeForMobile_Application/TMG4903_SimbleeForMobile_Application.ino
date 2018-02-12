/*
 * Copyright (c) 2017 RF Digital Corp. All Rights Reserved.
 *
 * The source code contained in this file and all intellectual property embodied in
 * or covering the source code is the property of RF Digital Corp. or its licensors.
 * Your right to use this source code and intellectual property is non-transferable,
 * non-sub licensable, revocable, and subject to terms and conditions of the
 * SIMBLEE SOFTWARE LICENSE AGREEMENT.
 * http://www.simblee.com/licenses/SimbleeSoftwareLicenseAgreement.txt
 *
 * THE SOURCE CODE IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND.
 *
 * This heading must NOT be removed from this file.
 */

#include <SimbleeForMobile.h>
#include <Simblee_TMG4903.h>

Simblee_TMG4903 TMG4903;

#include "ams_logo_transparent_png.h"
#include "Simblee_Logo_png.h"

#define ams_GRAY rgb(135,135,135)
#define SimbleeBlue rgb(55,122,177)

uint8_t textProx, textClear, textRed, textGreen, textBlue;
uint8_t mercury;
unsigned long lastUpdated = 0;
unsigned long updateRate = 100;
asm(".global _printf_float");

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  delay(100);

  if (!TMG4903.begin()) {
    printf("Failed to intialize Sensor, Halting!\n");
    while (1);
  }

  SimbleeForMobile.deviceName = "ams TMG4903";
  SimbleeForMobile.advertisementData = "RFD";
  SimbleeForMobile.domain = "tmg.simblee.com";
  SimbleeForMobile.baseline = "Nov 28 2016 16:00";
  SimbleeForMobile.begin();
}

void loop() {
  //read proximity register
  unsigned long loopTime = millis();

  // read proximity
  uint16_t proximity = TMG4903.readProximity();

  // read color registers
  uint16_t clearT = TMG4903.readClear();
  uint16_t red = TMG4903.readRed();
  uint16_t green = TMG4903.readGreen();
  uint16_t blue = TMG4903.readBlue();

  printf("Proximity: %d\tClear: %d\tRed: %d\tGreen: %d\tBlue: %d\n", proximity, clearT, red, green, blue);

  delay(50);

  if (SimbleeForMobile.updatable && updateRate < (loopTime - lastUpdated))
  {
    lastUpdated = loopTime;

    // update the text first with the actual temp
    SimbleeForMobile.updateValue(textProx, proximity);
    SimbleeForMobile.updateValue(textClear, clearT);
    SimbleeForMobile.updateValue(textRed, red);
    SimbleeForMobile.updateValue(textGreen, green);
    SimbleeForMobile.updateValue(textBlue, blue);
  }

  SimbleeForMobile.process();

}

void ui()
{
#define  IMAGE1  1
#define  IMAGE2  2
  SimbleeForMobile.beginScreen(WHITE);
  SimbleeForMobile.drawRect(0,0,320,20,ams_GRAY);
  SimbleeForMobile.drawRect(0,80,320,490,ams_GRAY);
  SimbleeForMobile.drawRect(3,85,314,481,SimbleeBlue);
  SimbleeForMobile.drawRect(5,87,310,477,WHITE);
  SimbleeForMobile.drawRect(0,20,320,64,SimbleeBlue);
  SimbleeForMobile.drawRect(2,22,316,59,WHITE);
  SimbleeForMobile.imageSource(3, PNG, Simblee_Logo_png, Simblee_Logo_png_len);
  SimbleeForMobile.drawImage(3, 60, 25);
  SimbleeForMobile.drawRect(58,100,204,135,SimbleeBlue);
  SimbleeForMobile.drawRect(60,102,200,131,WHITE);
  SimbleeForMobile.imageSource(IMAGE2, PNG, ams_logo_transparent_png, ams_logo_transparent_png_len);
  SimbleeForMobile.drawImage(IMAGE2, 115-47, 110);

  SimbleeForMobile.drawText(80, 200-50, "TMG4903", ams_GRAY, 40);
  SimbleeForMobile.drawText(70,240-50,"Gesture, Color, Proximity\n                 Sensor", ams_GRAY, 18);

  SimbleeForMobile.drawText(65, 240, "Proximity", BLACK, 30);
  textProx = SimbleeForMobile.drawText(220, 240, "", BLUE, 30);
  SimbleeForMobile.drawText(65, 290, "Clear", BLACK, 30);
  textClear = SimbleeForMobile.drawText(220, 290, "", BLUE, 30);

  SimbleeForMobile.drawText(65, 340, "Red", BLACK, 30);
  textRed = SimbleeForMobile.drawText(220, 340, "", BLUE, 30);
  SimbleeForMobile.drawText(65, 390, "Green", BLACK, 30);
  textGreen = SimbleeForMobile.drawText(220, 390, "", BLUE, 30);
  SimbleeForMobile.drawText(65, 440, "Blue", BLACK, 30);
  textBlue = SimbleeForMobile.drawText(220, 440, "", BLUE, 30);
  // usable area: 56, 136, 51, 262
  // mercury area: 65, 136, 33, 262 + 15


  SimbleeForMobile.endScreen();
}

void ui_event(event_t &event)
{
}

