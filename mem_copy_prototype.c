#include <msp430fr5969.h>
#include <stdint.h>

#define TEXT_ADDR_SRC ((uint32_t*) 0x10000)
#define TEXT_ADDR_DST ((uint32_t*) 0x4400)
#define TEXT_SIZE   0x300  // Adjust size based on your code size

volatile uint16_t copy_successes = 0; // Should equal TEXT_SIZE ideally

uint16_t SaveRegister = 0;                                  // Save MPUSAM register to restore state after interrupt

void copy_text_section(void) {
    MPUSAM |= MPUSEG1WE;
    //MPUSAM |= MPUSEG2WE;
    //MPUSAM |= MPUSEG2RE;
    MPUSAM |= MPUSEG3RE;
    memcpy(TEXT_ADDR_DST, TEXT_ADDR_SRC, TEXT_SIZE);
}

// Function: allPortsToZero
// - all ports have a value of zero, avoids leaving an unused pin on
void allPortsToZero(void) {
    P1DIR = 0x00;
    P2DIR = 0x00;
    P3DIR = 0x00;
    P4DIR = 0x00;
    PJDIR = 0x00;

    P1OUT = 0x00;
    P2OUT = 0x00;
    P3OUT = 0x00;
    P4OUT = 0x00;
    PJOUT = 0x00;

    P1REN = 0x00;
    P2REN = 0x00;
    P3REN = 0x00;
    P4REN = 0x00;
    PJREN = 0x00;

    P1SEL0 = 0x00;
    P1SEL1 = 0x00;
    P2SEL0 = 0x00;
    P2SEL1 = 0x00;
    P3SEL0 = 0x00;
    P3SEL1 = 0x00;
    P4SEL0 = 0x00;
    P4SEL1 = 0x00;
    PJSEL0 = 0x00;
    PJSEL1 = 0x00;
}

void MPUInit(void) {
    // Configure MPU
    MPUCTL0 = MPUPW;                          // Write PWD to access MPU registers
    MPUSEGB1 = 0x6000;
    MPUSEGB2 = 0xFFFF;                        // Borders are assigned to segments

    //  Segment 1    - Execute, Read
    //  Segment 2    - Execute, Read
    //  Segment 3    - Execute, Read
    //  User Mem Seg - Execute, Read, Write
    MPUSAM =             MPUSEG1RE | MPUSEG1XE |
                         MPUSEG2RE | MPUSEG2XE |
                         MPUSEG3RE | MPUSEG3XE |
                         MPUSEGIRE | MPUSEGIXE | MPUSEGIWE;

    MPUSAM |= MPUSEG1VS | MPUSEG2VS | MPUSEG3VS;        // Causes reset upon segment violation

    MPUCTL0 = MPUPW | MPUENA | MPUSEGIE;               // Enable MPU protection
}

void clockSystemInit(void) {
    PJSEL0 |= BIT4 | BIT5;                      // For XT1
    FRCTL0 = FRCTLPW | NWAITS_1;                // Must occur before initializing CSC above 8 MHz

    // XT1 Setup
    CSCTL0_H = CSKEY >> 8;                      // Unlock CS registers
    CSCTL1 = DCORSEL | DCOFSEL_4;               // Set DCO to 16MHz
    CSCTL2 = SELA__LFXTCLK | SELS__DCOCLK | SELM__DCOCLK;
    CSCTL3 = DIVA__1 | DIVS__1 | DIVM__1;       // Set all dividers
    CSCTL4 &= ~LFXTOFF;
    do
    {
          CSCTL5 &= ~LFXTOFFG;                  // Clear XT1 fault flag
          SFRIFG1 &= ~OFIFG;
    }while (SFRIFG1&OFIFG);                     // Test oscillator fault flag
    CSCTL0_H = 0;                               // Lock CS registers

    // Output MCLK
    PJSEL0 |= BIT1;
    PJDIR |= BIT1;
}

void startup(void) {
    allPortsToZero();
    clockSystemInit();
    MPUInit();

    // Disable the GPIO power-on default high-impedance mode to activate
    // previously configured port settings
    PM5CTL0 &= ~LOCKLPM5;

    copy_text_section();
}


void main(void) {
    WDTCTL = WDTPW | WDTHOLD;                   // stop watchdog timer

    startup();

    uint16_t i = 0;

    uint8_t* SRC_pointer = TEXT_ADDR_SRC;
    uint8_t* DST_pointer = TEXT_ADDR_DST;

    for (i = 0; i < TEXT_SIZE; i++) {
        if (*(SRC_pointer + i) == *(DST_pointer + i)) {
            copy_successes += 1;
        }
    }

    while(1) {
        __bis_SR_register(GIE + LPM0_bits);     // Enter LPM0, enable interrupt
        __no_operation();                       // for debugger
    }
}


// DMA interrupt
#pragma vector=DMA_VECTOR
__interrupt void DMA_ISR(void) {
    switch(__even_in_range(DMAIV, DMAIV_DMA2IFG)) {
        case DMAIV_NONE: break;
        case DMAIV_DMA0IFG: // DMA0IFG = DMA Channel 0
            SaveRegister = MPUSAM;
            MPUSAM &= ~MPUSEG1WE;
            MPUSAM &= ~MPUSEG3RE;
            MPUSAM = SaveRegister;
            break;
        case DMAIV_DMA1IFG: // DMA1IFG = DMA Channel 1
            break;
        case DMAIV_DMA2IFG: break; // DMA2IFG = DMA Channel 2
        default: break;
    }
}
