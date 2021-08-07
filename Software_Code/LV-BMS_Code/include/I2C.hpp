/*
 * I2C.hpp
 *
 *  Created on: Jan 23, 2021
 *      Author: Dennis
 */

#ifndef INCLUDE_I2C_HPP_
#define INCLUDE_I2C_HPP_


#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#include "inc/hw_i2c.h"
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_gpio.h"
#include "inc/hw_types.h"
#include "inc/hw_can.h"
#include "inc/hw_adc.h"

#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/pin_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/rom_map.h"
#include "driverlib/uart.h"
#include "driverlib/timer.h"
#include "driverlib/i2c.h"
#include "driverlib/can.h"
#include "driverlib/adc.h"
#include "driverlib/ssi.h"

#include <include/GPIO.hpp>
#include <include/Timer.hpp>

/*
#define DISABLE_INT asm(" BIC #8,SR")
#define DELAY_LIMIT 0xffff
*/

// register map
#define SYS_STAT        0x00
#define CELLBAL1        0x01
#define CELLBAL2        0x02
#define CELLBAL3        0x03
#define SYS_CTRL1       0x04
#define SYS_CTRL2       0x05
#define PROTECT1        0x06
#define PROTECT2        0x07
#define PROTECT3        0x08
#define OV_TRIP         0x09
#define UV_TRIP         0x0A
#define CC_CFG          0x0B

#define VC1_HI_BYTE     0x0C
#define VC1_LO_BYTE     0x0D
#define VC2_HI_BYTE     0x0E
#define VC2_LO_BYTE     0x0F
#define VC3_HI_BYTE     0x10
#define VC3_LO_BYTE     0x11
#define VC4_HI_BYTE     0x12
#define VC4_LO_BYTE     0x13
#define VC5_HI_BYTE     0x14
#define VC5_LO_BYTE     0x15
#define VC6_HI_BYTE     0x16
#define VC6_LO_BYTE     0x17
#define VC7_HI_BYTE     0x18
#define VC7_LO_BYTE     0x19
#define VC8_HI_BYTE     0x1A
#define VC8_LO_BYTE     0x1B
#define VC9_HI_BYTE     0x1C
#define VC9_LO_BYTE     0x1D
#define VC10_HI_BYTE    0x1E
#define VC10_LO_BYTE    0x1F
#define VC11_HI_BYTE    0x20
#define VC11_LO_BYTE    0x21
#define VC12_HI_BYTE    0x22
#define VC12_LO_BYTE    0x23
#define VC13_HI_BYTE    0x24
#define VC13_LO_BYTE    0x25
#define VC14_HI_BYTE    0x26
#define VC14_LO_BYTE    0x27
#define VC15_HI_BYTE    0x28
#define VC15_LO_BYTE    0x29

#define BAT_HI_BYTE     0x2A
#define BAT_LO_BYTE     0x2B

#define TS1_HI_BYTE     0x2C
#define TS1_LO_BYTE     0x2D
#define TS2_HI_BYTE     0x2E
#define TS2_LO_BYTE     0x2F
#define TS3_HI_BYTE     0x30
#define TS3_LO_BYTE     0x31

#define CC_HI_BYTE      0x32
#define CC_LO_BYTE      0x33

#define ADCGAIN1        0x50
#define ADCOFFSET       0x51
#define ADCGAIN2        0x59

#define ADCGAIN_BASE            365

// function from TI reference design
#define LOW_BYTE(Data)          (uint8_t)(0xff & Data)
#define HIGH_BYTE(Data)         (uint8_t)(0xff & (Data >> 8))

// for bit clear operations of the SYS_STAT register
#define STAT_CC_READY           (0x80)
#define STAT_DEVICE_XREADY      (0x20)
#define STAT_OVRD_ALERT         (0x10)
#define STAT_UV                 (0x08)
#define STAT_OV                 (0x04)
#define STAT_SCD                (0x02)
#define STAT_OCD                (0x01)
#define STAT_FLAGS              (0x3F)

