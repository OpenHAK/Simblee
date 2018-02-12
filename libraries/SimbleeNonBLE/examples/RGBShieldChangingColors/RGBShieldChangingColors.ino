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

int led_red = 2;
int led_green = 3;
int led_blue = 4;
int led_yellow = 5;
int i = 0;
int mode = 0;

// the setup routine runs once when you press reset:
void setup() {                
  pinMode(led_red, OUTPUT);
  pinMode(led_green, OUTPUT);
  pinMode(led_blue, OUTPUT);
  pinMode(led_yellow, OUTPUT);
}

// the loop routine runs over and over again forever:
void loop() {
  
  analogWrite(led_red, i);
  analogWrite(led_green, (255 - i));
  analogWrite(led_blue, i);
  analogWrite(led_yellow, (255 - i));

  if (mode == 0)
  {
    if (++i >= 255)
    {
      mode = 1;
    }
  }
  else
  {
    if (--i == 0)
    {
      mode = 0;
    }
  }
  delay(25);
}

