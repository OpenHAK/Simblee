/*
 * Copyright by RF Digital Corp. 2017.
 * www.RFDigital.com
 * 
 * THE SOURCE CODE IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND.
 */

#ifndef _SPIS_H_INCLUDED
#define _SPIS_H_INCLUDED

#include "../SPI/SPI.h"

class SPISClass {
public:
  SPISClass(NRF_SPIS_Type *_spi, int _pinCS, int _pinSCK, int _pinMOSI, int _pinMISO);

  // SPI Configuration methods

  void begin(char *rxBuffer, char *txBuffer, int len);
  void end(void);

  void setBitOrder(BitOrder _bitOrder);
  void setDataMode(uint8_t _dataMode);

  void setCPOL(bool _activeHigh);
  void setCPHA(bool _leading);

  void setBuffers(char *rxBuffer, char *txBuffer, int len);

  int setCS(int _pinCS);
  int setSCK(int _pinSCK);
  int setMOSI(int _pinMOSI);
  int setMISO(int _pinMISO);

private:
  NRF_SPIS_Type *spis;
  int pinCS;
  int pinSCK;
  int pinMOSI;
  int pinMISO;
};

extern void SPIS_onReceive(int bytes_received, bool overflow)  __attribute__((weak));

extern SPISClass SPIS;

#endif
