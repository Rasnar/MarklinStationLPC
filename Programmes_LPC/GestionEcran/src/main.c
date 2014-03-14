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
#include "mario.h"
#endif


int main(void) {

	Init_display();
	Set_cursor(0,0);
	Create_partial_screen(0, 319, 0, 239);

	/* Display size = 320x240 */
	// All screen unlock
	// Create_partial_screen(0, 319, 0, 239);

	/*Set the "background" of the display*/
	int var;
	for (var = 0; var < 320*240; var++) {
		Write_pixel(0,0,0);
	}

	Draw_Images(&mario);
	/*	Test writing letter
	uint8_t color[3] = {255,0,0};
	Write_char('A',0,0,color);*/
	while(1) {
	}
	return 0 ;
}
