/*!*****************************************************************************
* @file   tof_main_funct.c
*
*   Copyright (c) 2015-2017 Heptagon
*
* @brief  Main high-level ToF functions
*******************************************************************************/

#include "i2c_tof.h"

/*declaring shared variables*/
int iI2cDevice_ = giINVALID_I2C_DEVICE;
eTofType CurrTofType_ = eTOF_TYPE_LAURA;
char szSerialId_[gMAX_STD_STR_SIZE];
char szI2CDevice_[gMAX_STD_STR_SIZE];
uint32_t u32DeviceIdent_ = 0U;
uint16_t * pu16CalibrationFactor_ = NULL;
uint16_t u16CalibrationFactorData_[gMAX_CALIBRATION_FACTOR_DATA_SIZE_IN_WORDS];
s3dRangeConfig sCurrToFConfig_;
uint16_t u16Generic_calibration_data_size_in_words_;
uint16_t u16Generic_calibration_f0_data_size_in_words_;
void(*tof_generic_default_config_get_)(s3dRangeConfig *ps3dRangeConfig);
int (*tof_generic_calibration_white_100mm_)(uint16_t * pu16MbxCalData);
int (*tof_generic_calibration_black_400mm_)(uint16_t * pu16MbxCalData);
int (*tof_generic_calibration_f0_data_get_)(uint16_t * pu16F0Data);
int (*tof_generic_exposure_time_)(uint16_t *pu16ExposureTime_usec);
int(*tof_generic_single_measure_n_err_get_results_)(int *piDist_mm, int *piAmplitude, uint16_t *pu16ExposureTime_usec, int *piErrCode, int *piExtendError, uint32_t u32MeasFlags);


void tof_initialize_internal_values(void)
{
    iI2cDevice_ = giINVALID_I2C_DEVICE;
    szSerialId_[0] = '\0';
    u32DeviceIdent_ = 0U;
    pu16CalibrationFactor_ = NULL;
    CurrTofType_ = eToF_TYPE_UNDETECTED;
    tof_device_setup_empty_device();
}

/******************************************************************************
*@desc connect ToF - this is the first function that need to be called to
*      connect to the ToF.  Currently - This function will Connect, Initialize
*      and configure the ToF Device
*@param szDevice (IN) - I2C path.  EX: "/dev/i2c-2"
*@param sConfig (IN) - Pass device Configuration, NULL means use default configuration
******************************************************************************/
int tof_connect(const char * szDevice,s3dRangeConfig *sConfig)
{
    int iRtn;

    tof_log(eTOF_LOG_CMD_DEBUG_LVL, "%s::%d starting\n", __FUNCTION__, __LINE__);
    if (iI2cDevice_ != giINVALID_I2C_DEVICE) {
        tof_disconnect();
    }
    tof_initialize_internal_values();
    strncpy(szI2CDevice_, szDevice, gMAX_STD_STR_SIZE); /*save szDevice for reinitialization*/

    iI2cDevice_ = i2c_open(szDevice, O_RDWR, gTOF_3D_RANGER_ADDRESS);
    if (iI2cDevice_ < gTOF_RTN_SUCCESS /*0*/)
    {
        tof_log(eTOF_LOG_ERROR_LVL, "%s::%d Got %d when opening %s\n", __FUNCTION__, __LINE__, iI2cDevice_, szDevice);
        goto FAIL_NO_DEBUG_PRINT;
    }

    iRtn = tof_poweron_w_cfg(sConfig, eToF_TYPE_UNDETECTED);
    if (iRtn < 0) {
        tof_log(eTOF_LOG_ERROR_LVL, "%s::%d Unable to init the device\n", __FUNCTION__, __LINE__);
        goto FAIL;
    }
    return gTOF_RTN_SUCCESS; /*0*/

FAIL:
    tof_dump_device_status(NULL);
FAIL_NO_DEBUG_PRINT:
    tof_initialize_internal_values(); /*So other function know, ToF was not open correctly*/
    return gTOF_RTN_GENERIC_FAIL /*-1*/;
}

/******************************************************************************
*@desc re-initialized device
*@return 0 for success, -1 for error, -2 for uninitialized, -2 for power off
******************************************************************************/
int tof_reinitialize_device(void)
{
    int iRtn;
    int iOldLogLevel = iLogDisplayLevel_;

    if (iI2cDevice_ == giINVALID_I2C_DEVICE){
        tof_log(eTOF_LOG_ERROR_LVL, "Need to connect to ToF before using API:%s\n", __FUNCTION__);
        return gTOF_RTN_GENERIC_FAIL /*-1*/;
    }

    iRtn = tof_device_initialization_status(FALSE);
    tof_disconnect();
    if (iRtn == gTOF_RTN_POWERED_OFF) {
        return gTOF_RTN_POWERED_OFF /*-2*/;
    }

    tof_log_set_level(eTOF_LOG_DISABLE_LVL);  /*so will not print error message*/
    i2c_write_word(addr_ADDA_CMD, CMD_VALID_MASK | CMD_RESET_MASK); /*Will always return error*/
    tof_log_set_level(iOldLogLevel); /*restore log level*/
    usleep(1000);

    iRtn = tof_connect(szI2CDevice_, &sCurrToFConfig_);
    if (iRtn < gTOF_RTN_SUCCESS /*0*/) {
        tof_log(eTOF_LOG_ERROR_LVL, "%s::%d - unable to re-connect to the device(%s) (%d)\n", __FUNCTION__, __LINE__, szI2CDevice_, iRtn);
        return gTOF_RTN_GENERIC_FAIL /*-1*/;
    }
    return tof_device_initialization_status(FALSE);
}

/******************************************************************************
*@desc disconnect(close) ToF
*Note: Leave MCPU in Standby state
******************************************************************************/
void tof_disconnect(void)
{
    tof_log(eTOF_LOG_CMD_DEBUG_LVL, "%s::%d starting\n", __FUNCTION__, __LINE__);
    if (iI2cDevice_ != giINVALID_I2C_DEVICE){
        tof_mcpu_state_set_w_retry(eTOF_MCPU_STATE_STANDBY, TRUE);
        i2c_close(iI2cDevice_);
    }
    tof_initialize_internal_values();
}

/******************************************************************************
*@desc sets the MCPU State (Standby, OFF, ON)
*@param eState (IN) - What state you what the MCPU to be in
*@return the MCPU state or <0 if error
******************************************************************************/
int tof_mcpu_state_set(eTofMcpuState eState)
{
    int iRtn;

    if (iI2cDevice_ == giINVALID_I2C_DEVICE){
        tof_log(eTOF_LOG_ERROR_LVL, "Need to connect to ToF before using API:%s\n", __FUNCTION__);
        return gTOF_RTN_GENERIC_FAIL /*-1*/;
    }

    switch (eState){
    case eTOF_MCPU_STATE_STANDBY:
        /*Set MCPU to STANDBY */
        iRtn = i2c_write_word(addr_ADDA_CMD, CMD_VALID_MASK | CMD_GO_STANDBY /*0x0090*/);
        if (iRtn < gTOF_RTN_SUCCESS /*0*/) {
            tof_log(eTOF_LOG_ERROR_LVL, "%s::%d - unable to go into Standby (%d)\n", __FUNCTION__, __LINE__, iRtn);
            goto FAIL;
        }
        break;

    case eTOF_MCPU_STATE_OFF:
        /*Set MCPU OFF*/
        iRtn = i2c_write_word(addr_ADDA_CMD, CMD_VALID_MASK | CMD_GO_MCPU_OFF  /*0x0091*/);
        if (iRtn < gTOF_RTN_SUCCESS /*0*/) {
            tof_log(eTOF_LOG_ERROR_LVL, "%s::%d - unable to turn off MCPU(%d)\n", __FUNCTION__, __LINE__, iRtn);
            goto FAIL;
        }
        break;

    case eTOF_MCPU_STATE_ON:
        /*Set MCPU ON*/
        iRtn = i2c_write_word(addr_ADDA_CMD, CMD_VALID_MASK | CMD_WAKE_UP_MCPU_2_ON_MODE /*0x0092*/);
        if (iRtn < gTOF_RTN_SUCCESS /*0*/) {
            tof_log(eTOF_LOG_ERROR_LVL, "%s::%d - unable to wake-up MPCU(%d)\n", __FUNCTION__, __LINE__, iRtn);
            goto FAIL;
        }
        break;

    default:
        tof_log(eTOF_LOG_ERROR_LVL, "%s::%d - Unknown requested State(%d)\n", __FUNCTION__, __LINE__, eState);
        goto FAIL;
        break;
    }

    return (i2c_read_word(addr_ADDA_DSTATUS) & DSTATUS_PM_STATUS_MASK);

FAIL:
    return gTOF_RTN_GENERIC_FAIL /*-1*/;
}

