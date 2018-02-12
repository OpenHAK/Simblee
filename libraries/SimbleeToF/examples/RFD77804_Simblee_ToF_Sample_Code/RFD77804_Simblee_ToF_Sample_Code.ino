/*
   Copyright (c) 2017 RF Digital Corp. All Rights Reserved.

   The source code contained in this file and all intellectual property embodied in
   or covering the source code is the property of RF Digital Corp. or its licensors.
   Your right to use this source code and intellectual property is non-transferable,
   non-sub licensable, revocable, and subject to terms and conditions of the
   SIMBLEE SOFTWARE LICENSE AGREEMENT.
   http://www.simblee.com/licenses/SimbleeSoftwareLicenseAgreement.txt

   THE SOURCE CODE IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND.

   This heading must NOT be removed from this file.
*/

#include <Wire.h>
#include "SimbleeToF.h"
#include "RFDLogo_png.h"
#include <SimbleeForMobile.h>
#include <SimbleeBLE.h>

// image source numbers
#define RFD_LOGO 1
#define SIMBLEE_LOGO 2

// select a flash page that isn't in use (see Memory.h for more info)
#define  MY_FLASH_PAGE  251
#define  MY_FLASH_VALUE  0x12345678

// double level of indirection required to get gcc
// to apply the stringizing operator correctly
#define  str(x)   xstr(x)
#define  xstr(x)  #x

uint32_t *p = ADDRESS_OF_PAGE(MY_FLASH_PAGE);

#define RSSI_THRESHOLD  -40

#define RED_LED 2
#define GREEN_LED 3
#define BLUE_LED 4
#define BUTTONA 5
#define BUTTONB 6

SimbleeToF tof;

volatile bool isDualMode = false;
volatile bool gotMeasurementPacket = false;
volatile bool waitingForPairing = false;
volatile bool isPaired = false;
volatile bool startedDualMode = false;

// SimbleeForMobile stuff
int8_t currentScreen = -1;
uint8_t headerTextShadow, headerText;
uint8_t containerOutline, container;
uint8_t graph[10];
uint8_t dataPoints[44];
uint16_t pointData[44];
uint16_t pointsSize[44];
uint16_t pointsY[44];
uint8_t nextPoint = 0;
uint8_t graphTopLeftX = 72;
uint8_t graphTopLeftY = 86;
uint8_t graphWidth = 218;
uint16_t graphHeight = 353;
uint8_t howToPairButton, homeButton;

int myESN;

int16_t distance = 0;
uint8_t red, green, blue = 0;
float hue = 0;

struct myPayload {
  int16_t distance;
} __attribute__((packed));
myPayload packet;

void setup() {
  Serial.begin(115200);
  Wire.begin();
  Wire.beginTransmission(0x4C);
  Wire.write(0x00);
  uint8_t error = Wire.endTransmission();
  if (error != 0)
    isDualMode = false;
  else {
    if (tof.begin())
      isDualMode = true;
  }

  //  if (tof.begin())
  //    isDualMode = true;
  //  else
  //    isDualMode = false;

  if (isDualMode) {
    Serial.println("DUALMODE");
    SimbleeForMobile.advertisementData = "ToF Demo";
    SimbleeForMobile.begin();
  }
  else {
    Wire.end(); //turn off I2C so it doesn't interfere with buttons
    Serial.println("COM");
    myESN = SimbleeCOM.getESN();
    pinMode(RED_LED, OUTPUT);
    pinMode(GREEN_LED, OUTPUT);
    pinMode(BLUE_LED, OUTPUT);
    pinMode(BUTTONA, INPUT_PULLDOWN);
    pinMode(BUTTONB, INPUT_PULLDOWN);
    SimbleeCOM.mode = LOW_LATENCY;
    SimbleeCOM.begin();
    if (*p != 0xFFFFFFFF) {
      isPaired = true;
      Serial.println("already paired");
    }
    else {
      Serial.println("not paired");
      digitalWrite(RED_LED, HIGH); //to indicate that it has not yet been paired
    }
  }

}

