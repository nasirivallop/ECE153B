/*

* ECE 153B

*

* Name(s):

* Section:

* Lab: 6C

*/

#include "DMA.h"

#include "CRC.h"

void completeCRC(uint32_t crc); // Main.c

void DMA_Init(void)
{

    volatile uint32_t wait;

    RCC->AHB1ENR |= RCC_AHB1ENR_DMA1EN;

    // delay for ~20us

    for (wait = 0; wait < 2000; wait++)
        ;

    // Disable channel

    DMA1_Channel6->CCR &= ~DMA_CCR_EN;

    // mem to mem

    DMA1_Channel6->CCR |= DMA_CCR_MEM2MEM;

    // priority: high

    DMA1_Channel6->CCR &= ~DMA_CCR_PL;

    DMA1_Channel6->CCR |= DMA_CCR_PL_1;

    // periph size: 32-bit

    DMA1_Channel6->CCR &= ~DMA_CCR_PSIZE;

    DMA1_Channel6->CCR |= DMA_CCR_PSIZE_1;

    // mem size: 32-bit

    DMA1_Channel6->CCR &= ~DMA_CCR_MSIZE;

    DMA1_Channel6->CCR |= DMA_CCR_MSIZE_1;

    // disable periph increment mode

    DMA1_Channel6->CCR &= ~DMA_CCR_PINC;

    // enable memory increment mode

    DMA1_Channel6->CCR |= DMA_CCR_MINC;

    // disable circular mode

    DMA1_Channel6->CCR &= ~DMA_CCR_CIRC;

    // mem (source) to periph (destination)

    DMA1_Channel6->CCR |= DMA_CCR_DIR;

    // source: data buffer

    DMA1_Channel6->CMAR = (uint32_t)DataBuffer;

    // destination: CRC data register

    DMA1_Channel6->CPAR = (uint32_t) & (CRC->DR);

    // Disable half transfer interrupt

    DMA1_Channel6->CCR &= ~DMA_CCR_HTIE;

    // Disable transfer error interrupt

    DMA1_Channel6->CCR &= ~DMA_CCR_TEIE;

    // Enable transfer complete interrupt

    DMA1_Channel6->CCR |= DMA_CCR_TCIE;

    // Set interrupt priority to 0 in NVIC

    NVIC_SetPriority(DMA1_Channel6_IRQn, 0);

    // Enable interrupt in NVIC

    NVIC_EnableIRQ(DMA1_Channel6_IRQn);
}

void DMA1_Channel6_IRQHandler(void)
{

    NVIC_ClearPendingIRQ(DMA1_Channel6_IRQn);

    if (DMA1->ISR & DMA_ISR_TCIF6)
    {

        DMA1->IFCR = DMA_IFCR_CTCIF6;

        completeCRC(CRC->DR);
    }

    DMA1->IFCR = DMA_IFCR_CGIF6;
}
