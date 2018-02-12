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

#include <SPI.h>

#define  READY  25

// increase buffer size to test SPIS_Slave overflow
char buf[10];
int n;

void setup()
{
  Serial.begin(9600);

  // slave select is an output (initialize high)
  digitalWrite(SS2, HIGH);
  pinMode(SS2, OUTPUT);

  pinMode(READY, INPUT_PULLDOWN);

  // override the default SPI2 pins (if needed)
  // SPI2.setSCK(24);
  // SPI2.setMOSI(23);
  // SPI2.setMISO(21);

  SPI2.begin();
  // maximum throughput at 4MHz is about 127,500 bytes/sec
  SPI2.setFrequency(4000);
  SPI2.setDataMode(SPI_MODE0);
  SPI2.setBitOrder(MSBFIRST);

  printf("master ready\n");
  printf("send up to 10 characters to reverse\n");
}

void loop()
{
  if (Serial.available())
  {
    // clear the buffer for user input
    memset(buf, 0, sizeof(buf));
    
    // works will all possible serial monitor line endings
    n = 0;
    while (Serial.available())
    {
      char ch = Serial.read();
      if (ch != '\r' && ch != '\n')
        if (n < sizeof(buf))
          buf[n++] = ch;
      delay(10);
    }

    // don't send zero bytes
    if (! n)
      return;

    // send request to slave
    printf("sending %d bytes: %s\n", n, buf);
    digitalWrite(SS2, LOW);
    SPI2.transfer(buf, n);
    digitalWrite(SS2, HIGH);

    // wait until the slave is ready
    uint32_t start = millis();
    while (!digitalRead(READY) && millis() - start < 10)
      ;
    if (millis() - start >= 10)
    {
      printf("ready timeout after 10ms\n");
      return;
    }

    // read response from slave
    buf[0] = 0;  // indicate to slave that the master is reading a response
    digitalWrite(SS2, LOW);
    // if the response was a different size than the request, then the slave
    // could load tx[0] with the response length, and put the data in tx[1 ... n]
    // the master would first receive the response length as follows:
    // n = SPI2.transfer(0);
    SPI2.transfer(buf, n);
    digitalWrite(SS2, HIGH);
    printf("received %d bytes: %s\n", n, buf);
  }
}
