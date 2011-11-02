#pragma once
#include <stdio.h>
#include <stdint.h>
#include "matlabOut.h"

uint32_t	calcArraySize(mxArray* inArray);


uint32_t	calcArraySize(mxArray* inArray){
	/*
	 * Calculates the total size in Bytes which is required to
	 * contain a struct array and its children.
	 * NOTE: this function is recursive, as it will call itself to
	 * 		 determine the size required by the struct's children.
	 */
	
	uint32_t	size = 0;
	
	if (inArray->isStruct){
		size += 4*8;	// Array Flags[16B] + Dimensions Array [16B]
		//########################################################
	}else{
		fatal("structArraySize(): trying to determine size of an mxArray that isn't a structure array.\n");
		return size;
	}
	return 0;
}
