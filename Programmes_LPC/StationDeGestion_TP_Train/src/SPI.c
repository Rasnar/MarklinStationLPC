/*
 * SPI.c
 *
 *  Created on: 14 mars 2014
 *      Author: David
 */
#include "SPI.h"

/* CPOL = rising/failling edge
 * CPHA = clock phase control */
void Init_SPI_master_mode(uint8_t S_CPHA, uint8_t S_CPOL, uint32_t SPI_rate, uint8_t _16_or_8bits_com)
{
	LPC_PINCON->PINSEL0 |=0x5500;

	if(_16_or_8bits_com == 8)
		LPC_SPI->SPCR &=~ BIT_ENABLE; // set 8bits mode on SPI
	else
		LPC_SPI->SPCR |= BIT_ENABLE; // set 16bits mode on SPI

	LPC_SPI->SPCR |= 1 << MODE_SELECT; // configure spi on master mode
	LPC_SPI->SPCCR = 1000; 	// SCK = pclk/8
}

void Write_only_SPI_8bits(uint8_t datas)
{
	LPC_SPI->SPDR = datas;
	while(!(LPC_SPI->SPSR & (1 << SPIF)));
}

uint8_t Write_Read_SPI_8bits(uint8_t datas)
{
	LPC_SPI->SPDR = datas;
	while((LPC_SPI->SPSR & (1 << SPIF)) == 0);
	return LPC_SPI->SPDR;
}
