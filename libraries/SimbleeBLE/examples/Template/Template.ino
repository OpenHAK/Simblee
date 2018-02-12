/*
The sketch is an empty template for Bluetooth Low Energy 4.
Simply remove what you dont need, and fill in the rest.
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

void setup()
{
  SimbleeBLE.deviceName = "Simblee";
  SimbleeBLE.advertisementData = "data";
  SimbleeBLE.advertisementInterval = MILLISECONDS(300);
  SimbleeBLE.txPowerLevel = -20;  // (-20dbM to +4 dBm)

  // start the BLE stack
  SimbleeBLE.begin();
}

void loop()
{
  // switch to lower power mode
  Simblee_ULPDelay(INFINITE);  

  // to send one char
  // SimbleeBLE.send((char)temp);

  // to send multiple chars
  // RFduinBLE.send(&data, len);
}

void SimbleeBLE_onAdvertisement(bool start)
{
}

void SimbleeBLE_onConnect()
{
}

void SimbleeBLE_onDisconnect()
{
}

// returns the dBm signal strength indicated by the receiver after connection (-0dBm to -127dBm)
void SimbleeBLE_onRSSI(int rssi)
{
}

void SimbleeBLE_onReceive(char *data, int len)
{
}
