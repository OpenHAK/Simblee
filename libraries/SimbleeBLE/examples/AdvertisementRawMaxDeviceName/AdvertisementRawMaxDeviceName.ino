/*
Demonstrates advanced raw access to the advertisement and scan response packets.

This example only applies if you have a specific need for raw access to the advertisement
packet.  This is for advanced use only, as the Simblee will not advertise is the packet
is invalid.
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

// the advertisement packet is composed of a series of variable length blocks, that can appear in any order.
// each block starts with a length byte, followed by a type byte, followed by the data.
// the payload cannot exceed 31 bytes.

// Note: since the Simblee UUID is missing from the advertisement packet, the Simblee iPhone apps will not
// be able to see this advertisement.

uint8_t advdata[] =
{
  30,    // length
  0x09,  // complete local name type
  0x41,  // 'A'
  0x42,  // 'B'
  0x43,  // 'C'
  0x44,  // 'D'
  0x45,  // 'E'
  0x46,  // 'F'
  0x47,  // 'G'
  0x48,  // 'H'
  0x49,  // 'I'
  0x4A,  // 'J'
  0x4B,  // 'K'
  0x4C,  // 'L'
  0x4D,  // 'M'
  0x4E,  // 'N'
  0x4F,  // 'O'
  0x50,  // 'P'
  0x51,  // 'Q'
  0x52,  // 'R'
  0x53,  // 'S'
  0x54,  // 'T'
  0x55,  // 'U'
  0x56,  // 'V'
  0x57,  // 'W'
  0x58,  // 'X'
  0x59,  // 'Y'
  0x5A,  // 'Z'
  0x30,  // '1'
  0x31,  // '2'
  0x32,  // '3'
};

// pin 3 on the RGB shield is the green led
int led = 3;

void setup() {
  // led used to indicate that the Simblee is advertising
  pinMode(led, OUTPUT);

  SimbleeBLE_advdata = advdata;
  SimbleeBLE_advdata_len = sizeof(advdata);

  // uncomment for nonconnectable advertising
  // (nonconnectable forces a minimum 100ms advertisement_interval, >= 100ms intervals are okay)
  // SimbleeBLE.connectable = false;
  
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