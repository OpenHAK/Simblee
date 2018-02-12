/*
 * Modified by RF Digital Corp. 2015.
 * www.RFDigital.com
 * 
 * THE SOURCE CODE IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND.
 */

/*
 * Copyright (c) 2010 by Cristian Maglie <c.maglie@bug.st>
 * SPI Master library for arduino.
 *
 * This file is free software; you can redistribute it and/or modify
 * it under the terms of either the GNU General Public License version 2
 * or the GNU Lesser General Public License version 2.1, both as
 * published by the Free Software Foundation.
 */

#include "SPI.h"

 SPIClass::SPIClass(NRF_SPI_Type *_spi, int _pinSCK, int _pinMOSI, int _pinMISO) : spi(_spi)
 {
    pinSCK = _pinSCK;
    pinMOSI = _pinMOSI;
    pinMISO = _pinMISO;
}

void SPIClass::begin()
{
    pinMode(pinSCK, OUTPUT);
    pinMode(pinMOSI, OUTPUT);
    pinMode(pinMISO, INPUT);

    spi->PSELSCK = pinSCK;
    spi->PSELMOSI = pinMOSI;
    spi->PSELMISO = pinMISO;

    // Default speed set to 4Mhz
    setFrequency(4000);
    setDataMode(SPI_MODE0);
    setBitOrder(MSBFIRST);

    spi->EVENTS_READY = 0;

    spi->ENABLE = (SPI_ENABLE_ENABLE_Enabled << SPI_ENABLE_ENABLE_Pos);
}

void SPIClass::usingInterrupt(int pinNumber)
{
  interruptMask = pinNumber;  
  interruptSave = (NRF_GPIO->PIN_CNF[pinNumber] >> GPIO_PIN_CNF_SENSE_Pos); //read current HIGH/LOW trigger
  interruptMode = 1;
}

void SPIClass::notUsingInterrupt(int pinNumber)
{
  interruptMask = pinNumber;  
  // if interrupt already disabled, then restore 
  if (interruptMode == 1) {
     switch (interruptSave) {
        case 2: // HIGH
        Simblee_pinWake(interruptMask, HIGH); 
        break;
        case 3: // LOW
        Simblee_pinWake(interruptMask, LOW);
        break;
        case 0: // DISABLED
        Simblee_pinWake(interruptMask, DISABLE);
        break;
        default:
        break;
    }
}
interruptMode = 0;
}

void SPIClass::end()
{
    spi->ENABLE &= ~(SPI_ENABLE_ENABLE_Enabled << SPI_ENABLE_ENABLE_Pos);
}

void SPIClass::beginTransaction(SPISettings settings)
{
    // disable interrupt
    if (interruptMode == 1) {
        Simblee_pinWake(interruptMask, DISABLE);
    }

    switch (settings.config_clkDiv) {
        case 0:
        setFrequency(8000);
        break;

        case 1:
        setFrequency(4000);
        break;

        case 2:
        setFrequency(2000);
        break;

        case 3:
        setFrequency(1000);
        break;

        case 4:
        setFrequency(500);
        break;

        case 5:
        setFrequency(250);
        break;

        case 6:
        setFrequency(125);
        break;

        default:
        setFrequency(4000);
        break;
    }

    setDataMode(settings.config_mode);

    if (settings.config_bitOrder)
        setBitOrder(MSBFIRST);
    else
        setBitOrder(LSBFIRST);
    
}

void SPIClass::endTransaction(void)
{
    //restore interrupt
    if (interruptMode == 1) {
        switch (interruptSave) {
        case 2: // HIGH
        Simblee_pinWake(interruptMask, HIGH); 
        break;
        case 3: // LOW
        Simblee_pinWake(interruptMask, LOW);
        break;
        case 0: // DISABLED
        Simblee_pinWake(interruptMask, DISABLE);
        break;
        default:
        break;
    }
}
}

uint16_t SPIClass::transfer16(uint16_t _data)
{
    uint8_t highByte, lowByte; 

    if(spi->CONFIG && 0x01)  //LSB first
    {
        spi->EVENTS_READY = 0;          //clear ready event
        // SPI is double buffered
        spi->TXD = _data & 0xFF;      //lower 8 bits
        spi->TXD = (_data >> 8);        // upper 8 bits
        while (spi->EVENTS_READY == 0) ; // wait for ready event
        spi->EVENTS_READY = 0;          //clear ready event
        lowByte = spi->RXD;
        while (spi->EVENTS_READY == 0) ; // wait for ready event
        spi->EVENTS_READY = 0;          //clear ready event
        highByte = spi->RXD;
    }
    else
    {
        spi->EVENTS_READY = 0;          //clear ready event
        // SPI is double buffered
        spi->TXD = (_data >> 8);        // upper 8 bits
        spi->TXD = _data & 0xFF;      //lower 8 bits
        while (spi->EVENTS_READY == 0) ; // wait for ready event
        spi->EVENTS_READY = 0;          //clear ready event
        highByte = spi->RXD;
        while (spi->EVENTS_READY == 0) ; // wait for ready event
        spi->EVENTS_READY = 0;          //clear ready event
        lowByte = spi->RXD;
    }

  uint16_t data = word(highByte, lowByte);

  return data;
}

