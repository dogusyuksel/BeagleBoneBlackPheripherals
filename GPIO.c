/*
 * GPIO.c
 *
 *  Created on: Oct 25, 2016
 *      Author: dogus
 */
#include <stdio.h>
#include "GPIO.h"

int GPIO_Init(unsigned int pinNumber, GPIO_DIR direction)
{
	FILE *io,*iodir;
	io = fopen("/sys/class/gpio/export", "w");
	if(io==NULL)
		return RET_NOK;
	fseek(io,0,SEEK_SET);
	fprintf(io,"%d",pinNumber);
	fflush(io);

	char fileNameDir[40] = {0};
	sprintf(fileNameDir, "/sys/class/gpio/gpio%d/direction", pinNumber);

	iodir = fopen(fileNameDir, "w");
	if(iodir==NULL)
		return RET_NOK;
	fseek(iodir,0,SEEK_SET);

	if(direction == INPUT)
		fprintf(iodir,"in");
	else if(direction == OUTPUT)
		fprintf(iodir,"out");
	else
		return RET_NOK;

	fflush(iodir);

	fclose(iodir);
	fclose(io);

	return RET_OK;
}

int GPIO_Write_Pin(unsigned int pinNumber, GPIO_TYPE value)
{
	FILE *ioval;
	char fileNameVal[40] = {0};
	sprintf(fileNameVal, "/sys/class/gpio/gpio%d/value", pinNumber);

	ioval = fopen(fileNameVal, "w");
	if(ioval==NULL)
		return RET_NOK;
	fseek(ioval,0,SEEK_SET);

	if(value == HIGH)
		fprintf(ioval,"%d",1);
	else if(value==LOW)
		fprintf(ioval,"%d",0);
	else
		return RET_NOK;

	fflush(ioval);
	fclose(ioval);

	return RET_OK;
}

GPIO_TYPE GPIO_Read_Pin(unsigned int pinNumber)
{
	FILE *inval;
	int value;

	char fileNameVal[40] = {0};
	sprintf(fileNameVal, "/sys/class/gpio/gpio%d/value", pinNumber);

	inval = fopen(fileNameVal, "r");
	if(inval == NULL)
		return RET_NOK;
	fseek(inval,0,SEEK_SET);
	fscanf(inval, "%d", &value);
	fclose(inval);

	if(value)
		return HIGH;
	else
		return LOW;
}
