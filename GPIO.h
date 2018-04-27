/*
 * GPIO.h
 *
 *  Created on: Oct 25, 2016
 *      Author: dogus
 */

#ifndef GPIO_H_
#define GPIO_H_

#define RET_OK  0
#define RET_NOK -1

typedef enum
{
	INPUT,
	OUTPUT
}GPIO_DIR;

typedef enum
{
	LOW,
	HIGH
}GPIO_TYPE;

int GPIO_Init(unsigned int pinNumber, GPIO_DIR direction);
GPIO_TYPE GPIO_Read_Pin(unsigned int pinNumber);
int GPIO_Write_Pin(unsigned int pinNumber, GPIO_TYPE value);


#endif /* GPIO_H_ */
