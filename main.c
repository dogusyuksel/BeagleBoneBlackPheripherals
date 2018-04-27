/*
 * main.c
 *
 *  Created on: Oct 25, 2016
 *      Author: dogus
 */
#include "AES.h"
#include "FileSystem.h"
#include "UserLeds.h"
#include "GPIO.h"
#include "SerialPort.h"

#include<stdio.h>
#include<stdint.h>
#include<string.h>
#include<stdlib.h>

#include<sys/socket.h>
#include<arpa/inet.h>
#include<unistd.h>

unsigned char buffer[16] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66};

#define MAIN_C_FILE_NAME "/home/root/MAIN_APPLICATION_C.c"
#define MAIN_APP_FILE_NAME "/home/root/MAINAPP"
#define MAIN_APP_FILE_PATH "/home/root/"

//USB Variables
#define USB_FILE_PATH "/media/D_USB/Generated_Image.c"
#define BufferSize 16
#define SpecialBuffer "////Dogus Yuksel"

//SerialPort Variables
int fileUploaderMessagefinished = 0;
unsigned int fileUploaderPacketCounter = 0;
char m_fileUploadName[120] = {0};
FILE *iFileHandleFileUploadFunction;
int FileCounter = 0;

//TCP Variables
#define TCP_PORT 2503
int client_sock = -1;

unsigned char isBootSuccess = 0;
unsigned char isItCabled = 0;

int PORT_USBDEV = -1;

void pauseSec(int sec);
char  cPCExecute_FILE_UPLOAD(char * message, unsigned int messageLen, int port);
char  cPCExecute_FILE_UPLOAD_TCP(char * message, unsigned int messageLen);
void SendShellCommandOutputViaSerialPort(char *command);

