#ifndef SPI_INTERFACE
#define SPI_INTERFACE

#include <stdint.h>
#include <stdbool.h>

#include "stm32f4xx_hal.h"

void SPI_Init(bool Polarity, bool Phase);

void SPI_SelectDevice(void);
void SPI_DeselectDevice(void);

void SPI_Send(uint8_t* MessageToSend, size_t BytesToSend);
void SPI_Read(uint8_t* MessageReceived, size_t SizeOfBuffer);

#endif
