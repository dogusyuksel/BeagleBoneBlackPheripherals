/*
 * UserLeds.c
 *
 *  Created on: Oct 25, 2016
 *      Author: dogus
 */

#include "UserLeds.h"

#include <stdio.h>

FILE *led1, *led2, *led3, *led4;
unsigned char isInitExecuted = 0;

int UserLedsInit()
{
	led1 = fopen("/sys/class/leds/beaglebone:green:usr0/brightness", "w");
	led2 = fopen("/sys/class/leds/beaglebone:green:usr1/brightness", "w");
	led3 = fopen("/sys/class/leds/beaglebone:green:usr2/brightness", "w");
	led4 = fopen("/sys/class/leds/beaglebone:green:usr3/brightness", "w");

	if((led1==NULL)||(led2==NULL)||(led3==NULL)||(led4==NULL))
		return -1;

	//Slience
	fprintf(led4,"%d",0);
	fflush(led4);
	fprintf(led3,"%d",0);
	fflush(led3);
	fprintf(led2,"%d",0);
	fflush(led2);
	fprintf(led1,"%d",0);
	fflush(led1);

	isInitExecuted = 1;
	return 1;
}

void UserLedsClose()
{
	fclose(led1);
	fclose(led2);
	fclose(led3);
	fclose(led4);
}

void LedON(unsigned char ledNumber)
{
	if(isInitExecuted==0)
		UserLedsInit();

	if(ledNumber==1)
	{
		fprintf(led1,"%d",1);
		fflush(led1);
	}else if(ledNumber==2)
	{
		fprintf(led2,"%d",1);
		fflush(led2);
	}else if(ledNumber==3)
	{
		fprintf(led3,"%d",1);
		fflush(led3);
	}else if(ledNumber==4)
	{
		fprintf(led4,"%d",1);
		fflush(led4);
	}
}


void LedOFF(unsigned char ledNumber)
{
	if(isInitExecuted==0)
		UserLedsInit();

	if(ledNumber==1)
	{
		fprintf(led1,"%d",0);
		fflush(led1);
	}else if(ledNumber==2)
	{
		fprintf(led2,"%d",0);
		fflush(led2);
	}else if(ledNumber==3)
	{
		fprintf(led3,"%d",0);
		fflush(led3);
	}else if(ledNumber==4)
	{
		fprintf(led4,"%d",0);
		fflush(led4);
	}
}
