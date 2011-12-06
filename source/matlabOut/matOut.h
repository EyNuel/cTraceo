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

#pragma 	once
#include	<stdint.h>
#include	<stdbool.h>


/*
 * Text written to file header
 */
#define MATLAB_HEADER_TEXT "The cTraceo Acoustic Raytracing program. Signal Processing LABoratory, Universidade do Algarve 2011.                        "

/*
 * Definitions for matlab data types (miTYPE)
 */
#define miINT8		1
#define miUINT8		2
#define miINT16		3
#define miUINT16	4
#define miINT32		5
#define miUINT32	6
#define miSINGLE	7
#define miDOUBLE	9
#define miINT64		12
#define miUINT64	13
#define miMATRIX	14

/*
 * Definitions for matlab Array types (mxCLASS)
 */
#define mxCELL_CLASS	1
#define mxSTRUCT_CLASS	2
#define mxOBJECT_CLASS	3
#define mxCHAR_CLASS	4
#define mxSPARSE_CLASS	5
#define mxDOUBLE_CLASS	6
#define mxSINGLE_CLASS	7
#define mxINT8_CLASS	8
#define mxUINT8_CLASS	9
#define mxINT16_CLASS	10
#define mxUINT16_CLASS	11
#define mxINT32_CLASS	12
#define mxUINT32_CLASS	13

/*
 * Definitions for matlab numeric data types_
 */

 #define mxREAL			0
 #define mxCOMPLEX		1
 #define mxGLOBAL		2
 #define mxLOGICAL		3


/*
 * replacement definition for matlab's mxArray and others
 */

struct mxArray{
	/*
	 * An mxArray contains an array's:
	 * 		type		[actually always double]
	 * 		dimensions	[implemented as always 2D]
	 *		data 
	 * 		if numeric, whether the variable is real or complex
	 *		[If sparse, its indices and nonzero maximum elements -- not implemented]
	 * 		If a structure [or object -- not implemented], the number of fields and field names
	 * TODO: add support for structure elements
	 */
	
	uint8_t			mxCLASS;		//one of the Matlab array classes (Char array: mxCHAR_CLASS; Double precision array: mxDOUBLE_CLASS; etc...)
	uint8_t			miTYPE;
	size_t			dataElementSize;//Size in bytes, of the individual data elements (1B: Char, 4B Float, 8B: Double...)
	uintptr_t		nBytes;			//the total amount of bytes required to write this mxArray to a matfile. (NOTE: this is calculated just before writing to file, in calcArraySize())
	double*			pr_double;		//pointer to real part of double precision  data
	double*			pi_double;		//pointer to imaginary part of double precision data (if data is complex)
	char*			pr_char;		//pointer to char data
	uint32_t		dims[2];		//always 2D
	uintptr_t		numericType;	//real, complex, logical or global [don't actually now what global implies...]
	bool			isStruct;		//determines if this mxArray is a structure
	bool 			isChild;		//determines wheter this mxArray is a Child of another (when set, its name will not be written to the matfile, as it is already defined in the parent's fieldnames
	uintptr_t		nFields;
	char**			fieldNames;		//something like: {	"theta","r","z"};
	struct mxArray	**field;		//pointer to member mxArrays. only used when isStruct is set.
};
typedef struct mxArray mxArray;


typedef FILE MATFile;

/*
 *  all the files which make up the matlabOut package.
 * 	NOTE: 	these are included at the end so that they can make use of the
 * 			definitions contained in this file
 */
#include	"toolsMisc.c"
#include	"toolsMemory.c"
#include	"calcArraySize.c"
#include	"matClose.c"
#include	"writeEmptyArray.c"
#include	"matPutVariable.c"
#include	"mxCalcSingleSubscript.c"
#include	"mxCreateString.c"
#include	"mxDestroyArray.c"
#include	"mxGetImagData.c"
#include	"mxGetPr.c"
#include	"writeDataElement.c"
#include	"writeStructArray.c"
#include	"dataElementSize.c"
#include	"matOpen.c"
#include	"mxCreateDoubleMatrix.c"
#include	"mxCreateStructMatrix.c"
#include	"mxGetData.c"
#include	"mxGetPi.c"
#include	"mxSetFieldByNumber.c"
#include	"writeArray.c"
#include	"writeMatfileHeader.c"


