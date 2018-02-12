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

#include "SimbleeCloud.h"

#define  MCS_FUNCTION_CALL  0xffffffff

#define  MAX_MODULE_PAYLOAD  180

enum
{
  MCS_FUNCTION_NONE,
  MCS_FUNCTION_PING,
  MCS_FUNCTION_PONG,
  MCS_FUNCTION_COUNT,
  MCS_FUNCTION_INITIAL
};

// arduino (wifi/ethernet) ssl pattern:
// - XXXClient contains connect() and connectSSL()
// - XXXSSLClient (base class XXXClient) connect() calls connectSSL() in base
SimbleeCloud::SimbleeCloud(Client *client)
{
  _client = client;

  myESN = Simblee_getESN();
  userID = 0;

  _active = false;
}

bool SimbleeCloud::connect(const char *hostname)
{
  if (_client->connect(hostname, 443) == 1)
  {
    _active = true;

    // start byte
    _client->write((const uint8_t *)"!", 1);

    // start message (my esn, then user id for authorization)
    send(myESN, &userID, 4);
  }
  else
    _active = false;

  return _active;
}

bool SimbleeCloud::connect()
{
  return connect("connect.simbleecloud.com");
}

void SimbleeCloud::disconnect()
{
  _client->stop();
  _active = false;
}

void SimbleeCloud::send(unsigned int destESN, const void *data, int len)
{
  if (len > MAX_MODULE_PAYLOAD)
    len = MAX_MODULE_PAYLOAD;

  uint8_t msg[1 + 4 + MAX_MODULE_PAYLOAD];
  size_t msg_len = 1 + 4 + len;

  msg[0] = msg_len;
  memcpy(msg + 1, &destESN, 4);
  memcpy(msg + 5, data, len);

  _client->write(msg, msg_len);
}

void SimbleeCloud::process()
{
  // nothing to process
  if (! _active)
    return;

  // detect disconnect from cloud
  if (! _client->connected())
  {
    disconnect();
    return;
  }

  // process inbound messages
  while (true)
  {
    if (_client->available() < 1)
      break;

    int msg_len = _client->peek();

    if (! _client->isAvailable(msg_len))
      break;

    // add 3 byte padding so origin_esn and payload are word aligned
    uint8_t buf[3 + 1 + 4 + MAX_MODULE_PAYLOAD];

    uint8_t *msg = buf + 3;

    if (msg_len > 1 + 4 + MAX_MODULE_PAYLOAD)
      msg_len = 1 + 4 + MAX_MODULE_PAYLOAD;

    _client->read(msg, msg_len);

    // msg[0] = msg_len
    // msg[1:4] = origin_esn
    // msg[5:n] = payload

    // uint32_t origin_esn = msg[1] | (msg[2] << 8) | (msg[3] << 16) | (msg[4] << 24);
    unsigned origin_esn = *(unsigned *)(msg + 1);
    if (origin_esn == MCS_FUNCTION_CALL)
    {
      uint8_t funct = msg[5];
      if (funct == MCS_FUNCTION_PING)
      {
        // retain conv area
        msg[5] = MCS_FUNCTION_PONG;
        _client->write(msg, msg_len);
      }
      else if (funct == MCS_FUNCTION_PONG)
      {
        // process pong
      }
      else if (funct == MCS_FUNCTION_COUNT)
      {
        if (SimbleeCloud_onCountResult)
        {
          int count;
          memcpy(&count, msg + 6, sizeof(count));
          SimbleeCloud_onCountResult(count);
        }
      }
      else if (funct == MCS_FUNCTION_INITIAL)
      {
        if (SimbleeCloud_onInitial)
        {
          unsigned int esn;
          memcpy(&esn, msg + 6, sizeof(esn));
          SimbleeCloud_onInitial(esn);
        }
      }

      // ignore unknown mcs function calls
    }
    else
    {
      if (SimbleeCloud_onReceive)
        SimbleeCloud_onReceive(origin_esn, msg + 5, msg_len - 5);
    }
  }
}

void SimbleeCloud::countAsync(unsigned int esn)
{
  uint8_t payload[1 + 4] = { MCS_FUNCTION_COUNT };
  memcpy(payload + 1, &esn, sizeof(esn));

  send(MCS_FUNCTION_CALL, payload, sizeof(payload));
}
