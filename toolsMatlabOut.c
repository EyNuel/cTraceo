// collection of functions to manually write matlab .mat files.
// this can be used instead of using the matlab API.

#pragma once
#include "toolsFileAccess.c"
#include <string.h>
#include <stdio.h>

/*
 * Definitions for matlab data types
 */
#define MATLAB_DATATYPE__miINT8		1
#define MATLAB_DATATYPE__miUINT8	2
#define MATLAB_DATATYPE__miINT16	3
#define MATLAB_DATATYPE__miUINT16	4
#define MATLAB_DATATYPE__miINT32	5
#define MATLAB_DATATYPE__miUINT32	6
#define MATLAB_DATATYPE__miSINGLE	7
#define MATLAB_DATATYPE__miDOUBLE	9
#define MATLAB_DATATYPE__miINT64	12
#define MATLAB_DATATYPE__miUINT64	13
#define MATLAB_DATATYPE__miMATRIX	14

/*
 * Definitions for matlab Array types (classes)
 */
#define MATLAB_ARRAYTYPE__mxCELL_CLASS		1
#define MATLAB_ARRAYTYPE__mxSTRUCT_CLASS	2
#define MATLAB_ARRAYTYPE__mxOBJECT_CLASS	3
#define MATLAB_ARRAYTYPE__mxCHAR_CLASS		4
#define MATLAB_ARRAYTYPE__mxSPARSE_CLASS	5
#define MATLAB_ARRAYTYPE__mxDOUBLE_CLASS	6
#define MATLAB_ARRAYTYPE__mxSINGLE_CLASS	7
#define MATLAB_ARRAYTYPE__mxINT8_CLASS		8
#define MATLAB_ARRAYTYPE__mxUINT8_CLASS		9
#define MATLAB_ARRAYTYPE__mxINT16_CLASS		10
#define MATLAB_ARRAYTYPE__mxUINT16_CLASS	11
#define MATLAB_ARRAYTYPE__mxINT32_CLASS		12
#define MATLAB_ARRAYTYPE__mxUINT32_CLASS	13

/*
 * Definitions for matlab numeric data types_
 */
 #define MATLAB_NUMERICTYPE__COMPLEX		1
 #define MATLAB_NUMERICTYPE__GLOBAL			2
 #define MATLAB_NUMERICTYPE__LOGICAL		3



FILE*	writeMatfileHeader(FILE* outfile, const char descriptiveText[124]);
///FILE*	writeMatfileData(FILE* outfile);
FILE*	writeArray(FILE* outfile);
void	newMatfile(const char* filename);

/*
 * NOTE: size_t fwrite (const void *array, size_t size, size_t count, FILE *stream);
 */


FILE* writeMatfileHeader(FILE* outfile, const char descriptiveText[124]){
	/*
	 * write 124 bytes of description
	 */
	fwrite(descriptiveText, 1, 124, outfile);
	

	/*
	 * write 2 Bytes "Version"
	 */
	uint16_t	tempUInt16 = 0x0100;
	fwrite(&tempUInt16, sizeof(uint16_t), 1, outfile);
	printf(":%u:\n", tempUInt16);
	

	/*
	 * write 2 bytes "Endianness indicator": the characters "M"
	 * and "I" concatenated and written as 16 bits.
	 */
	tempUInt16 =  'M';
	tempUInt16 <<= 8;
	tempUInt16 |= 'I';
	fwrite(&tempUInt16, sizeof(uint16_t), 1, outfile);
	
	return outfile;
}

