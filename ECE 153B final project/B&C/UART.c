/*

* ECE 153B

*

* Name(s):

* Section:

* Project

*

* USART1 (Bluetooth, PB6 = TX, PB7 = RX) or USART2 (ST-Link, PA2/PA3)

* TX uses DMA (non-blocking), RX uses the RXNE interrupt.

* A full line received over UART is handed to UART_Result() from the main loop

* (via UART_process_input()) so it never runs inside an interrupt.

*/

#include "UART.h"

#include "DMA.h"

#include <string.h>

#ifndef IO_SIZE
#define IO_SIZE 128
#endif

void UART_Result(char *inputs, uint32_t size); // implemented in Main.c

void DMA_Init_UARTx(DMA_Channel_TypeDef *tx, USART_TypeDef *uart);

void on_complete_transfer(void);

void UART_process_input(void);

static USART_TypeDef *activeUART = USART1;

static DMA_Channel_TypeDef *activeTx = DMA1_Channel4;

// ---------------- TX state ----------------

static char txActive[IO_SIZE]; // buffer the DMA is currently sending

static char txPending[IO_SIZE]; // text queued while DMA is busy

static volatile uint32_t pendingLen = 0;

static volatile uint8_t txBusy = 0;

// ---------------- RX state ----------------

static char rxBuffer[IO_SIZE];

static volatile uint32_t rxCount = 0;

static char rxLine[IO_SIZE];

static volatile uint32_t rxLineLen = 0;

static volatile uint8_t rxLineReady = 0;

static void UART1_GPIO_Config(void)
{

    RCC->AHB2ENR |= RCC_AHB2ENR_GPIOBEN;

    // PB6, PB7 alternate function 7 (USART1)

    GPIOB->MODER &= ~(GPIO_MODER_MODE6 | GPIO_MODER_MODE7);

    GPIOB->MODER |= (GPIO_MODER_MODE6_1 | GPIO_MODER_MODE7_1);

    GPIOB->AFR[0] &= ~(GPIO_AFRL_AFSEL6 | GPIO_AFRL_AFSEL7);

    GPIOB->AFR[0] |= (7UL << 24) | (7UL << 28);

    GPIOB->OSPEEDR |= (GPIO_OSPEEDR_OSPEED6 | GPIO_OSPEEDR_OSPEED7);

    GPIOB->OTYPER &= ~(GPIO_OTYPER_OT6 | GPIO_OTYPER_OT7);

    GPIOB->PUPDR &= ~(GPIO_PUPDR_PUPD6 | GPIO_PUPDR_PUPD7);

    GPIOB->PUPDR |= (GPIO_PUPDR_PUPD6_0 | GPIO_PUPDR_PUPD7_0);
}

static void UART2_GPIO_Config(void)
{

    RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;

    // PA2, PA3 alternate function 7 (USART2)

    GPIOA->MODER &= ~(GPIO_MODER_MODE2 | GPIO_MODER_MODE3);

    GPIOA->MODER |= (GPIO_MODER_MODE2_1 | GPIO_MODER_MODE3_1);

    GPIOA->AFR[0] &= ~(GPIO_AFRL_AFSEL2 | GPIO_AFRL_AFSEL3);

    GPIOA->AFR[0] |= (7UL << 8) | (7UL << 12);

    GPIOA->OSPEEDR |= (GPIO_OSPEEDR_OSPEED2 | GPIO_OSPEEDR_OSPEED3);

    GPIOA->OTYPER &= ~(GPIO_OTYPER_OT2 | GPIO_OTYPER_OT3);

    GPIOA->PUPDR &= ~(GPIO_PUPDR_PUPD2 | GPIO_PUPDR_PUPD3);

    GPIOA->PUPDR |= (GPIO_PUPDR_PUPD2_0 | GPIO_PUPDR_PUPD3_0);
}

static void USART_Config(USART_TypeDef *USARTx)
{

    USARTx->CR1 &= ~USART_CR1_UE; // Disable before configuring

    USARTx->CR1 &= ~(USART_CR1_M0 | USART_CR1_M1); // 8 data bits

    USARTx->CR1 &= ~USART_CR1_OVER8; // Oversampling by 16

    USARTx->CR2 &= ~USART_CR2_STOP; // 1 stop bit

    USARTx->BRR = 8333; // 80 MHz / 9600 baud

    USARTx->CR3 |= USART_CR3_DMAT; // TX through DMA

    USARTx->CR1 |= USART_CR1_RXNEIE; // RX interrupt

    USARTx->CR1 |= (USART_CR1_TE | USART_CR1_RE);

    USARTx->CR1 |= USART_CR1_UE;

    while ((USARTx->ISR & USART_ISR_TEACK) == 0)
        ;

    while ((USARTx->ISR & USART_ISR_REACK) == 0)
        ;
}

