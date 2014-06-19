/**
*@file ControlTrain.c
*@author Marques Rafael, Berger Antoine et Da Silva David
*@version 1.0
*@date 1 avril 2014
*@brief BusCan configuration
*/

#include "ControlTrain.h"


void StopGoTrain(str_bus* str,int statu){
	str->id_Bus = 0x00004711;
	str->dlc = 0x5;

	str->data[0] = 0;
	str->data[1] = 0;

	str->data[2] = 0;
	str->data[3] = 0;

	str->data[4] = statu;
}
void ChangeDirection(str_bus* str,int NoTrain,int direction){
	str->id_Bus = 0x000A4711;
	str->dlc = 0x5;

	str->data[0] = 0;
	str->data[1] = 0;

	str->data[2] = NoTrain >> 8;
	str->data[3] = NoTrain & 255;

	str->data[4] = direction;
	str->data[5] = 0;

	str->data[6] = 0;
	str->data[7] = 0;
}
void ChangeSpeed(str_bus* str,int NoTrain,int speed){
	str->id_Bus = 0x00084711;
	str->dlc = 0x6;

	str->data[0] = 0;
	str->data[1] = 0;

	str->data[2] = NoTrain >> 8;
	str->data[3] = NoTrain & 255;

	str->data[4] = speed >> 8;
	str->data[5] = speed & 255;

	str->data[6] = 0;
	str->data[7] = 0;
}
void TurnLight(str_bus* str,int NoTrain,int statu){
	str->id_Bus = 0x000C4711;
	str->dlc = 0x6;

	str->data[0] = 0;
	str->data[1] = 0;

	str->data[2] = NoTrain >> 8;
	str->data[3] = NoTrain & 255;

	str->data[4] = 0;
	str->data[5] = statu;

	str->data[6] = 0;
	str->data[7] = 0;
}
