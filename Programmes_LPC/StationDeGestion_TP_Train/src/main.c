/*
 *@file main.c
 *@author Swagteam
 *@version 1.0
 *@date 1 april 2014
 *@brief main Main
 */

#ifdef __USE_CMSIS
#include "LPC17xx.h"
#include "Ecran.h"
#include "police.h"
#include "SPI.h"
#include "SD.h"
#include "Touchscreen.h"
#include "uart.h"
#include "button.h"
#include "stdbool.h"
#include "ControlTrain.h"
#endif
#define MAX_TRAIN 100

#define START_TRAIN "STATRAIN"
#define STOP_TRAIN "STOTRAIN"
#define CHANGE_SPEED "LxxVxxxx"
#define CHANGE_DIR "LxxSx"

#define OPTION_SPEED 0
#define OPTION_TRAIN_NUMBER 1

#define SPI_RATE_TOUCHSCREEN 1500000

bool flag_interrupt = 0;
uint16_t lights[MAX_TRAIN];
uint16_t speed_train[MAX_TRAIN];
bool train_direction[MAX_TRAIN]; // 1 => forward, 0 => back
int n_train = 1;
bool start_or_stop = 0; // 0 => stop, 1 => start

uint8_t red[3] = { 255, 0, 0 };
uint8_t green[3] = { 0, 255, 0 };
uint8_t blue[3] = { 0, 0, 255 };
uint8_t color_button[3] = { 229, 208, 64 };
uint8_t black[3] = { 0, 0, 0 };

uint8_t option_selected = OPTION_TRAIN_NUMBER;

/**
 *@brief Interruption generated by pressing on the touchscreen OR the rotate button
 *@param val Value to be converted to a string
 *@param base Base on which we want the conversion
 *@return A pointer to the string that contain the conversion
 */
char* itoa(int val, int base) {
	static char buf[32] = { 0 };
	int i = 30;
	for (; val && i; --i, val /= base)
		buf[i] = "0123456789abcdef"[val % base];
	return &buf[i + 1];
}

/**
 *@brief Interruption generated by pressing on the touchscreen OR the rotate button
 *@details When someone press on the screen this interruption is called,
 *@details we run a timer to ignore all rebounds. The "flag_interrupt", which
 *@details valid the pressure, is only set after the end of timer. This flag
 *@details is scrutinized in the main function.
 *@details If we press the rotate button we just need to see if the other "edge"
 *@details state to know which way we are turning
 */
void EINT3_IRQHandler(void) {
	if (LPC_GPIOINT->IO2IntStatF == (1 << 10)) {
		/* -- Clear interrupt on the touchscreen -- */
		LPC_GPIOINT->IO2IntClr |= 1 << 10;

		//flag_interrupt = 1;
		/* if anti-rebound is not running */
		if (!LPC_TIM0->TCR) {
			LPC_TIM0->MR0 = LPC_TIM0->TC + 25000; /* Interruption in the next 1ms */
			LPC_TIM0->TCR = 1; /* Enable timer 0 for anti-rebound */
		}
	}
	/* If rotate button turned */
	else if (LPC_GPIOINT->IO2IntStatR == (1 << 6)) {
		LPC_GPIOINT->IO2IntClr |= 1 << 6;
		if ((LPC_GPIO2->FIOPIN & (1 << 5)) == (1 << 5)) {
			switch (option_selected) {
			case OPTION_SPEED:
				if (speed_train[n_train] < 1000)
				{
					speed_train[n_train] = speed_train[n_train] + 50;
					send_speed(n_train, speed_train[n_train]);
					Write_string_with_background("0   ", 170, 280, black,color_button);
					Write_string_with_background(itoa(speed_train[n_train], 10), 170, 280, black,color_button);
				}
				break;
			case OPTION_TRAIN_NUMBER:
				if (n_train < 99)
				{
					n_train = n_train + 1;
					/* Show the train number selected */
					Write_string_with_background("  ", 172, 35, black,color_button);
					Write_string_with_background(itoa(n_train, 10), 172, 35, black,color_button);
					/* Show the speed value of the train selected */
					Write_string_with_background("0   ", 170, 280, black,color_button);
					Write_string_with_background(itoa(speed_train[n_train], 10), 170, 280, black,color_button);
				}
				break;
			}

		} else {
			switch (option_selected) {
			case OPTION_SPEED:
				if (speed_train[n_train] > 0)
				{
					speed_train[n_train] = speed_train[n_train] - 50;
					send_speed(n_train, speed_train[n_train]);
					Write_string_with_background("0   ", 170, 280, black,color_button);
					Write_string_with_background(itoa(speed_train[n_train], 10), 170, 280, black,color_button);
				}
				break;
			case OPTION_TRAIN_NUMBER:
				if (!(n_train <= 1))
				{
					n_train = n_train - 1;
					/* Show the train number selected */
					Write_string_with_background("  ", 172, 35, black,color_button);
					Write_string_with_background(itoa(n_train, 10), 172, 35, black,color_button);
					/* Show the speed value of the train selected */
					Write_string_with_background("0   ", 170, 280, black,color_button);
					Write_string_with_background(itoa(speed_train[n_train], 10), 170, 280, black,color_button);
				}
				break;
			}
		}
	}
}