/******************************************************************************
*@desc sets the MCPU State forcibly (Standby, OFF, ON), but can change settings
*@param eState (IN) - What state you what the MCPU to be in
*@return the MCPU state or <0 if error
*@Note will not return error if the requested state is not met
******************************************************************************/
int tof_mcpu_state_set_forcibly(eTofMcpuState eState)
{
    int iStatus;
    int iRtn;
    int iTry = 0;
    int iDone = FALSE;

    if (iI2cDevice_ == giINVALID_I2C_DEVICE){
        tof_log(eTOF_LOG_ERROR_LVL, "Need to connect to ToF before using API:%s\n", __FUNCTION__);
        return gTOF_RTN_GENERIC_FAIL /*-1*/;
    }

    iStatus = i2c_read_word(addr_ADDA_DSTATUS);
    if (iStatus < gTOF_RTN_SUCCESS /*0*/) {
        tof_log(eTOF_LOG_ERROR_LVL, "%s::%d - unable to get device status(%d)\n", __FUNCTION__, __LINE__, iStatus);
        goto FAIL;
    }

    do {
        switch (eState){
        case eTOF_MCPU_STATE_STANDBY:
            if (iTry > 0) { /*Only do this after trying non-force way first*/
                if ((iStatus & DSTATUS_PM_STATUS_MASK) == DSTATUS_PM_STATUS_MCPU_OFF) {
                    /*Turn on MCPU first*/
                    i2c_write_byte(addr_ADDA_PMU_CFG_UP, 0x06);
                    i2c_write_word(addr_ADDA_CMD, CMD_VALID_MASK | CMD_WAKE_UP_MCPU_2_ON_MODE /*0x0092*/);
                    usleep(300);
                }
                iRtn = i2c_write_byte(addr_ADDA_MCPU_PMC, MCPU_STANDBY_REQ);
                if (iRtn < gTOF_RTN_SUCCESS /*0*/) {
                    tof_log(eTOF_LOG_ERROR_LVL, "%s::%d - MCPU Standby Request Fail(%d)\n", __FUNCTION__, __LINE__, iRtn);
                    goto FAIL;
                }
            }

            iRtn = i2c_write_word(addr_ADDA_CMD, CMD_VALID_MASK | CMD_GO_STANDBY /*0x0090*/);
            if (iRtn < gTOF_RTN_SUCCESS /*0*/) {
                tof_log(eTOF_LOG_ERROR_LVL, "%s::%d - unable to go into Standby (%d)\n", __FUNCTION__, __LINE__, iRtn);
                goto FAIL;
            }
            break;

        case eTOF_MCPU_STATE_OFF:
            if (iTry > 1) {  /*Only do this if the first time didn't work -- Need to restore it after doing this step */
                /* Hold-off CPU */
                iRtn = i2c_write_byte(addr_ADDA_PMU_CFG_UP, 0x00);
                if (iRtn < gTOF_RTN_SUCCESS /*0*/) {
                    tof_log(eTOF_LOG_ERROR_LVL, "%s::%d - addr_ADDA_PMU_CFG_UP Fail(%d)\n", __FUNCTION__, __LINE__, iRtn);
                    goto FAIL;
                }
                /* Disable all interrupts */
                iRtn = i2c_write_byte(addr_ADDA_IER, IER_ENABLE_SHIFT);
                if (iRtn < gTOF_RTN_SUCCESS /*0*/) {
                    tof_log(eTOF_LOG_ERROR_LVL, "%s::%d - addr_ADDA_IER Fail(%d)\n", __FUNCTION__, __LINE__, iRtn);
                    goto FAIL;
                }

                iRtn = i2c_write_byte(addr_ADDA_MCPU_PMC, MCPU_SLEEP_REQ);
                if (iRtn < gTOF_RTN_SUCCESS /*0*/) {
                    tof_log(eTOF_LOG_ERROR_LVL, "%s::%d - Fail to request to PMU for MCPU(%d)\n", __FUNCTION__, __LINE__, iRtn);
                    goto FAIL;
                }
            }
            
            if (iTry > 0) { /*Only do this after trying non-force way first*/
                iRtn = i2c_write_byte(addr_ADDA_PMU_CFG_UP, 0x05);
                if (iRtn < gTOF_RTN_SUCCESS /*0*/) {
                    tof_log(eTOF_LOG_ERROR_LVL, "%s::%d - unable to configure PMU(%d)\n", __FUNCTION__, __LINE__, iRtn);
                    goto FAIL;
                }
            }

            /*Set MCPU OFF*/
            iRtn = i2c_write_word(addr_ADDA_CMD, CMD_VALID_MASK | CMD_GO_MCPU_OFF  /*0x0091*/);
            if (iRtn < gTOF_RTN_SUCCESS /*0*/) {
                tof_log(eTOF_LOG_ERROR_LVL, "%s::%d - unable to turn off MCPU(%d)\n", __FUNCTION__, __LINE__, iRtn);
                goto FAIL;
            }
            break;

        case eTOF_MCPU_STATE_ON:
            if (iTry > 0) { /*Only do this after trying non-force way first*/
                if ((iStatus & DSTATUS_PM_STATUS_MASK) == DSTATUS_PM_STATUS_STANDBY) {/*Turn MCPU OFF First*/
                    i2c_write_word(addr_ADDA_PMU_CFG, MCPU_INIT_STATE  /*0x0200*/);
                    i2c_write_word(addr_ADDA_CMD, CMD_VALID_MASK | CMD_GO_MCPU_OFF  /*0x0091*/);
                    usleep(300);
                }

                iRtn = i2c_write_byte(addr_ADDA_PMU_CFG_UP, 0x06);
                if (iRtn < gTOF_RTN_SUCCESS /*0*/) {
                    tof_log(eTOF_LOG_ERROR_LVL, "%s::%d - unable to configure PMU(%d)\n", __FUNCTION__, __LINE__, iRtn);
                    goto FAIL;
                }
            }

            /*Set MCPU ON*/
            iRtn = i2c_write_word(addr_ADDA_CMD, CMD_VALID_MASK | CMD_WAKE_UP_MCPU_2_ON_MODE /*0x0092*/);
            if (iRtn < gTOF_RTN_SUCCESS /*0*/) {
                tof_log(eTOF_LOG_ERROR_LVL, "%s::%d - unable to wake-up MPCU(%d)\n", __FUNCTION__, __LINE__, iRtn);
                goto FAIL;
            }
            break;

        default:
            tof_log(eTOF_LOG_ERROR_LVL, "%s::%d - Unknown requested State(%d)\n", __FUNCTION__, __LINE__, eState);
            goto FAIL;
            break;
        }
        usleep(100);  /*Sleep for 100uSeconds to give the system time to change state*/

        iStatus = i2c_read_word(addr_ADDA_DSTATUS);
        if (iStatus < gTOF_RTN_SUCCESS /*0*/) {
            tof_log(eTOF_LOG_ERROR_LVL, "%s::%d - unable to get device status(%d)\n", __FUNCTION__, __LINE__, iStatus);
            goto FAIL;
        }

        if ((iStatus&DSTATUS_PM_STATUS_MASK) == (int)eState) {
            iDone = TRUE;
        }
        iTry++;
    } while ((iDone != TRUE) & (iTry < 3));

    return (iStatus & DSTATUS_PM_STATUS_MASK);
    
FAIL:
    tof_dump_device_status(NULL);
    return gTOF_RTN_GENERIC_FAIL /*-1*/;
}

/******************************************************************************
*@desc will change the MCPU state and will retry for it to happen, if there is
*      an issue it will use force (which may change some configurations)
*@param eState (IN) - What state you what the MCPU to be in
*@param iAllowForce (IN) - non-zero means that Force can be used, else will try
*                          without using force 
*@return 0 for SUCCESS or <0 if error
******************************************************************************/
int tof_mcpu_state_set_w_retry(eTofMcpuState eState, int iAllowForce)
{
    int iCnt;
    int iStatus;
    int iMaxTries = 100;

    if (iI2cDevice_ == giINVALID_I2C_DEVICE){
        tof_log(eTOF_LOG_ERROR_LVL, "Need to connect to ToF before using API:%s\n", __FUNCTION__);
        return gTOF_RTN_GENERIC_FAIL /*-1*/;
    }

    iStatus = tof_mcpu_state_set(eState);
    if (iStatus < gTOF_RTN_SUCCESS /*0*/) {
        tof_log(eTOF_LOG_ERROR_LVL, "%s::%d - unable to change state of the MCPU(%d) got error (%d)\n", __FUNCTION__, __LINE__, eState, iStatus);
        goto FAIL;
    }
    
    /*Check to see if in the right state*/
    if ((iStatus & DSTATUS_PM_STATUS_MASK) == eState) {
        return gTOF_RTN_SUCCESS /*0*/;
    }

    /*if something wrong, try using force*/
    iCnt = 0;
    do {
        if (iCnt >= iMaxTries) {
            tof_log(eTOF_LOG_ERROR_LVL, "%s::%d - unable to turn off MPCU after %d tries\n", __FUNCTION__, __LINE__, iCnt);
            goto FAIL;
        }

        if (iAllowForce) {
            iStatus = tof_mcpu_state_set_forcibly(eState);
        }
        else {
            iStatus = tof_mcpu_state_set(eState);
        }

        if (iStatus < gTOF_RTN_SUCCESS /*0*/) {
            tof_log(eTOF_LOG_ERROR_LVL, "%s::%d - unable to change state of the MCPU(%d:%d) got error (%d)\n", __FUNCTION__, __LINE__, eState, iAllowForce, iStatus);
            goto FAIL;
        }
        iCnt++;
        usleep(10); /*10 uSeconds*/
    } while ((iStatus & DSTATUS_PM_STATUS_MASK) != eState);

    return gTOF_RTN_SUCCESS /*0*/;
FAIL:
    tof_dump_device_status(NULL);
    return gTOF_RTN_GENERIC_FAIL /*-1*/;
}

