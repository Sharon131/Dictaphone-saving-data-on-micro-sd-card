#ifndef SD_CARD
#define SD_CARD

//#include "stm32f4xx_hal.h"


void SD_Init(void);

uint8_t SD_SendCommand(uint8_t CommandToSend, uint32_t ArgsForCommand);

bool SD_ReceiveDataBlock(uint8_t* Buffer, uint16_t NumberOfBytesToReceive);
bool SD_SendDataBlock(uint8_t* Buffer, uint16_t NumberOfBytesToSend, uint8_t Token);

#endif
