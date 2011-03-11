/*
 *	globals.h
 *	Defines global variables needed by cTraceo.
*/
#pragma once
#include <inttypes.h>	//needed for inequivocal type qualifiers
#include <complex.h>
/*
	NOTE:	The actual value defined is not important, as it will never be use -only its name will.
*/

/********************************************************************************
 * Configuration:																*
 *******************************************************************************/
#define VERBOSE				0	//when set to 1, more information will be shown.
#define VERBOSITY			3	//verbosity level (0-10). High levels will make the code impractically slow (seriously!)
#define MAX_LINE_LEN		256	//Maximum number of chars to read at once from a file (\n not included)
#define MEM_FACTOR			20	//The memory allocated for each ray is determined like so: ( abs(rbox2-rbox1)/dsi )* MEM_FACTOR
								//NOTE:	for deepwater cases, values between 3-5 are ok.
								//		for shallow water, or other cases with a lot of reflections,
								//		values of 15-25 may be adequate
#define KEEP_RAYS_IN_MEM	0	//[boolean] determines whether a rays coordinates are kept in memory after being written to the .mat file. (mat become usefull for multiprocessing)

//basics:
#define	TRUE	1
#define FALSE	0

//debugging help:
#define WHERESTR				"[%s,\tline %d]:\t"
#define WHEREARG				__FILE__, __LINE__
#define DEBUG(level, ...)		if(VERBOSE == TRUE && VERBOSITY >= level){fprintf(stderr, WHERESTR, WHEREARG);fprintf(stderr, __VA_ARGS__);}
//TODO rewrite DEBUG so that it doesn't appear in code when VERBOSE is undefined

/********************************************************************************
 * Minor data structures.														*
 *******************************************************************************/

typedef struct	vector{
	float	r;	//range component of vector
	float	z;	//depth component of vector
}vector_t;		//TODO add position components

typedef struct	point{
	float	r;	//range component of point
	float	z;	//depth component of point
}point_t;


/********************************************************************************
 * Output data structures.														*
 *******************************************************************************/

typedef struct	ray{
	/*
	 * NOTE: memory ocupied is 44B overhead + 96B per ray coordinate. TODO recalculate, as this has since become larger
	 */
	uintptr_t		nCoords;
	uintptr_t		iKill;		//indicates if ray has been "killed"
	float			theta;		//launching angle of the ray
	float			rMin, rMax;	//used to determine if a ray "turns back"
	uintptr_t		iReturn;	//indicates if a ray "turns back"
	float*			r;			//range of ray at index
	float*			z;			//depth of ray at index
	float*			c;			//speed of sound at index
	uint32_t*		iRefl;		//indicates if there is a reflection at a certain index of the ray coordinates.
	uint32_t		sRefl;		//number of surface reflections
	uint32_t		bRefl;		//number of bottom reflections
	uint32_t		oRefl;		//number of object reflections
	uint32_t		nRefl;		//number of total reflections
	complex float*	decay;		//decay of ray
	float*			phase;		//ray phase
	float*			tau;		//acumulated travel time
	float*			s;			//acumulated distance travelled by the ray
	float*			ic;			//see Chapter 3 of Traceo Manual
	vector_t*		boundaryTg;	//"tbdry" a boundary's tangent vector
	int32_t*		boundaryJ;	//"jbdry",	indicates at what boundary a ray is (-1 => above surface; 1 => below bottom)
	uintptr_t		nRefrac;	//"nrefr", number of refraction points
	point_t*		refrac;		//"rrefr, zrefr", coordinates of refraction points. used in "solveEikonalEq.c"

	float*			p;			//used in solveDynamicEq
	float*			q;			//used in solveDynamicEq
	float*			caustc;		//used in solveDynamicEq
	complex float*	amp;		//ray amplitude
}ray_t;



/********************************************************************************
 * Structures that contain the settings that are read from the input file.		*
 *******************************************************************************/
 