// maps for settings in protection registers
#define SCD_DELAY_50us      0x0
#define SCD_DELAY_100us     0x1
#define SCD_DEALY_200us     0x2
#define SCD_DELAY_400us     0x3

#define SCD_THRESH_44mV_22mV    0
#define SCD_THRESH_67mV_33mV    1
#define SCD_THRESH_89mV_44mV    2
#define SCD_THRESH_111mV_56mV   3
#define SCD_THRESH_133mV_67mV   4
#define SCD_TRHESH_155mV_68mV   5
#define SCD_THRESH_178mV_89mV   6
#define SCD_THRESH_200mV_100mV  7

#define OCD_DEALY_10ms      0x0
#define OCD_DELAY_20ms      0x1
#define OCD_DELAY_40ms      0x2
#define OCD_DELAY_80ms      0x3
#define OCD_DELAY_160ms     0x4
#define OCD_DELAY_320ms     0x5
#define OCD_DELAY_640ms     0x6
#define OCD_DELAY_1280ms    0x7

#define OCD_THRESH_17mV_8mV     0
#define OCD_THRESH_22mV_11mV    1
#define OCD_THRESH_28mV_14mV    2
#define OCD_THRESH_33mV_17mV    3
#define OCD_THRESH_39mV_19mV    4
#define OCD_THRESH_44mV_22mV    5
#define OCD_THRESH_50mV_25mV    6
#define OCD_THRESH_56mV_28MV    7
#define OCD_THRESH_61mV_31mV    8
#define OCD_THRESH_67mV_33mV    9
#define OCD_THRESH_72mV_36mV    0xA
#define OCD_THRESH_78mV_39mV    0xB
#define OCD_THRESH_83mV_42mV    0xC
#define OCD_THRESH_89mV_44mV    0xD
#define OCD_THRESH_94mV_47mV    0xE
#define OCD_THRESH_100mV_50mV   0xF

#define UV_DELAY_1s             0
#define UV_DELAY_4s             1
#define UV_DELAY_8s             2
#define UV_DELAY_16s            3

#define OV_DELAY_1s             0
#define OV_DELAY_2s             1
#define OV_DELAY_4s             2
#define OV_DELAY_8s             3

#define OV_THRESH_BASE          0x2008
#define UV_THRESH_BASE          0x1000
#define OV_STEP                 0x10
#define UV_STEP                 0x10

