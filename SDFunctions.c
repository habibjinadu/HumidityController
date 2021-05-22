/*
 * File:   SDFunctions.c
 * Author: adeso
 *
 * Created on May 17, 2021, 8:08 PM
 */


#include "xc.h"
#include "string.h" // include string.h to use string handling
#include "diskio.h"
#include "ff.h"
#include "ffconf.h"

void writeToEndOfFile(FIL (*file), char* word)
{
    UINT bytesWritten; // create a variable to store the number of bytes written
    UINT length = strlen(word); // get the length of the char array
    f_lseek(file, f_size(file)); // go to the end of the file
    f_write(file, word, length, &bytesWritten); // write to end of the file
}