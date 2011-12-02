#pragma once
#include <stdio.h>
#include <stdint.h>
#include "matlabOut.h"

void writeDataElement(FILE* outfile, uint32_t miTYPE, void* data, size_t dataItemSize, uint32_t nDataItems);

void writeDataElement(FILE* outfile, uint32_t miTYPE, void* data, size_t dataItemSize, uint32_t nDataItems){
	/*
	 * TODO: documentation
	 * TODO: verify successfull write
	 */
	uint8_t		tempUInt8;
	uint32_t	nBytes;
	uint32_t	paddingBytes = 0;
	uintptr_t	i;
	uint8_t		emptyChar = 0x00;
	
	
	//calculate the number of data bytes:
	nBytes	= dataItemSize * nDataItems;
	if (miTYPE == mxCHAR_CLASS){
		//mxCHAR_CLASS is strange: altough the data is 8bit, it is written as 16bit uints,
		//making this just a bit trickier
		miTYPE = miUINT16;
		nBytes *= 2;
		//write the datatype identifier
		fwrite(&miTYPE, sizeof(uint32_t), 1, outfile);
		miTYPE = mxCHAR_CLASS;
	}else{
		//write the datatype identifier
		fwrite(&miTYPE, sizeof(uint32_t), 1, outfile);
	}
	
	
	//write the number of data bytes to the data element's tag:
	fwrite(&nBytes, sizeof(uint32_t), 1, outfile);
	
	
	// write the data
	if (miTYPE == mxCHAR_CLASS){
		//Again, because mxCharClass is strange, we have to write one char
		//at a time, each followed by a null byte
		uint8_t		*tempPtr = data;
		tempUInt8 = 0x00;
		
		for (uintptr_t iChar=0; iChar<strlen(data); iChar++){
			fwrite(tempPtr, sizeof(uint8_t), 1, outfile);
			tempPtr += 1;
			fwrite(&tempUInt8, sizeof(uint8_t), 1, outfile);
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
	//printf("numBytes: %u, paddingBytes: %u\n", nBytes, paddingBytes);
	for (i=0; i<paddingBytes; i++){
		fwrite(&emptyChar, sizeof(uint8_t), 1, outfile);
	}
}
