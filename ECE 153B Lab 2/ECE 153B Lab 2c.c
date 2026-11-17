/* ======================= Main.c ======================= */

/*

*ECE 153B - Spring 2025

*

* Name(s): Patrick Lee, Nate Sirivallop

* Section: Wed 7pm

* Lab: 2C

*/

#include "stm32l476xx.h"

#include "LED.h"

#include "RTC.h"

#include "SysClock.h"

#include <stdio.h>

static char strTime[12] = {0};

static char strDate[12] = {0};

void RTC_Set_Alarm(void)
{

    RTC_Disable_Write_Protection(); // remove RTC write protect (must come before touching RTC->CR)

    RTC->CR &= ~(RTC_CR_ALRAE | RTC_CR_ALRBE); // clear alarm enable for A and B

    RTC->CR &= ~(RTC_CR_ALRAIE | RTC_CR_ALRBIE); // clear interrupt enable for A and B

    while (((RTC->ISR & RTC_ISR_ALRAWF) == 0) || ((RTC->ISR & RTC_ISR_ALRBWF) == 0))
        ; // wait until BOTH alarm registers can be written

    RTC->ALRMAR = 0;

    RTC->ALRMAR |= 0x30; // alarm A: seconds = 30 (BCD)

    RTC->ALRMAR |= (RTC_ALRMAR_MSK2 | RTC_ALRMAR_MSK3 | RTC_ALRMAR_MSK4); // ignore min/hour/date

    RTC->ALRMAR &= ~RTC_ALRMAR_MSK1; // compare seconds -> fires at :30 every minute

    RTC->ALRMBR = 0;

    RTC->ALRMBR |= (RTC_ALRMBR_MSK1 | RTC_ALRMBR_MSK2 | RTC_ALRMBR_MSK3 | RTC_ALRMBR_MSK4); // alarm B: every second

    RTC->CR |= (RTC_CR_ALRAIE | RTC_CR_ALRBIE); // enable interrupts for A and B

    RTC->CR |= (RTC_CR_ALRAE | RTC_CR_ALRBE); // enable both alarms

    RTC_Enable_Write_Protection();
}

void RTC_Alarm_Enable(void)
{

    // Connect External Line to the GPIO

    // RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;

    // SYSCFG->EXTICR[3] &= ~SYSCFG_EXTICR4_EXTI13; // reset to 0

    // SYSCFG->EXTICR[3] |= SYSCFG_EXTICR4_EXTI13_PC;

    // Rising trigger selection

    // 0 = trigger disabled, 1 = trigger enabled

    // RTC alarm is EXTI line 18, which lives in the *1 registers only.

    // (Writing bit 18 into the *2 registers enabled an unrelated line.)

    EXTI->RTSR1 |= EXTI_RTSR1_RT18;

    // Interrupt Mask Register

    // 0 = masked, 1 = not masked (enabled)

    EXTI->IMR1 |= EXTI_IMR1_IM18;

    // event mask

    EXTI->EMR1 |= EXTI_EMR1_EM18;

    EXTI->PR1 = EXTI_PR1_PIF18; // clear any pending flag (write 1 to clear)

    NVIC_SetPriority(RTC_Alarm_IRQn, 0); // set interrupt to highest priority

    NVIC_EnableIRQ(RTC_Alarm_IRQn); // Enable
}

void RTC_Alarm_IRQHandler(void)
{

    if ((EXTI->PR1 & EXTI_PR1_PIF18) != 0) // check interrupt flag

    {

        // Check each alarm separately: at :30 both fire together, and an

        // "else if" would silently drop alarm B's toggle.

        if (RTC->ISR & RTC_ISR_ALRAF)
        {

            // alarm A triggered

            // Green_LED_Toggle();

            RTC->ISR &= ~RTC_ISR_ALRAF;
        }

        if (RTC->ISR & RTC_ISR_ALRBF)
        {

            // alarm B triggered

            Green_LED_Toggle();

            RTC->ISR &= ~RTC_ISR_ALRBF;
        }

        // Clear EXTI flag (write 1 to clear)

        EXTI->PR1 = EXTI_PR1_PIF18;
    }
}

