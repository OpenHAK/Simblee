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
#include <Simblee_AS6200.h>

Simblee_AS6200 AS6200;

#include "thermometer_png.h"
#include "ams_logo_transparent_png.h"
#include "Simblee_Logo_png.h"

#define LED         3
#define BTN         2

#define ams_GRAY rgb(135,135,135)
#define SimbleeBlue rgb(55,122,177)
asm(".global _printf_float");

void setup() {
  // put your setup code here, to run once:
  pinMode(LED, OUTPUT);
  pinMode(BTN, INPUT);
  delay(100);
  digitalWrite(LED, LOW);
  Serial.begin(115200);
  Wire.begin();

  if (!AS6200.begin()) {
    printf("Failed to intialize Sensor, Halting!\n");
    while (1);
  }

  SimbleeForMobile.deviceName = "AS6200 Temp";
  SimbleeForMobile.advertisementData = "ams";
  SimbleeForMobile.domain = "as6200.simblee.com";
  SimbleeForMobile.baseline = "Nov 28 2016 12:00";
  SimbleeForMobile.begin();
}

bool first_sample;

float first_temp;
float min_temp;
float max_temp;
float temp_range;

uint8_t text;
uint8_t mercury;
unsigned long lastUpdated = 0;
unsigned long updateRate = 500; // half a second

// include newlib printf float support (%f used in sprintf below)
asm(".global _printf_float");

void loop() {
  // put your main code here, to run repeatedly:
  float tempAS6200;
  float simbleeTemp;
  unsigned long loopTime = millis();
  
  digitalWrite(LED, HIGH);
  tempAS6200 = AS6200.readTemperatureC();
  simbleeTemp = Simblee_temperature(CELSIUS);
  printf("Simblee: %.2f\tAS6200: %f\n", simbleeTemp, tempAS6200);
  digitalWrite(LED, LOW);
  delay(125);

  if (SimbleeForMobile.updatable && updateRate < (loopTime - lastUpdated))
  {
    lastUpdated = loopTime;
    
    // get a cpu temperature sample
    // degrees c (-128.00 to +127.00)
    // degrees f (-198.00 to +260.00)
    // CELSIUS / FAHRENHEIT
    float temp = tempAS6200;
  
    // requires newlib printf float support
    char buf[16];
    sprintf(buf, "%.03f", temp);
  
    // base everything on the first sample / ambient temperature
    if (first_sample)
    {
      first_temp = temp;
      
      // putting your finger on the Simblee shield raises the temp approx 2 degrees
      min_temp = first_temp - 0.25;
      max_temp = first_temp + 1.0;
      temp_range = max_temp - min_temp;
      
      first_sample = false;
    }

    // update the text first with the actual temp
    SimbleeForMobile.updateText(text, buf);

    // restrict temp to range
    if (temp < min_temp)
      temp = min_temp;
    if (temp > max_temp)
      temp = max_temp;

    int mercury_range = 262;
    int mercury_position = ((temp - min_temp) / temp_range) * mercury_range;

    // invert so the smallest value at the bottom
    mercury_position = mercury_range - (mercury_position);

    // update the mercury
    SimbleeForMobile.updateRect(mercury, 45, 176 + mercury_position, 33, 262 + 15 - mercury_position);
  }

  // process must be called in the loop for SimbleeForMobile
  SimbleeForMobile.process();
}

void SimbleeForMobile_onConnect()
{
  first_sample = true;
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
  SimbleeForMobile.drawRect(105,150,204,120,SimbleeBlue);
  SimbleeForMobile.drawRect(107,152,200,116,WHITE);
  SimbleeForMobile.drawRect(105,300,204,60,SimbleeBlue);
  SimbleeForMobile.drawRect(107,302,200,56,WHITE);
  
  
  SimbleeForMobile.imageSource(IMAGE2, PNG, ams_logo_transparent_png, ams_logo_transparent_png_len);
  SimbleeForMobile.drawImage(IMAGE2, 115, 160);
  //
  // \xb0 is the symbol for "degrees"
  // Android requires the unicode version, \xc2\xb0
  //
  if(SimbleeForMobile.remoteDeviceType == REMOTE_DEVICE_OS_IOS) {
    SimbleeForMobile.drawText(270, 308, "\xb0" "C", SimbleeBlue);
  } else if(SimbleeForMobile.remoteDeviceType == REMOTE_DEVICE_OS_ANDROID) {
    SimbleeForMobile.drawText(270, 308, "\xc2\xb0" "C", SimbleeBlue);
  } else if(SimbleeForMobile.remoteDeviceType == REMOTE_DEVICE_OS_UNKNOWN) {
    SimbleeForMobile.drawText(270, 308, "C", SimbleeBlue);
  }
    
  text = SimbleeForMobile.drawText(145, 300, "", SimbleeBlue, 45);
  SimbleeForMobile.drawText(145, 200, "AS6200", ams_GRAY, 40);
  SimbleeForMobile.drawText(134,240,"Temperature Sensor", ams_GRAY, 18);
  // usable area: 56, 136, 51, 262
  // mercury area: 65, 136, 33, 262 + 15
  mercury = SimbleeForMobile.drawRect(45, 176, 33, 262 + 15, rgb(160,0,0), rgb(204,0,0));

  // hide the mercury until the image is uploaded
  SimbleeForMobile.setVisible(mercury, false);



  
  SimbleeForMobile.imageSource(IMAGE1, PNG, thermometer_png, thermometer_png_len);  
  SimbleeForMobile.drawImage(IMAGE1, 10, 130);

  SimbleeForMobile.setVisible(mercury, true);

  

  SimbleeForMobile.endScreen();
}

void ui_event(event_t &event)
{
}
