/*
 * memory_writer.c
 *
 *  Created on: Mar 20, 2025
 *      Author: wisea
 */


#include "memory_writer.h"

void copy_text_section(uint8_t* SRC_pointer, uint8_t* DST_pointer, uint16_t text_size) {
    MPUSAM |= MPUSEG2WE;
    MPUSAM |= MPUSEG3RE;
    memcpy(DST_pointer, SRC_pointer, text_size);
}

void MPUInit(void) {
    // Configure MPU
    MPUCTL0 = MPUPW;                          // Write PWD to access MPU registers
    MPUSEGB1 = 0x600;
    MPUSEGB2 = 0xFFF;                        // Borders are assigned to segments

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

uint16_t verify_same_mem_segments(uint8_t* SRC_pointer, uint8_t* DST_pointer, uint16_t text_size) {
	uint16_t i = 0;
	uint16_t copy_successes = 0;

//    uint8_t* SRC_pointer = TEXT_ADDR_SRC;
//    uint8_t* DST_pointer = TEXT_ADDR_DST;

    for (i = 0; i < text_size; i++) {
        if (*(SRC_pointer + i) == *(DST_pointer + i)) {
            copy_successes += 1;
        }
    }
    return copy_successes;
}
