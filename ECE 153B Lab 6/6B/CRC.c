/*

* ECE 153B

*

* Name(s):

* Section:

* Lab: 6B

*/

#include "CRC.h"

// Enable the hardware CRC unit (default: poly 0x04C11DB7, init 0xFFFFFFFF, 32-bit, no reversal)

void CRC_Init(void)
{

    RCC->AHB1ENR |= RCC_AHB1ENR_CRCEN;

    CRC->CR = 0; // 32-bit polynomial, no input/output reversal

    CRC->INIT = 0xFFFFFFFF;

    CRC->POL = 0x04C11DB7;

    CRC->CR |= CRC_CR_RESET;
}

/**

* @brief Algorithm implementation of the CRC

* @param Crc: specifies the previous Crc data

* @param Data: specifies the input data

* @retval Crc: the CRC result of the input data

*/

uint32_t CrcSoftwareFunc(uint32_t Initial_Crc, uint32_t Input_Data, uint32_t POLY)

{

    uint8_t bindex = 0;

    uint32_t Crc = 0;

    /* Initial XOR operation with the previous Crc value */

    Crc = Initial_Crc ^ Input_Data;

    /* The CRC algorithm routine: one iteration per input bit (32 bits) */

    for (bindex = 0; bindex < (sizeof(Input_Data) * 8); bindex++)
    {

        if (Crc & 0x80000000)
        {

            Crc = (Crc << 1) ^ POLY;
        }

        else
        {

            Crc = Crc << 1;
        }
    }

    return Crc;
}
