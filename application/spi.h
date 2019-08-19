#ifndef SPI_INTERFACE
#define SPI_INTERFACE

#include "stm32f4xx_hal.h"

void SPI_Init(void);

void SPI_Send(char* MessageToSend);
void SPI_Read(char* MessageReceived);

#endif
