/*

ECE 153B

Name(s): Nate Sirivallop, Patrick Lee

Section: Wednesday 6pm

Lab: 3A

*/

#include "stm32l476xx.h"

#include "LED.h"

#include "SysClock.h"

#include "SysTimer.h"

int main(void)
{

    // Initialization

    System_Clock_Init();

    SysTick_Init();

    LED_Init();

    while (1)
    {

        Green_LED_Toggle();

        delay(1000); // 1 s
    }
}
