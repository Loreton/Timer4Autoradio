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
int debounceValue;
    flag.isCarON = LnPORTA.V12_CMD;

    // ===============================================
    // - R A I E
    // - This  interrupt  can  wake  the  device  from  Sleep.
    // - The user, in the Interrupt Service Routine, clears the interrupt by:
    // -     a) Any read or write of PORTA. This will end the mismatch condition, then
    // -     b) Clear the flag bit RAIF.
    // - A mismatch condition will continue to set flag bit RAIF.
    // - Reading PORTA will end the mismatch condition and
    // - allow flag bit RAIF to be cleared.
    // ===============================================
    // if ( INTCONbits.RAIF )  {
    if ( INTCONbits.RAIE && INTCONbits.RAIF )  {
        // flag.isCarON = LnPORTA.V12_CMD;

        INTCONbits.RAIF = 0;
    }

    // ===============================================
    // - I N T 0
    // ===============================================
    // if ( INTCONbits.INTF )  {
    else if ( INTCONbits.INTE && INTCONbits.INTF )  {
        flag.PulsanteHasPushed = ! LnPORTA.PULSANTE;

        if (LnPORTA.PULSANTE == OFF) {            // Se il pulsante è spinto ...
            LnPORTA.LED = ON;

                // debounce by waiting and seeing if still held down
            debounceValue = STOP_Time;        // in mSecondi
            while (debounceValue > 0) {
                if (LnPORTA.PULSANTE == ON) break;
                mSecDELAY(STOP_delayStep);
                toggle_bit(LnPORTA.LED);
                debounceValue -= STOP_delayStep;
            }

                // indica che lo STOP è stato raggiunto

                // --------------------------------
                // - Fine del tempo di stop
                // - Vediamo se si vuole lo START
                // --------------------------------
            if (LnPORTA.PULSANTE == OFF) {
                DESIRED_STATE = FORCED_OFF;
                LnPORTA.LED = OFF;

                mSecDELAY(1000);
                debounceValue = START_Time;        // in mSecondi
                while (debounceValue > 0) {
                    if (LnPORTA.PULSANTE == ON) break;
                    mSecDELAY(START_delayStep);
                    toggle_bit(LnPORTA.LED);
                    debounceValue -= START_delayStep;
                }

                if (LnPORTA.PULSANTE == OFF)      // Se ancora spinto
                    DESIRED_STATE = FORCED_ON;

            }
        }


        LnPORTA.LED = OFF;
        INTCONbits.INTF = 0;
    }

    // ===============================================
    // - T M R 0
    // ===============================================
    // if ( INTCONbits.T0IF )  {
    else if ( INTCONbits.T0IE && INTCONbits.T0IF )  {
        TMR0 = TMR0_Value; // must be initialized/reloaded before clearing T0IF bit.
        TMR0_Counter += 1;  // mi è comodo per verificare la freq del TMR0

        if (LnPORTA.V12_CMD == ON) {
            if (TMR0_Counter > 20) {
                toggle_bit(LnPORTA.LED);  // mi è comodo per verificare la freq del TMR0
                TMR0_Counter = 0;  // mi è comodo per verificare la freq del TMR0
            }
        }
        INTCONbits.T0IF = 0;
    }

}




//############################################################################################
// # void interrrupt_RAIE_SetUp()
//      OCB – PORTA INTERRUPT-ON-CHANGE REGISTER
//############################################################################################
void setup_interrrupt_OnChange() {
    LnIOCA.V12_CMD         = ENABLE;        // interrupt-on-change  [1=enabled 0=disabled]

    INTCONbits.RAIF         = 0;          // Change Interrupt Flag bit
    INTCONbits.RAIE         = 1;          // Change Interrupt Enable bit
}


//############################################################################################
// # void interrrupt_INT_SetUp()
//      External Interrupt
//############################################################################################

void setup_interrupt_INT0() {
    OPTION_REGbits.INTEDG   = FAILING_EDGE;     //cause interrupt at falling edge
    // OPTION_REGbits.INTEDG   = RAISING_EDGE;     //cause interrupt at falling edge

    INTCONbits.INTF         = OFF;          // PORTA Edge Interrupt Flag bit
    INTCONbits.INTE         = ON;          // PORTA Edge Interrupt Enable bit
}


//############################################################################################
// - Setup TMR0 Interrupt
//      non funziona con lo sleep()
//############################################################################################
void setUp_TMR0() {
    OPTION_REGbits.T0CS = 0;    // T0CS: Timer0 Clock Source Select bit
                                //  0 = Internal instruction cycle clock (FOSC/4)
                                //  1 = Transition on RA2/T0CKI pin

    OPTION_REGbits.T0SE = 0;    // T0SE: TMR0 Source Edge Select bit
                                //  0 = Increment on low-to-high transition on T0CKI pin
                                //  1 = Increment on high-to-low transition on T0CKI pin

    OPTION_REGbits.PSA = 0;    // PSA: Prescaler Assignment bit
                                //  0 = Prescaler is assigned to the Timer0 module
                                //  1 = Prescaler is assigned to the WDT

    OPTION_REGbits.PS  = 0b111; // PS<2:0>: Prescaler Rate Select bits
                                //  000     1:2
                                //  001     1:4
                                //  010     1:8
                                //  011     1:16
                                //  100     1:32
                                //  101     1:64
                                //  110     1:128
                                //  111     1:256


    // f = OSC/4
    // t = 1/f = 1/1,000,000 = 0.000001 Sec = 1uSec
    //     Period = (256 - TMR0)*(4/fosc)*(Prescaler)

    TMR0                = TMR0_Value; // must be itialized before clearing T0IF bit.
    TMR0_Counter        = 0;
    INTCONbits.T0IF     = 0;    // T0IF: Timer0 Overflow Interrupt Flag bit
                                //  1 = TMR0 register has overflowed (must be cleared in software)
                                //  0 = TMR0 register did not overflow
                                //  T0IF bit is set when Timer0 rolls over.
                                //   Timer0 is unchanged on Reset and should be
                                //   initialized before clearing T0IF bit.

    INTCONbits.T0IE     = 1;    // T0IE: Timer0 Overflow Interrupt Enable bit
                                //  1 = Enables the Timer0 interrupt
                                //  0 = Disables the Timer0 interrupt
}