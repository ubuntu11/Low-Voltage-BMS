# Low Voltage Battery Management System with AFE(BQ76920) and Microcontroller (TM4C123GH6PM)
This is a hardware and software development of a low voltage battery management syetem, which is mainly designed for our autonomous BEV and self-driving car. However, it could also be applied to other electric devices which need an monitoring system for low voltage battery.

**LV-BMS version 1 PCB:**
![LV-BMS version 1 PCB](https://github.com/PingCheng-Wei/Low-Voltage-BMS/blob/main/assets/BMS_version1.jpg)

The repository includes:
* The Altium PCB design of LV-BMS with either low-side MOSFET switch control(version 2) or high-side(version 3).
* Source code for the microcontroller based on C++, which is compatible for all version(1~3) design, and is written and exploited in Code Composer Studio.
* Texas Instruments TivaWare Peripheral Driver Library for the control of microcontroller. [datasheet](https://www.ti.com/lit/ug/spmu298e/spmu298e.pdf?ts=1628540888902&ref_url=https%253A%252F%252Fwww.google.com%252F)

# Table Of Contents
* [Concept and Functionality of the LV-BMS](https://github.com/PingCheng-Wei/Low-Voltage-BMS/concept-and-funtionality-of-the-lv-bms)

# Concept and Functionality of the LV-BMS
The main idea that we need a battery management system is that lithium battery has a higher energy density, higher efficiency and lighter weight in comparison with other art of battery such as LiFePo4, which is crucial in our case to gain better performence, but it could also be life-threatening when something goes wrong. Therefore, we need a monitoring system, BMS, to keep supervising the status of the lithium battery stack during the execution of whole system and take defensive action if it is needed. The status at here means the **voltages**, **current** and **temperatures** of the battery stack. To be more specific, voltages could be the **cell voltages** as well as the **entire voltage** and current is the **entire current** that flows out the battery stack and temperatures could be the **cell temperatures** as well as the **MOSFETs' temperatures**. 

BMS monitor these three status to make sure they are all in the acceptable range. if not, then BMS will cut off the power supply of the whole sytem(Battery) by switching the MOSFETs to off so that no further damage could happen and if the stutas are back to normal again, then BMS will turn on the power suppy again.

# Hardware Part

## Components (datasheets)

## Analog Front End (AFE)

## I2C Communication

## Analog to Digital Converter (ADC)


# Software Part

## Enviroment and Library (datasheet)
* Code in C++
* Code Composer Studio with setting of `TM4C123GH6PM` microcontroller for flashing
* TivaWare Peripheral Driver Library

## Concept



# Installation 
1. Clone this repository
2. Download Altium and activate the licence in order to use it 
    * imoprt all the schematic libraries, PCB libraries and compiled libraries of the conponents from `Component Footprint Import` directory
        ```
        Open Altium and ...
        ```
    * In order to open the PCB project, click the file with  `.PrjPcb` extension. This will automatically open all the schematics and PCBs file which belong to the Project.
        ```
        For instance: LV-BMS_21_V2.PrjPcb
        ```
3. Download Code Composer Studio
    * Set the microcontroller to `TM4C123GH6PM`
    * make sure the path to the TivaWare Peripheral Driver Library is correct in the properties of the project.
        ```
        if you download this repository directly without any changing, your directory should have the following structure:

        <dataset root directory>/
        images/
            amodal_masks/
                image_000000/
                    channel_000.png
                    channel_001.png

        
        
        ```