/******************************************************************************
*@desc turn on MCPU and wait for idle
*@param iMaxTries (IN) - max number of time to loop (50uSec per loop)
*@return 0 for SUCCESS or <0 if error
******************************************************************************/
int tof_mcpu_on_and_wait_for_idle(int iMaxTries)
{
    int iRtn;
    int iStatus;
    int iDone = FALSE;
    int iCnt = 0;


    do {
        /*Set Initialization*/
        i2c_write_byte(addr_ADDA_PMU_CFG_UP, 0x06);

        /*Turn on MCPU*/
        iRtn = i2c_write_word(addr_ADDA_CMD, CMD_VALID_MASK | CMD_WAKE_UP_MCPU_2_ON_MODE /*0x0092*/);
        if (iRtn < gTOF_RTN_SUCCESS /*0*/) {
            tof_log(eTOF_LOG_ERROR_LVL, "%s::%d - unable to turn on MCPU(%d)\n", __FUNCTION__, __LINE__, iRtn);
            return gTOF_RTN_GENERIC_FAIL /*-1*/;
        }

        iStatus = i2c_read_word(addr_ADDA_DSTATUS);
        if (iStatus != 0x1F8){
            usleep(50);
            iCnt++;
            if (iCnt > iMaxTries) {
                tof_log(eTOF_LOG_ERROR_LVL, "%s::%d - device status is in the wrong state after trying %d times (0x%04X)\n", __FUNCTION__, __LINE__,iCnt, iStatus);
                return gTOF_RTN_GENERIC_FAIL /*-1*/;
            }
        }
        else {
            iDone = TRUE; //Exit loop
        }
    } while (!iDone);
    return gTOF_RTN_SUCCESS /*0*/;
}

/******************************************************************************
*@desc gets/display the ToF Status - used in debugging and making sure device
*      is in the right state before doing a task
*@param iDisp (IN) - true means display output, false means just return the value
*@param piMcpuStatus (OUT) - Return 2bit MCPU Status
*@return 0 for SUCCESS or <0 if error
******************************************************************************/
int tof_dev_status_last_get(int iDisp, int *piMcpuStatus)
{
    int iRtn;

    if (iI2cDevice_ == giINVALID_I2C_DEVICE){
        tof_log(eTOF_LOG_ERROR_LVL, "Need to connect to ToF before using API:%s\n", __FUNCTION__);
        return gTOF_RTN_GENERIC_FAIL /*-1*/;
    }

    iRtn = i2c_read_byte(addr_ADDA_DSTATUS);
    if (iRtn < gTOF_RTN_SUCCESS /*0*/) {
        tof_log(eTOF_LOG_ERROR_LVL, "%s::%d - read addr_ADDA_DSTATUS Fail(%d)\n", __FUNCTION__, __LINE__, iRtn);
        return gTOF_RTN_GENERIC_FAIL /*-1*/;
    }

    *piMcpuStatus = (iRtn & DSTATUS_PM_STATUS_MASK);

    if (iDisp == FALSE) {
        return gTOF_RTN_SUCCESS /*0*/;
    }

    /*Display device status*/
    tof_log(eTOF_LOG_ERROR_LVL, "RAW State:0x%04X ", iRtn);

    return gTOF_RTN_SUCCESS /*0*/;
}

/******************************************************************************
*@desc Program and validate the TOF Configuration
*@param sConfig (IN) - Device Configuration to write to the device
*@return 0 for SUCCESS or <0 if error
******************************************************************************/
int tof_device_configuration(s3dRangeConfig sConfig)
{
    int iRtn;

    if (iI2cDevice_ == giINVALID_I2C_DEVICE){
        tof_log(eTOF_LOG_ERROR_LVL, "Need to connect to ToF before using API:%s\n", __FUNCTION__);
        return gTOF_RTN_GENERIC_FAIL /*-1*/;
    }

    /*Command configuration Register A*/
    iRtn = tof_write_n_verify_word(addr_ADDA_RFSH, sConfig.u16Cmd_CfgR_A);
    if (iRtn < gTOF_RTN_SUCCESS /*0*/) {
        tof_log(eTOF_LOG_ERROR_LVL, "%s::%d - Config %s => 0x%04X Fail(%d)\n", __FUNCTION__, __LINE__, "u16Cmd_CfgR_A", sConfig.u16Cmd_CfgR_A, iRtn);
        goto FAIL;
    }
    else {
        tof_log(eTOF_LOG_CMD_DEBUG_LVL, "%s: Cmd_CfgR_A[0x%04X] => 0x%04X\n", __FUNCTION__, addr_ADDA_RFSH, sConfig.u16Cmd_CfgR_A);
}

    /*Command configuration Register B*/
    iRtn = tof_write_n_verify_word(addr_ADDA_AUXCTRL, sConfig.u16Cmd_CfgR_B);
    if (iRtn < gTOF_RTN_SUCCESS /*0*/) {
        tof_log(eTOF_LOG_ERROR_LVL, "%s::%d - Config %s => 0x%04X Fail(%d)\n", __FUNCTION__, __LINE__, "u16Cmd_CfgR_B", sConfig.u16Cmd_CfgR_B, iRtn);
        goto FAIL;
    }
    else {
        tof_log(eTOF_LOG_CMD_DEBUG_LVL, "%s: Cmd_CfgR_B[0x%04X] => 0x%04X\n", __FUNCTION__, addr_ADDA_AUXCTRL, sConfig.u16Cmd_CfgR_B);
    }

    /*Command configuration Register 0*/
    iRtn = tof_write_n_verify_word(addr_ADDA_HFCFG0, sConfig.u16HFCfg_0);
    if (iRtn < gTOF_RTN_SUCCESS /*0*/) {
        tof_log(eTOF_LOG_ERROR_LVL, "%s::%d - Config %s => 0x%04X Fail(%d)\n", __FUNCTION__, __LINE__, "u16HFCfg_0", sConfig.u16HFCfg_0, iRtn);
        goto FAIL;
    }
    else {
        tof_log(eTOF_LOG_CMD_DEBUG_LVL,  "%s: HFCfg_0[0x%04X] => 0x%04X\n", __FUNCTION__, addr_ADDA_HFCFG0, sConfig.u16HFCfg_0);
    }

    /*Command configuration Register 1*/
    iRtn = tof_write_n_verify_word(addr_ADDA_HFCFG1, sConfig.u16HFCfg_1);
    if (iRtn < gTOF_RTN_SUCCESS /*0*/) {
        tof_log(eTOF_LOG_ERROR_LVL, "%s::%d - Config %s => 0x%04X Fail(%d)\n", __FUNCTION__, __LINE__, "u16HFCfg_1", sConfig.u16HFCfg_1, iRtn);
        goto FAIL;
    }
    else {
        tof_log(eTOF_LOG_CMD_DEBUG_LVL,  "%s: HFCfg_1[0x%04X] => 0x%04X\n", __FUNCTION__, addr_ADDA_HFCFG1, sConfig.u16HFCfg_1);
     }

    /*Command configuration Register 2*/
    iRtn = tof_write_n_verify_word(addr_ADDA_HFCFG2, sConfig.u16HFCfg_2);
    if (iRtn < gTOF_RTN_SUCCESS /*0*/) {
        tof_log(eTOF_LOG_ERROR_LVL, "%s::%d - Config %s = > 0x%04X Fail(%d)\n", __FUNCTION__, __LINE__, "u16HFCfg_2", sConfig.u16HFCfg_2, iRtn);
        goto FAIL;
    }
    else {
        tof_log(eTOF_LOG_CMD_DEBUG_LVL,  "%s: HFCfg_2[0x%04X] => 0x%04X\n", __FUNCTION__, addr_ADDA_HFCFG2, sConfig.u16HFCfg_2);
    }

    /*Command configuration Register 3*/
    iRtn = tof_write_n_verify_word(addr_ADDA_HFCFG3, sConfig.u16HFCfg_3);
    if (iRtn < gTOF_RTN_SUCCESS /*0*/) {
        tof_log(eTOF_LOG_ERROR_LVL, "%s::%d - Config %s = > 0x%04X Fail(%d)\n", __FUNCTION__, __LINE__, "u16HFCfg_3", sConfig.u16HFCfg_3, iRtn);
        goto FAIL;
    }
    else {
        tof_log(eTOF_LOG_CMD_DEBUG_LVL, "%s: HFCfg_3[0x%04X] => 0x%04X\n", __FUNCTION__, addr_ADDA_HFCFG3, sConfig.u16HFCfg_3);
    }
    return gTOF_RTN_SUCCESS /*0*/;

FAIL:
    tof_dump_device_status(NULL);
    return gTOF_RTN_GENERIC_FAIL /*-1*/;
}

/******************************************************************************
*@desc informs tof_measurement_get_w_wait to use the factory calibration factors
******************************************************************************/
void tof_calibration_factors_set_factory(void) 
{
    tof_log(eTOF_LOG_CMD_DEBUG_LVL, "%s::%d starting\n", __FUNCTION__, __LINE__);
    pu16CalibrationFactor_ = NULL;
}

