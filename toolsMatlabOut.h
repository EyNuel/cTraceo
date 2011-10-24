
#pragma 	once
#include	<stdint.h>
#include	<stdbool.h>
//#include	"toolsMatlabOut.c"
//#include	"globals.h"

/*
 * Text written to file header
 */
#define MATLAB_HEADER_TEXT "The cTraceo Acoustic Raytracing program. Signal Processing LABoratory, Universidade do Algarve 2011.                        "

/*
 * Definitions for matlab data types
 */
#define MATLAB_DATATYPE__miINT8		1
#define MATLAB_DATATYPE__miUINT8	2
#define MATLAB_DATATYPE__miINT16	3
#define MATLAB_DATATYPE__miUINT16	4
#define MATLAB_DATATYPE__miINT32	5
#define MATLAB_DATATYPE__miUINT32	6
#define MATLAB_DATATYPE__miSINGLE	7
#define MATLAB_DATATYPE__miDOUBLE	9
#define MATLAB_DATATYPE__miINT64	12
#define MATLAB_DATATYPE__miUINT64	13
#define MATLAB_DATATYPE__miMATRIX	14

/*
 * Definitions for matlab Array types (classes)
 */
#define MATLAB_ARRAYTYPE__mxCELL_CLASS		1
#define MATLAB_ARRAYTYPE__mxSTRUCT_CLASS	2
#define MATLAB_ARRAYTYPE__mxOBJECT_CLASS	3
#define MATLAB_ARRAYTYPE__mxCHAR_CLASS		4
#define MATLAB_ARRAYTYPE__mxSPARSE_CLASS	5
#define MATLAB_ARRAYTYPE__mxDOUBLE_CLASS	6
#define MATLAB_ARRAYTYPE__mxSINGLE_CLASS	7
#define MATLAB_ARRAYTYPE__mxINT8_CLASS		8
#define MATLAB_ARRAYTYPE__mxUINT8_CLASS		9
#define MATLAB_ARRAYTYPE__mxINT16_CLASS		10
#define MATLAB_ARRAYTYPE__mxUINT16_CLASS	11
#define MATLAB_ARRAYTYPE__mxINT32_CLASS		12
#define MATLAB_ARRAYTYPE__mxUINT32_CLASS	13

/*
 * Definitions for matlab numeric data types_
 */

 #define MATLAB_NUMERICTYPE__REAL			0
 #define MATLAB_NUMERICTYPE__COMPLEX		1
 #define MATLAB_NUMERICTYPE__GLOBAL			2
 #define MATLAB_NUMERICTYPE__LOGICAL		3

 #define mxREAL			0
 #define mxCOMPLEX		1
 #define mxGLOBAL		2
 #define mxLOGICAL		3

/*
 * replacement definition for matlab's mxArray and others
 */

typedef struct{
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
	size_t			dataElementSize;//Size in bytes, of the individual data elements (1B: Char, 4B Float, 8B: Double...)
	void*			pr;				//pointer to the real part of the data
	void*			pi;				//pointer to the imaginary part of the data (if data is complex)
	uint32_t		dims[2];		//always 2D
	uintptr_t		numericType;	//real, complex, logical or global [don't actually now what global implies...]
	bool			isStruct;
	uintptr_t		nFields;
	const char**	fieldNames;		//something like: {	"theta","r","z"};
}mxArray;




typedef FILE MATFile;
