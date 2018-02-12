/*!*****************************************************************************
* @file   trinity_funct.c
*
*   Copyright (c) 2015-2017 Heptagon
*
* @brief  Trinity functions
*******************************************************************************/

#include "i2c_tof.h"

#define gCALIBRATION_TRINITY_F0_DATA_ADDRESS (0xC848)
#define gTRINITY_BASE_DIST_ADDRESS (0x2000)
#define gTRINITY_BASE_AMPL_ADDRESS (0x2012)
#define gTRINITY_EXPOSURE_TIME_ADDRESS (0x202A)

static const uint16_t u16TrinityDefaultLowAmp_A = 1300;
static const uint16_t u16TrinityDefaultExposureTimeCutoff_B = 5000;
static const uint16_t u16TrinityDefaultHighAmp_C = 5600;
static const uint16_t u16TrinityMaxConfidenceAmp = 6500;

static const s3dRangeConfig gsTrinityDefaultCfg = {
    .u16Cmd_CfgR_A = 0xE100U,
    .u16Cmd_CfgR_B = 0x10FFU,
    .u16HFCfg_0 = 0x07D0U,
    .u16HFCfg_1 = 0x5008U,
    .u16HFCfg_2 = 0xA041U,
    .u16HFCfg_3 = 0x45D4U,
};

static const s3dRangeConfig gsTrinityCal100mmWhiteCfg = {
    .u16Cmd_CfgR_A = 0xE100U,
    .u16Cmd_CfgR_B = 0x10FFU,
    .u16HFCfg_0 = 0x07D0U,
    .u16HFCfg_1 = 0x5008U,
    .u16HFCfg_2 = 0x1C81U,
    .u16HFCfg_3 = 0x4154U,
};

static const s3dRangeConfig gsTrinityCal400mmBlackCfg = {
    .u16Cmd_CfgR_A = 0xE100U,
    .u16Cmd_CfgR_B = 0x10FFU,
    .u16HFCfg_0 = 0x07D0U,
    .u16HFCfg_1 = 0x5008U,
    .u16HFCfg_2 = 0xFC81U,
    .u16HFCfg_3 = 0x4154U,
};

void tof_trinity_default_config_get(s3dRangeConfig *ps3dRangeConfig)
{
    memcpy(ps3dRangeConfig, &gsTrinityDefaultCfg, sizeof(s3dRangeConfig));
}

