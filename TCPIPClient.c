#include "TCPIPClient.h"

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h> //for the hostent stucture
#include <stddef.h>
#include <stdio.h>
#include <unistd.h>


struct sockaddr_in serverAddress;
struct hostent *server;
int socketfd = -1;

int TCP_Client_Connect(char *ip, int port)
{
	server = gethostbyname(ip);
	if (server == NULL) {
		printf("Socket Client: error - unable to resolve host name.\n");
		return -1;
	}

	socketfd = socket(AF_INET, SOCK_STREAM, 0);
	if (socketfd < 0){
		printf("Socket Client: error opening TCP IP-based socket.\n");
		return -1;
	}

	// clear the data in the serverAddress sockaddr_in struct
	bzero((char *) &serverAddress, sizeof(serverAddress));
	int portNumber = port;
	serverAddress.sin_family = AF_INET; //set the address family to be IP
	serverAddress.sin_port = htons(portNumber); //set port number to 80
	bcopy((char *)server->h_addr,(char *)&serverAddress.sin_addr.s_addr,
	server->h_length); //set address to resolved hostname address
	// try to connect to the server
	if (connect(socketfd, (struct sockaddr *) &serverAddress,
		sizeof(serverAddress)) < 0){
		printf("Socket Client: error connecting to the server.\n");
		return -1;
	}

	return 0;
}


int TCP_Client_Read(unsigned char *buffer, int len)
{
	if (read(socketfd, buffer, len) < 0){
		printf("Socket Client: error reading from socket");
		return -1;
	}
	return len;
}


int TCP_Client_Write(unsigned char *buffer, int len)
{
	if (write(socketfd, buffer, len) < 0){
		printf("Socket Client: error writing to socket");
		return -1;
	}
	return 0;
}


void TCP_Client_Close(int socket)
{
	close(socketfd);
	socketfd = -1;
}



