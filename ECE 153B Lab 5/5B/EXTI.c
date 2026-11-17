#include "EXTI.h"

#include "DAC.h"

void EXTI_Init(void)
{

    // Configure EXTI for the user button (PC13, active low, external pull-up on the Nucleo board)

    RCC->AHB2ENR |= RCC_AHB2ENR_GPIOCEN;

    RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;

    // PC13 input, no internal pull (the board already has a pull-up)

    GPIOC->MODER &= ~GPIO_MODER_MODE13;

    GPIOC->PUPDR &= ~GPIO_PUPDR_PUPD13;

    // Route EXTI13 to port C

    SYSCFG->EXTICR[3] &= ~SYSCFG_EXTICR4_EXTI13;

    SYSCFG->EXTICR[3] |= SYSCFG_EXTICR4_EXTI13_PC;

    // Falling edge = button press

    EXTI->RTSR1 &= ~EXTI_RTSR1_RT13;

    EXTI->FTSR1 |= EXTI_FTSR1_FT13;

    // Unmask and clear any stale pending flag

    EXTI->IMR1 |= EXTI_IMR1_IM13;

    EXTI->PR1 = EXTI_PR1_PIF13;

    NVIC_SetPriority(EXTI15_10_IRQn, 0);

    NVIC_EnableIRQ(EXTI15_10_IRQn);
}

#define DAC_MIN 0

#define DAC_MAX 4095

#define DAC_INCREMENT 256

static uint32_t dac_value = 0;

static enum
{

    DOWN,

    UP,

} direction = UP;

// Each button press steps the DAC output up to max, then back down to min

void EXTI15_10_IRQHandler(void)
{

    if ((EXTI->PR1 & EXTI_PR1_PIF13) == 0)
        return;

    EXTI->PR1 = EXTI_PR1_PIF13; // clear pending flag (write 1 to clear)

    if (direction == UP)
    {

        if (dac_value + DAC_INCREMENT >= DAC_MAX)
        {

            dac_value = DAC_MAX;

            direction = DOWN;
        }
        else
        {

            dac_value += DAC_INCREMENT;
        }
    }

    else
    {

        if (dac_value <= DAC_MIN + DAC_INCREMENT)
        {

            dac_value = DAC_MIN;

            direction = UP;
        }
        else
        {

            dac_value -= DAC_INCREMENT;
        }
    }

    DAC_Write_Value(dac_value);
}