typedef struct source{
	float		ds;				//ray step
	float		rx,zx;			//source coords
	float		rbox1, rbox2;	//the box that limits the range of the rays
	float		freqx;			//source frequency
	uint32_t	nThetas;		//number of launching angles
	float		theta1, thetaN;	//first and last launching angle
	float		dTheta;			//the increment between launching angles
	float*		thetas;			//the array that will actually contain the launching angles (is allocated in "readin.c")
}source_t;

/** TODO can safely be removed
typedef struct interfaceProperties{
	*
	 * Contains the properties of the surface or bottom interfaces. used in "interface" structs
	 * See page 39 of "Traceo" manual.
	 *
	float		cp;		//"cpati",	compressional speed
	float		cs;		//"csati",	shear speed
	float		rho;	//"rhoati",	density
	float		ap;		//"apati",	compressional attenuation
	float		as;		//"asati"	shear attenuation
}interfaceProperties_t;
*/

typedef struct interface{
	/*
		Used for both the "bathymetry" as well as "altimetry" block
	*/
	//See #defines following this block for possible values
	uint32_t				surfaceType;			//formerly "atype"
	uint32_t				surfacePropertyType;	//formerly "aptype"
	float*					r;						//"rati(n)"				|
	float*					z;						//"zati(n)"				 }	these pointers are mallocced in "readin.c"
	float*					cp;						//"cpati",	compressional speed
	float*					cs;						//"csati",	shear speed
	float*					rho;					//"rhoati",	density
	float*					ap;						//"apati",	compressional attenuation
	float*					as;						//"asati"	shear attenuation
	uint32_t				surfaceInterpolation;	//formerly "aitype"
	uint32_t				surfaceAttenUnits;		//formerly "atiu"
	uint32_t				numSurfaceCoords;		//formerly "nati"
}interface_t;

//possible values for surfaceType (see page 38, TraceO manual):
#define SURFACE_TYPE__ABSORVENT	1	//formerly "A"
#define SURFACE_TYPE__ELASTIC	2	//formerly "E"
#define	SURFACE_TYPE__RIGID		3	//formerly "R"
#define SURFACE_TYPE__VACUUM	4	//formerly "V". Vacuum over surface

//possible values for surfacePropertyType (see page 38, Traceo manual):
#define	SURFACE_PROPERTY_TYPE__HOMOGENEOUS		5	//formerly "H"
#define SURFACE_PROPERTY_TYPE__NON_HOMOGENEOUS	6	//formerly "N"

//possible values for surfaceInterpolation (see page 38, Traceo manual):
#define SURFACE_INTERPOLATION__FLAT		7	//"FL", flat surface
#define SURFACE_INTERPOLATION__SLOPED	8	//"SL", sloped surface
#define SURFACE_INTERPOLATION__2P		9	//"2P", piecewise linear interpolation
#define SURFACE_INTERPOLATION__3P		10	//"3P", piecewise parabolic interpolation
#define SURFACE_INTERPOLATION__4P		11	//"4P", piecewise cubic interpolation

//possible values for attenUnits (see page 39, Traceo manual):
#define	SURFACE_ATTEN_UNITS__dBperkHz		12	//"F",	dB/kHz
#define	SURFACE_ATTEN_UNITS__dBperMeter		13	//"M",	dB/meter
#define	SURFACE_ATTEN_UNITS__dBperNeper		14	//"N",	dB/neper
#define	SURFACE_ATTEN_UNITS__qFactor		15	//"Q",	Q factor
#define	SURFACE_ATTEN_UNITS__dBperLambda	16	//"W",	dB/<lambda>


typedef struct soundSpeed{
	uint32_t	cDist;			//"cdist", type of sound speed distribution
	uint32_t	cClass;			//"cclass", class of sound speed
	uint32_t	nr0, nz0;		//"nr0,nz0", number of points in range and depth
	float*		z0;				//"z0",	depth
	float*		r0;				//"r0", range
	float*		c01d;			//"c0", sound speed at (z0)
	float**	c02d;			//"c02d", sound speed at (r0,z0)
}soundSpeed_t;

//possible values for cDistribuition (see page 39, Traceo Manual)
#define	C_DIST__PROFILE		17	//"c(z,z)", sound speed profile c=c(z)
#define	C_DIST__FIELD		18	//"c(r,z)", sound speed field c=c(r,z)