/******************************************************************************
*@desc set the calibration factors that will be used in tof_measurement_get_w_wait
*@param u16CalibrationFactorData (IN) the calibration factors to will be use in getting a measurement
*@Return 0 for Success, else a negative number for error
******************************************************************************/
int tof_calibration_factors_set(uint16_t * pu16CalibrationFactorData)
{
    uint16_t u16ChkSum;
    int ii;

    tof_log(eTOF_LOG_CMD_DEBUG_LVL, "%s::%d starting\n", __FUNCTION__, __LINE__);
    //check for endian issue
    if (pu16CalibrationFactorData[0] != 0xCA1B) {
        tof_log(eTOF_LOG_ERROR_LVL, "%s::%d - Initial Calibration factor first u16 NOT matching: wanted[0x%04X], got [0x%04X]\n", __FUNCTION__, __LINE__, 0xCA1BU, pu16CalibrationFactorData[0]);
        goto FAIL;
    }

    if (pu16CalibrationFactorData[1] > gMAX_CALIBRATION_FACTOR_DATA_SIZE_IN_WORDS) {
        tof_log(eTOF_LOG_ERROR_LVL, "%s::%d - Calibiration data is to big (%d Btyes) - MAX size is %d Bytes\n", __FUNCTION__, __LINE__, pu16CalibrationFactorData[1], (int)(gMAX_CALIBRATION_FACTOR_DATA_SIZE_IN_WORDS*sizeof(uint16_t)));
        goto FAIL;
    }

    //Check the Checksum to see if it matches
    u16ChkSum = 0;
    for (ii = 0; ii < pu16CalibrationFactorData[1]-1; ii++)
    {
        u16ChkSum += pu16CalibrationFactorData[ii];
    }
    u16ChkSum = -u16ChkSum;

    if (pu16CalibrationFactorData[pu16CalibrationFactorData[1]-1] != u16ChkSum) {
        tof_log(eTOF_LOG_ERROR_LVL, "%s::%d - Calibration factor checksum NOT matching: wanted[0x%04X], got [0x%04X]\n", __FUNCTION__, __LINE__, u16ChkSum, pu16CalibrationFactorData[pu16CalibrationFactorData[1] - 1]);
        goto FAIL;
    }

    memcpy(u16CalibrationFactorData_, pu16CalibrationFactorData, (pu16CalibrationFactorData[1]) * sizeof(uint16_t));
    pu16CalibrationFactor_ = u16CalibrationFactorData_;

    return gTOF_RTN_SUCCESS /*0*/;
FAIL:
    tof_log(eTOF_LOG_ERROR_LVL, "%s::%d - Calibration factor NOT LOADED do to error in input file\n", __FUNCTION__, __LINE__);
    return gTOF_RTN_GENERIC_FAIL /*-1*/;
}

/******************************************************************************
*@desc get the calibration factors that is being used
*@return pointer to the Calibration Factors that are being used
******************************************************************************/
uint16_t * tof_calibration_factors_get(void)
{
    return pu16CalibrationFactor_;
}

/***************************************************************************************
*@desc initial power-on and configuration of the device
*      Following Power-on initialization sequence - From Apps Notes
*@param sConfig(IN) - Overrides the device configuration.  NULL means use default configuration 
*@param eDetectDeviceType(IN) - Overrides the detection of the device.   eToF_TYPE_UNDETECTED means 
*                               auto detect which ToF device is connect.  
*@return 0 for SUCCESS or <0 if error
****************************************************************************************/
int tof_poweron_w_cfg(s3dRangeConfig *sConfig, eTofType eDetectDeviceType)
{
    int iRtn;

    tof_log(eTOF_LOG_CMD_DEBUG_LVL, "%s::%d starting\n", __FUNCTION__, __LINE__);

    if (iI2cDevice_ == giINVALID_I2C_DEVICE){
        tof_log(eTOF_LOG_ERROR_LVL, "Need to connect to ToF before using API:%s\n", __FUNCTION__);
        return gTOF_RTN_GENERIC_FAIL /*-1*/;
    }

    /*put device into standby mode*/
    iRtn = tof_mcpu_state_set_w_retry(eTOF_MCPU_STATE_STANDBY, TRUE);
    if (iRtn < gTOF_RTN_SUCCESS /*0*/){
        tof_log(eTOF_LOG_ERROR_LVL, "%s::%d - Unable to go into standby mode(%d)\n", __FUNCTION__, __LINE__, iRtn);
        goto FAIL;
    }

    /*Drive INT_PAD High*/
    iRtn = i2c_write_byte(addr_ADDA_ICSR, ICRS_INT_MODE/*0x04*/);
    if (iRtn < gTOF_RTN_SUCCESS /*0*/){
        tof_log(eTOF_LOG_ERROR_LVL, "%s::%d - Unable to Drive INT_PAD High I2C(%d)\n", __FUNCTION__, __LINE__, iRtn);
        goto FAIL;
    }

    /*Configure I2C interface*/
    iRtn = i2c_write_word(addr_I2C_ACC_CFG, I2C_MCPU_DEBUG_ACC | I2C_HOST_DEBUG_ACC | I2C_AUTO_INC_DATAP | I2C_AUTO_INC /*0x65*/);
    if (iRtn < gTOF_RTN_SUCCESS /*0*/){
        tof_log(eTOF_LOG_ERROR_LVL, "%s::%d - Unable to configure I2C Interface(%d)\n", __FUNCTION__, __LINE__, iRtn);
        goto FAIL;
    }

    iRtn = tof_mcpu_state_set_w_retry(eTOF_MCPU_STATE_OFF, TRUE);
    if (iRtn != gTOF_RTN_SUCCESS /*0*/){
        tof_log(eTOF_LOG_ERROR_LVL, "%s::%d - unable to have MCPU go into off state(%d)\n", __FUNCTION__, __LINE__, iRtn);
        goto FAIL;
    }

    /*Read Module ID + Firmware ID + configure system for the device*/
    iRtn = tof_module_select_and_config(eToF_TYPE_UNDETECTED);

    iRtn = tof_mcpu_state_set_w_retry(eTOF_MCPU_STATE_ON, TRUE);
    if (iRtn != gTOF_RTN_SUCCESS /*0*/){
        tof_log(eTOF_LOG_ERROR_LVL, "%s::%d - unable to have MCPU go into on state(%d)\n", __FUNCTION__, __LINE__, iRtn);
        goto FAIL;
    }

    usleep(500); /*Sleep for 500uS*/

    if (sConfig == NULL) {
        tof_log(eTOF_LOG_CMD_DEBUG_LVL, "Configuring ToF Device with default settings\n");
        tof_generic_default_config_get_(&sCurrToFConfig_);
    }
    else {
        tof_log(eTOF_LOG_CMD_DEBUG_LVL, "Configuring ToF Device with CUSTOM settings\n");
        memcpy(&sCurrToFConfig_, sConfig, sizeof(s3dRangeConfig));
    }
   
    iRtn = tof_device_configuration(sCurrToFConfig_);
    if (iRtn < gTOF_RTN_SUCCESS /*0*/) {
        tof_log(eTOF_LOG_ERROR_LVL, "%s::%d - Unable to set the configuration for the device(%d)\n", __FUNCTION__, __LINE__, iRtn);
        goto FAIL;
    }

    iRtn = tof_mcpu_state_set_w_retry(eTOF_MCPU_STATE_STANDBY, TRUE);
    if (iRtn != gTOF_RTN_SUCCESS /*0*/){
        tof_log(eTOF_LOG_ERROR_LVL, "%s::%d - unable to have MCPU go into Standby state(%d)\n", __FUNCTION__, __LINE__, iRtn);
        goto FAIL;
    }

    return gTOF_RTN_SUCCESS /*0*/;

FAIL:
    tof_dump_device_status(NULL);
    tof_mcpu_state_set_w_retry(eTOF_MCPU_STATE_STANDBY, TRUE);
    return gTOF_RTN_GENERIC_FAIL /*-1*/;
}


