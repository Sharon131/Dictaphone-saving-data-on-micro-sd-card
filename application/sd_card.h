#ifndef SD_CARD
#define SD_CARD

//#include "stm32f4xx_hal.h"
#include <stdint.h>
#include <stdbool.h>

void SD_Init(void);

uint8_t SD_SendCommand(uint8_t CommandToSend, uint32_t ArgsForCommand);

bool SD_SingleBlockRead(uint8_t* Buffer);
bool SD_MultipleBlockRead(uint8_t* Buffer, uint16_t NoOfBlockToRead);

bool SD_SendSingleBlock(uint8_t* Buffer);
bool SD_SendMultipleBlock(uint8_t* Buffer, uint16_t NoOfBlockToSend);

#endif