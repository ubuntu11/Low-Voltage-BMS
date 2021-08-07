/*
 *
 * I2C.cpp
 *
 *  Created on: Jan 23, 2021
 *      Author: Dennis
 */

#include <include/I2C.hpp>

//--------------------------------------------------------------------------------------------------------------------------

// declare the instance
// for the ISR to know the bq769x0 instance
bq769x0* bq769x0::instancePointer = nullptr;  // 0 nullptr

//--------------------------------------------------------------------------------------------------------------------------

bq769x0::bq769x0(uint8_t bqType, int bqI2CAddress)  // in our case 21E, it should be "bqType = bq76920, bqI2CAddress = 0x08"
{
    // Auto-generated constructor stub
    bq_Type = bqType;
    I2CSlaveAddress = bqI2CAddress;

    if (bq_Type == bq76920)
    {
        numberOfCells = 5;
        numberOfTemps = 1;
    }
    else if (bq_Type == bq76930)
    {
        numberOfCells = 10;
        numberOfTemps = 2;
    }
    else  // bq_Type == bq76940
    {
        numberOfCells = 15;
        numberOfTemps = 3;
    }

    // prevent errors if someone reduced MAX_NUMBER_OF_CELLS accidentally
    if (numberOfCells > MAX_NUMBER_OF_CELLS)
    {
        numberOfCells = MAX_NUMBER_OF_CELLS;
    }

    // prevent errors if someone reduced MAX_NUMBER_OF_THERMISTORS accidentally
    if(numberOfTemps > MAX_NUMBER_OF_THERMISTORS)
    {
        numberOfTemps = MAX_NUMBER_OF_THERMISTORS;
    }
}

//--------------------------------------------------------------------------------------------------------------------------

int bq769x0::Init(RegisterGroup *registers)  // We use PA6(I2C1SCL), PA7(I2C1SDA) for our I2C
{
    // 1. Enable the I2C clock using the RCGCI2C register in the System Control module (see page 348).
    HWREG(0x400FE000 + 0x620) |= 0b0010;  // I2C module 0-3, we use I2C module 1

    // 2. Enable the clock to the appropriate GPIO module via the RCGCGPIO register in the System
    // Control module (see page 340). To find out which GPIO port to enable, refer to Table 23-5 on page 1351.
    HWREG(0x400FE000 + 0x608) |= 0b000001;  // GPIO Port A-F, for I2C is Port A

    // 3. In the GPIO module, enable the appropriate pins for their alternate function using the
    // GPIOAFSEL register (see page 671). To determine which GPIOs to configure, see Table 23-4 on page 1344.
    // Pin 0-7, for I2C are PA6, PA7 the right one
    HWREG(0x40004000 + 0x420) |= 0b11000000;  // set to be controlled by the alternate hardware function.
    HWREG(0x40004000 + 0x51C) |= 0b11000000;  // digital enable register

    // 4. Enable the I2CSDA (PA7 in our case) pin for open-drain operation. See page 676.
    HWREG(0x40004000 + 0x50C) |= 0b10000000;  // configured PA7 as open drain.

    // 5. Configure the PMCn fields in the GPIOPCTL register to assign the I2C signals to the
    // appropriate pins. See page 688 and Table 23-5 on page 1351.
    HWREG(0x40004000 + 0x52C) |= 0x33000000;  // put a 3 in PMC7 and PMC6 (pins PA7 and PA6)


    // 6. Initialize the I2C Master by writing the I2CMCR register with a value of 0x0000.0010.
    HWREG(0x40021000 + 0x020) |= 0x00000010;  // enable master model  // 0x00000020 enable slave mode

    // 7. Set the desired SCL clock speed of 100 Kbps by writing the I2CMTPR register with the correct
    // value. The value written to the I2CMTPR register represents the number of system clock periods
    // in one SCL clock period. The TPR value is determined by the following equation:
    //   TPR = (System Clock/(2*(SCL_LP + SCL_HP)*SCL_CLK))-1 => TPR = (80MHz or SysCtlClockGet()/(2*(6+4)*100000))-1
    HWREG(0x40021000 + 0x00C) |= 0x00000027;               // => TPR = 39 (0x27 in hexadecimal)

/*
 * by using driverlib function to realize the I2C Initialization
 * However, somehow it won't work. That's why we use register as above
 *
    // Enable I2C1 peripheral
    SysCtlPeripheralEnable(SYSCTL_PERIPH_I2C1);

    // Wait for the I2C1 module to be ready
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_I2C1));

    // Reset module
    //SysCtlPeripheralReset(SYSCTL_PERIPH_I2C1);

    // Enable GPIO peripheral that contains I2C1 (in our case: GPIOA)
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);

    // Wait for the GPIOA module to be ready.
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOA));

    // Enable the appropriate pins for their alternate function using the GPIOAFSEL register
    GPIOPinConfigure(GPIO_PA6_I2C1SCL);
    GPIOPinConfigure(GPIO_PA7_I2C1SDA);
    GPIOPinTypeI2CSCL(GPIO_PORTA_BASE, GPIO_PIN_6);  // configuration for the SCL pin
    GPIOPinTypeI2C(GPIO_PORTA_BASE, GPIO_PIN_7);     // configuration for the SDA pin

    // Enable the I2CSDA (PA7 in our case) pin for open-drain operation.
    GPIOPinTypeGPIOOutputOD(GPIO_PORTA_BASE, GPIO_PIN_7);

    // Initializes the I2C Master block
    // Use the system clock for the I2C1 module
    // If The last parameter "true" -> transfer data at 400 Kbps |  "false" -> transfer data at 100 Kbps
    I2CMasterInitExpClk(I2C1_BASE, SysCtlClockGet(), false);

    SysCtlDelay(1000);
*/

    // Initialize cell voltage variables
    for (int i = 0; i < MAX_NUMBER_OF_CELLS; i++)
    {
        cellVoltages[i] = 0;
    }

    // Initialize Temp variables
    for (int i = 0; i < MAX_NUMBER_OF_THERMISTORS; i++)
    {
        temperatures[i] = 0;
    }

    //--------------------------------------------------------------------------------------------------------------------------

    // Boot the bq-chip if pin is defined (else: manual boot via push button has to be done before calling this method)
    GPIOPinTypeGPIOOutput(BOOT_BASE, BOOT_PIN);

    GPIOPinWrite(BOOT_BASE, BOOT_PIN, BOOT_PIN);
    Delay(2);  // wait 2 ms for device to receive boot signal (datasheet: max. 2 ms)
    GPIOPinWrite(BOOT_BASE, BOOT_PIN, 0x00);
    Delay(10);  // wait for device to boot up IC completely (datasheet: max. 10 ms)

    //--------------------------------------------------------------------------------------------------------------------------

    // Test communication
    I2CWriteRegister(CC_CFG, 0x19);  // should be set to 0x19 according to datasheet
    // store the value into our register's group
    registers->CCCfg = I2CReadRegister(CC_CFG);

    // Check whether the data correctly store in slave register
    if (I2CReadRegister(CC_CFG) == 0x19)
    {
        // LED Blink check
        for(int i = 0; i < 3; i++)
        {
            GPIOPinWrite(LED_BASE, OK_LED, OK_LED);
            SysCtlDelay(1000000);
            GPIOPinWrite(LED_BASE, OK_LED, 0x00);
            SysCtlDelay(1000000);
        }

        // Initial settings for bq769x0
        I2CWriteRegister(SYS_CTRL1, 0b00011000);  // Enable voltage and temperature ADC readings
        registers->SysCtrl1.SysCtrl1Byte = I2CReadRegister(SYS_CTRL1);  // Check the data

        I2CWriteRegister(SYS_CTRL2, 0b01000000);  // switch CC_EN on
        registers->SysCtrl2.SysCtrl2Byte = I2CReadRegister(SYS_CTRL2);  // Check the data

        //--------------------------------------------------------------------------------------------------------------------------

        // Build a digital input interrupt (GPIO interrupt handler) and Attach ALERT interrupt to this instance
        instancePointer = this;

        // Set the alert pin as GPIO input, it read the alert signal which be sent from BQ alert Pin
        // if alert signal is high, it means that there is a error or fault in BQ detected
        GPIOPinTypeGPIOInput(INTERRUPT_BASE, INTERRUPT_PIN);
        // Configure the alert pin, in our case PA0
        GPIOPadConfigSet(INTERRUPT_BASE, INTERRUPT_PIN, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);
        // Define in what case the GPIO interrupt should be triggered, in our case when alert pin is driven high by BQ, so GPIO_RISING_EDGE
        GPIOIntTypeSet(INTERRUPT_BASE, INTERRUPT_PIN, GPIO_RISING_EDGE);  // other options to choose: GPIO_HIGH_LEVEL, GPIO_RISING_EDGE, GPIO_BOTH_EDGES
        // Registers an interrupt handler (function => "alertISR()") that should be toggled when GPIO interrupt is triggered
        GPIOIntRegister(INTERRUPT_BASE, alertISR);  // no difference between bq769x0::alertISR and alerISR at this point
        // Enable the GPIO interrupt
        GPIOIntEnable(INTERRUPT_BASE, INTERRUPT_INT_PIN);

        //--------------------------------------------------------------------------------------------------------------------------

        // Get ADC offset and gain
        adcOffset = (signed int) I2CReadRegister(ADCOFFSET);  // mV, convert from 2's complement
        adcGain = 365 + (((I2CReadRegister(ADCGAIN1) & 0b00001100) << 1) | ((I2CReadRegister(ADCGAIN2) & 0b11100000) >> 5)); // uV/LSB
        // store the value
        registers->ADCOffset = adcOffset;
        registers->ADCGain1.ADCGain1Byte = adcGain;

        return 0;
    }
    else
    {
        // return 1 means => "BMS communication error"
        return 1;
    }
}

