/*!*****************************************************************************
* @file   tof_base_funct.c
*
* Copyright (c) 2015-2017 Heptagon
*
* @brief  Low-level ToF functions
*******************************************************************************/
#include "i2c_tof.h"
#include <stdarg.h>

int iLogDisplayLevel_ = eTOF_LOG_CMD_DEBUG_LVL; /*default Log printing level*/

/******************************************************************************
*@desc read a single word(16 bits) using I2C indirectly (two steps)
*@param u16Addr (IN) - I2C Device Address to read from
*@param pu16Data (OUT) - Read data from the device
*@return 0 for SUCCESS or <0 if error
******************************************************************************/
int tof_read_indirect_word(uint16_t u16Addr, uint16_t *pu16Data)
{
    int iRtn;

    tof_log(eTOF_LOG_ALL_DEBUG_LVL, "%s::%d read address 0x%04X ", __FUNCTION__, __LINE__, u16Addr);

    iRtn = i2c_write_word(addr_I2C_ADDR_PTR, u16Addr);
    if (iRtn < gTOF_RTN_SUCCESS /*0*/) {
        tof_log(eTOF_LOG_ERROR_LVL, "%s::%d - addr_I2C_ADDR_PTR(0x % X:0x % X Fail(%d)\n", __FUNCTION__, __LINE__, u16Addr, *pu16Data, iRtn);
        return gTOF_RTN_GENERIC_FAIL /*-1*/;
    }
    iRtn = i2c_read_word(addr_I2C_DATA_PORT);
    if (iRtn < gTOF_RTN_SUCCESS /*0*/) {
        tof_log(eTOF_LOG_ERROR_LVL, "%s::%d - addr_I2C_DATA_PORT(0x%X:0x%X Fail(%d)\n", __FUNCTION__, __LINE__, u16Addr, *pu16Data, iRtn);
        return gTOF_RTN_GENERIC_FAIL /*-1*/;
    }
    *pu16Data = (uint16_t)iRtn;
    tof_log(eTOF_LOG_ALL_DEBUG_LVL, " with 0x%04X\n", *pu16Data);
    return gTOF_RTN_SUCCESS /*0*/;
}

/******************************************************************************
*@desc write a single word(16 bits) using I2C indirectly (two steps)
*@param u16Addr (IN) - I2C Device Address to write to
*@param u16Data (IN) - write data to the device
*@return 0 for SUCCESS or <0 if error
******************************************************************************/
int tof_write_indirect_word( uint16_t u16Addr, uint16_t u16Data)
{
    int iRtn;

    tof_log(eTOF_LOG_ALL_DEBUG_LVL, "%s::%d write address 0x%04X with 0x%04X\n", __FUNCTION__, __LINE__, u16Addr, u16Data);

    iRtn = i2c_write_word( addr_I2C_ADDR_PTR, u16Addr);
    if (iRtn < gTOF_RTN_SUCCESS /*0*/) {
        tof_log(eTOF_LOG_ERROR_LVL, "%s::%d - addr_I2C_ADDR_PTR(0x%X:0x%X Fail(%d)\n", __FUNCTION__, __LINE__, u16Addr, u16Data, iRtn);
        return gTOF_RTN_GENERIC_FAIL /*-1*/;
    }
    iRtn = i2c_write_word(addr_I2C_DATA_PORT, u16Data);
    if (iRtn < gTOF_RTN_SUCCESS /*0*/) {
        tof_log(eTOF_LOG_ERROR_LVL, "%s::%d - addr_I2C_DATA_PORT(0x%X:0x%X Fail(%d)\n", __FUNCTION__, __LINE__, u16Addr, u16Data, iRtn);
        return gTOF_RTN_GENERIC_FAIL /*-1*/;
    }
    return gTOF_RTN_SUCCESS /*0*/;
}

