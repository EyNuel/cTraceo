#pragma once
#include <stdint.h>
#include <stdbool.h>
#include "matlabOut.h"


void	mxSetFieldByNumber(	mxArray*	mxStruct, uint32_t index, uint32_t iField, mxArray* inArray);

void	mxSetFieldByNumber(	mxArray*	mxStruct,		//pointer to the mxStruct
							uint32_t	index,			//linear index of the element (if arrays is multidimensional this must be calculated: remember matlab matrixes are column-major order)
							uint32_t	iField,			//index of the structure's field which we want to set.
							mxArray*	inArray){		//the mxArray we want to assign to the mxStruct
	/* 
	 * Assigns an mxArray to one of the fields of a structArray
	 */
	 
	/* do some input validation */
	//printf("mxSetFieldByNumber(): address of inArray: %lu\n", inArray);
	if (mxStruct == NULL){
		fatal("mxSetFieldByNumber(): Error: assigning mxArray to null pointer!\n");
	}
	if (mxStruct[index].isStruct == false){
		fatal("mxSetFieldByNumber(): Assigning an mxArray to something that is not a struct!\n");
	}
	//verify if index is valid
	if (index >= mxStruct->dims[0]*mxStruct->dims[1]){
		char	string[256];
		sprintf(string, "mxSetFieldByNumber(): index %u is out of bounds for %ux%u structure array.\n", index, mxStruct->dims[0], mxStruct->dims[1]);
		fatal((const char *)string);
	}
	//verify if iField is valid:
	if (iField >= mxStruct->nFields){
		fatal("mxSetFieldByNumber(): trying to assign mxArray to ");
	}
	
	inArray->isChild = true;	//defines this mxArray as a child of another one (necessary for calculating the size of structures)
	mxStruct[index].field[iField] = inArray;
	
	//printf("assigned: %s\n", mxStruct[index].fieldNames[iField]);
	//print the addresses of the mxStruct members:
	/*
	for (uintptr_t iStruct=0; iStruct<mxStruct->dims[0]*mxStruct->dims[1]; iStruct++){
		printf("&mxStruct[0]: %lu\n", &mxStruct[iStruct]);
	}
	*/
}
