/*
ProximityMovingAverage
by Simblee
12/6/2013

This sketch is for Bluetooth Low Energy 4 RSSI signal strength
investigation.

The sketch displays a row on the serial monitor for each RSSI 
sample containing:
- the current RSSI moving average
- an estimated distance in ft
- the intensity value used for the led

The distance is an estimate, and the accuracy will vary based
on the accuracy of the samples provided and the wireless
characteristics of your environment.  It will most likely only
be accurate to a few feet, as triangulation is not used.
However, it does provide some entertainment value.

To start the RSSI sampling, use any of the iPhone apps and
establish a connection with the sketch.

RSSI information is available to the Simblee after a connection
has been established.  The iPhone application is able to access
the RSSI value prior to connection if needed.
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

#include <SimbleeBLE.h>

float average = 1.0;  // must be outside value range (-0dBm to -127dBm)

// number of samples in the average
int samples = 20;

// sample #1 from environment (rssi -45dBm = distance 1.5ft)
float r1 = -45;
float d1 = 1.5;

// sample #2 from environment (rssi -75dBm = distance 20ft)
float r2 = -75;
float d2 = 20;

// constant to account for loss due to reflection, polzarization, etc
// n will be ~2 for an open space
// n will be ~3 for "typical" environment
// n will be ~4 for a cluttered industrial site
float n = (r2 - r1) / (10 * log10(d1 / d2));

// green led on the rgb shield
int led = 3;

void setup() {
  pinMode(led, OUTPUT);
  
  Serial.begin(57600);
  
  // dump the value of n in case your curious
  Serial.println(n);
  
  // start the BLE stack
  SimbleeBLE.begin();
}

void loop() {
  // switch to lower power mode
  Simblee_ULPDelay(INFINITE);
}

void SimbleeBLE_onConnect()
{
}

void SimbleeBLE_onDisconnect()
{
  digitalWrite(led, LOW);
}

// returns the dBm signal strength indicated by the receiver
// received signal strength indication (-0dBm to -127dBm)
void SimbleeBLE_onRSSI(int rssi)
{
  // initialize average with instaneous value to counteract the initial smooth rise
  if (average == 1.0)
    average = rssi;

  // single pole low pass recursive IIR filter
  average = rssi * (1.0 / samples) + average * (samples - 1.0) / samples;

  // approximate distance  
  float distance = d1 * pow(10, (r1 - average) / (10 * n));

  // intensity (based on distance[linear] not rssi[logarithmic])
  float d = distance;
  if (d < d1)
    d = d1;
  else if (d > d2)
    d = d2;
  int intensity = (d2 - d) / (d2 - d1) * 255;
  analogWrite(led, intensity);

  Serial.print((int)average);
  Serial.print("\t");
  Serial.print((int)distance);
  Serial.print("\t");
  Serial.print(intensity);
  Serial.println("");
}

