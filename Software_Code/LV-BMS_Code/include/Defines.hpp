/*
 * Defines.hpp
 *
 *  Created on: Feb 01, 2021
 *      Author: Dennis
 */

#ifndef INCLUDE_DEFINES_HPP_
#define INCLUDE_DEFINES_HPP_

// Voltage limit in mV or s
#define OVERVOLTAGE_SHUTDOWN                    42500   // max cell voltage 4,25 V
#define UNDERVOLTAGE_SHUTDOWN                   3000    // min cell voltage 3 V
#define OVER_TOTAL_VOLTAGE_SHUTDOWN             18000   // max total voltage 18 V
#define UNDER_TOTAL_VOLTAGE_SHUTDOWN            12000   // min total voltage 12 V
#define UNDERVOLTAGE_DELAY                      2       // delay 2 s
#define OVERVOLTAGE_DELAY                       2       // delay 2 s

// Current limit in mA or us
#define OVERCURRENT_SHUTDOWN                    30000   // max current 30 A
#define SHORTCIRCUIT_SHUTDOWN                   30000   // max shortcircuit current 30 A
#define OVERCURRENT_DELAY                       320     // delay 320 us
#define SHORTCIRCUIT_DELAY                      200     // delay 200 us

// Temperature limit in ¢XC*10
#define MAX_CELL_TMEPERATURE                    650     // max cell Temperature 65 Grad
#define MAX_FET_TEMPERATURE                     1000    // max FET Temperature 100 Grad
#define MAX_AFE_TEMPERATURE_CHG                 800     // max IC (AFE) charge temperature 80 grad
#define MAX_AFE_TEMPERATURE_DSG                 800     // max IC (AFE) discharge temperature 80 grad
#define MIN_AFE_TEMPERATURE_CHG                 0       // min IC (AFE) charge temperature 0 grad
#define MIN_AFE_TEMPERATURE_DSG                 0       // min IC (AFE) discharge temperature 0 grad

/*
// Balancing limit
#define OVER_BALANCING_TEMPERATURE_SHUTDOWN     500     // 50 Grad AFE-Chip Übertemperaturabschaltung beim Balancen
#define OVER_BALANCING_TEMPERATURE_WARNING      480     // 48 Grad AFE-Chip Übertemperaturwarnung beim Balancen
#define OVER_CELL_DIFFERENCE_SHUTDOWN           5000    // Wird nicht verwendet
#define OVER_CELL_DIFFERENCE_WARNING            2000    // Wird nicht verwendet

#define BALANCE_RESTRICT                        29800   // Nur wenn die niedrigste Spannung da drüber ist, lässt sich das Balancen starten
#define BALANCING_CYCLE_TIME_SEC                10

#define CAN_MESSAGE_BALANCE_REQUEST_BIT       0
#define CAN_MESSAGE_SHUTDOWN_BIT              1
#define CAN_MESSAGE_VOLTAGES              0x701
#define CAN_MESSAGE_TEMP_AFE              0x704
#define CAN_MESSAGE_TEMP_CELL             0x70C
#define CAN_MESSAGE_STATUS                0x709
#define CAN_MESSAGE_STACK_INFO            0x70A
#define CAN_MESSAGE_ERROR                 0x708
#define CAN_MESSAGE_Control_LVBMS_STATUS  0x705
#define CAN_MESSAGE_CONTROL_AFE_IN        0x70B
*/



#endif /* INCLUDE_DEFINES_HPP_ */
