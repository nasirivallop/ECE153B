/*

* ECE 153B

*

* Name(s):

* Section:

* Lab: 6B

*/

#include "stm32l476xx.h"

#include "SysClock.h"

#include "SysTimer.h"

#include "LED.h"

#include "CRC.h"

#include "UART.h"

#include <stdio.h>

/* Expected CRC Value */

#define uwExpectedCRCValue 0x5A60861E

void CRC_Init(void); // CRC.c

static volatile uint32_t Hardware_ComputedCRC;

int main(void)
{

    // Switch System Clock = 80 MHz

    System_Clock_Init();

    // Initialize modules

    SysTick_Init();

    LED_Init();

    UART2_GPIO_Init();

    UART2_Init();

    USART_Init(USART2);

    CRC_Init();

    while (1)
    {

        LED_Toggle();

        // initialize CRC (reloads CRC->INIT into the data register)

        CRC->CR |= CRC_CR_RESET;

        // start timer

        startTimer();

        // compute CRC with the hardware unit

        for (uint32_t i = 0; i < BUFFER_SIZE; i++)
        {

            CRC->DR = DataBuffer[i];
        }

        Hardware_ComputedCRC = CRC->DR;

        // end timer

        uint32_t timespan = endTimer();

        // check CRC: stop (LED off) only if the result is WRONG

        if (Hardware_ComputedCRC != uwExpectedCRCValue)
        {

            LED_Off();

            break;
        }

        // print time

        printf("%u us\n", (unsigned int)timespan);

        // delay

        delay(1000);
    }

    while (1)
        ; // never return from main
}