/**
 *@brief Interrupt generated by a timer when we press on the touchscreen
 *@details This timer permit to ignore multiple pressions on the screen
 *@details because of rebounds.
 */
void TIMER0_IRQHandler() {
	LPC_TIM0->TCR = 0; /* Disable timer 0 */
	/* if interruption on touchscreen still usable (not a glitch) */
	if (((LPC_GPIO2->FIOPIN & (1 << 10)) == 0))
		flag_interrupt = 1;
	/* Clear interrupt flag */
	LPC_TIM0->IR = 1;
}

int main(void) {

	/* Wait screen to be ready */
	int var;
	for (var = 0; var < 10000000; ++var);

	for (var = 0; var < MAX_TRAIN; var++)
	{
		train_direction[var] = 1;
		speed_train[var] = 0;
		lights[var] = 1;
	}

	Init_Extlab2();
	Init_Rotate_button();
	Init_display();
	Select_display_bus();

	Set_cursor(0, 0);
	/* -- All screen selected -- */
	/* Display size = 320x240 */
	Create_partial_screen(0, 319, 0, 239);

	/* -- Init touchscreen and configuration anti-rebound -- */
	Init_SPI_master_mode(0, 0, 300000, 8);
	LPC_TIM0->MCR = 1; /* Interrupt on MR0 value */
	NVIC_EnableIRQ(TIMER0_IRQn);
	Init_touchscreen();

	init_SD();
	Read_SD_multi_block(450);
	/* Restore frequency used with the touchscreen */
	Change_Frequency_SPI(SPI_RATE_TOUCHSCREEN);

	/* Init UART to use on XBEE */
	uart3_init(9600);

	/* Add buttons to specifics positions */
	AddButton("START_STOP", 30, 90, 10, 70);
	AddButton("TRAIN_NUMBER", 150, 210, 10, 70);
	AddButton("DIRECTION", 20, 80, 250, 310);
	AddButton("LIGHTS", 90, 150, 250, 310);
	AddButton("SPEED", 160, 220, 250, 310);


	/* Show the train number selected */
	Write_string_with_background("  ", 172, 35, black,color_button);
	Write_string_with_background(itoa(n_train, 10), 172, 35, black,color_button);

	/* Show the speed value of the train selected */
	Write_string_with_background("0   ", 170, 280, black,color_button);
	Write_string_with_background(itoa(speed_train[n_train], 10), 170, 280, black,color_button);

	/* Show the start button */
	change_zone_color(35, 84, 15, 64, green);
	Write_string_with_background("START", 40, 29, black,green);
	/* Stop all trains */
	uart3_send(STOP_TRAIN, 9); // Send data on uart

	while (1) {
		/* If touchscreen pressed */
		if ((flag_interrupt == 1) && ((LPC_GPIO2->FIOPIN & (1 << 10)) == 0)) {
			uint16_t x = 0;
			uint16_t y = 0;

			/* read positions on touchscreen */
			Read_x_and_y_12bits(&x, &y);

			/* Except a bug of read SPI on touchscreen */
			if ((x == 0) || (y == 4095)) {
				flag_interrupt = 0;
				continue;
			}

			uint32_t x_pixel = (240 * (uint32_t) x) / 4096;
			uint32_t y_pixel = 320 - (320 * (uint32_t) y) / 4096;

			char * button_name = IsClickOnAButton(x_pixel, y_pixel);
			if (button_name != 0) {
				if (!strcmp(button_name, "START_STOP")) {
					if (start_or_stop) {
						uart3_send(STOP_TRAIN, 9); // Send data on uart
						change_zone_color(35, 84, 15, 64, green);
						Write_string_with_background("START", 40, 29, black,green);
					} else {
						uart3_send(START_TRAIN, 9); // Send data on uart
						change_zone_color(35, 84, 15, 64, red);
						Write_string_with_background("STOP", 45, 29, black,	red);
					}
					start_or_stop = !start_or_stop;
				} else if (!strcmp(button_name, "TRAIN_NUMBER")) {
					option_selected = OPTION_TRAIN_NUMBER;
				} else if (!strcmp(button_name, "DIRECTION")) {
					send_direction(n_train, train_direction[n_train]);
					train_direction[n_train] = !train_direction[n_train];
				} else if (!strcmp(button_name, "SPEED")) {
					option_selected = OPTION_SPEED;
				} else if (!strcmp(button_name, "LIGHTS")) {
					send_lights(n_train, lights[n_train]);
					lights[n_train] = !lights[n_train];
				}
			}
			flag_interrupt = 0;
		}
	}
	return 0;
}
