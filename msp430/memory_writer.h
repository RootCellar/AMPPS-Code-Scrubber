/*
 * memory_writer.h
 *
 *  Created on: Mar 20, 2025
 *      Author: Mary Bollllllling
 */

#ifndef MEMORY_WRITER_H_
#define MEMORY_WRITER_H_

#include <stdint.h>
#include <msp430fr5969.h>

#define TEXT_ADDR_SRC ((uint8_t*) 0x10000)
#define TEXT_ADDR_DST ((uint8_t*) 0x6100)
#define TEXT_SIZE   0x1000  // Adjust size based on your code size

void copy_text_section(uint8_t* SRC_pointer, uint8_t* DST_pointer, uint16_t text_size);
void MPUInit(void);
uint16_t verify_same_mem_segments(uint8_t* SRC_pointer, uint8_t* DST_pointer, uint16_t text_size);

#endif /* MEMORY_WRITER_H_ */
