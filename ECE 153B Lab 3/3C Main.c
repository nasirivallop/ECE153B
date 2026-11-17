/*

* ECE 153B

*

* Name(s):

* Section:

* Lab: 3C

*/

#include <stdio.h>

#include "stm32l476xx.h"

uint32_t volatile currentValue = 0;

uint32_t volatile lastValue = 0;

uint32_t volatile overflowCount = 0;

uint32_t volatile timeInterval = 0;

uint32_t volatile clockPeriod = 1;

uint32_t volatile clk = 0;

void Input_Capture_Setup()
{

    // [TODO]

    RCC->AHB2ENR |= RCC_AHB2ENR_GPIOBEN;

    GPIOB->MODER &= ~GPIO_MODER_MODE6;

    GPIOB->MODER |= GPIO_MODER_MODE6_1;

    GPIOB->AFR[0] &= ~GPIO_AFRL_AFSEL6;

    GPIOB->AFR[0] |= GPIO_AFRL_AFSEL6_1;

    GPIOB->PUPDR &= ~GPIO_PUPDR_PUPD6;

    RCC->APB1ENR1 |= RCC_APB1ENR1_TIM4EN;

    TIM4->PSC = 15;

    TIM4->CR1 |= TIM_CR1_ARPE;

    TIM4->ARR = TIM_ARR_ARR;

    TIM4->CCMR1 &= ~TIM_CCMR1_CC1S;

    TIM4->CCMR1 |= TIM_CCMR1_CC1S_0;

    TIM4->CCER &= ~(TIM_CCER_CC1NP | TIM_CCER_CC1P);

    TIM4->CCER |= TIM_CCER_CC1E;

    TIM4->DIER |= (TIM_DIER_CC1IE | TIM_DIER_UIE); // no DMA request needed (UDE removed)

    TIM4->EGR |= TIM_EGR_UG;

    TIM4->SR &= ~TIM_SR_UIF;

    TIM4->CR1 &= ~TIM_CR1_DIR;

    TIM4->CR1 |= TIM_CR1_CEN;

    NVIC_EnableIRQ(TIM4_IRQn);

    NVIC_SetPriority(TIM4_IRQn, 2);
}

void TIM4_IRQHandler(void)
{

    // [TODO]

    if (TIM4->SR & TIM_SR_UIF)
    {

        TIM4->SR &= ~TIM_SR_UIF;

        overflowCount++;

        return;
    }

    if (TIM4->SR & TIM_SR_CC1IF)
    {

        TIM4->SR &= ~TIM_SR_CC1IF;

        clk++;

        if (clk % 2 == 1)
        {

            lastValue = TIM4->CCR1;

            overflowCount = 0;

            TIM4->CCER |= TIM_CCER_CC1P;

            return;
        }

        if (clk % 2 == 0)
        {

            currentValue = TIM4->CCR1;

            timeInterval = clockPeriod * (((1 + TIM4->ARR) * overflowCount + currentValue) - (lastValue));

            TIM4->CCER &= ~TIM_CCER_CC1P;

            return;
        }

        return;
    }
}

void Trigger_Setup()
{

    // [TODO]

    RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;

    GPIOA->MODER &= ~GPIO_MODER_MODE9;

    GPIOA->MODER |= GPIO_MODER_MODE9_1;

    GPIOA->OSPEEDR &= ~GPIO_OSPEEDR_OSPEED9;

    GPIOA->OSPEEDR |= GPIO_OSPEEDR_OSPEED9_0;

    GPIOA->OSPEEDR |= GPIO_OSPEEDR_OSPEED9_1;

    GPIOA->PUPDR &= ~GPIO_PUPDR_PUPD9;

    GPIOA->AFR[1] &= ~GPIO_AFRH_AFSEL9;

    GPIOA->AFR[1] |= GPIO_AFRH_AFSEL9_0;

    RCC->APB2ENR |= RCC_APB2ENR_TIM1EN;

    TIM1->PSC = 15;

    TIM1->CR1 |= TIM_CR1_ARPE;

    TIM1->ARR = TIM_ARR_ARR;

    TIM1->CCR2 = 15;

    TIM1->CCMR1 &= ~TIM_CCMR1_OC2M;

    TIM1->CCMR1 |= (TIM_CCMR1_OC2M_2 | TIM_CCMR1_OC2M_1);

    TIM1->CCMR1 |= TIM_CCMR1_OC2PE;

    TIM1->CCER |= TIM_CCER_CC2E;

    TIM1->BDTR |= (TIM_BDTR_MOE | TIM_BDTR_OSSR | TIM_BDTR_OSSI);

    TIM1->EGR |= TIM_EGR_UG;

    TIM1->SR &= ~TIM_SR_UIF; // no TIM1 interrupt handler, so don't enable UIE

    TIM1->CR1 &= ~TIM_CR1_DIR;

    TIM1->CCER &= ~TIM_CCER_CC2P;

    TIM1->CR1 |= TIM_CR1_CEN;
}

int main(void)
{

    double distance;

    // Enable High Speed Internal Clock (HSI = 16 MHz)

    RCC->CR |= RCC_CR_HSION;

    while ((RCC->CR & RCC_CR_HSIRDY) == 0)
        ; // Wait until HSI is ready

    // Select HSI as system clock source

    RCC->CFGR &= ~RCC_CFGR_SW;

    RCC->CFGR |= RCC_CFGR_SW_HSI;

    while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_HSI)
        ; // Wait until HSI is system clock source

    // Input Capture Setup

    Input_Capture_Setup();

    // Trigger Setup

    Trigger_Setup();

    while (1)
    {

        // [TODO] Store your measurements on Stack

        distance = timeInterval / 58.0;

        if (distance > 50)
        {

            distance = 0;
        }

        else if (distance < 5)
        {

            distance = 0;
        }
    }
}