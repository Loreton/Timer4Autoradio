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
void countDown(int TIMER_VALUE);


#include <Timer4Autoradio_PIC12F635.h>


    #ifdef LN_PIC12F635
        // RA0 - Pin7
        // RA1 - Pin6
        // RA2 - Pin5
        // RA3 - Pin4
        // RA4 - Pin3
        // RA5 - Pin2

                // If  the  WURE  bit  is  enabled  (‘0’)
                // When waking from Sleep, the WUR function resets the device
        #pragma config WURE=ON                                     // DETERMINANTE - Se OFF sembra che faccia un reset continuo
        #pragma config FCMEN=OFF, IESO=OFF, BOREN=ON
        #pragma config CPD=OFF,   CP=OFF,   MCLRE=OFF,  PWRTE=OFF      // CP/CPD=ON significa NON protetto
        #pragma config WDTE=OFF

        // #pragma config FOSC=EXTRCIO             // EXTRCIO:    RA4=I/O,    RA5=External RC
        // #pragma config FOSC=INTOSCCLK           // INTOSC:     RA4=CLKOUT, RA5=I/O
        #pragma config FOSC=INTOSCIO            // INTOSCIO:   RA4=I/O,    RA5=I/O
        // #pragma config FOSC=EC                  // EC:         RA4=I/O,    RA5=CLKIN


    #endif


#define TEMPO_ON  30 // in secondi


// ***************************************************************************
// 1 - Non appena si accende la macchina il 12VoltComandato
//      accende il relè che va ad alimentare l'autoradio ed il circuito.
// 2 - Il circuito rileva se la macchina è accesa tramite il bit carSTATE
//      dove è inserito un partitore.
// 3 - Se è accesa allora OUT_LINE va ON
// 4 - Attendiamo che la macchina venga spenta
// 5 - Viene fatto partire un timer di 15 minuti circa
//   - Ora si aprono due strade:
//     A - Attendiamo che venga stinto il pulsante
//     B - Attendiamo che scada il timer
// ***************************************************************************

int main(void) {

    flag.flags   = 0x00;     /* Clear all bits */

    disableIRQ();                   // disable all interrupts
    setMainConfig();
    SetupClock();
    setPortConfig();
    // blinkLED();                     // Lampeggio iniziale

    // setup_interrrupt_OnChange();    // usato per itercettare se la CAR è spenta o accesa
    setup_interrupt_INT0();         // Pulsante per forzare l'accensione/spegnimento
    setUp_TMR0();                   // Lo usiamo per far lampeggiare il LED quando la CAR è accesa
    enableIRQ();                    // enable all interrupts



    // -------------------------------
    // - STARTing data assumptions
    // -------------------------------
    //disableTMR0();
    // flag.isCarON = LnPORTA.V12_CMD;
    DESIRED_STATE = FORCED_OFF;
    if (LnPORTA.V12_CMD == True) {
        DESIRED_STATE = FORCED_FREE;
        LnPORTA.OUT_LINE = ON;
        // LnPORTA.LED = LnPORTA.OUT_LINE;
    }

    while(1) {

            // ------------------------
            // - Macchina ACCESA
            // ------------------------
        while (LnPORTA.V12_CMD == True) {
            if (DESIRED_STATE == FORCED_ON)
                LnPORTA.OUT_LINE = ON;
            else if (DESIRED_STATE == FORCED_OFF)
                LnPORTA.OUT_LINE = OFF;
            else
                LnPORTA.OUT_LINE = ON;

            // flag.isCarON = LnPORTA.V12_CMD;
        }

            // ------------------------
            // - Macchina SPENTA
            // ------------------------
        while (LnPORTA.V12_CMD == False) {
            // SLEEP();
            // flag.isCarON = LnPORTA.V12_CMD;
            // if (LnPORTA.V12_CMD) continue;

            if (DESIRED_STATE == FORCED_ON) DESIRED_STATE = FORCED_FREE;

            if (DESIRED_STATE == FORCED_OFF) {
                LnPORTA.OUT_LINE = OFF;
            }
            else {
                disableTMR0();
                countDown(CoutDown_Time);
                enableTMR0();
            }

            if (LnPORTA.V12_CMD == True) DESIRED_STATE = FORCED_ON;
            // flag.goToSleep = True;

        }



    }

    return 0;
}

//###############################################################
// - countDown(int TIMER_VALUE)  in Sec
// - Accende la radio ed attende il tempo richiesto
// - Se la macchina è accesa    esce
// - Se DESIRED_STATE == OFF    esce
//###############################################################
void countDown(int TIMER_VALUE) {
int myTimer, mSec;


    LnPORTA.OUT_LINE = ON;  // Accendiamo per sicurezza
    myTimer = 0;
    // if (TIMER_VALUE < 0) TIMER_VALUE -= 0xFFFF;

    int TEST;
    while (myTimer < TIMER_VALUE) {

        // ------------------------------------
        // - Ogni secondo lo spezziamo come
        // - richiesto dagli STEP
        // ------------------------------------
        mSec = 0;
        while (mSec < 1000) {
            // flag.isCarON = LnPORTA.V12_CMD;
            if (LnPORTA.V12_CMD == True) return; // inutile proseguire
            if (DESIRED_STATE == FORCED_OFF) break;
            toggle_pin( LnPORTA.LED );

            if (myTimer > CoutDown_TimeWNG) {
                TEST = CoutDown_STEPSpeed;
                mSecDELAY(CoutDown_STEPSpeed);
                mSec += CoutDown_STEPSpeed;
            }
            else {
                TEST = CoutDown_STEPSlow;
                mSecDELAY(CoutDown_STEPSlow);
                mSec += CoutDown_STEPSlow;
            }
        }

        if (DESIRED_STATE == FORCED_OFF) break;
        myTimer += 1;
    }

    DESIRED_STATE = FORCED_OFF;     // Forziamo lo Status
    LnPORTA.OUT_LINE = OFF;         // Spegniamo
    return;
}



// ###############################################################
// - blinkLED()
// ###############################################################
void blinkLED() {
    PORTA = 0;
    for (int c=0; c<=10; c++) {
        toggle_pin( LnPORTA.LED );
        mSecDELAY(200);
    }
    PORTA = 0;

}


