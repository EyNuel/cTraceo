#pragma once
#include <stdint.h>

uint32_t	dataElementSize(size_t dataItemSize, uint32_t nDataItems);

uint32_t	dataElementSize(size_t dataItemSize, uint32_t nDataItems){
	/*
	 * calculates the size of an unwritten data element.
	 * This is needed because the header for an array needs to
	 * contain the number of bytes that compose the array.
	 * NOTE: includes the size of any required padding bytes.
	 */
	uint32_t	nBytes;
	uint32_t	paddingBytes = 0;
	
	nBytes	= dataItemSize * nDataItems;
	if (nBytes % 8 > 0){
		paddingBytes= 8 - nBytes % 8;	//This could probably be neatly rewritten with the ternary operator
	}
	
	return(8 +nBytes +paddingBytes);
}
