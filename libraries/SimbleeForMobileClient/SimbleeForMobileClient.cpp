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

#include "SimbleeForMobileClient.h"

#include "../SimbleeForMobile/SimbleeForMobile.h"

SimbleeForMobileClient::SimbleeForMobileClient() : _sock(SFM_MAX_SOCK_NUM)
{
}

SimbleeForMobileClient::SimbleeForMobileClient(uint8_t sock) : _sock(sock)
{
}

/*
uint8_t SimbleeForMobileClient::status()
{
  return (SimbleeForMobile.connected(_sock) ? SnSR::ESTABLISHED :: SnSR::CLOSED);
}
*/

// arduino (wifi/ethernet) ssl pattern:
// - XXXClient contains connect() and connectSSL()
// - XXXSSLClient (base class XXXClient) connect() calls connectSSL() in base
// SimbleeForMobile connect() is ssl for port 443, non-ssl for everything else
// SUCCESS 1
// (INVALID) 0
// TIMED_OUT -1
// INVALID_SERVER -2
// TRUNCATED -3
// INVALID_RESPONSE -4
int SimbleeForMobileClient::connect(IPAddress ip, uint16_t port)
{
  stop();

  if (_sock == SFM_MAX_SOCK_NUM)
  {
    for (_sock = 0; _sock < SFM_MAX_SOCK_NUM; _sock++)
      if (! SimbleeForMobile.socketUsed(_sock))
        break;

    if (_sock == SFM_MAX_SOCK_NUM)
      return -5;
  }

  if (SimbleeForMobile.socketConnectv4(_sock, ip, port))
    return 1;

  int error = SimbleeForMobile.socketError(_sock);

  stop();

  if (error == 60)
    return -1;
  if (error == 61)
    return -2;
  // printf("error: %d\n", error);

  return 0;
}

// "connect.simbleecloud.com" (must use a hostname for SSL, and it must match the SSL cert)
// "45.55.28.194"
// "2604:a880:1:20::71:4001"
// SUCCESS 1
// (INVALID) 0
// TIMED_OUT -1
// INVALID_SERVER -2
// TRUNCATED -3
// INVALID_RESPONSE -4
int SimbleeForMobileClient::connect(const char *host, uint16_t port)
{
  stop();
  
  if (_sock == SFM_MAX_SOCK_NUM)
  {
    for (_sock = 0; _sock < SFM_MAX_SOCK_NUM; _sock++)
      if (! SimbleeForMobile.socketUsed(_sock))
        break;

    if (_sock == SFM_MAX_SOCK_NUM)
      return -5;
  }

  if (SimbleeForMobile.socketConnect(_sock, host, port))
    return 1;

  int error = SimbleeForMobile.socketError(_sock);

  stop();

  if (error == 60)
    return -1;
  if (error == 2)
    return -2;
  // printf("error: %d\n", error);

  return 0;
}

size_t SimbleeForMobileClient::write(uint8_t b)
{
  // repeated calls to this method is very inefficient
  return write(&b, 1);
}

size_t SimbleeForMobileClient::write(const uint8_t *buf, size_t size)
{
  if (_sock == SFM_MAX_SOCK_NUM)
    return 0;

  SimbleeForMobile.socketSend(_sock, buf, size);

  return size;
}

// this method requests more data when the buffer is empty (ie: process the data byte by byte)
// to process data by packet/block/message, use isAvailble()
int SimbleeForMobileClient::available()
{
  if (_sock == SFM_MAX_SOCK_NUM)
    return -1;

  return SimbleeForMobile.socketAvailable(_sock);
}

int SimbleeForMobileClient::read()
{
  uint8_t b;

  if (read(&b, 1) < 1)
    return -1;

  return b;
}

int SimbleeForMobileClient::read(uint8_t *buf, size_t size)
{
  if (_sock == SFM_MAX_SOCK_NUM)
    return -1;

  int n = SimbleeForMobile.socketReceive(_sock, buf, size);

  return (n < 1 ? -1 : n);
}

int SimbleeForMobileClient::peek()
{
  if (_sock == SFM_MAX_SOCK_NUM)
    return -1;

  return SimbleeForMobile.socketPeek(_sock);
}

void SimbleeForMobileClient::flush()
{
  // do nothing (no one calls this in the standard pattern anyway)
}

void SimbleeForMobileClient::stop()
{
  if (_sock == SFM_MAX_SOCK_NUM)
    return;

  SimbleeForMobile.socketClose(_sock);

   _sock = SFM_MAX_SOCK_NUM;
}

uint8_t SimbleeForMobileClient::connected()
{
  if (_sock == SFM_MAX_SOCK_NUM)
    return 0;

  return SimbleeForMobile.socketConnected(_sock);
}

// the next function allows us to use the client returned by
// SimbleeForMobileClient::available() as the condition in an if-statement.

SimbleeForMobileClient::operator bool()
{
  return (_sock != SFM_MAX_SOCK_NUM);
}

// use this method to process data by packet/block/message
// this method may request more data, based on the amount of data needed
bool SimbleeForMobileClient::isAvailable(int avail)
{
  if (_sock == SFM_MAX_SOCK_NUM)
    return false;

  return SimbleeForMobile.socketIsAvailable(_sock, avail);
}

bool SimbleeForMobileClient::operator==(const SimbleeForMobileClient& rhs)
{
  return (_sock != SFM_MAX_SOCK_NUM && rhs._sock != SFM_MAX_SOCK_NUM && _sock == rhs._sock);
}
