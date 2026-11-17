/*

* ECE 153B

*

* Name(s):

* Section:

* Lab: 6A

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

#ifndef POLYNOME
#define POLYNOME 0x04C11DB7
#endif

static volatile uint32_t Software_ComputedCRC;

int main(void)
{

    // Switch System Clock = 80 MHz

    System_Clock_Init();

    // TODO initialize modules

    SysTick_Init();

    LED_Init();

    UART2_GPIO_Init();

    UART2_Init();

    USART_Init(USART2);

    while (1)
    {

        LED_Toggle();

        // initialize CRC

        Software_ComputedCRC = INITIAL_CRC_VALUE;

        // start timer

        startTimer();

        // compute CRC

        for (uint32_t i = 0; i < BUFFER_SIZE; i++)
        {

            Software_ComputedCRC = CrcSoftwareFunc(Software_ComputedCRC, DataBuffer[i], POLYNOME);
        }

        // end timer

        uint32_t timespan = endTimer();

        // check CRC: stop (LED off) only if the result is WRONG

        if (Software_ComputedCRC != uwExpectedCRCValue)
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