/*
 * AES.h
 *
 *  Created on: Oct 25, 2016
 *      Author: dogus
 */

#ifndef AES_H_
#define AES_H_

		typedef enum
		{
			AES_128,      /*!< AES-128 */
			AES_192,      /*!< AES-192 */
			AES_256       /*!< AES-256 */
		}AesType;



      void Aes_Init (AesType type/*, const uint8_t *key*/);

      void EncryptBlock (unsigned char data[16]);
      void DecryptBlock (unsigned char data[16]);

      void SubBytes();
      void InvSubBytes();
      void ShiftRows();
      void InvShiftRows();
      unsigned char GaloisMultiplication (unsigned char a, unsigned char b);
      void MixColumn (unsigned char column[4]);
      void MixColumns();
      void InvMixColumn (unsigned char column[4]);
      void InvMixColumns();
      void AddRoundKey (int round);
      void KeyExpansionSubWord (unsigned char data[4]);
      void KeyExpansionRotWord (unsigned char data[4]);
      void KeyExpansion();

#endif /* AES_H_ */
