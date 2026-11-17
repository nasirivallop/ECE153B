/*

* ECE 153B

*

* Name(s):

* Section:

* Project

*/

#include "stm32l476xx.h"

#include "motor.h"

#define MOTOR_DOWN 0

#define MOTOR_UP 1

static const uint32_t MASK = (GPIO_ODR_OD5 | GPIO_ODR_OD6 | GPIO_ODR_OD8 | GPIO_ODR_OD9);

static const uint32_t HalfStep[8] = {

    GPIO_ODR_OD6 | GPIO_ODR_OD8,

    GPIO_ODR_OD6,

    GPIO_ODR_OD6 | GPIO_ODR_OD9,

    GPIO_ODR_OD9,

    GPIO_ODR_OD5 | GPIO_ODR_OD9,

    GPIO_ODR_OD5,

    GPIO_ODR_OD5 | GPIO_ODR_OD8,

    GPIO_ODR_OD8

};

static volatile int8_t dire = 2; // start stopped

static volatile uint8_t step = 0;

void Motor_Init(void)
{

    RCC->AHB2ENR |= RCC_AHB2ENR_GPIOCEN;

    // PC5, PC6, PC8, PC9 as general purpose outputs

    GPIOC->MODER &= ~(GPIO_MODER_MODE5 | GPIO_MODER_MODE6 | GPIO_MODER_MODE8 | GPIO_MODER_MODE9);

    GPIOC->MODER |= (GPIO_MODER_MODE5_0 | GPIO_MODER_MODE6_0 | GPIO_MODER_MODE8_0 | GPIO_MODER_MODE9_0);

    // Fast speed

    GPIOC->OSPEEDR &= ~(GPIO_OSPEEDR_OSPEED5 | GPIO_OSPEEDR_OSPEED6 | GPIO_OSPEEDR_OSPEED8 | GPIO_OSPEEDR_OSPEED9);

    GPIOC->OSPEEDR |= (GPIO_OSPEEDR_OSPEED5_1 | GPIO_OSPEEDR_OSPEED6_1 | GPIO_OSPEEDR_OSPEED8_1 | GPIO_OSPEEDR_OSPEED9_1);

    // Push-pull

    GPIOC->OTYPER &= ~(GPIO_OTYPER_OT5 | GPIO_OTYPER_OT6 | GPIO_OTYPER_OT8 | GPIO_OTYPER_OT9);

    // No pull-up / pull-down

    GPIOC->PUPDR &= ~(GPIO_PUPDR_PUPD5 | GPIO_PUPDR_PUPD6 | GPIO_PUPDR_PUPD8 | GPIO_PUPDR_PUPD9);

    // All coils off

    GPIOC->ODR &= ~MASK;
}

void rotate(void)
{

    if (dire == MOTOR_UP)
    {

        step = (step + 1) & 7;
    }

    else if (dire == MOTOR_DOWN)
    {

        step = (step - 1) & 7;
    }

    // When stopped, keep the current step energized so the door holds its position

    // Only touch the motor pins, leave the rest of port C alone

    GPIOC->ODR = (GPIOC->ODR & ~MASK) | HalfStep[step];
}

void setDire(int8_t direction)
{

    dire = direction;
}
