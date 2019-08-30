#include "spi.h"

#define SPI_BUFFER_SIZE		    8
#define SPI_LSB_VALUE			1
#define SPI_MSB_VALUE			128

#define EMPTY_MSG_VALUE         0

//TODO: zweryfikowac, czy poprawne wartosci sa wpisywane w elementy struktur we wszystkich Initach

//another struct for all of these??
static GPIO_TypeDef* SPI_MISO_Port = GPIOG;
static uint16_t SPI_MISO_Pin = GPIO_PIN_9;
static GPIO_TypeDef* SPI_MOSI_Port = GPIOG;
static uint16_t SPI_MOSI_Pin = GPIO_PIN_14;
static GPIO_TypeDef* SPI_SCK_Port = GPIOB;
static uint16_t SPI_SCK_Pin = GPIO_PIN_2;
static GPIO_TypeDef* SPI_CS_Port = GPIOC;
static uint16_t SPI_CS_Pin = GPIO_PIN_8;

static bool SPI_Polarity;
static bool SPI_Phase;
static bool SPI_MSB_First;
static uint8_t CRC_Polynomial = 7;

static void __SPI_Init_MISO_MOSI_Pins(void){
	GPIO_InitTypeDef GPIO_InitStruct_9;
	GPIO_InitTypeDef GPIO_InitStruct_14;
	
	__GPIOG_CLK_ENABLE();
	
  GPIO_InitStruct_9.Pin   = GPIO_PIN_9;
  GPIO_InitStruct_9.Mode  = GPIO_MODE_INPUT;
  GPIO_InitStruct_9.Pull  = GPIO_PULLDOWN;
  GPIO_InitStruct_9.Speed = GPIO_SPEED_MEDIUM;
  
  HAL_GPIO_Init(GPIOG, &GPIO_InitStruct_9);
   
    GPIO_InitStruct_14.Pin   = GPIO_PIN_14;
  GPIO_InitStruct_14.Mode  = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct_14.Pull  = GPIO_PULLDOWN;
  GPIO_InitStruct_14.Speed = GPIO_SPEED_MEDIUM;
  
  HAL_GPIO_Init(GPIOG, &GPIO_InitStruct_14); 
}

static void __SPI_Init_CS_Pin(void){
	GPIO_InitTypeDef GPIO_InitStruct_2;
	
	__GPIOB_CLK_ENABLE();
	
	GPIO_InitStruct_2.Pin   = GPIO_PIN_2;
  GPIO_InitStruct_2.Mode  = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct_2.Pull  = GPIO_PULLUP;
  GPIO_InitStruct_2.Speed = GPIO_SPEED_LOW;
  
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct_2); 
	
}

static void __SPI_Init_SCK_Pin(void){
	GPIO_InitTypeDef GPIO_InitStruct_8;
	
	__GPIOC_CLK_ENABLE();

	GPIO_InitStruct_8.Pin   = GPIO_PIN_8;
    GPIO_InitStruct_8.Mode  = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct_8.Speed = GPIO_SPEED_MEDIUM;
	
	if(SPI_Polarity == 0){						//i don't think it will be necessary
		GPIO_InitStruct_8.Pull  = GPIO_PULLDOWN;
	} else{
		GPIO_InitStruct_8.Pull  = GPIO_PULLUP;
	}
  
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct_8); 
}

// More arg to add later maybe?: GPIO_TypeDef* MISO_Port, uint16_t MISO_Pin, GPIO_TypeDef* MOSI_Port, uint16_t MOSI_Pin, 
//		GPIO_TypeDef* SCK_Port, uint16_t SCK_Pin, GPIO_TypeDef* CS_Port, uint16_t CS_Pin, as structure

void SPI_Init(bool Polarity, bool Phase){
	SPI_Polarity = Polarity;
    SPI_Phase = Phase;

	__SPI_Init_MISO_MOSI_Pins();
	
	__SPI_Init_CS_Pin();
	
	__SPI_Init_SCK_Pin();

    //send clock, change mode to spi???
}

void __SPI_CopyString(char* Destination, char* Source, size_t SizeToCopy){
	
	while(SizeToCopy != 0) {
		*Destination = *Source;
		SizeToCopy--;
	}
}

void __SPI_SetPinTo();

uint8_t __SPI_Trasmit(char CharacterToSend){
	uint8_t ShiftRegister = CharacterToSend;


	for(uint8_t index=0; index < SPI_BUFFER_SIZE; index++){
		
        //clock low
        HAL_GPIO_Write_Pin(SPI_SCK_Port, SPI_SCK_Pin, GPIO_PIN_RESET);

        //write
        
        if((ShiftRegister & SPI_MSB_VALUE) == SPI_MSB_VALUE){
			HAL_GPIO_Write_Pin(SPI_MOSI_Port, SPI_MOSI_Pin, GPIO_PIN_SET);
		} else {
			HAL_GPIO_Write_Pin(SPI_MOSI_Port, SPI_MOSI_Pin, GPIO_PIN_RESET);
		}

        ShiftRegister = ShiftRegister << 1;

        //delay until -> do implementacji

		//clock high
        HAL_GPIO_Write_Pin(SPI_SCK_Port, SPI_SCK_Pin, GPIO_PIN_SET);
        
        //read
        if(HAL_GPIO_ReadPin(SPI_MISO_Port, SPI_MISO_Pin) == GPIO_PIN_SET){
            ShiftRegister |= 1;
        } else {
            ShiftRegister |= 0;
        }

        //delay until

	}
	
    return ShiftRegister;
}

char __SPI_ReadCharacter(void){//will not be needed. Delete
	char CharacterRead = 0;
	
	for(uint8_t index=0; index < SPI_BUFFER_SIZE; index++){
			
		//set low MOSI pin
				
		CharacterRead = CharacterRead << 1;
		CharacterRead = CharacterRead | SPI_LSB_VALUE;
	}
	
}

//Add changing conn to spi in sd card interface
void SPI_Send(char* MessageToSend){
	
    while(*MessageToSend != 0){
        __SPI_Trasmit(*MessageToSend);
        MessageToSend++;
    }
}

void SPI_Read(char* MessageReceived, size_t SizeOfBuffer){
	uint16_t index = 0;

    while(index != SizeOfBuffer){
        *MessageReceived = __SPI_Trasmit(EMPTY_MSG_VALUE);
        MessageReceived++;
        index++;
    }
}
