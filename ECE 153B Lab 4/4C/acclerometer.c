#include "SPI.h"

#include "SysTimer.h"

#include "accelerometer.h"

#include "stdio.h"

// Write data to the accelerometer

void accWrite(uint8_t addr, uint8_t val)
{

    // Write the address with R/W = 0 (Write)

    uint16_t addrShifted = (((addr) << 8) | val); // 6-bit address, R/W = 0

    SPI_Transfer_Data(addrShifted);

    // Write the data
}

// Read data from the accelerometer

uint8_t accRead(uint8_t addr)
{

    // Send the address with R/W = 1 (Read)

    uint16_t addrShifted = ((0x80 << 8) | (addr << 8)); // R/W = 1, MB = 0

    // Return the received data

    return (uint8_t)(SPI_Transfer_Data(addrShifted) & 0xFF); // data is in the low byte
}

// Initialize the accelerometer with desired settings

void initAcc(void)
{

    accWrite(0x2C, 0x0A); // BW_RATE: 100Hz

    accWrite(0x31, 0x08); // DATA_FORMAT: Full resolution, ±2g

    accWrite(0x2D, 0x08); // POWER_CTL: Measurement mode
}

// Read X, Y, Z acceleration values

void readValues(double *x, double *y, double *z)
{

    int16_t rawX = (int16_t)((accRead(0x33) << 8) | accRead(0x32));

    int16_t rawY = (int16_t)((accRead(0x35) << 8) | accRead(0x34));

    int16_t rawZ = (int16_t)((accRead(0x37) << 8) | accRead(0x36));

    // double check data format

    const double scale = 0.0039; // Full-resolution scale factor (g/LSB)

    *x = rawX * scale;

    *y = rawY * scale;

    *z = rawZ * scale;
}