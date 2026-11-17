/*

* ECE 153B

*

* Name(s):

* Section:

* Lab: 6C

*/

#include "stm32l476xx.h"

#include "SysClock.h"

#include "SysTimer.h"

#include "LED.h"

#include "CRC.h"

#include "DMA.h"

#include "UART.h"

#include <stdio.h>

/* Expected CRC Value */

static uint32_t uwExpectedCRCValue = 0x5A60861E;

static volatile uint32_t ComputedCRC;

static volatile uint32_t ComputationDone = 0;

void CRC_Init(void); // CRC.c

void DMA_Init(void); // DMA.c

// Called from the DMA transfer-complete interrupt

void completeCRC(uint32_t crc)
{

    ComputedCRC = crc;

    ComputationDone = 1;
}

int main(void)
{

    uint32_t time;

    // Switch System Clock = 80 MHz

    System_Clock_Init();

    // systick init

    SysTick_Init();

    // uart init

    UART2_GPIO_Init();

    UART2_Init();

    USART_Init(USART2);

    // led, crc, dma init

    LED_Init();

    CRC_Init();

    DMA_Init();

    while (1)
    {

        LED_Toggle();

        // reset CRC and flag

        CRC->CR |= CRC_CR_RESET;

        ComputationDone = 0;

        // start timer

        startTimer();

        // start DMA: feed the whole buffer into CRC->DR

        DMA1_Channel6->CCR &= ~DMA_CCR_EN;

        DMA1_Channel6->CMAR = (uint32_t)DataBuffer;

        DMA1_Channel6->CNDTR = BUFFER_SIZE;

        DMA1_Channel6->CCR |= DMA_CCR_EN;

        // wait for the DMA interrupt to report completion

        while (!ComputationDone)
            ;

        // stop timer

        time = endTimer();

        DMA1_Channel6->CCR &= ~DMA_CCR_EN;

        // check CRC: stop (LED off) only if the result is WRONG

        if (ComputedCRC != uwExpectedCRCValue)
        {

            LED_Off();

            break;
        }

        // print time

        printf("%u us\n", (unsigned int)time);

        // delay

        delay(1000);
    }

    while (1)
        ;
}