void loop() {

  if (isDualMode) {
    if (!startedDualMode) {
      SimbleeBLE.dualModeBegin();
      startedDualMode = true;
    }

    packet.distance = tof.tof_simple_measurement();

    if (packet.distance > 2047) {
      packet.distance = 2047;
    }

    if (packet.distance < 0) {
      packet.distance = 0;
    }

    if (SimbleeForMobile.updatable && SimbleeForMobile.screen == 1) {
      pointData[nextPoint] = packet.distance;
      int16_t pixels = map(pointData[nextPoint], 0, 2047, graphHeight, 0);
      pointsY[nextPoint] = graphTopLeftY + pixels - 1;
      uint16_t prevPointSize = pointsSize[nextPoint];
      pointsSize[nextPoint] = graphHeight - pixels + 1;
      printf("\nNextPoint %d\nDistance: %d", nextPoint, pointData[nextPoint]);
      printf("pixels %d\n", pixels);
      printf("pointsY[nextPoint] %d\n", pointsY[nextPoint]);
      printf("Previous PointSize: %d\n", prevPointSize);
      printf("pointsSize[nextpoint] %d\n\n", pointsSize[nextPoint]);
      if (prevPointSize > pointsSize[nextPoint]) {
        SimbleeForMobile.updateH(dataPoints[nextPoint], pointsSize[nextPoint]);
        SimbleeForMobile.updateY(dataPoints[nextPoint], pointsY[nextPoint]);
      } else {
        SimbleeForMobile.updateY(dataPoints[nextPoint], pointsY[nextPoint]);
        SimbleeForMobile.updateH(dataPoints[nextPoint], pointsSize[nextPoint]);
      }

      nextPoint++;

      if (nextPoint == 44) {
        nextPoint = 0;
      }
    }
  }

  else {
    if (digitalRead(BUTTONA) && (isPaired == false)) {
      ledsOff();
      digitalWrite(BLUE_LED, HIGH); // to inicate that we are waiting for pairing now
      waitingForPairing = true;
    }
    if (digitalRead(BUTTONB) && (isPaired == true)) {
      ledsOff();
      flashPageErase(MY_FLASH_PAGE);
      digitalWrite(RED_LED, HIGH); // to inicate that we are unpaired
      isPaired = false;
    }
    if (gotMeasurementPacket) {
      if (packet.distance != -1) {
        hue = map(packet.distance, 0, 2047, 0, 255);
        analogWrite(RED_LED, hue);
        analogWrite(GREEN_LED, hue);
        analogWrite(BLUE_LED, hue);
      }
      gotMeasurementPacket = false;
    }
  }

  SimbleeForMobile.process();
}

void ui() {
  if (SimbleeForMobile.screen == currentScreen) {
    return;
  }

  SimbleeForMobile.beginScreen(0xE4E4E4);
  headerTextShadow = SimbleeForMobile.drawText(49, 26, "Simblee ToF Example", WHITE, 24);
  headerText = SimbleeForMobile.drawText(50, 25, "Simblee ToF Example", BLACK, 24);
  containerOutline = SimbleeForMobile.drawRect(19, 75, SimbleeForMobile.screenWidth - 38, SimbleeForMobile.screenHeight - 90, GRAY);
  container = SimbleeForMobile.drawRect(20, 76, SimbleeForMobile.screenWidth - 40, SimbleeForMobile.screenHeight - 92, WHITE);

  if (SimbleeForMobile.screen == 1) {
    // draw graph
    graph[0] = SimbleeForMobile.drawRect(graphTopLeftX, graphTopLeftY, graphWidth, graphHeight, 0xEEEEEE);
    graph[1] = SimbleeForMobile.drawRect(graphTopLeftX - 1, graphTopLeftY, 1, graphHeight + 1, GRAY);
    graph[2] = SimbleeForMobile.drawRect(graphTopLeftX - 6, graphTopLeftY + graphHeight, graphWidth + 6, 1, GRAY);

    // draw graph lines
    graph[3] = SimbleeForMobile.drawRect(graphTopLeftX - 6, graphTopLeftY, 5, 1, GRAY);
    graph[4] = SimbleeForMobile.drawRect(graphTopLeftX - 6, graphTopLeftY + (graphHeight / 2), 5, 1, GRAY);

    // draw graph text
    graph[5] = SimbleeForMobile.drawText(graphTopLeftX - 47, graphTopLeftY - 5, "2047 mm", GRAY, 10);
    graph[6] = SimbleeForMobile.drawText(graphTopLeftX - 46, graphTopLeftY + (graphHeight / 2) - 5, "1023 mm", GRAY, 10);
    graph[7] = SimbleeForMobile.drawText(graphTopLeftX - 32, graphTopLeftY + graphHeight - 6, "0 mm", GRAY, 10);

    // draw graph points
    for (uint8_t i = 0; i < 44; i++) {
      pointsSize[i] = 1;
      pointsY[i] = graphTopLeftY + graphHeight - 1;
      dataPoints[i] = SimbleeForMobile.drawRect(graphTopLeftX + (i * 5), pointsY[i] - 1, 3, pointsSize[i], BLUE);
      pointData[i] = 0;
    }

    //SimbleeForMobile.drawRect(graphTopLeftX + (1 * 5), graphTopLeftY + graphHeight - 1, 25, 10, RED);
    nextPoint = 0;

    // draw button
    howToPairButton = SimbleeForMobile.drawButton((SimbleeForMobile.screenWidth - 260) / 2, 455, 260, "How to Pair the Second Node", GRAY);
    SimbleeForMobile.setEvents(howToPairButton, EVENT_PRESS);
  } else if (SimbleeForMobile.screen == 2) {
    // draw instructions header
    SimbleeForMobile.drawText((SimbleeForMobile.screenWidth - 256) / 2, graphTopLeftY, "How To Pair the Second Node", BLACK, 20);
    SimbleeForMobile.drawText((SimbleeForMobile.screenWidth - 256) / 2, graphTopLeftY + 32, "Second Node Requires:\n" \
                              "\tRFD77201 Simblee 7 Pin Adapter\n\tRFD22122 RGB Led Button Shield\n\tRFD22126 Battery Shield", BLACK, 16);
    // draw instructions text                 
    SimbleeForMobile.drawText(37, graphTopLeftY + 110, "1. Turn on the second device. The red LED\n" \
                              "should be illuminated indicating that the\n" \
                              "device is not yet paired.", BLACK, 14);  //32
    SimbleeForMobile.drawText(37, graphTopLeftY + 162, "2. Press the left button on the second\n" \
                              "device. This initiated pairing mode as \nindicated by the blue light.", BLACK, 14);
    SimbleeForMobile.drawText(37, graphTopLeftY + 213, "3. Bring the second device within ~1ft of\n" \
                              "the main device.", BLACK, 14);
    SimbleeForMobile.drawText(37, graphTopLeftY + 247, "4. When the second device is within range\n" \
                              "of the main device, the green light will \nflash, indicating a successful pairing.", BLACK, 14);
    SimbleeForMobile.drawText(37, graphTopLeftY + 301, "5. When paired, the second device will turn\n the LED color" \
                             " white. The LED intensity will\nchange based on the distance measured.", BLACK, 14);
    // draw button
    homeButton = SimbleeForMobile.drawButton((SimbleeForMobile.screenWidth - 80) / 2, 455, 80, "Home", GRAY);
    SimbleeForMobile.setEvents(homeButton, EVENT_PRESS);
  }

  SimbleeForMobile.imageSource(RFD_LOGO, PNG, RFDLogo_png, RFDLogo_png_len);
  SimbleeForMobile.drawImage(RFD_LOGO, 110, 505);
  SimbleeForMobile.endScreen();
}

