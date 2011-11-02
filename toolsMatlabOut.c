/*
 * collection of functions to manually write matlab .mat files.
 * This can be used instead of using the matlab API.
 * NOTE: the text wriiten to the matlab header is obtained from
 * 		 MATLAB_HEADER_TEXT, defined in toolsMatlabOut.h
 */


#pragma once
//#include "toolsFileAccess.c"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include "toolsMatlabOut.h"
#include "toolsMisc.c"
#include "toolsMemory.c"


/*
 * Function prototypes;
 */


/*
 * functions:
 */


#if 0
//this was used for initial testing
//void writeMatfileData(FILE* outfile, uint8_t dataType, void){
FILE* writeMatfileData(FILE* outfile){
	
	uintptr_t	i;
	uint32_t	tempUInt32;
	float		data[6] = {1,2,3,4,5,6};
	
	// write data type:
	tempUInt32 = miUINT32;
	fwrite(&tempUInt32, sizeof(uint32_t), 1, outfile);
	
	//write number of bytes:
	tempUInt32 = 24;
	fwrite(&tempUInt32, sizeof(uint32_t), 1, outfile);
	
	for(i=0; i<6; i++){
		fwrite(&data[i], sizeof(float), 1, outfile);
		//printf(":%f:\n", data[i]);
	}
	//fatal("The end");
	return outfile;
}
#endif


/*
 * Used during testing
void newMatfile(const char* filename){
	FILE*	outfile = openFile(filename, "wb");		//open file in mode "binary+write"
	
	//outfile =
	writeMatfileHeader(outfile, "Test testing the test.");
	writeArray(outfile);
	
	
	fclose(outfile);
}
*/




