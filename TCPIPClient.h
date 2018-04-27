#ifndef TCPIPCLIENT_H_
#define TCPIPCLIENT_H_

int TCP_Client_Connect(char *ip, int port);
int TCP_Client_Read(unsigned char *buffer, int len);
int TCP_Client_Write(unsigned char *buffer, int len);
void TCP_Client_Close(int socket);

#endif /* TCPIPCLIENT_H_ */