//--------------------------------------------------------------------------------------------------------------------------

// the actual ISR, called by static function alertISR()
void bq769x0::setAlertInterruptFlag(void)
{
    alertInterruptFlag = true;
//    instancePointer->alertInterruptFlag = true;
}

//--------------------------------------------------------------------------------------------------------------------------

// The bq769x0 drives the ALERT pin high if the SYS_STAT register contains a new value
// (either new CC reading or an error)
void bq769x0::alertISR(void)
{
    uint32_t status=0;

    status = GPIOIntStatus(INTERRUPT_BASE, true);
    GPIOIntClear(INTERRUPT_BASE, status);

    // make sure there is a Pin 0 interrupt
    if(status & INTERRUPT_INT_PIN == INTERRUPT_INT_PIN)
    {
        // LED Blink check
        for(int i = 0; i < 5; i++)
        {
            GPIOPinWrite(LED_BASE, FAULT_LED, FAULT_LED);
            SysCtlDelay(1000000);
            GPIOPinWrite(LED_BASE, FAULT_LED, 0x00);
            SysCtlDelay(1000000);
        }

        // Yes, there was a Pin 0 interrupt

        // read the Pin input
        uint8_t value = GPIOPinRead(INTERRUPT_BASE, INTERRUPT_PIN);

        if (instancePointer != 0)
        {
            instancePointer->setAlertInterruptFlag();
        }
    }
}

//--------------------------------------------------------------------------------------------------------------------------

void bq769x0::setShuntResistorValue(float res_mOhm)
{
    shuntResistorValue_mOhm = res_mOhm;
}

//--------------------------------------------------------------------------------------------------------------------------

void bq769x0::setThermistorBetaValue(int beta_K)
{
    thermistorBetaValue = beta_K;
}

//--------------------------------------------------------------------------------------------------------------------------

