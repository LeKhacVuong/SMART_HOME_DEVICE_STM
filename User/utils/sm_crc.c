/*
  CRC.c - Simple library to calculate 8-, 16-, and 32-Bit CRC
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
#include <sm_crc.h>
#include <stdio.h>


//---- Prototypes ----//
__attribute__((unused)) static void CRC_CalculateCRC8Table(void);
__attribute__((unused)) static void CRC_CalculateCRC16Table(void);
__attribute__((unused)) static void CRC_CalculateCRC32Table(void);

static uint8_t CRC_ReverseBitOrder8(uint8_t value);
static uint32_t CRC_ReverseBitOrder32(uint32_t value);

static uint16_t crc_16_buypass_lut[256u];

#if (CRC_8_MODE == TABLE)
	static uint8_t* CRC8Table;
#endif

#if (CRC_16_MODE == TABLE)
	static uint16_t* CRC16Table = crc_16_buypass_lut;
#endif

#if (CRC_32_MODE == TABLE)
	static uint32_t* CRC32Table;
#endif


void sm_CRC_Init(void)
{
	CRC16Table = crc_16_buypass_lut;
}


uint8_t sm_CRC_CalculateCRC8(const uint8_t *Buffer, uint16_t Length)
{
	uint8_t retVal = 0u;
	uint16_t byteIndex = 0u;


	if(Buffer != NULL)
    {
#if (CRC_8_MODE == RUNTTIME)
		uint8_t bitIndex = 0u;

		retVal = CRC_8_INIT_VALUE;

		/* Do calculation procedure for each byte */
		for(byteIndex = 0u; byteIndex < Length; byteIndex++)
        {
			/* XOR new byte with temp result */
			retVal ^= (Buffer[byteIndex] << (CRC_8_RESULT_WIDTH - 8u));

			/* Do calculation for current data */
			for(bitIndex = 0u; bitIndex < 8u; bitIndex++)
            {
				if(retVal & (1u << (CRC_8_RESULT_WIDTH - 1u)))
                {
					retVal = (uint8_t)((retVal << 1u) ^ CRC_8_POLYNOMIAL);
				}
				else
                {
					retVal = (retVal << 1u);
				}
			}
		}

		/* XOR result with specified value */
		retVal ^= CRC_8_XOR_VALUE;

#elif (CRC_8_MODE == TABLE)
		retVal = CRC_8_INIT_VALUE;

		for(byteIndex = 0u; byteIndex < Length; byteIndex++)
        {
			retVal = CRC8Table[(retVal) ^ Buffer[byteIndex]];
		}

		/* XOR result with specified value */
		retVal ^= CRC_8_XOR_VALUE;

#else
		/* Mode not implemented */
		retVal = 0x00u;

#endif
	}

	return retVal;
}


uint16_t sm_CRC_CalculateCRC16(const uint8_t *Buffer, uint32_t Length)
{
	uint32_t retVal = 0u;
   uint32_t byteIndex = 0u;


    if(Buffer != NULL)
    {
#if (CRC_16_MODE==RUNTTIME)
		retVal = CRC_16_INIT_VALUE;

		/* Do calculation procedure for each byte */
		for(byteIndex = 0u; byteIndex < Length; byteIndex++)
        {
			/* XOR new byte with temp result */
			retVal ^= (Buffer[byteIndex] << (CRC_16_RESULT_WIDTH - 8u));

            uint8_t bitIndex = 0u;
			/* Do calculation for current data */
			for(bitIndex = 0u; bitIndex < 8u; bitIndex++)
            {
				if(retVal & (1u << (CRC_16_RESULT_WIDTH - 1u)))
                {
					retVal = (retVal << 1u) ^ CRC_16_POLYNOMIAL;
				}
				else
                {
					retVal = (retVal << 1u);
				}
			}
		}

		/* XOR result with specified value */
		retVal ^= CRC_16_XOR_VALUE;

#elif (CRC_16_MODE==TABLE)
		retVal = CRC_16_INIT_VALUE;

		/* Update the CRC using the data */
		for(byteIndex = 0u; byteIndex < Length; byteIndex++)
        {
			retVal = (uint16_t)((retVal << 8u) ^ CRC16Table[(retVal >> 8u) ^ Buffer[byteIndex]]);
		}

		/* XOR result with specified value */
		retVal ^= CRC_16_XOR_VALUE;
#else
		/* Mode not implemented */
		retVal = 0x0000u;

#endif
	}

   return retVal;
}


