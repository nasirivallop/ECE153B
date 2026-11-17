#include "ADC.h"

#include "stm32l476xx.h"

#include <stdint.h>

void ADC_Wakeup(void)
{

    volatile int wait_time; // volatile so the compiler cannot remove the wait loop

    // To start ADC operations, the following sequence should be applied

    // DEEPPWD = 0: ADC not in deep-power down

    // DEEPPWD = 1: ADC in deep-power-down (default reset state)

    if ((ADC1->CR & ADC_CR_DEEPPWD) == ADC_CR_DEEPPWD)

        ADC1->CR &= ~ADC_CR_DEEPPWD; // Exit deep power down mode if still in that state

    // Enable the ADC internal voltage regulator

    // Before performing any operation such as launching a calibration or enabling the ADC, the ADC

    // voltage regulator must first be enabled and the software must wait for the regulator start-up

    // time.

    ADC1->CR |= ADC_CR_ADVREGEN;

    // Wait for ADC voltage regulator start-up time

    // The software must wait for the startup time of the ADC voltage regulator (T_ADCVREG_STUP)

    // before launching a calibration or enabling the ADC.

    // T_ADCVREG_STUP = 20 us

    wait_time = 20 * (80000000 / 1000000); // over-waits at 16 MHz, which is safe

    while (wait_time != 0)
    {

        wait_time--;
    }
}

void ADC_Common_Configuration()
{

    // [TODO]

    RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;

    SYSCFG->CFGR1 |= SYSCFG_CFGR1_BOOSTEN;

    ADC123_COMMON->CCR |= ADC_CCR_VREFEN;

    ADC123_COMMON->CCR &= ~ADC_CCR_PRESC;

    ADC123_COMMON->CCR &= ~ADC_CCR_CKMODE;

    ADC123_COMMON->CCR |= ADC_CCR_CKMODE_0;

    ADC123_COMMON->CCR &= ~ADC_CCR_DUAL;
}

void ADC_Pin_Init(void)
{

    // [TODO]

    RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;

    // Set PA1 to analog mode (MODER1[1:0] = 11)

    GPIOA->MODER |= GPIO_MODER_MODE1; // Set bits 3:2 to 11

    // Disable pull-up/pull-down (PUPDR1[1:0] = 00)

    GPIOA->PUPDR &= ~GPIO_PUPDR_PUPD1; // Clear bits 3:2

    GPIOA->ASCR |= GPIO_ASCR_ASC1;
}

void ADC_Init(void)
{

    // Enable ADC Clock

    RCC->AHB2ENR |= RCC_AHB2ENR_ADCEN;

    // Reset ADC using RCC

    RCC->AHB2RSTR |= RCC_AHB2RSTR_ADCRST; // Assert reset

    RCC->AHB2RSTR &= ~RCC_AHB2RSTR_ADCRST; // Clear reset

    // Ensure ADC is disabled before configuration

    ADC1->CR &= ~ADC_CR_ADEN;

    // Other ADC Initialization

    ADC_Pin_Init();

    ADC_Common_Configuration();

    ADC_Wakeup();

    // Configure resolution and data alignment

    ADC1->CFGR &= ~ADC_CFGR_ALIGN; // right alignment

    ADC1->CFGR &= ~ADC_CFGR_RES; // 12bit resolution

    // Set sequence length = 1 and first channel = 6

    ADC1->SQR1 &= ~ADC_SQR1_L; // set sequence length to 1

    ADC1->SQR1 &= ~ADC_SQR1_SQ1; // clear SQ1 before writing channel 6 (0b00110)

    ADC1->SQR1 |= (ADC_SQR1_SQ1_1 | ADC_SQR1_SQ1_2);

    // Set SQ1 to channel 6

    // Ensure that channel 6 is set to single-ended mode

    ADC1->DIFSEL &= ~ADC_DIFSEL_DIFSEL_6;

    // Set the sampling time for channel 6 to be 24.5 ADC clock cycles

    ADC1->SMPR1 &= ~ADC_SMPR1_SMP6; // clear (was missing the ~, which wiped every other channel's setting)

    ADC1->SMPR1 |= (ADC_SMPR1_SMP6_0 | ADC_SMPR1_SMP6_1);

    // Ensure that the ADC is in single conversion mode and that hardware trigger detection is disabled

    ADC1->CFGR &= ~ADC_CFGR_CONT;

    ADC1->CFGR &= ~ADC_CFGR_EXTEN;

    // Enable the ADC. Before moving on, wait until the ADC is ready

    ADC1->CR |= ADC_CR_ADEN;

    while (!(ADC1->ISR & ADC_ISR_ADRDY))
        ;
}