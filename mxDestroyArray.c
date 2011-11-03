#pragma once
#include <stdlib.h>
#include "matlabOut.h"

void mxDestroyArray(mxArray* inArray);

void mxDestroyArray(mxArray* inArray){
	/*
	 * Free's all memory alocated to an mxArray.
	 */
	 if (inArray->pr_double != NULL){
		free(inArray->pr_double);
	 }
	 if (inArray->pr_char != NULL){
		free(inArray->pr_char);
	 }
	 if (inArray->numericType == mxCOMPLEX && inArray->pi_double != NULL){
		free(inArray->pi_double);
	 }
	 //if this is a structure, free its children
	 if (inArray->isStruct){
		if (inArray->fieldNames != NULL){
			for (uintptr_t i=0; i<inArray->nFields; i++){
				if (inArray->fieldNames[i] != NULL){
					free((void*)inArray->fieldNames[i]);
				}
				//mxDestroyArray(&inArray->field[i]);
			}
		}
	 }
}
