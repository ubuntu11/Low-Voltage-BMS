/*
 * ADC.h
 *
 *  Created on: Feb 01, 2021
 *      Author: Dennis
 */

#ifndef ADC_HPP_
#define ADC_HPP_

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

typedef struct
{
    uint32_t temperature1;
    uint32_t temperature2;
    uint32_t temperature3;
} BatteryStackTemperature;

typedef struct
{
    uint32_t DSGFetTemp;
    uint32_t CHGFetTemp;
} MOSFETTemperature;

typedef struct
{
    uint32_t cellVoltage4;
    uint32_t cellVoltage3;
    uint32_t cellVoltage2;
    uint32_t cellVoltage1;
    uint32_t cellGND;
    uint32_t PackVoltage;
} VoltageMeasurement;



class ADC
{
public:
    ADC();

    void Init(void);
    void readStackTemp(void);
    void readMosfetTemp(void);
    void readVoltage(void);      // include read the Backup voltage and Pack+

    BatteryStackTemperature getBatteryStackTemperature(void);
    MOSFETTemperature getMosfetTemperature(void);
    VoltageMeasurement getVoltageMeasurement(void);

private:

    // Variable
    BatteryStackTemperature stackTemperature;
    MOSFETTemperature mosfetTemperature;
    VoltageMeasurement voltageMeasurement;

    /*
     * function
     */

    // V_mes => ADC value, V_ref => range of ADC, VCC => power supply of the measurement circuit
    // R_div => resistance value on the circuit, R_ntc_normal => normal resistance value of the NTC
    // T_ref => Reference-Temperature in Kelvin (298,15k = 25¢XC), B => B-factor
    uint32_t convertTempIndegC(uint32_t V_mes, float V_ref, float VCC, float B, int R_div, float T_ref, float R_ntc_normal);

    // we use voltage divider to reduce the Bat+ voltage down to range(0~3V)
    // R_1 : R_last = 5 : 1 should be around so
    // Calculate the ADC value to true battery voltage
    uint32_t convertVoltage(uint32_t V_mes, float V_ref, int R_1, float R_last);


};

#endif /* ADC_HPP_ */
