# BeagleBoneBlackPheripherals
This project do nothing actually. This just hold handmade header to use BBB effectively.

ADC CONVERTER
------------------------------
 
•	GND_ADC should be connected to mutual ground
•	AIN0-6 can be used to ADC Input
•	1.8 volt is max connected voltage
•	Code Example;

    int main(void)
    {
      ADC_Init();
      unsigned int value = 0;

      while(1)
      {
        ADC_Read_Pin(0, &value);
        printf(“Value: %d”, (value & 0x0FFF);
      }
    }

AES ENCRYPTION
-----------------------------

  //change here as you wish
  const unsigned char AES_KEY[16] = { xx, xx, xx, xx, xx, xx, xx, xx, xx, xx, xx, xx, xx, xx, xx, xx };

    int main()
    {
      Aes_Init(AES_128);
      char *buffer = (char *)calloc(16, sizeof(char));
      sprint(buffer, “selam”); EncryptBlock(buffer); DecryptBlock(buffer);
      printf(“output: %s”, buffer);
    }

FILE SYSTEM
-------------------------------

    {
      if (FileExist((char *)MAIN_C_FILE_NAME) != -1)
        FileRemove((char *)MAIN_C_FILE_NAME);

      FILE *fileEncrypted = FileOpen((char *)MAIN_C_FILE_NAME, RA); 
      if(fileEncrypted == NULL)
        printf("Cannot Created the new decrypted file\n");
    }

GPIO
---------------------------

    {
      GPIO_Init(48, INPUT);
      GPIO_TYPE ret = GPIO_Read_Pin(48);
      if(ret == HIGH)
        //sth
      else
        //sth else
    }

SERIAL PORT
-------------------------------

  {
    int PORT_USBDEV = -1;

    PORT_USBDEV = Uart_Init(4, 9600);
    if(PORT_USBDEV<0)
      printf("serial port open fail\n"); 

    unsigned int messageLen = 0;
    Uart_Flush(PORT_USBDEV);
    receiveReturn = Uart_Receive(PORT_USBDEV, &message[0], 2, generalTimeout);
    if(receiveReturn == 2) //"OK" came for example
      //data come
  }

TCP/IP CLIENT CONNECTION
----------------------------------------------

    if(TCP_Client_Connect("192.168.1.80", 2503) != -1)
    {
      if(TCP_Client_Write(dummy_buffer, 2) != -1)
      {
        if(TCP_Client_Read(buffer, 2) == 2) //"OK" came for example
        {
          //do sth
        }
      }
    }

USER LEDS
-------------------------

      LedON(1);
      LedON(2); 
      LedOFF(3);
 
