/*
 * This code contains testing code for csValues.c
 * Compiled with: clang -march=i686 -m32 -lm -o t_csValues.bin t_csValues.c
 */

#include <stdio.h>
#include <stdlib.h>
#include "../tools.c"
#include "../csValues.c"
#include "../readIn.c"
#include <math.h>

int main(void){
	const char*	infile = NULL;
	globals_t*	globals = NULL;
	
	vector_t* 	slowness = NULL;
	double*		ri = mallocDouble(1);
	double*		zi = mallocDouble(1);
	double*		ci = mallocDouble(1);
	double*		cc = mallocDouble(1);
	double*		si = mallocDouble(1);
	double*		cri = mallocDouble(1);
	double*		czi = mallocDouble(1);
	double*		crri = mallocDouble(1);
	double*		czzi = mallocDouble(1);
	double*		crzi = mallocDouble(1);

	ri[0] = 10;
	zi[0] = 10;
	globals = mallocGlobals();
	slowness = malloc(sizeof(vector_t));

	infile = "../munk.in";
	
		
	readIn(globals, infile);

	printSettings(globals);

	csValues(globals, ri, zi, ci, cc, si, cri, czi, slowness, crri, czzi, crzi);

	exit(EXIT_SUCCESS);
}
