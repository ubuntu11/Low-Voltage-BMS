/*
 * TestCAN.cpp
 *
 *  Created on: 15 Mar 2019
 *
 */


#include <include/TestCAN.hpp>

CAN::CAN(){

    lv_temp_msg.ui32MsgID = C1_LV_Temperature;
    lv_temp_msg.ui32MsgIDMask = 0;
    lv_temp_msg.ui32Flags = MSG_OBJ_NO_FLAGS;
    lv_temp_msg.ui32MsgLen = 8;

    lv_voltage_msg.ui32MsgID = C2_LV_Voltages;
    lv_voltage_msg.ui32MsgIDMask = 0;
    lv_voltage_msg.ui32Flags = MSG_OBJ_NO_FLAGS;
    lv_voltage_msg.ui32MsgLen = 8;

    lv_errors_status_msg.ui32MsgID =  C2_LV_Status_and_Errors;
    lv_errors_status_msg.ui32MsgIDMask = 0;
    lv_errors_status_msg.ui32Flags = MSG_OBJ_NO_FLAGS;
    lv_errors_status_msg.ui32MsgLen = 6;

    lv_stack_info_msg.ui32MsgID =  C2_LV_AFE_Overview;
    lv_stack_info_msg.ui32MsgIDMask = 0;
    lv_stack_info_msg.ui32Flags = MSG_OBJ_NO_FLAGS;
    lv_stack_info_msg.ui32MsgLen = 4;

    lv_cell_status_msg.ui32MsgID =  C2_LV_Cell_Status;
    lv_cell_status_msg.ui32MsgIDMask = 0;
    lv_cell_status_msg.ui32Flags = MSG_OBJ_NO_FLAGS;
    lv_cell_status_msg.ui32MsgLen = 3;
}
/*
void CAN::sendTest(){
    uint8_t data[1]={0};
    data[0]=sendTestData[0];
    test_msg.pui8MsgData = data;
    sendCANMessage(&test_msg, C1_IU_Postbox);
}
*/
void CAN::sendTemperature(uint8_t* TempBuffer)
{

    uint8_t sendData[8]= {0};

    sendData[0] |= TempBuffer[0] >> 2;      // stack temperature 1
    sendData[1] |= TempBuffer[0] << 6;      // stack temperature 1

    sendData[1] |= TempBuffer[1] >> 4;      // stack temperature 2
    sendData[2] |= TempBuffer[1] << 4;      // stack temperature 2

    sendData[2] |= TempBuffer[2] >> 6;      // stack temperature 3
    sendData[3] |= TempBuffer[2] << 2;      // stack temperature 3

    sendData[3] |= TempBuffer[3] >> 8;      // discharge MOSFET temperature
    sendData[4] |= TempBuffer[3];           // discharge MOSFET temperature

    sendData[5] |= TempBuffer[4] >> 2;      // charge MOSFET temperature
    sendData[6] |= TempBuffer[4] << 6;      // charge MOSFET temperature

    lv_temp_msg.pui8MsgData = sendData;
    sendCANMessage(&lv_temp_msg , C1_LV_Temp_Postbox);

}

