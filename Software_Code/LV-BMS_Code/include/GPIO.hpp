/*
 * GPIO.h
 *
 *  Created on: Feb 01, 2021
 *      Author: Dennis
 */

#ifndef GPIO_H_
#define GPIO_H_

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

#include <include/Timer.hpp>

#define LED_PORT            SYSCTL_PERIPH_GPIOB
#define LED_BASE            GPIO_PORTB_BASE
#define FAULT_LED           GPIO_PIN_0
#define OK_LED              GPIO_PIN_1



class GPIO
{
public:
    GPIO();

    void Init(void);
    void pinEnable(uint32_t gpioPortBase, uint32_t gpioPin, bool state);
    void chargePumpEnable(void);
    void packMonitorEnable(void);
    void LEDBlink(uint32_t gpioPortBase, uint32_t gpioPin);

};

#endif /* GPIO_H_ */
