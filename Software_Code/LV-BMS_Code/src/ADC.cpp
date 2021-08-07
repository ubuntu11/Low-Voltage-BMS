/*
 * ADC.cpp
 *
 *  Created on: Feb 01, 2021
 *      Author: Dennis
 */

#include <include/ADC.hpp>

ADC::ADC()
{
    // TODO Auto-generated constructor stub
}

//--------------------------------------------------------------------------------------------------------------------------

void ADC::Init(void)
{
    // Enables ADC peripheral
    SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_ADC0));

    SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC1);
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_ADC1));

    // Enable Ports
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);

    // Configure the Pins for ADC input
    GPIOPinTypeADC(GPIO_PORTB_BASE, GPIO_PIN_4 | GPIO_PIN_5);    // for Temperature 1 & 3
    GPIOPinTypeADC(GPIO_PORTE_BASE, GPIO_PIN_4);                 // for Temperature 2
    GPIOPinTypeADC(GPIO_PORTE_BASE, GPIO_PIN_1 | GPIO_PIN_2);    // for CHG/DSG - NTCs
    GPIOPinTypeADC(GPIO_PORTE_BASE, GPIO_PIN_0);                 // for Pack+ Voltage
    GPIOPinTypeADC(GPIO_PORTD_BASE, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3); // for backup voltage
    GPIOPinTypeADC(GPIO_PORTE_BASE, GPIO_PIN_5);                 // for backup voltage

    // A sample sequence must be disabled before it is configured.
    ADCSequenceDisable(ADC0_BASE, 0);
    ADCSequenceDisable(ADC1_BASE, 1);

    // Configure the initiation criteria for sample sequence
    ADCSequenceConfigure(ADC0_BASE, 0, ADC_TRIGGER_PROCESSOR, 0);
    ADCSequenceConfigure(ADC0_BASE, 1, ADC_TRIGGER_PROCESSOR, 1);
    ADCSequenceConfigure(ADC0_BASE, 2, ADC_TRIGGER_PROCESSOR, 2);
    ADCSequenceConfigure(ADC1_BASE, 0, ADC_TRIGGER_PROCESSOR, 0);
    ADCSequenceConfigure(ADC1_BASE, 1, ADC_TRIGGER_PROCESSOR, 1);

    // Set up internal 3V reference for each ADC
    ADCReferenceSet(ADC0_BASE, ADC_REF_INT);
    ADCReferenceSet(ADC1_BASE, ADC_REF_INT);

    // configures the hardware oversampling for the ADC, which can be used to provide better resolution on the sampled data
    ADCHardwareOversampleConfigure(ADC0_BASE, 16);
    ADCHardwareOversampleConfigure(ADC1_BASE, 16);

    // Set up each step of a sample
    // Set up ADC Base 0, sample sequence 0 for stack Temperature measurement
    ADCSequenceStepConfigure(ADC0_BASE, 0, 0, ADC_CTL_CH10);    // measure Temperature 1
    ADCSequenceStepConfigure(ADC0_BASE, 0, 1, ADC_CTL_CH10);    // measure Temperature 1
    ADCSequenceStepConfigure(ADC0_BASE, 0, 2, ADC_CTL_CH10);    // measure Temperature 1
    ADCSequenceStepConfigure(ADC0_BASE, 0, 3, ADC_CTL_CH11);    // measure Temperature 3
    ADCSequenceStepConfigure(ADC0_BASE, 0, 4, ADC_CTL_CH11);    // measure Temperature 3
    ADCSequenceStepConfigure(ADC0_BASE, 0, 5, ADC_CTL_CH11);    // measure Temperature 3
    ADCSequenceStepConfigure(ADC0_BASE, 0, 6, ADC_CTL_CH9);     // measure Temperature 2
    ADCSequenceStepConfigure(ADC0_BASE, 0, 7, ADC_CTL_CH9 | ADC_CTL_IE | ADC_CTL_END);  // measure Temperature 2

    // Set up ADC Base 0, sample sequence 1 for CHG & DSG NTCs
    ADCSequenceStepConfigure(ADC0_BASE, 1, 0, ADC_CTL_CH1);     // measure DSG-NTC
    ADCSequenceStepConfigure(ADC0_BASE, 1, 1, ADC_CTL_CH1);     // measure DSG-NTC
    ADCSequenceStepConfigure(ADC0_BASE, 1, 2, ADC_CTL_CH2);     // measure CHG-NTC
    ADCSequenceStepConfigure(ADC0_BASE, 1, 3, ADC_CTL_CH2 | ADC_CTL_IE | ADC_CTL_END);  // measure CHG-NTC

    // Set up ADC Base 0, sample sequence 2 for PACK+ Voltage
    ADCSequenceStepConfigure(ADC0_BASE, 2, 0, ADC_CTL_CH3);     // measure PACK+ Voltage
    ADCSequenceStepConfigure(ADC0_BASE, 2, 1, ADC_CTL_CH3);     // measure PACK+ Voltage
    ADCSequenceStepConfigure(ADC0_BASE, 2, 2, ADC_CTL_CH3);     // measure PACK+ Voltage
    ADCSequenceStepConfigure(ADC0_BASE, 2, 3, ADC_CTL_CH3 | ADC_CTL_IE | ADC_CTL_END);  // measure PACK+ Voltage

    // Set up ADC Base 1, sample sequence 0 for Backup Voltage
    ADCSequenceStepConfigure(ADC1_BASE, 0, 0, ADC_CTL_CH6);     // measure cell 4 (Backup)
    ADCSequenceStepConfigure(ADC1_BASE, 0, 1, ADC_CTL_CH6);     // measure cell 4 (Backup)
    ADCSequenceStepConfigure(ADC1_BASE, 0, 2, ADC_CTL_CH6);     // measure cell 4 (Backup)
    ADCSequenceStepConfigure(ADC1_BASE, 0, 3, ADC_CTL_CH7);     // measure cell 3 (Backup)
    ADCSequenceStepConfigure(ADC1_BASE, 0, 4, ADC_CTL_CH7);     // measure cell 3 (Backup)
    ADCSequenceStepConfigure(ADC1_BASE, 0, 5, ADC_CTL_CH7);     // measure cell 3 (Backup)
    ADCSequenceStepConfigure(ADC1_BASE, 0, 6, ADC_CTL_CH8);     // measure cell 2 (Backup)
    ADCSequenceStepConfigure(ADC1_BASE, 0, 7, ADC_CTL_CH8 | ADC_CTL_IE | ADC_CTL_END);  // measure cell 2 (Backup)

    // Set up ADC Base 1, sample sequence 1 for Backup Voltage
    ADCSequenceStepConfigure(ADC1_BASE, 1, 0, ADC_CTL_CH5);     // measure cell 1 (Backup)
    ADCSequenceStepConfigure(ADC1_BASE, 1, 1, ADC_CTL_CH5);     // measure cell 1 (Backup)
    ADCSequenceStepConfigure(ADC1_BASE, 1, 2, ADC_CTL_CH4);     // measure GND (Backup)
    ADCSequenceStepConfigure(ADC1_BASE, 1, 3, ADC_CTL_CH4 | ADC_CTL_IE | ADC_CTL_END);  // measure GND (Backup)

    // Enable the Port base
    ADCSequenceEnable(ADC0_BASE,0);
    ADCSequenceEnable(ADC0_BASE,1);
    ADCSequenceEnable(ADC0_BASE,2);
    ADCSequenceEnable(ADC1_BASE,0);
    ADCSequenceEnable(ADC1_BASE,1);


    /*
    SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_ADC0));

    SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC1);
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_ADC1));

    GPIOPinTypeADC(GPIO_PORTD_BASE, GPIO_PIN_2 | GPIO_PIN_3);    // CellTemprature1(PD2)  CellTemprature0(PD3)
    GPIOPinTypeADC(GPIO_PORTD_BASE, GPIO_PIN_1 | GPIO_PIN_0);    // NTC_1(PD1),NTC_2(PD0)
    GPIOPinTypeADC(GPIO_PORTE_BASE, GPIO_PIN_5 );                // CellTemprature2(PE5)

    // A sample sequence must be disabled before it is configured.
    ADCSequenceDisable(ADC0_BASE, 0);
    ADCSequenceDisable(ADC1_BASE, 1);

    // Configure the initiation criteria for sample sequence
    ADCSequenceConfigure(ADC0_BASE, 0, ADC_TRIGGER_PROCESSOR, 0);
    ADCSequenceConfigure(ADC1_BASE, 1, ADC_TRIGGER_PROCESSOR, 0);

    // Set up internal 3V reference for each ADC
    ADCReferenceSet(ADC0_BASE, ADC_REF_INT);
    ADCReferenceSet(ADC1_BASE, ADC_REF_INT);

    // configures the hardware oversampling for the ADC, which can be used to provide better resolution on the sampled data
    ADCHardwareOversampleConfigure(ADC0_BASE, 16);
    ADCHardwareOversampleConfigure(ADC1_BASE, 16);

    // Set up each step of a sample
    ADCSequenceStepConfigure(ADC0_BASE, 0, 0, ADC_CTL_CH7);  // for CHG DSG NTCs
    ADCSequenceStepConfigure(ADC0_BASE, 0, 1, ADC_CTL_CH6 | ADC_CTL_IE | ADC_CTL_END);

    ADCSequenceStepConfigure(ADC1_BASE, 1, 0, ADC_CTL_CH5);  // for stack temp
    ADCSequenceStepConfigure(ADC1_BASE, 1, 1, ADC_CTL_CH4);  // for stack temp
    ADCSequenceStepConfigure(ADC1_BASE, 1, 2, ADC_CTL_CH8 | ADC_CTL_IE | ADC_CTL_END);

    // Enable the Port base
    ADCSequenceEnable(ADC0_BASE,0);
    ADCSequenceEnable(ADC1_BASE,1);
     */
}