void UART1_Init(void)
{

    RCC->APB2ENR |= RCC_APB2ENR_USART1EN;

    RCC->CCIPR &= ~RCC_CCIPR_USART1SEL;

    RCC->CCIPR |= RCC_CCIPR_USART1SEL_0; // SYSCLK

    UART1_GPIO_Config();

    activeUART = USART1;

    activeTx = DMA1_Channel4;

    DMA_Init_UARTx(activeTx, activeUART);

    USART_Config(USART1);

    // Lower priority (higher number) than DMA so DMA can always finish

    NVIC_SetPriority(USART1_IRQn, 3);

    NVIC_EnableIRQ(USART1_IRQn);
}

void UART2_Init(void)
{

    RCC->APB1ENR1 |= RCC_APB1ENR1_USART2EN;

    RCC->CCIPR &= ~RCC_CCIPR_USART2SEL;

    RCC->CCIPR |= RCC_CCIPR_USART2SEL_0; // SYSCLK

    UART2_GPIO_Config();

    activeUART = USART2;

    activeTx = DMA1_Channel7;

    DMA_Init_UARTx(activeTx, activeUART);

    USART_Config(USART2);

    NVIC_SetPriority(USART2_IRQn, 3);

    NVIC_EnableIRQ(USART2_IRQn);
}

// Must be called with txBusy already set

static void start_tx(uint32_t len)
{

    activeTx->CCR &= ~DMA_CCR_EN;

    activeTx->CMAR = (uint32_t)txActive;

    activeTx->CNDTR = len;

    activeTx->CCR |= DMA_CCR_EN;
}

// Non-blocking print (blocks only if both buffers are full)

void UART_print(char *data)
{

    uint32_t len = strlen(data);

    if (len == 0)
        return;

    if (len > IO_SIZE)
        len = IO_SIZE;

    while (1)
    {

        __disable_irq();

        if (!txBusy)
        {

            memcpy(txActive, data, len);

            txBusy = 1;

            start_tx(len);

            __enable_irq();

            return;
        }

        if (pendingLen + len <= IO_SIZE)
        {

            memcpy(&txPending[pendingLen], data, len);

            pendingLen += len;

            __enable_irq();

            return;
        }

        __enable_irq(); // queue full: let the DMA interrupt drain it, then retry
    }
}

// Called from the DMA transfer-complete interrupt

void on_complete_transfer(void)
{

    if (pendingLen > 0)
    {

        uint32_t len = pendingLen;

        memcpy(txActive, txPending, len);

        pendingLen = 0;

        start_tx(len);
    }
    else
    {

        txBusy = 0;
    }
}

static void USART_RX_Handler(USART_TypeDef *USARTx)
{

    if (USARTx->ISR & USART_ISR_ORE)
    {

        USARTx->ICR = USART_ICR_ORECF; // clear overrun so RX keeps working
    }

    if (USARTx->ISR & USART_ISR_RXNE)
    {

        char c = (char)(USARTx->RDR & 0xFF);

        if (c == '\r' || c == '\n')
        {

            if (rxCount > 0 && !rxLineReady)
            {

                memcpy(rxLine, rxBuffer, rxCount);

                rxLine[rxCount] = '\0';

                rxLineLen = rxCount;

                rxLineReady = 1;
            }

            rxCount = 0;
        }

        else if (rxCount < IO_SIZE - 1)
        {

            rxBuffer[rxCount++] = c;
        }
    }
}

void USART1_IRQHandler(void)
{

    USART_RX_Handler(USART1);
}

void USART2_IRQHandler(void)
{

    USART_RX_Handler(USART2);
}

// Call from the main loop: runs UART_Result() on a complete received line

void UART_process_input(void)
{

    char line[IO_SIZE];

    uint32_t len;

    if (!rxLineReady)
        return;

    __disable_irq();

    len = rxLineLen;

    memcpy(line, rxLine, len + 1);

    rxLineReady = 0;

    __enable_irq();

    UART_Result(line, len);
}
