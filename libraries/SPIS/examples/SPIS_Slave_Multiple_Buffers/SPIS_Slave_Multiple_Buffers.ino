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

#include <SPIS.h>

// rx buffer and tx buffer should be the same length (maximum size is 255)
char rx[3][10];
char tx[10];
volatile int head_buffer = 0;
volatile int tail_buffer = 0;

void setup()
{
  Serial.begin(9600);

  // override the default SPIS pins (if needed)
  // SPIS.setCS(20);
  // SPIS.setSCK(24);
  // SPIS.setMOSI(23);
  // SPIS.setMISO(21);

  // use rx buffer 0 initially
  SPIS.begin(rx[0], tx, sizeof(tx));
  // SPI slave hardware works for all frequencies
  SPIS.setDataMode(SPI_MODE0);
  SPIS.setBitOrder(MSBFIRST);

  printf("slave ready\n");
}

void loop()
{
  if (head_buffer != tail_buffer)
  {
    printf("received %s in buffer %d\n", rx[head_buffer], head_buffer);
    head_buffer++;
    if (head_buffer == countof(rx))
      head_buffer = 0;
  }
}

void SPIS_onReceive(int bytes_received, bool overflow)
{
  // use the next rx buffer
  tail_buffer++;
  if (tail_buffer == countof(rx))
    tail_buffer = 0;

  // switch rx buffer
  SPIS.setBuffers(rx[tail_buffer], tx, sizeof(tx));  
}
