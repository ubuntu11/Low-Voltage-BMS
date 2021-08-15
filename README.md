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
* [Development of Hardware Part](https://github.com/PingCheng-Wei/Low-Voltage-BMS#development-of-hardware-part)
    * [Components & Datasheets](https://github.com/PingCheng-Wei/Low-Voltage-BMS#components--datasheets)
    * [Explaination of the Functionality in each Part](https://github.com/PingCheng-Wei/Low-Voltage-BMS#explaination-of-the-functionality-in-each-part)
        * [Analog Front End](https://github.com/PingCheng-Wei/Low-Voltage-BMS#analog-front-end-afe)
        * [I2C Communication](https://github.com/PingCheng-Wei/Low-Voltage-BMS#i2c-communication)
        * [FET Driver](https://github.com/PingCheng-Wei/Low-Voltage-BMS#fet-driver)
        * [Analog to Digital Converter](https://github.com/PingCheng-Wei/Low-Voltage-BMS#analog-to-digital-converter-adc)
    * [Altium PCB Design](https://github.com/PingCheng-Wei/Low-Voltage-BMS#altium-pcb-design)
* [Concept of Software Part](https://github.com/PingCheng-Wei/Low-Voltage-BMS#concept-of-software-part)
    * [Enviroment and Library](https://github.com/PingCheng-Wei/Low-Voltage-BMS#enviroment-and-library-datasheet)
* [Installation](https://github.com/PingCheng-Wei/Low-Voltage-BMS#installation)


# Concept and Functionality of the LV-BMS
The main idea that we need a battery management system is that lithium battery has a higher energy density, higher efficiency and lighter weight in comparison with other art of battery such as LiFePo4, which is crucial in our case to gain better performence, but it could also be life-threatening when something goes wrong. Therefore, we need a monitoring system, BMS, to keep supervising the status of the lithium battery stack during the execution of whole system and take defensive action if it is needed. The status at here means the `voltages`, `current` and `temperatures` of the battery stack. To be more specific, voltages could be the `cell voltages` as well as the `entire voltage` and current is the `entire current` that flows out the battery stack and temperatures could be the `cell temperatures` as well as the `MOSFETs' temperatures`. 

BMS monitor these three status to make sure they are all operating in the acceptable range. if not, then BMS will cut off the power supply of the whole system (LV-Battery) by switching the MOSFETs to off so that no further damage could happen and if the stutas are back to normal again, then BMS will turn on the power suppy again.


# Development of Hardware Part
The general structure of the hardware PCB design is based on the following functional parts:

* `Analog Front End (AFE)`, which is mainly responsible for collecting the parameters of the battery pack such as voltages, current as well as temperatures by using internal ADC measures. It could also control the current between the cells in order to avoid overloading and underloading, which is called cell-balancing. At here we pick [BQ76920](https://www.ti.com/lit/ds/symlink/bq76920.pdf?ts=1628676283263&ref_url=https%253A%252F%252Fwww.ti.com%252Fproduct%252FBQ76920) from TI as our AFE since it has not only all the basic features that we need but also some other extra features e.g. hardware protection features, charge/discharge low-side NCH FET drivers, alert interrupt to host microcontroller, simple I2C compatible interface (CRC option)

* `Inter-Integrated Circuit (I2C) Interface`, which is really crucial to realize the communication between our host microcontroller (uC), `TM4C123GH6PM`, and AFE ,`BQ76920`. Through the I2C communication, uC can read value from AFE register, send commmed to AFE or even write value into the register in AFE and AFE can also sent alert to uC so that uC could take an action to deal with the errors

* `Microcontroller (uC)`, which is like the brain of the system and where we flash our code into in order to realize all the functions that we need. Here is `TM4C123GH6PM` chosen as our uC because it has lots of features to utilize and there is also a compatible [Peripheral Driver Library](https://www.ti.com/lit/ug/spmu298e/spmu298e.pdf?ts=1628540888902&ref_url=https%253A%252F%252Fwww.google.com%252F) on hand.

* `FET Driver`, which is needed if we want to have a high-side switch control feature according to our AFE, BQ76920, and provides the advantages of avoiding ground disconnection in the system and continuous communication between the battery pack and host system. Here [BQ76200](https://www.ti.com/lit/ds/symlink/bq76200.pdf?ts=1629016585303&ref_url=https%253A%252F%252Fwww.google.com%252F) is chosen as it is directly competible and circuit design is also provide in both [BQ76200](https://www.ti.com/lit/ds/symlink/bq76200.pdf?ts=1629016585303&ref_url=https%253A%252F%252Fwww.google.com%252F) and [BQ76920](https://www.ti.com/lit/ds/symlink/bq76920.pdf?ts=1628676283263&ref_url=https%253A%252F%252Fwww.ti.com%252Fproduct%252FBQ76920) datasheet.

    Here is an overview of how BQ and uC structured:
    
    ![overview of BQ-chip](https://github.com/PingCheng-Wei/Low-Voltage-BMS/blob/main/assets/LV_structure.png)

Other feature that we need for our autonomous BEV and self-driving car:
* `CAN Bus system`, which is a communication system between our uC and centeral `main control unit (MCU)` in our cars so that MCU could know what is going on right now in LV-system and whether every thing is fine. For some cases, MCU could even take some preventative steps to protect the car if something in LV-system goes wrong.
* `Analog to digital converter (ADC) in uC`, which is exploited for more temperatures monitoring such as Battery stack temperatures and MOSFETs' temperatures.


## Components & Datasheets
You could find all the components and their footprints that are utilized on the PCB in the [Bill of Materials (BOM)](https://github.com/PingCheng-Wei/Low-Voltage-BMS/blob/main/assets/BOM_LV_21E.xlsx). Here we just list the most important components:

* Microcontroller: [TM4C123GH6PM](https://www.ti.com/lit/ds/spms376e/spms376e.pdf?ts=1628616879792&ref_url=https%253A%252F%252Fwww.google.com%252F)
* Analog Front End: [BQ76920](https://www.ti.com/lit/ds/symlink/bq76920.pdf?ts=1628676283263&ref_url=https%253A%252F%252Fwww.ti.com%252Fproduct%252FBQ76920) without CRC
* FET Driver: [BQ76200](https://www.ti.com/lit/ds/symlink/bq76200.pdf?ts=1629016585303&ref_url=https%253A%252F%252Fwww.google.com%252F)
* MOSFETs: [AUIRFS8409-7P](https://www.infineon.com/dgdl/auirfs8409-7p.pdf?fileId=5546d462533600a4015355b745e314f0)
* 3.3V CAN Bus Transceivers: [SN65HVD230](https://www.ti.com/lit/ds/symlink/sn65hvd230.pdf?ts=1628607753622)

Other useful datasheets for more understanding of the circuit design:
* BQ relevant:
    * [bq76200 Beyond the Simple Application Schematic](https://www.ti.com/lit/an/slua794/slua794.pdf?ts=1629017183044&ref_url=https%253A%252F%252Fwww.google.com%252F)
    * [FET Configurations for the bq76200 High-Side N-Channel FET Driver](https://www.ti.com/lit/an/slva729a/slva729a.pdf?ts=1629017239970&ref_url=https%253A%252F%252Fwww.google.com%252F)
    * [bq769x0 Family Top 10 Design Considerations](https://www.ti.com/lit/an/slua749a/slua749a.pdf?ts=1629017197968&ref_url=https%253A%252F%252Fwww.google.com%252F)
    * [10s battery monitoring, balancing, and comp protection, 50A discharge ref design](https://www.ti.com/lit/ug/tiduar8c/tiduar8c.pdf?ts=1629017610394&ref_url=https%253A%252F%252Fwww.google.com%252F)
    * [bq769x0 BMS Configurations for Cordless Appliances](https://www.ti.com/lit/an/slua810/slua810.pdf?ts=1628994785378)

* I2C relevant:
    * [Understanding the I2C Bus](https://www.ti.com/lit/an/slva704/slva704.pdf?ts=1629009658472&ref_url=https%253A%252F%252Fwww.google.com%252F)
    * [I2C Communication Sample Code for the bq76940 with a CRC Option Based on the MSP430G2553](https://www.ti.com/lit/an/slva626b/slva626b.pdf?ts=1629017637344&ref_url=https%253A%252F%252Fwww.google.com%252F)

* Microcontroller relevant:
    * [MCU_tm4c123gh6pm](https://www.ti.com/lit/ds/spms376e/spms376e.pdf?ts=1628616879792&ref_url=https%253A%252F%252Fwww.google.com%252F)
    * [TivaWare Peripheral Driver Library Users's Guide](https://www.ti.com/lit/ug/spmu298e/spmu298e.pdf?ts=1628540888902&ref_url=https%253A%252F%252Fwww.google.com%252F)

## Explaination of the Functionality in each Part
### Analog Front End (AFE)
As described above, we use an AFE, BQ76920, to realize the parameters monitoring and MOSFETs control. Here we would like to explain more about the funtionality behind `BQ76920`. It is also helpful to go deep into datasheet as it not only provides you a better overview of hardware but also better understanding of the algorithm in software code.

BQ76920 is a pre-programmed chips that already has the function like voltages, current and temperatures measurement and it would automatically keep monitoring/update the value in a certain amount of time. If something goes wrong, the internal system status will be set to `bit 0` and automatically disable the charge and discharge control (CHG & DSG Pins set to 0). Therefore, in order to use BQ76920, it is rather important to grasp the concept of how to let microcontroller communicate with the BQ's register and control the BQ's status through I2C than understand how internal measurement works. here is an important system status of BQ76920 in register:
![status in register map](https://github.com/PingCheng-Wei/Low-Voltage-BMS/blob/main/assets/registermap_stat_ctrl2.jpg)

Take cell voltages measurement for example, `it update the cell voltages at a rate of 250 ms` and store the value inside its register for other functions utiliztion such as overvoltage and undervoltage check. This means that if BQ76920 updates the voltage and finds out they are out of the limit range, it will internally set the corresponding system status to 0 (means error) and since system status are not all 1, it will then automatically turn off the power supply by shuting down the CHG or DSG Pin to bit 0, which is bit 1 (means on) when everything is fine, and send a alert signal to our microcontroller. This all happen in BQ internally by itself. Thus, you might wonder why we need a microcontroller then if everything works internally. The reason is that we need a microcontroller to clear the alert/error bit. By that it means that microcontroller will keep demanding the BQ to update its system status, in this case keep measuring the cell voltages, and until the voltages are back into acceptable range microcontroller can then successfully clear the alert/error bit and BQ would then allow the charge or discharge control by setting CHG or DSG Pin to bit 1.

### I2C Communication
In order to grasp the deep understanding of how I2C works, it is highly recommanded that you go through and deep into this article "[Understanding the I2C Bus](https://www.ti.com/lit/an/slva704/slva704.pdf?ts=1629009658472&ref_url=https%253A%252F%252Fwww.google.com%252F)". In the article you should understand how I2C recognize the start as well as end bit from SCL (Serial Clock) & SDA (Serial Data) and how the signals are read. Here we would like to show some I2C signal from Oscilloscope:

* overview:
    ![oszi_i2c_overview](https://github.com/PingCheng-Wei/Low-Voltage-BMS/blob/main/assets/oszi_i2c_ueberblick.jpeg)
* I2C Signal:
    ![oszi_i2c_signal](https://github.com/PingCheng-Wei/Low-Voltage-BMS/blob/main/assets/oszi_i2c.jpeg)

### FET Driver
As mentioned above, BQ76920 itself only support the low-side switch feature. However, in some cases it would benefit more if we would able to use high-side switch. That is why we need a FET Driver to convert the signal of CHG/DSG from BQ76920 to higher voltage. The idea behind [BQ76200](https://www.ti.com/lit/ds/symlink/bq76200.pdf?ts=1629016585303&ref_url=https%253A%252F%252Fwww.google.com%252F) is using a `charge pump capacitor`, which works like an extra small battery connected in series with low-voltage battery, as you could see in the circuit example `between the VDDCP and BAT pins`. During the operation the charge pump capacitor will keep charging and discharging process to store and release the energy. By doing so we could raise our gate-voltage of MOSFETs to surpass the source-voltage of MOSFETs, which is same as low-voltage battery voltage, and achieve a positive gate-source-voltage to turn on the high-side MOSFETs. 

Here is how the voltage from CHG and DSG changes after the FET driver:
![oszi_chg_dsg](https://github.com/PingCheng-Wei/Low-Voltage-BMS/blob/main/assets/oszi_chg_dsg.JPG)

It is also interesting to see the charging and discharging process in charge pump capacitor:
![oszi_vddcp](https://github.com/PingCheng-Wei/Low-Voltage-BMS/blob/main/assets/oszi_vddcp.jpeg)

If everything works fine the `gate-source-voltage` of the MOSFETs would looks like this:
![oszi_vgs](https://github.com/PingCheng-Wei/Low-Voltage-BMS/blob/main/assets/oszi_vgs.jpeg)

`Here is the capacity of the charge pump capacitor very critical !!!` If the value is too small, then the capacitor can't store enough energy and it will soon be all released, which leads to just some peaks of higher voltage as the images below show:

![chg_dsg_bq_driver](https://github.com/PingCheng-Wei/Low-Voltage-BMS/blob/main/assets/chg_dsg_bq_driver.JPG)

If the capacity is too big, then the capacitor does not have enough time to reach the needed energy since the charging rate is too low. Therefore, `after several testing we have chosen $2.2 uF$ instead of $470nF$ according to datasheet`.

### Analog to Digital Converter (ADC)
Since the microcontroller can't understand the analog value such as 3.3V or 1A and its internal measurement only send back a bit by bit signal e.g. in a range of 0 ~ 4096 corresponding to 12 bits signal, we need to convert/calculate this bit by bit signal back to the corresponding analog value range and we will recieve the so called digital value. Here is the general equation of ADC:
    ![ADC_equation](https://github.com/PingCheng-Wei/Low-Voltage-BMS/blob/main/assets/ADC_equation.png)

For example if we have the following situation:
* ADC signal bit length: 12 bits
* Voltage range: 0 ~ 4.3V
* ADC measured value: 3000 

then our converted digital value would be:
    ![ADC_calculation](https://github.com/PingCheng-Wei/Low-Voltage-BMS/blob/main/assets/ADC_calculation.png)

which means the measured ADC valus 3000 is corresponding to 3.15V.


## Altium PCB Design
overview of each function part of the PCB design based on LV-BMS version 1:
![overview of PCB design](https://github.com/PingCheng-Wei/Low-Voltage-BMS/blob/main/assets/overview.JPG)

Polygons and netz distribution based on LV-BMS version 1:
| Net von Polygons | Analog Part  | Digital Part  | Power Part      |
| ---------------- | -----------  | ------------  | -------------   |
| Top Layer        | GND          | GND           | (komplizierter) |
| GND Layer        | GND          | GND           | GND             |
| VCC Layer        | V_REF (3.0V) | +3.3V         | Bat +           |
| Bottom Layer     | GND          | GND           | GND             |

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

