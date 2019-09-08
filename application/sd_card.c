#include "sd_card.h"

#define SPI_POL_IN_SDCARD_MODE	1
#define SPI_PHA_IN_SDCARD_MODE	1

#define SD_LENGTH_OF_COMMAND	6

#define SD_IDLE_STATE_VALUE		0xFF

#define SD_DATA_BLOCK_SIZE		512

#define SD_CMD0_CRC_VALUE		0x95
#define SD_CMD8_CRC_VALUE		0x87

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

#define CMD17_18_24_DATA_TOKEN	0xFE
#define CMD25_DATA_TOKEN		0xFC
#define CMD25_STOP_TRAN_TOKEN	0xFD

#define DATA_RESPONSE_ACCEPTED	5
#define DATA_RESPONSE_CRC_ERR	11
#define DATA_RESPONSE_WRITE_ERR	13

#define SD_SPI_MISO_PORT	GPIOG
#define SD_SPI_MISO_PIN		GPIO_PIN_9
#define SD_SPI_MOSI_PORT	GPIOG
#define SD_SPI_MOSI_PIN		GPIO_PIN_14
#define SD_SPI_SCK_PORT		GPIOB
#define SD_SPI_SCK_PIN		GPIO_PIN_2
#define SD_SPI_CS_PORT		GPIOC
#define SD_SPI_CS_PIN		GPIO_PIN_8

static uint8_t MessageBuffer[10];

/* Idles MOSI line for MaxNoOfByteCycles Clock Cycles * 8. 
	MaxNoOfByteCycles -> Number of empty bytes to send. */
uint8_t SD_WaitFor(uint8_t MaxNoOfByteCycles){
	uint8_t ByteReceived = SD_IDLE_STATE_VALUE;

	while(MaxNoOfByteCycles != 0 && ByteReceived == SD_IDLE_STATE_VALUE){
		ByteReceived = SPI_Transmit(SD_IDLE_STATE_VALUE);
		MaxNoOfByteCycles--;
	}

	return ByteReceived;
}

bool __SD_IsCardReady(void){
	return (SPI_Transmit(SD_IDLE_STATE_VALUE) == SD_IDLE_STATE_VALUE);
}

/* Sends clock signal 10 times to synchronize. */
void __SD_InitClockSignal(void){
	
	SD_WaitFor(10);
}

/* Send command to change to SPI mode from SDIO mode. */
void __SD_InitChangeToSPI_Mode(void){
	
	SD_SendCommand(CMD0, 0);
}

void SD_Init(void){//TODO: correct
	uint16_t Timeout = 60000;
    
  SPI_Init(SPI_POL_IN_SDCARD_MODE, SPI_PHA_IN_SDCARD_MODE);
	
	SPI_DeselectDevice();
	
	__SD_InitClockSignal();
	
	SPI_SelectDevice();
	
	__SD_InitChangeToSPI_Mode();
	
	
	SPI_DeselectDevice();

	SD_WaitFor(1);
}



uint8_t SD_SendCommand(uint8_t CommandToSend, uint32_t ArgsForCommand){
	
	SPI_Transmit(CommandToSend);
	SPI_Transmit((uint8_t)(ArgsForCommand >> 24));
	SPI_Transmit((uint8_t)(ArgsForCommand >> 16));
	SPI_Transmit((uint8_t)(ArgsForCommand >> 8));
	SPI_Transmit((uint8_t)ArgsForCommand);
	
	if(CommandToSend == CMD0){
		SPI_Transmit(SD_CMD0_CRC_VALUE);
	} else if (CommandToSend == CMD8) {
		SPI_Transmit(SD_CMD8_CRC_VALUE);
	} else {
		SPI_Transmit(0);
	}

	if(CommandToSend == CMD12){
		SPI_Transmit(SD_IDLE_STATE_VALUE);//discard received data
	}

	uint8_t Timeout = 10;
	uint8_t Response = 0x80;

	while( (Response & 0x80) != 0 && Timeout != 0){
		Response = SPI_Transmit(SD_IDLE_STATE_VALUE);
		Timeout--;
	}

	return Response;
}

