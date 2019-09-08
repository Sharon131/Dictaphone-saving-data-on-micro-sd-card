#ifndef SD_CARD
#define SD_CARD

//#include "stm32f4xx_hal.h"
#include <stdint.h>
#include <stdbool.h>
#include "spi.h"

void SD_Init(void);

uint8_t SD_WaitFor(uint8_t MaxNoOfByteCycles);

uint8_t SD_SendCommand(uint8_t CommandToSend, uint32_t ArgsForCommand);

bool SD_SingleBlockRead(uint8_t* Buffer, uint32_t Address);
bool SD_MultipleBlocksRead(uint8_t* Buffer, uint16_t NoOfBlockToRead, uint32_t Address);

bool SD_SendSingleBlock(uint8_t* Buffer);
bool SD_SendMultipleBlocks(uint8_t* Buffer, uint16_t NoOfBlockToSend);

#endif
