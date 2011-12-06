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
#include <stdlib.h>
#include "matOut.h"

void mxDestroyArray(mxArray* inArray);

void mxDestroyArray(mxArray* inArray){
	/*
	 * Free's all memory alocated to an mxArray.
	 */
	if (inArray != NULL){
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
}