//--------------------------------------------------------------------------------------------------------------------------

void ADC::readStackTemp(void)
{
    // Initialization stack temperature
    stackTemperature.temperature1 = 0;
    stackTemperature.temperature2 = 0;
    stackTemperature.temperature3 = 0;

    // initialize the Buffer (array)
    uint32_t ADCFiFoBuffer[8] = {0};

    // Clear the sample sequence interrupt source
    ADCIntClear(ADC0_BASE, 0);

    // Triggers the sample sequence
    ADCProcessorTrigger(ADC0_BASE, 0);

    // Wait until the sample sequence has completed
    while(!ADCIntStatus(ADC0_BASE, 0, false)){};

    // Read the ADC0, sample sequence 0 data and store it temporarily in Buffer
    ADCSequenceDataGet(ADC0_BASE, 0, ADCFiFoBuffer);

    // Initialize an array to store the converted ADC temperature value
    uint32_t adcTempInDegC[8];

    // Convert the ADC temperature value
    for (int i = 0; i < sizeof(ADCFiFoBuffer)/4; i++)
    {
        adcTempInDegC[i] = convertTempIndegC(ADCFiFoBuffer[i], 3.3, 3.0, 3428, 470, 298.15, 10000);
    }

    // Calculate the average of each Temperature value to have a better accuracy and store it (in celsius)
    stackTemperature.temperature1 = (adcTempInDegC[0] + adcTempInDegC[1] + adcTempInDegC[2]) / 3;  // in celsius
    stackTemperature.temperature2 = (adcTempInDegC[3] + adcTempInDegC[4] + adcTempInDegC[5]) / 3;  // in celsius
    stackTemperature.temperature3 = (adcTempInDegC[6] + adcTempInDegC[7]) / 2;                     // in celsius
}