int main()
{
	int dumLed = 0;
	for(dumLed = 0; dumLed<5;dumLed++)
	{
		LedON(1);
		LedON(2);
		LedOFF(3);
		LedOFF(4);
		pauseSec(1);
		LedON(3);
		LedON(4);
		LedOFF(1);
		LedOFF(2);
		pauseSec(1);
	}
	for(dumLed=1;dumLed<=4;dumLed++)
		LedOFF(dumLed);

	///////control USB here
	char usbFileName[120] = {0};
	sprintf(usbFileName, "%s", (char *)USB_FILE_PATH);
	if (FileExist(usbFileName) != -1)
	{
		int size = FileSize(usbFileName);

		printf("DODO_ %s   %d\n", __func__, __LINE__);
		FILE *usbFileIndex = FileOpen(usbFileName, R_ONLY);
		if(usbFileIndex == NULL)
		{
			printf("Cannot Open the USB File\n");
			return 0;
		}
		if (FileExist((char *)MAIN_C_FILE_NAME) != -1)
		{
			FileRemove((char *)MAIN_C_FILE_NAME);
		}
		FILE *fileEncrypted = FileOpen((char *)MAIN_C_FILE_NAME, RA);
		if(fileEncrypted == NULL)
		{
			printf("Cannot Created the new decrypted file\n");
			return 0;
		}

		Aes_Init(AES_128);
		char *buffer = (char *)malloc(BufferSize * sizeof(char));
		int i = 0;

		printf("DODO_ Size: %d\n", size);
		for(i=0;i<(size/BufferSize);i++)
		{
			memset(buffer, 0, BufferSize);
			FileSeek(usbFileIndex, BufferSize * i, SEEK_SET);
			FileRead(usbFileIndex, buffer, BufferSize);
			DecryptBlock(buffer);
			FileWrite(fileEncrypted, buffer, BufferSize);

			//printf("DODO_ %s   %d\n", __func__, __LINE__);
			if(i== ((size/BufferSize) - 1))
			{
				if(memcmp(buffer, (char *)SpecialBuffer, BufferSize) == 0)
				{
					printf("DODO_ %s   %d\n", __func__, __LINE__);
					//boot success
					isBootSuccess = 1;
				}
			}
		}
		printf("DODO_ %s   %d\n", __func__, __LINE__);
		FileClose(usbFileIndex);
		FileClose(fileEncrypted);
	}
	else
	{
		isItCabled = 1;
		printf("DODO_ %s   %d\n", __func__, __LINE__);
		//means there is no USB, search for serialport here
		GPIO_Init(48, INPUT);

		GPIO_TYPE ret = GPIO_Read_Pin(48);
		if(ret == HIGH)
		{
			printf("DODO_ there is serial hardware\n");
			unsigned int generalTimeout = 25400; //it is 25 seconds
			//OsPortReset(PORT_USBDEV);
			int receiveReturn = 0;
			unsigned char message[2500] = {0}; //ADDED
			memset(message, 0, 2500);//ADDED
			PORT_USBDEV = Uart_Init(4, 9600);//ADDED
			if(PORT_USBDEV<0)//ADDED
				printf("serial port open fail\n");
			unsigned int messageLen = 0; //ADDED
			Uart_Flush(PORT_USBDEV);


			while(1)
			{
				receiveReturn = Uart_Receive(PORT_USBDEV, &message[0], 2, generalTimeout);
				printf("receive return len: %d   line: %d\n", receiveReturn, __LINE__);
				if(receiveReturn == 2)
				{
					if((message[0] == 0xEE) && (message[1] == 0xEE))
					{
						//end of proccess
						//Uart_Close(PORT_USBDEV);
						printf("END OF WHOLE PROCESS\n");
						isBootSuccess = 1;
						break;
					}
					else
					{
						if((message[0] == 0xFE) && (message[1] == 0xD0))
						{
							receiveReturn = Uart_Receive(PORT_USBDEV, &message[2], 2, generalTimeout);
							if(receiveReturn == 2)
							{
								unsigned int len = (message[3] * 256) + message[2];
								receiveReturn = Uart_Receive(PORT_USBDEV, &message[4], len + 1, generalTimeout);
								if(receiveReturn == len + 1)
								{
									messageLen = len + 5;
									//it means full packet come, DONT CONTROL CRC AND PROCCESS DIRECTLY
									cPCExecute_FILE_UPLOAD(message, messageLen, PORT_USBDEV);
								}
								else{
									Uart_Close(PORT_USBDEV);
									printf("COmm error line: %d\n", __LINE__);
									//return 0;
								}
							}
							else{
								Uart_Close(PORT_USBDEV);
								printf("COmm error line: %d\n", __LINE__);
								break;
								//return 0;
							}
						}
						else{
							Uart_Close(PORT_USBDEV);
							printf("COmm error line: %d\n", __LINE__);
							break;
							//return 0;
						}
					}
				}
				else{
					Uart_Close(PORT_USBDEV);
					printf("COmm error line: %d\n", __LINE__);
					break;
					//return 0;
				}
			  }
		}
		else
		{
			//means there is no serial port hardware, search for tcp ip connection
			printf("DODO_ there is NO serial hardware\n");
			isItCabled = 1;
			int socket_desc , c;
			struct sockaddr_in server , client;

			//Create socket
			socket_desc = socket(AF_INET , SOCK_STREAM , 0);
			if (socket_desc == -1)
			{
				printf("Could not create socket\n");
			}
			printf("Socket created\n");

			//Prepare the sockaddr_in structure
			server.sin_family = AF_INET;
			server.sin_addr.s_addr = INADDR_ANY;
			server.sin_port = htons( TCP_PORT );

			//Bind
			if( bind(socket_desc,(struct sockaddr *)&server , sizeof(server)) < 0)
			{
				//print the error message
				printf("bind failed. Error: line: %d\n", __LINE__);
				return 1;
			}
			printf("bind done\n");

			//Listen
			listen(socket_desc , 3);

			//Accept and incoming connection
			printf("Waiting for incoming connections...\n");
			c = sizeof(struct sockaddr_in);

			//accept connection from an incoming client
			client_sock = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c);
			if (client_sock < 0)
			{
				printf("accept failed\n");
				return 0;
			}
			printf("Connection accepted\n");

			///////////////////////
			int receiveReturn = 0;
			unsigned char message[2500] = {0}; //ADDED
			memset(message, 0, 2500);//ADDED
			unsigned int messageLen = 0; //ADDED

			while(1)
			{
				receiveReturn = recv(client_sock , &message[0], 2 , 0);
				printf("receive return len: %d   line: %d\n", receiveReturn, __LINE__);
				if(receiveReturn == 2)
				{
					if((message[0] == 0xEE) && (message[1] == 0xEE))
					{
						//end of proccess
						//fflush(stdout);
						printf("END OF WHOLE PROCESS TCP\n");
						isBootSuccess = 1;
						break;
					}
					else
					{
						if((message[0] == 0xFE) && (message[1] == 0xD0))
						{
							receiveReturn = recv(client_sock , &message[2], 2 , 0);
							if(receiveReturn == 2)
							{
								unsigned int len = (message[3] * 256) + message[2];
								receiveReturn = recv(client_sock , &message[4], len+1 , 0);
								if(receiveReturn == len + 1)
								{
									messageLen = len + 5;
									//it means full packet come, DONT CONTROL CRC AND PROCCESS DIRECTLY
									cPCExecute_FILE_UPLOAD_TCP(message, messageLen);
								}
								else{
									fflush(stdout);
									printf("COmm error line: %d\n", __LINE__);
									//return 0;
								}
							}
							else{
								fflush(stdout);
								printf("COmm error line: %d\n", __LINE__);
								break;
								//return 0;
							}
						}
						else{
							fflush(stdout);
							printf("COmm error line: %d\n", __LINE__);
							break;
							//return 0;
						}
					}
				}
				else{
					fflush(stdout);
					printf("COmm error line: %d\n", __LINE__);
					break;
					//return 0;
				}
			  }
			///////////////////////
		}
	}

	if(isBootSuccess==1)
	{
		printf("DODO_ %s   %d\n", __func__, __LINE__);
		for(dumLed=0;dumLed<5;dumLed++)
		{
			int dumLed2 = 0;
			for(dumLed2=1;dumLed2<=4;dumLed2++)
				LedON(dumLed2);
			pauseSec(1);
			for(dumLed2=1;dumLed2<=4;dumLed2++)
				LedOFF(dumLed2);
			pauseSec(1);
		}

		FileRemove((char *)MAIN_APP_FILE_NAME);

		//arm-angstrom-linux-gnueabi-gcc
		char command[1024] = {0};
		sprintf(command, "arm-angstrom-linux-gnueabi-gcc %s -o %s", (char *)m_fileUploadName, (char *)MAIN_APP_FILE_NAME);
		SendShellCommandOutputViaSerialPort(command);
		pauseSec(10);

		memset(command, 0, 1024);
		sprintf(command, "%s", (char *)MAIN_APP_FILE_NAME);
		system(command);
		pauseSec(1);
	}
	else
	{
		printf("DODO_ %s   %d\n", __func__, __LINE__);
		//just run old one
		char command[1024] = {0};
		sprintf(command, "%s", (char *)MAIN_APP_FILE_NAME);
		system(command);
		pauseSec(1);
	}

	printf("DODO_ %s   %d\n", __func__, __LINE__);
	return 1;
}

