/*
 * TestCAN.hpp
 */

#ifndef INCLUDE_TESTCAN_HPP_
#define INCLUDE_TESTCAN_HPP_

#include <include/CANDriver.hpp>
#include <include/ADC.hpp>
#include <include/I2C.hpp>

// #include <include/BMS.hpp>
//#include "CANDriver.hpp"
//#include "BMS.hpp"

typedef enum {

    //TX
    C1_LV_Temp_Postbox = 1,
    C1_IU_Postbox = 2,
    C2_LV_Voltage_Postbox = 3,
    C2_LV_Status_and_Errors_Postbox = 4,
    C2_LV_Cell_Status_Postbox = 5,
    C2_LV_AFE_Overview_Postbox = 6,
    //RX
    C2_LV_Control_Postbox = 7,
    C2_LV_Control_AFE_Postbox = 8,

    C1_MLX9 = 9,
    C1_MLX10 = 10,
    C1_MLX11 = 11,
    C1_MLX12 = 12,
    C1_MLX13 = 13,
    C1_MLX14 = 14,
    C1_MLX15 = 15,
    C1_MLX16 = 16,
}CANPost;
typedef enum {
    //Tx
    C1_LV_Temperature = 0x70C,
    C2_LV_Voltages = 0x701,
    C2_LV_Status_and_Errors = 0x708,
    C2_LV_Cell_Status = 0x709,
    C2_LV_AFE_Overview = 0x70A,
    C1_IU = 0x1C3,
    //Rx
    C2_LVBMS_Control = 0x705,
    C2_LVBMS_Control_AFE = 0x70B
}CANMsgID;

class CAN : public CANDriver{
public:
    CAN();
    void sendTest(void);
    void Initerror();
    void sendTemperature(uint8_t* TempBuffer);
    void sendVoltages(unsigned int* voltage_buffer);
//    void sendStackInfo(BatteryVoltageStack stack_voltage_current);
//    void sendStatusAndErrors(BatteryWarningsAndFaults errors_stack);
    void SendStatus(uint8_t cell0Status,uint8_t cell1Status,uint8_t cell2Status,uint8_t cell3Status,bool chargeMos, bool dischargeMos);
    void registerReceiveMessages(void);
    bool checkIfMessageGot(uint8_t Postbox);
    tCANMsgObject* GetMessage(uint32_t postbox);

private:
    tCANMsgObject test_msg;
    tCANMsgObject lv_temp_msg;
    tCANMsgObject lv_voltage_msg;
    tCANMsgObject lv_stack_info_msg;
    tCANMsgObject lv_errors_status_msg;
    tCANMsgObject lv_cell_status_msg;

};




#endif /* INCLUDE_TESTCAN_HPP_ */
