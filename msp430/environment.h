#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H

<<<<<<< HEAD
#include <msp430fr5969.h>

=======
/*
 * Environment Settings
*/

// Defines the size of the static agreements list
#define NUM_CODE_COPIES 3

/*
 * MSP430 Microcontroller environment-specific code
*/

#include <msp430fr5969.h>

#define SCRUBBING_ON_MSP430

>>>>>>> 8bcbada62ca2a6056c211b0f67eadf762774b618
#define LOCK_MEMORY_SEGMENT(x) \
	switch(x) {\
		case 0:\
			MPUSAM &= ~MPUSEG1WE;\
			break;\
		case 1:\
			MPUSAM &= ~MPUSEG2WE;\
			break;\
		case 2:\
			MPUSAM &= ~MPUSEG3WE;\
			break;\
	}

#define UNLOCK_MEMORY_SEGMENT(x) \
	switch(x) {\
		case 0:\
			MPUSAM |= MPUSEG1WE;\
			break;\
		case 1:\
			MPUSAM |= MPUSEG2WE;\
			break;\
		case 2:\
			MPUSAM |= MPUSEG3WE;\
			break;\
	}

#define DATA_WRITE(data_copies, which, loc, bits) data_copies[which][loc] = bits
#define DATA_READ(data_copies, which, loc) data_copies[which][loc]

<<<<<<< HEAD
void __lock_memory_segment(int which);
void __unlock_memory_segment(int which);

void __data_write(char** data_copies, int which, int loc, char value);
char __data_read(char** data_copies, int which, int loc);




=======
>>>>>>> 8bcbada62ca2a6056c211b0f67eadf762774b618
#endif