void pauseSec(int sec)
{
	time_t now,later;

	now = time(NULL);
	later = time(NULL);

	while((later - now) < (double)sec)
		later = time(NULL);
}

char  cPCExecute_FILE_UPLOAD(char * message, unsigned int messageLen, int port)
{
	fileUploaderPacketCounter++;

	char caMsg[30] = {0};

	fileUploaderMessagefinished = 0;
	if(messageLen==5)
		fileUploaderMessagefinished = 1;


	unsigned int len = messageLen - 5;

	if(fileUploaderPacketCounter == 1)
	{
		memset(m_fileUploadName, 0, 120);
		//isPacketFileUploader = 1; //first packet does not contain 0xFE, because it contains file name only, not file data
		//create file
		//sprintf(m_fileUploadName, "%s", (char *)MAIN_C_FILE_NAME);
		unsigned int len = (message[3] * 256) + message[2];
		memcpy(m_fileUploadName, &message[4], len);

		if (FileExist(m_fileUploadName) != -1)
		{
			FileRemove(m_fileUploadName);
		}
		iFileHandleFileUploadFunction = FileOpen(m_fileUploadName, RA);

		printf("SERIAL_ incoming packet number: %d\n", fileUploaderPacketCounter);
		FileCounter++;
	}
	else
	{
		//append file
		FileWrite(iFileHandleFileUploadFunction, &message[4] , len);

		sprintf (caMsg, "%u", fileUploaderPacketCounter);
		printf ("%s\n",caMsg);

		if(fileUploaderMessagefinished == 1)
		{
			fileUploaderPacketCounter = 0;
			fileUploaderMessagefinished = 0;

			//info message and close file
			FileClose(iFileHandleFileUploadFunction);
			iFileHandleFileUploadFunction = NULL;
			//isPacketFileUploader = 0;

			//memset(m_fileUploadName, 0, 120);
			//FileCounter = 0;
		}
	}

	unsigned char dumB[2] = {0, 0}; //REPLY_ERROR_CODE_OK
	Uart_Send(port, dumB, 2);

	return 1;
}

