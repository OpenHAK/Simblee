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

#include <SimbleeForMobile.h>

#define SIMBLEE_BLUE 0x387ab6

uint8_t counter = 1;

// SimbleeForMobile Object IDs
uint8_t ui_counter;
uint8_t ui_food;
uint8_t ui_box;

uint32_t lastTime;

void setup() {
  SimbleeForMobile.deviceName = "Objects";
  SimbleeForMobile.advertisementData = "Update";
  SimbleeForMobile.begin();
  lastTime = millis();
}

void loop() {
  if (millis() - lastTime >= 1000) {
    counter++;

    if (counter > 10) {
      counter = 1;
    }

    lastTime = millis();
  }

  if (SimbleeForMobile.updatable) {
    SimbleeForMobile.updateValue(ui_counter, counter);

    if (counter < 4) {
      SimbleeForMobile.updateText(ui_food, "Apples");
      SimbleeForMobile.updateColor(ui_counter, RED);
      SimbleeForMobile.updateOrigin(ui_box, 75, 250);
    } else if (counter >= 4 && counter < 7) {
      SimbleeForMobile.updateText(ui_food, "Bananas");
      SimbleeForMobile.updateColor(ui_counter, YELLOW);
      SimbleeForMobile.updateW(ui_box, 150);
      SimbleeForMobile.updateH(ui_box, 325);
    } else {
      SimbleeForMobile.updateText(ui_food, "Carrots");
      SimbleeForMobile.updateColor(ui_counter, GREEN);
      SimbleeForMobile.updateX(ui_box, 200);
      SimbleeForMobile.updateY(ui_box, 400);
    }
  }
  
  SimbleeForMobile.process();
}

void ui() {
  SimbleeForMobile.beginScreen(GRAY);
  SimbleeForMobile.drawText(100, 40, "Simblee", SIMBLEE_BLUE, 30);
  SimbleeForMobile.drawText(70, 120, "The Counter:", BLACK, 20);
  ui_counter = SimbleeForMobile.drawText(210, 120, counter, BLACK, 20);
  SimbleeForMobile.drawText(70, 160, "Food Groups:", BLACK, 20);
  ui_food = SimbleeForMobile.drawText(210, 160, "Apples", BLACK, 20);
  ui_box = SimbleeForMobile.drawRect(150, 250, 30, 30, BLUE);
  SimbleeForMobile.endScreen();
}

void SimbleeForMobile_onConnect() {
  counter = 1;
  lastTime = millis();
}

