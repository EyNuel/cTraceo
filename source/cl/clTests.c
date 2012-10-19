 
 /*
NOTE: compile with:
clang -Wno-missing-braces -Wstrict-prototypes -std=gnu99 -I/opt/AMDAPP/include -o clTests clTests.c
*/
 
 #ifdef __APPLE__
	#include <OpenCL/cl.h>
#else
	#include <CL/cl.h>
#endif
#include <stdio.h>


/*
 * Some notes:
 * Opencl does not define operations on vectors in the host code, but since we want this for
 * performance reasons, we use builtin vector support from clang/gcc for this.
 * Unfortunately, the compilers don't define vectors the same way, so we need some compiler
 * dependant code.
 * 
 */
#if 0
#ifdef __clang__
    typedef float float2 __attribute__((ext_vector_type(2)));
    /*
     * works with clang, not with gcc
     */
#else
    //there seems to be no macro test if the current compiler is GCC... (can this be?!)
    typedef float float2 __attribute__ ((vector_size(8)));
    /*
     * does not work with clang
     * works with gcc
     */
#endif

/*
#define .X [0]
#define .Y [1]
* NOTE: this isn't allowed (the pre-pocessor doesn't like ".X")
*/

/** From cl_platform.h

typedef cl_float    __cl_float2     __attribute__((vector_size(8)));

typedef union{
    cl_float  CL_ALIGNED(8) s[2];
    #if defined( __GNUC__) && ! defined( __STRICT_ANSI__ )
       __extension__ struct{ cl_float  x, y; };
       __extension__ struct{ cl_float  s0, s1; };
       __extension__ struct{ cl_float  lo, hi; };
    #endif
    #if defined( __CL_FLOAT2__) 
        __cl_float2     v2;
    #endif
}cl_float2;


 */

//typedef cl_float float2 __attribute__((ext_vector_type(2)));
/*
 * works with clang, with both a.x and a[0] syntax.
 * does not work with gcc
 */

/*
#ifdef __GNUC__
    //this is gcc
    typedef float float2 __attribute__ ((vector_size(8)));
#endif
*/

/*
union float {
     v;
    float e[4];
    union{
        float x;
        float y;
    }
}
*/
#endif

#define X 0
#define Y 1

typedef __cl_float2	vector_t;

int main(void){
    vector_t      a = { 1.0f, 2.0f };
    vector_t      b = { 1.1f, 2.2f };
    vector_t      c = { 0.0f, 0.0f };
    float         d = 5.55;
    vector_t      *e = NULL;
    
    e = malloc(sizeof(vector_t));
    
    
    #ifdef __gcc__
        //this is gcc
        printf("this was compiled in  gcc \n");
    #endif
    #ifdef __clang__
        //this is clang
        printf("this was compiled with clang\n");
    #endif
    
    
    printf("a= %f, %f\n", a[X], a[Y]);
    printf("b= %f, %f\n", b[X], b[Y]);
    
    c = a+b;
    printf("a + b= %f, %f\n", c[X], c[Y]);
    
    c = a/b;
    printf("a / b= %f, %f\n", c[X], c[Y]);
 
    c = a + (vector_t){1,1};
    printf("a + 1= %f, %f\n", c[X], c[Y]);
    
    c = a * (vector_t){4,4};
    printf("a * 4= %f, %f\n", c[X], c[Y]);
    
    c[X] = 4.444;
    c[Y] = d;
    printf("assigning to individual components of c: c= %f, %f\n", c[X], c[Y]);
    
    /*
     * The following fails -apparently it is not possible to assign data to individual elements of a vector when using a pointer
    
    (*e).x = 4.444;
    *e[Y] = d;
    printf("assigning to individual components of *e: e= %f, %f\n", e[X], e[Y]);
    
    * So, the idea is to operate on a local vector and then copy the entire vector to the pointer, like so:
    */
    c[X] = 4.444;
    c[Y] = d;
    *e = c;         //copy data from local vector 'c' to pointer 'e'
    c = (vector_t){0,0};      //'erase' contents of c
    printf("checking that content of c has been 'wiped' c= %f, %f\n", c[X], c[Y]);
    c = *e;         //get contents of e
    printf("reading from vector pointer *e: e= %f, %f\n", c[X], c[Y]);
    
    //this does NOT work:
    //c = (vector_t)(d);
    //but this does:
    c = (vector_t){d, d};
    printf("Conversion from single float to float2: c= %f, %f\n", c[X], c[Y]);
    
    /*
     * NOTE: The following fails to compile because the opencl built-in functions are only available in device code.
     *       NOT AVAILABLE IN HOST CODE.
     *
    c = cl_dot(a, b);
    printf("dot(a, b)= %f, %f\n", c[X], c[Y]);
    */
    return(0);
}