// define Register Group
typedef struct _Register_Group
{
    union
    {
        struct
        {
            unsigned char OCD           :1;
            unsigned char SCD           :1;
            unsigned char OV            :1;
            unsigned char UV            :1;
            unsigned char OVRD_ALERT    :1;
            unsigned char DEVICE_XREADY :1;
            unsigned char WAKE          :1;
            unsigned char CC_READY      :1;
        }StatusBit;
        unsigned char StatusByte;
    }SysStatus;

    union
    {
        struct
        {
            unsigned char RSVD          :3;
            unsigned char CB5           :1;
            unsigned char CB4           :1;
            unsigned char CB3           :1;
            unsigned char CB2           :1;
            unsigned char CB1           :1;
        }CellBal1Bit;
        unsigned char CellBal1Byte;
    }CellBal1;

    union
    {
        struct
        {
            unsigned char RSVD          :3;
            unsigned char CB10          :1;
            unsigned char CB9           :1;
            unsigned char CB8           :1;
            unsigned char CB7           :1;
            unsigned char CB6           :1;
        }CellBal2Bit;
        unsigned char CellBal2Byte;
    }CellBal2;

    union
    {
        struct
        {
            unsigned char RSVD          :3;
            unsigned char CB15          :1;
            unsigned char CB14          :1;
            unsigned char CB13          :1;
            unsigned char CB12          :1;
            unsigned char CB11          :1;
        }CellBal3Bit;
        unsigned char CellBal3Byte;
    }CellBal3;

    union
    {
        struct
        {
            unsigned char SHUT_B        :1;
            unsigned char SHUT_A        :1;
            unsigned char RSVD1         :1;
            unsigned char TEMP_SEL      :1;
            unsigned char ADC_EN        :1;
            unsigned char RSVD2         :2;
            unsigned char LOAD_PRESENT  :1;
        }SysCtrl1Bit;
        unsigned char SysCtrl1Byte;
    }SysCtrl1;

    union
    {
        struct
        {
            unsigned char CHG_ON        :1;
            unsigned char DSG_ON        :1;
            unsigned char WAKE_T        :2;
            unsigned char WAKE_EN       :1;
            unsigned char CC_ONESHOT    :1;
            unsigned char CC_EN         :1;
            unsigned char DELAY_DIS     :1;
        }SysCtrl2Bit;
        unsigned char SysCtrl2Byte;
    }SysCtrl2;

    union
    {
        struct
        {
            unsigned char SCD_THRESH    :3;
            unsigned char SCD_DELAY     :2;
            unsigned char RSVD          :2;
            unsigned char RSNS          :1;
        }Protect1Bit;
        unsigned char Protect1Byte;
    }Protect1;

    union
    {
        struct
        {
            unsigned char OCD_THRESH    :4;
            unsigned char OCD_DELAY     :3;
            unsigned char RSVD          :1;
        }Protect2Bit;
        unsigned char Protect2Byte;
    }Protect2;

    union
    {
        struct
        {
            unsigned char RSVD          :4;
            unsigned char OV_DELAY      :2;
            unsigned char UV_DELAY      :2;
        }Protect3Bit;
        unsigned char Protect3Byte;
    }Protect3;

    unsigned char OVTrip;
    unsigned char UVTrip;
    unsigned char CCCfg;            // should be set to 0x19 according to datasheet

    union
    {
        struct
        {
            unsigned char VC1_HI;
            unsigned char VC1_LO;
        }VCell1Byte;
        unsigned short VCell1Word;
    }VCell1;

    union
    {
        struct
        {
            unsigned char VC2_HI;
            unsigned char VC2_LO;
        }VCell2Byte;
        unsigned short VCell2Word;
    }VCell2;

    union
    {
        struct
        {
            unsigned char VC3_HI;
            unsigned char VC3_LO;
        }VCell3Byte;
        unsigned short VCell3Word;
    }VCell3;

    union
    {
        struct
        {
            unsigned char VC4_HI;
            unsigned char VC4_LO;
        }VCell4Byte;
        unsigned short VCell4Word;
    }VCell4;

    union
    {
        struct
        {
            unsigned char VC5_HI;
            unsigned char VC5_LO;
        }VCell5Byte;
        unsigned short VCell5Word;
    }VCell5;

    union
    {
        struct
        {
            unsigned char VC6_HI;
            unsigned char VC6_LO;
        }VCell6Byte;
        unsigned short VCell6Word;
    }VCell6;

    union
    {
        struct
        {
            unsigned char VC7_HI;
            unsigned char VC7_LO;
        }VCell7Byte;
        unsigned short VCell7Word;
    }VCell7;

    union
    {
        struct
        {
            unsigned char VC8_HI;
            unsigned char VC8_LO;
        }VCell8Byte;
        unsigned short VCell8Word;
    }VCell8;

    union
    {
        struct
        {
            unsigned char VC9_HI;
            unsigned char VC9_LO;
        }VCell9Byte;
        unsigned short VCell9Word;
    }VCell9;

    union
    {
        struct
        {
            unsigned char VC10_HI;
            unsigned char VC10_LO;
        }VCell10Byte;
        unsigned short VCell10Word;
    }VCell10;

    union
    {
        struct
        {
            unsigned char VC11_HI;
            unsigned char VC11_LO;
        }VCell11Byte;
        unsigned short VCell11Word;
    }VCell11;

    union
    {
        struct
        {
            unsigned char VC12_HI;
            unsigned char VC12_LO;
        }VCell12Byte;
        unsigned short VCell12Word;
    }VCell12;

    union
    {
        struct
        {
            unsigned char VC13_HI;
            unsigned char VC13_LO;
        }VCell13Byte;
        unsigned short VCell13Word;
    }VCell13;

    union
    {
        struct
        {
            unsigned char VC14_HI;
            unsigned char VC14_LO;
        }VCell14Byte;
        unsigned short VCell14Word;
    }VCell14;

    union
    {
        struct
        {
            unsigned char VC15_HI;
            unsigned char VC15_LO;
        }VCell15Byte;
        unsigned short VCell15Word;
    }VCell15;

    union
    {
        struct
        {
            unsigned char BAT_HI;
            unsigned char BAT_LO;
        }VBatByte;
        unsigned short VBatWord;
    }VBat;

    union
    {
        struct
        {
            unsigned char TS1_HI;
            unsigned char TS1_LO;
        }TS1Byte;
        unsigned short TS1Word;
    }TS1;

    union
    {
        struct
        {
            unsigned char TS2_HI;
            unsigned char TS2_LO;
        }TS2Byte;
        unsigned short TS2Word;
    }TS2;

    union
    {
        struct
        {
            unsigned char TS3_HI;
            unsigned char TS3_LO;
        }TS3Byte;
        unsigned short TS3Word;
    }TS3;

    union
    {
        struct
        {
            unsigned char CC_HI;
            unsigned char CC_LO;
        }CCByte;
        unsigned short CCWord;
    }CC;

    union
    {
        struct
        {
            unsigned char RSVD1         :2;
            unsigned char ADCGAIN_4_3   :2;
            unsigned char RSVD2         :4;
        }ADCGain1Bit;
        unsigned char ADCGain1Byte;
    }ADCGain1;

    unsigned char ADCOffset;

    union
    {
        struct
        {
            unsigned char RSVD          :5;
            unsigned char ADCGAIN_2_0   :3;
        }ADCGain2Bit;
        unsigned char ADCGain2Byte;
    }ADCGain2;

}RegisterGroup;


