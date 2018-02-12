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

#ifndef SimbleeCloud_h
#define SimbleeCloud_h

#include "Arduino.h"
#include "Client.h"

class SimbleeCloud
{
private:
  Client *_client;
  bool _active;

public:
  unsigned int myESN;
  unsigned int userID;

public:
  bool active() { return _active; }

public:
  SimbleeCloud(Client *client);

  bool connect(const char *hostname);
  bool connect();
  void disconnect();

  void send(unsigned int destESN, const void *data, int len);
  void process();

  void countAsync(unsigned int esn);
};

/* optional callbacks */

__attribute__((weak))
void SimbleeCloud_onInitial(
  unsigned int originESN);

__attribute__((weak))
void SimbleeCloud_onReceive(
  unsigned int originESN,
  const unsigned char *payload,
  int len);

__attribute__((weak))
void SimbleeCloud_onCountResult(
  int count);

#endif  // SimbleeCloud_h