/******************************************************************************
*@desc write and verify a single word(16 bits) using I2C
*@param iDeviceAddr (IN) - I2C Device Address to write to
*@param u16Data (IN) - write and verify data to the device
*@return 0 for SUCCESS, -1 mean write fail, -2 mean verify fail (didn't match)
******************************************************************************/
int tof_write_n_verify_word(int iDeviceAddr, uint16_t u16Data)
{
    int iRtn;
    uint16_t u16NewData;

    tof_log(eTOF_LOG_ALL_DEBUG_LVL, "%s::%d write address 0x%04X with 0x%04X\n", __FUNCTION__, __LINE__, iDeviceAddr, u16Data);

    iRtn = i2c_write_word(iDeviceAddr, u16Data);
    if (iRtn < gTOF_RTN_SUCCESS /*0*/) {
        tof_log(eTOF_LOG_ERROR_LVL, "%s::%d - unable to write word to device (0x%X:0x%X Fail(%d)\n", __FUNCTION__, __LINE__, iDeviceAddr, u16Data, iRtn);
        return gTOF_RTN_GENERIC_FAIL /*-1*/;
    }

    iRtn = i2c_read_word(iDeviceAddr);
    if (iRtn < gTOF_RTN_SUCCESS /*0*/) {
        tof_log(eTOF_LOG_ERROR_LVL, "%s::%d - unable to read the device (0x%X:0x%X Fail(%d)\n", __FUNCTION__, __LINE__, iDeviceAddr, u16Data, iRtn);
        return -2; /*Fail*/
    }
    u16NewData = (uint16_t)iRtn;
    if (u16NewData != u16Data) {
        tof_log(eTOF_LOG_ERROR_LVL, "%s::%d - unable to verify the device (0x%X: 0x%X != 0x%0X Fail\n", __FUNCTION__, __LINE__, iDeviceAddr, u16Data, u16NewData);
        return -2; /*Fail*/

    }
    return gTOF_RTN_SUCCESS /*0*/;
}

/******************************************************************************
*@desc Reads ToF memory - Note: will put the ToF into the right mode before
*      doing the read.  It will NOT restore the original mode
*@param u16Addr (IN) - I2C Device Address to write to
*@param u16SizeInBytes (IN) - How many bytes to read 
*@param pu8Buf (IN/OUT) - returns the read bytes (Note: caller should allocate the memory)
*@return 0 for SUCCESS or <0 if error
******************************************************************************/
int tof_read_mem(uint16_t u16Addr, uint16_t u16SizeInBytes, uint8_t* pu8Buf)
{
    int iRtn;
    int iPos = 0;

    /*Make sure system is in the right state:*/
    tof_mcpu_state_set_w_retry(eTOF_MCPU_STATE_OFF, TRUE);

    iRtn = i2c_write_word(addr_I2C_ADDR_PTR, u16Addr);
    if (iRtn < gTOF_RTN_SUCCESS /*0*/) {
        tof_log(eTOF_LOG_ERROR_LVL, "%s::%d - addr_I2C_ADDR_PTR(0x%X Fail(%d)\n", __FUNCTION__, __LINE__, u16Addr, iRtn);
        return gTOF_RTN_GENERIC_FAIL /*-1*/;
    }

    tof_log(eTOF_LOG_ALL_DEBUG_LVL, "%s::%d read address 0x%04X with %u Bytes\n", __FUNCTION__, __LINE__, u16Addr, u16SizeInBytes);
    while (iPos < u16SizeInBytes) {
        uint8_t u8NumBytesToProcess = (u16SizeInBytes - iPos >= 2 ? 2U : 1U);
        if (u8NumBytesToProcess == 2) {
            iRtn = i2c_read_word(addr_I2C_DATA_PORT);
            pu8Buf[iPos + 1] = (uint8_t)((iRtn & 0xFF00) >> 8); /*Get the higher Byte, lower Byte will be add later*/
        }
        else {
            iRtn = i2c_read_byte(addr_I2C_DATA_PORT);
        }

        if (iRtn == gTOF_RTN_GENERIC_FAIL /*-1*/) {
            tof_print_hex(pu8Buf, u16SizeInBytes, eTOF_LOG_ALL_DEBUG_LVL);
            return gTOF_RTN_GENERIC_FAIL /*-1*/;
        }

        pu8Buf[iPos] = (uint8_t)(iRtn & 0x00FF);
        iPos += u8NumBytesToProcess;
    }
    tof_print_hex(pu8Buf, u16SizeInBytes, eTOF_LOG_ALL_DEBUG_LVL);
    return gTOF_RTN_SUCCESS /*0*/;
}

