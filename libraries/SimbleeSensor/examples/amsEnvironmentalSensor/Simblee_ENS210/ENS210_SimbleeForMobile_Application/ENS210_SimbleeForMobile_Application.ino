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
#include <Simblee_ENS210.h>

#include "ams_logo_transparent_png.h"
#include "Simblee_Logo_png.h"

Simblee_ENS210 ENS210;

#define LED         3
#define BTN         2

#define ams_GRAY rgb(135,135,135)
#define SimbleeBlue rgb(55,122,177)
uint8_t text, textHumid;
uint8_t mercury;
unsigned long lastUpdated = 0;
unsigned long updateRate = 500; // half a second

bool first_sample;

float first_temp;
float min_temp;
float max_temp;
float temp_range;

asm(".global _printf_float");

void setup() 
{
  // put your setup code here, to run once:
  pinMode(LED, OUTPUT);
  pinMode(BTN, INPUT);
  delay(100);
  digitalWrite(LED, LOW);
  Serial.begin(115200);
  
  if (!ENS210.begin()) 
  {
    printf("Failed to intialize Sensor, Halting!\n");
    while (1);
  }
  
  SimbleeForMobile.deviceName = "ENS210";
  SimbleeForMobile.advertisementData = "ams";
  SimbleeForMobile.domain = "ens210.simblee.com";
  SimbleeForMobile.baseline = "Nov 28 2016 13:00";
  SimbleeForMobile.begin();
}

void loop() 
{
  // put your main code here, to run repeatedly:
  float ens210_Temp;
  float ens210_Humidity;
  float simbleeTemp;
  unsigned long loopTime = millis();
  
  digitalWrite(LED, HIGH);
  ens210_Temp = ENS210.readTemp();
  ens210_Humidity = ENS210.readHumidity();
  printf("ENS210 Temp: %.2fC\tENS210 Humidity: %.2f%%\n", ens210_Temp, ens210_Humidity);
  digitalWrite(LED, LOW);
  delay(125);

  if (SimbleeForMobile.updatable && updateRate < (loopTime - lastUpdated)) 
  {
    lastUpdated = loopTime;

    // get a cpu temperature sample
    // degrees c (-128.00 to +127.00)
    // degrees f (-198.00 to +260.00)
    // CELSIUS / FAHRENHEIT
    float temp = ens210_Temp;

    // requires newlib printf float support
    char buf[16], buf2[16];
    sprintf(buf, "%.02f", temp);
    sprintf(buf2, "%.02f", ens210_Humidity);

    // update the text first with the actual temp
    SimbleeForMobile.updateText(text, buf);
    SimbleeForMobile.updateText(textHumid, buf2);
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
  SimbleeForMobile.drawRect(58,100,204,135,SimbleeBlue);
  SimbleeForMobile.drawRect(60,102,200,131,WHITE);
 

  SimbleeForMobile.imageSource(IMAGE2, PNG, ams_logo_transparent_png, ams_logo_transparent_png_len);
  SimbleeForMobile.drawImage(IMAGE2, 115-47, 110);
  //
  // \xb0 is the symbol for "degrees"
  // Android requires the unicode version, \xc2\xb0
  //
  if(SimbleeForMobile.remoteDeviceType == REMOTE_DEVICE_OS_IOS) {
    SimbleeForMobile.drawText(205, 308, "\xb0" "C", BLUE);
  } else if(SimbleeForMobile.remoteDeviceType == REMOTE_DEVICE_OS_ANDROID) {
    SimbleeForMobile.drawText(205, 308, "\xc2\xb0" "C", BLUE);
  } else if(SimbleeForMobile.remoteDeviceType == REMOTE_DEVICE_OS_UNKNOWN) {
    SimbleeForMobile.drawText(205, 308, "C", BLUE);
  }
    
  
  SimbleeForMobile.drawText(145-47, 200-50, "ENS210", ams_GRAY, 40);
  SimbleeForMobile.drawText(134-70,240-50,"Humidity and Temperature\n                  Sensor", ams_GRAY, 18);

  

  SimbleeForMobile.drawText(60, 250, "Temperature", BLACK, 40);
  text = SimbleeForMobile.drawText(110, 300, "", BLUE, 45);
  SimbleeForMobile.drawText(90, 350, "Humidity", BLACK, 40);
  textHumid = SimbleeForMobile.drawText(110, 400, "", BLUE, 45);
  SimbleeForMobile.drawText(215, 425, "%", BLUE);
  // usable area: 56, 136, 51, 262
  // mercury area: 65, 136, 33, 262 + 15


  SimbleeForMobile.endScreen();
}

void ui_event(event_t &event)
{
}
