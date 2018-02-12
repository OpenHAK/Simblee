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

/*
This sketch demonstrates how to connect two Simblee's together and communicate 
directionally over SPI with multiple buffers.

The first Simblee should run SPI_Master_Multiple_Buffers.
The second Simblee should run SPIS_Slave_Multiple_Buffers.
Open the serial monitor on SPI_Master.
Press send.
Three messages are sent from the Master in quick succession.
The three messages are displayed on the serial monitor of the Slave.

The pins should be connected as follows:

Master Simblee         Slave Simbleee
     GND       ------>     GND
   SS2 (20)    ------>   SS2 (20)
  MOSI2 (23)   ------>  MOSI2 (23)
  MISO2 (21)   ------>  MISO2 (21)
  SCK2 (24)    ------>  SCK2 (24)
*/

#include <SPI.h>

char *messages[3] = { "one", "two", "three" };

char buf[10];
int n;

void setup()
{
  Serial.begin(9600);

  // slave select is an output (initialize high)
  digitalWrite(SS2, HIGH);
  pinMode(SS2, OUTPUT);

  // override the default SPI2 pins
  // SPI2.setSCK(24);
  // SPI2.setMOSI(23);
  // SPI2.setMISO(21);

  SPI2.begin();
  SPI2.setFrequency(4000);
  SPI2.setDataMode(SPI_MODE0);
  SPI2.setBitOrder(MSBFIRST);

  printf("master ready\n");
  printf("press a key to send messages\n");
}

void loop()
{
  if (Serial.available())
  {
    // works will all possible serial monitor line endings
    while (Serial.available())
    {
      Serial.read();
      delay(10);
    }

    printf("sending three messages\n");

    for (int i = 0; i < countof(messages); i++)
    {
      strcpy(buf, messages[i]);

      digitalWrite(SS2, LOW);
      SPI2.transfer(buf, strlen(buf)+1);  // transmit the trailing '\0'
      digitalWrite(SS2, HIGH);

      // give slave just enough time to switch buffers
      // (if this delay is omitted, then message two is ignored)
      delayMicroseconds(5);
    }

    printf("sent\n");
  }
}