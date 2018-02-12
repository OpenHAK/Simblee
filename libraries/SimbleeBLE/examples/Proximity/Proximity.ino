/*
The sketch demonstrates how to do receive Bluetooth Low Energy 4
RSSI signal strength information from the Simblee.

This information is available to the Simblee after a connection
has been established.  The iPhone application is able to access
the RSSI signal strength information before a connection is
established if needed.

This sketch establishes four zones.  In the first zone (furtherist away),
both leds are off.  In the second zone (closer), led1 is on (green).  In
the third zone, both leds are on (yellow).  In the forth and closest zone
led2 is on (red).
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

// pin 3 on the RGB sheild is the green led
int led1 = 3;

// pin 2 on the RGB shield is the red led
int led2 = 2;

void setup() {
  // configure leds
  pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT);

  // turn on led1 to indicate we are waiting for a connection
  digitalWrite(led1, HIGH);

  // drop the transmission power to move the zones closer to
  // the Simblee (which also reduces the interference from
  // other wireless devices and reflection)
  // (all zones are within approxiately 10 feet of the Simblee)
  SimbleeBLE.txPowerLevel = -20;
  
  // start the BLE stack
  SimbleeBLE.begin();
}

void loop() {
  // switch to lower power mode
  Simblee_ULPDelay(INFINITE);
}

void SimbleeBLE_onConnect()
{
  // turn off led1 when we have a connection
  digitalWrite(led1, LOW);
}

void SimbleeBLE_onDisconnect()
{
  // don't leave the leds on if they disconnect
  digitalWrite(led1, LOW);
  digitalWrite(led2, LOW);
}

// returns the dBm signal strength indicated by the receiver
// received signal strength indication (-0dBm to -127dBm)
void SimbleeBLE_onRSSI(int rssi)
{
  // turn off both leds if the signal strength is out of range
  if (rssi <= -75)
  {
    digitalWrite(led1, LOW);
    digitalWrite(led2, LOW);
  }
  // in range, but signal strength is weak
  else if (rssi <= -65)
  {
    digitalWrite(led1, HIGH);
    digitalWrite(led2, LOW);
  }
  // in range, signal strength is stronger
  else if (rssi <= -55)
  {
    digitalWrite(led1, HIGH);
    digitalWrite(led2, HIGH);
  }
  // in range, signal strength is strong
  else
  {
    digitalWrite(led1, LOW);
    digitalWrite(led2, HIGH);
  }
}