// Max Cells and Thermistors
#define MAX_NUMBER_OF_CELLS 15
#define MAX_NUMBER_OF_THERMISTORS 3

// IC type/size
#define bq76920 1
#define bq76930 2
#define bq76940 3

// output information to serial console for debugging
#define BQ769X0_DEBUG 1

// define bq slave address
#define I2C_SLAVE_ADDRESS 0x08

// define bq boot pin
#define BOOT_PERIPH         SYSCTL_PERIPH_GPIOC
#define BOOT_BASE           GPIO_PORTC_BASE
#define BOOT_PIN            GPIO_PIN_4

// define Interrupt pin
#define INTERRUPT_PERIPH    SYSCTL_PERIPH_GPIOA
#define INTERRUPT_BASE      GPIO_PORTA_BASE
#define INTERRUPT_INT_PIN   GPIO_INT_PIN_0
#define INTERRUPT_PIN       GPIO_PIN_0



class bq769x0
{
public:
    // Constructor, initialize the type of bq and store I2C Address
    bq769x0(uint8_t bqType = bq76920, int bqI2CAddress = I2C_SLAVE_ADDRESS);

    // Initialization of I2C
    int Init(RegisterGroup *registers);  // returns 0 if everything is OK

    // Hardware settings
    void setShuntResistorValue(float res_mOhm);
    void setThermistorBetaValue(int beta_K);

    // limit settings (for battery protection)
    void setTemperatureLimits(int minDischarge_degC, int maxDischarge_degC, int minCharge_degC, int maxCharge_degC);    // ¢XC
    long setShortCircuitDischargeProtection(RegisterGroup *registers, long current_mA, int delay_us = 70);
    long setOvercurrentDischargeProtection(RegisterGroup *registers, long current_mA, int delay_ms = 8);
    int setCellUndervoltageProtection(RegisterGroup *registers, int voltage_mV, int delay_s = 1);
    int setCellOvervoltageProtection(RegisterGroup *registers, int voltage_mV, int delay_s = 1);

    // balancing settings
    void setBalancingThresholds(int idleTime_min = 30, int absVoltage_mV = 3400, uint8_t voltageDifference_mV = 20);
    void setIdleCurrentThreshold(int current_mA);