char  cPCExecute_FILE_UPLOAD_TCP(char * message, unsigned int messageLen)
{
	fileUploaderPacketCounter++;

	char caMsg[30] = {0};

	fileUploaderMessagefinished = 0;
	if(messageLen==5)
		fileUploaderMessagefinished = 1;


	unsigned int len = messageLen - 5;

	if(fileUploaderPacketCounter == 1)
	{
		memset(m_fileUploadName, 0, 120);
		//isPacketFileUploader = 1; //first packet does not contain 0xFE, because it contains file name only, not file data
		//create file
		//sprintf(m_fileUploadName, "%s", (char *)MAIN_C_FILE_NAME);
		unsigned int len = (message[3] * 256) + message[2];
		memcpy(m_fileUploadName, &message[4], len);

		if (FileExist(m_fileUploadName) != -1)
		{
			FileRemove(m_fileUploadName);
		}
		iFileHandleFileUploadFunction = FileOpen(m_fileUploadName, RA);

		printf("SERIAL_ incoming packet number: %d\n", fileUploaderPacketCounter);
		FileCounter++;
	}
	else
	{
		//append file
		FileWrite(iFileHandleFileUploadFunction, &message[4] , len);

		sprintf (caMsg, "%u", fileUploaderPacketCounter);
		printf ("%s\n",caMsg);

		if(fileUploaderMessagefinished == 1)
		{
			fileUploaderPacketCounter = 0;
			fileUploaderMessagefinished = 0;

			//info message and close file
			FileClose(iFileHandleFileUploadFunction);
			iFileHandleFileUploadFunction = NULL;
			//isPacketFileUploader = 0;

			//memset(m_fileUploadName, 0, 120);
			//FileCounter = 0;
		}
	}

	unsigned char dumB[2] = {0, 0}; //REPLY_ERROR_CODE_OK
	write(client_sock , dumB , 2);

	return 1;
}

void SendShellCommandOutputViaSerialPort(char *m_command)
{
	char Command[1024] = {0};
	sprintf(Command, "%s", m_command);

	if(FileCounter >= 2)
	{
		memset(Command, 0, 1024);
		sprintf(Command, "make -C %s", (char *)MAIN_APP_FILE_PATH);
	}

	if((isItCabled == 1))
	{
	  printf("PIPEOPEN OPENED\n");
	  FILE *fp;
	  char path[1035] = {0};
	  char output[1000] = {0};

	  /* Open the command for reading. */
	  fp = popen(Command, "w");
	  if (fp == NULL) {
		printf("Failed to run command\n" );
		exit(1);
	  }

	  /* Read the output a line at a time - output it. */
	  while (fgets(path, sizeof(path)-1, fp) != NULL) {
		//printf("DODO PIPE:  %s", path);
		  strcat(output, path);
	  }

	  GPIO_TYPE ret = GPIO_Read_Pin(48); //if serial connected
	  if(ret == HIGH)
	  {
		  Uart_Send(PORT_USBDEV, output, strlen(output));
	      pauseSec(1);
	  }else
		  printf("PIPE OUTPUT: %s", output);

	  /* close */
	  pclose(fp);
	  Uart_Close(PORT_USBDEV);
	  fflush(stdout);
	  isItCabled = 0;
	  FileCounter = 0;
	  memset(m_fileUploadName, 0, 120);
	}
	else
		system(Command);

	printf("COMMAND: %s\n", Command);
}
