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

#define pullDOWNxxx




#ifndef I_AM_TIMER_PIC12F635_H
    #define I_AM_TIMER_PIC12F635_H
    #include <xc.h>




    // ##################################################
    // # DEFINE
    // ##################################################

    #define  testbit(var, bit)   ((var) & (1 <<(bit)))
    #define  setbit(var, bit)    ((var) |= (1 << (bit)))
    #define  clrbit(var, bit)    ((var) &= ~(1 << (bit)))

    #define enableIRQ()   (INTCONbits.GIE = 1)    // Interrupts of Hi/Lo Priority or Peripheral interrupts
    #define disableIRQ()  (INTCONbits.GIE = 0)    // Interrupts of Hi/Lo Priority or Peripheral interrupts


    // ---- Varie

    #define OUTPUT              0
    #define INPUT               1
    #define ENABLE            1
    #define DISABLE           0
    #define PULLUP              1
    #define PULLDWON            0
    #define ON                  1
    #define OFF                 0
    #define NO                  0
    #define YES                 0
    #define TRUE                1
    #define FALSE               0


    #define FAILING_EDGE        0
    #define RAISING_EDGE        1





    // #define V12_CMD_PIN         PORTAbits.RA1
    #define V12_CMD_PIN         bit1
    #define PULSANTE_PIN        PORTAbits.RA2
    #define ALARM_PIN           PORTAbits.RA4            // Uscita impulso
    #define LED_PIN             PORTAbits.RA5            // Uscita LED

    #define V12_CMD         bit1
    #define PULSANTE        bit2
    #define ALARM           bit4            // Uscita impulso
    #define LED             bit5            // Uscita LED


    // ##################################################
    // # CLOCK
    // ##################################################
    #ifndef _XTAL_FREQ
        // #define     _XTAL_FREQ      4000000     // EXTRCIO:    RA4=I/O,    RA5=External RC
        // #define     _XTAL_FREQ      2000000     // INTOSC:     RA4=CLKOUT, RA5=I/O
        #define     _XTAL_FREQ      1000000     // INTOSCIO:   RA4=I/O,    RA5=I/O
        // #define     _XTAL_FREQ      500000      // EC:         RA4=I/O,    RA5=CLKIN
        #define     __delay_us(x)   _delay((unsigned long)((x)*(_XTAL_FREQ/4000000.0)))
        #define     __delay_ms(x)   _delay((unsigned long)((x)*(_XTAL_FREQ/4000.0)))
        #define     __delay_sec(x)  _delay((unsigned long)((x)*(_XTAL_FREQ/4.0)))
    #endif

    #define toggle_pin( pin )  ( (pin == 1) ? pin = 0 : pin = 1 )
    #define toggle_bit( bit )  ( (bit == 1) ? bit = 0 : bit = 1 )


    #ifdef LN_SIMULATION
        #define     mSecDELAY(x)           __delay_us(x)
        #define     SecDELAY(x)           __delay_ms(x)
    #else
        #define     mSecDELAY(x)           __delay_ms(x)
        #define     SecDELAY(x)            __delay_sec(x)
    #endif


    // ############################################
    // # Flags Structure TYPE 2 BEGIN
    // ############################################
    typedef union {
        volatile unsigned char Flags;
        struct {
            unsigned V12Volt_ON:1;
            unsigned Stop:1;
            unsigned bit2:1;
            unsigned bit3:1;
            unsigned bit4:1;
            unsigned bit5:2;
            unsigned bit7:1;
        };
    } LnFLAGS;

    // ##################################################
    // # Structs
    // ##################################################
    typedef union {
        volatile unsigned char byte;
        struct {
            unsigned bit0:1;
            unsigned bit1:1;
            unsigned bit2:1;
            unsigned bit3:1;
            unsigned bit4:1;
            unsigned bit5:1;
            unsigned bit6:1;
            unsigned bit7:1;
        };
        struct {
            unsigned pin0:1;
            unsigned pin1:1;
            unsigned pin2:1;
            unsigned pin3:1;
            unsigned pin4:1;
            unsigned pin5:1;
            unsigned pin6:1;
            unsigned pin7:1;
        };
    } LnByte;

    typedef union {
        volatile unsigned char port;
        struct {
            unsigned pin0:1;
            unsigned pin1:1;
            unsigned pin2:1;
            unsigned pin3:1;
            unsigned pin4:1;
            unsigned pin5:1;
            unsigned pin6:1;
            unsigned pin7:1;
        };
    } LnPort;

    // -------------- LnByte example --------------
    // LnByte portBValue;
    // portBValue.byte = PORTB;
    // portBValue.bit0  = INTCONbits.RBIF;

    LnByte portAValue0, portAValue1;
    volatile LnByte LnTRISA;
    volatile LnByte LnPORTA;
    volatile LnByte LnWPUDA;
    volatile LnByte LnWDA;
    volatile LnByte LnIOCA;

    typedef struct {
            unsigned b0:1;
            unsigned b1:1;
            unsigned b2:1;
            unsigned b3:1;
            unsigned b4:1;
            unsigned b5:1;
            unsigned b6:1;
            unsigned b7:1;
    } unionByte;
    // -------------- LnByte example --------------
    // unionByte portBValue;
    // portBValue.byte = PORTB;  -- NON PERMESSO (vedi LnByte)
    // portBValue.b0  = INTCONbits.RBIF;







    // crea una struttura FlagU.ANONYMOUS_0.rca ..
    // - devono andare in un file.c

    LnFLAGS mySTATUS;
    // STATUS.Flags = 0xFF;  /* Clear all bits */
    // STATUS.isStart = 0;  /* Clear single bit */
    // STATUS.isStart = 1;     /* Set a bit */

    // ############################################
    // # Flags Structure TYPE 2 END
    // ############################################
    // unsigned char TMR0_Value = 0x01; // va moltiplicato per il PSA

    #define INACTIVE_TIMER      -1
    int stopDeBounceValue;     // counter di appoggio Periodo = TMR0counter01*TMR0Period
    #define TMR0_Value          150    // Period = (256 - TMR0)*(4/fosc)*(Prescaler)
    #define TEMPO_ON  30 // in secondi)
    int TimerON;          // tempo del Timer attivo in SEC
    int StartKeyTime;   // tempo di pressione del tasto di start in SEC




    // ##################################################
    // # Funzioni
    // ##################################################
    void setMainConfig(void);
    void setPortConfig(void);
    void SetupClock(void);
    void setup_interrrupt_OnChange(void);
    void setup_interrupt_INT0(void);



#endif



