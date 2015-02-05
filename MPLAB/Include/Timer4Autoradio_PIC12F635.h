/*
 * File:   interrupt-03.c
 * Author: Loreto
 *
 * Created on 24 febbraio 2014, 8.16
 Accende il LED RD0 ogni qualvolta arriva un IRQ sul pin RB0
 Spegne  il LED RD1 in corrispondenza dell'accensione di RD0
 Lo mantiene acceso per circa 500mS
 */
// #################################
// TRISIO == TRISA
// GP2 == RA2
// #################################

#define LN_PIC12F635
#define LN_SIMULATIONxx


#include <xc.h>
#ifndef I_AM_TIMER_PIC12F635_H
    #define I_AM_TIMER_PIC12F635_H



    // ##################################################
    // # DEFINE
    // ##################################################

    #define testbit(var, bit)   ((var) & (1 <<(bit)))
    #define setbit(var, bit)    ((var) |= (1 << (bit)))
    #define clrbit(var, bit)    ((var) &= ~(1 << (bit)))

    #define enableIRQ()     (INTCONbits.GIE = 1)    // Interrupts of Hi/Lo Priority or Peripheral interrupts
    #define disableIRQ()    (INTCONbits.GIE = 0)    // Interrupts of Hi/Lo Priority or Peripheral interrupts

    // #define enableTMR0()    (TMR0 = TMR0_Value, INTCONbits.T0IF = 0, INTCONbits.T0IE = 1)
    #define enableTMR0()    (INTCONbits.T0IE = 1)
    #define disableTMR0()   (INTCONbits.T0IE = 0)

    #define enableRAIE()    (INTCONbits.RAIF = 0, INTCONbits.RAIE = 1)
    #define disableRAIE()   (INTCONbits.RAIE = 0)

    #define enableINT0()    (OPTION_REGbits.INTEDG = FAILING_EDGE, INTCONbits.INTF = 0, INTCONbits.INTE = 1)
    #define disableINT0()   (INTCONbits.INTE = 0)



    // ---- Varie
    #define INPUT               1
    #define OUTPUT              0
    #define ENABLE              1
    #define DISABLE             0
    #define PULLUP              1
    #define PULLDWON            0
    #define ON                  1
    #define OFF                 0
    #define YES                 1
    #define NO                  0
    #define True                1
    #define False               0
    #define RAISING_EDGE        1
    #define FAILING_EDGE        0



    // ##################################################
    // # CLOCK
    // ##################################################
    #ifndef _XTAL_FREQ
        // #define     _XTAL_FREQ      4000000     // EXTRCIO:    RA4=I/O,    RA5=External RC
        #define     _XTAL_FREQ      2000000     // INTOSC:     RA4=CLKOUT, RA5=I/O
        // #define     _XTAL_FREQ      1000000     // INTOSCIO:   RA4=I/O,    RA5=I/O
        // #define     _XTAL_FREQ      500000      // EC:         RA4=I/O,    RA5=CLKIN
    #endif

    #define __delay_us(x)       _delay((unsigned long)((x)*(_XTAL_FREQ/4000000.0)))
    #define __delay_ms(x)       _delay((unsigned long)((x)*(_XTAL_FREQ/4000.0)))
    #define __delay_sec(x)      _delay((unsigned long)((x)*(_XTAL_FREQ/4.0)))
    #define toggle_pin( pin )  ( (pin == 1) ? pin = 0 : pin = 1 )
    #define toggle_bit( bit )  ( (bit == 1) ? bit = 0 : bit = 1 )


    #ifdef LN_SIMULATION
        #define     mSecDELAY(x)           __delay_us(x)
        #define     SecDELAY(x)            __delay_ms(x)
    #else
        #define     mSecDELAY(x)           __delay_ms(x)
        #define     SecDELAY(x)            __delay_sec(x)
    #endif

    // ##################################################
    // # Structs
    // ##################################################
    typedef union {
        volatile unsigned char flags;
        struct {
            unsigned TMR0isExpired:1;
            unsigned isCarON:1;
            unsigned TimerIsActive:1;
            unsigned PulsanteHasPushed:1;
            unsigned desiredStatex:1;
            unsigned forceSTOP:1;
            unsigned goToSleep:1;
            unsigned bit7:1;
        };
    } LnFLAG01;

    LnFLAG01 fSTATUS, flag;
    // STATUS.Flags = 0xFF;  /* Clear all bits */
    // STATUS.isStart = 0;  /* Clear single bit */
    // STATUS.isStart = 1;     /* Set a bit */

        // RA0 - Pin7
        // RA1 - Pin6
        // RA2 - Pin5
        // RA3 - Pin4
        // RA4 - Pin3
        // RA5 - Pin2

    // extern volatile unsigned char           LnTRISA               @ 0x085;
    typedef union {
        volatile unsigned char byte;
        struct {
                unsigned V12_CMD:1;     // RA0 - Pin7 V12_Comandata
                unsigned bit1:1;        // RA1 - Pin6
                unsigned PULSANTE:1;    // RA2 - Pin5
                unsigned MCLR:1;        // RA3 - Pin4
                unsigned OUT_LINE:1;    // RA4 - Pin3 (ClockOut)
                unsigned LED:1;         // RA5 - Pin2
        };
        struct {
                unsigned RA0:1;
                unsigned RA1:1;
                unsigned RA2:1;
                unsigned RA3:1;
                unsigned RA4:1;
                unsigned RA5:1;
        };
        struct {
                unsigned Pin7:1;
                unsigned Pin6:1;
                unsigned Pin5:1;
                unsigned Pin4:1;
                unsigned Pin3:1;
                unsigned Pin2:1;
        };
    } LnPortA_Bits;

    extern volatile LnPortA_Bits LnTRISA @ 0x085;
    extern volatile LnPortA_Bits LnPORTA @ 0x005;
    extern volatile LnPortA_Bits LnWPUDA @ 0x095;
    extern volatile LnPortA_Bits LnWDA   @ 0x097;
    extern volatile LnPortA_Bits LnIOCA  @ 0x096;


    // -------------- LnPort example --------------
    // LnPort portBValue;
    // portBValue.byte = PORTB;
    // portBValue.bit0  = INTCONbits.RBIF;



    // ############################################
    // # Flags Structure TYPE 2 END
    // ############################################
    // unsigned char TMR0_Value = 0x01; // va moltiplicato per il PSA

    // #define TIMER_INACTIVE     -1
    int stopDeBounceValue;     // counter di appoggio Periodo = TMR0counter01*TMR0Period
    // Period = (256 - TMR0)*(4/fosc)*(Prescaler)
    #define TMR0_Value          158 // 100mSec = (256 - 158)*(4/2000000)*(512)  Misurato.
    int TMR0_Counter;

    #define STOP_Time       1000
    #define START_Time      1000
    #define STOP_delayStep    500
    #define START_delayStep   250

    #define CoutDown_Time         15*60                    // in Secondi
    // #define CoutDown_TimeWNG      CoutDown_Time*0.8      // in secondi
    #define CoutDown_TimeWNG      CoutDown_Time-(2*60)      // in secondi - inizia il lampeggio veloce
    #define CoutDown_STEPSpeed    100                   // in mSec
    #define CoutDown_STEPSlow     500                   // in mSec

    volatile int DESIRED_STATE;
    #define FORCED_ON    1
    #define FORCED_OFF   2
    #define FORCED_FREE   3


    // ##################################################
    // # Funzioni
    // ##################################################
    void setMainConfig(void);
    void setPortConfig(void);
    void SetupClock(void);
    void setup_interrrupt_OnChange(void);
    void setup_interrupt_INT0(void);
    void setUp_TMR0(void);



#endif



