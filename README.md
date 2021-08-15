# Low Voltage Battery Management System with AFE(BQ76920) and Microcontroller (TM4C123GH6PM)
This is a hardware and software development of a low voltage battery management syetem, which is mainly designed for our autonomous BEV and self-driving car. However, it could also be applied to other electric devices which need an monitoring system for low voltage battery.

**LV-BMS version 1 PCB:**
![LV-BMS version 1 PCB](https://github.com/PingCheng-Wei/Low-Voltage-BMS/blob/main/assets/BMS_version1.jpg)

The repository includes:
* The Altium PCB design of LV-BMS with either low-side MOSFET switch control(version 2) or high-side(version 3).
* Source code for the microcontroller based on C++, which is compatible for all version(1~3) design, and is written and exploited in Code Composer Studio.
* Texas Instruments TivaWare Peripheral Driver Library for the control of microcontroller. [datasheet](https://www.ti.com/lit/ug/spmu298e/spmu298e.pdf?ts=1628540888902&ref_url=https%253A%252F%252Fwww.google.com%252F)

# Table Of Contents
* [Concept and Functionality of the LV-BMS](https://github.com/PingCheng-Wei/Low-Voltage-BMS#concept-and-functionality-of-the-lv-bms)


# Concept and Functionality of the LV-BMS
The main idea that we need a battery management system is that lithium battery has a higher energy density, higher efficiency and lighter weight in comparison with other art of battery such as LiFePo4, which is crucial in our case to gain better performence, but it could also be life-threatening when something goes wrong. Therefore, we need a monitoring system, BMS, to keep supervising the status of the lithium battery stack during the execution of whole system and take defensive action if it is needed. The status at here means the `voltages`, `current` and `temperatures` of the battery stack. To be more specific, voltages could be the `cell voltages` as well as the `entire voltage` and current is the `entire current` that flows out the battery stack and temperatures could be the `cell temperatures` as well as the `MOSFETs' temperatures`. 

BMS monitor these three status to make sure they are all operating in the acceptable range. if not, then BMS will cut off the power supply of the whole system (LV-Battery) by switching the MOSFETs to off so that no further damage could happen and if the stutas are back to normal again, then BMS will turn on the power suppy again.


# Development of Hardware Part
The general structure of the hardware PCB design is based on the following functional parts:

* `Analog Front End (AFE)`, which is mainly responsible for collecting the parameters of the battery pack such as voltages, current as well as temperatures by using internal ADC measures. It could also control the current between the cells in order to avoid overloading and underloading, which is called cell-balancing. At here we pick [BQ76920](https://www.ti.com/lit/ds/symlink/bq76920.pdf?ts=1628676283263&ref_url=https%253A%252F%252Fwww.ti.com%252Fproduct%252FBQ76920) from TI as our AFE since it has not only all the basic features that we need but also some other extra features e.g. hardware protection features, charge/discharge low-side NCH FET drivers, alert interrupt to host microcontroller, simple I2C compatible interface (CRC option)
* `Inter-Integrated Circuit (I2C) Interface`, which is really crucial to realize the communication between our host microcontroller (uC), `TM4C123GH6PM`, and AFE ,`BQ76920`. Through the I2C communication, uC can read value from AFE register, send commmed to AFE or even write value into the register in AFE and AFE can also sent alert to uC so that uC could take an action to deal with the errors
* `Microcontroller (uC)`, which is like the brain of the system and where we flash our code into in order to realize all the functions that we need. Here is `TM4C123GH6PM` chosen as our uC because it has lots of features to utilize and there is also a compatible [Peripheral Driver Library](https://www.ti.com/lit/ug/spmu298e/spmu298e.pdf?ts=1628540888902&ref_url=https%253A%252F%252Fwww.google.com%252F) on hand.

    Here is an overview of how BQ and uC structured:
    
    ![overview of BQ-chip](https://github.com/PingCheng-Wei/Low-Voltage-BMS/blob/main/assets/LV_structure.png)

Other feature that we need for our autonomous BEV and self-driving car:
* `CAN Bus system`, which is a communication system between our uC and centeral `main control unit (MCU)` in our cars so that MCU could know what is going on right now in LV-system and whether every thing is fine. For some cases, MCU could even take some preventative steps to protect the car if something in LV-system goes wrong.
* `Analog to digital converter (ADC) in uC`, which is exploited for more temperatures monitoring such as Battery stack temperatures and MOSFETs' temperatures.


## Components & Datasheets
You could find all the components and their footprints that are utilized on the PCB in the [Bill of Materials (BOM)](https://github.com/PingCheng-Wei/Low-Voltage-BMS/blob/main/assets/BOM_LV_21E.xlsx). Here we just list the most important components:

* Microcontroller: [TM4C123GH6PM](https://www.ti.com/lit/ds/spms376e/spms376e.pdf?ts=1628616879792&ref_url=https%253A%252F%252Fwww.google.com%252F)
* Analog Front End: [BQ76920](https://www.ti.com/lit/ds/symlink/bq76920.pdf?ts=1628676283263&ref_url=https%253A%252F%252Fwww.ti.com%252Fproduct%252FBQ76920) without CRC
* FET Driver: [BQ76200](#TODO)
* MOSFETs: [AUIRFS8409-7P](https://www.infineon.com/dgdl/auirfs8409-7p.pdf?fileId=5546d462533600a4015355b745e314f0)
* 3.3V CAN Bus Transceivers: [SN65HVD230](https://www.ti.com/lit/ds/symlink/sn65hvd230.pdf?ts=1628607753622)

Other useful datasheet for understanding the circuit design:
* [bq76200 Beyond the Simple Application Schematic]()
* [bq769x0 Family Top 10 Design Considerations]()
* [bq769x0 BMS Configurations for Cordless Appliances]()
* []()
* []()
* []()


## Analog Front End (AFE)

## I2C Communication

## Analog to Digital Converter (ADC)


# Concept of Software Part

## Enviroment and Library (datasheet)
* Code in C++
* Code Composer Studio with setting of `TM4C123GH6PM` microcontroller for flashing
* TivaWare Peripheral Driver Library





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

