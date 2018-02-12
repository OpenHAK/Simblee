/*
This sketch demonstrates how to use Bluetooth Low Energy
and SimbleeCOM in the same sketch.

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

// pin for the Green Led
int green_led = 3;

void setup()
{
  pinMode(green_led, OUTPUT);

  // start the SimbleeCOM stack
  SimbleeCOM.begin();
}

void loop()
{
}

void SimbleeCOM_onReceive(unsigned int esn, const char *payload, int len, int rssi)
{
  char led = payload[0];
  digitalWrite(green_led, led);
}

