#ifndef SPI_INTERFACE
#define SPI_INTERFACE

#include <stdint.h>
#include <stdbool.h>

#include "stm32f4xx_hal.h"

void SPI_Init(bool Polarity, bool Phase);

bool SD_IsCardReady(void);

void SPI_SelectDevice(void);
void SPI_DeselectDevice(void);

uint8_t SPI_Transmit(uint8_t CharacterToSend);

#endif