/******************************************************************************
*@desc Writes ToF memory - Note: will put the ToF into the right mode before
*      doing the write.  It will NOT restore the original mode
*@param u16Addr (IN) - I2C Device Address to write to
*@param u16SizeInBytes (IN) - How many bytes to write
*@param pu8Buf (IN) - What data to write
*@return 0 for SUCCESS or <0 if error
******************************************************************************/
int tof_write_mem( uint16_t u16Addr, uint16_t u16SizeInBytes, uint8_t* pu8Buf)
{
    int iRtn;
    int iPos = 0;
    uint16_t u16Data;

    /*Make sure system is in the right state:*/
    tof_mcpu_state_set_w_retry(eTOF_MCPU_STATE_OFF, TRUE);

    iRtn = i2c_write_word(addr_I2C_ADDR_PTR, u16Addr);
    if (iRtn < gTOF_RTN_SUCCESS /*0*/) {
        u16Data = (pu8Buf[iPos + 1]<<8) | pu8Buf[iPos];
        tof_log(eTOF_LOG_ERROR_LVL, "%s::%d - addr_I2C_ADDR_PTR(0x%X:0x%X Fail(%d)\n", __FUNCTION__, __LINE__, u16Addr, u16Data, iRtn);
        return gTOF_RTN_GENERIC_FAIL /*-1*/;
    }

    tof_log(eTOF_LOG_ALL_DEBUG_LVL, "%s::%d writing address 0x%04X with %u Bytes :", __FUNCTION__, __LINE__, u16Addr, u16SizeInBytes);
    while (iPos < u16SizeInBytes)  {
        uint8_t u8NumBytesToProcess = (u16SizeInBytes - iPos >= 2 ? 2U : 1U);
        if (u8NumBytesToProcess == 2U) {
            u16Data = (pu8Buf[iPos + 1]<<8) | pu8Buf[iPos]; /*Convert data to 16bit*/
            iRtn = i2c_write_word(addr_I2C_DATA_PORT, u16Data);
            tof_log(eTOF_LOG_ALL_DEBUG_LVL, "0x%04X:", u16Data);
        }
        else {
            iRtn = i2c_write_byte(addr_I2C_DATA_PORT, pu8Buf[iPos]);
            tof_log(eTOF_LOG_ALL_DEBUG_LVL, "0x%02X:", pu8Buf[iPos]);
        }

        if (iRtn == gTOF_RTN_GENERIC_FAIL /*-1*/) {
            tof_log(eTOF_LOG_ALL_DEBUG_LVL, "\n");
            return gTOF_RTN_GENERIC_FAIL /*-1*/;
        }

        iPos += u8NumBytesToProcess;
    }

    tof_log(eTOF_LOG_ALL_DEBUG_LVL, "\n");
    return gTOF_RTN_SUCCESS /*0*/;
}