/**********************************************************************************************
*@desc Does an extra check on the measurement to see if value has high confidence that is valid
*@     Note: only valid for Trinity
*@param iDist_mm (IN) - Distance in number of mm (1-2047mm)
*@param iAmp_Y (IN) - (confidence) amplitude as a measure of the quality of the measure(Y-Axis)
*@param u16ExposureTime_usec(IN) - Exposure time that was last use to make the measurement
*@param piErrCode (IN) - Error Code: 00=>no error, 01=>Near target 10=>Far field, 11=> General Err
*@param piExtendError (OUT) - Extend Error Code
*@return updated the piErrCode if the distance is not a high confidence that is valid
*        returns a 7 to signal that values don't have a high confidence that is valid.
***********************************************************************************************
*  Value doesn't have a high confidence, if it in Region AA or Region BB (Right Trapezoid)
*  Region AA ==> (X<B && Y>A)
*  Region BB ==> [(X>B) && (BY < (A-C)X + BC)]
* Pt_C +
*      |
* Pt_A +************|
*      |************|****
*Y-Axis|***Region***|*Region*
*iAmp_Y|*****AA*****|***BB*******
*       ------------+------------
*                 Pt_B    X-Axis -> Measurement Exp Time(u16ExposureTime_X)
***********************************************************************************************/
int tof_trinty_af_confidence_checker(int iDist_mm, int iAmplitude_Y, uint16_t u16ExposureTime_X_usec, int *piErrCode, int *piExtendError)
{
    int iLowAmp_A = (int)u16TrinityDefaultLowAmp_A;                         /*Pt_A*/
    int iExposureTimeCutoff_B = (int)u16TrinityDefaultExposureTimeCutoff_B; /*Pt_B*/
    int iHighAmp_C = (int)u16TrinityDefaultHighAmp_C;                       /*Pt_C*/
    int iAmptoDistFactor = 12;
    int iMinDistance_mm = 100;  /*From DataSheet*/
    int iMaxDistance_mm = 2000; /*From DataSheet*/

    tof_log(eTOF_LOG_CMD_DEBUG_LVL, "%s::%d starting\n", __FUNCTION__, __LINE__);

    *piExtendError = eTOF_MEASURE_EXENDED_ERR_NO_ERROR/*0*/; /*TRUSTED VALUE*/

    if (*piErrCode != 0){
        *piExtendError = eTOF_MEASURE_EXENDED_ERR_GENERAL_ERROR;
    }

    if (u16ExposureTime_X_usec < iExposureTimeCutoff_B) {
        /*Checking to see if value is in the Region AA */
        if (iAmplitude_Y < iLowAmp_A) {
            *piExtendError = eTOF_MEASURE_EXENDED_ERR_GENERAL_ERROR;
            return eTOF_MEASURE_EXENDED_ERR_GENERAL_ERROR;
        }
    }
    else /*Checking to see if value is in Region BB */
        if ((iExposureTimeCutoff_B*iAmplitude_Y) < (iLowAmp_A - iHighAmp_C)*u16ExposureTime_X_usec + (iExposureTimeCutoff_B*iHighAmp_C)) {
            *piExtendError = eTOF_MEASURE_EXENDED_ERR_GENERAL_ERROR;
            return eTOF_MEASURE_EXENDED_ERR_GENERAL_ERROR;
        }

    if ((iDist_mm < iMinDistance_mm) || (iDist_mm > iMaxDistance_mm)) {
        *piExtendError = eTOF_MEASURE_EXENDED_ERR_GENERAL_ERROR;
        return eTOF_MEASURE_EXENDED_ERR_GENERAL_ERROR;
    }

    if (iAmplitude_Y <= iAmptoDistFactor*iMaxDistance_mm / iDist_mm) {
        *piExtendError = eTOF_MEASURE_EXENDED_ERR_GENERAL_ERROR;
        return eTOF_MEASURE_EXENDED_ERR_GENERAL_ERROR;
    }
    return *piExtendError;
}

/******************************************************************************
*@desc Do the white 100mm calibration step - only outputs a single measurement
*      This is called twice to get two measurement
*@param u16MbxCalData (IN/OUT) - Fills the field with the data from Mailbox 0x0006
*                                Note: fail will cause this value to be all zero
*@return 0 for SUCCESS or <0 if error
******************************************************************************/
int tof_trinity_calibration_white_100mm(uint16_t * pu16MbxCalData)
{
    uint16_t u16DataSizeInWords = u16Generic_calibration_data_size_in_words_;
    return tof_pre130_calibration_shared(gsTrinityCal100mmWhiteCfg, pu16MbxCalData, u16DataSizeInWords);
}

/******************************************************************************
*@desc Do the black 400mm calibration step - only outputs a single measurement
*      This is called twice to get two measurement
*@param u16MbxCalData (IN/OUT) - Fills the field with the data from Mailbox 0x0006
*                                Note: fail will cause this value to be all zero
*@return 0 for SUCCESS or <0 if error
******************************************************************************/
int tof_trinity_calibration_black_400mm(uint16_t * pu16MbxCalData)
{
    uint16_t u16DataSizeInWords = u16Generic_calibration_data_size_in_words_;
    return tof_pre130_calibration_shared(gsTrinityCal400mmBlackCfg, pu16MbxCalData, u16DataSizeInWords);
}