/******************************************************************************
*@desc Issues a ToF measurement request.
*@param u32MeasFlags (IN) - Configure what measure flags should be use when getting the measurement
*@Return 0 for Success, else a negative number for error. Note: Leave MCPU in ON state,unless error then standby
******************************************************************************/
int tof_measurement_issue_measure_request(uint32_t u32MeasFlags)
{
    int iRtn;
    int iMaxTries = 200;

    tof_log(eTOF_LOG_CMD_DEBUG_LVL, "%s::%d starting\n", __FUNCTION__, __LINE__);
    if (iI2cDevice_ == giINVALID_I2C_DEVICE){
        tof_log(eTOF_LOG_ERROR_LVL, "Need to connect to ToF before using API:%s\n", __FUNCTION__);
        return gTOF_RTN_GENERIC_FAIL /*-1*/;
    }

    /*Make sure the device is in STANDBY - so it can take new initiation data*/
    iRtn = tof_mcpu_state_set_w_retry(eTOF_MCPU_STATE_STANDBY, TRUE);
    if (iRtn < gTOF_RTN_SUCCESS /*0*/) {
        tof_log(eTOF_LOG_ERROR_LVL, "%s::%d - unable to have MCPU go into Stand-by state(%d)\n", __FUNCTION__, __LINE__, iRtn);
        goto FAIL;
    }

    /*Set Initialization*/
    iRtn = i2c_write_word(addr_ADDA_PMU_CFG, PATCH_MEM_ENABLE | PATCH_CODE_LOAD_ENABLE  /*0x0500*/);
    if (iRtn < gTOF_RTN_SUCCESS /*0*/) {
        tof_log(eTOF_LOG_ERROR_LVL, "%s::%d - unable to set init(%d)\n", __FUNCTION__, __LINE__, iRtn);
        goto FAIL;
    }

    iRtn = tof_mcpu_state_set_w_retry(eTOF_MCPU_STATE_OFF, FALSE);
    if (iRtn != gTOF_RTN_SUCCESS /*0*/){
        tof_log(eTOF_LOG_ERROR_LVL, "%s::%d - unable to have MCPU go into off state(%d)\n", __FUNCTION__, __LINE__, iRtn);
        goto FAIL;
    }

    /*Only apply new Calibration Factors if they are set, else use the default values*/
    if (pu16CalibrationFactor_ != NULL){
        /*Write Calibration Data (using indirect addressing mode)*/
        iRtn = tof_write_mem(gCALIBRATION_FACTOR_ADDRESS, 2 * pu16CalibrationFactor_[1], (uint8_t *)pu16CalibrationFactor_);
        if (iRtn < gTOF_RTN_SUCCESS /*0*/){
            tof_log(eTOF_LOG_ERROR_LVL, "%s::%d - unable to set Calibration factor(Err:%d)\n", __FUNCTION__, __LINE__, iRtn);
            goto FAIL;
        }
    }

    /*Set Initialization*/
    iRtn = i2c_write_word(addr_ADDA_PMU_CFG, PATCH_MEM_ENABLE | MCPU_INIT_STATE  /*0x0600*/);
    if (iRtn < gTOF_RTN_SUCCESS /*0*/) {
        tof_log(eTOF_LOG_ERROR_LVL, "%s::%d - unable to set init(%d)\n", __FUNCTION__, __LINE__, iRtn);
        goto FAIL;
    }

    /*Make sure the device status is idle*/
    iRtn = tof_mcpu_on_and_wait_for_idle(iMaxTries);
    if (iRtn < gTOF_RTN_SUCCESS /*0*/) {
        tof_log(eTOF_LOG_ERROR_LVL, "%s::%d - unable to get the MCPU on into Idle state(%d)\n", __FUNCTION__, __LINE__, iRtn);
        goto FAIL;
    }

    /*0x81 = Single measure */
    iRtn = i2c_write_byte(addr_ADDA_CMD, CMD_VALID_MASK | CMD_SINGLE_MEASURE/*0x81*/);
    if (iRtn < gTOF_RTN_SUCCESS /*0*/) {
        tof_log(eTOF_LOG_ERROR_LVL, "%s::%d - Start Mesaurement Fail(%d)\n", __FUNCTION__, __LINE__, iRtn);
        goto FAIL;
    }

    return gTOF_RTN_SUCCESS; /*0*/

FAIL:
    tof_dump_device_status(NULL);
    tof_mcpu_state_set_w_retry(eTOF_MCPU_STATE_STANDBY, TRUE);
    return gTOF_RTN_GENERIC_FAIL /*-1*/;

}

/******************************************************************************
*@desc get a ToF measurement.  Request the measurement, wait and then get measurement.
*     This API is a blocking call.
*@param piDist_mm (OUT) - Distance in number of mm (1-2047mm): if error value is 00
*@param piAmplitude (OUT) - vector amplitude as a measure of the quality of the measure
*@param pu16ExposureTime_usec (OUT) - The Exposure Time in uSeconds that was use to get the measurement
*@param piErrCode (OUT) - Error Code: 00=>no error, 01=>Near target, 10=>Far field, 11=> General Err
*@param piExtendError(OUT) - Extend Error code (Enable by options in u32MeasFlags)
*@param u32MeasFlags (IN) - Configure what measure flags should be use when getting the measurement
*@param u32MaxWaitTime_ms (IN) - Max time to wait for a measurement to happen in mSeconds
*@return Distance in number of mm or <0 if error.  Note: leave in MCPU on state, except if error then standby state
******************************************************************************/
int tof_measurement_get_w_wait(int *piDist_mm, int *piAmplitude, uint16_t *pu16ExposureTime_usec, int *piErrCode, int *piExtendError, uint32_t u32MeasFlags, uint32_t u32MaxWaitTime_ms)
{
    int iRtn;

    tof_log(eTOF_LOG_CMD_DEBUG_LVL, "%s::%d starting\n", __FUNCTION__, __LINE__);

    *pu16ExposureTime_usec = 0U; /*Just in case there is an error*/
    *piAmplitude = 0U; /*Just in case there is an error*/

    if (iI2cDevice_ == giINVALID_I2C_DEVICE){
        tof_log(eTOF_LOG_ERROR_LVL, "Need to connect to ToF before using API:%s\n", __FUNCTION__);
        return gTOF_RTN_GENERIC_FAIL /*-1*/;
    }

    /*configure system and start a measurement request*/
    iRtn = tof_measurement_issue_measure_request(u32MeasFlags);
    if (iRtn < gTOF_RTN_SUCCESS /*0*/) {
        tof_log(eTOF_LOG_ERROR_LVL, "%s::%d - Start Mesaurement Fail(%d)\n", __FUNCTION__, __LINE__, iRtn);
        goto FAIL;
    }

    /*Wait for the measurement to be ready or timeout*/
    iRtn = tof_wait_on_interrupt_blocking(ICSR_INT_RESULT, u32MaxWaitTime_ms * 10 /*100 uSec per wait*/);
    if (iRtn < 0 /*error or timeout*/) {
        tof_log(eTOF_LOG_ERROR_LVL, "%s::%d - unable or timeout wating for interrupt(%d)\n", __FUNCTION__, __LINE__, iRtn);
        goto FAIL;
    }

    /*Get the measurement value*/
    iRtn = tof_generic_single_measure_n_err_get_results_(piDist_mm, piAmplitude, pu16ExposureTime_usec, piErrCode, piExtendError, u32MeasFlags);
    if (iRtn < 0) {
        tof_log(eTOF_LOG_ERROR_LVL, "%s::%d - unable to get measurement(%d)\n", __FUNCTION__, __LINE__, iRtn);
        goto FAIL;
    }
    return iRtn;

FAIL:
    tof_dump_device_status(NULL);
    tof_mcpu_state_set_w_retry(eTOF_MCPU_STATE_STANDBY, TRUE);
    return gTOF_RTN_GENERIC_FAIL /*-1*/;
}