void bq769x0::setTemperatureLimits(int minDischarge_degC, int maxDischarge_degC, int minCharge_degC, int maxCharge_degC)
{
    // Temperature limits (¢XC*10)
    minCellTempDischarge = minDischarge_degC;
    maxCellTempDischarge = maxDischarge_degC;
    minCellTempCharge = minCharge_degC;
    maxCellTempCharge = maxCharge_degC;
}

//--------------------------------------------------------------------------------------------------------------------------

long bq769x0::setShortCircuitDischargeProtection(RegisterGroup *registers, long current_mA, int delay_us)
{
    // RSNS = 0 => OCD and SCD thresholds at lower input range
    // RSNS = 1 => OCD and SCD thresholds at upper input range
    // Here will only RSNS = 1 considered, which means the thresholds are {44, 67, 89, 111, 133, 155, 178, 200}
    registers->Protect1.Protect1Bit.RSNS = 1;

    registers->Protect1.Protect1Bit.SCD_THRESH = 0;

    // Calculate the Voltage drop across the shunt
    long SRP_SRN_Voltage_mV = current_mA * shuntResistorValue_mOhm / 1000;  // in mV

    // Update the register of the SCD thresh in accordance with the Voltage
    if (SRP_SRN_Voltage_mV >= 200)
    {
        registers->Protect1.Protect1Bit.SCD_THRESH = SCD_THRESH_200mV_100mV;
    }
    else if (SRP_SRN_Voltage_mV >= 178)
    {
        registers->Protect1.Protect1Bit.SCD_THRESH = SCD_THRESH_178mV_89mV;
    }
    else if (SRP_SRN_Voltage_mV >= 155)
    {
        registers->Protect1.Protect1Bit.SCD_THRESH = SCD_TRHESH_155mV_68mV;
    }
    else if (SRP_SRN_Voltage_mV >= 133)
    {
        registers->Protect1.Protect1Bit.SCD_THRESH = SCD_THRESH_133mV_67mV;
    }
    else if (SRP_SRN_Voltage_mV >= 111)
    {
        registers->Protect1.Protect1Bit.SCD_THRESH = SCD_THRESH_111mV_56mV;
    }
    else if (SRP_SRN_Voltage_mV >= 89)
    {
        registers->Protect1.Protect1Bit.SCD_THRESH = SCD_THRESH_89mV_44mV;
    }
    else if (SRP_SRN_Voltage_mV >= 67)
    {
        registers->Protect1.Protect1Bit.SCD_THRESH = SCD_THRESH_67mV_33mV;
    }
    else if (SRP_SRN_Voltage_mV >= 44)
    {
        registers->Protect1.Protect1Bit.SCD_THRESH = SCD_THRESH_44mV_22mV;
    }


    registers->Protect1.Protect1Bit.SCD_DELAY = 0;

    // Update the register of the SCD delay in accordance with the input delay value
    if (delay_us >= 400)
    {
        registers->Protect1.Protect1Bit.SCD_DELAY = SCD_DELAY_400us;
    }
    else if (delay_us >= 200)
    {
        registers->Protect1.Protect1Bit.SCD_DELAY = SCD_DEALY_200us;
    }
    else if (delay_us >= 100)
    {
        registers->Protect1.Protect1Bit.SCD_DELAY = SCD_DELAY_100us;
    }
    else if (delay_us >= 50)
    {
        registers->Protect1.Protect1Bit.SCD_DELAY = SCD_DELAY_50us;
    }

    I2CWriteRegister(PROTECT1, registers->Protect1.Protect1Byte);

    // returns the actual current (in mA) threshold value
    return (long) registers->Protect1.Protect1Bit.SCD_THRESH * 1000 / shuntResistorValue_mOhm;
}

//--------------------------------------------------------------------------------------------------------------------------

