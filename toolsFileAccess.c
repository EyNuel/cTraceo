/********************************************************************************
 *  toolsFileAccess.c																		*
 * 	Collection of file access utility functions.								*
 *																				*
 *	Written by:		Emanuel Ey													*
 *					emanuel.ey@gmail.com										*
 *					Signal Processing Laboratory								*
 *					Universidade do Algarve										*
 *																				*
 *******************************************************************************/

#pragma once
#include <string.h>


///Prototypes:

FILE*			openFile(const char* , const char[4]);
double 			readDouble(FILE*);
int32_t			readInt(FILE*);
char*			readStringN(FILE*, uint32_t);
void			skipLine(FILE*);



///Functions:

FILE* 		openFile(const char *filename, const char mode[4]) {
	/* 
		Opens a file and returns a filepointer in case of success, exits with error code otherwise.
		Input values:
			filename	A string containing a full or relative path to the file.
			mode		A string containg the file acces mode.
		
		Return Value:
			A FILE pointer.
	*/
	
	FILE *temp;
	if (VERBOSE)
		printf("Accessing file: %s... ", filename);
		
	temp=fopen(filename, mode);
	if(temp==NULL) {
		fatal("Error while opening file.\n");
		exit(EXIT_FAILURE);		//this is redundant but avoids "control may reach end of non-void function" warning
	} else {
		if (VERBOSE)
			printf("Ok.\n");
		return temp;
	}
}

double 		readDouble(FILE* infile){
	/************************************************
	 *	Reads a double from a file and returns it	*
	 ***********************************************/
	 
	char*	junkString = mallocChar((uintptr_t)(MAX_LINE_LEN + 1));;
	double 	tempDouble;
	int32_t	junkInt;
	
	junkInt = fscanf(infile, "%s\n", junkString);
	tempDouble = atof(junkString);
	free(junkString);
	
	return(tempDouble);
}

int32_t		readInt(FILE* infile){
	/************************************************
	 *	Reads a int from a file and returns it		*
	 ***********************************************/
	 
	char*		junkString = mallocChar((uintptr_t)(MAX_LINE_LEN + 1));
	int32_t 	tempInt;
	int32_t		junkInt;
	
	junkInt = fscanf(infile, "%s\n", junkString);
	tempInt = (int32_t)atol(junkString);
	free(junkString);
	
	return(tempInt);
}

char*		readStringN(FILE* infile, uint32_t length){
	/********************************************************************
	 *	Reads a <lenght> chars from a filestream.						*
	 *******************************************************************/
	char*		outputString = mallocChar((uintptr_t)(MAX_LINE_LEN + 1));;
	char*		junkChar;

	junkChar = fgets(outputString, (int32_t)length, infile);
	return(outputString);
}

void		skipLine(FILE* infile){
	/************************************************
	 *	Reads a int from a file and returns it		*
	 ***********************************************/
	char*		junkString = mallocChar((uintptr_t)(MAX_LINE_LEN + 1));
	char*		junkChar;
	
	junkChar = fgets(junkString, MAX_LINE_LEN+1, infile);

	free(junkString);
}

