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

#include <Simblee_ENS210.h>

Simblee_ENS210 ENS210;

// include newlib printf float support
asm(".global _printf_float");

void setup() {
  // put your setup code here, to run once:
Serial.begin(115200);
  delay(100);
  printf("\nRFDigital ENS210 Temperature & Humidity sensor simple example\n\n");
  if (!ENS210.begin()) {
    printf("Failed to intialize Sensor, Halting!\n");
    while (1);
  }
}

void loop() {
  // put your main code here, to run repeatedly:
  printf("Temperature: %.02fC\tHumidity: %.02f%%\n", ENS210.readTemp(), ENS210.readHumidity());
  delay(1000);
}