/*
   Copyright (c) 2017 RF Digital Corp. All Rights Reserved.

   The source code contained in this file and all intellectual property embodied in
   or covering the source code is the property of RF Digital Corp. or its licensors.
   Your right to use this source code and intellectual property is non-transferable,
   non-sub licensable, revocable, and subject to terms and conditions of the
   SIMBLEE SOFTWARE LICENSE AGREEMENT.
   http://www.simblee.com/licenses/SimbleeSoftwareLicenseAgreement.txt

   THE SOURCE CODE IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND.

   This heading must NOT be removed from this file.
*/

#ifndef _Simblee_TMG4903_H_
#define _Simblee_TMG4903_H_

#include <Arduino.h>
#include <Wire.h>


#define TMG4903_ADDRESS   0x39     

#define TMG4903_ENABLE            0x80    // Enables states and interrupts
#define TMG4903_ATIME             0x81    // ADC integration time
#define TMG4903_PTIME             0x82    // Proximity sample time
#define TMG4903_WTIME             0x83    // ALS wait time
#define TMG4903_AILTL             0x84    // ALS interrupt low threshold low byte
#define TMG4903_AILTH             0x85    // ALS interrupt low threshold high byte
#define TMG4903_AIHTL             0x86    // ALS interrupt high threshold low byte
#define TMG4903_AIHTH             0x87    // ALS interrupt high threshold high byte
#define TMG4903_PILTL             0x88    // Proximity interrupt low threshold low byte
#define TMG4903_PILTH             0x89    // Proximity interrupt high threshold high byte
#define TMG4903_PIHTL             0x8A    // Proximity interrupt low threshold low byte
#define TMG4903_PIHTH             0x8B    // Proximity interrupt high threshold high byte
#define TMG4903_PERS              0x8C    // ALS & Proximity interrupt persistence filters
#define TMG4903_CFG0              0x8D    // Configuration register zero
#define TMG4903_PGCFG0            0x8E    // Proximity pulse width and count
#define TMG4903_PGCFG1            0x8F    // Proximity gain and LED current
#define TMG4903_CFG1              0x90    // Configuration register one
#define TMG4903_REVID             0x91    // Revision ID
#define TMG4903_ID                0x92    // Device ID
#define TMG4903_STATUS            0x93    // Device status register one
#define TMG4903_CDATAL            0x94    // Clear ADC low data register
#define TMG4903_CDATAH            0x95    // Clear ADC high data register
#define TMG4903_RDATAL            0x96    // Red ADC low data register
#define TMG4903_RDATAH            0x97    // Red ADC high data register
#define TMG4903_GDATAL            0x98    // Green ADC low data register
#define TMG4903_GDATAH            0x99    // Green ADC high data register
#define TMG4903_BDATAL            0x9A    // Blue ADC low data register
#define TMG4903_BDATAH            0x9B    // Blue ADC high data register
#define TMG4903_PDATAL            0x9C    // Proximity ADC low data register
#define TMG4903_PDATAH            0x9D    // Proximity ADC high data register
#define TMG4903_STATUS2           0x9E    // Additional device status
#define TMG4903_CFG2              0x9F    // Configuration register two
#define TMG4903_ICONFIG           0xA0    // IRBeam configuration register one
#define TMG4903_ICONFIG2          0xA1    // IRBeam configuration register two
#define TMG4903_ISNL              0xA2    // IRBeam symbol loops
#define TMG4903_ISOFF             0xA3    // IRBeam delay between symbol loops
#define TMG4903_IPNL              0xA4    // IRbeam packet loops
#define TMG4903_IPOFF             0xA5    // IRBeam delay between packet loops
#define TMG4903_IBT               0xA6    // IRBeam time period
#define TMG4903_ISLEN             0xA7    // IRBeam symbol length
#define TMG4903_ISTATUS           0xA8    // IRBeam status
#define TMG4903_ISTART            0xA9    // IRBeam start transmission
#define TMG4903_CFG3              0xAB    // Configuration register three
#define TMG4903_CFG4              0xAC    // Configuration register four
#define TMG4903_CFG5              0xAD    // Configuration register five
#define TMG4903_GCFG0             0xB0    // Gesture configuration register zero
#define TMG4903_GCFG1             0xB1    // Gesture configuration register one
#define TMG4903_GCFG2             0xB2    // Gesture configuration register two
#define TMG4903_STATUS3           0xB3    // Status register three
#define TMG4903_GTIME             0xB4    // Gesture time
#define TMG4903_GST_CTRL          0xB5    // Gesture control
#define TMG4903_GTHR_INL          0xB6    // Gesture entry threshold low byte
#define TMG4903_GTHR_INH          0xB7    // Gesture entry threshold high byte
#define TMG4903_GTHR_OUTL         0xB8    // Gesture exit threshold low byte
#define TMG4903_GTHR_OUTH         0xB9    // Gesture exit threshold high byte
#define TMG4903_GFIFO_LVL         0xBA    // Gesture FIFO entries waiting for readout
#define TMG4903_GSTATUS           0xBB    // Gesture status
#define TMG4903_CONTROL           0xBC    // Control register
#define TMG4903_AUXID             0xBD    // Auxiliary ID
#define TMG4903_OFFSETNL          0xC0    // North channel offset low byte
#define TMG4903_OFFSETNH          0xC1    // North channel offset high byte
#define TMG4903_OFFSETSL          0xC2    // South channel offset low byte
#define TMG4903_OFFSETSH          0xC3    // South channel offset high byte
#define TMG4903_OFFSETWL          0xC4    // West channel offset low byte
#define TMG4903_OFFSETWH          0xC5    // West channel offset high byte
#define TMG4903_OFFSETEL          0xC6    // East channel offset low byte
#define TMG4903_OFFSETEH          0xC7    // East channel offset high byte
#define TMG4903_PBSLN_MEASL       0xD0    // Measured baseline low byte
#define TMG4903_PBSLN_MEASH       0xD1    // Measured baseline high byte
#define TMG4903_PBSLNL            0xD2    // Stored baseline low byte
#define TMG4903_PBSLNH            0xD3    // Stored baseline high byte
#define TMG4903_AZ_CONFIG         0xD6    // Configure CRGB autozero frequency
#define TMG4903_CALIB             0xD7    // Start offset calibration
#define TMG4903_CALIBCFG0         0xD8    // Calibration configuration register zero
#define TMG4903_CALIBCFG1         0xD9    // Calibration configuration register one
#define TMG4903_CALIBCFG2         0xDA    // Calibration configuration register two
#define TMG4903_CALIBCFG3         0xDB    // Calibration configuration register three
#define TMG4903_CALIBSTAT         0xDC    // Calibration status
#define TMG4903_INTENAB           0xDD    // Interrupt enable
#define TMG4903_INCLEAR           0xDE    // Interrupt clear
#define TMG4903_GFIFO0L           0xF8    // Gesture FIFO North low byte
#define TMG4903_GFIFO0H           0xF9    // Gesture FIFO North high byte
#define TMG4903_GFIFO1L           0xFA    // Gesture FIFO South low byte
#define TMG4903_GFIFO1H           0xFB    // Gesture FIFO South high byte
#define TMG4903_GFIFO2L           0xFC    // Gesture FIFO West low byte
#define TMG4903_GFIFO2H           0xFD    // Gesture FIFO West high byte
#define TMG4903_GFIFO3L           0xFE    // Gesture FIFO East low byte
#define TMG4903_GFIFO3H           0xFF    // Gesture FIFO East high byte

