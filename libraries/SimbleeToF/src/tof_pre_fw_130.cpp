/*!*****************************************************************************
* @file   tof_pre_fw_130.c
*
*   Copyright (c) 2015-2017 Heptagon
*
* @brief  Main high-level ToF functions for PRE-Firmware 1.3.0
*******************************************************************************/

#include "i2c_tof.h"
void tof_olivia_patch_01000302(void);

/***************************************************************************************
*@desc Mailbox CMD 0x0006 - Get Calibration Data (PRE Firmware 1.30)
*      base off of App Note for Mailbox 0x0006
*@param pu16MbxCalData(OUT) - Calibration data for Mailbox 0x0006
*@return 0 for SUCCESS or <0 if error
****************************************************************************************/
int tof_pre130_mailbox_cmd_0x0006(uint16_t * pu16MbxCalData)
{
    return tof_mailbox_read_cmd(0x0006U, pu16MbxCalData, 27 /*size in words*/);
}

/******************************************************************************
*@desc Shared calibration function - only outputs a single measurement
*@param sCalibrationCfg - ToF calibration configuration that should be used
*@param u16MbxCalData (IN/OUT) - Fills the field with the data from Mailbox 0x0006
*                                Note: fail will cause this value to be all zero
*@return 0 for SUCCESS or <0 if error.  Note: Leave MCPU in Standby state
******************************************************************************/
int tof_pre130_calibration_shared(s3dRangeConfig sCalibrationCfg, uint16_t *pu16MbxCalData, uint16_t u16DataSizeInWords)
{
    int iRtn;
    int iFunctRtn = gTOF_RTN_GENERIC_FAIL /*-1*/;
    int iCnt = 0;
    int iMaxTries = 50;
    int iMaxCntTrys = 10;
    int iStatus;
    int iDone;

    tof_log(eTOF_LOG_CMD_DEBUG_LVL, "%s::%d starting\n", __FUNCTION__, __LINE__);

    /*Zero the calData - so if error zero is return*/
    memset(pu16MbxCalData, 0x00, u16DataSizeInWords * sizeof(uint16_t));

    /*Make sure MCPU is off*/
    iRtn = tof_mcpu_state_set_w_retry(eTOF_MCPU_STATE_OFF, TRUE);
    if (iRtn < gTOF_RTN_SUCCESS /*0*/) {
        tof_log(eTOF_LOG_ERROR_LVL, "%s::%d - unable to turn off the MCPU(%d)\n", __FUNCTION__, __LINE__, iRtn);
        goto FAIL;
    }

    /*Apply Olivia Patch ONLY for device ID 01000302*/
    tof_olivia_patch_01000302();

    tof_log(eTOF_LOG_CMD_DEBUG_LVL, "Configuring ToF for Calibration\n");
    iCnt = 0;
    iDone = FALSE;
    do {
        iRtn = tof_device_configuration(sCalibrationCfg);
        if (iRtn == gTOF_RTN_SUCCESS /*0*/) {
            iDone = TRUE;
        }
        else {
            usleep(100);
            iCnt++;
            if (iCnt < iMaxTries) {
                tof_log(eTOF_LOG_ERROR_LVL, "%s::%d - Unable to set the configuration for the device(%d)\n", __FUNCTION__, __LINE__, iRtn);
                goto FAIL;
            }
        }
    } while (iDone == FALSE);

    /*Turn on MCPU and the device status is idle*/
    iRtn = tof_mcpu_on_and_wait_for_idle(200 /*tries*/);
    if (iRtn < gTOF_RTN_SUCCESS /*0*/) {
        tof_log(eTOF_LOG_ERROR_LVL, "%s::%d - unable to get the MCPU on into Idle state(%d)\n", __FUNCTION__, __LINE__, iRtn);
        goto FAIL;
    }

    /*Make sure the device status is idle*/
    iCnt = 0;
    do {
        iStatus = i2c_read_word(addr_ADDA_DSTATUS);
        if (iStatus < gTOF_RTN_SUCCESS /*0*/) {
            tof_log(eTOF_LOG_ERROR_LVL, "%s::%d - unable to get device status(%d)\n", __FUNCTION__, __LINE__, iStatus);
            goto FAIL;
        }
        iCnt++;
        usleep(100);
    } while ((iStatus != 0x01F8) && (iCnt < iMaxTries));

    if (iCnt >= iMaxTries){
        tof_log(eTOF_LOG_ERROR_LVL, "%s::%d - Never got into the right mode after %d Tries (last mode:0x%04X)\n", __FUNCTION__, __LINE__, iMaxTries, iStatus);
    }

    /*Get measurement*/
    iRtn = i2c_write_byte(addr_ADDA_CMD, CMD_VALID_MASK | CMD_SINGLE_MEASURE/*0x81*/);
    if (iRtn < gTOF_RTN_SUCCESS /*0*/) {
        tof_log(eTOF_LOG_ERROR_LVL, "%s::%d - start a mesaurement has Fail(%d)\n", __FUNCTION__, __LINE__, iRtn);
        goto FAIL;
    }

    /*Wait for measurement and idle*/
    iCnt = 0;
    do {
        tof_wait_on_interrupt_blocking(ICSR_INT_RESULT, 20 * iMaxTries);

        iStatus = i2c_read_word(addr_ADDA_DSTATUS);
        if (iStatus < gTOF_RTN_SUCCESS /*0*/) {
            tof_log(eTOF_LOG_ERROR_LVL, "%s::%d - unable to get device status(%d)\n", __FUNCTION__, __LINE__, iStatus);
            goto FAIL;
        }
        if (iStatus != 0x01F8) {
            usleep(100);
            iCnt++;
        }
    } while ((iStatus != 0x01F8) && (iCnt < iMaxCntTrys));
    if (iCnt >= iMaxCntTrys) {
        tof_log(eTOF_LOG_ERROR_LVL, "%s::%d - waiting for mesaurement has timeout(%d)\n", __FUNCTION__, __LINE__, iRtn);
        goto FAIL;
    }

    /*Check to see if measurement is good*/
    iRtn = i2c_read_word(addr_ADDA_RSLTR);
    if (iRtn < gTOF_RTN_SUCCESS /*0*/) {
        tof_log(eTOF_LOG_ERROR_LVL, "%s::%d - unable to get mesaurement status(%d)\n", __FUNCTION__, __LINE__, iRtn);
        goto FAIL;
    }

    if ((iRtn & RSLTR_ERR_N_VALID_MASK) == 0x8000) {
        iRtn = tof_pre130_mailbox_cmd_0x0006(pu16MbxCalData);
        if (iRtn < gTOF_RTN_SUCCESS /*0*/) {
            tof_log(eTOF_LOG_ERROR_LVL, "%s::%d - Start Mesaurement Fail(%d)\n", __FUNCTION__, __LINE__, iRtn);
            goto FAIL;
        }
        iFunctRtn = gTOF_RTN_SUCCESS /*0*/;
    }
    else {
        tof_log(eTOF_LOG_CMD_DEBUG_LVL, "%s::%d - Didnt get a good measurment(0x%04X)\n", __FUNCTION__, __LINE__, iRtn);
    }
    
    /*Restore starting ToF Configuration*/
    iRtn = tof_device_configuration(sCurrToFConfig_);
    if (iRtn < gTOF_RTN_SUCCESS /*0*/) {
        tof_log(eTOF_LOG_ERROR_LVL, "%s::%d - Unable to set the configuration for the device(%d)\n", __FUNCTION__, __LINE__, iRtn);
        goto FAIL;
    }

    /*Go into Stand-by mode - low power mode*/
    i2c_write_word(addr_ADDA_CMD, CMD_VALID_MASK | CMD_GO_STANDBY /*0x0090*/);

    return iFunctRtn;

FAIL:
    tof_dump_device_status(NULL);
    tof_device_configuration(sCurrToFConfig_);
    /*Go into Stand-by mode - low power mode*/
    i2c_write_word(addr_ADDA_CMD, CMD_VALID_MASK | CMD_GO_STANDBY /*0x0090*/);
    return gTOF_RTN_GENERIC_FAIL /*-1*/;
}
/******************** END of tof_pre_fw_130.c *****************/