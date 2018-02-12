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

#include <SPI.h>
#include <SD.h>
#include <SimbleeForMobile.h>

const int CS = 6;
uint8_t button;
uint8_t button2; 
uint32_t buttonDown = 0;
uint8_t readFile = 0;
File data;

void setup() {
  Serial.begin(9600);
  SimbleeForMobile.deviceName = "Simblee";
  SimbleeForMobile.advertisementData = "SD Card";
  SimbleeForMobile.begin();
  Serial.print("Initializing SD card... ");

  // check for card and initialize
  if (!SD.begin(CS)) {
    Serial.println("Could not initialize card.");
  }

  Serial.println("Card initialized.");

  // create a new file, deleting any old files
  if (SD.exists("data.txt")) {
    if (!SD.remove("data.txt")) {
      return;
    }
  }

  data = SD.open("data.txt", FILE_WRITE);

  // write the header to it
  if (data) {
    data.println("+-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-+");
    data.println("+ SimbleeForMobile SD Card Example  +");
    data.println("+-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-+");
    data.close();
  }
}

void loop() {
  if (readFile) {
    readSDFile();
    readFile = 0;
  }

  SimbleeForMobile.process();
}

void readSDFile() {
  if (data) {
    data.seek(0);

    while (data.available()) {
      Serial.write(data.read());
    }

    data.seek(data.size());
  }
}

void ui() {
  SimbleeForMobile.beginScreen(0xFF00979C);
  button = SimbleeForMobile.drawButton(100, 200, 120, "PRESS ME");
  button2 = SimbleeForMobile.drawButton(100, 400, 120, "READ FILE");
  SimbleeForMobile.setEvents(button, EVENT_PRESS | EVENT_RELEASE);
  SimbleeForMobile.setEvents(button2, EVENT_PRESS);
  SimbleeForMobile.endScreen();
}

void ui_event(event_t &event) {
  if (event.id == button) {
    if (event.type == EVENT_PRESS) {
      buttonDown = millis();
      data.print(" - Button pressed at: ");
      data.println(buttonDown);
    } else if (event.type == EVENT_RELEASE) {
      uint32_t tempTime = millis();
      data.print(" - Button released at: ");
      data.println(tempTime);
      data.print("   Total time pressed: ");
      data.println(tempTime - buttonDown);
      data.print("   Current temperature: ");
      data.print(Simblee_temperature(FAHRENHEIT));
      data.println(" F");
      data.println();
    }
  } else if (event.id == button2) {
    if (event.type == EVENT_PRESS) {
      readFile = 1;
    }
  }
}

void SimbleeForMobile_onConnect() {
  buttonDown = 0;
  data = SD.open("data.txt", FILE_WRITE);

  if (data) {
    data.seek(data.size());
    data.println();
    data.println("Capturing SimbleeForMobile events:");
  }
}

void SimbleeForMobile_onDisconnect() {
  data.close();
}

