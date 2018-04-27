

#ifndef ADC_H_
#define ADC_H_

#define RET_OK  0
#define RET_NOK -1



int ADC_Init(void);
int ADC_Read_Pin(unsigned int pinNumber, unsigned int *readValue);



#endif