/******************************************************************************
*@desc Do the F0 calibration step.
*@param u16F0Data (IN/OUT) - Fills the field with the data from F0
*                          Note: fail will cause this value to be all zero
*@return 0 for SUCCESS or <0 if error.   Note:Leave MCPU in off state
******************************************************************************/
int tof_trinity_calibration_f0_data_get(uint16_t * pu16F0Data)
{
    int iRtn;

    tof_log(eTOF_LOG_CMD_DEBUG_LVL, "%s::%d starting\n", __FUNCTION__, __LINE__);

    if (iI2cDevice_ == giINVALID_I2C_DEVICE){
        tof_log(eTOF_LOG_ERROR_LVL, "Need to connect to ToF before using API:%s\n", __FUNCTION__);
        return gTOF_RTN_GENERIC_FAIL /*-1*/;
    }

    memset(pu16F0Data, 0x00, u16Generic_calibration_f0_data_size_in_words_ * sizeof(uint16_t));

    iRtn = tof_read_mem(gCALIBRATION_TRINITY_F0_DATA_ADDRESS, u16Generic_calibration_f0_data_size_in_words_ * sizeof(uint16_t), (uint8_t *)pu16F0Data);

    if (iRtn < gTOF_RTN_SUCCESS /*0*/) {
        tof_log(eTOF_LOG_ERROR_LVL, "Error - %s::unable to read F0 Data(%d)\n", __FUNCTION__, iRtn);
        tof_dump_device_status(NULL);
        return gTOF_RTN_GENERIC_FAIL /*-1*/;
    }
    return gTOF_RTN_SUCCESS /*0*/;
}


/******************************************************************************
*@desc do single Trinity measurement.  Request the measurement, wait and get measurement.  T
*     This API is a blocking call. 
*@param u16PerPixelMeas_mm (OUT) - returns the measurement of each pixel in units of mm
*@param u16PerPixelAmplitude (OUT) - vector amplitude as a measure of the quality of the measure 
*                                    per pixel.
*@param u8PerPixelErr - returns the error code per pixel
*@                      Error Code: 00=>no error, 01=>Near target, 10=>Far field, 11=> General Err
*@param pu8GrpErr (OUT) - Is non-zero if any pixel has an error
*@param pu16ExposureTime_usec (OUT) - The Exposure Time in uSeconds that was used to get the measurement
*@param u32MeasFlags (IN) - Configure what measure flags should be use when getting the measurement
*@Return 0 for Success, else a negative number for error
*@NOTE All memory pointers assumes that the memory has been already allowed
******************************************************************************/
int tof_generic_per_pixel_mesurement_get_w_wait(uint16_t * pu16PerPixelMeas_mm, uint16_t * pu16PerPixelAmplitude, uint8_t * pu8PerPixelErr, uint8_t *pu8PerPixelExtErr, uint8_t *pu8GrpErr, uint16_t *pu16ExposureTime_usec, uint32_t u32MeasFlags)
{
    int iRtn;
    int iPixelId;
    uint32_t u32MaxWaitTime_ms = 500;
    uint16_t u16TempMeas;

    tof_log(eTOF_LOG_CMD_DEBUG_LVL, "%s::%d starting\n", __FUNCTION__, __LINE__);

    *pu16ExposureTime_usec = 0U; /*Just in case there is an error*/

    if (iI2cDevice_ == giINVALID_I2C_DEVICE){
        tof_log(eTOF_LOG_ERROR_LVL, "Need to connect to ToF before using API:%s\n", __FUNCTION__);
        return gTOF_RTN_GENERIC_FAIL /*-1*/;
    }

    if (CurrTofType_ != eTOF_TYPE_TRINITY) {
        tof_log(eTOF_LOG_ERROR_LVL, "Function:%s only works for Trinity\n", __FUNCTION__);
        return gTOF_RTN_GENERIC_FAIL /*-1*/;
    }

    *pu8GrpErr = 0;

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

    /*For Trinity - Get the Exposure time in uSeconds*/
    tof_generic_exposure_time_(pu16ExposureTime_usec);

    /*Get the measurement value*/
    for (iPixelId = 0; iPixelId < gTRINITY_NUM_OBJ_PIXELS; iPixelId++){
        iRtn = tof_read_indirect_word(gTRINITY_BASE_DIST_ADDRESS + (uint16_t)(2 * iPixelId), &u16TempMeas);
        if (iRtn < gTOF_RTN_SUCCESS /*0*/) {
            tof_log(eTOF_LOG_ERROR_LVL, "%s::%d - unable to get Mesaurement(%d) Err:%d\n", __FUNCTION__, __LINE__, iPixelId, iRtn);
            goto FAIL;
        }
        pu16PerPixelMeas_mm[iPixelId] = (u16TempMeas & DISTANCE_MM_MASK_OLD) >> DISTANCE_MM_SHIFT_OLD;
        pu8PerPixelErr[iPixelId] = u16TempMeas >> RSLTR_ERR_CODE_SHIFT;
        if (pu8PerPixelErr[iPixelId] != 0) {
            *pu8GrpErr = pu8PerPixelErr[iPixelId];
        }

        iRtn = tof_read_indirect_word(gTRINITY_BASE_AMPL_ADDRESS + (uint16_t)(2 * iPixelId), &pu16PerPixelAmplitude[iPixelId]);
        if (iRtn < gTOF_RTN_SUCCESS /*0*/) {
            tof_log(eTOF_LOG_ERROR_LVL, "%s::%d - unable to get Amplitude(%d) Err:%d\n", __FUNCTION__, __LINE__, iPixelId, iRtn);
            goto FAIL;
        }
    }

    return gTOF_RTN_SUCCESS; /*0*/

FAIL:
    tof_dump_device_status(NULL);
    return gTOF_RTN_GENERIC_FAIL /*-1*/;
}

