/*
This sketch demonstrates how to send data from a Device
to another Device (using the Host as an intermediary)
in a Gazell network.

When Button A on Device0 is pressed and released,
the green led on Device1 will toggle.
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

device_t role = HOST;

// the last known state from DEVICE0 (default to off)
char state = 0;

void setup()
{
  // start the GZLL stack  
  SimbleeGZLL.begin(role);
}

void loop()
{
}

void SimbleeGZLL_onReceive(device_t device, int rssi, char *data, int len)
{
  if (device == DEVICE0)  // get the current state from DEVICE0
    state = data[0];
  else if (device == DEVICE1)  // relay the last known state to DEVICE1
    SimbleeGZLL.sendToDevice(device, state);
}
