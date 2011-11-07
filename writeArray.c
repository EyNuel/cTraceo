#pragma once
#include <stdio.h>
#include <stdint.h>
#include "matlabOut.h"
#include "writeDataElement.c"
#include "dataElementSize.c"
#include "mxGetData.c"
#include "mxGetImagData.c"

uintptr_t	writeArray(MATFile* outfile, const char* arrayName, mxArray* inArray);

uintptr_t	writeArray(MATFile* outfile, const char* arrayName, mxArray* inArray){
	/*
	 * writes an array to an open matfile.
	 */
	
	uint8_t		mxClass		= inArray->mxCLASS;
	uint8_t		flags;
	//uint16_t	tempUInt16	= 0x00;
	uint32_t	tempUInt32	= 0x00;;
	uint32_t	nArrayElements = inArray->dims[0] * inArray->dims[1];
	uint32_t	nArrayBytes;
	
	/* *********************************************************
	 * write miMATRIX tag and total number of bytes in the matrix
	 * NOTE: total number of bytes does not include the first 8B
	 * 		 of the file (4B[miMATRIX] +4B[nBytes]).
	 */
	tempUInt32	= miMATRIX;
	fwrite(&tempUInt32, sizeof(uint32_t), 1, outfile);
	
	
	/* get the mxArray's previously calculated size and add the
	 * number of bytes required for arrayName. Note that if this
	 * mxArray is member of a structure, it's name will not be
	 * written to file, so required size is different.
	 */
	nArrayBytes = inArray->nBytes;
	if (inArray->isChild == false){
		nArrayBytes += dataElementSize(sizeof(char), strlen(arrayName));
	}
	
	fwrite(&nArrayBytes, sizeof(uint32_t), 1, outfile);
	
	
	/* *********************************************************
	 * generate array flags:
	 * "undefined"(2B), "flags"(1B), "mxCLASS"(1B), "undefined"(4B)
	 * NOTE: although the 'array flags' block is called a data element in
	 * 		 the matfile reference manual, it does not follow the rules
	 * 		 for actual 'data elements'. the datatype is defined as
	 * 		 miUINT32, but is written as chars. Because of this
	 * 		 this block is written manually (without using the
	 * 		 writeDataElement() function.
	 */
	 
	//write datatype and number of bytes to element's tag
	tempUInt32 = miUINT32;
	fwrite(&tempUInt32, sizeof(uint32_t), 1, outfile);
	tempUInt32 = 8;
	fwrite(&tempUInt32,	sizeof(uint32_t), 1, outfile);
	
	//write flags and mxClass to the element's data block
	switch (inArray->numericType){
		case mxCOMPLEX:
			flags = 0x08;
			break;
		case mxGLOBAL:
			flags = 0x04;
			break;
		case mxLOGICAL:
			flags = 0x02;
			break;
		default:
			flags = 0x00;
			break;
	}
	tempUInt32 =	flags;
	tempUInt32 <<=	8;	//left shift the flags by one byte
	tempUInt32 |=	mxClass;
	fwrite(&tempUInt32, sizeof(uint32_t), 1, outfile);
	
	//write 4B of undefined data to element's data block
	tempUInt32 = 0x00;
	fwrite(&tempUInt32, sizeof(uint32_t), 1, outfile);
	
	
	/* *********************************************************
	 * write the dimensions element
	 */
	writeDataElement(outfile, miINT32, inArray->dims, sizeof(int32_t), 2);
	
	
	/*
	 * write the array name element
	 * NOTE: for mxArrays which are a child of another one
	 * 		 the name is not actually written as it is already
	 * 		 defined in the parent structure's filednames.
	 */
	if (inArray->isChild == true){
		writeDataElement(outfile, miINT8, (void*)arrayName, sizeof(char), 0);
	}else{
		writeDataElement(outfile, miINT8, (void*)arrayName, sizeof(char), strlen(arrayName));
	}
	
	
	/* *********************************************************
	 * write data element containing real part of array 
	 */
	double *tempDouble = NULL;
	tempDouble = (double*)mxGetData(inArray);
	printf("data[0]: %lf\n", tempDouble[0]);
	writeDataElement(outfile, inArray->mxCLASS, mxGetData(inArray), inArray->dataElementSize, nArrayElements);
	
	 
	/* *********************************************************
	 * write data element containing imaginary part of array (if complex)
	 */
	if(inArray->numericType == mxCOMPLEX){
		writeDataElement(outfile, miDOUBLE, mxGetImagData(inArray), sizeof(double), nArrayElements);
	}
	
	return 0;
}
