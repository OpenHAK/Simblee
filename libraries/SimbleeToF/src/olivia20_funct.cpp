/*!*****************************************************************************
* @file   Olivia20_funct.c
*
*   Copyright (c) 2015-2017 Heptagon
*
* @brief  Olivia 2.0 supporting functions
*******************************************************************************/

#include "i2c_tof.h"

#define gCALIBRATION_OLIVIA20_F0_DATA_ADDRESS (0xC848)
#define gOLIVIA20_EXPOSURE_TIME_ADDRESS (0x2006)

uint16_t pu16Olivia20CalDataConfidence_[2];

static const uint16_t u16Olivia20DefaultLowAmp_A = 1300;
static const uint16_t u16Olivia20DefaultExposureTimeCutoff_B = 5000;
static const uint16_t u16Olivia20DefaultHighAmp_C = 5600;
static const uint16_t u16Olivia20MaxConfidenceAmp = 6500;
static const int  iOlivia20AmpFactorNumerator = 9;
static const int  iOlivia20AmpFactorDenominator = 10;

static const s3dRangeConfig gsOlivia20DefaultCfg = {
    .u16Cmd_CfgR_A = 0xE100U,
    .u16Cmd_CfgR_B = 0x30FFU,
    .u16HFCfg_0 = 0x07D0U,
    .u16HFCfg_1 = 0x5008U,
    .u16HFCfg_2 = 0xA041U,
    .u16HFCfg_3 = 0x45D4U,
};

static const s3dRangeConfig gsOlivia20Cal100mmWhiteCfg = {
    .u16Cmd_CfgR_A = 0xE100U,
    .u16Cmd_CfgR_B = 0x10FFU,
    .u16HFCfg_0 = 0x07D0U,
    .u16HFCfg_1 = 0x5008U,
    .u16HFCfg_2 = 0x1C81U,
    .u16HFCfg_3 = 0x4154U,
};

static const s3dRangeConfig gsOlivia20Cal400mmBlackCfg = {
    .u16Cmd_CfgR_A = 0xE100U,
    .u16Cmd_CfgR_B = 0x10FFU,
    .u16HFCfg_0 = 0x07D0U,
    .u16HFCfg_1 = 0x5008U,
    .u16HFCfg_2 = 0xFC81U,
    .u16HFCfg_3 = 0x4154U,
};

void tof_olivia20_default_config_get(s3dRangeConfig *ps3dRangeConfig)
{
    memcpy(ps3dRangeConfig, &gsOlivia20DefaultCfg, sizeof(s3dRangeConfig));
}

/******************************************************************************
*@desc Do the white 100mm calibration step - only outputs a single measurement
*      This is called twice to get two measurement
*@param u16MbxCalData (IN/OUT) - Fills the field with the data from Mailbox 0x0006
*                                Note: fail will cause this value to be all zero
*@return 0 for SUCCESS or <0 if error. Note: Leave MCPU in Standby state
******************************************************************************/
int tof_olivia20_calibration_white_100mm(uint16_t * pu16MbxCalData)
{
    uint16_t u16DataSizeInWords = u16Generic_calibration_data_size_in_words_;
    return tof_calibration_shared(gsOlivia20Cal100mmWhiteCfg, pu16MbxCalData, u16DataSizeInWords);
    return -1;
}

/******************************************************************************
*@desc Do the black 400mm calibration step - only outputs a single measurement
*      This is called twice to get two measurement
*@param u16MbxCalData (IN/OUT) - Fills the field with the data from Mailbox 0x0006
*                                Note: fail will cause this value to be all zero
*@return 0 for SUCCESS or <0 if error. Note: Leave MCPU in Standby state
******************************************************************************/
int tof_olivia20_calibration_black_400mm(uint16_t * pu16MbxCalData)
{
    uint16_t u16DataSizeInWords = u16Generic_calibration_data_size_in_words_;
    return tof_calibration_shared(gsOlivia20Cal400mmBlackCfg, pu16MbxCalData, u16DataSizeInWords);
    return -1;
}

/******************************************************************************
*@desc Do the F0 calibration step.  
*@param u16F0Data (IN/OUT) - Fills the field with the data from F0
*                          Note: fail will cause this value to be all zero
*@return 0 for SUCCESS or <0 if error.   Note:Leave MCPU in off state
******************************************************************************/
int tof_olivia20_calibration_f0_data_get(uint16_t * pu16F0Data)
{
    int iRtn;

    tof_log(eTOF_LOG_CMD_DEBUG_LVL, "%s::%d starting\n", __FUNCTION__, __LINE__);

    if (iI2cDevice_ == giINVALID_I2C_DEVICE){
        tof_log(eTOF_LOG_ERROR_LVL, "Need to connect to ToF before using API:%s\n", __FUNCTION__);
        return gTOF_RTN_GENERIC_FAIL /*-1*/;
    }

    memset(pu16F0Data, 0x00, u16Generic_calibration_f0_data_size_in_words_ * sizeof(uint16_t));

    iRtn = tof_read_mem(gCALIBRATION_OLIVIA20_F0_DATA_ADDRESS, u16Generic_calibration_f0_data_size_in_words_ * sizeof(uint16_t), (uint8_t *)pu16F0Data);

    if (iRtn < gTOF_RTN_SUCCESS /*0*/) {
        tof_log(eTOF_LOG_ERROR_LVL, "Error - %s::unable to read F0 Data(%d)\n", __FUNCTION__, iRtn);
        tof_dump_device_status(NULL);
        return gTOF_RTN_GENERIC_FAIL /*-1*/;
    }
    return gTOF_RTN_SUCCESS /*0*/;
}