//--------------------------------------------------------------------------------------------------------------------------

void ADC::readMosfetTemp(void)
{
    // Initialization DSG/CHG temperature
    mosfetTemperature.DSGFetTemp = 0;
    mosfetTemperature.CHGFetTemp = 0;

    // initialize the Buffer (array)
    uint32_t ADCFiFoBuffer[4] = {0};

    // Clear the sample sequence interrupt source
    ADCIntClear(ADC0_BASE, 1);

    // Triggers the sample sequence
    ADCProcessorTrigger(ADC0_BASE, 1);

    // Wait until the sample sequence has completed
    while(!ADCIntStatus(ADC0_BASE, 1, false)){};

    // Read the ADC0, sample sequence 1 data and store it temporarily in Buffer
    ADCSequenceDataGet(ADC0_BASE, 1, ADCFiFoBuffer);

    // Initialize an array to store the converted ADC temperature value
    uint32_t adcDSG_CHG[4];

    // Convert the ADC temperature value
    for (int i = 0; i < sizeof(ADCFiFoBuffer)/4; i++)
    {
        adcDSG_CHG[i] = convertTempIndegC(ADCFiFoBuffer[i], 3.3, 3.0, 3428, 600, 298.15, 10000);
    }

    // Calculate the average of each Temperature value to have a better accuracy and store it (in celsius)
    mosfetTemperature.DSGFetTemp = (adcDSG_CHG[0] + adcDSG_CHG[1]) / 2;
    mosfetTemperature.CHGFetTemp = (adcDSG_CHG[2] + adcDSG_CHG[3]) / 2;
}

