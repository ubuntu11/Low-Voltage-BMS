/*
 * CANDriver.hpp
 */



#ifndef CANDRIVER_HPP_
#define CANDRIVER_HPP_

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "inc/hw_ints.h"
#include "inc/hw_gpio.h"
#include "inc/hw_types.h"
#include "inc/hw_can.h"
#include "inc/hw_memmap.h"

#include "driverlib/can.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/pin_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/rom_map.h"


typedef enum {
    PF0 = 1,
    PB4 = 2,
    PE4 = 3,
}CAN0Rx;

typedef enum {
    PF3 = 4,
    PB5 = 5,
    PE5 = 6,
}CAN0Tx;

typedef enum {
    PA0 = GPIO_PIN_0,
}CAN1Rx;

typedef enum {
    PA1 = GPIO_PIN_1,
}CAN1Tx;

typedef enum {
    CAN0 = CAN0_BASE,
    CAN1 = CAN1_BASE,
}CAN_BASE;

//! CAN Treiber \class CANDriver CANDriver.hpp "CAN/CANDriver.hpp"
/*! Diese Klasse kann als Treiber fuer die CAN Kommunikation in Verbindung mit dem TM4C123GH6PM genutzt werden. Es stehen 32 Postboxen zur Verfuegung. Von dieser Klasse sollte immer eine
Tochterklasse erstellt werden und damit gearbeitet werden. Bitte diese Klasse nicht editieren oder nur nach Absprache mit STL.  */
class CANDriver {
public:
    CANDriver();
//    virtual ~CANDriver() = 0;
    //! CAN0 Initialisierungsfunktion
    /*!Initialisiert den Treiber fuer die uebergebenen Pins und Baudrate.
    Die Pins werden komplett konfiguriert, ggf. Hardware entlockt. Es wird automatisch an der Auswahl der Pins erkannt,
    welcher der beiden internen CAN-Module genutzt wird.
    */
    void init(CAN0Rx rx_pin, CAN0Tx tx_pin, uint32_t baudrate);
    //! CAN1 Initialisierungsfunktion
    /*!Initialisiert den Treiber fuer die uebergebenen Pins und Baudrate
    Die Pins werden komplett konfiguriert, ggf. Hardware entlockt. Es wird automatisch an der Auswahl der Pins erkannt,
    welcher der beiden internen CAN-Module genutzt wird.
    */
    void init(CAN1Rx rx_pin, CAN1Tx tx_pin, uint32_t baudrate);
    //! Ueberprueft den Status des Treibers
    /*! Ueberprueft ob Fehler vorliegen und ob das Modul in den passiven Modus gewechselt ist(es koennen keine CAN Nachrichten
    mehr empfangen und gesendet werden). Liegt dieser Fall vor, wird automatisch wieder in den aktiven Modus gewechselt. Sollte
    immer vor getMessageInterruptMask() aufgerufen werden.
    */
    bool checkStatusInterrupts(void);
    //!Checkt ob neue Nachrichten empfangen wurden
    /*!Gibt eine Mask zur�ck, die angibt, in welcher Postbox ein Interrupt aufgetreten ist. Diese Nachricht kann danach mit
    getMessage() gelesen werden. Zuvor sollte immer checkStatusInterrupts() ausgefuehrt werden.
    */
    uint32_t getMessageInterruptMask(void);
    //!Gibt Inhalt der als Parameter uebergebenen Postbox zurueck
    void getMessage(uint32_t postbox); // Holt die CAN Message ab und gibt einen Pointer auf die CAN Message zur�ck

protected:
    tCANMsgObject txMessage;
    CAN_BASE can_base;
    tCANMsgObject rxMessage;
    //!Funktion zum senden von CAN Nachrichten
    /*!Mit dieser Funktion kann eine Nachricht in eine Postbox platziert werden, die dann automatisch versendet wird.
    Wenn moeglich, sollte eine Postbox jeweils nur fuer eine Nachricht genutzt werden, um nicht gewuenschte Ueberschreibungen
    in der Postbox zu vermeiden. Auch darf keine Postbox gewaehlt werden, die bereits zum Empfangen von Nachrichten genutzt wird.
    Insgesamt gibt es 32 Postboxen.
    */
    void sendCANMessage (tCANMsgObject* txCANMsg, uint32_t postbox);
    //!Registrieren der CAN Nachrichten, die gelesen werden sollen
    /*!Diese Funktion ist hier leer und sollte zur Benutzung in der Tochterklasse ueberschrieben werden. Pro Nachricht
    die empfangen werden soll wird eine Postbox benoetigt. Insgesamt gibt es 32 Postboxen. Es kann keine Postbox zum Senden
    und Empfangen genutzt werden.
    */
//    virtual void registerReceiveMessages(void);
    uint8_t CANReceive[8]; // Array in dem der Inhalt der CAN Nachrichten gespeichert wird
private:
    bool canActive;
    void enableCan(uint32_t baudrate);
};



#endif /* CANDRIVER_HPP_ */
