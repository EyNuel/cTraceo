/********************************************************
*	"errorcodes.c"										*
*	Provides a helper function to print error messages.	*
*	See "errocodes.h" for the actual error codes.		*
********************************************************/
#include <stdio.h>
#include "errorcodes.h"
void printMsg(int);

void printMsg(int errorCode){
	/*
		Prints the message corresponding to an error code.
	*/
	switch (errorCode){
		case ERR__FILE_OPEN:
			printf("File access error.\n");
			break;
		case ERR__MEMORY_ALOCATION:
			printf("Memory allocation error.\n");
		default:
			printf("There was an error.\n");
			break;
	}
}
