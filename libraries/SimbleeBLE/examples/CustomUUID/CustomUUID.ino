/*
The sketch demonstrates how to use a different UUID for
the Simblee service and characteristics.

This example only applies if you have a specific
need to use a custom 128 bit uuid.

Note:
If you use a customUUID, the iPhone apps must also
be updated to discover Simblee's advertising with the
custom UUID.

The iPhone application source code in this distribution
has been enhanced to discover Simblee's with customUUIDs,
by declaring the UUID at the top of AppDelegate.m.
Without modification, the iPhone apps will use the
uuid 0xfe84, which has been allocated for Simblee
by the Bluetooth SIG.

The Simblee apps will continue to only discover Simblee's
that advertise using the Simblee UUID.
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

#include <SimbleeBLE.h>

// pin 3 on the RGB shield is the green led
// (shows when the Simblee is advertising or not)
int advertisement_led = 3;

// pin 2 on the RGB shield is the red led
// (goes on when the Simblee has a connection from the iPhone, and goes off on disconnect)
int connection_led = 2;

void setup() {
  // led used to indicate that the Simblee is advertising
  pinMode(advertisement_led, OUTPUT);
  
  // led used to indicate that the Simblee is connected
  pinMode(connection_led, OUTPUT);

  // 128 bit base uuid
  // (generated with http://www.uuidgenerator.net)
  SimbleeBLE.customUUID = "c97433f0-be8f-4dc8-b6f0-5343e6100eb4";

  // this must match the customUUID in the iPhone app declared
  // at the top of AppDelegate.m
   
  // start the BLE stack
  SimbleeBLE.begin();
}

void loop() {
  // switch to lower power mode
  Simblee_ULPDelay(INFINITE);
}

void SimbleeBLE_onAdvertisement(bool start)
{
  // turn the green led on if we start advertisement, and turn it
  // off if we stop advertisement
  
  if (start)
    digitalWrite(advertisement_led, HIGH);
  else
    digitalWrite(advertisement_led, LOW);
}

void SimbleeBLE_onConnect()
{
  digitalWrite(connection_led, HIGH);
}

void SimbleeBLE_onDisconnect()
{
  digitalWrite(connection_led, LOW);
}
