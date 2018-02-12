/*
The sketch demonstrates how to increase or decrease the Bluetooth Low Energy 4
Advertisement transmission interval.

Faster iterval = higher power consumption = lower connection latency
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
int led = 3;

// interval between advertisement transmissions ms (range is 20ms to 10.24s) - default 20ms
int interval = 675;  // 675 ms between advertisement transmissions

void setup() {
  // led used to indicate that the Simblee is advertising
  pinMode(led, OUTPUT);

  // this is the data we want to appear in the advertisement
  // (if the deviceName and advertisementData are too long to fix into the 31 byte
  // ble advertisement packet, then the advertisementData is truncated first down to
  // a single byte, then it will truncate the deviceName)
  SimbleeBLE.advertisementData = "-data";
  
  // change the advertisement interval
  SimbleeBLE.advertisementInterval = interval;

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
    digitalWrite(led, HIGH);
  else
    digitalWrite(led, LOW);
}
