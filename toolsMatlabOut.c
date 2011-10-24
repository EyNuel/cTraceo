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

FILE*		writeMatfileHeader(		FILE* outfile, const char descriptiveText[124]);
void		writeDataElement(		FILE* outfile, uint32_t dataType, void* data, size_t dataItemSize, uint32_t nDataItems);
uint32_t	dataElementSize(		size_t dataItemSize, uint32_t nDataItems);
uintptr_t	writeArray(				MATFile* outfile, const char* arrayName, mxArray* inArray);
//void		newMatfile(				const char* filename);
MATFile*	matOpen(				const char* filename, const char* openMode);
mxArray*	mxCreateDoubleMatrix(	uintptr_t nRows, uintptr_t nCols, uintptr_t numericType);
double*		mxGetPr(				mxArray* array);
double*		mxGetData(				mxArray* array);
double*		mxGetPi(				mxArray* array);
double*		mxGetImagData(			mxArray* array);
mxArray*	mxCreateString(			const char *);
void		mxSetFieldByNumber(		mxArray* mxRayStruct, uint32_t index, uint32_t iField, mxArray* inArray);
void		mxDestroyArray(			mxArray* array);
uintptr_t	matPutVariable(			MATFile* outfile, const char* arrayName, mxArray* inArray);
void		matClose(				MATFile* file);

/*
 * functions:
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
//this was used for initial testing
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
	/*
	 * TODO: documentation
	 * TODO: verify successfull write
	 */
	uint8_t		tempUInt8;
	uint32_t	nBytes;
	uint32_t	paddingBytes = 0;
	uintptr_t	i;
	char		emptyChar = 0;
	
	//write the datatype identifier
	fwrite(&dataType, sizeof(uint32_t), 1, outfile);
	
	//write the number of data bytes to the data element's tag:
	nBytes	= dataItemSize * nDataItems;
	if (dataType == MATLAB_ARRAYTYPE__mxCHAR_CLASS){
		//mxCHAR_CLASS is strange: altough the data is 8bit, it is written as 16bit uints,
		//making this just abit trickier
		nBytes *= 2;
	}
	
	fwrite(&nBytes, sizeof(uint32_t), 1, outfile);
	
	
	// write the data
	if (dataType == MATLAB_ARRAYTYPE__mxCHAR_CLASS){
		//Again, because mxCharClass is strange, we have to write one char
		//at a time, each followed by a null byte
		tempUInt8 = 0x00;
		for (int i=0; i<strlen(data); i++){
			fwrite(&data[i], sizeof(char), 1, outfile);
			fwrite(&tempUInt8, sizeof(char), 1, outfile);
		}
		
	}else{
		fwrite(data, dataItemSize, nDataItems, outfile);
	}
	
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
	/*
	 * calculates the size of an unwritten data element.
	 * This is needed because the header for an array needs to
	 * contain the number of bytes that compose the array.
	 */
	uint32_t	nBytes;
	uint32_t	paddingBytes = 0;
	
	nBytes	= dataItemSize * nDataItems;
	if (nBytes % 8 > 0){
		paddingBytes= 8 - nBytes % 8;	//This could probably be neatly rewritten with the ternary operator
	}
	
	return(8 +nBytes +paddingBytes);
}


