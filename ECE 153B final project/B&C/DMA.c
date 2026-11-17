/*

* ECE 153B

*

* Name(s):

* Section:

* Project

*/

#include "DMA.h"

#include "SysTimer.h"

#include "UART.h"

void on_complete_transfer(void); // implemented in UART.c

void DMA_Init_UARTx(DMA_Channel_TypeDef *tx, USART_TypeDef *uart)
{

    volatile uint32_t wait;

    RCC->AHB1ENR |= RCC_AHB1ENR_DMA1EN;

    // Short (~20 us) wait for the DMA clock to come up

    for (wait = 0; wait < 2000; wait++)
        ;

    tx->CCR &= ~DMA_CCR_EN; // Disable channel while configuring

    tx->CCR &= ~DMA_CCR_MEM2MEM; // Memory-to-peripheral, not mem-to-mem

    tx->CCR &= ~DMA_CCR_PL; // Priority: high

    tx->CCR |= DMA_CCR_PL_1;

    tx->CCR &= ~DMA_CCR_PSIZE; // 8-bit peripheral

    tx->CCR &= ~DMA_CCR_MSIZE; // 8-bit memory

    tx->CCR &= ~DMA_CCR_PINC; // Peripheral address fixed (TDR)

    tx->CCR |= DMA_CCR_MINC; // Memory address increments

    tx->CCR &= ~DMA_CCR_CIRC; // Not circular

    tx->CCR |= DMA_CCR_DIR; // Read from memory, write to peripheral

    tx->CPAR = (uint32_t) & (uart->TDR);

    tx->CCR &= ~DMA_CCR_HTIE; // No half-transfer interrupt

    tx->CCR &= ~DMA_CCR_TEIE; // No transfer-error interrupt

    tx->CCR |= DMA_CCR_TCIE; // Transfer-complete interrupt

    if (uart == USART1)
    {

        // USART1_TX = DMA1 Channel 4, request 2 (C4S = bits 15:12)

        DMA1_CSELR->CSELR &= ~(0xFUL << 12);

        DMA1_CSELR->CSELR |= (2UL << 12);

        NVIC_SetPriority(DMA1_Channel4_IRQn, 2);

        NVIC_EnableIRQ(DMA1_Channel4_IRQn);
    }
    else
    {

        // USART2_TX = DMA1 Channel 7, request 2 (C7S = bits 27:24)

        DMA1_CSELR->CSELR &= ~(0xFUL << 24);

        DMA1_CSELR->CSELR |= (2UL << 24);

        NVIC_SetPriority(DMA1_Channel7_IRQn, 2);

        NVIC_EnableIRQ(DMA1_Channel7_IRQn);
    }
}

void DMA1_Channel7_IRQHandler(void)
{

    NVIC_ClearPendingIRQ(DMA1_Channel7_IRQn);

    if (DMA1->ISR & DMA_ISR_TCIF7)
    {

        DMA1->IFCR = DMA_IFCR_CTCIF7;

        on_complete_transfer();
    }

    DMA1->IFCR = DMA_IFCR_CGIF7;
}

void DMA1_Channel4_IRQHandler(void)
{

    NVIC_ClearPendingIRQ(DMA1_Channel4_IRQn);

    if (DMA1->ISR & DMA_ISR_TCIF4)
    {

        DMA1->IFCR = DMA_IFCR_CTCIF4;

        on_complete_transfer();
    }

    DMA1->IFCR = DMA_IFCR_CGIF4;
}
