/*
This sketch demonstrates how to use Bluetooth Low Energy
and SimbleeCOM in the same sketch.

This is an alternative approach to DualMode - in DualMode both protocols are
operational simulatenously, in this approach only one protocol is operational at a
time (which offers you greater flexibility).

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

#include <SimbleeCOM.h>
#include <SimbleeBLE.h>

char led = 0;

int state = 0;

void setup() {
  Serial.begin(9600);
  printf("start");
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
    SimbleeCOM.begin();
    SimbleeCOM.send(&led, 1);
    state = 4;
    break;
 
  case 4:
    SimbleeCOM.end();
    state = 0;
    break;
  }
}

void SimbleeBLE_onReceive(char *data, int len)
{
  led = ! led;
  state = 2;
}

