/****************************************************************************************
 *  globals.h                                                                           *
 *  Defines data structures, constants and macros needed by cTraceo.                    *
 *                                                                                      *
 * ------------------------------------------------------------------------------------ *
 * License: This file is part of the cTraceo Raytracing Model and is released under the *
 *          Creative Commons Attribution-NonCommercial-ShareAlike 3.0 Unported License  *
 *          http://creativecommons.org/licenses/by-nc-sa/3.0/                           *
 *                                                                                      *
 * NOTE:    cTraceo is research code under active development.                          *
 *          The code may contain bugs and updates are possible in the future.           *
 *                                                                                      *
 * Written for project SENSOCEAN by:                                                    *
 *          Emanuel Ey                                                                  *
 *          emanuel.ey@gmail.com                                                        *
 *          Copyright (C) 2011                                                          *
 *          Signal Processing Laboratory                                                *
 *          Universidade do Algarve                                                     *
 *                                                                                      *
 ****************************************************************************************/

#pragma once
#include <stdint.h>     //needed for inequivocal type qualifiers as defined in C99
#include <stdbool.h>    //for boolean data type
#include <complex.h>


/********************************************************************************
 * Configuration:                                                               *
 ******************************************************** ************************/
//NOTE: VERSION has to be 20 chars wide so that the header and help texts are printed correctly
//              "----5---10---15---20"
#define VERSION "1.1 SP              "
//NOTE: HEADER is made up of consecutive strings, each 79 chars long:
#define HEADER  "* =========================================================================== *\n""*          The cTraceo Acoustic Raytracing Model, Version "VERSION"*\n""*  Released under the Creative Commons Attribution-NonCommercial-ShareAlike   *\n""*  3.0 Unported License ( http://creativecommons.org/licenses/by-nc-sa/3.0/ ) *\n""* --------------------------------------------------------------------------- *\n""*         Copyright (C) 2011, 2012 Emanuel Ey <emanuel.ey@gmail.com>          *\n""*        Copyright (C) 2010 Orlando Camargo Rodriguez <orodrig@ualg.pt>       *\n""*     SiPLab, Universidade do Algarve, Portugal <www.siplab.fct.ualg.pt>      *\n""* =========================================================================== *\n\n"



 
#ifndef VERBOSE
    #define VERBOSE                 0       //when set to 1, more information will be shown.
#endif              
#define VERBOSITY                   3       //verbosity level (0-10). High levels will make the code impractically slow (seriously!)
#define MAX_LINE_LEN                256     //Maximum number of chars to read at once from a file (\n not included)
#define MEM_FACTOR                  20      //The memory allocated for each ray is determined like so: ( abs(rbox2-rbox1)/dsi )* MEM_FACTOR
                                            //NOTE: for deepwater cases, values between 3-5 are ok.
                                            //      for shallow water, or other cases with a lot of reflections,
                                            //      values of 15-25 may be adequate
#define KEEP_RAYS_IN_MEM            0       //[boolean] determines whether a ray's coordinates are kept in memory after being written to the .mat file. (mat become usefull for multiprocessing)
#define MIN_REFLECTION_COEFFICIENT  1.0e-15 //used in solveEikonalEq(). When a rays reflection coeff is below this threshold, it is killed.



/********************************************************************************
 * ------------------------------ MATLAB -------------------------------------- *
 * Some data types vary between matlab versions, so we have to define           *
 * their type according to the matlab version used during compilation.          *
 * NOTE: To choose a specific matlab version for compilatio, edit the Makefile. *
 ********************************************************************************/
#if USE_MATLAB == 1
    #define R12     0
    #define R14     1
    #define R2007A  2
    #define R2007B  3
    #define R2008A  4
    #define R2008B  5
    #define R2010B  6


    //When writing matlab-matrixes to matfiles, the data type varies between matlab versions.
    #if MATLAB_VERSION == R12 || MATLAB_VERSION == R14
        //matlab R14 uses int32
        #define MWSIZE          int32_t
        #define MWINDEX         int32_t
    #elif   MATLAB_VERSION == R2007A || MATLAB_VERSION == R2007B || MATLAB_VERSION == R2008A || MATLAB_VERSION == R2008B || MATLAB_VERSION == R2010B
        //matlab R2008b and others use mwSize and mwIndex which defined in "matrix.h"
        #define MWSIZE          mwSize      
        #define MWINDEX         mwIndex
    #endif