//FILE* writeArray(FILE* outfile){
uintptr_t	writeArray(MATFile* outfile, const char* arrayName, mxArray* inArray){
	/*
	 * writes an array to an open matfile.
	 */
	
	//uint8_t		mxClass		= MATLAB_ARRAYTYPE__mxDOUBLE_CLASS;	//TODO get this from incomming mxArray (need to adapt mxArray first)
	uint8_t		mxClass		= inArray->mxCLASS;
	uint8_t		flags;
	uint16_t	tempUInt16	= 0x00;
	uint32_t	tempUInt32	= 0x00;;
	uint32_t	nArrayElements = inArray->dims[0] * inArray->dims[1];
	uint32_t	nArrayBytes;
	
	/*
	 * write miMATRIX tag and total number of bytes in the matrix
	 */
	tempUInt32	= MATLAB_DATATYPE__miMATRIX;
	fwrite(&tempUInt32, sizeof(uint32_t), 1, outfile);
	
	nArrayBytes = 4*8;
	nArrayBytes += dataElementSize(sizeof(char), strlen(arrayName));
	if (inArray->mxCLASS == MATLAB_ARRAYTYPE__mxCHAR_CLASS){
		//NOTE: mxCHAR_CLASS is strange: although datatype is 'char' 2B are written per character
		nArrayBytes += dataElementSize(2*sizeof(char), inArray->dims[0]*inArray->dims[1]);		//TODO: adapt to other data types
	}else{
		nArrayBytes += dataElementSize(inArray->dataElementSize, inArray->dims[0]*inArray->dims[1]);		//TODO: adapt to other data types
	}
	if (inArray->numericType == MATLAB_NUMERICTYPE__COMPLEX){
		nArrayBytes += dataElementSize(inArray->dataElementSize, inArray->dims[0]*inArray->dims[1]);		//TODO: adapt to other data types
	}
	
	fwrite(&nArrayBytes, sizeof(uint32_t), 1, outfile);
	
	
	/*
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
	tempUInt32 = MATLAB_DATATYPE__miUINT32;
	fwrite(&tempUInt32, sizeof(uint32_t), 1, outfile);
	tempUInt32 = 8;
	fwrite(&tempUInt32,	sizeof(uint32_t), 1, outfile);
	
	//write flags and mxClass to the element's data block
	switch (inArray->numericType){
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
	writeDataElement(outfile, MATLAB_DATATYPE__miINT32, inArray->dims, sizeof(int32_t), 2);
	
	
	/*
	 * write the array name element
	 */
	writeDataElement(outfile, MATLAB_DATATYPE__miINT8, (void*)arrayName, sizeof(char), strlen(arrayName));
	
	
	/*
	 * write data element containing real part of array 
	 */
	writeDataElement(outfile, inArray->mxCLASS, inArray->pr, inArray->dataElementSize, nArrayElements);
	
	
	/*
	 * write data element containing imaginary part of array (if complex)
	 */
	if(inArray->numericType == MATLAB_NUMERICTYPE__COMPLEX){
		writeDataElement(outfile, MATLAB_DATATYPE__miDOUBLE, inArray->pi, sizeof(double), nArrayElements);
	}
	
	return 0;
}

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

MATFile* matOpen(const char* filename, const char* openMode){
	/*
	 * Opens a matfile, tests for sucess and:
	 *   #if opening in 'write' mode, writes a matlab header to the file
	 *   #if opening in 'update' (append) mode, skips ahead to last position of file
	 *   #if opening in 'read' mode, throws an error [reading matfiles isn't planned]
	 */
	
	MATFile*	outfile = NULL;
	
	switch (openMode[0]){
		case 'w':
			//open file in mode "write+binary"
			outfile = fopen(filename, "wb");
			
			//verify file was opened successfully
			if(outfile == NULL) {
				fatal("Error while opening file.\n");
				exit(EXIT_FAILURE);		//this is redundant but avoids "control may reach end of non-void function" warning
			}
			
			//write matlab header to file:
			writeMatfileHeader(outfile, MATLAB_HEADER_TEXT);
			break;
		case 'u':
			fatal("Opening MATFiles in 'update' mode is not yet supported.");	//TODO
			break;
		default:
			fatal("Opening MATFiles in 'read' mode is not supported [nor planned].");
			break;
	}
	
	return outfile;
}

mxArray* mxCreateDoubleMatrix(uintptr_t nRows, uintptr_t nCols, uintptr_t numericType){
	/*
	 * creates a 2D array of double precision floating point values
	 * can be real or complex.
	 */
	printf("nRows: %lu, nCols: %lu\n", nRows, nCols);
	mxArray*	outArray = NULL;
	
	
	// do some input value validation
	if (nRows == 0 || nCols == 0){
		fatal("mxCreateDoubleMatrix(): array dimensions may not be null");
	}
	if (numericType != MATLAB_NUMERICTYPE__REAL && numericType != MATLAB_NUMERICTYPE__COMPLEX){
		fatal("mxCreateDoubleMatrix(): only 'real' and 'complex' arrays are supported. 'Logical' and 'global' are not implemented.");
	}
	
	// allocate memory
	outArray	= malloc(sizeof(mxArray));
	if (outArray == NULL){
		fatal("mxCreateDoubleMatrix(): memory allocation error.");
	}
	
	// initialize variables
	outArray->mxCLASS			= MATLAB_ARRAYTYPE__mxDOUBLE_CLASS;
	outArray->dataElementSize	= sizeof(double);
	outArray->pr 				= NULL;
	outArray->pi 				= NULL;
	outArray->dims[0]			= nRows;
	outArray->dims[1]			= nCols;
	outArray->numericType		= numericType;
	outArray->isStruct			= false;
	outArray->nFields			= 0;
	outArray->fieldNames		= NULL;
	
	// allocate memory for structure members
	// NOTE: mallocDouble already checks for succesfull allocation
	outArray->pr 	=	(void*)mallocDouble(nRows*nCols);
	if (numericType	==	MATLAB_NUMERICTYPE__COMPLEX){
		outArray->pi=	(void*)mallocDouble(nRows*nCols);
	}
	
	return outArray;
}

