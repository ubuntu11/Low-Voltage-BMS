/*
 * Timer.cpp
 *
 *  Created on: Feb 01, 2021
 *      Author: Dennis
 */

#include <include/Timer.hpp>

//--------------------------------------------------------------------------------------------------------------------------

/*
void Init(void)
{
    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
    SysCtlDelay(3);

    TimerConfigure(TIMER0_BASE, TIMER_CFG_SPLIT_PAIR | TIMER_CFG_B_PERIODIC);
    TimerLoadSet(TIMER0_BASE, TIMER_B, SysCtlClockGet()/1000 );

    // Configure the Timer0B interrupt for timer timeout.
    IntEnable(INT_TIMER0B);

    TimerIntEnable(TIMER0_BASE, TIMER_TIMB_TIMEOUT);
    IntMasterEnable();  // Enable interrupts to the processor.

    TimerEnable(TIMER0_BASE, TIMER_B);
}
*/

//--------------------------------------------------------------------------------------------------------------------------

/*
 *  this is a function block for BMS update measurement per 250ms
 */

void TimerBMSUpdatesInterruptBegin(void (*ISR)())
{
    uint32_t Period;
    Period = 20000000; // 250ms (Because system clock frequency is at 80MHz)

    // make sure to disable the timer Peripheral before you enable it
    SysCtlPeripheralDisable(SYSCTL_PERIPH_TIMER0);
    SysCtlPeripheralReset(SYSCTL_PERIPH_TIMER0);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
    SysCtlDelay(3);

    // ensure timer is disabled before configuring
    TimerDisable(TIMER0_BASE, TIMER_A);

    // configure Full-width periodic timer (32 bits)
    TimerConfigure(TIMER0_BASE, TIMER_CFG_PERIODIC);
    // Only TIMER_A should be used when the timer is configured for full-width operation.
    // set the time load that should be count down, that is (Period - 1)
    TimerLoadSet(TIMER0_BASE, TIMER_A, Period - 1);

    // set the function that should be triggered when interrupt occurs
    TimerIntRegister(TIMER0_BASE, TIMER_A, ISR);

    // Enable the timeout interrupt. In count down mode it's when the timer reaches 0 and resets back to load.
    TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
    IntMasterEnable();  // Enable interrupts to the processor.
    TimerEnable(TIMER0_BASE, TIMER_A);
}

//--------------------------------------------------------------------------------------------------------------------------

/*
 * this is a function block of precise miliseconds delays
 */

volatile uint32_t millis = 0;

// Interrupt handler for the timer
void SysTickInt(void)
{
  uint32_t status=0;

  status = TimerIntStatus(TIMER5_BASE, true);
  TimerIntClear(TIMER5_BASE, status);
  millis++;
}

//  Timer setup
void TimerDelayInterruptBegin(void)
{
    // We set the load value so the timer interrupts each 1ms
    uint32_t Period;
    Period = 80000; // 1ms (Because system clock frequency is at 80MHz)

    SysCtlPeripheralDisable(SYSCTL_PERIPH_TIMER5);
    SysCtlPeripheralReset(SYSCTL_PERIPH_TIMER5);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER5);
    SysCtlDelay(3);

    // ensure timer is disabled before configuring
    TimerDisable(TIMER5_BASE, TIMER_A);
    /*
    Configure the timer as periodic, by omission it's in count down mode.
    It counts from the load value to 0 and then resets back to the load value.
    REMEMBER: You need to configure the timer before setting the load and match
    */
    TimerConfigure(TIMER5_BASE, TIMER_CFG_PERIODIC);
    TimerLoadSet(TIMER5_BASE, TIMER_A, Period -1);

    // set the function that should be triggered when interrupt occurs
    TimerIntRegister(TIMER5_BASE, TIMER_A, SysTickInt);

    /*
    Enable the timeout interrupt. In count down mode it's when the timer reaches
    0 and resets back to load. In count up mode it's when the timer reaches load
    and resets back to 0.
    */
    TimerIntEnable(TIMER5_BASE, TIMER_TIMA_TIMEOUT);

    IntMasterEnable();  // Enable interrupts to the processor.

    TimerEnable(TIMER5_BASE, TIMER_A);
}


//  This is the delay function.
void Delay(uint32_t time_ms)
{
    volatile uint32_t temp = millis;
    while ((millis - temp) < time_ms){};
}

//--------------------------------------------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------------------------------------------

