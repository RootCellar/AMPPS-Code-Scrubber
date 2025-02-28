#include <msp430fr5969.h>

/*
 *
 * Author: Darian Marvel
 * Created 1/19/2025
 * Last edited by Mary Bolling on 2/10/2025
 *
 * Prototype memory voting and correction for AMPPS
 *
*/


// Includes

#include <stdlib.h>
#include <stdio.h>
#include "memory_correction.h"
//#include <time.h>
//#include <string.h>

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

uint16_t Ticks = 0;

// Functions

// Ripped from the AMPPS main code. Initializes UART, a means of communication between
// the MSP and a computer with an operating system
void UARTInit(void) {
    // Setup P2.0 UCA0TXD, P2.1 UCA0RXD
    P2SEL1 |= BIT0 | BIT1;                    // Set P2.0, P2.1 to non-IO
    P2SEL0 &= ~(BIT0 | BIT1);
    PJSEL0 |= BIT4 | BIT5;                    // For XT1

    // Disable the GPIO power-on default high-impedance mode to activate
    // previously configured port settings
    //PM5CTL0 &= ~LOCKLPM5;

    // Setup eUSCI_A0
    UCA0CTLW0 |= UCSWRST;                   // **Put state machine in reset**

    UCA0CTLW0 |= UCSSEL__SMCLK;                // CLK = ACLK
    UCA0BR0 = 8;                              // 115200 baud
    UCA0MCTLW |= 0xF7 | UCOS16 | UCBRF3 | UCBRF1;
                                              // UCBRSx value = 0xB5 (See UG)
    UCA0BR1 = 0;

    UCA0CTLW0 &= ~UCSWRST;                  // **Initialize USCI state machine**
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



//struct timespec get_time() {
//  struct timespec time;
//  clock_gettime(CLOCK_REALTIME, &time);
//  return time;
//}

//double roll_flip_chance() {
//  double random_value = rand() % FLIP_CHANCE_MOD;
//  //double random_value = 10000000;
//  return random_value / (double) FLIP_CHANCE_MOD;
//}
//
//char random_bit() {
//  return 1 << (rand() % BITS_IN_BYTE);
//}
//
//// Returns how many bits this function flipped during the call
////
///*int simulate_flips(char** data_copies, int num_copies, int data_size, double flip_rate) {
//
//  double random_roll;
//  int flips = 0;
//
//  for(i = 0; i < num_copies; i++) {
//    for(k = 0; k < data_size; k++) {
//      for(j = 0; j < BITS_IN_BYTE; j++) {
//        random_roll = roll_flip_chance();
//        if(random_roll < flip_rate) {
//          data_copies[i][k] ^= 1 << j;
//          flips++;
//        }
//      }
//    }
//  }
//
//  return flips;
//}
//*/
//
//// Performs a bit-level correction of the byte at loc in data_copies
//char correct_bits(char** data_copies, int num_copies, int loc) {
//
//  char agreements[num_copies];
//
//  char most_agreements;
//  char most_agreed_value;
//
//  char corrections = 0;
//
//  for(i = 0; i < BITS_IN_BYTE; i++) {
//
//    // Reset tracked agreements
//    for(k = 0; k < num_copies; k++) agreements[k] = 0;
//    most_agreements = 0;
//    most_agreed_value = 0;
//
//    char current_bit = 1 << i;
//
//    // Search forward for data copies that agree
//    // and keep track of how many are found
//    for(k = 0; k < num_copies; k++)
//      for(j = k+1; j < num_copies; j++) {
//        if((data_copies[j][loc] & current_bit) == (data_copies[k][loc] & current_bit)) {
//          agreements[k]++;
//        }
//      }
//
//    // Find the most agreed value
//    for(k = 0; k < num_copies; k++) {
//      if(agreements[k] > most_agreements) {
//        most_agreements = agreements[k];
//        most_agreed_value = data_copies[k][loc] & current_bit;
//      }
//    }
//
//    // Correct data copies to match
//    for(k = 0; k < num_copies; k++) {
//      if((data_copies[k][loc] & current_bit) != most_agreed_value) {
//        data_copies[k][loc] ^= current_bit;
//        corrections++;
//      }
//    }
//
//  }
//
//  return corrections;
//}
//
//// Searches through every byte and ensures that all data copies agree on
//// the value of each. In the event that the data copies don't agree on
//// the value of a particular byte, correct the copies of that byte
//// at the bit level.
//int correct_errors(char** data_copies, int num_copies, int data_size) {
//
//  if(num_copies < 2) return -1;
//
//  int corrections = 0;
//
//  for(i = 0; i < data_size; i++) {
//
//    char value = data_copies[0][i];
//
//    // Search forward through the data copies
//    // and make sure they all agree with the current byte.
//    // If they don't, perform a correction
//    for(k = 1; k < num_copies; k++) {
//      if(data_copies[k][i] != value) {
//        corrections += correct_bits(data_copies, num_copies, i);
//        k = num_copies;
//      }
//    }
//
//  }
//
//  return corrections;
//}
//
//struct memory_correction_test_result {
//  int num_flips;
//  int reported_corrections;
//  int unsolved_errors;
//};
//
//struct memory_correction_test_result test_memory_correction(char* original_data, char** data_copies, int num_copies, int data_size, int cycles, double flip_rate) {
//
//  struct memory_correction_test_result results =
//    (struct memory_correction_test_result) {.num_flips = 0, .reported_corrections = 0, .unsolved_errors = 0};
//
//  for(i = 0; i < cycles; i++) {
//    // //printf("%d\n", i);
//    // Cause memory bit flips and then have the algorithm attempt
//    // to correct them
//    results.num_flips += simulate_flips(data_copies, num_copies, data_size, flip_rate);
//    results.reported_corrections += correct_errors(data_copies, num_copies, data_size);
//
//    // Check to make sure the algorithm corrected every byte.
//    // If it didn't, increment the unsolved error count and
//    // correct it for the next iteration
//    for(j = 0; j < data_size; j++) {
//      for(k = 0; k < num_copies; k++) {
//        if(data_copies[k][j] != original_data[j]) {
//          results.unsolved_errors++;
//          data_copies[k][j] = original_data[j];
//        }
//      }
//    }
//  }
//
//  return results;
//}
//
//struct data_copies_collection {
//  char* original_data;
//  char** data_copies;
//  int num_copies;
//  int data_size;
//};
//
//void cleanup_data_copy_collection(struct data_copies_collection* data_copies) {
//  if(data_copies->original_data != NULL) {
//    free(data_copies->original_data);
//    data_copies->original_data = NULL;
//  }
//
//  if(data_copies->data_copies != NULL) {
//    for(i = 0; i < data_copies->num_copies; i++) {
//      if(data_copies->data_copies[i] != NULL) {
//        free(data_copies->data_copies[i]);
//        data_copies->data_copies[i] = NULL;
//      }
//    }
//
//    free(data_copies->data_copies);
//    data_copies->data_copies = NULL;
//  }
//}
//
//struct data_copies_collection create_data_copy_collection(int copies, int size) {
//
//  struct data_copies_collection toRet =
//    (struct data_copies_collection) {.original_data = NULL, .data_copies = NULL, .num_copies = copies, .data_size = size};
//
//  size_t data_bytes = sizeof(char) * size;
//  size_t copy_pointers_list_size = sizeof(char*) * copies;
//
//  toRet.original_data = (char*) malloc(data_bytes);
//  if(toRet.original_data == NULL) {
//    //printf("Unable to allocate memory for original_data\n");
//    return toRet;
//  }
//
//  toRet.data_copies = (char**) malloc(copy_pointers_list_size);
//  if(toRet.data_copies == NULL) {
//    //printf("Unable to allocate memory for data_copies list\n");
//    cleanup_data_copy_collection(&toRet);
//    return toRet;
//  }
//  memset(toRet.data_copies, 0, copy_pointers_list_size);
//
//  for(i = 0; i < copies; i++) {
//    toRet.data_copies[i] = (char*) malloc(data_bytes);
//    if(toRet.data_copies[i] == NULL) {
//      //printf("Unable to allocate memory for memory copy %d\n", i);
//      cleanup_data_copy_collection(&toRet);
//      return toRet;
//    }
//  }
//
//  return toRet;
//}
//
//void fill_copies_with_random(struct data_copies_collection data_copies) {
//  for(i = 0; i < data_copies.data_size; i++) {
//    data_copies.original_data[i] = rand();
//    for(k = 0; k < data_copies.num_copies; k++) {
//      data_copies.data_copies[k][i] = data_copies.original_data[i];
//    }
//  }
//}
//
//




int main(void)
{
	// Setup
	WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer
	PM5CTL0 &= ~LOCKLPM5;		// THIS LINE IS VERY IMPORTANT!!!!!!!!

	// Initialize the chip's UART capacity
	UARTInit();
	ticksTimerInit();
	P1DIR |= BIT0;
	P1OUT &= ~BIT0;
	P4DIR |= BIT6;
	P4OUT &= ~BIT6;

	while(1){
	    __bis_SR_register(GIE + LPM0_bits);     // Enter LPM0, enable interrupt
	    __no_operation();                       // for debugger
	    __no_operation();                       // SET BREAKPOINT HERE
		// Do the stuff


		// Send the time
	    if(runItBack == 1){
	    	UCA0TXBUF = Ticks;
	    	runItBack = 0;
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
        	P1OUT ^= BIT0;
        	runItBack = 1;
        	break;
        case USCI_UART_UCTXIFG:
        	P4OUT ^= BIT6;
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
