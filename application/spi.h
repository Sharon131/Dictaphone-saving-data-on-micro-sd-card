#ifndef SPI_INTERFACE
#define SPI_INTERFACE

#include "stm32f4xx_hal.h"

#include "stdbool.h"

void SPI_Init(bool Polarity, bool Phase);

void SPI_SelectDevice(void);
void SPI_DeselectDevice(void);

void SPI_Send(char* MessageToSend);
void SPI_Read(char* MessageReceived);

#endif
