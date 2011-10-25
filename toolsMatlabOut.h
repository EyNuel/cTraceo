
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
 * Definitions for matlab Array types (classes)
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
	uintptr_t		nBytes;			//the total amount of bytes required to write this struct to a matfile. (NOTE: this is calculated just before writing to file, in structArraySize())
	void*			pr;				//pointer to the real part of the data
	void*			pi;				//pointer to the imaginary part of the data (if data is complex)
	uint32_t		dims[2];		//always 2D
	uintptr_t		numericType;	//real, complex, logical or global [don't actually now what global implies...]
	bool			isStruct;
	uintptr_t		nFields;
	const char**	fieldNames;		//something like: {	"theta","r","z"};
}mxArray;




typedef FILE MATFile;