#if 0
//void writeMatfileData(FILE* outfile, uint8_t dataType, void){
FILE* writeMatfileData(FILE* outfile){
	
	uintptr_t	i;
	uint32_t	tempUInt32;
	float		data[6] = {1,2,3,4,5,6};
	
	// write data type:
	tempUInt32 = MATLAB_DATATYPE__miUINT32;
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

void writeDataElement(FILE* outfile, uint32_t dataType, void* data, size_t dataItemSize, uint32_t nDataItems){
	uint32_t	nBytes;
	uint32_t	paddingBytes = 0;
	uintptr_t	i;
	char		emptyChar = 0;
	
	//write the datatype identifier
	fwrite(&dataType, sizeof(uint32_t), 1, outfile);
	
	//write the number of data bytes to the data element's tag:
	nBytes	= dataItemSize * nDataItems;
	fwrite(&nBytes, sizeof(uint32_t), 1, outfile);
	
	
	// write the data
	fwrite(data, dataItemSize, nDataItems, outfile);
	
	/*
	 * padding may be required to ensure 64b boundaries between
	 * data elements.
	 */
	if (nBytes % 8 > 0){
		paddingBytes= 8 - nBytes % 8;	//This could probably be neatly rewritten with the ternary operator
	}
	printf("numBytes: %u, paddingBytes: %u\n", nBytes, paddingBytes);
	for (i=0; i<paddingBytes; i++){
		fwrite(&emptyChar, sizeof(char), 1, outfile);
	}
}

uint32_t	dataElementSize(size_t dataItemSize, uint32_t nDataItems){
	uint32_t	nBytes;
	uint32_t	paddingBytes = 0;
	
	nBytes	= dataItemSize * nDataItems;
	if (nBytes % 8 > 0){
		paddingBytes= 8 - nBytes % 8;	//This could probably be neatly rewritten with the ternary operator
	}
	
	return(8 +nBytes +paddingBytes);
}

FILE* writeArray(FILE* outfile){
	/*
	 * writes an array to an open matfile.
	 */
	
	uint32_t	numericType	= MATLAB_NUMERICTYPE__COMPLEX;	//TODO: make this an input parameter
	uint8_t		mxClass		= MATLAB_ARRAYTYPE__mxDOUBLE_CLASS;
	uint8_t		flags;
	uint16_t	tempUInt16	= 0x00;
	uint32_t	tempUInt32	= 0x00;;
	int32_t		dims[2]		= {1,4};
	char*		arrayName	= "someReallyLongVariableName";
	double 		dataReal[]	= {1.1, 2.0, 3.0, 4.0};
	double 		dataImag[]	= {1.1, 0.0, 0.0, 0.0};
	uint32_t	nArrayElements = dims[0] * dims[1];
	uint32_t	nArrayBytes	= 128;							//TODO: make this an input parameter
	
	/*
	 * write miMATRIX tag and total number of bytes in the matrix
	 */
	tempUInt32	= MATLAB_DATATYPE__miMATRIX;
	fwrite(&tempUInt32, sizeof(uint32_t), 1, outfile);
	
	nArrayBytes = 4*8;
	nArrayBytes += dataElementSize(sizeof(char), strlen(arrayName));
	nArrayBytes += dataElementSize(sizeof(double), 4);		//TODO: adapt to other data types
	if (numericType == MATLAB_NUMERICTYPE__COMPLEX){
		nArrayBytes += dataElementSize(sizeof(double), 4);		//TODO: adapt to other data types
	}
	
	fwrite(&nArrayBytes, sizeof(uint32_t), 1, outfile);
	
	
	/*
	 * generate array flags:
	 * "undefined"(2B), "flags"(1B), "mxCLASS"(1B), "undefined"(4B)
	 * NOTE: although the 'array flags' block is called a data element in
	 * 		 the matfile reference manual, it does not follow the rules
	 * 		 for actual 'data elements'. the datatype is defined as
	 * 		 miUINT32, but is written as chars. Because of this
	 * 		 this block is written manuall (without using the
	 * 		 writeDataElement() function.
	 */
	 
	//write datatype and number of bytes to element's tag
	tempUInt32 = MATLAB_DATATYPE__miUINT32;
	fwrite(&tempUInt32, sizeof(uint32_t), 1, outfile);
	tempUInt32 = 8;
	fwrite(&tempUInt32,	sizeof(uint32_t), 1, outfile);
	
	//write flags and mxClass to the element's data block
	switch (numericType){
		case MATLAB_NUMERICTYPE__COMPLEX:
			flags = 0x08;
			break;
		case MATLAB_NUMERICTYPE__GLOBAL:
			flags = 0x04;
			break;
		case MATLAB_NUMERICTYPE__LOGICAL:
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
	
	
	/*
	 * write the dimensions element
	 */
	writeDataElement(outfile, MATLAB_DATATYPE__miINT32, dims, sizeof(int32_t), 2);
	
	
	/*
	 * write the array name element
	 */
	writeDataElement(outfile, MATLAB_DATATYPE__miINT8, arrayName, sizeof(char), strlen(arrayName));
	
	
	/*
	 * write data element containing real part of array 
	 */
	writeDataElement(outfile, MATLAB_DATATYPE__miDOUBLE, dataReal, sizeof(double), nArrayElements);
	
	
	/*
	 * write data element containing imaginary part of array (if complex)
	 */
	if(numericType == MATLAB_NUMERICTYPE__COMPLEX){
		writeDataElement(outfile, MATLAB_DATATYPE__miDOUBLE, dataImag, sizeof(double), nArrayElements);
	}
	
	return outfile;
}


void newMatfile(const char* filename){
	FILE*	outfile = openFile(filename, "wb");		//open file in mode "binary+write"
	
	//outfile =
	writeMatfileHeader(outfile, "Test testing the test.");
	writeArray(outfile);
	
	
	fclose(outfile);
}


