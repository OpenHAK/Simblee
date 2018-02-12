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

/*
The sketch demonstrates various Serial.printf() calls.
*/

void setup() {
  char ch = 'A';
  int i = -123;
  double d = -123.456;
   
  Serial.begin(57600);
  
  Serial.printf("Hello World!\n");
  Serial.printf("ch = %c\n", ch);
  Serial.printf("i = %d, unsigned = %u, hex = %x, binary = %b\n", i, i, i, i);
  Serial.printf("d = %f\n", d);
  Serial.printf("string1 = %s, string2 = %s\n", "hi", F("hi"));

  // this works, but has been commented out for testing due to String impact on code size
  // Serial.printf("string3 = %s\n", String("hi").cstr());  
}

void loop() {
}
