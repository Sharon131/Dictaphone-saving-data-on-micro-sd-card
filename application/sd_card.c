#include "sd_card.h"
#include "spi.h"

#define SPI_POL_IN_SDCARD_MODE	1
#define SPI_PHA_IN_SDCARD_MODE	1

#define SD_LENGTH_OF_COMMAND		6

#define SD_CMD0_CRC_VALUE				0x95

static uint8_t MessageBuffer[10];

/* Sends clock signal 10 times to synchronize. */
void __SD_InitClockSignal(void){
	
	for(uint8_t index =0 ; index < 10; index++){
		MessageBuffer[index] = 0xFF;
	}
	
	SPI_Send(MessageBuffer, 10);
}

/* Send command to change to SPI mode from SDIO mode. */
void __SD_InitChangeToSPI_Mode(void){
	
	MessageBuffer[0] = 6;//change to cmd0, when it's defined
	MessageBuffer[1] = 0;
	MessageBuffer[2] = 0;
	MessageBuffer[3] = 0;
	MessageBuffer[4] = 0;
	MessageBuffer[5] = SD_CMD0_CRC_VALUE;
	
	SPI_Send(MessageBuffer, SD_LENGTH_OF_COMMAND);
}

void SD_Init(void){
	SPI_Init(SPI_POL_IN_SDCARD_MODE, SPI_PHA_IN_SDCARD_MODE);
	
	SPI_DeselectDevice();
	
	__SD_InitClockSignal();
	
	SPI_SelectDevice();
	
	__SD_InitChangeToSPI_Mode();
	
	//wait for 0x01
	
	
	SPI_DeselectDevice();
}
void SD_SendCommand(char* CommandToSend, char* ArgsForCommand){
	;
}

void SD_ReadMessage(char* MessageRead){
	;
}