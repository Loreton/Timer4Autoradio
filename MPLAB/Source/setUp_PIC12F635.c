/*
  Microcontroller = PIC12F635
  Internal Clock @ 4.0 MHz
*/
#include <xc.h>


#include <Timer4Autoradio_PIC12F635.h>




//############################################################################################
// # void setPortConfig() {
//############################################################################################
#define pullUP
#define pullDOWNxxx

void setPortConfig() {


    // IOCA: INTERRUPT-ON-CHANGE PORTA REGISTER (0=disabled 1=enabled)
    // WDA:   WEAK PULL-UP/PULL-DOWN DIRECTION REGISTER (1=PullUP 0=PullDOWN)
    // WPUDA: WEAK PULL-UP/PULL-DOWN ENABLE REGISTER    (1=Pull UP/Down enabled)

    #ifdef pullUP
        OPTION_REGbits.nRAPU    = 0;                        // 0=Enable Wake Pull-Up sulla porta A
        WDA                     = 0b11111111;               // =PullUP 0=PullDOWN
        WPUDA                   = 0b11111111;               // 1=PullUP/PullDown enabled

    #elif defined(pullDOWN)
        OPTION_REGbits.nRAPU    = 0;                        // 0=Enable Wake Pull-Up sulla porta A
        WDA                     = 0b00000000;               // 1=PullUP 0=PullDOWN
        WPUDA                   = 0b11111111;               // 1=PullUP/PullDown enabled

    #else
        OPTION_REGbits.nRAPU    = 1;                        // 1=Disable Wake Pull-Up sulla porta A
        WDA                     = 0b11111111;               // 1=PullUP 0=PullDOWN
        WPUDA                   = 0b11111111;               // 1=PullUP/PullDown enabled
        // WPUDA                   = 0b00000000;               // 0=PullUP/PullDown disabled
    #endif



        // ------------------------------
        // - PORT A Assignments
        // ------------------------------
    TRISA            =  0b00111111;             // 1=INPUT - 0=OUTPUT
                        //||||||||________ RA0
                        //|||||||_________ RA1
                        //||||||__________ RA2
                        //|||||___________ RA3  (MCLR)
                        //||||____________ RA4
                        //|||_____________ RA5
                        //||______________ RA6
                        //|_______________ RA7



        // ------------------------------
        // - PIN come RAIE
        // ------------------------------
    // TRISAbits.TRISA0        = INPUT;         // 1=set as input
    // WPUDAbits.WPUDA0        = ENABLE;        // Pull-UP/Down         [1=enabled 0=disabled]
    // WDAbits.WDA0            = PULLUP;        // Pull-UP/Down direction (1=PullUP 0=PullDOWN)
    // IOCAbits.IOCA0          = ENABLE;        // interrupt-on-change  [1=enabled 0=disabled]

        // ------------------------------
        // - PIN come RAIE
        // ------------------------------
    LnTRISA.V12_CMD        = INPUT;         // 1=set as input
    LnWPUDA.V12_CMD        = DISABLE;        // Pull-UP/Down         [1=enabled 0=disabled]
    // LnWDA.V12_CMD          = PULLUP;        // Pull-UP/Down direction (1=PullUP 0=PullDOWN)

        // ------------------------------
        // - PIN come INT0
        // ------------------------------
    LnTRISA.PULSANTE        = INPUT;        // 1=set as input
    LnWPUDA.PULSANTE        = ENABLE;        // Pull-UP/Down         [1=enabled 0=disabled]
    // LnWDA.PULSANTE          = PULLUP;        // Pull-UP/Down direction (1=PullUP 0=PullDOWN)
    // LnIOCA.PULSANTE         = DISABLE;        // interrupt-on-change  [1=enabled 0=disabled]


        // ------------------------------
        // - Output Pins
        // ------------------------------
    LnTRISA.OUT_LINE        = OUTPUT;
    LnTRISA.LED             = OUTPUT;



}



