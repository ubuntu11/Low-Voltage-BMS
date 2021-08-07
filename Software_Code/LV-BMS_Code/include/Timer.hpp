/*
 * Timer.h
 *
 *  Created on: Feb 01, 2021
 *      Author: Dennis
 */

#ifndef TIMER_H_
#define TIMER_H_

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

/*
class Timer
{
public:
    Timer();

    void Init(void);

private:

};
*/



void TimerBMSUpdatesInterruptBegin(void (*ISR)());
void SysTickInt(void);
void TimerDelayInterruptBegin(void);
void Delay(uint32_t time_ms);



#endif /* TIMER_H_ */
