/*
 * FileSystem.h
 *
 *  Created on: Oct 25, 2016
 *      Author: dogus
 */

#ifndef FILESYSTEM_H_
#define FILESYSTEM_H_

#include <stdio.h>
#include <stddef.h>
#include <time.h>
#include <termios.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <sys/stat.h>

#define RETURN_OK   1
#define RETURN_NOK -1


typedef enum
{
	R_ONLY,//r
	RW, //r+
	RW_CREATE, //w+
	A_ONLY, //a
	RA //a+
}FILE_FLAGS;

FILE* FileOpen(const char *fileName, FILE_FLAGS flags);
int FileRead(FILE *hdl, char *buf, int len);
int FileWrite(FILE *hdl, const char *buf, int len);
int FileSeek(FILE *hdl, long off, int orig); //orig = SEEK_CUR, SEEK_SET, SEEK_END
void FileClose(FILE *hdl);
int FileSize(const char *fileName);
int FileRemove(const char *fileName);
int FileExist(const char *file);
int FileRename(const char *oldname, const char *newname);
int FileCopy (const char *source, const char *target);

#endif /* FILESYSTEM_H_ */