#else
    #define MWSIZE          uintptr_t
    #define MWINDEX         uintptr_t
#endif

/********************************************************************************
 * Some utilities                                                               *
 ********************************************************************************/

//the following 5 lines are used to simplify access to star pressure elements (for particle velocity)
#define LEFT    0
#define CENTER  1
#define RIGHT   2

#define TOP     0
#define BOTTOM  2

#define LINUX   1
#define WINDOWS 2

//function macro used for showing debugging information:
#if VERBOSE == 1
    #define WHERESTR                "[%s,\tline %d]:\t"
    #define WHEREARG                __FILE__, __LINE__
    #define DEBUG(level, ...)       if(VERBOSITY >= level){fprintf(stderr, WHERESTR, WHEREARG);fprintf(stderr, __VA_ARGS__);}
#else
    //this should be optimized away nicely by the compiler:
    #define DEBUG(level, ...)       {}
#endif


/********************************************************************************
 * Minor data structures.                                                       *
 ********************************************************************************/

//TODO this seems somewhat redundant...
typedef struct  vector{
    float  r;  //range component of vector
    float  z;  //depth component of vector
}vector_t;      //TODO add position components?

typedef struct  point{
    float  r;  //range component of point
    float  z;  //depth component of point
}point_t;


/********************************************************************************
 * Output data structures.                                                      *
 *******************************************************************************/

typedef struct  ray{
    /*
     * NOTE: memory ocupied is 44B overhead + 96B per ray coordinate. TODO recalculate, as this has since become larger
     */
    uintptr_t       nCoords;
    bool            iKill;      //indicates if ray has been "killed"
    float          theta;      //launching angle of the ray
    float          rMin, rMax; //used to determine if a ray "turns back"
    bool            iReturn;    //indicates if a ray "turns back"
    float*         r;          //range of ray at index
    float*         z;          //depth of ray at index
    float*         c;          //speed of sound at index
    bool*           iRefl;      //indicates if there is a reflection at a certain index of the ray coordinates.
    uint32_t        sRefl;      //number of surface reflections
    uint32_t        bRefl;      //number of bottom reflections
    uint32_t        oRefl;      //number of object reflections
    uint32_t        nRefl;      //number of total reflections
    complex float* decay;      //decay of ray
    float*         phase;      //ray phase
    float*         tau;        //acumulated travel time
    float*         s;          //acumulated distance travelled by the ray
    float*         ic;         //see Chapter 3 of Traceo Manual
    vector_t*       boundaryTg; //"tbdry" a boundary's tangent vector
    int32_t*        boundaryJ;  //"jbdry",  indicates at what boundary a ray is (-1 => above surface; 1 => below bottom)
    uint32_t        nRefrac;    //"nrefr", number of refraction points
    //point_t*      refrac;     //"rrefr, zrefr", coordinates of refraction points. used in "solveEikonalEq.c"
    float*         rRefrac;
    float*         zRefrac;

    float*         p;          //used in solveDynamicEq
    float*         q;          //used in solveDynamicEq
    float*         caustc;     //used in solveDynamicEq
    complex float* amp;        //ray amplitude
}ray_t;



/********************************************************************************
 * Structures that contain the settings that are read from the input file.      *
 *******************************************************************************/
 
typedef struct source{
    float      ds;             //ray step
    float      rx,zx;          //source coords
    float      rbox1, rbox2;   //the box that limits the range of the rays
    float      freqx;          //source frequency
    uint32_t    nThetas;        //number of launching angles
    float      theta1, thetaN; //first and last launching angle
    float      dTheta;         //the increment between launching angles
    float*     thetas;         //the array that will actually contain the launching angles (is allocated in "readin.c")
}source_t;

