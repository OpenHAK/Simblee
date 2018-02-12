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
This sketch demonstrates updating flash memory over multiple
restarts of the Simblee.
*/

// select a flash page that isn't in use (see Memory.h for more info)
#define  MY_FLASH_PAGE  251

// pin 3 on the RGB shield is the green led
int led = 3;

// pin 5 on the RGB shield is button 1
int button = 5;

// debounce time (in ms)
int debounce_time = 10;

// maximum debounce timeout (in ms)
int debounce_timeout = 100;

struct data_t
{
  // we will use java's famous 0xCAFEBABE magic number to indicate
  // that the flash page has been initialized previously by our
  // sketch
  int magic_number;
  int count;
};

struct data_t *flash = (data_t*)ADDRESS_OF_PAGE(MY_FLASH_PAGE);

void flashSave(int count)
{
  flashPageErase(MY_FLASH_PAGE);
    
  struct data_t value;
  value.magic_number = 0xCAFEBABE;
  value.count = count;

  flashWriteBlock(flash, &value, sizeof(value)); 
}

void setup() {
  Serial.begin(57600);
  
  // led turned on/off when the button is pressed to indicate
  // flash memory was incremented
  pinMode(led, OUTPUT);

  // button to increment flash memory
  pinMode(button, INPUT);
  
  // if flash page is not initialized, initialize it
  if (flash->magic_number != 0xCAFEBABE)
    flashSave(0);
}

int debounce(int state)
{
  int start = millis();
  int debounce_start = start;
  
  while (millis() - start < debounce_timeout)
    if (digitalRead(button) == state)
    {
      if (millis() - debounce_start >= debounce_time)
        return 1;
    }
    else
      debounce_start = millis();

  return 0;
}

int delay_until_button(int state)
{
  // set button edge to wake up on
  if (state)
    Simblee_pinWake(button, HIGH);
  else
    Simblee_pinWake(button, LOW);
    
  do
    // switch to lower power mode until a button edge wakes us up
    Simblee_ULPDelay(INFINITE);
  while (! debounce(state));
  
  // if multiple buttons were configured, this is how you would determine what woke you up
  if (Simblee_pinWoke(button))
  {
    // execute code here
    Simblee_resetPinWake(button);
  }
}

void loop() {
  Serial.print("The current count value in flash is: ");
  Serial.println(flash->count);
  
  delay_until_button(HIGH);
  digitalWrite(led, HIGH);

  flashSave(flash->count + 1);
  
  delay_until_button(LOW);
  digitalWrite(led, LOW);
}

