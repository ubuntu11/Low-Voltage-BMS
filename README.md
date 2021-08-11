# Low Voltage Battery Management System with AFE(BQ76920) and Microcontroller (TM4C123GH6PM)
This is a hardware and software development of a low voltage battery management syetem, which is mainly designed for our autonomous BEV and self-driving car. However, it could also be applied to other electric devices which need an monitoring system for low voltage battery.

**LV-BMS version 1 PCB:**
![LV-BMS version 1 PCB](https://github.com/PingCheng-Wei/Low-Voltage-BMS/blob/main/assets/BMS_version1.jpg)

The repository includes:
* The Altium PCB design of LV-BMS with either low-side MOSFET switch control or high-side.
* Source code for the microcontroller based on C++ and is written and exploited in Code Composer Studio.
* Texas Instruments TivaWare Peripheral Driver Library for the control of microcontroller. [datasheet](https://www.ti.com/lit/ug/spmu298e/spmu298e.pdf?ts=1628540888902&ref_url=https%253A%252F%252Fwww.google.com%252F)

# Table Of Contents


# Concept and Functionality of the LV-BMS
The main goals of the 

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

