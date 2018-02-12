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
This sketch demonstrates how to read and write a structure
to flash memory on the Simblee.

This example is located under SimbleeNonBLE, but the same
code can be used with Bluetooth Low Energy 4 functionality.
*/

// select a flash page that isn't in use (see Memory.h for more info)
#define  MY_FLASH_PAGE  251

// double level of indirection required to get gcc
// to apply the stringizing operator correctly
#define  str(x)   xstr(x)
#define  xstr(x)  #x

struct data_t
{
  char a;
  // the compiler will insert 3 bytes of padding here so b is aligned on a dword boundary
  int b;
  float c;
};

struct data_t value = { '1', 2, 3.0 };

void dump_data( struct data_t *p )
{
  Serial.print("  a = ");
  Serial.println(p->a);
  Serial.print("  b = ");
  Serial.println(p->b);
  Serial.print("  c = ");
  Serial.println(p->c);  
}

void setup() {
  Serial.begin(38400);
  Serial.println("All output will appear on the serial monitor.");

  // a flash page is 1K in length, so page 251 starts at address 251 * 1024 = 257024 = 3EC00 hex
  data_t *p = (data_t*)ADDRESS_OF_PAGE(MY_FLASH_PAGE);
  int rc;

  Serial.println("The data stored in flash page " str(MY_FLASH_PAGE) " contains: ");
  dump_data(p);
  
  Serial.print("Attempting to erase flash page " str(MY_FLASH_PAGE) ": ");
  rc = flashPageErase(PAGE_FROM_ADDRESS(p));
  if (rc == 0)
    Serial.println("Success");
  else if (rc == 1)
    Serial.println("Error - the flash page is reserved");
  else if (rc == 2)
    Serial.println("Error - the flash page is used by the sketch");

  Serial.println("The data stored in flash page " str(MY_FLASH_PAGE) " contains: ");
  dump_data(p);
  
  Serial.print("Attempting to write data to flash page " str(MY_FLASH_PAGE) ": ");
  rc = flashWriteBlock(p, &value, sizeof(value));
  if (rc == 0)
    Serial.println("Success");
  else if (rc == 1)
    Serial.println("Error - the flash page is reserved");
  else if (rc == 2)
    Serial.println("Error - the flash page is used by the sketch");

  Serial.println("The data stored in flash page " str(MY_FLASH_PAGE) " contains: ");
  dump_data(p);
}

void loop() {
}

