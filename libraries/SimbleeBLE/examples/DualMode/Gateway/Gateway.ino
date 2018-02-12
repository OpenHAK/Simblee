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

void setup() {
  pinMode(3, OUTPUT);
  
  Serial.begin(9600);

  SimbleeBLE.begin();
}

void loop() {
}

void SimbleeBLE_onAdvertisement(bool start)
{
  /*
  if (start)
    SimbleeBLE.dualModeBegin();
  else
    SimbleeBLE.dualModeEnd();
  */
}

void SimbleeBLE_onConnect() {
  SimbleeBLE.dualModeBegin();
}

void SimbleeBLE_onDisconnect() {
  SimbleeBLE.dualModeEnd();
}

void SimbleeBLE_onReceive(char *data, int len) {
}

void SimbleeBLE_onDualModeStart() {
  digitalWrite(3, HIGH);
  
  // request/response pattern recommended
  SimbleeCOM.send("ABC", 4);
}

void SimbleeBLE_onDualModeStop() {
  digitalWrite(3, LOW);
}

void SimbleeCOM_onReceive(unsigned int esn, const char *payload, int len, int rssi) {
  printf("%s\n", payload);
}