/******************************************************************************
*@desc This read a address modify it with the byte value and with its mask.
*@param u8Addr (IN) - The address to read/Modify/write to
*@param u8Data (IN) - data to write to the address. Note: this will be & with the u8InDataMask
*@param u8InDataMask (IN) - This is the data MASK.  Ex: if you want to change one bit(BIT4) of the
*                            an address.  u8Data:0x10 u8InDataMask:0x10
*@return 0 for SUCCESS or <0 if error
******************************************************************************/
int tof_read_modify_write_byte(uint8_t u8Addr, uint8_t u8Data, uint8_t u8InDataMask)
{
    uint8_t u8WriteData;
    int iRtn;

    iRtn = i2c_read_byte(u8Addr);
    if (iRtn < gTOF_RTN_SUCCESS /*0*/) {
        tof_log(eTOF_LOG_ERROR_LVL, "%s::%d - unable to read address 0x%02X (%d)\n", __FUNCTION__, __LINE__, u8Addr, iRtn);
        goto FAIL;
    }

    u8WriteData = ((iRtn & 0xFF) & ~u8InDataMask) | (u8InDataMask & u8Data);
    tof_log(eTOF_LOG_ALL_DEBUG_LVL, "%s::%d reading address 0x%02X(0x%02X) with value 0x%02X and MASK 0x%02X to 0x%02X\n"
        , __FUNCTION__, __LINE__, u8Addr, (iRtn & 0xFF), u8Data, u8InDataMask, u8WriteData);

    iRtn = i2c_write_byte(u8Addr, u8WriteData);
    if (iRtn < gTOF_RTN_SUCCESS /*0*/) {
        tof_log(eTOF_LOG_ERROR_LVL, "%s::%d - unable to modify the 0x%02X with the value 0x%02X using MASK 0x%02X (%d)\n"
            , __FUNCTION__, __LINE__,u8Addr,u8Data,u8InDataMask,iRtn);
        goto FAIL;
    }

    return gTOF_RTN_SUCCESS /*0*/;
FAIL:
     return gTOF_RTN_GENERIC_FAIL /*-1*/;
}

/******************************************************************************
*@desc This read a address modify it with the word value and with its mask.
*@param u8Addr (IN) - The address to read/Modify/write to
*@param u16Data (IN) - data to write to the address. Note: this will be & with the u16InDataMask
*@param u16InDataMask (IN) - This is the data MASK.  Ex: if you want to change one bit(BIT4) of the
*                            an address.  u16Data:0x0010 u16InDataMask:0x0010
*@return 0 for SUCCESS or <0 if error
******************************************************************************/
int tof_read_modify_write_word(uint8_t u8Addr, uint16_t u16Data, uint16_t u16InDataMask)
{
    uint16_t u16WriteData;
    int iRtn;

    iRtn = i2c_read_word(u8Addr);
    if (iRtn < gTOF_RTN_SUCCESS /*0*/) {
        tof_log(eTOF_LOG_ERROR_LVL, "%s::%d - unable to read address 0x%04X (%d)\n", __FUNCTION__, __LINE__, u16Data, iRtn);
        goto FAIL;
    }

    u16WriteData = ((iRtn & 0xFFFF) & ~u16InDataMask) | (u16InDataMask & u16Data);
    tof_log(eTOF_LOG_ALL_DEBUG_LVL, "%s::%d reading address 0x%04X(0x%04X) with value 0x%04X and MASK 0x%04X to 0x%04X\n"
        , __FUNCTION__, __LINE__, u8Addr, (iRtn & 0xFFFF), u16Data, u16InDataMask, u16WriteData);

    iRtn = i2c_write_word(u8Addr, u16WriteData);
    if (iRtn < gTOF_RTN_SUCCESS /*0*/) {
        tof_log(eTOF_LOG_ERROR_LVL, "%s::%d - unable to modify the 0x%04X with the value 0x%04X using MASK 0x%04X (%d)\n"
            , __FUNCTION__, __LINE__, u8Addr, u16Data, u16InDataMask, iRtn);
        goto FAIL;
    }

    return gTOF_RTN_SUCCESS /*0*/;
FAIL:
    return gTOF_RTN_GENERIC_FAIL /*-1*/;
}

