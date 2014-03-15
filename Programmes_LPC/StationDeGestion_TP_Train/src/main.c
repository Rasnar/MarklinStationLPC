/*
===============================================================================
 Name        : main.c
 Author      : 
 Version     :
 Copyright   : Copyright (C) 
 Description : main definition
===============================================================================
*/

#ifdef __USE_CMSIS
#include "LPC17xx.h"
#include "Ecran.h"
#include "police.h"
//#include "mario.h"
#include "SPI.h"
#endif

//const tImage mario = {image_data_mario, 320, 240};

int main(void) {

	Init_display();
	Set_cursor(0,0);
	/* -- All screen selected -- */
	/* Display size = 320x240 */
	Create_partial_screen(0, 319, 0, 239);

	/*Set the "background" on the LCD*/
	int var;
	for (var = 0; var < 320*240; var++) {
		Write_pixel(0,0,0);
	}

	/* -- Test to draw an image on the display -- */
	//Select_display_bus();
	//Draw_Image(&mario);

	/* -- Test writing letter -- */
	/*uint8_t color[3] = {255,255,255};
	uint8_t color_back[3] = {0,0,255};
	Write_char_with_background('A',90,0,color,color_back);
	*/

    /* -- Test writing letter without background -- */
	/*uint8_t color2[3] = {255,255,255};
	Write_char('B',90,90,color2);*/

	/* -- SPI for touchscreen -- */

	Select_control_bus();
	LPC_GPIO0->FIODIR |= 1<<8;
	LPC_GPIO0->FIOCLR = 1 << 8; // Select (/CS) touchscreen
	Valide_datas_bus_to_extlab2();
	Init_SPI_master_mode(1, 0, 100, 8);
	//uint8_t test1 = Write_Read_SPI_8bits(0x80|0x10); // Send options
	//uint8_t test2 = Write_Read_SPI_8bits(0x00);
	//uint8_t test3 = Write_Read_SPI_8bits(0x80|0x10);

	while(1) {

		uint8_t test1 = Write_Read_SPI_8bits(0x80|0x30|0x04|0x3); // Send options
		uint8_t test2 = Write_Read_SPI_8bits(0x00);
		if((test1 > 0) || (test2>0))
			break;
	}
	return 0 ;
}
