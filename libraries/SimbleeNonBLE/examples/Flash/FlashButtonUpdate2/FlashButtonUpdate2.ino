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
This sketch demonstrates another technique for updating
flash memory over multiple restarts of the Simblee.
*/

// pin 3 on the RGB shield is the green led
int led = 3;

// pin 5 on the RGB shield is button 1
int button = 5;

// debounce time (in ms)
int debounce_time = 10;

// maximum debounce timeout (in ms)
int debounce_timeout = 100;

// magic number unnecessary because flash cannot be uninitialized
struct data_t
{
  int count;
};

// the flash page is dynamic (uses the first available flash page after the last sketch page)
// use __attribute__((aligned(1024)) to push a variable onto a separate flash page
// note: the flash page will be reset every time the code is uploaded
__attribute__((section(".flash")))
struct data_t value =
{ 
  0
};

void setup() {
  Serial.begin(57600);
  
  // led turned on/off when the button is pressed to indicate
  // flash memory was incremented
  pinMode(led, OUTPUT);

  // button to increment flash memory
  pinMode(button, INPUT);
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
  Serial.println(value.count);
  
  delay_until_button(HIGH);
  digitalWrite(led, HIGH);

  // make a temporary copy of value in RAM
  struct data_t temp = value;
  
  // update temporary copy
  temp.count++;

  // erase the flash page  
  flashPageErase(PAGE_FROM_ADDRESS(&value));  
  
  // write updated temporary copy back to flash
  flashWriteBlock(&value, &temp, sizeof(temp)); 
  
  delay_until_button(LOW);
  digitalWrite(led, LOW);
}