/******************************************************************************
*@desc Issues a ToF continuous measurement request.
*@param u32MeasFlags (IN) - Configure what measure flags should be use when getting the measurement
*@Return 0 for Success, else a negative number for error
******************************************************************************/
int tof_measurement_issue_continuous_measure_request(uint16_t u16RefreshRate_us)
{
    int iRtn;

    tof_log(eTOF_LOG_CMD_DEBUG_LVL, "%s::%d starting\n", __FUNCTION__, __LINE__);

    if (iI2cDevice_ == giINVALID_I2C_DEVICE){
        tof_log(eTOF_LOG_ERROR_LVL, "Need to connect to ToF before using API:%s\n", __FUNCTION__);
        return gTOF_RTN_GENERIC_FAIL /*-1*/;
    }

    /*Make sure the device is in STANDBY - so it can take new initiation data*/
    iRtn = tof_mcpu_state_set_w_retry(eTOF_MCPU_STATE_STANDBY, TRUE);
    if (iRtn < gTOF_RTN_SUCCESS /*0*/) {
        tof_log(eTOF_LOG_ERROR_LVL, "%s::%d - unable to have MCPU go into Standby state(%d)\n", __FUNCTION__, __LINE__, iRtn);
        goto FAIL;
    }

    /*Set Initialization*/
    iRtn = i2c_write_word(addr_ADDA_PMU_CFG, PATCH_MEM_ENABLE | PATCH_CODE_LOAD_ENABLE  /*0x0500*/);
    if (iRtn < gTOF_RTN_SUCCESS /*0*/) {
        tof_log(eTOF_LOG_ERROR_LVL, "%s::%d - unable to set init(%d)\n", __FUNCTION__, __LINE__, iRtn);
        goto FAIL;
    }

    iRtn = tof_mcpu_state_set_w_retry(eTOF_MCPU_STATE_OFF, FALSE);
    if (iRtn != gTOF_RTN_SUCCESS /*0*/){
        tof_log(eTOF_LOG_ERROR_LVL, "%s::%d - unable to have MCPU go into off state(%d)\n", __FUNCTION__, __LINE__, iRtn);
        goto FAIL;
    }

    /*Only apply new Calibration Factors if they are set, else use the default values*/
    if (pu16CalibrationFactor_ != NULL){
        /*Write Calibration Data (using indirect addressing mode)*/
        iRtn = tof_write_mem(gCALIBRATION_FACTOR_ADDRESS, 2 * pu16CalibrationFactor_[1], (uint8_t *)pu16CalibrationFactor_);
        if (iRtn < gTOF_RTN_SUCCESS /*0*/){
            tof_log(eTOF_LOG_ERROR_LVL, "%s::%d - unable to set Calibration factor(Err:%d)\n", __FUNCTION__, __LINE__, iRtn);
            goto FAIL;
        }
    }

    /*Set Initialization*/
    iRtn = i2c_write_word(addr_ADDA_PMU_CFG, PATCH_MEM_ENABLE | MCPU_INIT_STATE  /*0x0600*/);
    if (iRtn < gTOF_RTN_SUCCESS /*0*/) {
        tof_log(eTOF_LOG_ERROR_LVL, "%s::%d - unable to set init(%d)\n", __FUNCTION__, __LINE__, iRtn);
        goto FAIL;
    }

    /*Turn on MCPU and the device status is idle*/
    iRtn = tof_mcpu_on_and_wait_for_idle(50 /*tries*/);
    if (iRtn < gTOF_RTN_SUCCESS /*0*/) {
        tof_log(eTOF_LOG_ERROR_LVL, "%s::%d - unable to get the MCPU on into Idle state(%d)\n", __FUNCTION__, __LINE__, iRtn);
        goto FAIL;
    }

    /*Refresh Rate Configuration*/
    iRtn = i2c_read_byte(0x0E);
    if (iRtn < gTOF_RTN_SUCCESS /*0*/) {
        tof_log(eTOF_LOG_ERROR_LVL, "%s::%d - unable to read 0x0E(%d)\n", __FUNCTION__, __LINE__, iRtn);
        goto FAIL;
    }
    iRtn = i2c_write_byte(0x0E, (iRtn & 0xFF) | 0x01);
    if (iRtn < gTOF_RTN_SUCCESS /*0*/) {
        tof_log(eTOF_LOG_ERROR_LVL, "%s::%d - unable to modify the dflt config (%d)\n", __FUNCTION__, __LINE__, iRtn);
        goto FAIL;
    }
    iRtn = i2c_write_word(0x0C, 0xE100 | ((u16RefreshRate_us / 100) & 0x7F));
    if (iRtn < gTOF_RTN_SUCCESS /*0*/) {
        tof_log(eTOF_LOG_ERROR_LVL, "%s::%d - unable to set the Refresh Rate(%d)\n", __FUNCTION__, __LINE__, iRtn);
        goto FAIL;
    }

    /*Set Relevant Configuration*/
    iRtn = i2c_read_byte(0x00);
    if (iRtn < gTOF_RTN_SUCCESS /*0*/) {
        tof_log(eTOF_LOG_ERROR_LVL, "%s::%d - unable to read 0x00(%d)\n", __FUNCTION__, __LINE__, iRtn);
        goto FAIL;
    }
    iRtn = i2c_write_byte(0x00, (iRtn & 0xFF) | 0x01);
    if (iRtn < gTOF_RTN_SUCCESS /*0*/) {
        tof_log(eTOF_LOG_ERROR_LVL, "%s::%d - unable to modify the ICSR Register (%d)\n", __FUNCTION__, __LINE__, iRtn);
        goto FAIL;
    }
    iRtn = i2c_read_word(0x26);
    if (iRtn < gTOF_RTN_SUCCESS /*0*/) {
        tof_log(eTOF_LOG_ERROR_LVL, "%s::%d - unable to read 0x00(%d)\n", __FUNCTION__, __LINE__, iRtn);
        goto FAIL;
    }
    iRtn = i2c_write_byte(0x26, (iRtn & 0xFFFF) | 0x0800);
    if (iRtn < gTOF_RTN_SUCCESS /*0*/) {
        tof_log(eTOF_LOG_ERROR_LVL, "%s::%d - unable to modify the ICSR Register (%d)\n", __FUNCTION__, __LINE__, iRtn);
        goto FAIL;
    }

    /*0x82 = Start Continuous measure */
    iRtn = i2c_write_byte(addr_ADDA_CMD, 0x82);
    if (iRtn < gTOF_RTN_SUCCESS /*0*/) {
        tof_log(eTOF_LOG_ERROR_LVL, "%s::%d - Start Continuous measure(%d)\n", __FUNCTION__, __LINE__, iRtn);
        goto FAIL;
    }

    return gTOF_RTN_SUCCESS; /*0*/

FAIL:
    tof_dump_device_status(NULL);
    tof_mcpu_state_set_w_retry(eTOF_MCPU_STATE_STANDBY, TRUE);
    return gTOF_RTN_GENERIC_FAIL /*-1*/;
}

/***************************************************************************************
*@desc check for an interrupt to be set (system is ready) - (NON-BLOCKING)
*@param u16DoneBitMask (IN) - Which bit mask to wait for (See Interrupt Control Status Register)
*@return 0 if interrupt is not set, 1 if interrupt is set, or <0 if error
****************************************************************************************/
int tof_check_interrupt(uint16_t u16DoneBitMask)
{
    int iRtn;

    iRtn = i2c_read_byte(addr_ADDA_ICSR);
    if (iRtn < gTOF_RTN_SUCCESS /*0*/) {
        tof_log(eTOF_LOG_ERROR_LVL, "%s::%d - unable to read ICSR Register(%d)\n", __FUNCTION__, __LINE__, iRtn);
        return gTOF_RTN_GENERIC_FAIL /*-1*/;
    }
    if ((iRtn & u16DoneBitMask) == u16DoneBitMask) {
        return 1; /*Interrupt Set*/
    }

    return 0; /*Interrupt Not Set*/
}

/***************************************************************************************
*@desc waits for an interrupt to be set (system is ready) - BLOCKING
*@param u16DoneBitMask (IN) - Which bit mask to wait for (See Interrupt Control Status Register)
*@param u16MaxTries (IN) - Max number of times to try (each try is 100uSecond)
*@return 0 for SUCCESS or <0 if error
****************************************************************************************/
int tof_wait_on_interrupt_blocking(uint16_t u16DoneBitMask, uint16_t u16MaxTries)
{
    uint16_t u16CntWait = 0;
    uint8_t u8Done;
    int iRtn;

    u8Done = FALSE;
    u16CntWait = 0;
    do {
        iRtn = tof_check_interrupt(u16DoneBitMask);
        if (iRtn < gTOF_RTN_SUCCESS /*0*/) {
            goto FAIL;
        }
        if (iRtn == 0) { /*Interrupt not set*/
            u16CntWait++;
            if (u16CntWait >= u16MaxTries) {
                tof_log(eTOF_LOG_ERROR_LVL, "%s::%d - MAX Tries(%d)\n", __FUNCTION__, __LINE__, u16CntWait);
                goto FAIL;
            }
            usleep(100); /*pause for 100 uSecond*/
        }
        else { /*Interrupt is set*/
            u8Done = TRUE; /*Exit Loop*/
        }
    } while (!u8Done);

    return gTOF_RTN_SUCCESS /*0*/;

FAIL:
    return gTOF_RTN_GENERIC_FAIL /*-1*/;
}

/***************************************************************************************
*@desc Reads output data from a mailbox command
*@param u16MailBoxCmd(IN) Mailbox command
*@param u16OutputSizeInWords(IN) size of mailbox command output in words
*@param pu16OutputData(IN/OUT) memory to store output of mailbox command
*@return 0 for SUCCESS or <0 if error
****************************************************************************************/
int tof_mailbox_read_cmd(uint16_t u16MailBoxCmd, uint16_t *pu16OutputData, uint16_t u16OutputSizeInWords)
{
    int iRtn;
    int iMaxTries = 200;
    int iCnt = 0;
    int iStatus;
    int iDone = FALSE;
    int iM2hMsgLen = 0;
    int ii;

    tof_log(eTOF_LOG_CMD_DEBUG_LVL, "%s::%d starting (0x%02X)\n", __FUNCTION__, __LINE__, u16MailBoxCmd);

    if (iI2cDevice_ == giINVALID_I2C_DEVICE){
        tof_log(eTOF_LOG_ERROR_LVL, "Need to connect to ToF before using API:%s\n", __FUNCTION__);
        return gTOF_RTN_GENERIC_FAIL /*-1*/;
    }
    if (pu16OutputData == NULL) {
        tof_log(eTOF_LOG_ERROR_LVL, "%s::%d Need to allocate output buffer for mailbox command\n", __FUNCTION__, __LINE__);
        return gTOF_RTN_GENERIC_FAIL /*-1*/;
    }

    /*Clear the pu16OutputData (if a fail, data will be all zeros)*/
    for (ii = 0; ii < u16OutputSizeInWords; ii++){
        pu16OutputData[ii] = 0;
    }

    /*Make sure device is in MCPU ON State*/
    tof_mcpu_state_set_w_retry(eTOF_MCPU_STATE_ON, FALSE);

    /*Clear any outstanding MBX output*/
    iDone = FALSE;
    iCnt = 0;
    do {
        /*Read ICSR Register*/
        iRtn = i2c_read_byte(addr_ADDA_ICSR);
        if (iRtn < 0) {
            tof_log(eTOF_LOG_ERROR_LVL, "%s::%d - unable to read ICSR Register(%d)\n", __FUNCTION__, __LINE__, iRtn);
            goto FAIL;
        }

        if ((iRtn&ICSR_INT_M2H_REQ) == 0) {
            iDone = TRUE; /*Exit Loop*/
        }
        else {
            /*Read M2H MBX (MCPU to Host Mailbox)*/
            iRtn = i2c_read_word(addr_ADDA_M2H_MBX);
            if (iRtn < gTOF_RTN_SUCCESS /*0*/) {
                tof_log(eTOF_LOG_ERROR_LVL, "%s::%d - unable to read MCPU to Host Mailbox(%d)\n", __FUNCTION__, __LINE__, iRtn);
                goto FAIL;
            }
            if (iCnt >= iMaxTries) {
                tof_log(eTOF_LOG_ERROR_LVL, "%s::%d - MBX never came ready after %d tries\n", __FUNCTION__, __LINE__, iCnt);
                goto FAIL;
            }
            iCnt++;
            usleep(100); /*100 uSeconds*/
        }

    } while (!iDone);

    /*Issue Mailbox Command */
    iRtn = i2c_write_word(addr_ADDA_H2M_MBX, u16MailBoxCmd);
    if (iRtn < gTOF_RTN_SUCCESS /*0*/){
        tof_log(eTOF_LOG_ERROR_LVL, "%s::%d - Unable to write H2M MBX(%d)\n", __FUNCTION__, __LINE__, iRtn);
        goto FAIL;
    }

    /*Wait until Mailbox is ready*/
    iRtn = tof_wait_on_interrupt_blocking(ICSR_INT_M2H_REQ/*Bit 5*/, 200 /*20mSec Max wait*/);
    if (iRtn < gTOF_RTN_SUCCESS /*0*/) {
        tof_log(eTOF_LOG_ERROR_LVL, "%s::%d - timeout on ICSR wait for H2M MBX(%d)\n", __FUNCTION__, __LINE__, iRtn);
        goto FAIL;
    }

    /*Read M2H MBX*/
    iStatus = i2c_read_word(addr_ADDA_M2H_MBX);
    if (iStatus < gTOF_RTN_SUCCESS /*0*/) {
        tof_log(eTOF_LOG_ERROR_LVL, "%s::%d - unable to read M2H MBX(%d)\n", __FUNCTION__, __LINE__, iStatus);
        goto FAIL;
    }
    iM2hMsgLen = (iStatus & 0x3F00) >> 8;

    if (((iStatus & 0xff) == 0xCC) && (iM2hMsgLen == u16OutputSizeInWords)) {
        for (ii = 0; ii < iM2hMsgLen; ii++) {
            iRtn = tof_wait_on_interrupt_blocking(ICSR_INT_M2H_REQ/*Bit 5*/, 150 /*15mSec Max wait*/);
            if (iRtn < gTOF_RTN_SUCCESS /*0*/) {
                tof_log(eTOF_LOG_ERROR_LVL, "%s::%d - timeout on ICSR wait for H2M MBX(%d)\n", __FUNCTION__, __LINE__, iRtn);
                goto FAIL;
            }

            //Read M2H MBX
            iStatus = i2c_read_word(addr_ADDA_M2H_MBX);
            if (iStatus < gTOF_RTN_SUCCESS /*0*/) {
                tof_log(eTOF_LOG_ERROR_LVL, "%s::%d - unable to read M2H MBX(%d) on Idx(%d)\n", __FUNCTION__, __LINE__, iStatus, ii);
                goto FAIL;
            }
            pu16OutputData[ii] = (uint16_t)(iStatus & 0xFFFF);
        }
    }
    else {
        tof_log(eTOF_LOG_ERROR_LVL, "%s::%d - initial read M2H MBX(0x%04X) was not valid(Len:%d)\n", __FUNCTION__, __LINE__, iStatus, iM2hMsgLen);
        goto FAIL;
    }

    return gTOF_RTN_SUCCESS /*0*/;

FAIL:
    tof_dump_device_status(NULL);
    tof_mcpu_state_set_w_retry(eTOF_MCPU_STATE_STANDBY, TRUE);
    return gTOF_RTN_GENERIC_FAIL /*-1*/;
}

