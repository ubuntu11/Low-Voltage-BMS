/*
 * GPIO.cpp
 *
 *  Created on: Feb 01, 2021
 *      Author: Dennis
 */

#include <include/GPIO.hpp>

//--------------------------------------------------------------------------------------------------------------------------

GPIO::GPIO()
{
    // TODO Auto-generated constructor stub

}

//--------------------------------------------------------------------------------------------------------------------------

void GPIO::Init(void)
{
    //IOs, Enable all Port
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);


    // Set the Output Pin to enable
    GPIOPinTypeGPIOOutput(GPIO_PORTA_BASE, GPIO_PIN_2);   // for Pack monitor enable from FET driver
    GPIOPinTypeGPIOOutput(GPIO_PORTA_BASE, GPIO_PIN_4);   // for Charge pump enable from FET driver
    GPIOPinTypeGPIOOutput(LED_BASE, OK_LED | FAULT_LED);  // for LED


/*
    for later use (version 2), after we have designed a boot pin from uC in PCB for bq activation
    // Boot bq-chip if pin is defined (else: manual boot via push button has to be done before calling this method)
    if (bootPin >= 0)
    {
        SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIO...);
        while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOA))

        GPIOPinTypeGPIOOutput(GPIO_PORT..._BASE, GPIO_PIN_...);

        GPIOPinWrite(gpioPortBase, gpioPin, gpioPin)
        SysCtlDelay(1000000)  // wait 5 ms for device to receive boot signal (datasheet: max. 2 ms)
        GPIOPinWrite(gpioPortBase, gpioPin, 0x00)
        SysCtlDelay(1000000)  // wait for device to boot up IC completely (datasheet: max. 10 ms)

     }
*/
}

//--------------------------------------------------------------------------------------------------------------------------

void GPIO::pinEnable(uint32_t gpioPortBase, uint32_t gpioPin, bool state)
{
    if (state == false)
    {
        GPIOPinWrite(gpioPortBase, gpioPin, 0x00);
    }
    else
    {
        GPIOPinWrite(gpioPortBase, gpioPin, gpioPin);
    }
}

//--------------------------------------------------------------------------------------------------------------------------

// Pin for our charge enable Pin => GPIO_PORTA_BASE, GPIO_PIN_4
void GPIO::chargePumpEnable(void)
{
    // Enbable Charge pump from FET driver (PA4)
    pinEnable(GPIO_PORTA_BASE, GPIO_PIN_4, true);
    Delay(100);  // delay 100ms so that charge pump could ramp up

}

//--------------------------------------------------------------------------------------------------------------------------

// Pin for our pack enable Pin => GPIO_PORTA_BASE, GPIO_PIN_2
void GPIO::packMonitorEnable(void)
{
    // Enable Pack + monitor from FET driver (PA2)
    pinEnable(GPIO_PORTA_BASE, GPIO_PIN_2, true);
}

//--------------------------------------------------------------------------------------------------------------------------

void GPIO::LEDBlink(uint32_t gpioPortBase, uint32_t gpioPin)
{
    int i = 0;
    while(i < 5)
    {
        GPIOPinWrite(gpioPortBase, gpioPin, gpioPin);
        SysCtlDelay(10000);
        GPIOPinWrite(gpioPortBase, gpioPin, 0x00);
        SysCtlDelay(10000);
        i++;
    }
}
//--------------------------------------------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------------------------------------------

