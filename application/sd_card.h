#ifndef SD_CARD
#define SD_CARD

//#include "stm32f4xx_hal.h"


void SD_Init(void);

uint8_t SD_SendCommand(uint8_t CommandToSend, uint32_t ArgsForCommand);

bool SD_ReceiveDataPacket(uint8_t* Buffer, uint16_t NumberOfBytesToReceive);//TODO: make static

bool SingleBlockRead(void);
bool MultipleBlockRead(uint16_t NoOfBlockToRead);

bool SD_SendDataPacket(uint8_t* Buffer, uint16_t NumberOfBytesToSend, uint8_t Token);//TODO: make static

bool SD_SendSingleBlock(void);
bool SD_SendMultipleBlock(uint16_t NoOfBlockToSend);

#endif