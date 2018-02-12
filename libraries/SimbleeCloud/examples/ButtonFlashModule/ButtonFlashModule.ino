/*
This sketch demonstrates how to transmit data from a module into SimbleeCloud.

This sketch works in combination with the ButtonFlashPage.html (a multiple view
html page that demonstrates how to receive data from SimbleeCloud in html).

This sketch can optionally work in combination with the ButtonFlashService,
which demonstrates how data in SimbleeCloud can be processed by a php page.

To use this sketch, you will need to do the following:
1. Signup on admin.simbleecloud.com for an account (its free)
2. Add a module with your simblee factory esn (output on the serial monitor)
3. Add a service (with the esn 0x00000001) to https://service.simbleecloud.com
4. Add a pool (with the esn 0x00000002)
5. Compile and upload this sketch
6. Connect to your simblee with the SimbleeForMobile app on your smart phone
7. Open https://service.simbleecloud.com/ButtonFlashPage.html with an internet
   browser to view the results
*/

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

#include "SimbleeForMobile.h"
#include "SimbleeForMobileClient.h"
#include "SimbleeCloud.h"

// your user id from the admin.simbleecloud.com (required to authorize the module)
unsigned int userID = 0x8c7b58ac;

// ESN of module/service/pool you want to talk to in the SimbleeCloud
// (use: 0x00000001 to talk to the pool through the service, 
// or 0x00000002 to talk to the pool directly)
unsigned int destESN = 0x00000001;

SimbleeForMobileClient client;
SimbleeCloud cloud(&client);

int button = 5;  // Button A on the RGB Shield
int led = 3;     // Green led on the RGB Shield

// current button state
int state = 0;

// button debounce
int debounce = 0;
int debounce_time = 100;  // ms

// code to execute when button is pressed
void pressed();

void setup()
{
  Serial.begin(9600);
  
  pinMode(button, INPUT);
  pinMode(led, OUTPUT);

  printf("Module ESN is 0x%08x\n", cloud.myESN);
  cloud.userID = userID;

  SimbleeForMobile.begin();
}

void ui()
{
  if (cloud.connect())
    printf("active\n");
  else
    printf("failed\n");
}

void loop()
{
  // process must be called in the loop for SimbleeForMobile
  SimbleeForMobile.process();
  if (! SimbleeForMobile.active)
    return;

  // process must be called in the loop for SimbleeCloud
  cloud.process();
  if (! cloud.active())
    return;

  // state change in progress
  if (debounce > 0)
  {
    // debounce timer expired
    if (millis() - debounce >= debounce_time)
    {
      // transition to new state
      if (digitalRead(button) != state)
      {
        state = !state;  // invert state
        if (state)       // button is pressed
          pressed();
      }      
      debounce = 0;
    }
  }
  else
  {
    // if a button change is detected, start debounce
    if (digitalRead(button) != state)
      debounce = millis();
  }
}

void pressed()
{
  // turn the led on to indicate start of sending
  digitalWrite(3, HIGH);

  // send start message (ie: start the timer on the receiving side)
  cloud.send(destESN, "S", 1);

  for (int i = 1; i < 5; i++)
  {
    // send a '1' message
    cloud.send(destESN, "1", 1);
    // send a '0' message
    cloud.send(destESN, "0", 1);            
  }

  // send end message (ie: stop the timer on the receiving side)
  cloud.send(destESN, "E", 1);

  // turn the led off to indicate end of sending
  digitalWrite(led, LOW);
}