enum 
{
  TMG4903_PON = 0x01,    //Power on
  TMG4903_AEN = 0x02,    //ALS enable
  TMG4903_PEN = 0x04,    //Proximity enable
  TMG4903_WEN = 0x08,    //Wait enable
  TMG4903_AIEN = 0x10,   //ALS interrupt enable
  TMG4903_PIEN = 0x20,   //Proximity interrupt enable
  TMG4903_GEN = 0x40,    //Gesture enable
  TMG4903_IBEN = 0x80   //IR Beam enable
};

enum
{
  TMG4903_ALS_INTERRUPT_EVERY_CYCLE,   
  TMG4903_ALS_INTERRUPT_EVERY_OCCURANCE, 
  TMG4903_ALS_INTERRUPT_EVERY_2_OCCURANCES,   
  TMG4903_ALS_INTERRUPT_EVERY_3_OCCURANCES,   
  TMG4903_ALS_INTERRUPT_EVERY_5_OCCURANCES, 
  TMG4903_ALS_INTERRUPT_EVERY_10_OCCURANCES,  
  TMG4903_ALS_INTERRUPT_EVERY_15_OCCURANCES,   
  TMG4903_ALS_INTERRUPT_EVERY_20_OCCURANCES,   
  TMG4903_ALS_INTERRUPT_EVERY_25_OCCURANCES, 
  TMG4903_ALS_INTERRUPT_EVERY_30_OCCURANCES, 
  TMG4903_ALS_INTERRUPT_EVERY_35_OCCURANCES, 
  TMG4903_ALS_INTERRUPT_EVERY_40_OCCURANCES, 
  TMG4903_ALS_INTERRUPT_EVERY_45_OCCURANCES, 
  TMG4903_ALS_INTERRUPT_EVERY_50_OCCURANCES, 
  TMG4903_ALS_INTERRUPT_EVERY_55_OCCURANCES, 
};