long bq769x0::setOvercurrentDischargeProtection(RegisterGroup *registers, long current_mA, int delay_ms)
{
    // Remark: RSNS must be set to 1 in PROTECT1 register
    registers->Protect1.Protect1Bit.RSNS = 1;

    registers->Protect2.Protect2Bit.OCD_THRESH = 0;

    // Calculate the Voltage drop across shunt
    long SRP_SRN_Voltage = current_mA * shuntResistorValue_mOhm / 1000;  // in mV

    // Update the register of the OCD thresh in accordance with the Voltage
    if (SRP_SRN_Voltage >= 100)
    {
        registers->Protect2.Protect2Bit.OCD_THRESH = OCD_THRESH_100mV_50mV;
    }
    else if (SRP_SRN_Voltage >= 94)
    {
        registers->Protect2.Protect2Bit.OCD_THRESH = OCD_THRESH_94mV_47mV;
    }
    else if (SRP_SRN_Voltage >= 89)
    {
        registers->Protect2.Protect2Bit.OCD_THRESH = OCD_THRESH_89mV_44mV;
    }
    else if (SRP_SRN_Voltage >= 83)
    {
        registers->Protect2.Protect2Bit.OCD_THRESH = OCD_THRESH_83mV_42mV;
    }
    else if (SRP_SRN_Voltage >= 78)
    {
        registers->Protect2.Protect2Bit.OCD_THRESH = OCD_THRESH_78mV_39mV;
    }
    else if (SRP_SRN_Voltage >= 72)
    {
        registers->Protect2.Protect2Bit.OCD_THRESH = OCD_THRESH_72mV_36mV;
    }
    else if (SRP_SRN_Voltage >= 67)
    {
        registers->Protect2.Protect2Bit.OCD_THRESH = OCD_THRESH_67mV_33mV;
    }
    else if (SRP_SRN_Voltage >= 61)
    {
        registers->Protect2.Protect2Bit.OCD_THRESH = OCD_THRESH_61mV_31mV;
    }
    else if (SRP_SRN_Voltage >= 56)
    {
        registers->Protect2.Protect2Bit.OCD_THRESH = OCD_THRESH_56mV_28MV;
    }
    else if (SRP_SRN_Voltage >= 50)
    {
        registers->Protect2.Protect2Bit.OCD_THRESH = OCD_THRESH_50mV_25mV;
    }
    else if (SRP_SRN_Voltage >= 44)
    {
        registers->Protect2.Protect2Bit.OCD_THRESH = OCD_THRESH_44mV_22mV;
    }
    else if (SRP_SRN_Voltage >= 39)
    {
        registers->Protect2.Protect2Bit.OCD_THRESH = OCD_THRESH_39mV_19mV;
    }
    else if (SRP_SRN_Voltage >= 33)
    {
        registers->Protect2.Protect2Bit.OCD_THRESH = OCD_THRESH_33mV_17mV;
    }
    else if (SRP_SRN_Voltage >= 28)
    {
        registers->Protect2.Protect2Bit.OCD_THRESH = OCD_THRESH_28mV_14mV;
    }
    else if (SRP_SRN_Voltage >= 22)
    {
        registers->Protect2.Protect2Bit.OCD_THRESH = OCD_THRESH_22mV_11mV;
    }
    else if (SRP_SRN_Voltage >= 17)
    {
        registers->Protect2.Protect2Bit.OCD_THRESH = OCD_THRESH_17mV_8mV;
    }


    registers->Protect2.Protect2Bit.OCD_DELAY = 0;

    // Update the register of the OCD delay in accordance with the input delay value
    if (delay_ms >= 1280)
    {
        registers->Protect2.Protect2Bit.OCD_DELAY = OCD_DELAY_1280ms;
    }
    else if (delay_ms >= 640)
    {
        registers->Protect2.Protect2Bit.OCD_DELAY = OCD_DELAY_640ms;
    }
    else if (delay_ms >= 320)
    {
        registers->Protect2.Protect2Bit.OCD_DELAY = OCD_DELAY_320ms;
    }
    else if (delay_ms >= 160)
    {
        registers->Protect2.Protect2Bit.OCD_DELAY = OCD_DELAY_160ms;
    }
    else if (delay_ms >= 80)
    {
        registers->Protect2.Protect2Bit.OCD_DELAY = OCD_DELAY_80ms;
    }
    else if (delay_ms >= 40)
    {
        registers->Protect2.Protect2Bit.OCD_DELAY = OCD_DELAY_40ms;
    }
    else if (delay_ms >= 20)
    {
        registers->Protect2.Protect2Bit.OCD_DELAY = OCD_DELAY_20ms;
    }
    else if (delay_ms >= 10)
    {
        registers->Protect2.Protect2Bit.OCD_DELAY = OCD_DEALY_10ms;
    }

    I2CWriteRegister(PROTECT2, registers->Protect2.Protect2Byte);

    // returns the actual current (in mA) threshold value
    return (long) registers->Protect2.Protect2Bit.OCD_THRESH * 1000 / shuntResistorValue_mOhm;  // mA
}

//--------------------------------------------------------------------------------------------------------------------------

int bq769x0::setCellUndervoltageProtection(RegisterGroup *registers, int voltage_mV, int delay_s)
{
    int uv_trip = 0;
    minCellVoltage = voltage_mV;

    registers->Protect3.Protect3Byte = I2CReadRegister(PROTECT3);

    uv_trip = ((((long)voltage_mV - adcOffset) * 1000 / adcGain) >> 4) & 0x00FF;
    uv_trip += 1;   // always round up for lower cell voltage

    // store the UV threshold in the register
    I2CWriteRegister(UV_TRIP, uv_trip);
    registers->UVTrip = I2CReadRegister(UV_TRIP);

    registers->Protect3.Protect3Bit.UV_DELAY = 0;

    // Update the register of the UV delay in accordance with the input delay value
    if (delay_s >= 16)
    {
        registers->Protect3.Protect3Bit.UV_DELAY = UV_DELAY_16s;
    }
    else if (delay_s >= 8)
    {
        registers->Protect3.Protect3Bit.UV_DELAY = UV_DELAY_8s;
    }
    else if (delay_s >= 4)
    {
        registers->Protect3.Protect3Bit.UV_DELAY = UV_DELAY_4s;
    }
    else if (delay_s >= 1)
    {
        registers->Protect3.Protect3Bit.UV_DELAY = UV_DELAY_1s;
    }

    I2CWriteRegister(PROTECT3, registers->Protect3.Protect3Byte);

    // returns the actual voltage threshold value (in mV)
    return ((long)1 << 12 | uv_trip << 4) * adcGain / 1000 + adcOffset;
}

//--------------------------------------------------------------------------------------------------------------------------

int bq769x0::setCellOvervoltageProtection(RegisterGroup *registers, int voltage_mV, int delay_s)
{
    int ov_trip = 0;
    maxCellVoltage = voltage_mV;

    registers->Protect3.Protect3Byte = I2CReadRegister(PROTECT3);

    ov_trip = ((((long)voltage_mV - adcOffset) * 1000 / adcGain) >> 4) & 0x00FF;

    // store the OV threshold in the register
    I2CWriteRegister(OV_TRIP, ov_trip);
    registers->OVTrip = I2CReadRegister(OV_TRIP);

    registers->Protect3.Protect3Bit.OV_DELAY = 0;

    // Update the register of the OV delay in accordance with the input delay value
    if (delay_s >= 8)
    {
        registers->Protect3.Protect3Bit.OV_DELAY = OV_DELAY_8s;
    }
    else if (delay_s >= 4)
    {
        registers->Protect3.Protect3Bit.OV_DELAY = OV_DELAY_4s;
    }
    else if (delay_s >= 2)
    {
        registers->Protect3.Protect3Bit.OV_DELAY = OV_DELAY_2s;
    }
    else if (delay_s >= 1)
    {
        registers->Protect3.Protect3Bit.OV_DELAY = OV_DELAY_1s;
    }

    I2CWriteRegister(PROTECT3, registers->Protect3.Protect3Byte);

    // returns the actual voltage threshold value (in mV)
    return ((long)1 << 13 | ov_trip << 4) * adcGain / 1000 + adcOffset;
}

