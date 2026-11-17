/*

* ECE 153B

*

* Name(s):

* Section:

* Project

*/

#include "stm32l476xx.h"

#include "motor.h"

// Clears only the motor pins (PC5, PC6, PC8, PC9) when ANDed with ODR

static const uint32_t MASK = ~(GPIO_ODR_OD5 | GPIO_ODR_OD6 | GPIO_ODR_OD8 | GPIO_ODR_OD9);

static const uint32_t HalfStep[8] = {

    GPIO_ODR_OD5 | GPIO_ODR_OD9,

    GPIO_ODR_OD5,

    GPIO_ODR_OD5 | GPIO_ODR_OD8,

    GPIO_ODR_OD8,

    GPIO_ODR_OD6 | GPIO_ODR_OD8,

    GPIO_ODR_OD6,

    GPIO_ODR_OD6 | GPIO_ODR_OD9,

    GPIO_ODR_OD9

}; // TODO

static volatile int8_t dire = 0;

static volatile int step = 0;

void Motor_Init(void)
{

    RCC->AHB2ENR |= RCC_AHB2ENR_GPIOCEN;

    GPIOC->MODER &= ~GPIO_MODER_MODE5;

    GPIOC->MODER &= ~GPIO_MODER_MODE6;

    GPIOC->MODER &= ~GPIO_MODER_MODE8;

    GPIOC->MODER &= ~GPIO_MODER_MODE9;

    GPIOC->MODER |= GPIO_MODER_MODE5_0;

    GPIOC->MODER |= GPIO_MODER_MODE6_0;

    GPIOC->MODER |= GPIO_MODER_MODE8_0;

    GPIOC->MODER |= GPIO_MODER_MODE9_0;

    GPIOC->OSPEEDR &= ~GPIO_OSPEEDR_OSPEED5;

    GPIOC->OSPEEDR &= ~GPIO_OSPEEDR_OSPEED6;

    GPIOC->OSPEEDR &= ~GPIO_OSPEEDR_OSPEED8;

    GPIOC->OSPEEDR &= ~GPIO_OSPEEDR_OSPEED9;

    GPIOC->OSPEEDR |= GPIO_OSPEEDR_OSPEED5_1;

    GPIOC->OSPEEDR |= GPIO_OSPEEDR_OSPEED6_1;

    GPIOC->OSPEEDR |= GPIO_OSPEEDR_OSPEED8_1;

    GPIOC->OSPEEDR |= GPIO_OSPEEDR_OSPEED9_1;

    GPIOC->OTYPER &= ~GPIO_OTYPER_OT5;

    GPIOC->OTYPER &= ~GPIO_OTYPER_OT6;

    GPIOC->OTYPER &= ~GPIO_OTYPER_OT8;

    GPIOC->OTYPER &= ~GPIO_OTYPER_OT9;

    GPIOC->PUPDR &= ~GPIO_PUPDR_PUPD5;

    GPIOC->PUPDR &= ~GPIO_PUPDR_PUPD6;

    GPIOC->PUPDR &= ~GPIO_PUPDR_PUPD8;

    GPIOC->PUPDR &= ~GPIO_PUPDR_PUPD9;
}

void rotate(void)
{

    // TODO

    if (step > 7)
    { // CW

        step = 0;
    }
    else if (step < 0)
    { // CCW

        step = 7;
    }

    GPIOC->ODR &= MASK;

    GPIOC->ODR |= HalfStep[step];

    step += dire;
}

void setDire(int8_t direction)
{

    dire = direction;
}