void CAN::sendVoltages(unsigned int* voltage_buffer){

    uint8_t sendData[8]= {0};


    sendData[0] |= voltage_buffer[0] >> 8;      // cell 1
    sendData[1] |= voltage_buffer[0] & 0xff;    // cell 1

    sendData[2] |= voltage_buffer[1] >> 8;      // cell 2
    sendData[3] |= voltage_buffer[1] & 0xff;    // cell 2

    sendData[4] |= voltage_buffer[2] >> 8;      // cell 3
    sendData[5] |= voltage_buffer[2] & 0xff;    // cell 3

    sendData[6] |= voltage_buffer[3] >> 8;      // cell 4
    sendData[7] |= voltage_buffer[3] & 0xff;    // cell 4


    lv_voltage_msg.pui8MsgData = sendData;
    sendCANMessage(&lv_voltage_msg , C2_LV_Voltage_Postbox);
}
/*
void CAN::sendStackInfo(BatteryVoltageStack stack_voltage_current){

    uint8_t sendData[4]= {0};

    sendData[0] |= stack_voltage_current.current >> 8;
    sendData[1] |= stack_voltage_current.current & 0xff;

    sendData[2] |= stack_voltage_current.stackVoltage >> 8;
    sendData[3] |= stack_voltage_current.stackVoltage & 0xff;

    lv_stack_info_msg.pui8MsgData = sendData;
    sendCANMessage(&lv_stack_info_msg , C2_LV_AFE_Overview_Postbox);
}
void CAN::Initerror(){

    uint8_t sendData[4]= {0};
    unsigned int initplatzhalter=0x1B39;
    unsigned int initerror=0x1A0A;

    sendData[0] |= initplatzhalter >>8;
    sendData[1] |= initplatzhalter &0xFF;

    sendData[2] |= initerror >> 8;
    sendData[3] |= initerror & 0xff;

    lv_stack_info_msg.pui8MsgData = sendData;
    sendCANMessage(&lv_stack_info_msg , C2_LV_AFE_Overview_Postbox);
}


void CAN::sendStatusAndErrors(BatteryWarningsAndFaults errors_stack){

    uint8_t sendData[6]= {0};

    sendData[0] |= ((0x01 & errors_stack.NoWarningsAndFaults) |
            (0x02 * errors_stack.OverVoltageCell0) |
            (0x04 * errors_stack.OverVoltageCell1) |
            (0x08 * errors_stack.OverVoltageCell2) |
            (0x10 * errors_stack.OverVoltageCell3) |
            (0x20 * errors_stack.OverVoltageWarningCell0) |
            (0x40 * errors_stack.OverVoltageWarningCell1) |
            (0x80 * errors_stack.OverVoltageWarningCell2));

    sendData[1] |= (0x01 *errors_stack.OverVoltageWarningCell3  |
            0x02 * errors_stack.UnderVoltageCell0  |
            0x04 * errors_stack.UnderVoltageCell1 |
            0x08 * errors_stack.UnderVoltageCell2 |
            0x10 * errors_stack.UnderVoltageCell3 |
            0x20 * errors_stack.UnderVoltageWarningCell0 |
            0x40 * errors_stack.UnderVoltageWarningCell1 |
            0x80 * errors_stack.UnderVoltageWarningCell2 );

    sendData[2] |= (0x01 * errors_stack.UnderVoltageWarningCell3 |
            0x02 * errors_stack.OverTemperature0 |
            0x04 * errors_stack.OverTemperature1 |
            0x08 * errors_stack.OverTemperature2 |
            0x10 * errors_stack.OverTemperatureFet0 |
            0x20 * errors_stack.OverTemperatureFet1 |
            0x40 * errors_stack.OverTemperatureChip |
            0x80 * errors_stack.OverTemperatureWarning0 );

    sendData[3] |= (0x01 * errors_stack.OverTemperatureWarning1 |
            0x02 * errors_stack.OverTemperatureWarning2 |
            0x04 * errors_stack.OverTemperatureFetWarning0 |
            0x08 * errors_stack.OverTemperatureFetWarning1 |
            0x10 * errors_stack.OverTemperatureChipWarning |
            0x20 * errors_stack.OverVoltage |
            0x40 * errors_stack.OverVoltageWarning |
            0x80 * errors_stack.UnderVoltage);

    sendData[4] |= (0x01 * errors_stack.UnderVoltageWarning |
            0x02 * errors_stack.OverCurrent |
            0x04 * errors_stack.OverCurrentWarning |
            0x08 * errors_stack.OverDischargeCurrent |
            0x10 * errors_stack.OverChargeCurrent |
            0x20 * errors_stack.OverCellDifference |
            0x40 * errors_stack.OverCellDifferenceWarning);

    sendData[5] |= (0x01 * errors_stack.AfeUnderVoltageBit |
            0x02 * errors_stack.AfeOverVoltageBit |
            0x04 * errors_stack.AfeSCDBit |
            0x08 * errors_stack.AfeOverCurrentBit |
            0x10 * errors_stack.OverChargeCurrent);

    lv_errors_status_msg.pui8MsgData = sendData;
    sendCANMessage(&lv_errors_status_msg , C2_LV_Status_and_Errors_Postbox);
}

void CAN::SendStatus(uint8_t cell0Status,uint8_t cell1Status,uint8_t cell2Status,uint8_t cell3Status,bool chargeMos,bool dischargeMos){

     uint8_t sendData[3]= {0};

     sendData[0] |=(cell0Status | cell1Status << 4 );

     sendData[1] |=(cell2Status |cell3Status << 4 );

     sendData[2] |=(dischargeMos   | chargeMos << 1);


     lv_cell_status_msg.pui8MsgData = sendData;
     sendCANMessage(&lv_cell_status_msg , C2_LV_Cell_Status_Postbox);

}

void CAN::registerReceiveMessages(void){

    rxMessage.ui32Flags = MSG_OBJ_RX_INT_ENABLE | MSG_OBJ_USE_ID_FILTER;
    rxMessage.ui32MsgIDMask = 0xfffff;

    rxMessage.ui32MsgID = C2_LVBMS_Control_AFE;
    rxMessage.ui32MsgLen = 1;
    CANMessageSet(can_base,C2_LV_Control_AFE_Postbox, &rxMessage, MSG_OBJ_TYPE_RX);
}

tCANMsgObject* CAN::GetMessage(uint32_t postbox){

    // Holt die CAN Message von der angegebenen Postbox ab
    rxMessage.pui8MsgData = CANReceive;
    CANMessageGet(can_base, postbox, &rxMessage, 0);

    CANIntClear(can_base, postbox);

    return &rxMessage;
}

bool CAN::checkIfMessageGot(uint8_t Postbox){
    Postbox -=1;
    uint32_t canInterruptMask;
    canInterruptMask = getMessageInterruptMask();
    return (canInterruptMask & (1<<Postbox));

}
*/
