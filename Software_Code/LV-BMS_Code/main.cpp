
/**
 * main.c
 */

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
#include "driverlib/fpu.h"

#include <include/I2C.hpp>
#include <include/Defines.hpp>
#include <include/ADC.hpp>
#include <include/Timer.hpp>
#include <include/GPIO.hpp>
#include <include/CANDriver.hpp>
#include <include/TestCAN.hpp>



/*
 * define global variable
 */
RegisterGroup Registers;
bq769x0 BMS(bq76920, I2C_SLAVE_ADDRESS);  // BMS object

// -----------------------------------------------------------------------------------------------------------------------------
/*
 * Option 1: Just turn both CHG and DSG on by using the update function in I2C class
 */

// define global BMS update function for TimerBMSUpdatesInterruptBegin() function
void GlobalBMSUpdate(void)
{
    BMS.update(&Registers);

    // Visualization of the run of update
    GPIOPinWrite(LED_BASE, OK_LED, OK_LED);
    SysCtlDelay(5000000); // Delay(100);
    GPIOPinWrite(LED_BASE, OK_LED, 0x00);
    SysCtlDelay(5000000); // Delay(100);
}

// -----------------------------------------------------------------------------------------------------------------------------
/*
 * Option 2: Depending on which mode we are in by comparing the Bat+ and Pack+, turn on the corresponding CHG or DSG funktion
 */

// define global BMS charge update function for TimerBMSUpdatesInterruptBegin() function
void GlobalBMSUpdateCHG(void)
{
    BMS.updateCHG(&Registers);

    // Visualization of the run of update
    GPIOPinWrite(LED_BASE, OK_LED, OK_LED);
    SysCtlDelay(5000000); // Delay(100);
    GPIOPinWrite(LED_BASE, OK_LED, 0x00);
    SysCtlDelay(5000000); // Delay(100);
}

// define global BMS discharge update function for TimerBMSUpdatesInterruptBegin() function
void GlobalBMSUpdateDSG(void)
{
    BMS.updateDSG(&Registers);

    // Visualization of the run of update
    GPIOPinWrite(LED_BASE, OK_LED, OK_LED);
    SysCtlDelay(5000000); // Delay(100);
    GPIOPinWrite(LED_BASE, OK_LED, 0x00);
    SysCtlDelay(5000000); // Delay(100);
}

// -----------------------------------------------------------------------------------------------------------------------------