//--------------------------------------------------------------------------------------------------------------------------

void ADC::readVoltage(void)
{
    // Initialization all voltage variables
    voltageMeasurement.cellVoltage4 = 0;
    voltageMeasurement.cellVoltage3 = 0;
    voltageMeasurement.cellVoltage2 = 0;
    voltageMeasurement.cellVoltage1 = 0;
    voltageMeasurement.cellGND = 0;
    voltageMeasurement.PackVoltage = 0;

    // initialize the Buffer (array)
    uint32_t packBuffer[4] = {0};
    uint32_t voltageGroup1[8] = {0};
    uint32_t voltageGroup2[4] = {0};

    // Clear the sample sequence interrupt source
    ADCIntClear(ADC0_BASE, 2);
    ADCIntClear(ADC1_BASE, 0);
    ADCIntClear(ADC1_BASE, 1);


    // Triggers the sample sequence
    ADCProcessorTrigger(ADC0_BASE, 2);
    // Wait until the sample sequence has completed
    while(!ADCIntStatus(ADC0_BASE, 2, false)){};
    // Read the ADC0, sample sequence 2 data and store it temporarily in Buffer
    ADCSequenceDataGet(ADC0_BASE, 2, packBuffer);

    // Triggers the sample sequence
    ADCProcessorTrigger(ADC1_BASE, 0);
    // Wait until the sample sequence has completed
    while(!ADCIntStatus(ADC1_BASE, 0, false)){};
    // Read the ADC1, sample sequence 0 data and store it temporarily in Buffer
    ADCSequenceDataGet(ADC1_BASE, 0, voltageGroup1);

    // Triggers the sample sequence
    ADCProcessorTrigger(ADC1_BASE, 1);
    // Wait until the sample sequence has completed
    while(!ADCIntStatus(ADC1_BASE, 1, false)){};
    // Read the ADC0, sample sequence 2 data and store it temporarily in Buffer
    ADCSequenceDataGet(ADC1_BASE, 1, voltageGroup2);

    // Initialize an array to store the converted ADC value
    uint32_t packADC[4];
    uint32_t voltageGroup1ADC[8];
    uint32_t voltageGroup2ADC[4];

    // get the converted value
    // for Pack + (R_1 ~ 270kOhm, R_last ~ 12kOhm)
    for (int i = 0; i < sizeof(packBuffer)/4; i++)
    {
        packADC[i] = convertVoltage(packBuffer[i], 3.0, 315000, 12000);
    }
    // Calculate the average
    voltageMeasurement.PackVoltage = (packADC[0]+packADC[1]+packADC[2]+packADC[3]) / 4;

    // for Backup voltage cell 4, 3, 2 (R_1 = 51kOhm, R_last = 10kOhm)
    for (int i = 0; i < sizeof(voltageGroup1)/4; i++)
    {
        voltageGroup1ADC[i] = convertVoltage(voltageGroup1[i], 3.0, 58000, 10000);
    }
    // Calculate the average
    voltageMeasurement.cellVoltage4 = (voltageGroup1ADC[0]+voltageGroup1ADC[1]+voltageGroup1ADC[2]) / 3;
    voltageMeasurement.cellVoltage3 = (voltageGroup1ADC[3]+voltageGroup1ADC[4]+voltageGroup1ADC[5]) / 3;
    voltageMeasurement.cellVoltage2 = (voltageGroup1ADC[6]+voltageGroup1ADC[7]) / 2;

    // for Back voltage cell 1 & GND
    for (int i = 0; i < sizeof(voltageGroup2)/4; i++)
    {
        if (i > 1)
        {
            // Get the GND value
            voltageGroup2ADC[i] = convertVoltage(voltageGroup2[i], 3.0, 0, 0);
        }
        else
        {
            // Get the cell 1 value (R_1 = 51kOhm, R_last = 10kOhm)
            voltageGroup2ADC[i] = convertVoltage(voltageGroup2[i], 3.0, 58000, 10000);
        }
    }
    // Calculate the average
    voltageMeasurement.cellVoltage1 = (voltageGroup2ADC[0]+voltageGroup2ADC[1]) / 2;
    voltageMeasurement.cellGND = (voltageGroup2ADC[2]+voltageGroup2ADC[3]) / 2;
}

