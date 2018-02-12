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

#include <Simblee_AS6200.h>

Simblee_AS6200 AS6200;  

asm(".global _printf_float");

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  delay(100);
  printf("\nRFDigital AS6200 Temperature sensor simple example\n\n");
  if (!AS6200.begin()) {
    printf("Failed to intialize Sensor, Halting!\n");
    while (1);
  }
}

void loop() {
  // put your main code here, to run repeatedly:
  printf("Raw Temp: %d, Calculated Temp: %.02fF %.02fC\n", AS6200.readRawTemperature(),
  AS6200.readTemperatureF(), AS6200.readTemperatureC());
  delay(1000);
}