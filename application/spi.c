#include "spi.h"

#define SPI_BUFFER_SIZE		8
#define SPI_LSB_VALUE			1
#define SPI_MSB_VALUE			128

//TODO: zweryfikowac, czy poprawne wartosci sa wpisywane w elementy struktur we wszystkich Initach

//another struct for all of these??
static GPIO_TypeDef* SPI_MISO_Port;
static uint16_t SPI_MISO_Pin;
static GPIO_TypeDef* SPI_MOSI_Port;
static uint16_t SPI_MOSI_Pin;
static GPIO_TypeDef* SPI_SCK_Port;
static uint16_t SPI_SCK_Pin;
static GPIO_TypeDef* SPI_CS_Port;
static uint16_t SPI_CS_Pin;

static bool SPI_Polarity;
static bool SPI_Phase;

static char SPI_buffer[SPI_BUFFER_SIZE];

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
	
	if(SPI_Polarity == 0){											//i don't think it will be necessary
		GPIO_InitStruct_8.Pull  = GPIO_PULLDOWN;
	} else{
		GPIO_InitStruct_8.Pull  = GPIO_PULLUP;
	}
  
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct_8); 
}

// More arg to add later maybe?: GPIO_TypeDef* MISO_Port, uint16_t MISO_Pin, GPIO_TypeDef* MOSI_Port, uint16_t MOSI_Pin, 
//		GPIO_TypeDef* SCK_Port, uint16_t SCK_Pin, GPIO_TypeDef* CS_Port, uint16_t CS_Pin, 

void SPI_Init(bool Polarity, bool Phase){
	
	__SPI_Init_MISO_MOSI_Pins();
	
	__SPI_Init_CS_Pin();
	
	__SPI_Init_SCK_Pin();
}

void __SPI_CopyString(char* Destination, char* Source, size_t SizeToCopy){
	
	while(SizeToCopy != 0) {
		*Destination = *Source;
		SizeToCopy--;
	}
}

void __SPI_SendCharacter(char CharacterToSend){
	
	for(uint8_t index=0; index < SPI_BUFFER_SIZE; index++){
		
		if(SPI_Phase == 0) {
			
		} else {
			
		}
		//toggle clock ??
		
		if((CharacterToSend & SPI_MSB_VALUE) == SPI_MSB_VALUE){
			//set high MOSI pin
		} else {
			//set low MOSI pin
		}
		
		CharacterToSend = CharacterToSend << 1;
		
		//toggle clock
		
		//read
	}
	
}

char __SPI_ReadCharacter(void){
	char CharacterRead = 0;
	
	for(uint8_t index=0; index < SPI_BUFFER_SIZE; index++){
			
		//set low MOSI pin
				
		CharacterRead = CharacterRead << 1;
		CharacterRead = CharacterRead | SPI_LSB_VALUE;
	}
	
}

void SPI_Send(char* MessageToSend){
	;
}

void SPI_Read(char* MessageReceived){
	;
}
