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

#include "LED.h"

#include "DMA.h"

#include "UART.h"

#include "motor.h"

#include "SPI.h"

#include "I2C.h"

#include "accelerometer.h"

#include <stdio.h>

#include <string.h>

#include <ctype.h>

#define DOWN 0 // Closing state for door

#define UP 1 // Opening state for door

#define STOP 2 // Door is not moving

#ifndef IO_SIZE
#define IO_SIZE 128
#endif

void UART_process_input(void); // UART.c: calls UART_Result() when a full line arrives

static char buffer[IO_SIZE];

static int dir = STOP;

static int ovr = 0;

static uint32_t ovrStart = 0;

static uint32_t lastReport = 0;

double x, y, z;

uint8_t currTemp;

uint8_t prevTemp = 21;

void checkOverride(void)
{

    if (!ovr)
        return;

    uint32_t now = get_msTicks();

    if (now - lastReport >= 200)
    {

        sprintf(buffer, "x=%.2f y=%.2f z=%.2f | Temp: curr=%d prev=%d\n", x, y, z, (unsigned int)currTemp, (unsigned int)prevTemp);

        UART_print(buffer);

        lastReport = now;
    }

    if ((dir == UP && z > .95) || (dir == DOWN && y < -.95))
    {

        setDire(STOP);

        dir = STOP;

        sprintf(buffer, "Target reached, stopping\n");

        UART_print(buffer);
    }

    if (now - ovrStart >= 4000)
    {

        ovr = 0;

        setDire(STOP);

        dir = STOP;

        sprintf(buffer, "Override done\n");

        UART_print(buffer);
    }
}

void UART_Result(char *inputs, uint32_t size)
{

    for (uint32_t i = 0; i < size; i++)
    {

        if (inputs[i] == '\r' || inputs[i] == '\n' || inputs[i] == '\0')
        {

            inputs[i] = '\0';

            break;
        }

        inputs[i] = (char)tolower((unsigned char)inputs[i]);
    }

    inputs[size] = '\0'; // make sure strcmp always sees a terminated string

    if (strcmp(inputs, "up") == 0)
    {

        if (z > 0.8)
        {

            sprintf(buffer, "Already open\n");

            UART_print(buffer);
        }
        else
        {

            setDire(UP);

            dir = UP;

            ovr = 1;

            ovrStart = get_msTicks();

            lastReport = ovrStart;

            sprintf(buffer, "Opening\n");

            UART_print(buffer);
        }
    }

    else if (strcmp(inputs, "down") == 0)
    {

        if (y < -0.8)
        {

            sprintf(buffer, "Already closed\n");

            UART_print(buffer);
        }
        else
        {

            setDire(DOWN);

            dir = DOWN;

            ovr = 1;

            ovrStart = get_msTicks();

            lastReport = ovrStart;

            sprintf(buffer, "Closing\n");

            UART_print(buffer);
        }
    }

    else if (strcmp(inputs, "stop") == 0)
    {

        setDire(STOP);

        dir = STOP;

        ovr = 1;

        ovrStart = get_msTicks();

        lastReport = ovrStart;

        sprintf(buffer, "Stopping\n");

        UART_print(buffer);
    }

    else
    {

        sprintf(buffer, "Invalid command. Try: Up, Down, Stop.\n");

        UART_print(buffer);
    }
}

int main(void)
{

    uint8_t addrI2C;

    uint8_t DR;

    uint8_t DS;

    int initialState = 0;

    System_Clock_Init();

    Motor_Init();

    SysTick_Init();

    UART1_Init();

    LED_Init();

    SPI1_GPIO_Init();

    SPI1_Init();

    initAcc();

    I2C_GPIO_Init();

    I2C_Initialization();

    sprintf(buffer, "Program Starts.\r\n");

    UART_print(buffer);

    while (1)
    {

        addrI2C = 0b1001000 << 1;

        DS = 0x00; // Send this to write data

        I2C_SendData(I2C1, addrI2C, &DS, 1);

        I2C_ReceiveData(I2C1, addrI2C, &DR, 1);

        readValues(&x, &y, &z); // Read accelerometer

        currTemp = DR;

        UART_process_input(); // Handle any "up"/"down"/"stop" command from Bluetooth

        checkOverride();

        if (currTemp != prevTemp)
        {

            sprintf(buffer, "Temp: curr=%d prev=%d\n", (unsigned int)currTemp, (unsigned int)prevTemp);

            UART_print(buffer);
        }

        if (!ovr)
        {

            if ((currTemp >= 29) && (dir != UP) && (z < .9) && (prevTemp >= 28))
            {

                prevTemp = currTemp;

                setDire(UP);

                dir = UP;

                sprintf(buffer, "Temp: %d, Opening\n", (unsigned int)currTemp);

                UART_print(buffer);
            }

            else if ((currTemp <= 24) && (dir != DOWN) && (y > -0.90) && (prevTemp <= 25))
            {

                prevTemp = currTemp;

                setDire(DOWN);

                dir = DOWN;

                sprintf(buffer, "Temp: %d, Closing\n", (unsigned int)currTemp);

                UART_print(buffer);
            }

            else if (((dir == UP) && (z >= .95)) || ((dir == DOWN) && (y <= -0.95)))
            {

                prevTemp = currTemp; // Update the previous temperature

                setDire(STOP);

                dir = STOP;

                sprintf(buffer, "Temp: %d, Idle\n", (unsigned int)currTemp);

                UART_print(buffer);

                initialState++;
            }

            else
            {

                sprintf(buffer, "x=%.2f y=%.2f z=%.2f\n", x, y, z);

                UART_print(buffer);

                prevTemp = currTemp;
            }
        }

        LED_Toggle();

        delay(200);
    }
}