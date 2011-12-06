/*
 *  Copyright 2011 Emanuel Ey <emanuel.ey@gmail.com>
 * 
 *  This file is part of matOut.
 *
 *  MatOut is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  MatOut is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with matOut.  If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once
#include <stdio.h>
#include <stdint.h>
#include "matOut.h"

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
