#include "sd_card.h"
#include "spi.h"

#define SPI_POL_IN_SDCARD_MODE	1
#define SPI_PHA_IN_SDCARD_MODE	1

#define SD_LENGTH_OF_COMMAND		6

#define SD_CMD0_CRC_VALUE				0x95

/* Definitions for MMC/SDC command */
#define CMD0    (0x40+0)    /* GO_IDLE_STATE */
#define CMD1    (0x40+1)    /* SEND_OP_COND */
#define CMD8    (0x40+8)    /* SEND_IF_COND */
#define CMD9    (0x40+9)    /* SEND_CSD */
#define CMD10    (0x40+10)    /* SEND_CID */
#define CMD12    (0x40+12)    /* STOP_TRANSMISSION */
#define CMD16    (0x40+16)    /* SET_BLOCKLEN */
#define CMD17    (0x40+17)    /* READ_SINGLE_BLOCK */
#define CMD18    (0x40+18)    /* READ_MULTIPLE_BLOCK */
#define CMD23    (0x40+23)    /* SET_BLOCK_COUNT */
#define CMD24    (0x40+24)    /* WRITE_BLOCK */
#define CMD25    (0x40+25)    /* WRITE_MULTIPLE_BLOCK */
#define CMD41    (0x40+41)    /* SEND_OP_COND (ACMD) */
#define CMD55    (0x40+55)    /* APP_CMD */
#define CMD58    (0x40+58)    /* READ_OCR */

static uint8_t MessageBuffer[10];

/* Sends clock signal 10 times to synchronize. */
void __SD_InitClockSignal(void){
	
	for(uint8_t index =0 ; index < 10; index++){
		MessageBuffer[index] = 0xFF;
	}
	
	SPI_Send(MessageBuffer, 10);

    MessageBuffer[SD_LENGTH_OF_COMMAND] = 0;
}

/* Send command to change to SPI mode from SDIO mode. */
void __SD_InitChangeToSPI_Mode(void){
	
	MessageBuffer[0] = CMD0;
	MessageBuffer[1] = 0;
	MessageBuffer[2] = 0;
	MessageBuffer[3] = 0;
	MessageBuffer[4] = 0;
	MessageBuffer[5] = SD_CMD0_CRC_VALUE;
	
	SPI_Send(MessageBuffer, SD_LENGTH_OF_COMMAND);
}

void SD_Init(void){
	uint16_t Timeout = 60000;
    
    SPI_Init(SPI_POL_IN_SDCARD_MODE, SPI_PHA_IN_SDCARD_MODE);
	
	SPI_DeselectDevice();
	
	__SD_InitClockSignal();
	
	SPI_SelectDevice();
	
	__SD_InitChangeToSPI_Mode();
	
    char ByteReceived = 0;
	while(ByteReceived != 1 && Timeout != 0){
        SPI_Read(ByteReceived, 1);
        Timeout--;
    }
	
	SPI_DeselectDevice();

    ByteReceived = 0xFF;

    SPI_Send(ByteReceived, 1);
}
void SD_SendCommand(char* CommandToSend, char* ArgsForCommand){
	MessageBuffer[0] = CommandToSend;
    MessageBuffer[1] = ArgsForCommand[0];
    MessageBuffer[2] = ArgsForCommand[1];
    MessageBuffer[3] = ArgsForCommand[2];
    MessageBuffer[4] = ArgsForCommand[3];
    MessageBuffer[5] = 0;

    SPI_Send(MessageBuffer, SD_LENGTH_OF_COMMAND);
}

void SD_ReadMessage(char* MessageRead){
	SPI_Read(MessageRead, SD_LENGTH_OF_COMMAND);
}
