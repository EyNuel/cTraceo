#pragma once
#include <stdio.h>
#include <stdint.h>
#include "matlabOut.h"

/*
 * Writes an empty mxArray to file.
 * This is required when structures contain empty fields.
 * The structures fields must always exist, even if empty.
 */

void writeEmptyArray(FILE* outfile);

void writeEmptyArray(FILE* outfile){
	/*
	 * TODO: documentation
	 * TODO: verify successfull write
	 */
	uint32_t	nBytes, tempUInt32, flags, mxClass;
	
	/*
	 * Size required by an empty mxArray:
	 */
	/*
	 *	nBytes += 8;	//header:						"miMatrix"[4B]; "size"[4B]
	 *	nBytes += 16;	//Array flags:					"miUINT32"[4B]; "8"[4B]; "flags+mxCLASS"[4B]; "undefined"[4B]
	 *	nBytes += 16;	//2D dimensions (1x1)
	 *	nBytes += 8;	//empty array name				"miINT8"[4B]; "0"[4B]
	 *	nBytes += 8;	//empty "double" data element	"miDouble"[4B9; "0"[4B]
	 *	NOTE: size written to header does not include the 8B required for the header itself; 
	 */
	nBytes = 48;
	
	//write header:
	tempUInt32	= miMATRIX;
	fwrite(&tempUInt32, sizeof(uint32_t), 1, outfile);
	fwrite(&nBytes, sizeof(uint32_t), 1, outfile);
	
	
	//write array flags:
	tempUInt32	= miUINT32;
	fwrite(&tempUInt32, sizeof(uint32_t), 1, outfile);
	tempUInt32	= 8;
	fwrite(&tempUInt32, sizeof(uint32_t), 1, outfile);
	
	//NOTE: for the purpose of writing and empty array, we'll define it as "real" and "double"
	flags		=	0x00;
	mxClass 	=	mxDOUBLE_CLASS;
	
	tempUInt32	=	flags;
	tempUInt32	<<=	8;	//left shift the flags by one byte
	tempUInt32	|=	mxClass;
	fwrite(&tempUInt32, sizeof(uint32_t), 1, outfile);
	
	
	//write dimensions array (size is 1x1):
	tempUInt32	= miINT32;
	fwrite(&tempUInt32, sizeof(uint32_t), 1, outfile);
	tempUInt32	= 8;
	fwrite(&tempUInt32, sizeof(uint32_t), 1, outfile);
	tempUInt32	= 1;
	fwrite(&tempUInt32, sizeof(uint32_t), 1, outfile);
	fwrite(&tempUInt32, sizeof(uint32_t), 1, outfile);
	
	
	//write an empty name:
	tempUInt32	= miINT8;
	fwrite(&tempUInt32, sizeof(uint32_t), 1, outfile);
	tempUInt32	= 0;
	fwrite(&tempUInt32, sizeof(uint32_t), 1, outfile);
	
	
	//write mxClass (double) and size (which is zero):
	tempUInt32	= miDOUBLE;
	fwrite(&tempUInt32, sizeof(uint32_t), 1, outfile);
	tempUInt32	= 0;
	fwrite(&tempUInt32, sizeof(uint32_t), 1, outfile);
	
}
