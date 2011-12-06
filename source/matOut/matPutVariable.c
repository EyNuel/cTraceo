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
#include <stdint.h>
#include <stdbool.h>
#include "matOut.h"
#include "writeArray.c"
#include "writeStructArray.c"

uintptr_t matPutVariable(MATFile* outfile, const char* arrayName, mxArray* inArray);

uintptr_t matPutVariable(MATFile* outfile, const char* arrayName, mxArray* inArray){
	/*
	 * Writes an mxArray to a matfile;
	 * TODO add support for structures.
	 * TODO actually verify successfull write instead of siomply returning 0
	 */
	
	calcArraySize(inArray);
	//printf("After calcArraySize: inArray->nBytes = %lu \n", inArray->nBytes);
	
	if (inArray->isStruct == false){
		//assume that if it isn't struct, its a matrix
		writeArray(outfile, arrayName, inArray);
	}else{
		writeStructArray(outfile, arrayName, inArray);
	}
	return 0;
}
