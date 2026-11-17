/*

* ECE 153B

*

* Name(s):

* Section:

* Lab: 5A

*/

#include "ADC.h"

#include "PWM.h"

#include "SysClock.h"

#include "stm32l476xx.h"

#include <stdio.h>

volatile uint32_t measured = 0;

volatile double vIn = 0;

int main(void)
{

    // Initialization

    System_Clock_Init(); // Switch System Clock = 16 MHz

    ADC_Init();

    // [TODO] Initialize PWM

    PWM_Init();

    while (1)
    {

        // [TODO] Trigger ADC and get result

        ADC1->CR |= ADC_CR_ADSTART;

        // Wait for end of conversion (EOC is cleared by reading DR)

        while ((ADC1->ISR & ADC_ISR_EOC) == 0)
            ;

        measured = ADC1->DR;

        ADC1->ISR = ADC_ISR_EOS; // clear end-of-sequence flag (write 1 to clear)

        vIn = 3.3 / 4096.0 * measured;

        // [TODO] LED behavior based on ADC result

        set_LED_Voltage(vIn, 3.3);
    }
}