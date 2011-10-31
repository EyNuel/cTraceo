#pragma once
#include <stdint.h>
#include <stdbool.h>
#include "matlabOut.h"
#include "writeArray.c"
#include "writeStructArray.c"

uintptr_t matPutVariable(MATFile* outfile, const char* arrayName, mxArray* inArray);

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
		writeStructArray(outfile, arrayName, inArray);
	}
	return 0;
}