int main(void)
{
    // FPUEnable();
    // HWREG(0xE000ED88) |= (1<<20) | (1<<21) | (1<<22) | (1<<23);

    // Set up system clock
    SysCtlClockSet(SYSCTL_SYSDIV_2_5 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ); //80MHz


    // Initialization of uC Systems (GPIO, Timer, ADC, CAN)
    GPIO PinControl;
    PinControl.Init();

    ADC AnalogMeasurement;
    AnalogMeasurement.Init();

    TimerDelayInterruptBegin();

    // Initialization of CAN
    CAN BmsCanDriver;
    BmsCanDriver.init(PF0,PF3,1000000);
    unsigned int CANTIMERVALUE=0;
    unsigned int CANTIMERSTATUS=0;

    // enable the charge pump in FET Driver
    PinControl.chargePumpEnable();
    // enable the pack monitor in FET Driver
    PinControl.packMonitorEnable();

    // Visualization that BMS starts
    PinControl.pinEnable(LED_BASE, OK_LED, true);
    Delay(1000);  // delay 1s
    PinControl.pinEnable(LED_BASE, OK_LED, false);
    PinControl.pinEnable(LED_BASE, FAULT_LED, true);
    Delay(1000);  // delay 1s
    PinControl.pinEnable(LED_BASE, FAULT_LED, false);

    // Initialization of I2C (bq-chip)
    int error = BMS.Init(&Registers);

    // BMS system set up
    BMS.setShuntResistorValue(3);  // Shunt value 3mOhm
    BMS.setThermistorBetaValue(3428);  // B = 3428
    BMS.setTemperatureLimits(MIN_AFE_TEMPERATURE_DSG, MAX_AFE_TEMPERATURE_DSG, MIN_AFE_TEMPERATURE_CHG, MAX_AFE_TEMPERATURE_CHG);  // in ¢XC*10
    long shortcircuit_threshold_mA = BMS.setShortCircuitDischargeProtection(&Registers, SHORTCIRCUIT_SHUTDOWN, SHORTCIRCUIT_DELAY);
    long overcurrent_threshold_mA = BMS.setOvercurrentDischargeProtection(&Registers, OVERCURRENT_SHUTDOWN, OVERCURRENT_DELAY);
    int undervolt_threshold = BMS.setCellUndervoltageProtection(&Registers, UNDERVOLTAGE_SHUTDOWN, UNDERVOLTAGE_DELAY);
    int overvolt_threshold = BMS.setCellOvervoltageProtection(&Registers, OVERVOLTAGE_SHUTDOWN, OVERVOLTAGE_DELAY);
    SysCtlDelay(100000);
    // Set Balancing
    //  TODO ...

// -----------------------------------------------------------------------------------------------------------------------------
    /*
     * Option 1: Just turn both CHG and DSG on by using the update function in I2C class
     */
    TimerBMSUpdatesInterruptBegin(GlobalBMSUpdate);

// -----------------------------------------------------------------------------------------------------------------------------
    /*
     * Option 2: Depending on which mode we are in by comparing the Bat+ and Pack+, turn on the corresponding CHG or DSG funktion
     */

//    // Decide whether we are in Charge or Discharge mode by comparing the Bat+ and Pack+
//    if (batPlus > packPlus)
//    {
//        TimerBMSUpdatesInterruptBegin(GlobalBMSUpdateDSG);
//    }
//    else
//    {
//        TimerBMSUpdatesInterruptBegin(GlobalBMSUpdateCHG);
//    }

// -----------------------------------------------------------------------------------------------------------------------------

    // get the pack+ and bat+ voltage
    AnalogMeasurement.readVoltage();
    VoltageMeasurement voltage = AnalogMeasurement.getVoltageMeasurement();
    uint32_t packPlus = voltage.PackVoltage - voltage.cellGND;
    uint32_t batPlus = voltage.cellVoltage4 - voltage.cellGND;

    while(true)
    {
        // for testing and debugging use
        // BMS.update(&Registers);
        // Delay(200);  // delay 200ms (should be around 250 ms)


        // get the battery temperature
        AnalogMeasurement.readStackTemp();
        BatteryStackTemperature stackTemp = AnalogMeasurement.getBatteryStackTemperature();
        uint32_t temperature1 = stackTemp.temperature1;
        uint32_t temperature2 = stackTemp.temperature2;
        uint32_t temperature3 = stackTemp.temperature3;

        // get the MOSFETs temperature
        AnalogMeasurement.readMosfetTemp();
        MOSFETTemperature mosfetTemp = AnalogMeasurement.getMosfetTemperature();
        uint32_t DSGFetTemp = mosfetTemp.DSGFetTemp;
        uint32_t CHGFetTemp = mosfetTemp.CHGFetTemp;

        if (temperature1 > MAX_CELL_TMEPERATURE &&
            temperature2 > MAX_CELL_TMEPERATURE &&
            temperature3 > MAX_CELL_TMEPERATURE &&
            DSGFetTemp > MAX_FET_TEMPERATURE &&
            CHGFetTemp > MAX_FET_TEMPERATURE)

        {
            BMS.shutdown();
        }


        /*
         * CAN send out Message
         */

        // calculate the temperature back into celsius and convert into 8 bit integer
        uint8_t CanTemperatureBuffer[5];
        CanTemperatureBuffer[0] = (uint8_t) temperature1;
        CanTemperatureBuffer[1] = (uint8_t) temperature2;
        CanTemperatureBuffer[2] = (uint8_t) temperature3;
        CanTemperatureBuffer[3] = (uint8_t) DSGFetTemp;
        CanTemperatureBuffer[4] = (uint8_t) CHGFetTemp;
        // Send temperature
        BmsCanDriver.sendTemperature(CanTemperatureBuffer);



        int cell1 = BMS.getCellVoltage(1) / 100;
        int cell2 = BMS.getCellVoltage(2) / 100;
        int cell3 = BMS.getCellVoltage(3) / 100;
        int cell4 = BMS.getCellVoltage(4) / 100;

        // get the cell voltages and convert into 8 bit integer
        unsigned int CanVoltageBuffer[6] = {0};
        CanVoltageBuffer[0] = BMS.getCellVoltage(1) * 10;    // cell 1
        CanVoltageBuffer[1] = BMS.getCellVoltage(2) * 10;    // cell 2
        CanVoltageBuffer[2] = BMS.getCellVoltage(3) * 10;    // cell 3
        CanVoltageBuffer[3] = BMS.getCellVoltage(5) * 10;    // cell 4
        CanVoltageBuffer[4] = packPlus / 10;                 // pack+
        // Send Cell Voltage
        BmsCanDriver.sendVoltages(CanVoltageBuffer);

/*
        // In case we need backup voltage
        AnalogMeasurement.readVoltage();
        VoltageMeasurement voltage = AnalogMeasurement.getVoltageMeasurement();
        uint32_t packPlus = voltage.PackVoltage - voltage.cellGND;
        uint32_t cell1 = voltage.cellVoltage1 - voltage.cellGND;
        uint32_t cell2 = voltage.cellVoltage2 - voltage.cellVoltage1;
        uint32_t cell3 = voltage.cellVoltage3 - voltage.cellVoltage2;
        uint32_t cell4 = voltage.cellVoltage4 - voltage.cellVoltage3;
        uint32_t gnd = voltage.cellGND;
        SysCtlDelay(100000);
*/


    }
}
