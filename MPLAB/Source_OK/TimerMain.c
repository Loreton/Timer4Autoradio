/*
 * File:
 * Author: Loreto
 *
 * Created on 24 febbraio 2014, 8.16
    Accende il LED RD0 ogni qualvolta arriva un IRQ sul pin RB0
    Spegne  il LED RD1 in corrispondenza dell'accensione di RD0
    Lo mantiene acceso per circa 500mS
 */

void blinkLED(void);
void timerON(void);

#include <Timer4Autoradio_PIC12F635.h>

#ifdef LN_PIC12F635
            // If  the  WURE  bit  is  enabled  (‘0’)
            // When waking from Sleep, the WUR function resets the device
    #pragma config WURE=ON                                     // DETERMINANTE - Se OFF sembra che faccia un reset continuo
    #pragma config FCMEN=OFF, IESO=OFF, BOREN=ON
    #pragma config CPD=OFF,   CP=OFF,   MCLRE=OFF,  PWRTE=OFF      // CP/CPD=ON significa NON protetto
    #pragma config WDTE=OFF

    #pragma config FOSC=INTOSCIO               // INTOSCIO:   RA4=I/O,    RA5=I/O
    // #pragma config FOSC=INTOSCCLK           // INTOSC:     RA4=CLKOUT, RA5=I/O
#endif


#define TEMPO_ON  30 // in secondi)

int main(void) {
    mySTATUS.Flags        = 0x00;       /* Clear all bits */
    mySTATUS.V12Volt_ON   = FALSE;  /* Clear single bit */
    // Flag.isStart = 1;     /* Set a bit */


    disableIRQ();               // disable all interrupts
    setMainConfig();
    SetupClock();
    setPortConfig();
    blinkLED();

    setup_interrrupt_OnChange();
    setup_interrupt_INT0();
    enableIRQ();                // enable all interrupts

//counter01           = INACTIVE_TIMER;
//    TMR0counter02           = INACTIVE_TIMER;
    stopDeBounceValue       = INACTIVE_TIMER;
    TimerON = TEMPO_ON;

    LnPORTA.LED   = ON;
    LnPORTA.ALARM = ON;
   // myTime = INACTIVE_TIMER;
    while(1) {

        // if (mySTATUS.V12Volt_ON == TRUE) {
        //     LED_PIN    = 1;
        //     ALARM_PIN  = 1;
        //     mSecDELAY(5000);
        // }

        // while (PORTAbits.RA0 == 0) {
            // toggle_pin( LED_PIN );
        // }

        // while (PORTAbits.RA1 == 0) {
        //     LED_PIN  = 1;
            // toggle_pin( LED_PIN );
            // mSecDELAY(500);
        // }

        // while (PORTAbits.RA2 == 0) {
            // toggle_pin( LED_PIN );
            // LED_PIN  = 1;
            // mSecDELAY(500);
        // }
        // LED_PIN  = 0;


        SLEEP();  //  NON funziona con TMR0

    }
    return 0;
}





void blinkLED() {

    // PORTA = 0;
    for (int c=0; c<=10; c++) {
        toggle_pin( LnPORTA.LED );
        mSecDELAY(200);
    }
    // PORTA = 0;

}