/***************************************************************************************
*@desc writes data to mailbox
*@param u16MailBoxCmd(IN) Mailbox command
*@param pu16InputData(IN) data to be sent to the mailbox
*@return 0 for SUCCESS or <0 if error
****************************************************************************************/
int tof_mailbox_write_cmd(uint16_t u16MailBoxCmd, uint16_t *pu16InputData, uint16_t u16DataSizeInWords)
{
    int iRtn;
    int iMaxTries = 200;
    int iCnt = 0;
    int iDone = FALSE;
    int ii;
    int iInputSizeInWords;

    tof_log(eTOF_LOG_CMD_DEBUG_LVL, "%s::%d starting (0x%02X)\n", __FUNCTION__, __LINE__, u16MailBoxCmd);

    iInputSizeInWords = (u16MailBoxCmd >> 8) & 0x3F;

    if (iInputSizeInWords > 63) {
        tof_log(eTOF_LOG_ERROR_LVL, "%s::%d - tof_mailbox_write_cmd size to big(%d)\n", __FUNCTION__, __LINE__, iInputSizeInWords);
        goto FAIL;
    }

    /*Make sure device is in MCPU ON State*/
    tof_mcpu_state_set_w_retry(eTOF_MCPU_STATE_ON, FALSE);

    /*Clear any outstanding MBX output*/
    iDone = FALSE;
    iCnt = 0;
    do {
        /*Read ICSR Register*/
        iRtn = i2c_read_byte(addr_ADDA_ICSR);
        if (iRtn < 0) {
            tof_log(eTOF_LOG_ERROR_LVL, "%s::%d - unable to read ICSR Register(%d)\n", __FUNCTION__, __LINE__, iRtn);
            goto FAIL;
        }

        if ((iRtn&ICSR_INT_M2H_REQ) == 0) {
            iDone = TRUE; /*Exit Loop*/
        }
        else {
            /*Read M2H MBX (MCPU to Host Mailbox)*/
            iRtn = i2c_read_word(addr_ADDA_M2H_MBX);
            if (iRtn < gTOF_RTN_SUCCESS /*0*/) {
                tof_log(eTOF_LOG_ERROR_LVL, "%s::%d - unable to read MCPU to Host Mailbox(%d)\n", __FUNCTION__, __LINE__, iRtn);
                goto FAIL;
            }
            if (iCnt >= iMaxTries) {
                tof_log(eTOF_LOG_ERROR_LVL, "%s::%d - MBX never came ready after %d tries\n", __FUNCTION__, __LINE__, iCnt);
                goto FAIL;
            }
            iCnt++;
            usleep(100); /*100 uSeconds*/
        }

    } while (!iDone);

    /*Issue Mailbox Command */
    iRtn = i2c_write_word(addr_ADDA_H2M_MBX, u16MailBoxCmd);
    if (iRtn < gTOF_RTN_SUCCESS /*0*/){
        tof_log(eTOF_LOG_ERROR_LVL, "%s::%d - Unable to write H2M MBX(%d)\n", __FUNCTION__, __LINE__, iRtn);
        goto FAIL;
    }

    /*Wait until Mailbox is ready*/
    iRtn = tof_wait_on_interrupt_blocking(ICSR_INT_M2H_ACK/*Bit 6*/, 150 /*15mSec Max wait*/);
    if (iRtn < gTOF_RTN_SUCCESS /*0*/) {
        tof_log(eTOF_LOG_ERROR_LVL, "%s::%d - timeout on ICSR wait for H2M MBX(%d)\n", __FUNCTION__, __LINE__, iRtn);
        goto FAIL;
    }

    for (ii = 0; ii < iInputSizeInWords; ii++){
        /*Issue Mailbox Command*/
        iRtn = i2c_write_word(addr_ADDA_H2M_MBX, pu16InputData[ii]);
        if (iRtn < gTOF_RTN_SUCCESS /*0*/){
            tof_log(eTOF_LOG_ERROR_LVL, "%s::%d - Unable to write H2M MBX(CMD:0x%04X) (%d) on index:%d\n", __FUNCTION__, __LINE__, u16MailBoxCmd, iRtn, ii);
            goto FAIL;
        }

        /*Wait until Mailbox is ready*/
        iRtn = tof_wait_on_interrupt_blocking(ICSR_INT_M2H_ACK/*Bit 6*/, 150 /*15mSec Max wait*/);
        if (iRtn < gTOF_RTN_SUCCESS /*0*/) {
            tof_log(eTOF_LOG_ERROR_LVL, "%s::%d - timeout on ICSR wait for H2M MBX(%d)\n", __FUNCTION__, __LINE__, iRtn);
            goto FAIL;
        }
    }

    //TODO -- Read back the ACK from MailBox (Only for newer FW 1.3+)

    return gTOF_RTN_SUCCESS /*0*/;

FAIL:
    tof_dump_device_status(NULL);
    tof_mcpu_state_set_w_retry(eTOF_MCPU_STATE_STANDBY, TRUE);
    return gTOF_RTN_GENERIC_FAIL /*-1*/;
}



/******************************************************************************
*@desc gets and display the ToF module FW ID
*@return ToF module firmware ID or <0 if error
******************************************************************************/
int tof_module_fw_id_read(void)
{
    int iRtn;
    uint8_t u8Data[4];

    if (iI2cDevice_ == giINVALID_I2C_DEVICE){
        tof_log(eTOF_LOG_ERROR_LVL, "Need to connect to ToF before using API:%s\n", __FUNCTION__);
        return gTOF_RTN_GENERIC_FAIL /*-1*/;
    }

    iRtn = tof_read_mem(gDEVICE_FW_ID_ADDRESS, 4, u8Data);
    if (iRtn < gTOF_RTN_SUCCESS /*0*/) {
        tof_log(eTOF_LOG_ERROR_LVL, "%s::%d - Failed - read Module FW ID(%d)\n", __FUNCTION__, __LINE__, iRtn);
        tof_dump_device_status(NULL);
        return gTOF_RTN_GENERIC_FAIL /*-1*/;
    }

    u32DeviceIdent_ = ((u8Data[0] & 0x3F) << 24) | ((u8Data[1] & 0xFF) << 16) | ((u8Data[2] & 0xFF) << 8) | (u8Data[3] & 0xFF);
    tof_log(eTOF_LOG_CMD_DEBUG_LVL, "ToF Module FW ID: %01d.%02d  %08X\n", u8Data[0] & 0x3F, u8Data[1], u32DeviceIdent_);
    iRtn = ((u8Data[0] & 0x3F)<<8) | u8Data[1];
    return iRtn;
}

