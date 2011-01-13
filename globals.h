/*
 *	globals.h
 *	Defines global variables needed by cTraceo.
*/
#pragma once
#include <inttypes.h>	//needed for inequivocal type qualifiers

/*
	NOTE:	The actual value defined is not important, as it will never be use -only its name will.
*/

/********************************************************************************
 * Configuration:																*
 *******************************************************************************/
#define VERBOSE			1	//when set to 1, more information will be shown.
#define MAX_LINE_LEN	256	//Maximum number of chars to read at once from a file (\n not included)
//Previuously define in globli.h as:
//	parameter (inpfil=1,outfil=2,prtfil=3)
//	parameter (nangle=10000,nc0=5001,nc02d=1001,nbdry=5001)
//	parameter (nhyd=5001,nhyd2=1001,nmaxo=21,npo=201,np=20001,np2=201)
#define MAX_NUM_ANGLES	10000


//basics:
#define	TRUE	1
#define FALSE	0



/********************************************************************************
 * Structures that contains the settings that are read from the input file.		*
 *******************************************************************************/
 
typedef struct source{
	double		ds;				//ray step
	double		rx,zx;			//source coords
	double		rbox1, rbox2;	//the box that limits the range of the rays
	double		freqx;			//source frequency
	uint64_t	nThetas;		//number of launching angles
	double		theta1, thetaN;	//first and last launching angle
	double*		thetas;			//the array that will actually contain the launching angles (is allocated in "readin.c")
}source_t;

typedef struct surface{
	/*
		Used for both the "bathymetry" as well as "altimetry" block
	*/
	//See #defines following this block for possible values
	int64_t		surfaceType;			//formerly "atype"
	int64_t		surfaceProperties;		//formerly "aptype"
	int64_t		surfaceInterpolation;	//formerly "aitype"
	int64_t		surfaceAttenUnits;		//formerly "atiu"
	int64_t		numSurfaceCoords;		//formerly "nati"
	//TODO inlude a structure for the actual surface properties in struct _surface?
}surface_t;

//possible values for surfaceType (see page 38, TraceO manual):
#define SURFACE_TYPE__ABSORVENT	1	//formerly "A"
#define SURFACE_TYPE__ELASTIC	2	//formerly "E"
#define	SURFACE_TYPE__RIGID		3	//formerly "R"
#define SURFACE_TYPE__VACUUM	4	//formerly "V". Vacuum over surface

//possible values for surfaceProperties (see page 38, Traceo manual):
#define	SURFACE_PROPERTIES__HOMOGENEOUS		5	//formerly "H"
#define SURFACE_PROPERTIES__NON_HOMOGENEOUS	6	//formerly "N"

//possible values for surfaceInterpolation (see page 38, Traceo manual):
#define SURFACE_INTERPOLATION__FLAT		7	//"FL", flat surface
#define SURFACE_INTERPOLATION__SLOPED	8	//"SL", sloped surface
#define SURFACE_INTERPOLATION__2P		9	//"SL", piecewise linear interpolation
#define SURFACE_INTERPOLATION__3P		10	//"SL", piecewise parabolic interpolation
#define SURFACE_INTERPOLATION__4P		11	//"SL", piecewise cubic interpolation

//possible values for attenUnits (see page 39, Traceo manual):
#define	SURFACE_ATTEN_UNITS__dBperkHz		12	//"F",	dB/kHz
#define	SURFACE_ATTEN_UNITS__dBperMeter		13	//"M",	dB/meter
#define	SURFACE_ATTEN_UNITS__dBperNeper		14	//"N",	dB/neper
#define	SURFACE_ATTEN_UNITS__qFactor		15	//"q",	Q factor
#define	SURFACE_ATTEN_UNITS__dBperLambda	16	//"W",	dB/<lambda>


typedef struct soundSpeed{
	int64_t		cDistribution;			//"cdist", type of sound speed distribution
	int64_t		cClass;					//"cclass", class of sound speed
	int64_t		nr0, nz0;				//"nr0,nz0", number of point in range and depth
}soundSpeed_t;

//possible values for cDistribuition (see page 39, Traceo Manual)
#define	C_DISTRIBUTION__PROFILE		17	//"c(z,z)", sound speed profile c=c(z)
#define	C_DISTRIBUTION__FIELDS		18	//"c(r,z)", sound speed field c=c(r,z)

//possible values for cClass	(pages 39,40, Traceo Manual)
#define	C_CLASS__ISOVELOCITY		19	//"ISOV"
#define C_CLASS__LINEAR				20	//"LINP"
#define C_CLASS__PARABOLIC			21	//"PARP"
#define C_CLASS__EXPONENTIAL		22	//"EXPP"
#define C_CLASS__N2_LINEAR			23	//"N2LP", n^2 linear
#define C_CLASS__INV_SQUARE			24	//"ISQP", inverse-square gradient
#define C_CLASS__MUNK				25	//"MUNK"
#define C_CLASS__TABULATED			26	//"TBAL"


/*
typedef struct _object{
	//TODO: define _object structure (see pages 42,43)
}
*/
typedef struct output{
	int64_t	calcType;	//"catype"
	int64_t	arrayType;			//"artype"
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
#define	CALC_TYPE__COH_PART_VEL				35	//"PVL", write Coherent Particle Velocity
#define	CALC_TYPE__COH_ACOUS_PRESS_PART_VEL	36	//"PAV", write Coherent Acoustic Pressure asn Particle Velocity

//possible values for arrayType (Manual page 43)
#define ARRAY_TYPE__RECTANGULAR		37	//"RRY"
#define ARRAY_TYPE__HORIZONTAL		38	//"HRY"
#define ARRAY_TYPE__VERTICAL		39	//"VRY"
#define ARRAY_TYPE__LINEAR			40	//"LRY"


typedef struct settings {
	char*			cTitle;
	source_t		source;
	surface_t		altimetry;
	soundSpeed_t	soundSpeed;
//	_object			object;
	surface_t		bathymetry;
	output_t		output;
}settings_t;


/*
 * The c language doesn't support global variables across several files, so this is
 * a (somewhat ugly) workaround.
 * The structure "global" shall contain all non-local variables and be passed by pointer.
 */
typedef struct globals{
	settings_t		settings;
}globals_t;


