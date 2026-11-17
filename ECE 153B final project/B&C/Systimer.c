/*

* ECE 153B

*

* Name(s):

* Section:

* Project

*/

#include "SysTimer.h"

#include "motor.h"

static uint32_t volatile msTicks = 0; // global millisecond ticks

static uint32_t volatile stepperTicks = 0; // clock for stepper motor

static uint32_t volatile checkTick = 1; // rotate the motor every checkTick ms

void SysTick_Init(void)
{

    SysTick->CTRL = 0; // Disable SysTick IRQ and SysTick Counter

    SysTick->LOAD = 79999; // 80 MHz / 80000 = 1 kHz -> 1 ms tick

    SysTick->VAL = 0;

    // Enables SysTick exception request

    SysTick->CTRL |= SysTick_CTRL_TICKINT_Msk;

    // Select clock source

    // If CLKSOURCE = 0, the external clock is used. The frequency of SysTick clock is the

    // frequency of the AHB clock divided by 8.

    // If CLKSOURCE = 1, the processor clock is used.

    SysTick->CTRL |= SysTick_CTRL_CLKSOURCE_Msk;

    // SysTick interrupt in NVIC

    NVIC_SetPriority(SysTick_IRQn, 1); // Set Priority to 1

    NVIC_EnableIRQ(SysTick_IRQn);

    // Enable SysTick IRQ and SysTick Timer

    SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;
}

void SysTick_Handler(void)
{

    ++stepperTicks;

    if (stepperTicks >= checkTick)
    {

        stepperTicks = 0;

        rotate();
    }

    msTicks++;
}

uint32_t get_msTicks(void)
{

    return msTicks;
}

void delay(uint32_t ms)
{

    // SysTick must stay enabled the whole time: it also drives the stepper motor

    // and get_msTicks(). Unsigned subtraction keeps this correct across overflow.

    uint32_t start = msTicks;

    while ((msTicks - start) < ms)
        ;
}