//--------------------------------------------------------------------------------------------------------------------------

void bq769x0::setBalancingThresholds(int idleTime_min, int absVoltage_mV, uint8_t voltageDifference_mV)
{
    balancingMinIdleTime_s = idleTime_min * 60;
    balancingMinCellVoltage_mV = absVoltage_mV;
    balancingMaxVoltageDifference_mV = voltageDifference_mV;
}

//--------------------------------------------------------------------------------------------------------------------------

void bq769x0::setIdleCurrentThreshold(int current_mA)
{
    idleCurrentThreshold = current_mA;
}

//--------------------------------------------------------------------------------------------------------------------------

long  bq769x0::getBatteryCurrent(void)
{
    return batCurrent;
}

//--------------------------------------------------------------------------------------------------------------------------

long  bq769x0::getBatteryVoltage(void)
{
    return batVoltage;
}

//--------------------------------------------------------------------------------------------------------------------------

int  bq769x0::getCellVoltage(uint8_t idxCell)
{
    // from 1 to 15
    return cellVoltages[idxCell-1];
}

//--------------------------------------------------------------------------------------------------------------------------

int*  bq769x0::getCellVoltageBuffer(void)
{
    return cellVoltages;
}

//--------------------------------------------------------------------------------------------------------------------------
int  bq769x0::getMinCellVoltage(void)
{
    return cellVoltages[idxMinVoltageCell];
}

//--------------------------------------------------------------------------------------------------------------------------

int  bq769x0::getMaxCellVoltage(void)
{
    return cellVoltages[idxMaxVoltageCell];
}

//--------------------------------------------------------------------------------------------------------------------------

float bq769x0::getTemperatureDegC(uint8_t channel)
{
    if (channel >= 1 && channel <= 3)
    {
        return (float)temperatures[channel-1] / 10.0;
    }
    else
    {
        return -273.15;   // Error: Return absolute minimum temperature
    }
}

//--------------------------------------------------------------------------------------------------------------------------

float bq769x0::getTemperatureDegF(uint8_t channel)
{
    return getTemperatureDegC(channel) * 1.8 + 32;
}

//--------------------------------------------------------------------------------------------------------------------------

void bq769x0::I2CWriteRegister(uint32_t slaveReg, uint8_t data)
{
    // Set slave address into I2CMSA (offset 0x000), I2C1 Base = 0x40021000
    // true => I2C Master is initiating a read from the slave.
    // false => I2C Master is initiating a write to the slave.
    I2CMasterSlaveAddrSet(I2C1_BASE, I2C_SLAVE_ADDRESS, false);
    // in register: HWREG(0x40021000 + 0x000) = (slaveAddrress << 1) | (0 << 0);

    // Write the data into I2CMDR (offset 0x008)
    // Specify the register in slave which would be written by storing the register value in I2CMDR (temporary memory)
    // (slaveReg at here would be consider as an data to be send like telling the slave I want to write sth in this register)
    I2CMasterDataPut(I2C1_BASE, slaveReg);
    // in register: HWREG(0x40021000 + 0x008) = slaveReg;

    // Check whether the Bus in I2CMCS (offset 0x004) is busy
    // Wait for MCU to complete send transaction
//    while(I2CMasterBusBusy(I2C1_BASE));
    while( HWREG(0x40021000 + 0x004) & (1 << 6) ){};
    // in register: while( HWREG(0x40021000 + 0x004) & (1 << 6) ){};

    // Start sending out control byte and register address byte to slave device
    // the sent message comes from I2CMDR where we temporarily store the data
    I2CMasterControl(I2C1_BASE, I2C_MASTER_CMD_BURST_SEND_START);
    // in register: HWREG(0x40021000 + 0x004) = (1<<0)|(1<<1);
    //              (1<<0) specify RUN in register, (1<<1) specify START in register

    // Check whether the I2C in I2CMCS (offset 0x004) is busy
    // Wait for MCU to complete send transaction
    while(I2CMasterBusy(I2C1_BASE));
    // in register: while( HWREG(0x40021000 + 0x004) & (1 << 0) ){};

    // Write the data into I2CMDR (offset 0x008)
    // Send the register address that we want to write
    I2CMasterDataPut(I2C1_BASE, data);
    // in register: HWREG(0x40021000 + 0x008) = data;

    // Start sending out the last data that store in I2CMDR from the MCU
    // And finish the write
    I2CMasterControl(I2C1_BASE, I2C_MASTER_CMD_BURST_SEND_FINISH);  // corresponding to write 0-101 into I2CMCS => I2C_MASTER_CMD_SINGLE_SEND
    // in register: HWREG(0x40021000 + 0x004) = (1<<0)|(1<<2);
    //              (1<<0) specify RUN in register, (1<<2) specify STOP in register

    // Wait until MCU is done transferring.
    while(I2CMasterBusy(I2C1_BASE));
}

//--------------------------------------------------------------------------------------------------------------------------