void ui_event(event_t &event) {
  if (SimbleeForMobile.screen == 1 && event.id == howToPairButton) {
    SimbleeForMobile.showScreen(2);
  } else if (SimbleeForMobile.screen == 2 && event.id == homeButton) {
    SimbleeForMobile.showScreen(1);
  }
}

void SimbleeCOM_onReceive(unsigned int esn, const char* payload, int len, int rssi) {
  if (!isDualMode) {
    // if not paired, and waiting for pairing and rssi is greater than threshold then lets pair
    if (!isPaired && waitingForPairing && (rssi > RSSI_THRESHOLD)) {
      flashPageErase(MY_FLASH_PAGE);
      flashWrite(p, esn);
      waitingForPairing = false;
      isPaired = true;
      printf("paired\n");
      pairBlink(5, 100);
    }

    // if we're paired and esn of packet matches then this is a measurment packet
    else if (isPaired && esn == *p ) {
      memcpy(&packet, payload, len);
      gotMeasurementPacket = true;
      printf("got measurement\n");
    }

    printf("%d ", rssi);
    printf("0x%08x ", esn);
    for (int i = 0; i < len; i++)
      printf("%02x ", payload[i]);
    printf("\n");
    //printf("%s\n", payload);
  }
}

void SimbleeBLE_onDualModeStart() {
  SimbleeCOM.send((char*)&packet, sizeof(packet));
}

void SimbleeBLE_onDualModeEnd() {}

void SimbleeForMobile_onConnect() {
  currentScreen = -1;
  //SimbleeBLE.dualModeBegin();
}

void SimbleeForMobile_onDisconnect() {
  //SimbleeBLE.dualModeEnd();
}

void ledsOff() {
  digitalWrite(RED_LED, LOW);
  digitalWrite(GREEN_LED, LOW);
  digitalWrite(BLUE_LED, LOW);
}

void pairBlink(uint8_t count, uint8_t delay_ms)
{
  ledsOff();
  for (int i = 0; i < count; i++) {
    digitalWrite(GREEN_LED, HIGH);
    delay(delay_ms);
    digitalWrite(GREEN_LED, LOW);
    delay(delay_ms);
  }
}
