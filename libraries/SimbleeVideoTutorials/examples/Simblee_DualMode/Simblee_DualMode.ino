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
#include <SimbleeBLE.h>

uint8_t ledGREEN = 3;
uint8_t ledStatus = 0;
uint8_t onButton = 5;
uint8_t offButton = 6;
uint8_t readOnButton = 0;
uint8_t readOffButton = 0;
bool ledStatusUpdate = false;

// SimbleeForMobile variables
color_t bg = 0x002BC8FC;
uint8_t ledRect;
uint8_t ledText;

void setup() {
  pinMode(ledGREEN, OUTPUT);
  pinMode(onButton, INPUT);
  pinMode(offButton, INPUT);
  SimbleeForMobile.advertisementData = "Dual Mode";
  SimbleeForMobile.begin();
  SimbleeBLE.dualModeBegin();
}

void update() {
  digitalWrite(ledGREEN, ledStatus);

  if (SimbleeForMobile.updatable) {
    SimbleeForMobile.updateColor(ledRect, (ledStatus ? GREEN : WHITE));
    SimbleeForMobile.updateColor(ledText, (ledStatus ? WHITE : BLACK));
    SimbleeForMobile.updateText(ledText, (ledStatus ? "On" : "Off"));
    SimbleeForMobile.updateX(ledText, (SimbleeForMobile.screenWidth / 2) - (ledStatus ? 5 : 8));
  }  
}

void loop() {
  if (digitalRead(onButton)) {
    if (!readOnButton) {
      if (!ledStatus) {
        ledStatus = 1;
        ledStatusUpdate = true;
        update();
      }
      readOnButton = 1;
    }
  } else {
    readOnButton = 0;
  }

  if (digitalRead(offButton)) {
    if (!readOffButton) {
      if (ledStatus) {
        ledStatus = 0;
        ledStatusUpdate = true;
        update();
      }
      readOffButton = 1;
    }
  } else {
    readOffButton = 0;
  }

  SimbleeForMobile.process();
}

void ui() {
  SimbleeForMobile.beginScreen(bg);
  ledRect = SimbleeForMobile.drawRect(40, 80, SimbleeForMobile.screenWidth - 80, SimbleeForMobile.screenWidth - 80, (ledStatus ? GREEN : WHITE));
  ledText = SimbleeForMobile.drawText((SimbleeForMobile.screenWidth / 2) - (ledStatus ? 5 : 8), 71 + ((SimbleeForMobile.screenWidth - 80) / 2), (ledStatus ? "On" : "Off"), (ledStatus ? WHITE : BLACK));
  SimbleeForMobile.setEvents(ledRect, EVENT_RELEASE);
  SimbleeForMobile.setEvents(ledText, EVENT_RELEASE); 
  SimbleeForMobile.endScreen();
}

void ui_event(event_t &event) {
  if (event.id == ledRect || event.id == ledText) {
    ledStatus = !ledStatus;
    ledStatusUpdate = true;
    update();
  }
}

void SimbleeCOM_onReceive(unsigned int esn, const char* payload, int len, int rssi) {
  if (payload[0] == '1') {
    ledStatus = 1;
  } else {
    ledStatus = 0;
  }
  update();
}

void SimbleeBLE_onDualModeStart() {
  if (ledStatusUpdate) {
    SimbleeCOM.send((ledStatus ? "1" : "0"), 1);
    ledStatusUpdate = 0;
  } else {
    SimbleeCOM.send("?", 1);
  }
}

void SimbleeBLE_onDualModeEnd() {}

