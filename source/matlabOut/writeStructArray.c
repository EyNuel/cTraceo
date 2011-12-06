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

uintptr_t	writeStructArray(		MATFile* outfile, const char* arrayName, mxArray* inArray);

uintptr_t	writeStructArray(MATFile* outfile, const char* arrayName, mxArray* inArray){
	/*
	 * writes a structure array to an open matfile.
	 */
	uint32_t	tempUInt32;
	uint8_t		tempUInt8;
	uintptr_t	maxLengthFieldname = 0;
	uintptr_t	nBytes, paddingBytes = 0;
	
	
	/* *********************************************************
	 * write miMATRIX tag and total number of bytes in the matrix
	 * NOTE: total number of bytes does not include the first 8B
	 * 		 of the file (4B[miMATRIX] +4B[nBytes]).
	 */
	tempUInt32	= miMATRIX;
	fwrite(&tempUInt32, sizeof(uint32_t), 1, outfile);
	
	/* Calculate the struct array's size:
	 * NOTE: the 8 extra bytes below are for the array name tag.
	 * 		 Extra size to be added is obtained from the
	 * 		 arrayName's length.
	 * NOTE: The array name is only written if the struct is
	 * 		 not a child of another struct.
	 */
	tempUInt32	= inArray->nBytes + 8;
	//printf("total size without array name: %u\n", tempUInt32);
	if(inArray->isChild == false){
		// add size for full lines
		tempUInt32 += strlen(arrayName) / 8;
		// if padding is required, include another full line
		if ( (strlen(arrayName) % 8) > 0) {
			tempUInt32 += 8;
		}
	}
	//printf("total size inc. arrayname: %u\n", tempUInt32);
	//write size to file
	fwrite(&tempUInt32, sizeof(uint32_t), 1, outfile);
	
	
	
	/* *********************************************************
	 * generate array flags:
	 * "miUINT32"(4B), "8"(4B)
	 * "undefined"(2B), "flags"(1B), "mxCLASS"(1B), "undefined"(4B)
	 */
	 
	//write datatype and number of bytes to element's tag
	tempUInt32 = miUINT32;
	fwrite(&tempUInt32, sizeof(uint32_t), 1, outfile);
	tempUInt32 = 8;
	fwrite(&tempUInt32,	sizeof(uint32_t), 1, outfile);
	
	//write flags and mxClass to the element's data block
	//NOTE: for a struct array: flags=0x00
	tempUInt32 =	mxSTRUCT_CLASS;
	fwrite(&tempUInt32, sizeof(uint32_t), 1, outfile);
	
	//write 4B of undefined data to element's data block
	tempUInt32 = 0x00;
	fwrite(&tempUInt32, sizeof(uint32_t), 1, outfile);
	
	
	
	/* *********************************************************
	 * write the dimensions element
	 */
	writeDataElement(outfile, miINT32, inArray->dims, sizeof(int32_t), 2);
	
	
	
	/* *********************************************************
	 * write the struct array's name
	 * "1"(2B), "miINT8"(2B), "X"(1B), "padding"(3B)
	 */
	tempUInt32	= miINT8;
	fwrite(&tempUInt32, sizeof(uint32_t), 1, outfile);
	
	if(inArray->isChild == false){
		tempUInt32	= strlen(arrayName);
		fwrite(&tempUInt32, sizeof(uint32_t), 1, outfile);
		
		for (uintptr_t i=0; i<strlen(arrayName); i++){
			tempUInt8	= arrayName[i];
			fwrite(&tempUInt8, sizeof(uint8_t), 1, outfile);
		}
		
		//write padding at end:
		nBytes = strlen(arrayName);
		if (nBytes % 8 > 0){
			paddingBytes= 8 - nBytes % 8;	//This could probably be neatly rewritten with the ternary operator
		}
		tempUInt8 = 0x00;
		for (uintptr_t i=0; i<paddingBytes; i++){
			fwrite(&tempUInt8, sizeof(uint8_t), 1, outfile);
		}
	}else{
		// this struct is a child of another struct => don't write it's name (it is saved in the parent's fieldNames)
		tempUInt32	= 0x00;
		fwrite(&tempUInt32, sizeof(uint32_t), 1, outfile);
	}
	
	
	
	/* *********************************************************
	 * write the struct's fieldnames
	 * "4"(2B), "miINT32"(2B), "maxLengthFieldname"(4B)
	 * "minINT("(4B), "sum of FieldLengths" (4B)
	 * n times: "fieldName[i] +NULL"(length of string), "padding" (to get to maxLengthFieldname)
	 * "padding" (to get to 8B boundary)
	 */
	tempUInt32	=	0x04;	//Why 4??
	tempUInt32	<<=	16;		//left shift by 2B
	tempUInt32	|=	miINT32;
	fwrite(&tempUInt32, sizeof(uint32_t), 1, outfile);
	
	//get length of longest fieldname
	for (uintptr_t i=0; i<inArray->nFields; i++){
		maxLengthFieldname = max(maxLengthFieldname, strlen(inArray->fieldNames[i]));
	}
	maxLengthFieldname += 1;	//need to add one for the string's NULL terminator
	//printf("maxLengthFieldname: %lu\n", maxLengthFieldname);
	
	//write length of longest fieldName:
	fwrite(&maxLengthFieldname, sizeof(uint32_t), 1, outfile);
	
	//write "miINT8("(4B), "sum of FieldLengths" (4B)
	tempUInt32	= miINT8;
	fwrite(&tempUInt32, sizeof(uint32_t), 1, outfile);
	tempUInt32	= inArray->nFields * maxLengthFieldname;
	fwrite(&tempUInt32, sizeof(uint32_t), 1, outfile);
	
	//write fieldnames (with padding at end of each fiedname to reach maxLengthFieldname)
	tempUInt8	= 0x00;
	for (uintptr_t iField=0; iField<inArray->nFields; iField++){
		//write the fieldName:
		//printf("writing fieldName[%lu]: %s, strlen()= %lu\n", iField, inArray->fieldNames[iField], strlen(inArray->fieldNames[iField]));
		fwrite(inArray->fieldNames[iField], sizeof(uint8_t), strlen(inArray->fieldNames[iField]), outfile);
		
		//write individual padding:
		for (uintptr_t j=0; j<maxLengthFieldname-strlen(inArray->fieldNames[iField]); j++){
			fwrite(&tempUInt8, sizeof(uint8_t), 1, outfile);
		}
	}
	
	//after the fieldNames (which were already zero-padded to have a length of maxLengthFieldnames),
	//pad some more to reach the 8B boundary
	paddingBytes = 0;
	nBytes = maxLengthFieldname * inArray->nFields;
	//printf("=>=>=> nBytes: %lu\n", nBytes);
	if (nBytes > 8){
		if (nBytes % 8 > 0){
			paddingBytes= 8 - nBytes % 8;	//This could probably be neatly rewritten with the ternary operator
		}
	}else{
		paddingBytes = 8 - nBytes;
	}
	//printf("PaddingBytes: %lu\n", paddingBytes);
	for (uintptr_t i=0; i<paddingBytes; i++){
		fwrite(&tempUInt8, sizeof(uint8_t), 1, outfile);
		//printf("pad%lu ",i);
	}
	//printf("\n");
	
	
	
	/* *********************************************************
	 * write the structure's children to the matfile
	 */
	for (uintptr_t j=0; j<inArray->dims[0] * inArray->dims[1]; j++){
		for (uintptr_t i=0; i<inArray->nFields; i++){
			if (inArray[j].field[i] == NULL){
				//empty structure field => need to write an empty mxArray to file
				writeEmptyArray(outfile);
			
			}else if (inArray[j].field[i]->isStruct){
				writeStructArray(outfile, inArray->fieldNames[i], inArray[j].field[i]);
			
			}else{
				writeArray(outfile, inArray->fieldNames[i], inArray[j].field[i]);
			}
		}
	}
	
	 
	return 0;
}
