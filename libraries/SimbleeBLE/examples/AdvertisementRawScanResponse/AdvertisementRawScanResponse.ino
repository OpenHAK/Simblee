/*
Demonstrates advanced raw access to the advertisement and scan response packets.

This example only applies if you have a specific need for raw access to the advertisement
packet.  This is for advanced use only, as the Simblee will not advertise is the packet
is invalid.

Note:
This advertisement packet uses a custom 128 bit UUID.
If you use a customUUID, the iPhone apps must also
be updated to discover Simblee's advertising with the
custom UUID.

The iPhone application source code in this distribution
has been enhanced to discover Simblee's with customUUIDs,
by declaring the UUID in the +Load method of the
AppViewController.m.  Without modification, the iPhone
apps continue to operate as before.

The Simblee AppStore apps will continue to only discover
Simblee's that advertise using the previous fixed Simblee
UUID.
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

uint8_t advdata[] =
{
  0x05,  // length
  0x09,  // complete local name type
  0x41,  // 'A'
  0x42,  // 'B'
  0x43,  // 'C'
  0x44,  // 'D'
  
  0x02,  // length
  0x01,  // flags type
  0x06,  // le general discovery mode | br edr not supported

  0x02,  // length
  0x0A,  // tx power level
  0x04,  // +4dBm
};

uint8_t srdata[] =
{
  0x11,    // length
  0x07,    // 128 bit service uuid (complete)
  
  // 128 bit uuid
  // equivalent to SimbleeBLE.customUUID = "c97433f0-be8f-4dc8-b6f0-5343e6100eb4"
  // little endian
  0xB4,
  0x0E,
  0x10,
  0xE6,
  0x43,
  0x53,
  0xF0,
  0xB6,
  0xC8,
  0x4D,
  0x8F,
  0xBE,
  0xF0,
  0x33,
  0x74,
  0xC9,
};

// pin 3 on the RGB shield is the green led
int led = 3;

void setup() {
  // led used to indicate that the Simblee is advertising
  pinMode(led, OUTPUT);

  SimbleeBLE_advdata = advdata;
  SimbleeBLE_advdata_len = sizeof(advdata);

  SimbleeBLE_srdata = srdata;
  SimbleeBLE_srdata_len = sizeof(srdata);

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
