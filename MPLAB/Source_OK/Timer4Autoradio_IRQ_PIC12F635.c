/*
 * File:
 * Author: Loreto
 *
 * Created on 24 febbraio 2014, 8.16
 */

#include <xc.h>
#include <Timer4Autoradio_PIC12F635.h>



//############################################################################################
// - Interrupt Routine
//      se non si legge la porta non si ripristina il flag RAIF
//############################################################################################
void interrupt IrqHandler(void) {

    // if ( INTCONbits.RAIE && INTCONbits.RAIF )  {
    if ( INTCONbits.RAIF )  {
        portAValue0.byte = PORTA;              // se non si legge la porta non si ripristina il flag RBIF

        if (portAValue0.V12_CMD_PIN == 1) {
            mySTATUS.V12Volt_ON = TRUE;
            LED_PIN  = 1;
            ALARM_PIN = 1;
            mSecDELAY(4000);
            LED_PIN  = 0;
            ALARM_PIN = 0;
        }
        else {
            mySTATUS.V12Volt_ON = FALSE;
            LED_PIN  = 1;
            ALARM_PIN = 1;
            mSecDELAY(1000);
            LED_PIN  = 0;
            ALARM_PIN = 0;
        }


        INTCONbits.RAIF = 0;
    }
    // if ( INTCONbits.INTE && INTCONbits.INTF )  {
    else if ( INTCONbits.INTF )  {
        portAValue0.byte = PORTA;              // se non si legge la porta non si ripristina il flag RBIF
        LED_PIN  = 1;
        ALARM_PIN = 1;
        mSecDELAY(500);
        ALARM_PIN = 0;
        LED_PIN  = 0;

        INTCONbits.INTF = 0;
    }

    else if ( INTCONbits.T0IE && INTCONbits.T0IF )  {
        toggle_pin( LED_PIN );
        mSecDELAY(1000);
        toggle_pin( LED_PIN );
        mSecDELAY(1000);

        TMR0 = TMR0_Value;              // Ricarichiamo il TMR0
        INTCONbits.T0IF = 0;
    }
}





//############################################################################################
// # void interrrupt_RAIE_SetUp()
//      OCB – PORTA INTERRUPT-ON-CHANGE REGISTER
//############################################################################################
void setup_interrrupt_OnChange() {
    INTCONbits.RAIF         = 0;          // Change Interrupt Flag bit
    INTCONbits.RAIE         = 1;          // Change Interrupt Enable bit
}


//############################################################################################
// # void interrrupt_INT_SetUp()
//      External Interrupt
//############################################################################################

void setup_interrupt_INT0() {
    // IOCAbits.IOCA2          = DISABLE_0;          // 1=Interrupt-on-change enabled
    // INT0_PIN_IO             = INPUT;            // 1=set as input
    // ALARM_PIN_IO            = OUTPUT;            // 1=set as input
    // LED_PIN_IO              = OUTPUT;         // 1=set as input
    OPTION_REGbits.INTEDG   = FAILING_EDGE;     //cause interrupt at falling edge

    INTCONbits.INTF         = OFF;          // PORTA Edge Interrupt Flag bit
    INTCONbits.INTE         = ON;          // PORTA Edge Interrupt Enable bit
}