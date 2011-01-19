#include <stdio.h>
#include <stdlib.h>
#include "intLinear1D.c"
#include "intBarycParab1D.c"
#include "intBarycCubic1D.c"
#include "tools.c"
#include <math.h>

int main(void){

	double*		xVector = NULL;
	double*		yVector = NULL;
	double 		x[2], xi;
	double		y[2], fi, fxi, fxxi;
	double		j;
	uint32_t	i;
	
	x[0]	= 2;
	x[1] 	= 4;
	xi 		= 2.3;

	y[0] =	8.3;
	y[1] =	14.3;
	//test linear interpolation
	//intLinear1D(x, y, xi, &fi, &fxi);
	//printf("fi:%lf;\nfxi:%lf\n",fi,fxi);

	//test subvectors
	xVector = mallocDouble(10);
	yVector = mallocDouble(10);
	for(i=0; i<10; i++){
		xVector[i] = (double)i;
		//yVector[i] = sin((0.5*M_PI*(double)i));	//a sine
		//yVector[i] = (double)(i*i);					//a parabola
		yVector[i] = 0.5*pow((double)i,3) +1.76*pow((double)i,2) -2*(double)i +1;		//a 3rd degree polyn.
		
	}
	printf("\nx:");
	for(i=0; i<10; i++){
		printf(" %8.4lf",xVector[i]);
	}
	printf("\nf:");
	for(i=0; i<10; i++){
		printf(" %8.4lf",yVector[i]);
	}
	printf("\n");
	
	//interpolate some values:
	
	printf("\n1D Linear interpolation:\n");
	for(j=1.1; j<8; j+=1.5){
		
		intLinear1D(	subVector(xVector,(uintptr_t)j),
						subVector(yVector,(uintptr_t)j),
						j, &fi, &fxi);
		
		printf("xi:%8.4lf => fi:%8.4lf; \tfxi:%8.4lf;\n",j, fi, fxi);
		
	}
	
	printf("\n1D barycentric parabolic interpolation:\n");
	for(j=1.1; j<8; j+=1.5){
		
		intBarycParab1D(	subVector(xVector,(uintptr_t)j),
							subVector(yVector,(uintptr_t)j),
							j, &fi, &fxi, &fxxi);
		
		printf("xi:%8.4lf => fi:%8.4lf; \tfxi:%8.4lf; \tfxxi:%8.4lf;\n",j, fi, fxi, fxxi);
		
	}
	
	printf("\n1D barycentric cubic interpolation:\n");
	for(j=1.1; j<8; j+=1.5){
		
		intBarycCubic1D(	subVector(xVector,(uintptr_t)j),
							subVector(yVector,(uintptr_t)j),
							j, &fi, &fxi, &fxxi);
		
		printf("xi:%8.4lf => fi:%8.4lf; \tfxi:%8.4lf; \tfxxi:%8.4lf;\n",j, fi, fxi, fxxi);
		
	}
	/*
	 *	verify in matlab with testing/interpolation_tests.m:
	 */
	exit(EXIT_SUCCESS);
}