/******************************************************************************
*@desc print out hex values to the screen
*@param pu8Buff (IN) - buffer to print out
*@param iSizeInBytes (IN) - Size of buffer in bytes to print out
*@param eLogLvl what logLvl to use for the output printing
******************************************************************************/
void tof_print_hex(uint8_t *pu8Buff, int iSizeInBytes, eTofLogLvl eLogLvl)
{
    char szBuff[256];
    uint16_t u16Pos = 0;
    uint16_t u16LastStart = 0;
    uint16_t ii, jj;

    if (eLogLvl > iLogDisplayLevel_){
        return; //skip printing
    }
    tof_log(eLogLvl," HEX:: 0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F -- ASCII Values\n");
    szBuff[0] = '\0';
    for (ii = 0; ii < iSizeInBytes; ii++) {
        if (ii % 16 == 0) {
            if (ii != 0) {
                u16Pos += sprintf(&szBuff[u16Pos], "-- ");
                for (jj = ii - 16; jj < ii; jj++) {
                    if ((pu8Buff[jj] >= ' ') && (pu8Buff[jj] < 0x7f)) {
                        u16Pos += sprintf(&szBuff[u16Pos], "%c", (unsigned char)(pu8Buff[jj] & 0xFF));
                    }
                    else {
                        u16Pos += sprintf(&szBuff[u16Pos], ".");
                    }
                }
            }
            if (u16Pos != 0){
                u16Pos += sprintf(&szBuff[u16Pos], "\n");
                tof_log(eLogLvl, "%s", szBuff);
            }
            u16Pos = 0;
            u16Pos += sprintf(&szBuff[u16Pos], "%04X::", ii);
        }
        u16Pos += sprintf(&szBuff[u16Pos], "%02X ", (unsigned char)(pu8Buff[ii] & 0xFF));
    }

    if (iSizeInBytes % 16 != 0) {
        for (jj = 0; jj < 16 - iSizeInBytes % 16; jj++) {
            u16Pos += sprintf(&szBuff[u16Pos], "   ");
        }
        u16LastStart = iSizeInBytes - iSizeInBytes % 16;
    }
    else {
        if (iSizeInBytes > 15) u16LastStart = iSizeInBytes - 16;
    }

    u16Pos += sprintf(&szBuff[u16Pos], "-- ");
    for (jj = u16LastStart; jj < iSizeInBytes; jj++) {
        if ((pu8Buff[jj] > 26) && (pu8Buff[jj] < 0x7f)) {
            u16Pos += sprintf(&szBuff[u16Pos], "%c", (unsigned char)(pu8Buff[jj] & 0xFF));
        }
        else {
            u16Pos += sprintf(&szBuff[u16Pos], ".");
        }
    }

    u16Pos += sprintf(&szBuff[u16Pos], "\n");
    tof_log(eLogLvl, "%s", szBuff);;
}

/******************************************************************************
*@desc returns the ToF that is detected
******************************************************************************/
eTofType tof_device_type(void)
{
    return CurrTofType_;
}

/**********************************************************************************************
*@desc Display ToF log print
*@param szTag (IN) The tag which you want to show before log
*@param szFmt (IN) - printf string format
*@param args (IN) - printf arguments
*@return if >0 number of chars printed, else error value
***********************************************************************************************/
int tof_display_log(const char *szTag, const char *szFmt, va_list args)
{
    char szLog[gMAX_LOG_PRINT_SIZE];
    int nCharPrinted = 0;

    vsnprintf(szLog, gMAX_LOG_PRINT_SIZE, szFmt, args);
    nCharPrinted = printf("[%s] %s", szTag, szLog);
    return nCharPrinted;
}

/**********************************************************************************************
*@desc common ToF log print function
*@param szFmt (IN) - printf string format
*@param ... (IN) - printf arguments
*@return if >0 number of chars printed, else error value
***********************************************************************************************/
int tof_log(int iLogType, const char *szFmt, ...)
{
    int nCharPrinted = 0;

    if (iLogType <= iLogDisplayLevel_){
        va_list args;
        va_start(args, szFmt);
        nCharPrinted = tof_display_log(gLOG_DISPLAY_TAG, szFmt, args);
        va_end(args);
    }
    return nCharPrinted;
}

/**********************************************************************************************
*@desc Sets the ToF log print level
*@param iLogLevel (IN) - new ToF log print level
***********************************************************************************************/
void tof_log_set_level(int iLogLevel)
{
    if (iLogLevel <= eTOF_LOG_DISABLE_LVL) {
        iLogDisplayLevel_ = (int)eTOF_LOG_DISABLE_LVL;
    }
    else if (iLogLevel >= gTOF_LOG_ALL_INFO_LVL){
        iLogDisplayLevel_ = (int)gTOF_LOG_ALL_INFO_LVL;
    }
    else {
        iLogDisplayLevel_ = iLogLevel;
    }
}
/******************** END of tof_base_funct.c *****************/