void setMainConfig() {

    // OPTION = 0b00101011;            // 2.11 * 8 = 16 seconds
        // OPTION_REG: OPTION REGISTER
    OPTION_REG    =  0b10000111;
                //     ||||||||________ PS<2:0>: Prescaler Rate Select bits
                //     |||||||_________ PS<2:0>: Prescaler Rate Select bits
                //     ||||||__________ PS<2:0>: Prescaler Rate Select bits
                //     |||||___________ PSA: Prescaler Assignment bit
                //                              1=Prescaler is assigned to the WDT
                //                              0=Prescaler is assigned to the Timer0 module
                //     ||||____________ T0SE: Timer0 Source Edge Select bit
                //                              1=Increment on high-to-low transition on RA2/T0CKI pin
                //     |||_____________ T0CS: Timer0 Clock Source Select bit
                //                              1=Transition on RA2/T0CKI pin
                //                              0=Internal instruction cycle clock (FOSC/4)
                //     ||______________ INTEDG: Interrupt Edge Select bit
                //                              1=Interrupt on rising edge of RA2/INT pin
                //     |_______________ RAPU: PORTA Pull-up Enable bit
                //                              1=PORTA pull-ups are disabled

        // WDTCON: WATCHDOG TIMER CONTROL REGISTER
    // WDTCON = 0b00010111;            // xxx,1011(65536*31kHz=2.11sec),1(WDT Enable)
    WDTCONbits.SWDTEN = OFF;        //(WDT turned OFF)

        //INTCON - Disable all interrupts
    INTCON = 0b00000000;

    // IOCA: INTERRUPT-ON-CHANGE PORTA REGISTER ( 1 = Interrupt-on-change enabled)
    IOCA   = 0b00000000;   // ALL disabled



        //Comparitor Control
     CMCON0 = 0b00000111;
            //  ||||||||____bit 2-0 CM<2:0>: Comparator Mode bits (See Figure 7-5)
            //  |||||                           000 = CIN pins are configured as analog, COUT pin configured as I/O, Comparator output turned off
            //  |||||                           001 = CIN pins are configured as analog, COUT pin configured as Comparator output
            //  |||||                           010 = CIN pins are configured as analog, COUT pin configured as I/O, Comparator output available internally
            //  |||||                           011 = CIN- pin is configured as analog, CIN+ pin is configured as I/O, COUT pin configured as Comparator output, CVREF is non-inverting input
            //  |||||                           100 = CIN- pin is configured as analog, CIN+ pin is configured as I/O, COUT pin is configured as I/O, Comparator output available internally, CVREF is non-inverting input
            //  |||||                           101 = CIN pins are configured as analog and multiplexed, COUT pin is configured as Comparator output, CVREF is non-inverting input
            //  |||||                           110 = CIN pins are configured as analog and multiplexed, COUT pin is configured as I/O, Comparator output available internally, CVREF is non-inverting input
            //  |||||                           111 = CIN pins are configured as I/O, COUT pin is configured as I/O, Comparator output disabled, Comparator off.
            //  |||||
            //  |||||________ bit 3 CIS:      Comparator Input Switch bit
            //  ||||                             When CM<2:0> = 110 or 101:
            //  ||||                                 1 = CIN+ connects to VIN-
            //  ||||                                 0 = CIN- connects to VIN-
            //  ||||                             When CM<2:0> = 0xx or 100 or 111:
            //  ||||                                 CIS has no effect.
            //  ||||
            //  ||||________ bit 4 CINV:      Comparator Output Inversion bit
            //  ||||                                 1 = Output inverted
            //  ||||                                 0 = Output not inverted
            //  ||||
            //  |||_________ bit 5            Unimplemented: Read as ‘0’
            //  ||
            //  ||__________ bit 6 COUT:      Comparator Output bit
            //  |                               When CINV = 0:
            //  |                                   1 = VIN+ > VIN-
            //  |                                   0 = VIN+ < VIN-
            //  |                               When CINV = 1:
            //  |                                   1 = VIN+ < VIN-
            //  |                                   0 = VIN+ > VIN-
            //  |
            //  |___________ bit 7            Unimplemented: Read as ‘0’





}

void SetupClock() {


//############################################################################################
//                                                             Value on     Value on
//                                                             POR, BOR     all other
//                                                                          Resets
//     OSCCON   — IRCF2 IRCF1 IRCF0       OSTS HTS LTS SCS    -110 x000     -110 x000
// --------------------------------------------------------------------------------------------
//      bit 7       Unimplemented: Read as ‘0’
//      bit 6-4     IRCF<2:0>: Internal Oscillator Frequency Select bits
//          111 =   8 MHz               EXTRC:      RA4=CLKOUT, RA5=External RC
//          110 =   4 MHz (default)     EXTRCIO:    RA4=I/O,    RA5=External RC
//          101 =   2 MHz               INTOSC:     RA4=CLKOUT, RA5=I/O
//          100 =   1 MHz               INTOSCIO:   RA4=I/O,    RA5=I/O
//          011 =   500 kHz             EC:         RA4=I/O,    RA5=CLKIN
//          010 =   250 kHz             HS:         RA4_RA5=HighSpeed crystal/resonator
//          001 =   125 kHz             XT:         RA4_RA5=Crystal/resonator
//          000 =   31 kHz (LFINTOSC)   LP:         RA4_RA5=Low-power crystal
//
//      bit 3       OSTS: Oscillator Start-up Time-outPUT Status bit (1)
//          1 =  Device is running from the external clock defined by FOSC<2:0> of the Configuration Word
//          0 =  Device is running from the internal oscillator (HFINTOSC or LFINTOSC)
//
//      bit 2       HTS:  HFINTOSC Status bit (High Frequency – 8 MHz to 125 kHz)
//          1 =  HFINTOSC is stable
//          0 =  HFINTOSC is not stable
//
//      bit 1       LTS: LFINTOSC Stable bit (Low Frequency – 31 kHz)
//          1 =  LFINTOSC is stable
//          0 =  LFINTOSC is not stable
//
//      bit 0       SCS: System Clock Select bit
//          1 =  Internal oscillator is used for system clock
//          0 =  Clock source defined by FOSC<2:0> of the Configuration Word
//
//      Note  1:    Bit resets to ‘0’ with Two-Speed Start-up and LP, XT or HS selected as the Oscillator mode or Fail-Safe mode is enabled.
//############################################################################################
    //OSCCONbits.OSTS  = 1;
    //OSCCONbits.SCS   = 0;


    #if     _XTAL_FREQ  ==  8000000
        OSCCON  = 0b01110000;                   //8MHz  clock speed
    #elif   _XTAL_FREQ  ==  4000000
        OSCCON  = 0b00110000;                   // 4MHz  clock speed
    #elif   _XTAL_FREQ  ==  2000000
        OSCCON  = 0b01010000;                   //2MHz  clock speed
    #elif   _XTAL_FREQ  ==  1000000
        OSCCON  = 0b01000000;                   //1MHz  clock speed
    #elif   _XTAL_FREQ  ==   500000
        OSCCON  = 0b00110000;                   //500KHz  clock speed
    #elif   _XTAL_FREQ  ==   250000
        OSCCON  = 0b00100000;                   //250KHz  clock speed
    #elif   _XTAL_FREQ  ==   125000
        OSCCON  = 0b00010000;                   //125KHz  clock speed
    #else
        OSCCON  = 0b01100000;                   //4MHz  clock speed DEFAULT
    #endif

}


