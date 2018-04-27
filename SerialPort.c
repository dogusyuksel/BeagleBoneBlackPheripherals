/*
 * SerialPort..c
 *
 *  Created on: Oct 25, 2016
 *      Author: dogus
 */

#include "SerialPort.h"

#include <stdio.h>
#include <stddef.h>
#include <time.h>
#include <termios.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>

int Uart_Init(const int portIdx, unsigned int baud)
{
	if(portIdx < 0)
		return RETURN_NOK;

	char portName[32] = {0};
	unsigned char BAUD = 0;

	if(baud == 4800)
		BAUD = B4800;
	else if(baud == 9600)
		BAUD = B9600;
	else if(baud == 19200)
		BAUD = B19200;
	else
		return RETURN_NOK;

	int file;

	if(portIdx<0 || portIdx>5)
		return RETURN_NOK;

	sprintf(portName, "/dev/ttyO%d", portIdx);

	if ((file = open(portName, O_RDWR | O_NOCTTY | O_NDELAY))<0){
		return RETURN_NOK;
	}

	struct termios options; // the termios structure is vital
	tcgetattr(file, &options); // sets the parameters associated with file
	// Set up the communications options:
	// 9600 baud, 8-bit, enable receiver, no modem control lines

	options.c_cflag = BAUD | CS8 | CREAD | CLOCAL;
	options.c_iflag = IGNPAR | ICRNL; // ignore partity errors, CR -> newline
	options.c_oflag = 0;
	options.c_lflag = 0;

	options.c_cc[VTIME] = 0;
	options.c_cc[VMIN] = 1;

	tcflush(file, TCIFLUSH); // discard file information not transmitted
	tcsetattr(file, TCSANOW, &options); // changes occur immmediately


	return file;
}

void Uart_Close(int fd)
{
	close(fd);
}

int Uart_Send(int fd, unsigned char *buf, unsigned int buf_size)
{
	if(write(fd,buf,buf_size)<0)
		return RETURN_NOK;
	return RETURN_OK;
}

int CheckDataAvailability(int fileIndx, unsigned char blocking, long int time_sec, long int time_usec) //it control if data comes or not over the file
{
    fd_set fds;
    struct timeval tv;
    int sel_ret = 0;
    int ret = 0;

    FD_ZERO(&fds);

    if(fileIndx != -1)
        FD_SET(fileIndx, &fds);

    if(!blocking)
    {
        tv.tv_sec = time_sec;
        tv.tv_usec = time_usec;
        sel_ret = select((fileIndx + 1), &fds, NULL, NULL, &tv);
    }
    else
    {
        sel_ret = select((fileIndx + 1), &fds, NULL, NULL, NULL);
    }

    if(sel_ret > 0)
    {
        if((fileIndx != -1) && (FD_ISSET(fileIndx, &fds)))
            ret = 1;
       else
            ret = sel_ret;
    }
    else if (sel_ret == -1)
        ret =  -1;

    return ret;

}


int Uart_ReadByte(int fd, unsigned char *buf, unsigned int vaitINms)
{
	if(CheckDataAvailability(fd, 0, 0, (1000 * vaitINms))==1)//100 ms wait
	{
		return read(fd, (unsigned char *)buf, 1);
	}
	else
		return RETURN_NOK;
}



int Uart_Receive(int fd, unsigned char *buf, unsigned int buf_size, unsigned int vaitINms)
{
	int i = 0;
	for(i=0;i<buf_size;i++)
	{
		if(Uart_ReadByte(fd, &buf[i], vaitINms) != 1)
			return RETURN_NOK;
	}
	return buf_size;
}

void Uart_Flush(int fd)
{
	tcflush(fd, TCIOFLUSH);
}

