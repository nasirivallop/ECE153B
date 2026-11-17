#include "DAC.h"

#include "stm32l476xx.h"

static void DAC_Pin_Init(void)
{

    // [TODO]

    // Enable GPIOA clock

    RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;

    GPIOA->MODER |= GPIO_MODER_MODE4; // analog mode (11)

    GPIOA->PUPDR &= ~GPIO_PUPDR_PUPD4;
}

void DAC_Write_Value(uint32_t value)
{

    // [TODO] Write DAC1Ch1 right-aligned 12-bit value

    if (value > 0xfff) // corrects value if over 2^12-1

        value = 0xfff;

    DAC->DHR12R1 = value;
}

void DAC_Init(void)
{

    DAC_Pin_Init();

    // [TODO] Enable DAC clock

    RCC->APB1ENR1 |= RCC_APB1ENR1_DAC1EN;

    DAC1->CR &= ~DAC_CR_EN1;

    // [TODO] Enable software trigger mode

    DAC->CR &= ~(DAC_CR_TSEL1);

    DAC->CR |= DAC_CR_TSEL1;

    // [TODO] Disable trigger

    DAC->CR &= ~DAC_CR_TEN1;

    // [TODO] DAC1 connected in normal mode to external pin only with buffer enabled

    DAC1->MCR &= ~DAC_MCR_MODE1; // MODE1 = 0b000 = normal mode with buffer

    // [TODO] Enable DAC channel 1

    DAC1->CR |= DAC_CR_EN1;

    DAC_Write_Value(0);
}