int main(void)
{

    System_Clock_Init(); // Switch System Clock = 80 MHz

    LED_Init();

    RTC_Init();

    RTC_Alarm_Enable();

    RTC_Set_Alarm();

    while (1)
    {

        // [TODO]

        Get_RTC_Calendar(strTime, strDate);
    }
}

/* ======================= RTC.c ======================= */

/*

* ECE 153B - Spring 2025

*

* Name(s): Patrick Lee, Nate Sirivallop

* Section: Wed 7pm

* Lab: 2C

*/

#include "RTC.h"

// Use the 32.768 kHz low-speed external clock as RTC clock source

// Helper macro to convert a value from 2 digit decimal format to BCD format

// __VALUE__ Byte to be converted

#define __RTC_CONVERT_BIN2BCD(__VALUE__) (uint8_t)((((__VALUE__) / 10) << 4) | ((__VALUE__) % 10))

#define __RTC_CONVERT_BCD2BIN(__VALUE__) (uint8_t)(((uint8_t)((__VALUE__) & (uint8_t)0xF0) >> (uint8_t)0x4) * 10 + ((__VALUE__) & (uint8_t)0x0F))

#define RTC_WEEKDAY_MONDAY ((uint32_t)0x01) /*!< Monday */

#define RTC_WEEKDAY_TUESDAY ((uint32_t)0x02) /*!< Tuesday */

#define RTC_WEEKDAY_WEDNESDAY ((uint32_t)0x03) /*!< Wednesday */

#define RTC_WEEKDAY_THURSDAY ((uint32_t)0x04) /*!< Thrusday */

#define RTC_WEEKDAY_FRIDAY ((uint32_t)0x05) /*!< Friday */

#define RTC_WEEKDAY_SATURDAY ((uint32_t)0x06) /*!< Saturday */

#define RTC_WEEKDAY_SUNDAY ((uint32_t)0x07) /*!< Sunday */

#define RTC_MONTH_JANUARY ((uint8_t)0x01) /*!< January */

#define RTC_MONTH_FEBRUARY ((uint8_t)0x02) /*!< February */

#define RTC_MONTH_MARCH ((uint8_t)0x03) /*!< March */

#define RTC_MONTH_APRIL ((uint8_t)0x04) /*!< April */

#define RTC_MONTH_MAY ((uint8_t)0x05) /*!< May */

#define RTC_MONTH_JUNE ((uint8_t)0x06) /*!< June */

#define RTC_MONTH_JULY ((uint8_t)0x07) /*!< July */

#define RTC_MONTH_AUGUST ((uint8_t)0x08) /*!< August */

#define RTC_MONTH_SEPTEMBER ((uint8_t)0x09) /*!< September */

#define RTC_MONTH_OCTOBER ((uint8_t)0x10) /*!< October */

#define RTC_MONTH_NOVEMBER ((uint8_t)0x11) /*!< November */

#define RTC_MONTH_DECEMBER ((uint8_t)0x12) /*!< December */

    void RTC_BAK_SetRegister(uint32_t BackupRegister, uint32_t Data)
{

    register uint32_t tmp = 0;

    tmp = (uint32_t)(&(RTC->BKP0R));

    tmp += (BackupRegister * 4);

    /* Write the specified register */

    *(__IO uint32_t *)tmp = (uint32_t)Data;
}