uint32_t bq769x0::I2CReadRegister(uint32_t slaveReg)
{
    // Set slave address into I2CMSA (offset 0x000), I2C1 Base = 0x40021000
    // true => I2C Master is initiating a read from the slave.
    // false => I2C Master is initiating a write to the slave.
    I2CMasterSlaveAddrSet(I2C1_BASE, I2C_SLAVE_ADDRESS, false);
    // in register: HWREG(0x40021000 + 0x000) = (slaveAddrress << 1) | (0 << 0);

    // Write the data into I2CMDR (offset 0x008)
    // Specify the register in slave which would be written by storing the register value in I2CMDR (temporary memory)
    // (slaveReg at here would be consider as an data to be send like telling the slave I want to write sth in this register)
    I2CMasterDataPut(I2C1_BASE, slaveReg);
    // in register: HWREG(0x40021000 + 0x008) = slaveReg;

    // Check whether the Bus in I2CMCS (offset 0x004) is busy
    // Wait for MCU to complete send transaction
    while(I2CMasterBusBusy(I2C1_BASE));
    // in register: while( HWREG(0x40021000 + 0x004) & (1 << 6) ){};

    // Start sending out control byte and register address byte to slave device
    // the sent message comes from I2CMDR where we temporarily store the data
    I2CMasterControl(I2C1_BASE, I2C_MASTER_CMD_BURST_SEND_START);
    // in register: HWREG(0x40021000 + 0x004) = (1<<0)|(1<<1);
    //              (1<<0) specify RUN in register, (1<<1) specify START in register

    // Check whether the I2C in I2CMCS (offset 0x004) is busy
    // Wait for MCU to complete send transaction
    while(I2CMasterBusy(I2C1_BASE));
    // in register: while( HWREG(0x40021000 + 0x004) & (1 << 0) ){};

    //
    // After finishing slave address and slave register setting, we are now starting with data reading from slave
    //

    // Specify that we are going to read from slave device
    I2CMasterSlaveAddrSet(I2C1_BASE, I2C_SLAVE_ADDRESS, true);
    // in register: HWREG(0x40021000 + 0x000) = (slaveAddrress << 1) | (1 << 0);

    // Send control byte and register address byte to slave device
    // corresponding to write 0-111 into I2CMCS
    I2CMasterControl(I2C1_BASE, I2C_MASTER_CMD_SINGLE_RECEIVE);
    // in register: HWREG(0x40021000 + 0x004) = (1<<0)|(1<<1)|(1<<2);
    // SINGLE_RECEIVE : (1<<0) specify RUN in register, (1<<1) specify START in register, (1<<2) specify STOP in register

    // Wait for MCU to finish transaction
    while(I2CMasterBusy(I2C1_BASE));
    // in register: while( HWREG(0x40021000 + 0x004) & (1 << 0) ){};

    // Read the value (data) that store in I2CMDR
    // Return data pulled from the specified register
    return I2CMasterDataGet(I2C1_BASE);
    // in register: return HWREG(0x40021000 + 0x008)
}

//--------------------------------------------------------------------------------------------------------------------------

int bq769x0::checkStatus(RegisterGroup *registers)
{
    if (alertInterruptFlag == false && errorStatus == 0)  // (errorStatus == 0 || errorStatus == 128), !(errorStatus & 0b00111111)
    {
        // means everything is fine
        return 0;
    }
    else
    {
        registers->SysStatus.StatusByte = I2CReadRegister(SYS_STAT);

        if (registers->SysStatus.StatusBit.CC_READY == 1)
        {
            updateCurrent(registers, true);  // automatically clears CC ready flag
        }

        uint32_t sys_stat_clearCC = I2CReadRegister(SYS_STAT);

        // Serious error occurred
        if ((registers->SysStatus.StatusByte & 0b00111111))  //  || (alertInterruptFlag == true)
        {
            errorStatus = I2CReadRegister(SYS_STAT);

            // XR error
            if (registers->SysStatus.StatusByte & 0b00100000)
            {
                SysCtlDelay(100000); // clear this bit after waiting a few seconds
                I2CWriteRegister(SYS_STAT, 0b00100000);
            }
            uint32_t sys_stat_XR = I2CReadRegister(SYS_STAT);

            // Alert error
            if (registers->SysStatus.StatusByte & 0b00010000)
            {
                I2CWriteRegister(SYS_STAT, 0b00010000);
                SysCtlDelay(1000);
            }
            uint32_t sys_stat_Alert = I2CReadRegister(SYS_STAT);
            SysCtlDelay(10);

            // UV error
            if (registers->SysStatus.StatusByte & 0b00001000)
            {
                // Update the voltage of each cell
                updateVoltages();

                if (cellVoltages[idxMinVoltageCell] > minCellVoltage)
                {
                    I2CWriteRegister(SYS_STAT, 0b00001000);
                }
            }
            uint32_t sys_stat_UV = I2CReadRegister(SYS_STAT);
            SysCtlDelay(10);

            // OV error
            if (registers->SysStatus.StatusByte & 0b00000100)
            {
                // Update the voltage of each cell
                updateVoltages();

                if (cellVoltages[idxMaxVoltageCell] < maxCellVoltage)
                {
                    I2CWriteRegister(SYS_STAT, 0b00000100);
                }
            }
            uint32_t sys_stat_OV = I2CReadRegister(SYS_STAT);
            SysCtlDelay(10);

            // SCD
            if (registers->SysStatus.StatusByte & 0b00000010)
            {
                I2CWriteRegister(SYS_STAT, 0b00000010);
            }
            uint32_t sys_stat_SCD = I2CReadRegister(SYS_STAT);
            SysCtlDelay(10);

            // OCD
            if (registers->SysStatus.StatusByte & 0b00000001)
            {
                I2CWriteRegister(SYS_STAT, 0b00000001);
            }
            uint32_t sys_stat_OCD = I2CReadRegister(SYS_STAT);
            SysCtlDelay(10);

            // Update the errorStatus
            errorStatus = I2CReadRegister(SYS_STAT);
        }
        else
        {
            errorStatus = 0;
        }

        return errorStatus;
    }
}

//--------------------------------------------------------------------------------------------------------------------------

void bq769x0::update(RegisterGroup *registers)
{
    updateCurrent(registers, false);  // will only read new current value if alert was triggered
    updateVoltages();
    updateTemperatures();
//    updateBalancingSwitches(registers);

    enableDischarging(registers);
    enableCharging(registers);
}

//--------------------------------------------------------------------------------------------------------------------------