    // battery status
    long  getBatteryCurrent(void);
    long  getBatteryVoltage(void);
    int  getCellVoltage(uint8_t idxCell);            // from 1 to 15
    int*  getCellVoltageBuffer(void);
    int  getMinCellVoltage(void);
    int  getMaxCellVoltage(void);
    float getTemperatureDegC(uint8_t channel = 1);   // default TS1
    float getTemperatureDegF(uint8_t channel = 1);

    // CheckStatus,  update and shutdown
    int checkStatus(RegisterGroup *registers);       // returns 0 if everything is OK
    void update(RegisterGroup *registers);
    void updateDSG(RegisterGroup *registers);
    void updateCHG(RegisterGroup *registers);
    void shutdown(void);

    // charging control
    bool enableCharging(RegisterGroup *registers);
    void disableCharging(void);
    bool enableDischarging(RegisterGroup *registers);
    void disableDischarging(void);

    // automatic balancing when battery is within balancing thresholds
    void enableAutoBalancing(void);
    void disableAutoBalancing(void);

    // interrupt handling (not to be called manually!)
    void setAlertInterruptFlag(void);

    // test
    int getADCGain(void);
    int getADCOffset(void);

    // I2C Communication
    void I2CWriteRegister(uint32_t registerAddress, uint8_t data);
    uint32_t I2CReadRegister(uint32_t registerAddress);

/*
    // update functions
    void updateVoltages(void);
    void updateCurrent(RegisterGroup *registers, bool ignoreCCReadyFlag = false);
    void updateTemperatures(void);
    void updateBalancingSwitches(RegisterGroup *registers);
*/

private:

    static bq769x0* instancePointer;

    // variables
    int I2CSlaveAddress;
    uint8_t bq_Type;
    int numberOfCells;
    int cellVoltages[MAX_NUMBER_OF_CELLS];          // mV     // better with cellVoltages[numberOfCells] ??????????????
    int numberOfTemps;
    int temperatures[MAX_NUMBER_OF_THERMISTORS];    // ¢XC/10  // temperatures[numberOfTemps] ????????????
    float shuntResistorValue_mOhm;                  // mOhm
    int thermistorBetaValue = 3435;                 // typical value for Semitec 103AT-5 thermistor  // ??????? need to check with datasheet
    int adcGain = 380;                                    // uV/LSB
    int adcOffset = 30;                                  // mV
    int idxMaxVoltageCell;                          // store the index of the max voltage cell
    int idxMinVoltageCell;                          // store the index of the min voltage cell
    uint32_t errorStatus = 0;                            // for checkStatus used

    // current battery voltage and current
    long batVoltage;                                // mV
    long batCurrent;                                // mA

    // Cell voltage limits (mV)
    int maxCellVoltage;
    int minCellVoltage;

    // Current limits (mA)
    long maxChargeCurrent;
    long maxDischargeCurrent;
    int idleCurrentThreshold = 30;                  // mA

    // Temperature limits (¢XC/10)
    int minCellTempCharge;
    int minCellTempDischarge;
    int maxCellTempCharge;
    int maxCellTempDischarge;

    // Balancing
    bool autoBalancingEnable = false;
    bool balancingActive = false;

    // Balancing Threshold
    int balancingMinCellVoltage_mV;
    int balancingMaxVoltageDifference_mV;
    int balancingMinIdleTime_s = 1800;              // default: 30 minutes

    // indicates if a new current reading or an error is available from BMS IC
    // initialize with true to check and clear errors at start-up
    bool alertInterruptFlag = true;


    // functions
    static void alertISR(void);
/*
    void I2CWriteRegister(uint32_t registerAddress, uint8_t data);
    uint32_t I2CReadRegister(uint32_t registerAddress);
*/
    // update functions
    void updateVoltages(void);
    void updateCurrent(RegisterGroup *registers, bool ignoreCCReadyFlag = false);
    void updateTemperatures(void);
    void updateBalancingSwitches(RegisterGroup *registers);


};

#endif /* INCLUDE_I2C_HPP_ */
