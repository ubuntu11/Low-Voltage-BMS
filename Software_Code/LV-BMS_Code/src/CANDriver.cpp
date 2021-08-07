/*
 * CANDriver.cpp
 */

#include <include/CANDriver.hpp>

CANDriver::CANDriver() {
    //  Auto-generated constructor stub

}

//CANDriver::~CANDriver() {
//    // Auto-generated destructor stub
//}

void CANDriver::init(CAN1Rx rx_pin, CAN1Tx tx_pin, uint32_t baudrate){
    // Aktiviert Can1 mit den Pins PA0 und PA1 (keine anderen Moeglich)
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    GPIOPinConfigure(GPIO_PA0_CAN1RX);
    GPIOPinConfigure(GPIO_PA1_CAN1TX);
    GPIOPinTypeCAN(GPIO_PORTA_BASE, rx_pin | tx_pin);
    can_base = CAN1;
    enableCan(baudrate);
    //txMessage.ui32Flags = MSG_OBJ_TX_INT_ENABLE;
    //registerReceiveMessages();
}
void CANDriver::init(CAN0Rx rx_pin, CAN0Tx tx_pin, uint32_t baudrate){
    //Aktiviert Can0 mit den entsprechend definierten Pins
    if ((rx_pin == PF0)||(tx_pin == PF3)){
        SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
        HWREG(GPIO_PORTF_BASE + GPIO_O_LOCK) = GPIO_LOCK_KEY; // Unlock CAN 0 on Port F
        HWREG(GPIO_PORTF_BASE + GPIO_O_CR) = 0x1;
        if (rx_pin == PF0){
            GPIOPinConfigure(GPIO_PF0_CAN0RX);
            GPIOPinTypeCAN(GPIO_PORTF_BASE, GPIO_PIN_0);
        }
        if (tx_pin == PF3){
            GPIOPinConfigure(GPIO_PF3_CAN0TX);
            GPIOPinTypeCAN(GPIO_PORTF_BASE, GPIO_PIN_3);
        }
    }
    if ((rx_pin == PB4)||(tx_pin == PB5)){
        SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
        if (rx_pin == PB4){
            GPIOPinConfigure(GPIO_PB4_CAN0RX);
            GPIOPinTypeCAN(GPIO_PORTB_BASE, GPIO_PIN_4);
        }
        if (tx_pin == PB5){
            GPIOPinConfigure(GPIO_PB5_CAN0TX);
            GPIOPinTypeCAN(GPIO_PORTB_BASE, GPIO_PIN_5);
        }
    }
    if ((rx_pin == PE4)||(tx_pin == PE5)){
        SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
        if (rx_pin == PE4){
            GPIOPinConfigure(GPIO_PE4_CAN0RX);
            GPIOPinTypeCAN(GPIO_PORTE_BASE, GPIO_PIN_4);
        }
        if (tx_pin == PE5){
            GPIOPinConfigure(GPIO_PE5_CAN0TX);
            GPIOPinTypeCAN(GPIO_PORTE_BASE, GPIO_PIN_5);
        }
    }
    can_base = CAN0;
    enableCan(baudrate);
    //txMessage.ui32Flags = MSG_OBJ_TX_INT_ENABLE;
    //registerReceiveMessages();
}

void CANDriver::enableCan(uint32_t baudrate){
    if (can_base == CAN0){
        SysCtlPeripheralEnable(SYSCTL_PERIPH_CAN0);
    }
    else if (can_base == CAN1){
        SysCtlPeripheralEnable(SYSCTL_PERIPH_CAN1);
    }
    CANInit(can_base);
    CANBitRateSet(can_base, SysCtlClockGet(), baudrate);
    CANIntEnable(can_base, CAN_INT_MASTER | CAN_INT_STATUS | CAN_INT_ERROR);
    //  IntEnable(INT_CAN1);
    CANEnable(can_base);
    canActive = true;
    txMessage.ui32MsgIDMask = 0;
    //txMessage.ui32Flags = MSG_OBJ_TX_INT_ENABLE;
    txMessage.ui32Flags = 0;
}

void CANDriver::sendCANMessage (tCANMsgObject* txCANMsg, uint32_t postbox) {
    if (canActive){ // nur wenn CAN im aktiven Zustand ist senden
        CANMessageSet(can_base, postbox, txCANMsg, MSG_OBJ_TYPE_TX);
    }
}


bool CANDriver::checkStatusInterrupts(void){
    // Überprüft ob Statusinterrupts vorliegen und cleart diese

    uint32_t ui32Status;
    ui32Status = CANIntStatus(can_base, CAN_INT_STS_CAUSE);
    if (ui32Status == CAN_INT_INTID_STATUS) {
        ui32Status = CANStatusGet(can_base, CAN_STS_CONTROL); // durch Abfrage wird der Interrupt automatisch gecleart.
        // Hier falls nötig den Statusinterrupt auswerten
        if (ui32Status & CAN_STATUS_BUS_OFF) {
                 // Bus off
            // Enable the controller again to allow it to start decrementing the
            // error counter allowing the bus off condition to clear.
            //
        //  canActive = false; -> weiterhin senden, sonst wird der Error Counter nicht herabgezählt
            CANEnable(can_base);
            //
            // Wait for the bus off condition to clear. This condition can be
            // polled elsewhere depending on the application. But no CAN messages
            // can be sent until this condition clears.
            //
//          while(CANStatusGet(can_base) & CAN_STATUS_BUS_OFF)
//          {
//          }
        } else { // CAN Bus nicht off
            if (ui32Status & CAN_STATUS_EPASS) {
                // CAN controller is in the error passive state
        //      canActive = false;
                CANEnable(can_base); // Versuch wieder zu aktivieren
            } else { // CAN Controller nicht im Zustand Bus off oder Passive
                canActive = true;
                if (ui32Status & CAN_STATUS_EWARN) {
                    // an error counter has reached a limit of at least 96
                }
            }

        }
    }
    else{
        canActive = true;
    }
    return canActive;
}

uint32_t CANDriver::getMessageInterruptMask(void){
    // Gibt eine Mask zurück, die angibt, in welcher Postbox ein Interrupt aufgetreten ist
    // zuvor CheckStatusInterrupt aufrufen
    return CANIntStatus(can_base, CAN_INT_STS_OBJECT);
}




void CANDriver::getMessage(uint32_t postbox){
    // Holt die CAN Message von der angegebenen Postbox ab
    rxMessage.pui8MsgData = CANReceive;
    CANMessageGet(can_base, postbox, &rxMessage, 0);
    CANIntClear(can_base, postbox); // Der Interrupt kann auch automatisch gecleared werden, indem bool bClrPendingInt in CANMessageGet auf true gesetzt wird
}

//void CANDriver::registerReceiveMessages(void){
//
//}