//--------------------------------------------------------------------------------------------------------------------------

uint32_t ADC::convertTempIndegC(uint32_t V_mes, float V_ref, float VCC, float B, int R_div, float T_ref, float R_ntc_normal)
{
    // Calculate voltage
    float U_ntc;
    U_ntc= ((V_mes/4096.0) * V_ref);

    // Calculate the current
    float I;
    I = (VCC - U_ntc) / R_div;

    // Calculate the varied Resistance value
    float R_ntc;
    R_ntc = U_ntc/I;

    // Calculate the current Temperature (in Kelvin)
    float new_Temp;
    new_Temp = T_ref * B/ (B + T_ref * log(R_ntc / R_ntc_normal));  // in Kelvin

    // Convert into celsius (degree C)
    new_Temp = new_Temp - 273.1;

    return (uint32_t)(new_Temp * 10) ;  // (in degC*10)
}

//--------------------------------------------------------------------------------------------------------------------------

uint32_t ADC::convertVoltage(uint32_t V_mes, float V_ref, int R_1, float R_last)
{
    // Calculate the voltage drop on the last resistance
    float U_last_mV;
    U_last_mV = ((V_mes/4096.0) * V_ref) * 1000;  // in mV

    // Calculate the factor of the voltage divider
    float factor;
    if (R_1 == 0)
    {
        factor = 1;
    }
    else
    {
        factor = ((R_1 + R_last) / R_last);
    }

    // Convert back to the real voltage from the voltage divider
    float U_real_mV;
    U_real_mV = U_last_mV * factor;  // in mV

    return (uint32_t)(U_real_mV);
}

//--------------------------------------------------------------------------------------------------------------------------

BatteryStackTemperature ADC::getBatteryStackTemperature()
{
    return stackTemperature;
}

//--------------------------------------------------------------------------------------------------------------------------

MOSFETTemperature ADC::getMosfetTemperature()
{
    return mosfetTemperature;
}

//--------------------------------------------------------------------------------------------------------------------------

VoltageMeasurement ADC::getVoltageMeasurement()
{
    return voltageMeasurement;
}

//--------------------------------------------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------------------------------------------