void SPIClass::setBitOrder(BitOrder _bitOrder)
{
    if (_bitOrder == MSBFIRST)
        spi->CONFIG &= ~(SPI_CONFIG_ORDER_LsbFirst << SPI_CONFIG_ORDER_Pos);
    else
        spi->CONFIG |= (SPI_CONFIG_ORDER_LsbFirst << SPI_CONFIG_ORDER_Pos);
}

void SPIClass::setDataMode(uint8_t _dataMode)
{
  setCPOL((_dataMode & 0x02) >> 1);
  setCPHA(_dataMode & 0x01);
}

void SPIClass::setCPOL(bool _activeLow)
{
    if (_activeLow)
     spi->CONFIG |= (SPI_CONFIG_CPOL_ActiveLow << SPI_CONFIG_CPOL_Pos);
 else
     spi->CONFIG &= ~(SPI_CONFIG_CPOL_ActiveLow << SPI_CONFIG_CPOL_Pos);
}

void SPIClass::setCPHA(bool _trailing)
{
    if (_trailing)
     spi->CONFIG |= (SPI_CONFIG_CPHA_Trailing << SPI_CONFIG_CPHA_Pos);
 else
     spi->CONFIG &= ~(SPI_CONFIG_CPHA_Trailing << SPI_CONFIG_CPHA_Pos);
}

void SPIClass::setFrequency(int _speedKbps)
{
    if (_speedKbps == 125)
        spi->FREQUENCY = (SPI_FREQUENCY_FREQUENCY_K125 << SPI_FREQUENCY_FREQUENCY_Pos);
    else if (_speedKbps == 250)
        spi->FREQUENCY = (SPI_FREQUENCY_FREQUENCY_K250 << SPI_FREQUENCY_FREQUENCY_Pos);
    else if (_speedKbps == 500)
        spi->FREQUENCY = (SPI_FREQUENCY_FREQUENCY_K500 << SPI_FREQUENCY_FREQUENCY_Pos);
    else if (_speedKbps == 1000)
        spi->FREQUENCY = (SPI_FREQUENCY_FREQUENCY_M1 << SPI_FREQUENCY_FREQUENCY_Pos);
    else if (_speedKbps == 2000)
        spi->FREQUENCY = (SPI_FREQUENCY_FREQUENCY_M2 << SPI_FREQUENCY_FREQUENCY_Pos);
    else if (_speedKbps == 4000)
        spi->FREQUENCY = (SPI_FREQUENCY_FREQUENCY_M4 << SPI_FREQUENCY_FREQUENCY_Pos);
    else if (_speedKbps == 8000)
        spi->FREQUENCY = (SPI_FREQUENCY_FREQUENCY_M8 << SPI_FREQUENCY_FREQUENCY_Pos);
}

void SPIClass::setClockDivider(uint8_t _divider)
{
    if (_divider == SPI_CLOCK_DIV2)
        setFrequency(8000);
    else if (_divider == SPI_CLOCK_DIV4)
        setFrequency(4000);
    else if (_divider == SPI_CLOCK_DIV8)
        setFrequency(2000);
    else if (_divider == SPI_CLOCK_DIV16)
        setFrequency(1000);
    else if (_divider == SPI_CLOCK_DIV32)
        setFrequency(500);
    else if (_divider == SPI_CLOCK_DIV64)
        setFrequency(250);
    else if (_divider == SPI_CLOCK_DIV128)
        setFrequency(125);
}

byte SPIClass::transfer(uint8_t _data)
{
    spi->TXD = _data;

    while (spi->EVENTS_READY == 0)
      ;
  spi->EVENTS_READY = 0;

  _data = spi->RXD;

  return _data;
}

void SPIClass::transfer(void *buf, size_t count)
{
    if (count == 0) return;
    spi->EVENTS_READY = 0;
    uint8_t *p = (uint8_t *)buf;
    spi->TXD = *p;
    while (--count)
    {
        spi->TXD = *(p+1); 
        while (spi->EVENTS_READY == 0) ;
        spi->EVENTS_READY = 0;
        *p++ = spi->RXD;
    }
    while (spi->EVENTS_READY == 0) ;
        spi->EVENTS_READY = 0;
        *p = spi->RXD;
}

void SPIClass::attachInterrupt(void)
{
    // Should be enableInterrupt()
}

void SPIClass::detachInterrupt(void)
{
    // Should be disableInterrupt()
}

int SPIClass::setSCK(int _pinSCK)
{
    int old = pinSCK;
    pinSCK = _pinSCK;
    pinMode(pinSCK, OUTPUT);
    spi->PSELSCK = pinSCK;
    return old;
}

int SPIClass::setMOSI(int _pinMOSI)
{
    int old = pinMOSI;
    pinMOSI = _pinMOSI;
    pinMode(pinMOSI, OUTPUT);
    spi->PSELMOSI = pinMOSI;
    return old;
}

int SPIClass::setMISO(int _pinMISO)
{
    int old = pinMISO;
    pinMISO = _pinMISO;
    pinMode(pinMISO, INPUT);
    spi->PSELMISO = pinMISO;
    return old;
}

#if SPI_INTERFACES_COUNT > 0
SPIClass SPI(SPI_INTERFACE, PIN_SPI_SCK, PIN_SPI_MOSI, PIN_SPI_MISO);
SPIClass SPI2(SPI_INTERFACE_2, PIN_SPI_SCK_2, PIN_SPI_MOSI_2, PIN_SPI_MISO_2);
#endif