void bq769x0::updateDSG(RegisterGroup *registers)
{
    updateCurrent(registers, false);  // will only read new current value if alert was triggered
    updateVoltages();
    updateTemperatures();
//    updateBalancingSwitches(registers);

    enableDischarging(registers);
}

//--------------------------------------------------------------------------------------------------------------------------

void bq769x0::updateCHG(RegisterGroup *registers)
{
    updateCurrent(registers, false);  // will only read new current value if alert was triggered
    updateVoltages();
    updateTemperatures();
//    updateBalancingSwitches(registers);

    enableCharging(registers);
}

//--------------------------------------------------------------------------------------------------------------------------

void bq769x0::shutdown()
{
    I2CWriteRegister(SYS_CTRL1, 0x0);
    I2CWriteRegister(SYS_CTRL1, 0x1);
    I2CWriteRegister(SYS_CTRL1, 0x2);
}

//--------------------------------------------------------------------------------------------------------------------------

// If ignoreCCReadFlag == true, the current is read independent of an interrupt
// indicating the availability of a new CC reading
void bq769x0::updateCurrent(RegisterGroup *registers, bool ignoreCCReadyFlag)
{
    int adcVal = 0;

    registers->SysStatus.StatusByte = I2CReadRegister(SYS_STAT);

    if (ignoreCCReadyFlag == true || registers->SysStatus.StatusBit.CC_READY == 1)
    {

        uint32_t CC_HI = I2CReadRegister(CC_HI_BYTE);
        uint32_t CC_LO = I2CReadRegister(CC_LO_BYTE);

        adcVal = (I2CReadRegister(CC_HI_BYTE) << 8) | I2CReadRegister(CC_LO_BYTE);
        batCurrent = (int16_t) adcVal * 8.44 / shuntResistorValue_mOhm;  // mA
    }
    if (batCurrent > -10 && batCurrent < 10)  // control batCurrent range, minus means the current flows in opposite direction
    {
      batCurrent = 0;
    }

//    alertInterruptFlag = true;

    // no error occurred which caused alert
    if (!(registers->SysStatus.StatusByte & 0b00111111))
    {
        alertInterruptFlag = false;
    }
    else
    {
        alertInterruptFlag = true;
    }

    I2CWriteRegister(SYS_STAT, 0b10000000);  // Clear CC ready flag and set it available
    registers->SysStatus.StatusByte = I2CReadRegister(SYS_STAT);
}

//--------------------------------------------------------------------------------------------------------------------------

void bq769x0::updateVoltages(void)
{
    long adcVal = 0;

    // read battery pack (Bat+) voltage
    adcVal = (I2CReadRegister(BAT_HI_BYTE) << 8) | I2CReadRegister(BAT_LO_BYTE);
    batVoltage = 4.0 * adcGain * adcVal / 1000.0 + 4 * adcOffset;  // in mV


    // Initialize the index for the new round of measurement
    idxMaxVoltageCell = 0;
    idxMinVoltageCell = 0;

    // read cell voltages
    for (int i = 0; i < numberOfCells; i++)
    {
        adcVal = (I2CReadRegister(VC1_HI_BYTE + (i*2)) << 8) | I2CReadRegister(VC1_LO_BYTE + (i*2));
        cellVoltages[i] = adcVal * adcGain / 1000 + adcOffset;  // in mV

        // because we just have 4 batteries we have to set the 4th measurement equal to the 3th one
        // otherwise the value of 4th measurement would be around 0
        if (i == 3)
        {
            cellVoltages[i] = cellVoltages[2];
        }

        // start comparing
        if (cellVoltages[i] > cellVoltages[idxMaxVoltageCell])
        {
            idxMaxVoltageCell = i;
        }
        if (cellVoltages[i] < cellVoltages[idxMinVoltageCell] && cellVoltages[i] > 500)  // mV
        {
            idxMinVoltageCell = i;
        }
    }
}

//--------------------------------------------------------------------------------------------------------------------------

void  bq769x0::updateTemperatures(void)
{
    float tmp = 0;
    int adcVal = 0;
    int vtsx = 0;
    unsigned long rts = 0;

    // read cell voltages
    for (int i = 0; i < numberOfTemps; i++)
    {
        // calculate R_thermistor according to bq769x0 datasheet
        adcVal = (I2CReadRegister(TS1_HI_BYTE + (i*2)) << 8) | I2CReadRegister(TS1_LO_BYTE + (i*2));
        vtsx = adcVal * 0.382;  // mV
        rts = 10000.0 * vtsx / (3300.0 - vtsx);  // Ohm

        // Temperature calculation using Beta equation
        // - According to bq769x0 datasheet, only 10k thermistors should be used
        // - 25¢XC reference temperature for Beta equation assumed
        tmp = 1.0/(1.0/(273.15+25) + 1.0/thermistorBetaValue * log(rts/10000.0)); // K
        // remember to double check the thermistorBetaValue in the datasheet of the NTC the we use !!!!!!!

        temperatures[i] = (tmp - 273.15) * 10.0;  // in degC*10
    }
}

//--------------------------------------------------------------------------------------------------------------------------

