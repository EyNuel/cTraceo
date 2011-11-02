#pragma once
#include <stdlib.h>
#include "matlabOut.h"

void mxDestroyArray(mxArray* inArray);

void mxDestroyArray(mxArray* inArray){
	/*
	 * Free's all memory alocated to an mxArray.
	 * TODO: add support for freeing structure's subelements
	 */
	 if (inArray->pr != NULL){
		free(inArray->pr);
	 }
	 if (inArray->numericType == mxCOMPLEX && inArray->pi != NULL){
		free(inArray->pi);
	 }
	 if (inArray->isStruct){
		if (inArray->fieldNames != NULL){
			for (int i=0; i<inArray->nFields; i++){
				if (inArray->fieldNames[i] != NULL){
					free((void*)inArray->fieldNames[i]);
				}
			}
		}
	 }
}
