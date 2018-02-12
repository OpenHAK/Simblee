/*
 * Copyright (c) 2016 RF Digital Corp. All Rights Reserved.
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
#include <Simblee_CCS811.h>

Simblee_CCS811 CCS811;

#include "ams_logo_transparent_png.h"
#include "Simblee_Logo_png.h"
#define ams_GRAY rgb(135,135,135)
#define SimbleeBlue rgb(55,122,177)

uint8_t text, textHumid;
uint8_t mercury;

unsigned long lastUpdated = 0;
unsigned long updateRate = 100; // half a second

asm(".global _printf_float");

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  delay(100);

  if (!CCS811.begin()) {
    printf("Failed to intialize Sensor, Halting!\n");
    while (1);
  }

  SimbleeForMobile.deviceName = "CCS811";
  SimbleeForMobile.advertisementData = "ams";
  SimbleeForMobile.begin();
}

void loop() {
  // put your main code here, to run repeatedly:
  uint16_t eCO2;
  uint16_t TVOC;
  unsigned long loopTime = millis();

  TVOC = CCS811.readTVOC();
  eCO2 = CCS811.readECO2();
  printf("CCS811 CO2: %dppm\tTVOC: %dppb\n", eCO2, TVOC);
  delay(1000);

   if (SimbleeForMobile.updatable && updateRate < (loopTime - lastUpdated))
  {
    lastUpdated = loopTime;
 
    SimbleeForMobile.updateValue(text, eCO2);
    SimbleeForMobile.updateValue(textHumid, TVOC);
  }

  // process must be called in the loop for SimbleeForMobile
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
  SimbleeForMobile.drawRect(58,100,204,115,SimbleeBlue);
  SimbleeForMobile.drawRect(60,102,200,111,WHITE);
 
  SimbleeForMobile.imageSource(IMAGE2, PNG, ams_logo_transparent_png, ams_logo_transparent_png_len);
  SimbleeForMobile.drawImage(IMAGE2, 115-47, 110);
  
  SimbleeForMobile.drawText(145-47, 200-50, "CCS811", ams_GRAY, 40);
  SimbleeForMobile.drawText(134-40,240-50,"Air Quality Sensor", ams_GRAY, 18);

  SimbleeForMobile.drawText(120, 250, "CO2", BLACK, 40);
  text = SimbleeForMobile.drawText(110, 300, "", BLUE, 45);
  SimbleeForMobile.drawText(215,325, "ppm", BLUE);
  SimbleeForMobile.drawText(120, 350, "VOC", BLACK, 40);
  textHumid = SimbleeForMobile.drawText(110, 400, "", BLUE, 45);
  SimbleeForMobile.drawText(215, 425, "ppb", BLUE);

  SimbleeForMobile.endScreen();
}
