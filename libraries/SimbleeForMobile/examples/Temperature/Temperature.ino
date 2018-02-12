/*
The sketch demonstrates how to do accept a Bluetooth Low Energy 4
Advertisement connection with the Simblee, then send CPU temperature
updates once a second.

This sketch works with SimbleeForMobile.
*/

/*
 * Copyright (c) 2015 RF Digital Corp. All Rights Reserved.
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

#include "thermometer_png.h"

void setup()
{
  // this is the data we want to appear in the advertisement
  // (if the deviceName and advertisementData are too long to fix into the 31 byte
  // ble advertisement packet, then the advertisementData is truncated first down to
  // a single byte, then it will truncate the deviceName)
  SimbleeForMobile.advertisementData = "temp";

  // use a subdomain to create an application specific cache
  SimbleeForMobile.domain = "temp.Simblee.com";

  // establish a baseline to use the cache during development to bypass uploading
  // the image each time
  SimbleeForMobile.baseline = "Oct 23 2014";

  // start SimbleeForMobile
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

void loop()
{
  unsigned long loopTime = millis();
  
  // sample once per second
  // todo: Simblee_ULPDelay( SECONDS(1) );
  
  if (SimbleeForMobile.updatable && updateRate < (loopTime - lastUpdated))
  {
    lastUpdated = loopTime;
    
    // get a cpu temperature sample
    // degrees c (-128.00 to +127.00)
    // degrees f (-198.00 to +260.00)
    // CELSIUS / FAHRENHEIT
    float temp = Simblee_temperature(CELSIUS);
  
    // requires newlib printf float support
    char buf[16];
    sprintf(buf, "%.02f", temp);
  
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
    SimbleeForMobile.updateRect(mercury, 65, 136 + mercury_position, 33, 262 + 15 - mercury_position);
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

  SimbleeForMobile.beginScreen(WHITE);

  //
  // \xb0 is the symbol for "degrees"
  // Android requires the unicode version, \xc2\xb0
  //
  if(SimbleeForMobile.remoteDeviceType == REMOTE_DEVICE_OS_IOS) {
    SimbleeForMobile.drawText(270, 248, "\xb0" "C", BLUE);
  } else if(SimbleeForMobile.remoteDeviceType == REMOTE_DEVICE_OS_ANDROID) {
    SimbleeForMobile.drawText(270, 248, "\xc2\xb0" "C", BLUE);
  } else if(SimbleeForMobile.remoteDeviceType == REMOTE_DEVICE_OS_UNKNOWN) {
    SimbleeForMobile.drawText(270, 248, "C", BLUE);
  }
    
  text = SimbleeForMobile.drawText(145, 240, "", BLUE, 45);

  // usable area: 56, 136, 51, 262
  // mercury area: 65, 136, 33, 262 + 15
  mercury = SimbleeForMobile.drawRect(65, 136, 33, 262 + 15, rgb(160,0,0), rgb(204,0,0));

  // hide the mercury until the image is uploaded
  SimbleeForMobile.setVisible(mercury, false);
  
  SimbleeForMobile.imageSource(IMAGE1, PNG, thermometer_png, thermometer_png_len);  
  SimbleeForMobile.drawImage(IMAGE1, 30, 100);
  
  SimbleeForMobile.setVisible(mercury, true);

  SimbleeForMobile.endScreen();
}

void ui_event(event_t &event)
{
}