void RTC_Init(void)
{

    /* Enables the PWR Clock and Enables access to the backup domain #######*/

    /* To change the source clock of the RTC feature (LSE, LSI), you have to:

    - Enable the power clock

    - Enable write access to configure the RTC clock source (to be done once after reset).

    - Reset the Back up Domain

    - Configure the needed RTC clock source */

    RTC_Clock_Init();

    // Disable RTC registers write protection

    RTC_Disable_Write_Protection();

    // Enter in initialization mode

    RTC->ISR |= RTC_ISR_INIT;

    while ((RTC->ISR & RTC_ISR_INITF) == 0)
        ; // Wait until INITF is set after entering initialization mode

    // Configure the Date

    /* Note: __LL_RTC_CONVERT_BIN2BCD helper macro can be used if user wants to */

    /* provide directly the decimal value: */

    RTC_Set_Calendar_Date(RTC_WEEKDAY_WEDNESDAY, 0x25, RTC_MONTH_JANUARY, 0x23); /* [TODO] These values are stubs - fill in current date */

    // Configure the Time

    RTC_Set_Time(RTC_TR_PM, 0x07, 0x00, 0x00); /* [TODO] These values are stubs - fill in current time */

    // Exit of initialization mode

    RTC->ISR &= ~RTC_ISR_INIT;

    while ((RTC->ISR & RTC_ISR_RSF) == 0)
        ; /* Wait for synchro */

    /* Note: Needed only if Shadow registers is enabled */

    /* LL_RTC_IsShadowRegBypassEnabled function can be used */

    // Enable RTC registers write protection

    RTC_Enable_Write_Protection();

    // Writes a data in a RTC Backup data Register1

    // to indicate date/time updated

    RTC_BAK_SetRegister(1, 0x32F2);
}

#define POSITION_VAL(VAL) (__CLZ(__RBIT(VAL))) // count number of trailing zeroes

#define RTC_POSITION_TR_PM (uint32_t)POSITION_VAL(RTC_TR_PM) // am/pm

#define RTC_POSITION_TR_HT (uint32_t)POSITION_VAL(RTC_TR_HT) // Hour

#define RTC_POSITION_TR_HU (uint32_t)POSITION_VAL(RTC_TR_HU) // Hour

#define RTC_POSITION_TR_MT (uint32_t)POSITION_VAL(RTC_TR_MNT) // Minute

#define RTC_POSITION_TR_MU (uint32_t)POSITION_VAL(RTC_TR_MNU) // Minute

#define RTC_POSITION_TR_ST (uint32_t)POSITION_VAL(RTC_TR_ST) // Second

#define RTC_POSITION_TR_SU (uint32_t)POSITION_VAL(RTC_TR_SU) // Second

#define RTC_POSITION_DR_YT (uint32_t)POSITION_VAL(RTC_DR_YT) // Year

#define RTC_POSITION_DR_YU (uint32_t)POSITION_VAL(RTC_DR_YU) // Year

#define RTC_POSITION_DR_MT (uint32_t)POSITION_VAL(RTC_DR_MT) // Month

#define RTC_POSITION_DR_MU (uint32_t)POSITION_VAL(RTC_DR_MU) // Month

#define RTC_POSITION_DR_DT (uint32_t)POSITION_VAL(RTC_DR_DT) // Day

#define RTC_POSITION_DR_DU (uint32_t)POSITION_VAL(RTC_DR_DU) // Day

#define RTC_POSITION_DR_WDU (uint32_t)POSITION_VAL(RTC_DR_WDU) // Weekday

void RTC_Set_Calendar_Date(uint32_t WeekDay, uint32_t Day, uint32_t Month, uint32_t Year)
{

    // Write the date values in the correct place within the RTC Date Register

    RTC->DR = Year << RTC_POSITION_DR_YU | WeekDay << RTC_POSITION_DR_WDU | Month << RTC_POSITION_DR_MU | Day << RTC_POSITION_DR_DU;
}

void RTC_Set_Time(uint32_t Format12_24, uint32_t Hour, uint32_t Minute, uint32_t Second)
{

    // Write the time values in the correct place within the RTC Time Register

    RTC->TR = Format12_24 | Hour << RTC_POSITION_TR_HU | Minute << RTC_POSITION_TR_MU | Second << RTC_POSITION_TR_SU;
}

