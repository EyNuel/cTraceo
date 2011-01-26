/*
 * This code contains some testing cases for bracket.c
 * Compiled with: clang -march=i686 -m32 -lm -o t_bracket.exe t_bracket.c
 */

#include <stdio.h>
#include <stdlib.h>
#include "../tools.c"
#include "../bracket.c"
#include <math.h>

int main(void){

	double*		xVector = NULL;	//shall contain a sorted vector of numbers
	double 		xi;				//an element who's "bracketing" elements are to be found
	uint32_t	i, j, n;
	
	//a sequence with an even number of elements
	n = 6;
	xVector = mallocDouble(n);
	xVector[0] = 0.1365;
	xVector[1] = 0.1730;
	xVector[2] = 0.2844;
	xVector[3] = 0.5155;
	xVector[4] = 0.6946;
	xVector[5] = 0.8385;
	xi = 0.52;				//returned index should be "3"
	
	printf("\nx:");
	for(i=0; i<n; i++){
		printf(" %5.4lf",xVector[i]);
	}
	printf("\nxi: %5.4lf\tindex: ",xi);
	
	bracket(n, xVector, &xi, &j);
	printf("%u\n",j);
	free(xVector);
	
	/* a sequence with an odd number of elements */
	n = 7;
	xVector = mallocDouble(n);
	xVector[0] = 0.1365;
	xVector[1] = 0.1730;
	xVector[2] = 0.2844;
	xVector[3] = 0.5155;
	xVector[4] = 0.6946;
	xVector[5] = 0.8385;
	xVector[6] = 0.9385;
	xi = 0.8000;				//returned index should be "4"
	
	printf("\nx:");
	for(i=0; i<n; i++){
		printf(" %5.4lf",xVector[i]);
	}
	printf("\nxi: %5.4lf\tindex: ",xi);
	
	bracket(n, xVector, &xi, &j);
	printf("%u\n",j);
	
	/* a case in wich xi belongs to the sequence	*/
	xi = 0.1730;
	printf("\nx:");
	for(i=0; i<n; i++){
		printf(" %5.4lf",xVector[i]);
	}
	printf("\nxi: %5.4lf\tindex: ",xi);
	
	bracket(n, xVector, &xi, &j);
	printf("%u\n",j);
	free(xVector);

	exit(EXIT_SUCCESS);
}
