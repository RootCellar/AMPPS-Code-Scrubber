/*
 *
 * Author: Darian Marvel
 * Created 1/19/2025
 * Last edited by Mary Bolling on 3/20/2025
 *
 * Prototype memory voting and correction for AMPPS
 *
*/


// Includes

#include <msp430fr5969.h>
#include "memory_writer.h"
#include <stdlib.h>
#include <stdio.h>
#include "memory_correction.h"


// Definitions

#define FLIP_RATE (1.0/(1000.0*1000.0))
#define NUM_COPIES (3)
#define DATA_SIZE (16*1024)
#define NUM_TEST_LOOPS (1*100)
#define FLIP_CHANCE_MOD (1000*1000*100)
#define BITS_IN_BYTE (8)


// Please, God, forgive me
int i, j, k;
int runItBack = 0;
uint8_t tickSenderIndex = 0;
int needsTransmit = 0;

#define CHAR_STR_LEN 18
#define NUM_STR_LEN  5		// keeps track of how many digits a number has
char tickString[CHAR_STR_LEN] = "1111 2222 3333 ";

uint16_t Ticks = 0;
int runCorrections;

int beforeTicks, tickDiff;
uint16_t copySuccesses;

// Functions


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
    	// TODO: Gotta change this so it doesn't run forever

    CSCTL0_H = 0;                               // Lock CS registers

    // Output MCLK
    PJSEL0 |= BIT1;
    PJDIR |= BIT1;
}


// Ripped from the AMPPS main code. Initializes UART, a means of communication between
// the MSP and a computer with an operating system
void UARTInit(void) {
	  // Configure GPIO
	  P2SEL1 |= BIT0 | BIT1;                    // USCI_A0 UART operation
	  P2SEL0 &= ~(BIT0 | BIT1);

	  // Disable the GPIO power-on default high-impedance mode to activate
	  // previously configured port settings
	  PM5CTL0 &= ~LOCKLPM5;

	  // Startup clock system with max DCO setting ~8MHz
	  CSCTL0_H = CSKEY >> 8;                    // Unlock clock registers
	  CSCTL1 = DCOFSEL_3 | DCORSEL;             // Set DCO to 8MHz
	  CSCTL2 = SELA__VLOCLK | SELS__DCOCLK | SELM__DCOCLK;
	  CSCTL3 = DIVA__1 | DIVS__1 | DIVM__1;     // Set all dividers
	  CSCTL0_H = 0;                             // Lock CS registers

	  // Configure USCI_A0 for UART mode
	  UCA0CTLW0 = UCSWRST;                      // Put eUSCI in reset
	  UCA0CTLW0 |= UCSSEL__SMCLK;               // CLK = SMCLK
	  // Baud Rate calculation
	  // 8000000/(16*9600) = 52.083
	  // Fractional portion = 0.083
	  // User's Guide Table 21-4: UCBRSx = 0x04
	  // UCBRFx = int ( (52.083-52)*16) = 1
	  UCA0BR0 = 52;                             // 8000000/16/9600
	  UCA0BR1 = 0x00;
	  UCA0MCTLW |= UCOS16 | UCBRF_1;
	  UCA0CTLW0 &= ~UCSWRST;                    // Initialize eUSCI
    UCA0IE |= UCTXIE;                       // Enable USCI_A0 TX interrupt
    UCA0IE |= UCRXIE;                       // Enable USCI_A0 RX interrupt
}


// Function: ticksTimerInit
// - intializes Timer0B to handle Ticks
// Ripped from the AMPPS main code
void ticksTimerInit(void) {
    // Setup T0B0
    TB0CCTL0 |= CCIE;                           // CCR0 interrupt enabled
    TB0CCR0 = 328;                              // 100 Ticks (ish) per second
    TB0CTL = TBSSEL_1 | MC_1 | TBCLR;           // ACLK, upmode, clear TBR
}


// Turn an integer into characters
void int_to_chars(char* str, int len, uint16_t num) {
  int i;


  uint32_t base = 1;
  for(i = 0; i < len; i++) {
    base *= 10;
  }
  base /= 100;
  str[len] = ' ';

  int idx = 0;
  for(i = len-1; i > 0 && base != 0; i--) {
    uint16_t c = (num % (base*10));
    c /= base;
    str[idx] = c + '0';
    base /= 10;
    idx++;
  }
}


