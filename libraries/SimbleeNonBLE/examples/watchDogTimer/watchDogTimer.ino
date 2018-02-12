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

int count = 5;

void setup() {
  Serial.begin(9600);

  printf("reset\n");
  
  Simblee_setWatchDogTimer(1000);
}

void loop() {
  // put your main code here, to run repeatedly:
  printf("count = %d\n", count);
  delay(900);
  count--;
  if (count > 0)
    Simblee_resetWatchDogTimer();
}