bool SD_ReceiveDataPacket(uint8_t* Buffer, uint16_t NumberOfBytesToReceive){
	uint8_t ReceivedToken = SD_WaitFor(10);

	if(ReceivedToken != CMD17_18_24_DATA_TOKEN)		return false;

	while(NumberOfBytesToReceive != 0){
		*Buffer = SPI_Transmit(SD_IDLE_STATE_VALUE);
		Buffer++;
		NumberOfBytesToReceive--;
	}

	SPI_Transmit(SD_IDLE_STATE_VALUE);//discard crc
	SPI_Transmit(SD_IDLE_STATE_VALUE);

	return true;
}

bool SD_SingleBlockRead(uint8_t* Buffer, uint32_t Address){
	uint8_t CommandResponse = SD_SendCommand(CMD17, Address);

	if(CommandResponse != 0)		return false;

	bool WasReadSuccessful = SD_ReceiveDataPacket(Buffer, SD_DATA_BLOCK_SIZE);

	SD_WaitFor(2);//just in case

	return WasReadSuccessful;
}

bool SD_MultipleBlocksRead(uint8_t* Buffer, uint16_t NoOfBlockToRead, uint32_t Address){
	if(NoOfBlockToRead == 0)		return false;
	
	bool WasReadSuccessful = 0;
	
	uint8_t CommandResponse = SD_SendCommand(CMD18, Address);

	if(CommandResponse != 0)		return false;

	while(NoOfBlockToRead != 0){
		WasReadSuccessful = SD_ReceiveDataPacket(Buffer, SD_DATA_BLOCK_SIZE);
		Buffer += 512;

		if(!WasReadSuccessful)	return false;
	}

	SD_SendCommand(CMD12, 0);

	while(!__SD_IsCardReady());

	return true;
}

bool SD_SendDataPacket(uint8_t* Buffer, uint16_t NumberOfBytesToSend, uint8_t Token){

	uint8_t ReceivedByte = SD_WaitFor(2);

	SPI_Transmit(Token);

	while(NumberOfBytesToSend != 0){
		SPI_Transmit(*Buffer);
		Buffer++;
		NumberOfBytesToSend--;
	}

	SPI_Transmit(SD_IDLE_STATE_VALUE);//crc
	SPI_Transmit(SD_IDLE_STATE_VALUE);

	uint8_t DataResponse = SPI_Transmit(SD_IDLE_STATE_VALUE);

	while(!__SD_IsCardReady());

	return (DataResponse & DATA_RESPONSE_ACCEPTED);
}

bool SD_SendSingleBlock(uint8_t* Buffer){
	uint8_t CommandResponse; // = SD_SendCommand(CMD24, );//TODO: add address

	if(CommandResponse != 0)		return false;

	SD_WaitFor(2);

	bool WasWriteSuccessful = SD_SendDataPacket(Buffer, SD_DATA_BLOCK_SIZE, CMD17_18_24_DATA_TOKEN);
}

bool SD_SendMultipleBlocks(uint8_t* Buffer, uint16_t NoOfBlockToSend){
	uint8_t CommandResponse; // = SD_SendCommand(CMD25, );//TODO: add address

	if(CommandResponse != 0)		return false;

	SD_WaitFor(2);

	while(NoOfBlockToSend != 0){
		bool WasWriteSuccessful = SD_SendDataPacket(Buffer, SD_DATA_BLOCK_SIZE, CMD25_DATA_TOKEN);
		
		if(!WasWriteSuccessful)		return false;
		
		NoOfBlockToSend--;
		Buffer += 512;
	}

	SPI_Transmit(CMD25_STOP_TRAN_TOKEN);
}