enum
{
  TMG4903_PROXIMITY_INTERRUPT_EVERY_CYCLE,   
  TMG4903_PROXIMITY_INTERRUPT_EVERY_OCCURANCE, 
  TMG4903_PROXIMITY_INTERRUPT_EVERY_2_OCCURANCES,   
  TMG4903_PROXIMITY_INTERRUPT_EVERY_3_OCCURANCES,   
  TMG4903_PROXIMITY_INTERRUPT_EVERY_5_OCCURANCES, 
  TMG4903_PROXIMITY_INTERRUPT_EVERY_10_OCCURANCES,  
  TMG4903_PROXIMITY_INTERRUPT_EVERY_15_OCCURANCES,   
  TMG4903_PROXIMITY_INTERRUPT_EVERY_20_OCCURANCES,   
  TMG4903_PROXIMITY_INTERRUPT_EVERY_25_OCCURANCES, 
  TMG4903_PROXIMITY_INTERRUPT_EVERY_30_OCCURANCES, 
  TMG4903_PROXIMITY_INTERRUPT_EVERY_35_OCCURANCES, 
  TMG4903_PROXIMITY_INTERRUPT_EVERY_40_OCCURANCES, 
  TMG4903_PROXIMITY_INTERRUPT_EVERY_45_OCCURANCES, 
  TMG4903_PROXIMITY_INTERRUPT_EVERY_50_OCCURANCES, 
  TMG4903_PROXIMITY_INTERRUPT_EVERY_55_OCCURANCES, 
};

enum
{
  TMG4903_CFG0_WAIT_LONG        = 0x04,
  TMG4903_CFG0_RLOWPOWER_IDLE   = 0x20 
};

enum
{
  TMG4903_CFG1_AGAIN_1x                        = 0x00,
  TMG4903_CFG1_AGAIN_4x                        = 0x01,
  TMG4903_CFG1_AGAIN_16x                       = 0x02,
  TMG4903_CFG1_AGAIN_64x                       = 0x03,
  TMG4903_CFG1_ALS_SATURATION_INTERUPT         = 0x40,
  TMG4903_CFG1_PROXIMITY_SATURATION_INTERUPT   = 0x80  
};

enum
{
  TMG4903_CFG2_ALS_MASK                        = 0x08,
  TMG4903_CFG2_PROXIMITY_MASK_NORTH            = 0x10,
  TMG4903_CFG2_PROXIMITY_MASK_SOUTH            = 0x20,
  TMG4903_CFG2_PROXIMITY_MASK_WEST             = 0x40,
  TMG4903_CFG2_PROXIMITY_MASK_EAST             = 0x80
};