uint32_t sm_CRC_CalculateCRC32(const uint8_t *Buffer, uint32_t Length)
{
	uint32_t retVal = 0u;
	uint32_t byteIndex = 0u;


	if(Buffer != NULL)
    {
#if (CRC_32_MODE==RUNTTIME)
		retVal = CRC_32_INIT_VALUE;

        /* Do calculation procedure for each byte */
		for(byteIndex = 0u; byteIndex < Length; byteIndex++)
        {
            /* XOR new byte with temp result */
            retVal ^= (CRC_ReverseBitOrder8(Buffer[byteIndex]) << (CRC_32_RESULT_WIDTH - 8u));

            uint8_t bitIndex = 0u;
            /* Do calculation for current data */
			for(bitIndex = 0u; bitIndex < 8u; bitIndex++)
            {
                if(retVal & (1u << (CRC_32_RESULT_WIDTH - 1u)))
                {
                    retVal = (retVal << 1u) ^ CRC_32_POLYNOMIAL;
                }
                else
                {
                    retVal = (retVal << 1u);
                }
            }
        }

        /* XOR result with specified value */
		retVal ^= CRC_32_XOR_VALUE;

#elif (CRC_32_MODE==TABLE)
        uint8_t data = 0u;

		retVal = CRC_32_INIT_VALUE;

		for(byteIndex = 0u; byteIndex < Length; ++byteIndex)
        {
            data = CRC_ReverseBitOrder8(Buffer[byteIndex]) ^ (retVal >> (CRC_32_RESULT_WIDTH - 8u));
            retVal = CRC32Table[data] ^ (retVal << 8u);
        }

		/* XOR result with specified value */
		retVal ^= CRC_32_XOR_VALUE;

#else
		/* Mode not implemented */
		retVal = 0x00000000u;

#endif
	}

    /* Reflect result */
    retVal = CRC_ReverseBitOrder32(retVal);

	return retVal;
}


static void CRC_CalculateCRC8Table(void)
{
#if (CRC_8_MODE==TABLE)
	uint16_t i = 0u, j = 0u;

	for(i = 0u; i < 256u; ++i)
	{
		uint8_t curr = i;

		for(j = 0u; j < 8u; ++j)
		{
			if((curr & 0x80u) != 0u)
			{
				curr = (curr << 1u) ^ CRC_8_POLYNOMIAL;
			}
			else
			{
				curr <<= 1u;
			}
		}

		CRC8Table[i] = curr;
	}
#endif
}


static void CRC_CalculateCRC16Table(void)
{
#if (CRC_16_MODE==TABLE)
	uint16_t i = 0u, j = 0u;
	uint16_t result = 0u;
	uint16_t xor_flag = 0u;

	for(i = 0u; i < 256u; i++)
	{
		result = i << 8u;

		for(j = 0u; j < 8u; j++)
		{
			/* Flag for XOR if leftmost bit is set */
			xor_flag = result & 0x8000u;

			/* Shift CRC */
			result <<= 1u;

			/* Perform the XOR */
			if(xor_flag != 0u)
				result ^= CRC_16_POLYNOMIAL;
		}

		CRC16Table[i] = result;
	}
#endif
}


static void CRC_CalculateCRC32Table(void)
{
#if (CRC_32_MODE==TABLE)
    uint32_t remainder = 0u;

    for(uint32_t dividend = 0u; dividend < 256u; ++dividend)
    {
        remainder = dividend << (CRC_32_RESULT_WIDTH - 8u);

        for(uint8_t bit = 8u; bit > 0u; --bit)
        {
            if(remainder & (1u << (CRC_32_RESULT_WIDTH - 1u)))
            {
                remainder = (remainder << 1u) ^ CRC_32_POLYNOMIAL;
            }
            else
            {
                remainder = (remainder << 1u);
            }
        }

        CRC32Table[dividend] = remainder;
    }
#endif
}


static uint8_t CRC_ReverseBitOrder8(uint8_t value)
{
    value = (uint8_t)((value & 0xF0) >> 4u | (value & 0x0F) << 4u);
    value = (uint8_t)((value & 0xCC) >> 2u | (value & 0x33) << 2u);
    value = (uint8_t)((value & 0xAA) >> 1u | (value & 0x55) << 1u);

    return value;
}


static uint32_t CRC_ReverseBitOrder32(uint32_t value)
{
    uint32_t reversed = 0u;

    for(uint8_t i = 31u; value; )
    {
        reversed |= (value & 1u) << i;
        value >>= 1u;
        -- i;
    }

    return reversed;
}