int main(void)
{
	// TODO:
	/*
	 Make tick string longer (3 5-digit numbers)			- done
	 Make sure scrub reports the correct number of fixes
	 Verify that the data segments are the same				- done
	 */

	// Setup
	WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer
	PM5CTL0 &= ~LOCKLPM5;		// THIS LINE IS VERY IMPORTANT!!!!!!!!

	clockSystemInit();
	// Initialize the chip's UART capacity
	UARTInit();
	ticksTimerInit();
	MPUInit();
	P1DIR |= BIT0;
	P1OUT &= ~BIT0;
	P4DIR |= BIT6;
	P4OUT &= ~BIT6;

	//int len = 16384;

	char* data_copies[3];
	data_copies[0] = (char*) 0x4950;
	data_copies[1] = (char*) 0x6100;
	data_copies[2] = (char*) 0x10000;


	MPUSAM |= MPUSEG1WE;
	copy_text_section(TEXT_ADDR_SRC, TEXT_ADDR_DST_ONE, TEXT_SIZE);
	*(TEXT_ADDR_DST_ONE + 1) = 6;	// Self-inflicted bit flip
	MPUSAM &= ~MPUSEG1WE;
	MPUSAM |= MPUSEG2WE;
	copy_text_section(TEXT_ADDR_SRC, TEXT_ADDR_DST_TWO, TEXT_SIZE);
	MPUSAM &= ~MPUSEG2WE;

    __no_operation();
    __no_operation();


	while(1){
	    __bis_SR_register(GIE + LPM0_bits);     // Enter LPM0, enable interrupt
	    __no_operation();                       // for debugger
	    __no_operation();                       // SET BREAKPOINT HERE
		// Do the stuff


		// Send the time
	    if(runItBack == 1){
	    	needsTransmit = 1;
	    	beforeTicks = Ticks;
	    	copySuccesses = verify_same_mem_segments(TEXT_ADDR_SRC, TEXT_ADDR_DST_TWO, TEXT_SIZE);
	    	runCorrections = correct_errors(data_copies, 3, TEXT_SIZE);

	    	tickDiff = Ticks - beforeTicks;
	    	P4OUT ^= BIT6;
	    	runItBack = 0;

	    	int_to_chars(tickString, NUM_STR_LEN + 1, runCorrections);
	    	tickString[NUM_STR_LEN] = ' ';
	    	int_to_chars(tickString + NUM_STR_LEN + 1, NUM_STR_LEN + 1, copySuccesses);
	    	tickString[2*NUM_STR_LEN + 1] = ' ';
	    	copySuccesses = verify_same_mem_segments(TEXT_ADDR_SRC, TEXT_ADDR_DST_ONE, TEXT_SIZE);
	    	int_to_chars(tickString + 2*(NUM_STR_LEN) + 2, NUM_STR_LEN + 1, copySuccesses);
	    	tickString[CHAR_STR_LEN - 1] = ' ';
	    }

	    if(needsTransmit){
	    	// Transmit
	    	UCA0TXBUF = tickString[tickSenderIndex];
	    	needsTransmit = 0;
	    }
	}

}


//--------------------------------------------------------------------------
//-------------------------------Interrupts---------------------------------
//--------------------------------------------------------------------------

// UART INTERRUPT
#pragma vector=USCI_A0_VECTOR
__interrupt void USCI_A0_ISR(void){
    switch (__even_in_range(UCA0IV, USCI_UART_UCTXCPTIFG)) {
        case USCI_NONE: break;              // No interrupt
        case USCI_UART_UCRXIFG:             // RXIFG
        	P4OUT ^= BIT6;
        	runItBack = 1;
        	tickSenderIndex = 0;
        	__bic_SR_register_on_exit(LPM0_bits);
        	break;
        case USCI_UART_UCTXIFG:
        	//P4OUT ^= BIT6;
        	// If you finished transmitting and another digit needs transmitting, signal the MSP that
        	// it can send that now
        	if(tickSenderIndex < CHAR_STR_LEN - 1){
        		needsTransmit = 1;
        		tickSenderIndex++;
        	}
        	else{
        		needsTransmit = 0;
        	}
        	__bic_SR_register_on_exit(LPM0_bits);
        	break;     						// TXIFG
        case USCI_UART_UCSTTIFG: break;     // TTIFG
        case USCI_UART_UCTXCPTIFG: break;   // TXCPTIFG
        default: break;
    }
}

// TIMER0B0 INTERRUPT
// - time to increment Ticks
// Replaced with PORT3 interrupt
#pragma vector=TIMER0_B0_VECTOR
__interrupt void TIMER0_B0_ISR(void) {
//    SaveRegister = MPUSAM;              	// Save MPUSAM to restore its state after
//    MPUSAM |= MPUSEG1WE;                	// Enable write access to Seg 1

    Ticks += 1;                             // Increment Ticks
//    Flags |= 0x40000000;                    // Set BIT 31

//    MPUSAM = SaveRegister;              	// Restore MPUSAM
    __bic_SR_register_on_exit(LPM0_bits);   // Exit LPM0
	return;
}