/*************************************************************************************
*@desc gets and display the ToF module serial ID
*@param szSerialId(OUT) returns the ToF Module Serial ID, pass it NULL to display only
*@return ToF module serial ID or <0 if error
*************************************************************************************/
int tof_module_serial_id_read(char * szSerialId)
{
    int iRtn;
    uint8_t u8Data[gMAX_STD_STR_SIZE];
    int ii;

    if (iI2cDevice_ == giINVALID_I2C_DEVICE){
        tof_log(eTOF_LOG_ERROR_LVL, "Need to connect to ToF before using API:%s\n", __FUNCTION__);
        return gTOF_RTN_GENERIC_FAIL /*-1*/;
    }

    iRtn = tof_read_mem(gDEVICE_SERIAL_ID_ADDRESS, gMAX_DEVICE_SERIAL_NUM_SIZE_IN_BYTES, u8Data);
    if (iRtn < gTOF_RTN_SUCCESS /*0*/) {
        tof_log(eTOF_LOG_ERROR_LVL, "Error - %s::unable to read memory(%d)\n", __FUNCTION__, iRtn);
        tof_dump_device_status(NULL);
        return gTOF_RTN_GENERIC_FAIL /*-1*/;
    }
    u8Data[gMAX_DEVICE_SERIAL_NUM_SIZE_IN_BYTES] = '\0';

    //Replace any NON-ASCII chars with '.'
    for (ii = 0; ii < gMAX_DEVICE_SERIAL_NUM_SIZE_IN_BYTES; ii++){
        if ((u8Data[ii]< 0x20) || (u8Data[ii] > 0x7F)) {
            u8Data[ii] = '.';
        }
    }

    /*Only print when not passing back the Serial ID*/
    if (szSerialId == NULL){
        tof_log(eTOF_LOG_CMD_DEBUG_LVL, "Module Serial ID:%s\n", &u8Data[1]);
    }

    if (szSerialId != NULL) {
        strncpy(szSerialId_, (char *)&u8Data[1], gMAX_DEVICE_SERIAL_NUM_SIZE_IN_BYTES);
        strncpy(szSerialId, szSerialId_, gMAX_DEVICE_SERIAL_NUM_SIZE_IN_BYTES);
    }
    return iRtn;
}
//
///******************************************************************************
//*@desc Shared calibration function - only outputs a single measurement
//*@param sCalibrationCfg - ToF calibration configuration that should be used
//*@param u16MbxCalData (IN/OUT) - Fills the field with the data from Mailbox 0x0006
//*                                Note: fail will cause this value to be all zero
//*@return 0 for SUCCESS or <0 if error.  Note: Leave MCPU in Standby state
//******************************************************************************/
int tof_calibration_shared(s3dRangeConfig sCalibrationCfg, uint16_t *pu16MbxCalData, uint16_t u16DataSizeInWords)
{
    int iRtn;
    int iFunctRtn = gTOF_RTN_GENERIC_FAIL /*-1*/;
    int iCnt = 0;
    int iMaxTries = 50;
    int iMaxCntTrys = 10;
    int iDone;
    int iStatus;

    tof_log(eTOF_LOG_CMD_DEBUG_LVL, "%s::%d starting\n", __FUNCTION__, __LINE__);

    /*Zero the calData - so if error zero is return*/
    memset(pu16MbxCalData, 0x00, u16DataSizeInWords * sizeof(uint16_t));

    /*Make sure MCPU is on*/
    iRtn = tof_mcpu_state_set_w_retry(eTOF_MCPU_STATE_ON, TRUE);
    if (iRtn < gTOF_RTN_SUCCESS /*0*/) {
        tof_log(eTOF_LOG_ERROR_LVL, "%s::%d - unable to turn on the MCPU(%d)\n", __FUNCTION__, __LINE__, iRtn);
        goto FAIL;
    }

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
    iRtn = tof_mcpu_on_and_wait_for_idle(50 /*tries*/);
    if (iRtn < gTOF_RTN_SUCCESS /*0*/) {
        tof_log(eTOF_LOG_ERROR_LVL, "%s::%d - unable to get the MCPU on into Idle state(%d)\n", __FUNCTION__, __LINE__, iRtn);
        goto FAIL;
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
        iRtn = tof_mailbox_read_cmd(0x0009, pu16MbxCalData, u16DataSizeInWords);
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

/******************************************************************************
*@desc check to see if device was initial or is power off
*@iInitializeIfUninitialized (IN) - If non-zero, will try to re-initialize if
*           device is uninitialized
*@return 0 for already initialized, -2 for uninitialized, -2 for power off
******************************************************************************/
int tof_device_initialization_status(int reinitializeIfUninitialized)
{
    int iRtn;
    iRtn = i2c_read_word(addr_ADDA_HFCFG0);

    if (iRtn < 0) {
        return gTOF_RTN_POWERED_OFF /*-2*/;
    }
    if (iRtn == 0) {
        if (reinitializeIfUninitialized) {
            return tof_reinitialize_device();
        }
        return gTOF_RTN_NOT_INITIALIZED /*-1*/;
    }
    return gTOF_RTN_SUCCESS /*0*/;
}

/******************************************************************************
*@desc Dumps to the display the device statues ( Interrupts, configuration, and status 
*@PARAM szOutput (IN/OUT) - if NON-NULL will save the output as a pre-allocate
*                           string
******************************************************************************/
void tof_dump_device_status(char *szOutput)
{
    int  piMcpuStatus;
    int iICSR;
    int iIER;
    int iPMU;
    int iI2C_CFG;
    int iDSTATUS;
    int iMCPU_PMC;
    int iPos = 0;

    iICSR = i2c_read_byte(addr_ADDA_ICSR);        /*Address: 0x00*/
    iIER = i2c_read_byte(addr_ADDA_IER);          /*Address: 0x02*/
    iDSTATUS = i2c_read_word(addr_ADDA_DSTATUS);  /*Address: 0x06*/
    iPMU = i2c_read_word(addr_ADDA_PMU_CFG);      /*Address: 0x14*/
    iI2C_CFG = i2c_read_word(addr_I2C_ACC_CFG);   /*Address: 0x1C*/
    iMCPU_PMC = i2c_read_word(addr_ADDA_PMU_CFG); /*Address: 0x1E*/

    if (szOutput == NULL){
        tof_log(eTOF_LOG_ERROR_LVL, "ICSR:0x%02X  IER:0x%02X  DSTATUS:0x%04X  PMU:0x%04X  I2C:0x%04X   MCPU_PMC:0x%04X\n", 0xFF & iICSR
            , 0xFF & iIER, 0xFFFF & iDSTATUS, 0xFFFF & iPMU, 0xFFFF & iI2C_CFG, 0xFFFF & iMCPU_PMC);
    }
    else {
        iPos = sprintf(szOutput, "ICSR:0x%02X  IER:0x%02X  DSTATUS:0x%04X  PMU:0x%04X  I2C:0x%04X   MCPU_PMC:0x%04X\n"
            , 0xFF & iICSR, 0xFF & iIER, 0xFFFF & iDSTATUS, 0xFFFF & iPMU, 0xFFFF & iI2C_CFG, 0xFFFF & iMCPU_PMC);
    }
    tof_dev_status_last_get(TRUE, &piMcpuStatus);
    tof_dump_cfg_setting_get(&szOutput[iPos]);
}

/******************************************************************************
*@desc Dumps to the display the HW configuration
*@PARAM szOutput (IN/OUT) - if NON-NULL will save the output as a pre-allocate 
*                           string
******************************************************************************/
void tof_dump_cfg_setting_get(char *szOutput)
{
    int iRtn;
    char szBuff[256];

    if (iI2cDevice_ == giINVALID_I2C_DEVICE){
        tof_log(eTOF_LOG_ERROR_LVL, "Need to connect to ToF before using API:%s\n", __FUNCTION__);
        return; /*Fail*/
    }

    /*Command configuration Register A*/
    iRtn = i2c_read_word(addr_ADDA_RFSH);
    sprintf(szBuff," CfgR_A:0x%04X\n", iRtn);

    /*Command configuration Register B*/
    iRtn = i2c_read_word(addr_ADDA_AUXCTRL);
    sprintf(szBuff,"%s CfgR_B:0x%04X\n",szBuff,iRtn);

    /*Command configuration Register D*/
    iRtn = i2c_read_word(addr_ADDA_HFCFG0);
    sprintf(szBuff, "%s  Cfg_0:0x%04X\n", szBuff, iRtn);

    /*Command configuration Register 1*/
    iRtn = i2c_read_word(addr_ADDA_HFCFG1);
    sprintf(szBuff, "%s  Cfg_1:0x%04X\n", szBuff, iRtn);

    /*Command configuration Register 2*/
    iRtn = i2c_read_word(addr_ADDA_HFCFG2);
    sprintf(szBuff, "%s  Cfg_2:0x%04X\n", szBuff, iRtn);

    /*Command configuration Register 3*/
    iRtn = i2c_read_word(addr_ADDA_HFCFG3);
    sprintf(szBuff, "%s  Cfg_3:0x%04X\n", szBuff, iRtn);

    if (szOutput == NULL) {
        tof_log(eTOF_LOG_ERROR_LVL, "%s", szBuff);
    }
    else {
        strcpy(szOutput, szBuff);
    }
}

/******************** END of tof_main_funct.c *****************/
