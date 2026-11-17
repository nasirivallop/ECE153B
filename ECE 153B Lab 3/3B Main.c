/*

* ECE 153B

*

* Name(s):

* Section:

* Lab: 3B

*/

#include "stm32l476xx.h"

void LED_Pin_Init(void);

void TIM2_CH1_Init(void);

void SERVO_Pin_Init(void);

void TIM5_CH1_Init(void);

void LED_Pin_Init(void)
{

    // Enable GPIO Clocks

    RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;

    // Initialize Green LED

    GPIOA->MODER &= ~GPIO_MODER_MODE5; // Clear mode bits

    GPIOA->MODER |= GPIO_MODER_MODE5_1; // set bit 10 == pin 5 as alternate

    // GPIO Mode

    GPIOA->OTYPER &= ~GPIO_OTYPER_OT5;

    // Alternative Function

    GPIOA->AFR[0] &= ~GPIO_AFRL_AFSEL5;

    GPIOA->AFR[0] |= GPIO_AFRL_AFSEL5_0;

    // Set I/O output speed value as very high speed

    GPIOA->OSPEEDR |= GPIO_OSPEEDR_OSPEED5_0;

    GPIOA->OSPEEDR |= GPIO_OSPEEDR_OSPEED5_1;

    // GPIO Push-Pull: No pull-up, no pull-down

    GPIOA->PUPDR &= ~GPIO_PUPDR_PUPD5;
}

void TIM2_CH1_Init(void)
{

    // Enable the timer clock

    RCC->APB1ENR1 |= RCC_APB1ENR1_TIM2EN;

    // Counting direction: 0 = up-counting, 1 = down-counting

    TIM2->CR1 &= ~TIM_CR1_DIR;

    TIM2->CR1 |= TIM_CR1_ARPE;

    // Prescaler

    TIM2->PSC = 15; // 15

    // Auto-reload

    TIM2->ARR = 999; // 999

    // Disable output compare mode

    TIM2->CCMR1 &= ~TIM_CCMR1_OC1FE;

    // PWM mode 1

    TIM2->CCMR1 &= ~TIM_CCMR1_OC1M;

    TIM2->CCMR1 |= TIM_CCMR1_OC1M_1;

    TIM2->CCMR1 |= TIM_CCMR1_OC1M_2;

    // Output 1 preload enable

    TIM2->CCMR1 |= TIM_CCMR1_OC1PE;

    // Select output polarity: active high

    TIM2->CCER &= ~TIM_CCER_CC1P; // is this high or low

    // Enable output for ch1

    TIM2->CCER |= TIM_CCER_CC1E;

    // Output Compare Register for channel 1

    TIM2->CCR1 = 500; // 500

    // Enable counter

    TIM2->CR1 |= TIM_CR1_CEN;
}

void SERVO_Pin_Init(void)
{

    // Set mode as Alternative Function 2 (TIM5_CH1)

    RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;

    GPIOA->MODER &= ~GPIO_MODER_MODE0;

    GPIOA->MODER |= GPIO_MODER_MODE0_1;

    GPIOA->AFR[0] &= ~GPIO_AFRL_AFSEL0;

    GPIOA->AFR[0] |= GPIO_AFRL_AFSEL0_1;

    // Set I/O output speed value as very high speed

    GPIOA->OSPEEDR |= GPIO_OSPEEDR_OSPEED0_0;

    GPIOA->OSPEEDR |= GPIO_OSPEEDR_OSPEED0_1;

    // Push-pull output

    GPIOA->OTYPER &= ~GPIO_OTYPER_OT0;

    // Set I/O as no pull-up pull-down

    GPIOA->PUPDR &= ~GPIO_PUPDR_PUPD0;
}

void TIM5_CH1_Init(void)
{

    // Enable the timer clock

    RCC->APB1ENR1 |= RCC_APB1ENR1_TIM5EN;

    // Counting direction: 0 = up-counting, 1 = down-counting

    TIM5->CR1 &= ~TIM_CR1_DIR;

    // Prescaler

    TIM5->PSC = 39; // 39

    // Auto-reload

    TIM5->ARR = 1999; // 1999

    // Disable output compare mode

    TIM5->CCMR1 &= ~TIM_CCMR1_OC1FE;

    // PWM mode 1

    TIM5->CCMR1 &= ~TIM_CCMR1_OC1M;

    TIM5->CCMR1 |= TIM_CCMR1_OC1M_1;

    TIM5->CCMR1 |= TIM_CCMR1_OC1M_2;

    // Output 1 preload enable

    TIM5->CCMR1 |= TIM_CCMR1_OC1PE;

    // Select output polarity: active high

    TIM5->CCER &= ~TIM_CCER_CC1P; // is this high or low

    // Enable output for ch1

    TIM5->CCER |= TIM_CCER_CC1E;

    // Output Compare Register for channel 1

    // 1ms ccr1 = 100

    // 1.5ms ccr1 = ccr = 150,

    // 2ms = 200

    TIM5->CCR1 = 100; // 100

    // Enable counter

    TIM5->CR1 |= TIM_CR1_CEN;
}

int main(void)
{

    volatile int i; // volatile so the busy-wait loops are not optimized away

    // int led = 0;

    int dire = 1;

    LED_Pin_Init();

    SERVO_Pin_Init();

    TIM2_CH1_Init();

    TIM5_CH1_Init();

    if (dire == 0)
    {

        TIM2->CR1 &= ~TIM_CR1_DIR;
    }
    else
    {

        TIM2->CR1 |= TIM_CR1_DIR;
    }

    TIM2->CCR1 = 0; // TODO

    TIM5->CCR1 = 100; // TODO

    while (1)
    {

        // TODO

        // LED CODE

        /*

        while(TIM2->CCR1 < 999){

        TIM2->CCR1 ++;

        for(i=0;i<200;i++); // delay

        }

        while(TIM2->CCR1 > 0){

        TIM2->CCR1 --;

        for(i=0;i<200;i++); // delay

        }

        */

        // SERVO CODE

        while (TIM5->CCR1 <= 130)
        {

            if (TIM2->CCR1 < 989)
            {

                TIM2->CCR1 += 6;
            }

            TIM5->CCR1++;

            for (i = 0; i < 2000; i++)
                ;
        }

        for (i = 0; i < 100000; i++)
            ;

        while (TIM5->CCR1 <= 230)
        {

            if (TIM2->CCR1 < 989)
            {

                TIM2->CCR1 += 6;
            }

            TIM5->CCR1++;

            for (i = 0; i < 2000; i++)
                ;
        }

        for (i = 0; i < 100000; i++)
            ;

        while (TIM5->CCR1 >= 60)
        {

            if (TIM2->CCR1 > 8)
            {

                TIM2->CCR1 -= 6;
            }

            TIM5->CCR1--;

            for (i = 0; i < 2000; i++)
                ;
        }

        for (i = 0; i < 100000; i++)
            ;
    }
}