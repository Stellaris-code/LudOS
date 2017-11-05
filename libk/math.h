/*
 *	S T A N D A R D   H E A D E R	F I L E
 *
 *		M A T H   P A C K A G E
 *
 *	    Max R. Dursteler, Rockville Md.
 */
#pragma once

        /* Functions */

#pragma GCC system_header

#ifdef __cplusplus
extern "C"
{
#endif

extern double fabs(), floor(double), ceil(double), fmod(double);
extern double sqrt(double), exp(double), exp2(double), math_log(double), log10(double ), pow(double, double), pw10(double);

extern double sin(double), cos(), tan(), asin(double), acos(), atan(double);

struct complex {     /* structure for complex numbers */
        double rl;
        double im;
};
        /* Usefull constants */

#define M_BLKSIZE 512
#define M_MAXINT	32767
#define M_BIG	72057594037927936.		/* Maximum precision of DOUBLE */
#define M_HUGE	1.701411733192644270e38 	/* Largest DOUBLE */
#define M_LOGHUGE 39				/* 10^39 --> Largest power */
#define M_LN2	0.69314718055994530941
#define M_LN10	2.30258509299404568401
#define M_E	2.7182818284590452353602874
#define M_SQRT2	1.41421356237309504880
#define M_HALFSQRT2	.70710678118654752440
#define M_TWOPI	6.28318530717958647692
#define M_RADPDEG 0.01745329251994329576

                /* macros */
#define M_PI	3.141592653589793238462643
#define M_QUARTPI 0.78539816339744830962
#define M_HALFPI	1.57079632679489661923

#define sqr(x)	((x) * (x))
#define sgn(x)	((x) < 0 ? -1 : 1)
#define xswap(a,b) a ^= b, b ^= a, a ^= b
//#define max(x,y)	((x) >= (y) ? (x) : (y))
//#define min(x,y)	((x) <= (y) ? (x) : (y))
#define abs(x)		((x)<0? -(x):(x))	/* Integer Absolute value */
#define fabs(x) 	((x)<0.? -(x):(x))	/* Floating absolute value */
#define mod(x,y)	((x)%(y))		/* Integer modulo */
#define logE(x) 	(log(x))		/* Natural log */
#define ln(x)		(log(x))		/* Natural log */
#define cos(x)		(sin(x+M_HALFPI)) 	/* Cosinus [radians] */
#define tan(x)		(sin(x)/cos(x)) 	/* Tangens [radians] */

#ifdef __cplusplus
}
#endif

/* End of math.h */
