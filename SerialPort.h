/*
 * SerialPort.h
 *
 *  Created on: Oct 25, 2016
 *      Author: dogus
 */

#ifndef SERIALPORT_H_
#define SERIALPORT_H_

#define RETURN_OK   1
#define RETURN_NOK -1

#define DATA_AVAILABILITY_TIMEOUT 100000 //100 ms

typedef struct
{
	unsigned int baud_rate;
	unsigned char data_bits;
	unsigned char stop_bits;
	char parity;
	unsigned char rts_cts;
	unsigned char dtr_mode;
}Uart_Comm_Param;


int Uart_Init(const int portIdx, unsigned int baud);
void Uart_Close(int fd);
void Uart_Flush(int fd);
int Uart_Send(int fd, unsigned char *buf, unsigned int buf_size); //send a buffer with specified length
int Uart_Receive(int fd, unsigned char *buf, unsigned int buf_size, unsigned int vaitINms); //receive buffer
int Uart_ReadByte(int fd, unsigned char *buf, unsigned int vaitINms); //receive single byte
int CheckDataAvailability(int fileIndx, unsigned char blocking, long int time_sec, long int time_usec);

#endif /* SERIALPORT_H_ */
