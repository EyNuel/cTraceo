#pragma once
#include <stdint.h>
#include <stdbool.h>
#include "matlabOut.h"


void	mxSetFieldByNumber(	mxArray* mxRayStruct, uint32_t index, uint32_t iField, mxArray* inArray);

void	mxSetFieldByNumber(	mxArray*	mxStruct,		//pointer to the mxStruct
							uint32_t	index,			//linear index of the element (if arrays is multidimensional this must be calculated: remember matlab matrixes are column-major order)
							uint32_t	iField,			//index of the structure's field which we want to set.
							mxArray*	inArray){		//the mxArray we want to assign to the mxStruct
	/* 
	 * Assigns an mxArray to one of the fields of a structArray
	 */
	printf("mxSetFieldByNumber(): address of inArray: %lu\n", inArray);
	inArray->isChild = true;	//determines that this mxArray is a child of another one (necessary for calculating the size of structures)
	mxStruct[index].field[iField] = inArray;
	
}
