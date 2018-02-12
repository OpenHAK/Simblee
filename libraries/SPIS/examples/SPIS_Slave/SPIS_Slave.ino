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
directionally over SPI.

The first Simblee should run SPI_Master.
The second Simblee should run SPIS_Slave.
Open the serial monitor on SPI_Master.
Type a phrase (variable length, truncated at 10 characters).
Phrase is sent to SPI_Slave, reversed, and sent back to SPI_Master and displayed.

The pins should be connected as follows:

Master Simblee         Slave Simbleee
     GND       ------>     GND
   SS2 (20)    ------>   SS2 (20)
  MOSI2 (23)   ------>  MOSI2 (23)
  MISO2 (21)   ------>  MISO2 (21)
  SCK2 (24)    ------>  SCK2 (24)
  READY (25)   ------>  READY (25)
*/

#include <SPIS.h>

#define  READY  25

// rx buffer and tx buffer should be the same length (maximum size is 255)
char rx[10];
char tx[10];

void setup()
{
  Serial.begin(9600);

  pinMode(READY, OUTPUT);

  // override the default SPIS pins (if needed)
  // SPIS.setCS(20);
  // SPIS.setSCK(24);
  // SPIS.setMOSI(23);
  // SPIS.setMISO(21);

  SPIS.begin(rx, tx, sizeof(rx));
  // SPI slave hardware works for all frequencies
  SPIS.setDataMode(SPI_MODE0);
  SPIS.setBitOrder(MSBFIRST);

  printf("slave ready\n");
}

void loop()
{
}

void SPIS_onReceive(int bytes_received, bool overflow)
{
  // master sketch is compiled or the slave is reset
  if (! bytes_received)
    return;

  // sent by master to read tx buffer from slave
  if (! rx[0])
  {
    digitalWrite(READY, LOW);
    return;
  }
      
  // detect whether master sent more bytes than fit in my rx buffer
  if (overflow)
    printf("OVERFLOW DETECTED!\n");

  printf("received %d bytes: %.*s\n", bytes_received, bytes_received, rx);

  // reverse bytes received
  for (int i = 0; i < bytes_received; i++)
    tx[i] = rx[bytes_received - 1 - i];

  printf("sending %d bytes: %.*s\n", bytes_received, bytes_received, tx);

  // if the response was a different size than the request, then the slave
  // could load tx[0] with the response length, and put the data in tx[1 ... n]

  // indicate that the slave is ready
  digitalWrite(READY, HIGH);
}
