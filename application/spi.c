#include "spi.h"

#define SPI_BUFFER_SIZE	8

static char SPI_buffer[SPI_BUFFER_SIZE];


void SPI_Init(void){
	SPI_InitTypeDef   SPI_InitStructure;
	
	__HAL_RCC_SPI1_CLK_ENABLE();		//?? który interfejs?
	
  static SPI_HandleTypeDef spi = { .Instance = SPI1 };	//ponownie, który interfejs?
  
	SPI_InitStructure.Direction = SPI_DIRECTION_2LINES;
  SPI_InitStructure.Mode = SPI_MODE_MASTER;
  SPI_InitStructure.DataSize = SPI_DATASIZE_8BIT;
  SPI_InitStructure.CLKPolarity = SPI_POLARITY_HIGH;
  SPI_InitStructure.CLKPhase = SPI_PHASE_2EDGE;
  SPI_InitStructure.NSS = SPI_NSS_SOFT;
  SPI_InitStructure.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_4;
  SPI_InitStructure.FirstBit = SPI_FIRSTBIT_MSB;
  SPI_InitStructure.CRCPolynomial = 7;
	
	spi.Init = SPI_InitStructure;
	
  HAL_SPI_Init(&spi);
	
}

void SPI_Send(char* MessageToSend){
	;
}


void SPI_Read(char* MessageReceived){
	;
}