// sets balancing registers if balancing is allowed
// To activate a particular cell balancing channel, simply set the corresponding bit for that cell in the CELLBAL1, CELLBAL2, or CELLBAL3 register
void bq769x0::updateBalancingSwitches(RegisterGroup *registers)
{
    int numberOfSections = numberOfCells/5;

    // check if balancing allowed, (checkStatus(registers) == 128 || checkStatus(registers) == 0), !(checkStatus(registers) & 0b00111111)
    if (checkStatus(registers) == 0 &&
        cellVoltages[idxMaxVoltageCell] > balancingMinCellVoltage_mV &&
        (cellVoltages[idxMaxVoltageCell] - cellVoltages[idxMinVoltageCell]) > balancingMaxVoltageDifference_mV )
    {
        balancingActive = true;

        int balancingFlags;        // store the command of which cell to balance in bit
        int balancingFlagsTarget;  // store the cell address that have to be balanced

        // go through all section of cell balancing, depend on which bq we use (max 3 section)
        for (int section = 0; section < numberOfSections; section++)
        {
            balancingFlags = 0;

            // each section has 5 cell (1-5, 6-10, 11-15)
            for (int i = 0; i < 5; i++)
            {
                if ((cellVoltages[section*5 + i] - cellVoltages[idxMinVoltageCell]) > balancingMaxVoltageDifference_mV)
                {
                    // try to enable balancing of current cell
                    balancingFlagsTarget = balancingFlags | (1 << i);

                    // check if attempting to balance adjacent cells.
                    // Adjacent cells should not be balanced simultaneously. This may cause cell pins to exceed their absolute maximum conditions
                    bool adjacentCellCollision = ((balancingFlagsTarget << 1) & balancingFlags) || ((balancingFlags << 1) & balancingFlagsTarget);

                    if (adjacentCellCollision == false)
                    {
                        balancingFlags = balancingFlagsTarget;
                    }
                }
            }

            // set balancing register for this section
            I2CWriteRegister(CELLBAL1+section, balancingFlags);
        }

        // maybe here add system delay (250 ms) for the balancing
        Delay(250);
    }
    else if (balancingActive == true)
    {
        // clear all CELLBAL registers
        for (int section = 0; section < numberOfSections; section++)
        {
            I2CWriteRegister(CELLBAL1+section, 0x0);
        }

        balancingActive = false;
    }
}

//--------------------------------------------------------------------------------------------------------------------------

bool bq769x0::enableDischarging(RegisterGroup *registers)
{
/*
 * Debugging test, step for step
    uint32_t sys_ctrl2 = I2CReadRegister(SYS_CTRL2);  // 0b1000000
    sys_ctrl2 |= 0b00000010;  // 66
    I2CWriteRegister(SYS_CTRL2, sys_ctrl2);  // switch DSG on

    uint32_t sys_stat = I2CReadRegister(SYS_STAT);
    uint32_t CHGAddress = I2CReadRegister(SYS_CTRL2);

    int a = I2CReadRegister(SYS_CTRL2) & 0b00000010;

    if (I2CReadRegister(SYS_CTRL2) == 66)
    {
        return true;
        GPIOPinWrite(LED_BASE, OK_LED, OK_LED);
    }
    else
    {
        return false;
    }
*/

// for Debugging ......
    int errorstatus = checkStatus(registers);
    uint32_t sys_ctr2_l = I2CReadRegister(SYS_CTRL2);

    // (checkStatus(registers) == 128 || checkStatus(registers) == 0), !(errorstatus & 0b00111111)
    if (!(errorstatus & 0b00111111) &&
        cellVoltages[idxMaxVoltageCell] < maxCellVoltage &&
        temperatures[0] < maxCellTempCharge &&
        temperatures[0] > minCellTempCharge)
    {

        uint32_t sys_ctrl2 = I2CReadRegister(SYS_CTRL2);
        I2CWriteRegister(SYS_CTRL2, sys_ctrl2 | 0b00000010);  // switch DSG on

// for Debugging ......
        uint32_t sys_stat = I2CReadRegister(SYS_STAT);
        uint32_t sys_ctrl2_2 = I2CReadRegister(SYS_CTRL2);

        return true;
    }
    else
    {
        uint32_t CHGAddress = I2CReadRegister(SYS_CTRL2);
        return false;
    }

}

//--------------------------------------------------------------------------------------------------------------------------

void bq769x0::disableDischarging(void)
{
    uint8_t sys_ctrl2 = I2CReadRegister(SYS_CTRL2);
    I2CWriteRegister(SYS_CTRL2, sys_ctrl2 | 0b00000010);  // switch DSG off
}

//--------------------------------------------------------------------------------------------------------------------------

bool bq769x0::enableCharging(RegisterGroup *registers)
{
    // (checkStatus(registers) == 128 || checkStatus(registers) == 0), !(checkStatus(registers) & 0b00111111
    if (checkStatus(registers) == 0 &&
        cellVoltages[idxMaxVoltageCell] < maxCellVoltage &&
        temperatures[0] < maxCellTempCharge &&
        temperatures[0] > minCellTempCharge)
    {
        uint8_t sys_ctrl2 = I2CReadRegister(SYS_CTRL2);
        I2CWriteRegister(SYS_CTRL2, sys_ctrl2 | 0b00000001);  // switch CHG on
        registers->SysCtrl2.SysCtrl2Byte = I2CReadRegister(SYS_CTRL2);  // store and to see the current value
        return true;
    }
    else
    {
        return false;
    }
}

//--------------------------------------------------------------------------------------------------------------------------

void bq769x0::disableCharging(void)
{
    uint8_t sys_ctrl2 = I2CReadRegister(SYS_CTRL2);
    I2CWriteRegister(SYS_CTRL2, sys_ctrl2 | 0b00000001);  // switch CHG off
}

//--------------------------------------------------------------------------------------------------------------------------

void bq769x0::enableAutoBalancing(void)
{
  autoBalancingEnable = true;
}

//--------------------------------------------------------------------------------------------------------------------------

void bq769x0::disableAutoBalancing(void)
{
  autoBalancingEnable = false;
}

//--------------------------------------------------------------------------------------------------------------------------

int bq769x0::getADCGain(void)
{
    return adcGain;
}

//--------------------------------------------------------------------------------------------------------------------------

int bq769x0::getADCOffset(void)
{
    return adcOffset;
}

//--------------------------------------------------------------------------------------------------------------------------



//--------------------------------------------------------------------------------------------------------------------------



//--------------------------------------------------------------------------------------------------------------------------



//--------------------------------------------------------------------------------------------------------------------------