/******************************************************************************
*@desc Get the exposure time from the last measurement for Olivia 2.0
*@param pu16ExposureTime_usec (IN/OUT) - The exposure Time in uSeconds
*@return 0 for SUCCESS or <0 if error
******************************************************************************/
int tof_olivia20_exposure_time(uint16_t *pu16ExposureTime_usec)
{
    int iRtn;

    tof_log(eTOF_LOG_CMD_DEBUG_LVL, "%s::%d starting\n", __FUNCTION__, __LINE__);

    /*For OLIVIA 2.0 - Get the Exposure time in uSeconds*/
    iRtn = tof_read_indirect_word(gOLIVIA20_EXPOSURE_TIME_ADDRESS, pu16ExposureTime_usec);

    return iRtn;
}

/******************************************************************************
*@desc get a ToF single measurement results once it is ready
*@param piDist_mm (OUT) - Distance in number of mm (1-4095mm): if error value is 00
*@param piAmplitude (OUT) - vector amplitude as a measure of the quality of the measure
*@param pu16ExposureTime_usec (OUT) - The Exposure Time in uSeconds that was use to get the measurement
*@param piErrCode (OUT) - Error Code: 00=>no error, 01=>Near target, 10=>Far field, 11=> General Err
*@param piExtendError(OUT) - Extend Error code (Enable by options in u32MeasFlags)
*@param u32MeasFlags (IN) - Configure what measure flags should be use when getting the measurement
*@return Distance in number of mm or 0xFFFF if error.
*@NOTE: Leave in standby state, unless eTOF_MEASURE_FLAG_DONT_LEAVE_IN_STANDBY then left in MCPU ON state
******************************************************************************/
int tof_olivia20_single_measure_n_err_get_results(int *piDist_mm, int *piAmplitude, uint16_t *pu16ExposureTime_usec, int *piErrCode, int *piExtendError, uint32_t u32MeasFlags)
{
    int iRtn;
    int iFunctionRtn = gTOF_RTN_GENERIC_FAIL /*-1*/;

    tof_log(eTOF_LOG_CMD_DEBUG_LVL, "%s::%d starting\n", __FUNCTION__, __LINE__);

    /*Default values if there is an error*/
    *piDist_mm = 0xFFFF;
    *piAmplitude = 0;
    *pu16ExposureTime_usec = 0U;
    *piErrCode = RSLTR_ERR_CODE_ERROR/*3*/;
    *piExtendError = eTOF_MEASURE_EXENDED_ERR_GENERAL_ERROR;

    iRtn = i2c_read_word(addr_ADDA_RSLTR);
    if (iRtn < gTOF_RTN_SUCCESS /*0*/) {
        tof_log(eTOF_LOG_ERROR_LVL, "%s::%d - Unable to get Mesaurement results(%d)\n", __FUNCTION__, __LINE__, iRtn);
        goto FINISH;
    }

    *piDist_mm = (iRtn & DISTANCE_MM_MASK) >> DISTANCE_MM_SHIFT;
    *piErrCode = (iRtn & RSLTR_ERR_CODE_MASK) >> RSLTR_ERR_CODE_SHIFT;
    if (iRtn & RSLTR_VALID) {
        /*Get the confidence value of the last measurement value*/
        iRtn = i2c_read_word(addr_ADDA_RSL_CON);
        if (iRtn < gTOF_RTN_SUCCESS /*0*/) {
            tof_log(eTOF_LOG_ERROR_LVL, "%s::%d - unable to get Amplitude value(%d)\n", __FUNCTION__, __LINE__, iRtn);
            goto FINISH;
        }
        *piAmplitude = (iRtn & CNFD_VEC_AMPL_MASK) >> CNFD_VEC_AMPL_SHIFT;

        tof_generic_exposure_time_(pu16ExposureTime_usec);
        iFunctionRtn = *piDist_mm;
    }
    else {
        *piDist_mm = 0xFFFF;
        iFunctionRtn = 0;
    }

    *piExtendError = eTOF_MEASURE_EXENDED_ERR_NO_ERROR;
    if (u32MeasFlags & eTOF_MEASURE_FLAG_EXTRA_ERROR_DETECTION) {
        //Mailbox 0x0008
    }


FINISH:
    if ((u32MeasFlags & eTOF_MEASURE_FLAG_DONT_LEAVE_IN_STANDBY) != eTOF_MEASURE_FLAG_DONT_LEAVE_IN_STANDBY) {
        /*Go into Stand-by mode - low power mode*/
        i2c_write_word(addr_ADDA_CMD, CMD_VALID_MASK | CMD_GO_STANDBY /*0x0090*/);
    }
    return iFunctionRtn;
}
/******************** END of olivia20_funct.c *****************/