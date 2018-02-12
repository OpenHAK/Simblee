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

#include "SimbleeCOM.h"

char payload[15];

int packets = 0;
int errors = 0;

uint8_t ch = 0;

void setup()
{
  Serial.begin(9600);
  
  pinMode(5, INPUT);
  pinMode(6, INPUT);

  Simblee_pinWake(5, HIGH);
  Simblee_pinWake(6, HIGH);
  
  SimbleeCOM.begin();
}

void loop()
{
  if (Simblee_pinWoke(5))
  {
    printf("Starting test\n");
    ch = 0;
    for (int i = 0; i < 1000; i++)
    {
      memset(payload, ch, sizeof(payload));
      while (! SimbleeCOM.send(payload, sizeof(payload)))
        ;  // queue full
      ch++;
    }
    printf("Test complete\n");
    Simblee_resetPinWake(5);
  }
  
  if (Simblee_pinWoke(6))
  {
    printf("Results: packets=%d, errors=%d\n", packets, errors);
    packets = 0;
    errors = 0;
    ch = 0;
    Simblee_resetPinWake(6);
  }
}

void SimbleeCOM_onReceive(unsigned int esn, const char *payload, int len, int rssi)
{
  for (int i = 0; i < sizeof(payload); i++)
    if (payload[i] != ch)
    {
      errors++;
      ch = payload[i];
      break;
    }
  packets++;
  ch++;
}
