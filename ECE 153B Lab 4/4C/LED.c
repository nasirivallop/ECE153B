/*


ECE 153B*

Name(s): Nate Sirivallop, Patrick Lee

Section: Wednesday 6pm

Lab: 3A*/

#include "LED.h"

void LED_Init(void)
{

    // Enable GPIO Clocks

    // [TODO]

    RCC->AHB2ENR |= RCC_AHB2ENR_GPIOCEN;

    RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;

    // Initialize Green LED

    // [TODO]

    GPIOA->MODER &= ~GPIO_MODER_MODE5; // Clear mode bits

    GPIOA->MODER |= GPIO_MODER_MODE5_0; // set bit 10 == pin 5 as output1

    GPIOA->OTYPER &= ~GPIO_OTYPER_OT5; // push pull

    GPIOA->PUPDR &= ~GPIO_PUPDR_PUPD5; // pull up pull down, no pu no pd
}

void Green_LED_Off(void)
{

    GPIOA->ODR &= ~GPIO_ODR_OD5;
}

void Green_LED_On(void)
{

    GPIOA->ODR |= GPIO_ODR_OD5;
}

void Green_LED_Toggle(void)
{

    GPIOA->ODR ^= GPIO_ODR_OD5;
}