enum
{
  TMG4903_CFG3_SLEEP_AFTER_INTERRUPT           = 0x08,
  TMG4903_CFG3_USEPROX_FOR_ALS                 = 0x40
};

enum
{
  TMG4903_CFG4_PROX_INT_DIRECT_GPIO            = 0x10,
  TMG4903_CFG4_PROX_INT_DIRECT                 = 0x20,
  TMG4903_CFG4_ALS_INT_DIRECT_GPIO             = 0x40,
  TMG4903_CFG4_PALS_INT_DIRECT                 = 0x80
};

enum
{
  TMG4903_CFG5_PPROX_FILTER                    = 0x01,
  TMG4903_CFG5_PROX_FILTER_SIZE                = 0x02,        
  TMG4903_CFG5_PROX_FILTER_DOWNSAMPLE          = 0x04,        
  TMG4903_CFG5_DISABLE_IR_CORRECTION           = 0x08,
  TMG4903_CFG5_LONG_LTFSTOP_DISCARD_ALS        = 0x20            
};

enum e_TMG4903_PGCFG0
{
  TMG4903_PGCFG0_PROXIMITY_PULSE_COUNT_1    = 0x00,
  TMG4903_PGCFG0_PROXIMITY_PULSE_COUNT_4	= 0x03,
  TMG4903_PGCFG0_PROXIMITY_PULSE_COUNT_8	= 0x08,
  TMG4903_PGCFG0_PROXIMITY_PULSE_COUNT_16	= 0x10,
  TMG4903_PGCFG0_PROXIMITY_PULSE_COUNT_32	= 0x20, 
  TMG4903_PGCFG0_PROXIMITY_PULSE_COUNT_64	= 0x3F,
  TMG4903_PGCFG0_PROXIMITY_PULSE_4us      = 0x00,
  TMG4903_PGCFG0_PROXIMITY_PULSE_8us      = 0x40,
  TMG4903_PGCFG0_PROXIMITY_PULSE_16us     = 0x80,
  TMG4903_PGCFG0_PROXIMITY_PULSE_32us     = 0xC0
};

enum e_TMG4903_PGCFG1
{
  TMG4903_PGCFG1_LED_DRIVE_10MA           = 0x00,
  TMG4903_PGCFG1_LED_DRIVE_30MA           = 0x02,
  TMG4903_PGCFG1_LED_DRIVE_50MA           = 0x04,
  TMG4903_PGCFG1_LED_DRIVE_70MA           = 0x06,
  TMG4903_PGCFG1_LED_DRIVE_90MA           = 0x08,
  TMG4903_PGCFG1_LED_DRIVE_110MA          = 0x0A,
  TMG4903_PGCFG1_LED_DRIVE_130MA          = 0x0C,
  TMG4903_PGCFG1_LED_DRIVE_150MA          = 0x0E,
  TMG4903_PGCFG1_LED_DRIVE_170MA          = 0x10,
  TMG4903_PGCFG1_LED_DRIVE_190MA          = 0x12,
  TMG4903_PGCFG1_LED_DRIVE_210MA          = 0x14,
  TMG4903_PGCFG1_LED_DRIVE_230MA          = 0x16,
  TMG4903_PGCFG1_LED_DRIVE_250MA          = 0x18,
  TMG4903_PGCFG1_LED_DRIVE_270MA          = 0x1A,
  TMG4903_PGCFG1_LED_DRIVE_290MA          = 0x1C,
  TMG4903_PGCFG1_LED_DRIVE_310MA          = 0x1E,
  TMG4903_PGCFG1_PROXIMITY_GAIN_1x        = 0x00,
  TMG4903_PGCFG1_PROXIMITY_GAIN_2x        = 0x40,
  TMG4903_PGCFG1_PROXIMITY_GAIN_4x        = 0x80,
  TMG4903_PGCFG1_PROXIMITY_GAIN_8x        = 0xC0
};

