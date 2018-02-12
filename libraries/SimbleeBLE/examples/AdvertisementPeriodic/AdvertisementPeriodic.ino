/*
The sketch demonstrates how to do power conservative Bluetooth Low Energy 4
Advertisement with the Simblee.  Advertisement of "111" is done for an
interval in seconds, then advertisement of "222" is done for the same
interval, then the Simblee is switched ultra low power mode.  The Simblee
can be woken up by pressing Button 1, at which time the device will
advertise again.
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

// pin 5 on the RGB shield is button 1
int button = 5;

// time to advertise in milliseconds
int duration = SECONDS(5);

void advertise(const char *data, uint32_t ms)
{
  // this is the data we want to appear in the advertisement
  // (if the deviceName and advertisementData are too long to fix into the 31 byte
  // ble advertisement packet, then the advertisementData is truncated first down to
  // a single byte, then it will truncate the deviceName)
  SimbleeBLE.advertisementData = data;
  
  // start the BLE stack
  SimbleeBLE.begin();
  
  // advertise for ms milliseconds
  Simblee_ULPDelay(ms);
  
  // stop the BLE stack
  SimbleeBLE.end();
}

void setup()
{
  // led used to indicate that the Simblee is advertising
  pinMode(led, OUTPUT);
}

void loop()
{
  // start advertising
  digitalWrite(led, HIGH);
  
  // advertise "111" for indicated time
  advertise("111", duration);
  
  // adverise "222" for indicated time
  advertise("222", duration);

  // stop advertising 
  digitalWrite(led, LOW);
  
  // button used to wake up the Simblee (when button 1 is pressed, the Simblee will restart advertising)
  pinMode(button, INPUT);
  Simblee_pinWake(5, HIGH);
  
  // wait indefinitely in ultra low power and wake up on button 1 press
  Simblee_ULPDelay(INFINITE); 
  
  Simblee_resetPinWake(5);
}
