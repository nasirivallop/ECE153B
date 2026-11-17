/*

* ECE 153B

*

* Name(s):

* Section:

* Project

*/

#include "stm32l476xx.h"

#include "SysClock.h"

#include "SysTimer.h"

#include "UART.h"

#include "motor.h"

#include <stdio.h>

int main(void)
{

    char ch;

    // Switch System Clock = 80 MHz

    System_Clock_Init();

    Motor_Init();

    SysTick_Init();

    UART2_GPIO_Init();

    UART2_Init();

    USART_Init(USART2); // TODO

    printf("Program Starts.\r\n");

    while (1)
    {

        printf("Clockwise = a; Counter Clockwise = b; Stop = c\r\n");

        scanf(" %c", &ch); // leading space skips the Enter/newline characters

        if (ch == 'a')
        {

            setDire((int8_t)1);

            printf("Clockwise\n");
        }

        else if (ch == 'b')
        {

            setDire((int8_t)-1);

            printf("Counterclockwise\n");
        }

        else if (ch == 'c')
        {

            setDire((int8_t)0);

            printf("Stopped\n");
        }

        else
        {

            printf("Error\n");
        }
    }
}