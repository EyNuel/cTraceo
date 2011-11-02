#pragma once
#include <stdio.h>
#include "stdint.h"

FILE* writeMatfileHeader(		FILE* outfile, const char descriptiveText[124]);

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
