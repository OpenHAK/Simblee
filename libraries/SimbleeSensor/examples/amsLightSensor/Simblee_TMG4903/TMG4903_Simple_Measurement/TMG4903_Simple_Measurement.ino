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

#include <Simblee_TMG4903.h>

Simblee_TMG4903 TMG4903;  

asm(".global _printf_float");

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  delay(100);
  printf("\nRFDigital TMG4903 sensor simple example\n\n");
  if (!TMG4903.begin()) {
    printf("Failed to intialize Sensor, Halting!\n");
    while (1);
  }
}

void loop() {
  // put your main code here, to run repeatedly:
   printf("Red: %d, Green: %d, Blue: %d, Clear: %d, Proximity: %d\n", TMG4903.readRed()
    , TMG4903.readGreen(), TMG4903.readBlue(), TMG4903.readClear(), TMG4903.readProximity());
  delay(1000);
}