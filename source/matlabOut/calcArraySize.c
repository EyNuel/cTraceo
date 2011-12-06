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
#include "dataElementSize.c"

uint32_t	calcArraySize(mxArray* inArray);


uint32_t	calcArraySize(mxArray* inArray){
	/*
	 * Calculates the total size in Bytes which is required to
	 * contain a struct array and its children.
	 * NOTE: this function is recursive, as it will call itself to
	 * 		 determine the size required by the struct's children.
	 * NOTE: an mxArray's name is not known before the call to
	 * 		 matPutVariable which writes it to a file, so the
	 * 		 number of bytes needed to write an mxArray's name 
	 *  	 is not calculated in this function.
	 * 		 This is actually not very important, as array names
	 * 		 are only written for mxArrays which aren't members
	 * 		 in a structure.
	 */
	
	uintptr_t	nBytes = 0;
	uintptr_t	maxLengthFieldname = 0;
	
	
	if (inArray == NULL){
		//printf("calcArraySize(): received null pointer: ");
		/*
		 * Size required by an empty mxArray:
		 * NOTE: empty arrays are written to the matfile by writeEmptyArray()
		 */
		nBytes += 8;	//header:						"miMatrix"[4B]; "size"[4B]
		nBytes += 16;	//Array flags:					"miUINT32"[4B]; "8"[4B]; "flags+mxCLASS"[4B]; "undefined"[4B]
		nBytes += 16;	//2D dimensions (1x1)
		nBytes += 8;	//empty array name				"miINT8"[4B]; "0"[4B]
		nBytes += 8;	//empty "double" data element	"miDouble"[4B9; "0"[4B]
		
		return nBytes;
	}
	
	
	// Array Flags[16B] + Dimensions Array [16B]
	nBytes += 4*8;
	
	/*
	 * NOTE: The array names of member structures aren't
	 * 		written, only a single byte defining the length
	 * 		of the array name as being zero.
	 * 		So: for parent arrays, the size required to write the
	 * 		name to the file is calculated at the moment it is
	 * 		written to disk, everytjing else (including the 8B
	 * 		which define the arrayname as being of length 0) is
	 * 		calculated here.
	 */
	if (inArray->isChild){
		nBytes += 8;
	}
	
	/*
	 * Handle mxArrays which are structs:
	 */
	if (inArray->isStruct){
		//printf("calculating size of mxStruct:\n");
		/*
		 * Determine size required for the fieldnames:
		 */
		 
		// Add size required for "field name length"(8B) + header
		// header for the field names (another 8B)
		
		nBytes += 8 + 8;
		//printf("headers: %lu\n", nBytes);
		
		//get length of longest fieldname
		for (uintptr_t i=0; i<inArray->nFields; i++){
			maxLengthFieldname = max(maxLengthFieldname, strlen(inArray->fieldNames[i]));
		}
		
		//need to add 1B for the string's NULL terminator
		maxLengthFieldname += 1;
		nBytes += maxLengthFieldname * inArray->nFields;
		
		// determine padding required for the fieldnames
		if (nBytes % 8 > 0){
			//add padding to size
			//printf("padded: %lu\n", (8 - nBytes % 8));
			nBytes += 8 - nBytes % 8;
		}
		//printf("size required for fieldnames: %lu\n", maxLengthFieldname * inArray->nFields);
		//printf("toal size required icluding fieldnames: %lu\n", nBytes);
		
		
		
		/*
		 * Determine size required for the structure's children
		 */
		for (uintptr_t j=0; j<inArray->dims[0]*inArray->dims[1]; j++){
			for (uintptr_t i=0; i<inArray->nFields; i++){
				nBytes += calcArraySize(inArray[j].field[i]);
				//printf("size required including child[%lu]: %lu\n", i, nBytes);
			}
		}
	
	/*
	 * Handle mxArrays which aren't structures:
	 */
	}else{
		// determine size required for the actual data:
		if (inArray->mxCLASS == mxCHAR_CLASS){
			/*
			 * NOTE: mxCHAR_CLASS is strange: although datatype
			 * 		 is 'char' (which should be 1B) 2B are
			 * 		 written per character
			 */
			nBytes += dataElementSize(2*sizeof(char), inArray->dims[0]*inArray->dims[1]);
		}else{
			/*
			 * All other data types [seem to] behave as expected
			 */
			nBytes += dataElementSize(inArray->dataElementSize, inArray->dims[0]*inArray->dims[1]);
			//printf("dataElementSize(): %u\n", dataElementSize(inArray->dataElementSize, inArray->dims[0]*inArray->dims[1]));
			
			//if data is complex, get size of imaginary part
			if (inArray->numericType == mxCOMPLEX){
				nBytes += dataElementSize(inArray->dataElementSize, inArray->dims[0]*inArray->dims[1]);
			}
		}
	}
	
	//save the size of the mxArray's contents
	inArray->nBytes = nBytes;
	
	/*
	 * NOTE: the size given in an mxArray's header does not
	 * 		 include the 8 bytes of the header itself, thus
	 * 		 an additional 8B are actually required to write the
	 * 		 mxArray's header. However, in case of nested
	 * 		 structures, the 8B for the header are needed.
	 * 		 Because of this, those 8B are added to the return
	 * 		 value.
	 * 		TODO: this sounds confusing...
	 */
	//printf("nBytes+8: %lu\n", nBytes+8);
	return nBytes +8;
}
