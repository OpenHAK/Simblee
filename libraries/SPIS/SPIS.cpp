/*
 * Copyright by RF Digital Corp. 2017.
 * www.RFDigital.com
 * 
 * THE SOURCE CODE IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND.
 */

#include "SPIS.h"

extern "C" void SPI1_TWI1_Interrupt()
{
    NRF_SPIS_Type *spis = NRF_SPIS1;

    if (SPIS_onReceive)
    {
        int bytes_received = spis->AMOUNTRX;
        bool overflow = ((spis->STATUS & SPIS_STATUS_OVERFLOW_Msk) >> SPIS_STATUS_OVERFLOW_Pos);
        SPIS_onReceive(bytes_received, overflow);
    }

    spis->STATUS |= (SPIS_STATUS_OVERFLOW_Clear << SPIS_STATUS_OVERFLOW_Pos);

    spis->EVENTS_ACQUIRED = 0;

    spis->TASKS_RELEASE = 1;
}

SPISClass::SPISClass(NRF_SPIS_Type *_spis, int _pinCS, int _pinSCK, int _pinMOSI, int _pinMISO) : spis(_spis)
{
    pinCS = _pinCS;
    pinSCK = _pinSCK;
    pinMOSI = _pinMOSI;
    pinMISO = _pinMISO;
}

void SPISClass::begin(char *rxBuffer, char *txBuffer, int len)
{
    pinMode(pinCS, INPUT);
    pinMode(pinSCK, INPUT);
    pinMode(pinMOSI, INPUT);
    pinMode(pinMISO, INPUT);

    spis->PSELCSN = pinCS;
    spis->PSELSCK = pinSCK;
    spis->PSELMOSI = pinMOSI;
    spis->PSELMISO = pinMISO;

    setDataMode(SPI_MODE0);
    setBitOrder(MSBFIRST);

    spis->DEF = 0;  // default character (sent if semaphore not acquired)
    spis->ORC = 0;  // over-read character (master reads more than slave tx buffer bytes)

    // clear overflow error conditions
    spis->STATUS |= (SPIS_STATUS_OVERFLOW_Clear << SPIS_STATUS_OVERFLOW_Pos);

    // end / acquire shortcut (prevent rx buffer overwriting)
    spis->SHORTS = (SPIS_SHORTS_END_ACQUIRE_Enabled << SPIS_SHORTS_END_ACQUIRE_Pos);

    // 1 = high priority app, 3 = low priority app (required for printf)
    dynamic_attachInterrupt_priority(SPI1_TWI1_IRQn, SPI1_TWI1_Interrupt, 3);
    spis->INTENSET = (SPIS_INTENSET_ACQUIRED_Enabled << SPIS_INTENSET_ACQUIRED_Pos);

    spis->ENABLE = (SPIS_ENABLE_ENABLE_Enabled << SPIS_ENABLE_ENABLE_Pos);

    setBuffers(rxBuffer, txBuffer, len);

    spis->TASKS_RELEASE = 1;
}

void SPISClass::end()
{
    spis->ENABLE &= ~(SPIS_ENABLE_ENABLE_Enabled << SPIS_ENABLE_ENABLE_Pos);
}

void SPISClass::setBitOrder(BitOrder _bitOrder)
{
    if (_bitOrder == MSBFIRST)
        spis->CONFIG &= ~(SPIS_CONFIG_ORDER_LsbFirst << SPIS_CONFIG_ORDER_Pos);
    else
        spis->CONFIG |= (SPIS_CONFIG_ORDER_LsbFirst << SPIS_CONFIG_ORDER_Pos);
}

void SPISClass::setDataMode(uint8_t _dataMode)
{
    setCPOL((_dataMode & 0x02) >> 1);
    setCPHA(_dataMode & 0x01);
}

void SPISClass::setCPOL(bool _activeLow)
{
    if (_activeLow)
        spis->CONFIG |= (SPIS_CONFIG_CPOL_ActiveLow << SPIS_CONFIG_CPOL_Pos);
    else
        spis->CONFIG &= ~(SPIS_CONFIG_CPOL_ActiveLow << SPIS_CONFIG_CPOL_Pos);
}

void SPISClass::setCPHA(bool _trailing)
{
    if (_trailing)
        spis->CONFIG |= (SPIS_CONFIG_CPHA_Trailing << SPIS_CONFIG_CPHA_Pos);
    else
        spis->CONFIG &= ~(SPIS_CONFIG_CPHA_Trailing << SPIS_CONFIG_CPHA_Pos);
}

void SPISClass::setBuffers(char *rxBuffer, char *txBuffer, int len)
{
  spis->RXDPTR = (uint32_t)rxBuffer;
  spis->MAXRX = len;

  spis->TXDPTR = (uint32_t)txBuffer;
  spis->MAXTX = len;
}

int SPISClass::setCS(int _pinCS)
{
    int old = pinCS;
    pinCS = _pinCS;
    pinMode(pinCS, INPUT);
    spis->PSELCSN = pinCS;
    return old;
}

int SPISClass::setSCK(int _pinSCK)
{
    int old = pinSCK;
    pinSCK = _pinSCK;
    pinMode(pinSCK, OUTPUT);
    spis->PSELSCK = pinSCK;
    return old;
}

int SPISClass::setMOSI(int _pinMOSI)
{
    int old = pinMOSI;
    pinMOSI = _pinMOSI;
    pinMode(pinMOSI, OUTPUT);
    spis->PSELMOSI = pinMOSI;
    return old;
}

int SPISClass::setMISO(int _pinMISO)
{
    int old = pinMISO;
    pinMISO = _pinMISO;
    pinMode(pinMISO, INPUT);
    spis->PSELMISO = pinMISO;
    return old;
}

SPISClass SPIS(NRF_SPIS1, PIN_SPI_SS_2, PIN_SPI_SCK_2, PIN_SPI_MOSI_2, PIN_SPI_MISO_2);
