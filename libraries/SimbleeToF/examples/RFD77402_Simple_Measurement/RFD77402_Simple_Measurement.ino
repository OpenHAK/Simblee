/*
 * Copyright (c) 2017 RF Digital Corp. All Rights Reserved.
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
 
#include <Wire.h>
#include "SimbleeToF.h"

SimbleeToF tof;

void setup()
{
  Serial.begin(115200);
  delay(100);
  printf("RFDigital RFD77402 IoT 3D ToF Sensor simple measurement example\n\n");
  if (!tof.begin()) {
    printf("Failed to intialize Sensor, Halting!\n");
    while (1);
  }
}

void loop()
{
  printf("Dist_mm:%d mm\n", tof.tof_simple_measurement());
  delay(1000);
}