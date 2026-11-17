
#include "SysClock.h"

// System clock = MSI at 16 MHz

void System_Clock_Init(void)
{

    // Enable MSI and wait until it is ready

    RCC->CR |= RCC_CR_MSION;

    while ((RCC->CR & RCC_CR_MSIRDY) == 0)
        ;

    // Set the MSI range BEFORE switching to it (range can only change while MSI is off or ready)

    RCC->CR &= ~RCC_CR_MSIRANGE; // Clear current range

    RCC->CR |= RCC_CR_MSIRANGE_8; // 1000 = 16 MHz (RANGE_4 was only 1 MHz)

    RCC->CR |= RCC_CR_MSIRGSEL; // Use MSIRANGE from RCC_CR

    while ((RCC->CR & RCC_CR_MSIRDY) == 0)
        ;

    // Select MSI as system clock source

    RCC->CFGR &= ~RCC_CFGR_SW; // 00 = MSI

    while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_MSI)
        ;
}