typedef struct interface{
    /*
        Used for both the "batimetry" as well as "altimetry" block
    */
    //See #defines following this block for possible values
    uint32_t                surfaceType;            //formerly "atype"
    uint32_t                surfacePropertyType;    //formerly "aptype"
    float*                 r;                      //"rati(n)"             |
    float*                 z;                      //"zati(n)"              }  these pointers are mallocced in "readin.c"
    float*                 cp;                     //"cpati",  compressional speed
    float*                 cs;                     //"csati",  shear speed
    float*                 rho;                    //"rhoati", density
    float*                 ap;                     //"apati",  compressional attenuation
    float*                 as;                     //"asati"   shear attenuation
    uint32_t                surfaceInterpolation;   //formerly "aitype"
    uint32_t                surfaceAttenUnits;      //formerly "atiu"
    uint32_t                numSurfaceCoords;       //formerly "nati"
}interface_t;

//possible values for surfaceType (see page 38, Traceo manual):
#define SURFACE_TYPE__ABSORVENT 1   //formerly "A"
#define SURFACE_TYPE__ELASTIC   2   //formerly "E"
#define SURFACE_TYPE__RIGID     3   //formerly "R"
#define SURFACE_TYPE__VACUUM    4   //formerly "V". Vacuum over surface

//possible values for surfacePropertyType (see page 38, Traceo manual):
#define SURFACE_PROPERTY_TYPE__HOMOGENEOUS      5   //formerly "H"
#define SURFACE_PROPERTY_TYPE__NON_HOMOGENEOUS  6   //formerly "N"

//possible values for surfaceInterpolation (see page 38, Traceo manual):
#define SURFACE_INTERPOLATION__FLAT     7   //"FL", flat surface
#define SURFACE_INTERPOLATION__SLOPED   8   //"SL", sloped surface
#define SURFACE_INTERPOLATION__2P       9   //"2P", piecewise linear interpolation
#define SURFACE_INTERPOLATION__3P       10  //"3P", piecewise parabolic interpolation
#define SURFACE_INTERPOLATION__4P       11  //"4P", piecewise cubic interpolation

//possible values for attenUnits (see page 39, Traceo manual):
#define SURFACE_ATTEN_UNITS__dBperkHz       12  //"F",  dB/kHz
#define SURFACE_ATTEN_UNITS__dBperMeter     13  //"M",  dB/meter
#define SURFACE_ATTEN_UNITS__dBperNeper     14  //"N",  dB/neper
#define SURFACE_ATTEN_UNITS__qFactor        15  //"Q",  Q factor
#define SURFACE_ATTEN_UNITS__dBperLambda    16  //"W",  dB/<lambda>


typedef struct soundSpeed{
    uint32_t    cDist;          //"cdist", type of sound speed distribution -profile or field (i.e. range dependent)
    uint32_t    cClass;         //"cclass", class of sound speed
    uint32_t    nr, nz;         //"nr0,nz0", number of points in range and depth
    float*     z;              //"z0", depth
    float*     r;              //"r0", range
    float*     c1D;            //"c0", sound speed at (z0)
    float**    c2D;            //"c02d", sound speed at (r0,z0)
}soundSpeed_t;

//possible values for cDistribuition (see page 39, Traceo Manual)
#define C_DIST__PROFILE     17  //"c(z,z)", sound speed profile c=c(z)
#define C_DIST__FIELD       18  //"c(r,z)", sound speed field c=c(r,z)

//possible values for cClass    (pages 39,40, Traceo Manual)
#define C_CLASS__ISOVELOCITY        19  //"ISOV"
#define C_CLASS__LINEAR             20  //"LINP"
#define C_CLASS__PARABOLIC          21  //"PARP"
#define C_CLASS__EXPONENTIAL        22  //"EXPP"
#define C_CLASS__N2_LINEAR          23  //"N2LP", n^2 linear
#define C_CLASS__INV_SQUARE         24  //"ISQP", inverse-square gradient
#define C_CLASS__MUNK               25  //"MUNK"
#define C_CLASS__TABULATED          26  //"TBAL"


typedef struct object{
    /*
     * contains all information regarding a specific object.
     */
    uint32_t                surfaceType;            //"otype",      Object surface type (rigid, aboservent, etc), as defined for interface_t
    uint32_t                surfaceAttenUnits;      //"obju",       attenuation units, as define for interface_t
    float                  cp;                     //"cpati",      compressional speed
    float                  cs;                     //"csati",      shear speed
    float                  rho;                    //"rhoati",     density
    float                  ap;                     //"apati",      compressional attenuation
    float                  as;                     //"asati",      shear attenuation
    uint32_t                nCoords;                //"no",         number of coordinates
    float*                 r;                      //"ro"      |
    float*                 zDown;                  //"zdn"      >  actual coordinates that define the object geometry
    float*                 zUp;                    //"zup"     |
}object_t;