void RTC_Clock_Init(void)
{

    // Enable write access to Backup domain

    if ((RCC->APB1ENR1 & RCC_APB1ENR1_PWREN) == 0)
    {

        RCC->APB1ENR1 |= RCC_APB1ENR1_PWREN; // Power interface clock enable

        (void)RCC->APB1ENR1; // Delay after an RCC peripheral clock enabling
    }

    // Select LSE as RTC clock source

    if ((PWR->CR1 & PWR_CR1_DBP) == 0)
    {

        PWR->CR1 |= PWR_CR1_DBP; // Enable write access to Backup domain

        while ((PWR->CR1 & PWR_CR1_DBP) == 0)
            ; // Wait for Backup domain Write protection disable
    }

    // Reset LSEON and LSEBYP bits before configuring the LSE

    RCC->BDCR &= ~(RCC_BDCR_LSEON | RCC_BDCR_LSEBYP);

    // RTC Clock selection can be changed only if the Backup Domain is reset

    RCC->BDCR |= RCC_BDCR_BDRST;

    RCC->BDCR &= ~RCC_BDCR_BDRST;

    // Note from STM32L4 Reference Manual:

    // RTC/LCD Clock: (1) LSE is in the Backup domain. (2) HSE and LSI are not.

    while ((RCC->BDCR & RCC_BDCR_LSERDY) == 0)
    { // Wait until LSE clock ready

        RCC->BDCR |= RCC_BDCR_LSEON;
    }

    // Select LSE as RTC clock source

    // BDCR = Backup Domain Control Register

    RCC->BDCR &= ~RCC_BDCR_RTCSEL; // RTCSEL[1:0]: 00 = No Clock, 01 = LSE, 10 = LSI, 11 = HSE

    RCC->BDCR |= RCC_BDCR_RTCSEL_0; // Select LSE as RTC clock

    RCC->BDCR |= RCC_BDCR_RTCEN; // Enable RTC

    RCC->APB1ENR1 &= ~RCC_APB1ENR1_PWREN; // Power interface clock disable
}

void RTC_Disable_Write_Protection(void)
{

    // write two bytes (0xCA and 0x53) sequentially -- must be plain writes, not |=

    RTC->WPR = 0xCA;

    RTC->WPR = 0x53;
}

void RTC_Enable_Write_Protection(void)
{

    // write any wrong key to re-lock

    RTC->WPR = 0xFF;
}

uint32_t RTC_TIME_GetHour(void)
{

    return (((RTC->TR) & (RTC_TR_HT | RTC_TR_HU)) >> RTC_POSITION_TR_HU);
}

uint32_t RTC_TIME_GetMinute(void)

{

    return (((RTC->TR) & (RTC_TR_MNT | RTC_TR_MNU)) >> RTC_POSITION_TR_MU);
}

uint32_t RTC_TIME_GetSecond(void)
{

    return ((RTC->TR) & (RTC_TR_ST | RTC_TR_SU)) >> RTC_POSITION_TR_SU;
}

uint32_t RTC_DATE_GetMonth(void)
{

    return (((RTC->DR) & (RTC_DR_MT | RTC_DR_MU)) >> RTC_POSITION_DR_MU);
}

uint32_t RTC_DATE_GetDay(void)
{

    return (((RTC->DR) & (RTC_DR_DT | RTC_DR_DU)) >> RTC_POSITION_DR_DU);
}

uint32_t RTC_DATE_GetYear(void)
{

    return (((RTC->DR) & (RTC_DR_YT | RTC_DR_YU)) >> RTC_POSITION_DR_YU);
}

uint32_t RTC_DATE_GetWeekDay(void)
{

    return (((RTC->DR) & (RTC_DR_WDU)) >> RTC_POSITION_DR_WDU);
}

void Get_RTC_Calendar(char *strTime, char *strDate)
{

    /* Note: need to convert in decimal value in using __LL_RTC_CONVERT_BCD2BIN helper macro */

    /* Display time Format : hh:mm:ss */

    sprintf((char *)strTime, "%.2d:%.2d:%.2d",

            __RTC_CONVERT_BCD2BIN(RTC_TIME_GetHour()),

            __RTC_CONVERT_BCD2BIN(RTC_TIME_GetMinute()),

            __RTC_CONVERT_BCD2BIN(RTC_TIME_GetSecond()));

    /* Display date Format : mm-dd-yy */

    sprintf((char *)strDate, "%.2d-%.2d-%.4d",

            __RTC_CONVERT_BCD2BIN(RTC_DATE_GetMonth()),

            __RTC_CONVERT_BCD2BIN(RTC_DATE_GetDay()),

            2000 + __RTC_CONVERT_BCD2BIN(RTC_DATE_GetYear()));
}