/*
 * FileSystem.c
 *
 *  Created on: Oct 25, 2016
 *      Author: dogus
 */
#include"FileSystem.h"

#include <stdio.h>
#include <stddef.h>
#include <time.h>
#include <termios.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <sys/stat.h>
#include <stdlib.h>

/************************  FILE SYSTEM RELATED  ****************************************************/

//------------------------------------------------------------------------------------------------
/**
 *   @details - it selects a file that is suitable for "flags" parameter
 *   @param *id : file name that will be worked on
 *   @param flags : openning file format
 *   @return <=0: failed - >0:succeed
 *   @note it returns number of opened files before
 */
//------------------------------------------------------------------------------------------------
FILE *FileOpen(const char *fileName, FILE_FLAGS flags)
{
	char flg[5] = {0};

	if(flags==R_ONLY)
		strcpy(flg, "r");
	else if(flags==RW)
		strcpy(flg, "r+");
	else if(flags==RW_CREATE)
		strcpy(flg, "w+");
	else if(flags==A_ONLY)
		strcpy(flg, "a");
	else if(flags==RA)
		strcpy(flg, "a+");
	else
		return NULL;

	return fopen(fileName, flg);
}


//------------------------------------------------------------------------------------------------
/**
 *   @details - it reads data from the file that is referred by "hdl"
 *   @param hdl : index of file (handle)
 *   @param *buf : buffer that contains read data
 *   @param len : length of data that will be read
 *   @return <0: failed - >=0:succeed
 *   @note it returns number of character that was read
 */
//------------------------------------------------------------------------------------------------
int FileRead(FILE *hdl, char *buf, int len)
{
	int ret = fread(buf, 1, len, hdl);
	if(ret == len)
		return len;
	return RETURN_NOK;
}


//------------------------------------------------------------------------------------------------
/**
 *   @details - it writes data from the file that is referred by "hdl"
 *   @param hdl : index of file (handle)
 *   @param *buf : buffer that contains written data
 *   @param len : length of data that will be written
 *   @return <0: failed - >=0:succeed
 *   @note it returns number of character that will be written
 */
//------------------------------------------------------------------------------------------------
int FileWrite(FILE *hdl, const char *buf, int len)
{
	int ret = fwrite(buf, 1, len, hdl);
	if(ret == len)
		return len;
	return RETURN_NOK;
}


//------------------------------------------------------------------------------------------------
/**
 *   @details - it replaces cursor of the file
 *   @param hdl  : index of file (handle)
 *   @param off  : movement length of the file cursor
 *   @param orig : method to move the cursor (SEEK_SET(0)/SEEK_CUR(1)/SEEK_END(2))
 *   @return <0: failed - >=0:succeed
 *   @note it returns the number of "off" parameter
 */
//------------------------------------------------------------------------------------------------
int FileSeek(FILE *hdl, long off, int orig)
{
	return fseek(hdl, off, orig);
}


//------------------------------------------------------------------------------------------------
/**
 *   @details - it closes the file that is referred by "hdl"
 *   @param hdl : index of file (handle)
 *   @return <0: failed - >=0:succeed
 *   @note it returns "hdl" parameters(if file open function was succeed before, hdl must be greater than or equals to zero.
 */
//------------------------------------------------------------------------------------------------
void FileClose(FILE *hdl)
{
	fclose(hdl);
}




//------------------------------------------------------------------------------------------------
/**
 *   @details - it removes the file that is represented by "hdl"
 *   @param *file : file name
 *   @return <0: failed - >=0:succeed
 *   @note it returns -1 in failed, 0 in succeed
 */
//------------------------------------------------------------------------------------------------
int FileRemove(const char *fileName)
{
	return unlink(fileName);
}



//------------------------------------------------------------------------------------------------
/**
 *   @details - it controls the file that is represented by "*file" is in RAM or flash or not?
 *   @param *file : file name
 *   @return @return ==0: succeed - ==-1:failed
 *   @note EP_FileExist(const char *file) return 0 in succeed, 1 in failed
 */
//------------------------------------------------------------------------------------------------
int FileExist(const char *fileName)
{
	FILE *fd = FileOpen(fileName, R_ONLY);

    if (fd == NULL)
    	return RETURN_NOK;

    return RETURN_OK;
}



//------------------------------------------------------------------------------------------------
/**
 *   @details - it gives the number of character in the file that is represented by "*file"
 *   @param *file : name of the file
 *   @return <0: failed - >=0:succeed
 *   @note -
 */
int FileSize(const char *fileName)
{
	struct stat st;
	stat(fileName, &st);
	return st.st_size;
}




//------------------------------------------------------------------------------------------------
/**
 *   @details - it replaces "*oldname" file name with "*newname" string
 *   @param *oldname : old file name
 *   @param *newname : new file name
 *   @return <0: failed - >=0:succeed
 *   @note EP_FileRename(oldname, newname) return 0 in succeed, 1 in failed
 */
//------------------------------------------------------------------------------------------------
int FileRename(const char *oldname, const char *newname)
{
	return rename(oldname, newname);
}

//------------------------------------------------------------------------------------------------
/**
 *   @details - it copies at the path "source" to the path "target"
 *   @param *source : path of source
 *   @param *target : path of target
 *   @return <0: failed - >=0:succeed
 *   @note -
 */
//------------------------------------------------------------------------------------------------
int   FileCopy (const char *source, const char *target)
{
	if(FileExist(source) == RETURN_NOK)//there is no source then quit
	{
		return RETURN_NOK;
	}

	FILE  *sor = NULL;
	sor = FileOpen(source, RW);//open source for reading
	if(sor == NULL)
		return RETURN_NOK;

	int size = -100;
	size = FileSize(source);//find size of the source
	if(size<0)
		return RETURN_NOK;

	char *sourceReadBuf = (char*)malloc(size*sizeof(char));//this will be hold the rest of main string
	if(sourceReadBuf==NULL)//if there is not enough memory then quit
		return RETURN_NOK;

	FileRead(sor, sourceReadBuf, size);//read the file in "size" characters

	FileClose(sor);


	if(FileExist(target)==RETURN_OK)//there is target then remove it
	{
		if(FileRemove(target) == RETURN_NOK)//if cannot remove it, quit
			return RETURN_NOK;
	}

	//create a target
	FILE *tar = NULL;
	tar = FileOpen(target, RW_CREATE);
	if(tar == NULL)//cannot open then quit
		return RETURN_NOK;

	if(FileWrite(tar, sourceReadBuf, size) == RETURN_NOK)//copy data to new one
		return RETURN_NOK;

	FileClose(tar);

	free(sourceReadBuf);

	return RETURN_OK;//all is well
}




/************************  END OF FILE SYSTEM RELATED  ***************************************************/
