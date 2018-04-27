#include "ADC.h"
#include <stdio.h>

#define MGRNUM	8

int ADC_Init(void)
{
	FILE *ain;
	ain = fopen("/sys/devices/bone_capemgr.8/slots", "w");
	if(ain == NULL)
		return RET_NOK;
	fseek(ain,0,SEEK_SET);
	fprintf(ain,"cape-bone-iio");
	fflush(ain);
	fclose(ain);
	return RET_OK;
}

int ADC_Read_Pin(unsigned int pinNumber, unsigned int *readValue)
{
	FILE *aval;
	int value;
	char buf[50] = {0};
	sprintf(buf, "/sys/devices/ocp.2/helper.14/AIN%d", pinNumber);
	aval = fopen(buf, "r");
	if(aval == NULL)
		return RET_NOK;
	fseek(aval,0,SEEK_SET);
	fscanf(aval,"%d",&value);
	fflush(aval);
	fclose(aval);
	*readValue = value;
	return RET_OK;
}
