/*
 *	S T A N D A R D   H E A D E R	F I L E
 *
 *		M A T H   P A C K A G E
 *
 *	    Max R. Dursteler, Rockville Md.
 */

        /* Functions */

extern double fabs(), floor(double), ceil(double), fmod(double);
extern double sqrt(double), exp(double), exp2(double), log(double), log10(double ), pow(double, double), pw10(double);

extern double sin(double), cos(), tan(), asin(double), acos(), atan(double);

struct complex {     /* structure for complex numbers */
        double rl;
        double im;
};
        /* Usefull constants */

#define BLKSIZE 512
#define MAXINT	32767
#define BIG	72057594037927936.		/* Maximum precision of DOUBLE */
#define HUGE	1.701411733192644270e38 	/* Largest DOUBLE */
#define LOGHUGE 39				/* 10^39 --> Largest power */
#define LN2	0.69314718055994530941
#define LN10	2.30258509299404568401
#define E	2.7182818284590452353602874
#define SQRT2	1.41421356237309504880
#define HALFSQRT2	.70710678118654752440
#define TWOPI	6.28318530717958647692
#define RADPDEG 0.01745329251994329576

                /* macros */
#define PI	3.141592653589793238462643
#define QUARTPI 0.78539816339744830962
#define HALFPI	1.57079632679489661923

#define sqr(x)	((x) * (x))
#define sgn(x)	((x) < 0 ? -1 : 1)
#define xswap(a,b) a ^= b, b ^= a, a ^= b
#define max(x,y)	((x) >= (y) ? (x) : (y))
#define min(x,y)	((x) <= (y) ? (x) : (y))
#define abs(x)		((x)<0? -(x):(x))	/* Integer Absolute value */
#define fabs(x) 	((x)<0.? -(x):(x))	/* Floating absolute value */
#define mod(x,y)	((x)%(y))		/* Integer modulo */
#define logE(x) 	(log(x))		/* Natural log */
#define ln(x)		(log(x))		/* Natural log */
#define cos(x)		(sin(x+HALFPI)) 	/* Cosinus [radians] */
#define tan(x)		(sin(x)/cos(x)) 	/* Tangens [radians] */

/* End of math.h */