typedef struct objects{
    /*
     * contains common object information as well as a pointer to an array with the actual object information.
     */
    uint32_t    numObjects;             //"nobj"
    uint32_t    surfaceInterpolation;   //"oitype", Object interpolation type
    object_t*   object;
}objects_t;

typedef struct output{
    uint32_t            calcType;           //"catype"
    uint32_t            arrayType;          //"artype"
    uint32_t            nArrayR, nArrayZ;   //"nra", "nza"  Array sizes in R and Z
    float*             arrayR;             //"nra"         Array R (ranges)
    float*             arrayZ;             //"nrz"         Array Z (depths)
    //complex float*       pressure1D;         //will contain coherent acoustic pressure at each array element (1D), calculated in "calcCohAcoustPress.c"
    complex float**    pressure2D;         //will contain coherent acoustic pressure at each array element (2D), calculated in "calcCohAcoustPress.c"
    complex float      (**pressure_H)[3];      //used when calculating particle velocity (pressure at left, center, right)
    complex float      (**pressure_V)[3];      //used when calculating particle velocity (pressure at top, center, bottom)
                                                /* Note the redundancy: the center pressure is present in both cases.
                                                 *                      This is done for performance reasons.
                                                 */
    float              dr, dz;             //horizontal and vertical offset of the star pressure elements
    float              miss;               //"miss"        distance threshold for finding eigenrays
}output_t;

//possible values for calculationType (see page 43)
#define CALC_TYPE__RAY_COORDS               27  //"RCO"
#define CALC_TYPE__ALL_RAY_INFO             28  //"ARI"
#define CALC_TYPE__EIGENRAYS_REG_FALSI      29  //"ERF", write Eigenrays (use Regula Falsi method)
#define CALC_TYPE__EIGENRAYS_PROXIMITY      30  //"EPR", write Eigenrays (use Proximity method)
#define CALC_TYPE__AMP_DELAY_REG_FALSI      31  //"ADR", write Amplitudes and Delays (use Regula Falsi method)
#define CALC_TYPE__AMP_DELAY_PROXIMITY      32  //"ADP", write Amplitudes and Delays (use Proximity method)
#define CALC_TYPE__COH_ACOUS_PRESS          33  //"CPR", write Coherent Acoustic Pressure
#define CALC_TYPE__COH_TRANS_LOSS           34  //"CTL", write Coherent Transmission loss
#define CALC_TYPE__PART_VEL                 35  //"PVL", write Coherent Particle Velocity
#define CALC_TYPE__COH_ACOUS_PRESS_PART_VEL 36  //"PAV", write Coherent Acoustic Pressure and Particle Velocity

//possible values for arrayType (Manual page 43)
#define ARRAY_TYPE__RECTANGULAR     37  //"RRY"
#define ARRAY_TYPE__HORIZONTAL      38  //"HRY"
#define ARRAY_TYPE__VERTICAL        39  //"VRY"
#define ARRAY_TYPE__LINEAR          40  //"LRY"


typedef struct settings {
    /*
     * Contains all input information
     */
    char*           cTitle;
    source_t        source;
    interface_t     altimetry;
    soundSpeed_t    soundSpeed;
    objects_t       objects;
    interface_t     batimetry;
    output_t        output;
}settings_t;


/********************************************************************************
 * Structures that contain the results which will be written to matfiles.       *
 *******************************************************************************/

//#define USE_MATLAB 0

#if USE_MATLAB == 1
    #include <matrix.h>     //needed for matlab matrix dimensions (mwSize)
#else
    #include "matOut/matOut.h"
#endif




typedef struct arrivals{
    /*
     * Used in calcAmpDelxx to temporarily contain arrivals before they are written to 
     * a matlab structure at the end of the function.
     */
    float          nArrivals;
    mxArray*        mxArrivalStruct;
}arrivals_t;

typedef struct eigenrays{
    /*
     * Similar to the above, but used in calcEigenrayXX.
     * Only difference is in variable naming.
     */
    float          nEigenrays;
    mxArray*        mxEigenrayStruct;
}eigenrays_t;
    


