/*

* ECE 153B

*

* Name(s):

* Section:

* Lab: 4C

*/

#include "stm32l476xx.h"

#include "LED.h"

#include "SysClock.h"

#include "SysTimer.h"

#include "UART.h"

#include "SPI.h"

#include "accelerometer.h"

#include <stdio.h>

// Initializes USARTx

// USART2: UART Communication with Termite

// USART1: Bluetooth Communication with Phone

void Init_USARTx(int x)
{

    if (x == 1)
    {

        UART1_Init();

        UART1_GPIO_Init();

        USART_Init(USART1);
    }
    else if (x == 2)
    {

        UART2_Init();

        UART2_GPIO_Init();

        USART_Init(USART2);
    }
}

int main(void)
{

    double x, y, z;

    System_Clock_Init(); // System Clock = 80 MHz

    SysTick_Init();

    LED_Init();

    Init_USARTx(2);

    // Initialize SPI and the ADXL345 accelerometer

    SPI1_GPIO_Init();

    SPI1_Init();

    initAcc();

    printf("Program Starts.\r\n");

    while (1)
    {

        readValues(&x, &y, &z);

        printf("Acceleration: %.2f, %.2f, %.2f\r\n", x, y, z);

        Green_LED_Toggle();

        delay(1000);
    }
}