//possible values for cClass	(pages 39,40, Traceo Manual)
#define	C_CLASS__ISOVELOCITY		19	//"ISOV"
#define C_CLASS__LINEAR				20	//"LINP"
#define C_CLASS__PARABOLIC			21	//"PARP"
#define C_CLASS__EXPONENTIAL		22	//"EXPP"
#define C_CLASS__N2_LINEAR			23	//"N2LP", n^2 linear
#define C_CLASS__INV_SQUARE			24	//"ISQP", inverse-square gradient
#define C_CLASS__MUNK				25	//"MUNK"
#define C_CLASS__TABULATED			26	//"TBAL"


typedef struct object{
	/*
	 * contains all information regarding a specific object.
	 */
	uint32_t				surfaceType;			//"otype",		Object surface type (rigid, aboservent, etc), as defined for interface_t
	uint32_t				surfaceAttenUnits;		//"obju",		attenuation units, as define for interface_t
	float					cp;						//"cpati",		compressional speed
	float					cs;						//"csati",		shear speed
	float					rho;					//"rhoati",		density
	float					ap;						//"apati",		compressional attenuation
	float					as;						//"asati",		shear attenuation
	uint32_t				nCoords;				//"no",			number of coordinates
	float*					r;						//"ro"		|
	float*					zDown;					//"zdn"		 >	actual coordinates that define the object geometry
	float*					zUp;					//"zup"		|
}object_t;

typedef struct objects{
	/*
	 * contains common object information as well as a pointer to an array with the actual object information.
	 */
	uint32_t	numObjects;				//"nobj"
	uint32_t	surfaceInterpolation;	//"oitype", Object interpolation type
	object_t*	object;
}objects_t;

typedef struct output{
	uint32_t	calcType;			//"catype"
	uint32_t	arrayType;			//"artype"
	uint32_t	nArrayR, nArrayZ;	//"nra", "nrz"
	float*		arrayR;				//"nra"			Array size in R
	float*		arrayZ;				//"nrz"			Array size in Z
	float		miss;				//"miss"		threshold for finding eigenrays
}output_t;

//possible values for calculationType (see page 43)
#define	CALC_TYPE__RAY_COORDS				27	//"RCO"
#define	CALC_TYPE__ALL_RAY_INFO				28	//"ARI"
#define	CALC_TYPE__EIGENRAYS_REG_FALSI		29	//"ERF", write Eigenrays (use Regula Falsi method)
#define	CALC_TYPE__EIGENRAYS_PROXIMITY		30	//"EPR", write Eigenrays (use Proximity method)
#define	CALC_TYPE__AMP_DELAY_REG_FALSI		31	//"ADR", write Amplitudes and Delays (use Regula Falsi method)
#define	CALC_TYPE__AMP_DELAY_PROXIMITY		32	//"ADP", write Amplitudes and Delays (use Proximity method)
#define	CALC_TYPE__COH_ACOUS_PRESS			33	//"CPR", write Coherent Acoustic Pressure
#define	CALC_TYPE__COH_TRANS_LOSS			34	//"CTL", write Coherent Transmission loss
#define	CALC_TYPE__PART_VEL					35	//"PVL", write Coherent Particle Velocity
#define	CALC_TYPE__COH_ACOUS_PRESS_PART_VEL	36	//"PAV", write Coherent Acoustic Pressure asn Particle Velocity

//possible values for arrayType (Manual page 43)
#define ARRAY_TYPE__RECTANGULAR		37	//"RRY"
#define ARRAY_TYPE__HORIZONTAL		38	//"HRY"
#define ARRAY_TYPE__VERTICAL		39	//"VRY"
#define ARRAY_TYPE__LINEAR			40	//"LRY"


typedef struct settings {
	/*
	 * Contains all input information
	 */
	char*			cTitle;
	source_t		source;
	interface_t		altimetry;
	soundSpeed_t	soundSpeed;
	objects_t		objects;
	interface_t		batimetry;
	output_t		output;
}settings_t;

