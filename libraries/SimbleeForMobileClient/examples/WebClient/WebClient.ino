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

SimbleeForMobileClient client;

bool started = false;

void setup()
{
  Serial.begin(9600);
  SimbleeForMobile.begin();  
}

void ui()
{
  Serial.println("connecting...");

  if (client.connect("www.simblee.com", 443)) {
    Serial.println("connected");
    client.println("GET /webclient.html HTTP/1.1");
    client.println("Host: www.simblee.com");
    client.println("Connection: close");
    client.println();
  } else {
    Serial.println("connection failed");
  }
  
  started = true;
}

void loop()
{
  // process must be called in the loop for SimbleeForMobile
  SimbleeForMobile.process();

  if (started) { 
    if (client.available()) {
      char c = client.read();
      Serial.print(c);
    }

    if (!client.connected()) {
      Serial.println();
      Serial.println("disconnecting.");
      client.stop();
      started = false;
    }
  }
}
