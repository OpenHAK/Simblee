/*
The sketch demonstrates how to generate a true non-deterministic random
number stream based on internal thermal noise.
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

#include <SimbleeRNG.h>

void setup() {
  Serial.begin(9600);
  
  uint8_t stream[8];
  SimbleeRNG::generate(stream, sizeof(stream));
  
  printf("Random number stream (in C format):\n");

  printf("{ ");
  for (int i = 0; i < 8; i++)
  {
    if (i > 0)
      printf(", ");
    printf("0x%02x", stream[i]);
  }
  printf(" } \n");
}

void loop() {
}
