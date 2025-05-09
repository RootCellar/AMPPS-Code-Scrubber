#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H

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

#endif