double*	mxGetPr(mxArray* array){
	/*
	 * Returns pointer to the first element of the mxArray's real data.
	 * NOTE: in the matlab API this functions returns a void* pointer.
	 */
	return(array->pr);
}

double*	mxGetData(mxArray* array){
	/*
	 * Returns pointer to the first element of the mxArray's real data.
	 * NOTE: in the matlab API this functions returns a void* pointer.
	 * NOTE: This is actually identical to mxGetPr()
	 */
	return(array->pr);
}

double*	mxGetPi(mxArray* array){
	/*
	 * Returns pointer to the first element of the mxArray's imaginary data.
	 * NOTE: in the matlab API this functions returns a void* pointer.
	 */
	if (array->numericType == mxCOMPLEX){
		return(array->pi);
	}else{
		fatal("mxGetPi(): mxArray does not contain imaginary data.");
	}
	return NULL;
}

double* mxGetImagData(mxArray* array){
	/*
	 * Returns pointer to the first element of the mxArray's imaginary data.
	 * NOTE: in the matlab API this functions returns a void* pointer.
	 */
	if (array->numericType == mxCOMPLEX){
		return(array->pi);
	}else{
		fatal("mxGetImagData(): mxArray does not contain imaginary data.");
	}
	return NULL;
}

mxArray* mxCreateString(const char *inString){
	// allocate memory for struct
	mxArray*	outArray = malloc(sizeof(mxArray));
	if (outArray == NULL){
		fatal("mxCreateString(): memory allocation error.");
	}
	
	// initialize variables
	outArray->mxCLASS			= MATLAB_ARRAYTYPE__mxCHAR_CLASS;
	outArray->dataElementSize	= sizeof(char);
	outArray->pr 				= NULL;
	outArray->pi 				= NULL;
	outArray->dims[0]			= 1;
	outArray->dims[1]			= strlen(inString);
	outArray->numericType		= mxREAL;
	outArray->isStruct			= false;
	outArray->nFields			= 0;
	outArray->fieldNames		= NULL;
	
	// allocate memory for string
	// NOTE: mallocChar already checks for succesfull allocation
	outArray->pr 	=	(void*)mallocChar(strlen(inString));
	
	//Copy string to data pointer
	//strncpy(char* dst, const char* src, size_t size);
	strncpy(outArray->pr, inString, strlen(inString));
	printf("strlen(inString): %u", (uint32_t)strlen(inString));
	
	return outArray;
}

void	mxSetFieldByNumber(	mxArray*	mxStruct,	//pointer to the mxStruct
							uint32_t	index,			//linear index of the element (if arrays is multidimensional this must be calculated: remember matlab matrixes are column-major order)
							uint32_t	iField,			//index of the structure's field which we want to set.
							mxArray*	inArray){		//the mxArray we want to copy into the mxStruct
	//TODO
	
}

void	mxDestroyArray(mxArray* array){
	/*
	 * Free's all memory alocated to an mxArray.
	 * TODO: add support for freeing structure's subelements
	 */
	 if (array->pr != NULL){
		free(array->pr);
	 }
	 if (array->numericType == mxCOMPLEX && array->pi != NULL){
		free(array->pi);
	 }
	 if (array->isStruct){
		if (array->fieldNames != NULL){
			for (int i=0; i<array->nFields; i++){
				if (array->fieldNames[i] != NULL){
					free((void*)array->fieldNames[i]);
				}
			}
		}
	 }
}

/// Write all ray information to matfile:
uintptr_t matPutVariable(MATFile* outfile, const char* arrayName, mxArray* inArray){
	/*
	 * Writes an mxArray to a matfile;
	 * TODO add support for structures.
	 * TODO actually verify successfull write instead of siomply returning 0
	 */
	if (inArray->isStruct == false){
		//assume that if it isn't struct, its a matrix
		writeArray(outfile, arrayName, inArray);
	}else{
		//TODO
		//writeStruct();
	}
	return 0;
}

/// Finish up
void matClose(MATFile* file){
	fclose(file);
}


