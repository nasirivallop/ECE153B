#include "PWM.h"

// [TODO]

void PWM_Init(void)
{

    // Enable clocks

    RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN; // Assuming LED is on PA5

    RCC->APB1ENR1 |= RCC_APB1ENR1_TIM2EN; // Assuming TIM2

    // Set PA5 to AF mode for TIM2_CH1

    GPIOA->MODER &= ~GPIO_MODER_MODE5;

    GPIOA->MODER |= GPIO_MODER_MODE5_1; // Alternate function

    GPIOA->OSPEEDR |= GPIO_OSPEEDR_OSPEED5;

    GPIOA->PUPDR &= ~GPIO_PUPDR_PUPD5;

    GPIOA->AFR[0] &= ~GPIO_AFRL_AFSEL5;

    GPIOA->AFR[0] |= GPIO_AFRL_AFSEL5_0; // AF1 = TIM2_CH1

    // Configure TIM2

    TIM2->CR1 &= ~TIM_CR1_DIR;

    TIM2->PSC = 15; // 16 MHz / (15 + 1) = 1 MHz timer clock

    TIM2->ARR = 999; // 1 MHz / (999 + 1) = 1 kHz PWM frequency

    // TIM2->CCMR1 &= ~TIM_CCMR1_CC1S;

    // TIM2->CCMR1 |= 0b00;

    TIM2->CCMR1 &= ~TIM_CCMR1_OC1M;

    TIM2->CCMR1 |= TIM_CCMR1_OC1M_2 | TIM_CCMR1_OC1M_1;

    TIM2->CCMR1 |= TIM_CCMR1_OC1PE; // enable output preload

    TIM2->CCER &= ~TIM_CCER_CC1P;

    TIM2->CCER |= TIM_CCER_CC1E; // Enable the channel 1 output

    TIM2->CCR1 = 0; // was TIM1 (wrong timer)

    TIM2->CR1 |= TIM_CR1_CEN;
}

void set_LED_Voltage(double voltage, double maxVoltage)
{

    TIM2->CCR1 = (uint32_t)(voltage / maxVoltage * (TIM2->ARR + 1));
}