/******************************************************************************
*@desc Get the exposure time from the last measurement for Trinity
*@param pu16ExposureTime_usec (IN/OUT) - The exposure Time in uSeconds
*@return 0 for SUCCESS or <0 if error
******************************************************************************/
int tof_trinity_exposure_time(uint16_t *pu16ExposureTime_usec)
{
    int iRtn;

    tof_log(eTOF_LOG_CMD_DEBUG_LVL, "%s::%d starting\n", __FUNCTION__, __LINE__);

    if (CurrTofType_ != eTOF_TYPE_TRINITY) {
        tof_log(eTOF_LOG_ERROR_LVL, "Function:%s only works for Trinity\n", __FUNCTION__);
        return gTOF_RTN_GENERIC_FAIL /*-1*/;
    }

    /*For TRINITY - Get the Exposure time in uSeconds*/
    iRtn = tof_read_indirect_word(gTRINITY_EXPOSURE_TIME_ADDRESS, pu16ExposureTime_usec);

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
int tof_trinity_single_measure_n_err_get_results(int *piDist_mm, int *piAmplitude, uint16_t *pu16ExposureTime_usec, int *piErrCode, int *piExtendError, uint32_t u32MeasFlags)
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

    *piDist_mm = (iRtn & DISTANCE_MM_MASK_OLD) >> DISTANCE_MM_SHIFT_OLD;
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
        tof_trinty_af_confidence_checker(*piDist_mm, *piAmplitude, *pu16ExposureTime_usec, piErrCode, piExtendError);
    }


FINISH:
    if ((u32MeasFlags & eTOF_MEASURE_FLAG_DONT_LEAVE_IN_STANDBY) != eTOF_MEASURE_FLAG_DONT_LEAVE_IN_STANDBY) {
        /*Go into Stand-by mode - low power mode*/
        i2c_write_word(addr_ADDA_CMD, CMD_VALID_MASK | CMD_GO_STANDBY /*0x0090*/);
    }
    return iFunctionRtn;
}
/******************** END of trinity_funct.c *****************/