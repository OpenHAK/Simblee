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
#include "Cats_jpg.h"

#define CATS_IMG 1

uint8_t currentScreen;

// SimbleeForMobile Object IDs
uint8_t button;
uint8_t box;
color_t box_color;
uint8_t slider;
uint8_t textbox;
uint8_t screen_button1;
uint8_t image;

void setup() {
  SimbleeForMobile.deviceName = "Simblee";
  SimbleeForMobile.advertisementData = "Events";
  SimbleeForMobile.begin();
  Serial.begin(9600);
}

void loop() {
  SimbleeForMobile.process();
}

void drawScreen1() {
  SimbleeForMobile.beginScreen(YELLOW);
  button = SimbleeForMobile.drawButton(50, 50, SimbleeForMobile.screenWidth - 100, "Press me", BLUE);
  box = SimbleeForMobile.drawRect(100, 200, SimbleeForMobile.screenWidth - 200, SimbleeForMobile.screenWidth - 200, BLUE);
  box_color = BLUE;
  slider = SimbleeForMobile.drawSlider(50, 350, SimbleeForMobile.screenWidth - 100, 0, 255, BLUE);
  textbox = SimbleeForMobile.drawTextField(50, 400, SimbleeForMobile.screenWidth - 100, "", "Type a command...", BLUE);
  screen_button1 = SimbleeForMobile.drawButton(50, 450, SimbleeForMobile.screenWidth - 100, "Screen 2", BLUE);
  SimbleeForMobile.setEvents(button, EVENT_PRESS);
  SimbleeForMobile.setEvents(box, EVENT_DRAG | EVENT_POSITION);
  SimbleeForMobile.setEvents(slider, EVENT_DRAG | EVENT_RELEASE);
  SimbleeForMobile.setEvents(screen_button1, EVENT_PRESS);
  SimbleeForMobile.endScreen();
}

void drawScreen2() {
  SimbleeForMobile.beginScreen(WHITE);
  SimbleeForMobile.imageSource(CATS_IMG, JPG, Cats_jpg, Cats_jpg_len);
  image = SimbleeForMobile.drawImage(CATS_IMG, 25, 100);
  SimbleeForMobile.setEvents(image, EVENT_PRESS | EVENT_COLOR);
  SimbleeForMobile.endScreen();
}

void ui() {
  if (SimbleeForMobile.screen == currentScreen) {
    return;
  }

  currentScreen = SimbleeForMobile.screen;

  if (SimbleeForMobile.screen == 1) {
    drawScreen1();
  } else if (SimbleeForMobile.screen == 2) {
    drawScreen2();
  }
}

void ui_event(event_t &event) {
  if (event.id == textbox) {
    if (strcmp(event.text, "Hide") == 0) {
      SimbleeForMobile.setVisible(button, 0);
    } else if (strcmp(event.text, "Show") == 0) {
      SimbleeForMobile.setVisible(button, 1);
    }

    SimbleeForMobile.updateText(textbox, "");
  }
  
  if (event.type == EVENT_PRESS) {
    if (event.id == button) {
      color_t color = rand() % 0x00FFFFFF;
      SimbleeForMobile.updateColor(button, color);
    } else if (event.id == screen_button1) {
      SimbleeForMobile.showScreen(2);
    } else if (event.id == image) {
      Serial.print("Red: ");
      Serial.print(event.red, HEX);
      Serial.print(" Green: ");
      Serial.print(event.green, HEX);
      Serial.print(" Blue: ");
      Serial.print(event.blue, HEX);
      Serial.print(" Alpha: ");
      Serial.println(event.alpha, HEX);
    }
  } else if (event.type == EVENT_DRAG) {
    if (event.id == box) {
      Serial.print("X: ");
      Serial.print((int16_t) event.x);
      Serial.print(" Y: ");
      Serial.println((int16_t) event.y);
    } else if (event.id == slider) {
      color_t color = (event.value << 16) | (box_color & 0xFF00FFFF);
      SimbleeForMobile.updateColor(box, color);
      box_color = color;
    }
  } else if (event.type == EVENT_RELEASE) {
    if (event.id == slider) {
      Serial.print("Final Color: ");
      Serial.println(box_color, HEX);
    }
  }
}

void SimbleeForMobile_onConnect() {
  currentScreen = 0;
}

