// Simblee TCP Client over SimbleeForMobile

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

#ifndef SimbleeForMobileClient_h
#define SimbleeForMobileClient_h

#include "Arduino.h"
#include "Print.h"
#include "Client.h"
#include "IPAddress.h"

class SimbleeForMobileClient : public Client
{
private:
  uint8_t _sock;

private:
  bool init();

public:
  SimbleeForMobileClient();
  SimbleeForMobileClient(uint8_t sock);

  // uint8_t status();
  virtual int connect(IPAddress ip, uint16_t port);
  virtual int connect(const char *host, uint16_t port);
  virtual size_t write(uint8_t);
  virtual size_t write(const uint8_t *buf, size_t size);
  virtual int available();
  virtual int read();
  virtual int read(uint8_t *buf, size_t size);
  virtual int peek();
  virtual void flush();
  virtual void stop();
  virtual uint8_t connected();
  virtual operator bool();
  virtual bool isAvailable(int avail);
  virtual bool operator==(const bool value) { return bool() == value; }
  virtual bool operator!=(const bool value) { return bool() != value; }
  virtual bool operator==(const SimbleeForMobileClient&);
  virtual bool operator!=(const SimbleeForMobileClient& rhs) { return !this->operator==(rhs); };

  using Print::write;
};

#endif  // SimbleeForMobileClient_h
