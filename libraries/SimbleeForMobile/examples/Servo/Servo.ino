/*
The sketch accepts a Bluetooth Low Energy 4 connection from an
iPhone and accepts commands from the iPhone to run upto 4
standard servos.

This sketch is suppose to work with the simbleeServo application.

It receives two bytes from the iPhone.  The first byte contains
the servos to set (bit1 = servo a, bit2 = servo b, etc), and
the value is the number of degrees (0-180) to position the servo
too.
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

#include <Servo.h>
#include <SimbleeForMobile.h>

Servo s1;
Servo s2;
Servo s3;
Servo s4;

void setup() {
  s1.attach(2);
  s2.attach(3);
  s3.attach(4);
  s4.attach(5);

  SimbleeForMobile.advertisementInterval = 375;
  SimbleeForMobile.advertisementData = "-servo";
  
  // use a shared cache
  SimbleeForMobile.domain = "Simblee.com";
  
  SimbleeForMobile.begin();
}

void loop() {
  // todo: Simblee_ULPDelay(INFINITE);
  
  // process must be called in the loop for SimbleeForMobile
  SimbleeForMobile.process();
}

int8_t servo;
uint8_t value[4];

uint8_t output;
uint8_t textfield;
uint8_t stepper;
uint8_t slider;
uint8_t quickset;

uint8_t eventID = 0;

void ui()
{
  // initial output selected
  servo = 0;

  // initial servo values  
  for (uint8_t i = 0; i < 4; i++)
    value[i] = 90;

  SimbleeForMobile.beginScreen(WHITE);
  
  SimbleeForMobile.drawText(110, 100, "Servo Output:");

  char *titles1[] = { "A", "B", "C", "D" };
  output = SimbleeForMobile.drawSegment(20, 150, 280, titles1, countof(titles1));

  SimbleeForMobile.drawText(40, 280, "Servo Position (0-180 degrees):");

  SimbleeForMobile.drawText(20, 331, "Value:");
  textfield = SimbleeForMobile.drawTextField(100, 325, 60, 90);
  stepper = SimbleeForMobile.drawStepper(200, 325, 100, 0, 180);
  
  slider = SimbleeForMobile.drawSlider(20, 380, 280, 0, 180);
  
  char *titles2[] = { "0", "90", "180" };
  quickset = SimbleeForMobile.drawSegment(20, 430, 280, titles2, countof(titles2));  
  
  SimbleeForMobile.endScreen();

  // populate initial values
  SimbleeForMobile.updateValue(output, servo);
  update_ui(value[servo]);
}

void move_servo()
{
  uint8_t val = value[servo];

  // degrees must be in the range 0-179
  if (val == 180)
    val = 179;
  
  if (servo == 0)
    s1.write(val);
  else if (servo == 1)
    s2.write(val);
  else if (servo == 2)
    s3.write(val);
  else if (servo == 3)
    s4.write(val);
}

void update_ui(uint8_t val)
{
  value[servo] = val;
  
  if (val == 0)
    SimbleeForMobile.updateValue(quickset, 0);
  else if (val == 90)
    SimbleeForMobile.updateValue(quickset, 1);
  else if (val == 180)
    SimbleeForMobile.updateValue(quickset, 2);
  else
    SimbleeForMobile.updateValue(quickset, -1);  // don't select anything

  if(eventID != textfield) SimbleeForMobile.updateValue(textfield, val);

  if(eventID != stepper) SimbleeForMobile.updateValue(stepper, val);
  
  if(eventID != slider) SimbleeForMobile.updateValue(slider, val);

  move_servo();
}

void ui_event(event_t &event)
{
  eventID = event.id;
  
  if (event.id == output)
  {
    servo = event.value;
    update_ui(value[servo]);
  }
  else if (event.id == textfield || event.id == stepper || event.id == slider)
    update_ui(event.value);
  else if (event.id == quickset)
  {
    if (event.value == 0)
      update_ui(0);
    else if (event.value == 1)
      update_ui(90);
    else if (event.value == 2)
      update_ui(180);
  }
}