enum e_TMG4903_CFG1
{
  TMG4903_CFG1_ALS_GAIN_1x        = 0x00,
  TMG4903_CFG1_ALS_GAIN_4x        = 0x01,
  TMG4903_CFG1_ALS_GAIN_16x       = 0x02,
  TMG4903_CFG1_ALS_GAIN_64x       		= 0x03,
  TMG4903_CFG1_ALS_SATURATION_INT       = 0x40,
  TMG4903_CFG1_PROX_SATURATION_INT      = 0x80
};



class Simblee_TMG4903 {
  public:
    boolean begin(void);
	
	void setEnable(uint8_t value);

    uint8_t readID(void);
    void setALSintegrationTime(uint8_t integrationTime); //1 cycle = 2.78ms, 0xFF = 2.78ms, 0x00 = 711ms. Max count = cycles * 1024, default 0xFF
    void setProximitySampleTime(uint8_t sampleTime); // 0x00 = 2.78ms 360Hz, 0xFF = 711ms, Sample time 2.78ms X (PTIME +1) Frequency 360/PTIME + 1, default 0x00
    void setWaitTime(uint8_t waitTime); //set wait time, amount of time in low power mode, 0x00 = 712ms (8.54s if WLONG =1), 0xFF = 2.78ms (0.03s if WLONG =1)
    void setALSinterruptThreshold(uint16_t lowerThreshold, uint16_t upperThreshold);
    void setProximityInterruptThreshold(uint16_t lowerThreshold, uint16_t upperThreshold);
    void setInterruptPersistance(uint8_t value);
    void setConfigZero(uint8_t value);
    void setConfigOne(uint8_t value);
    void setConfigTwo(uint8_t value);
    void setConfigThree(uint8_t value);
    void setConfigFour(uint8_t value);
    void setConfigFive(uint8_t value);


    void setProximityGestureConfigZero(uint8_t value); // 7:6 proximity pulse length, 5:0 Proximity pulse count
    void setProximityGestureConfigOne(uint8_t value); // 

    uint8_t readRevisionID(void);
    uint8_t readStatus(void);
    uint8_t readStatus2(void);
    uint8_t readStatus3(void);

    uint16_t readClear(void); //read clear light value
    uint16_t readRed(void); //read red channel
    uint16_t readGreen(void); //read green channel
    uint16_t readBlue(void); //read blue channel

    uint16_t readProximity(void); //read blue channel

    void setIConfig(uint8_t value);
    void setIConfig2(uint8_t value);
    void setISNL(uint8_t value);
    void setISOFF(uint8_t value);
    void setIPNL(uint8_t value);
    void setIPOFF(uint8_t value);
    void setIBT(uint8_t value);
    void setISLEN(uint8_t value);
    uint8_t readIstatus(void);
    void setISTART(uint8_t value);

    void setGestureConfigZero(uint8_t value);
    void setGestureConfigOne(uint8_t value);
    void setGestureConfigTwo(uint8_t value);
    void setGestureSampleTime(uint8_t value);
    void setGestureControl(uint8_t value);
    void setGestureThreshold(uint16_t entryThreshold, uint16_t exitThreshold);
    uint8_t readGestureFIFO(void);
    uint8_t readGestureStatus(void);
    void setControl(uint8_t value);
    uint8_t readAUXID(void);

    void setProximityGestureOffsets(uint16_t OFFSETN, uint16_t OFFSETS, uint16_t OFFSETW, uint16_t OFFSETE);
    uint16_t readMeasuredBaseline(void); 
    uint16_t readStoredBaseline(void); 

    void setAutozero(uint8_t value);
    bool startCalibration(void);
	void stopCalibration(void);

    void setCalibrationConfigZero(uint8_t value);
    void setCalibrationConfigTwo(uint8_t value);
    void setCalibrationConfigThree(uint8_t value);
    void setCalibrationConfigFour(uint8_t value);

    uint8_t readCalibrationStatus(void);

    void setInterruptEnable(uint8_t value);
    void setInterruptClear(uint8_t value);

    uint16_t readGestureFIFO_North(void);
    uint16_t readGestureFIFO_South(void);
    uint16_t readGestureFIFO_West(void);
    uint16_t readGestureFIFO_East(void);

  private:

    bool _reportIR = 0;
};

#endif