static uint16_t crc_16_buypass_lut[256u] = {
		0x0000, 0x1021, 0x2042, 0x3063, 0x4084, 0x50a5, 0x60c6, 0x70e7,
		0x8108, 0x9129, 0xa14a, 0xb16b, 0xc18c, 0xd1ad, 0xe1ce, 0xf1ef,
		0x1231, 0x0210, 0x3273, 0x2252, 0x52b5, 0x4294, 0x72f7, 0x62d6,
		0x9339, 0x8318, 0xb37b, 0xa35a, 0xd3bd, 0xc39c, 0xf3ff, 0xe3de,
		0x2462, 0x3443, 0x0420, 0x1401, 0x64e6, 0x74c7, 0x44a4, 0x5485,
		0xa56a, 0xb54b, 0x8528, 0x9509, 0xe5ee, 0xf5cf, 0xc5ac, 0xd58d,
		0x3653, 0x2672, 0x1611, 0x0630, 0x76d7, 0x66f6, 0x5695, 0x46b4,
		0xb75b, 0xa77a, 0x9719, 0x8738, 0xf7df, 0xe7fe, 0xd79d, 0xc7bc,
		0x48c4, 0x58e5, 0x6886, 0x78a7, 0x0840, 0x1861, 0x2802, 0x3823,
		0xc9cc, 0xd9ed, 0xe98e, 0xf9af, 0x8948, 0x9969, 0xa90a, 0xb92b,
		0x5af5, 0x4ad4, 0x7ab7, 0x6a96, 0x1a71, 0x0a50, 0x3a33, 0x2a12,
		0xdbfd, 0xcbdc, 0xfbbf, 0xeb9e, 0x9b79, 0x8b58, 0xbb3b, 0xab1a,
		0x6ca6, 0x7c87, 0x4ce4, 0x5cc5, 0x2c22, 0x3c03, 0x0c60, 0x1c41,
		0xedae, 0xfd8f, 0xcdec, 0xddcd, 0xad2a, 0xbd0b, 0x8d68, 0x9d49,
		0x7e97, 0x6eb6, 0x5ed5, 0x4ef4, 0x3e13, 0x2e32, 0x1e51, 0x0e70,
		0xff9f, 0xefbe, 0xdfdd, 0xcffc, 0xbf1b, 0xaf3a, 0x9f59, 0x8f78,
		0x9188, 0x81a9, 0xb1ca, 0xa1eb, 0xd10c, 0xc12d, 0xf14e, 0xe16f,
		0x1080, 0x00a1, 0x30c2, 0x20e3, 0x5004, 0x4025, 0x7046, 0x6067,
		0x83b9, 0x9398, 0xa3fb, 0xb3da, 0xc33d, 0xd31c, 0xe37f, 0xf35e,
		0x02b1, 0x1290, 0x22f3, 0x32d2, 0x4235, 0x5214, 0x6277, 0x7256,
		0xb5ea, 0xa5cb, 0x95a8, 0x8589, 0xf56e, 0xe54f, 0xd52c, 0xc50d,
		0x34e2, 0x24c3, 0x14a0, 0x0481, 0x7466, 0x6447, 0x5424, 0x4405,
		0xa7db, 0xb7fa, 0x8799, 0x97b8, 0xe75f, 0xf77e, 0xc71d, 0xd73c,
		0x26d3, 0x36f2, 0x0691, 0x16b0, 0x6657, 0x7676, 0x4615, 0x5634,
		0xd94c, 0xc96d, 0xf90e, 0xe92f, 0x99c8, 0x89e9, 0xb98a, 0xa9ab,
		0x5844, 0x4865, 0x7806, 0x6827, 0x18c0, 0x08e1, 0x3882, 0x28a3,
		0xcb7d, 0xdb5c, 0xeb3f, 0xfb1e, 0x8bf9, 0x9bd8, 0xabbb, 0xbb9a,
		0x4a75, 0x5a54, 0x6a37, 0x7a16, 0x0af1, 0x1ad0, 0x2ab3, 0x3a92,
		0xfd2e, 0xed0f, 0xdd6c, 0xcd4d, 0xbdaa, 0xad8b, 0x9de8, 0x8dc9,
		0x7c26, 0x6c07, 0x5c64, 0x4c45, 0x3ca2, 0x2c83, 0x1ce0, 0x0cc1,
		0xef1f, 0xff3e, 0xcf5d, 0xdf7c, 0xaf9b, 0xbfba, 0x8fd9, 0x9ff8,
		0x6e17, 0x7e36, 0x4e55, 0x5e74, 0x2e93, 0x3eb2, 0x0ed1, 0x1ef0
};
