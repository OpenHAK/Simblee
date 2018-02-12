/*!*****************************************************************************
* @file   i2c_tof.c
*
* Copyright (c) 2015-2017 Heptagon
*
* @brief  Base I2C functions - Adds a layer between native I2C and ToF calls 
*         This is to make debugging easier, if there is an issue.
*******************************************************************************/

#include "i2c_tof.h"

int i2c__close(int iI2cDevice);
int i2c__open(const char * szDeviceName, int iFlags, int iI2cAddr);
int i2c__write_byte(int iI2cFd, __u8 u8Address);
int i2c__read_byte_data(int iI2cFd, __u8 u8Address);
int i2c__write_byte_data(int iI2cFd, __u8 u8Address, __u8 u8Val);
int i2c__read_word_data(int iI2cFd, __u8 u8Address);
int i2c__write_word_data(int iI2cFd, __u8 u8Address, __u16 u16Val);

//Uncomment for CPU using big endian - if the lower levels doesn't handle the endian
//#define I2C_CPU_BIG_ENDIAN


//For Big Endian CPU -> Swap Big Endian to Little Endian, Little Endian CPU do nothing
static __u16 support_endian(__u16 u16Data){
#ifdef I2C_CPU_BIG_ENDIAN
    return ((u16Data >> 8) | ((u16Data & 0xff) << 8));
#else 
    return u16Data;
#endif
}

/******************************************************************************
*@desc opens an I2C device
*@param szDeviceName (IN) - I2C Device Name used to open the device
*@param iFlags(IN) - I2C Device configuration flags
*@param iI2cAddr(IN) - I2C Device slave ID address
*@return an id or <0 if error
*@NOTE iI2cDevice_ is set by caller
******************************************************************************/
int i2c_open(const char * szDeviceName, int iFlags, int iI2cAddr)
{
     return 0;
}

/******************************************************************************
*@desc opens an I2C device
*@param iId (IN) - I2C Device Id (return from i2c_open)
*@param iParam(IN) - I2C Device parameter value
*@return <0 if error
******************************************************************************/
int i2c_close(int iI2cDevice)
{
    return 0;
}


/******************************************************************************
 *@desc read a single byte(8 bits) from I2C
 *@param iDeviceAddr (IN) - I2C Device Address to read from
 *@return The read I2C Byte or <0 if error
 ******************************************************************************/
int i2c_read_byte(int iDeviceAddr)
{
  uint8_t data;
  Wire.beginTransmission(gTOF_3D_RANGER_ADDRESS);
  Wire.write(iDeviceAddr);
  Wire.endTransmission();

  Wire.requestFrom(gTOF_3D_RANGER_ADDRESS, 1);
  data = Wire.read();
  Wire.endTransmission();

  return data;
}

/******************************************************************************
*@desc read a single word(16 bits) from I2C
*@param iDeviceAddr (IN) - I2C Device Address to read from
*@return The read I2C Word or <0 if error (Endian is support both error[neg #]
*        and u16 value in the same return field)
******************************************************************************/
int i2c_read_word(int iDeviceAddr)
{
  int data;
  int byteLow, byteHigh;
  Wire.beginTransmission(gTOF_3D_RANGER_ADDRESS);
  Wire.write(iDeviceAddr);
  Wire.endTransmission();

  Wire.requestFrom(gTOF_3D_RANGER_ADDRESS, 2);
  byteLow = Wire.read();
  byteHigh = Wire.read();

  Wire.endTransmission();
  data = (byteHigh << 8) | byteLow;

  return data;
}

/******************************************************************************
*@desc write a single byte(8 bits) from I2C
*@param iDeviceAddr (IN) - I2C Device Address to write to
*@param iValue (IN) - what value to write (will handle endian)
*@return <0 if error
******************************************************************************/
int i2c_write_byte(int iDeviceAddr, int iValue)
{
  Wire.beginTransmission(gTOF_3D_RANGER_ADDRESS);
  Wire.write(iDeviceAddr);
  Wire.write(iValue);
  Wire.endTransmission();

  return gTOF_RTN_SUCCESS;
}

/******************************************************************************
*@desc read a single word(16 bits) from I2C
*@param iDeviceAddr (IN) - I2C Device Address to write to
*@param iValue (IN) - what value to write (will flip the endian if needed)
*@return  <0 if error
******************************************************************************/
int i2c_write_word(int iDeviceAddr, int iValue)
{
  uint8_t byteLow, byteHigh;
  byteHigh = (iValue >> 8) & 0xFF;
  byteLow = iValue & 0xFF;

  Wire.beginTransmission(gTOF_3D_RANGER_ADDRESS);
  Wire.write(iDeviceAddr);
  Wire.write(byteLow);
  Wire.write(byteHigh);
  Wire.endTransmission();

  return gTOF_RTN_SUCCESS;
}

/******************** END of i2c_tof.c *****************/