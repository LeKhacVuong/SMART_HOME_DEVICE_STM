/*
 * crc.h
 *
 *  Created on: Jun 1, 2022
 *      Author: khanh
 */

#ifndef CRC_H_
#define CRC_H_

/*
  CRC.h - Simple library to calculate 8-, 16-, and 32-Bit CRC
  Copyright (c) 2018 Patrick F.
  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.
  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
  You should have received a copy of the GNU General Public License
  along with program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <stdint.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif


#define RUNTTIME							0
#define TABLE								1
#define HARDWARE							2


/* ---------- Defines for 8-bit SAE J1850 CRC calculation (Not reflected) ------------------------------------------------------- */
#define CRC_8_RESULT_WIDTH                  8u
#define CRC_8_POLYNOMIAL                    0x1Du
#define CRC_8_INIT_VALUE                    0xFFu
#define CRC_8_XOR_VALUE                     0xFFu
#define CRC_8_MODE							RUNTTIME

/* ---------- Defines for 16-bit CCITT CRC calculation (Not reflected) ---------------------------------------------------------- */
#define CRC_16_RESULT_WIDTH                 16u
#define CRC_16_POLYNOMIAL                   0x1021u
#define CRC_16_INIT_VALUE                   0xFFFFu
#define CRC_16_XOR_VALUE                    0x0000u
#define CRC_16_MODE							TABLE

/* ---------- Defines for 32-bit CCITT CRC calculation (Reflected) -------------------------------------------------------------- */
#define CRC_32_RESULT_WIDTH                 32u
#define CRC_32_POLYNOMIAL                   0x04C11DB7u
#define CRC_32_INIT_VALUE                   0xFFFFFFFFu
#define CRC_32_XOR_VALUE                    0xFFFFFFFFu
#define CRC_32_MODE							RUNTTIME


/**
 * @brief
 */
void sm_CRC_Init(void);

/**
 * @brief
 * @param Buffer
 * @param Length
 * @return
 */
uint8_t sm_CRC_CalculateCRC8(const uint8_t *Buffer, uint16_t Length);
/**
 * @brief
 * @param Buffer
 * @param Length
 * @return
 */
uint16_t sm_CRC_CalculateCRC16(const uint8_t *Buffer, uint32_t Length);

/**
 * @brief
 * @param Buffer
 * @param Length
 * @return
 */
uint32_t sm_CRC_CalculateCRC32(const uint8_t *Buffer, uint32_t Length);


#ifdef __cplusplus
}
#endif

#endif /* CRC_H_ */
