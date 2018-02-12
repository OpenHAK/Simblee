/*
This sketch demonstrates how to use Bluetooth Low Energy
and Gazell in the same sketch.

This sketch accepts a connection from the LedButton iPhone App.
When the button on the iPhone app is pressed, the green led on
the Host will toggle.
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

#include <SimbleeGZLL.h>
#include <SimbleeBLE.h>

device_t role = DEVICE0;

char led = 0;

int state = 0;

void setup()
{
  Serial.begin(57600);
  Serial.println("start");  
}

void loop()
{
  switch (state)
  {
  case 0:
    SimbleeBLE.begin();
    state = 1;
    break;
    
  case 1:
    // waiting for a BLE button press
    break;
    
  case 2:
    // this disconnects us from the iPhone App
    // (we get a warning on the iPhone App because we are
    // not detecting this condition correctly in the App)
    SimbleeBLE.end();
    state = 3;
    break;
    
  case 3:
    SimbleeGZLL.begin(role);
    SimbleeGZLL.sendToHost(led);
    state = 4;
    break;
 
  case 4:
    // waiting for GZLL acknowledgement from Host
    break;
 
  case 5:
    SimbleeGZLL.end();
    state = 0;
    break;
  }
}

void SimbleeBLE_onReceive(char *data, int len)
{
  led = ! led;
  state = 2;
}

void SimbleeGZLL_onReceive(device_t device, int rssi, char *data, int len)
{
  state = 5;
}
