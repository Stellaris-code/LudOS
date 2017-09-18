/*****************************************************************************
 *									     *
 * MATH.C  Mathematical subroutines for use with Lattice or Microsoft-	     *
 *	   compiler.							     *
 *	   By Max R. D^Arsteler						     *
 *									     *
 *****************************************************************************
 */

/*****************************************************************************
 *  This file contains the combined source files of the most
 *  important mathe- matical formulas written for the Lattice C Compiler.
 *  It makes no use of the 8087.  Some of the functions make use of the
 *  internal binary representation of double precision numbers.  As the
 *  internal data representation may vary between different compilers,
 *  watch out for errors e.g.  in the power function.  The function values
 *  are calculated through polynominal or rational approximations.  The
 *  results are for allmost all functions precise for the first 9 to 10
 *  decimal digits.  For still higher precision or checking of values use
 *  functions from file mathf.c (not available here), which contains
 *  Taylor's expansions series.  As this requires many iterations with
 *  multiplications and divisions, the calculation time is too long for use
 *  in regular programs.  In order to use these functions, a program has to
 *  include the header file "math.h".  Furthermore, either the compiled
 *  version (math.obj) or a library with the compiled functions has to be
 *  linked to the program.  As exemples see included testprograms
 *  "tstsin.c" and "tstpow.c" To compile:  "mcb tstsin <CR>", to link:"link
 *  c+%1+math.obj, %1, ,mc <CR>".  I recommend to compile the source of
 *  each function seperately with your personal compiler and use the MS-DOS
 *  librarian to build a library of just the functions you need.  Some of
 *  the more esotheric functions used on "UNIX" are such as Bessel
 *  functions or the gamma function are not implemented.  Evidently, this
 *  is not a professional implementation.
 *
 *   Rockville, 11/27/83      Max R. D^Arsteler
 *			       12405 Village Sq. Terrace
 *			       Rockville Md. 20852    H 301 984-1168
 ******************************************************************************/

/* SQRT.C
 * Calculates square root
 * Precision: 11.05 E-10
 * Range: 0 to e308
 * Author: Max R. D^Arsteler
 */
double sqrt(double x)
{
        typedef union { 	      /* Makes use of internal data  */
                double d;	      /* representation */
                unsigned u[4];
        } DBL;
        double y, re, p, q;
        unsigned ex;	    /* exponens*/
        DBL *xp;

        xp = (DBL *)&x;
        ex = xp->u[3] & ~0100017; /* save exponens */
        re = ex & 020 ? 1.4142135623730950488 : 1.0;
        ex = ex - 037740 >> 1;
        ex &= ~0100017;
        xp->u[3] &= ~0177760;	  /* erase exponens and sign*/
        xp->u[3] |= 037740;	  /* arrange for mantissa in range 0.5 to 1.0 */
        if (xp->d < .7071067812) {  /* multiply by sqrt(2) if mantissa < 0.7 */
                xp->d *= 1.4142135623730950488;
                if (re > 1.0) re = 1.189207115002721;
                else re = 1.0 / 1.18920711500271;
        }
        p =	       .54525387389085 +   /* Polynomial approximation */
            xp->d * (13.65944682358639 +   /* from: COMPUTER APPROXIMATIONS*/
            xp->d * (27.090122712655   +   /* Hart, J.F. et al. 1968 */
            xp->d *   6.43107724139784));
        q =	      4.17003771413707 +
            xp->d * (24.84175210765124 +
            xp->d * (17.71411083016702 +
            xp->d ));
        xp->d = p / q;
        xp->u[3] = xp->u[3] + ex & ~0100000;
        xp->d *= re;
        return(xp->d);
}


/* EXP2.C
 * Calculates exponens to base 2 using polynomial approximations
 * Precision: 10E-10
 * Range:
 * Author: Max R. D^Arsteler
 */
double exp2(double x)
{
        typedef union {
                double d;
                unsigned u[4];
        } DBL;
        double y, x2, p, q, re;
        int ix;
        DBL *xp, *yp;

        xp = (DBL *)&x;
        y = 0.0;
        yp = (DBL *)&y;
        if(xp->d > 1023.0 || xp->d < -1023.0) return (1E307);
        ix = (int) xp->d;
        yp->u[3] += ix + 1023 << 4;
        yp->u[3] &= ~0100017;
        if ((xp->d -= (double) ix) == 0.0) return(yp->d);
        if (xp->d < 0.0) {
              yp->u[3] -= 1 << 4;
              yp->u[3] &= ~0100017;
              xp->d++;
        }
        if (xp->d >= 0.5) {  /* adjust to range 0-0.5 */
                xp->d -= 0.5;
                re = 1.41421356237309504880;
        }
        else re = 1.0;
        x2 = xp->d * xp->d;
        p = xp->d * (7.2152891511493 +
               x2 *  0.0576900723731);
        q =	    20.8189237930062 +
               x2 ;
        xp->d = (q + p) / (q - p);
        yp->d *= re * xp->d;
        return(yp->d);
}


/* POW.C
 * Calculates y^x
 * Precision:
 * Range: o to big for y, +/-1023 for x
 * Author: Max R. D^Arsteler, 10/2/83
 */

double pow(double y,double x)
{
        typedef union {
                double d;
                unsigned u[4];
        } DBL;
        double z, w, p, p2, q, re;
        unsigned ex;	    /* exponens*/
        int iz;
        DBL *yp, *zp, *wp;

        yp = (DBL *)&y;
        if (yp->d <= 0.0) y = -y;
        z = 0.0;
        zp = (DBL *)&z;
        zp->u[3] = yp->u[3] & ~0100017; /* save exponens */
        iz = (zp->u[3] >> 4)-1023;
        if ((yp->d - zp->d) == 0.0)
                z = (double)iz;
        else {
                yp->u[3] -= ++iz << 4; /* arrange for range 0.5 to 0.99999999999 */
                yp->d *= 1.4142135623730950488;  /* shift for 1/sqrt(2) to sqrt(2) */
                p = (yp->d - 1.0) / (yp->d + 1.0);
                p2 = p * p;
                z = p  * (2.000000000046727 +	/* Polynomial approximation */
                    p2 * (0.666666635059382 +	/* from: COMPUTER APPROXIMATIONS*/
                    p2 * (0.4000059794795   +	/* Hart, J.F. et al. 1968 */
                    p2 * (0.28525381498     +
                    p2 *  0.2376245609 ))));
                z = z * 1.442695040888634 + (double)iz	- 0.5;
        }
        z *= x;
        w = 0.0;
        wp = (DBL *)&w;
        if(zp->d > 1023.0 || zp->d < -1023.0) return (1E307);
        iz = (int) zp->d;
        wp->u[3] += iz + 1023 << 4;
        wp->u[3] &= ~0100017;
        if ((zp->d -= (double) iz) == 0.0) return(wp->d);
        while (zp->d < 0.0) {
              wp->u[3] -= 1 << 4;
              wp->u[3] &= ~0100017;
              zp->d++;
        }
        if (zp->d >= 0.5) {  /* adjust to range 0-0.5 */
                zp->d -= 0.5;
                re = 1.41421356237309504880;
        }
        else re = 1.0;
        p2 = zp->d * zp->d;
        p = zp->d * (7.2152891511493 +
               p2 *  0.0576900723731);
        q =	    20.8189237930062 +
               p2 ;
        zp->d = re * wp->d * (q + p) / (q - p);
        return(zp->d);
}


/* FMOD.C
 * Returns the number f such that x = i*y + f. i is an integer, and
 * 0 <= f < y.
 */
double fmod(double x, double y)
{
        double zint, z;
        int i;

        z = x / y;
        zint = 0.0;
        while (z > 32768.0) {
                zint += 32768.0;
                z -= 32768;
        }
        while (z < -32768.0) {
                zint -= 32768.0;
                z += 32768.0;
        }
        i = (int) z;
        zint += (double) i;
        return( x - zint * y);
}

/*ASIN.C
 *Calculates arcsin(x)
 *Range: 0 <= x <= 1
 *Precision: +/- .000,000,02
 *Header: math.h
 *Author: Max R. D^Arsteler
 */
extern double sqrt();

double asin(double x)

{
        double y;
        int sign;

        if (x > 1.0 || x < -1.0) return(1);
        sign = 0;
        if (x < 0) {
                 sign = 1;
                 x = -x;
        }
        y = ((((((-.0012624911	* x
                 + .0066700901) * x
                 - .0170881256) * x
                 + .0308918810) * x
                 - .0501743046) * x
                 + .0889789874) * x
                 - .2145988016) * x
                 +1.5707963050;
        y = 1.57079632679 - sqrt(1.0 - x) * y;
        if (sign) y = -y;
        return(y);
}

/* SIN.C
 * Calculates sin(x), angle x must be in rad.
 * Range: -pi/2 <= x <= pi/2
 * Precision: +/- .000,000,005
 * Header: math.h
 * Author: Max R. D^Arsteler
 */

double sin(double x)
{
        double xi, y, q, q2;
        int sign;

        xi = x; sign = 1;
        while (xi < -1.57079632679489661923) xi += 6.28318530717958647692;
        while (xi > 4.71238898038468985769) xi -= 6.28318530717958647692;
        if (xi > 1.57079632679489661923) {
                xi -= 3.141592653589793238462643;
                sign = -1;
        }
        q = xi / 1.57079632679; q2 = q * q;
        y = ((((.00015148419  * q2
              - .00467376557) * q2
              + .07968967928) * q2
              - .64596371106) * q2
              +1.57079631847) * q;
        return(sign < 0? -y : y);
}


/* LOG.C
 * Calculates natural logarithmus
 * Precision: 11.56 E-10
 * Range: 0 to e308
 * Author: Max R. D^Arsteler
 */
double log(double x)
{
        typedef union {
                double d;
                unsigned u[4];
        } DBL;
        double y, z, z2, p;
        unsigned ex;	    /* exponens*/
        int ix;
        DBL *xp, *yp;

        xp = (DBL *)&x;
        if (xp->d <= 0.0) return(y);
        y = 0.0;
        yp = (DBL *)&y;
        yp->u[3] = xp->u[3] & ~0100017; /* save exponens */
        ix = (yp->u[3] >> 4)-1023;
        if ((xp->d - yp->d) == 0.0) return( .693147180559945 * (double)ix);
        xp->u[3] -= ++ix << 4; /* arrange for range 0.5 to 0.99999999999 */
        xp->d *= 1.4142135623730950488;  /* shift for 1/sqrt(2) to sqrt(2) */
        z = (xp->d - 1.0) / (xp->d + 1.0);
        z2 = z * z;
        y = z  * (2.000000000046727 +	/* Polynomial approximation */
            z2 * (0.666666635059382 +	/* from: COMPUTER APPROXIMATIONS*/
            z2 * (0.4000059794795   +	/* Hart, J.F. et al. 1968 */
            z2 * (0.28525381498     +
            z2 *  0.2376245609 ))));
        y = y + .693147180559945 * ((double)ix	- 0.5);
        return(yp->d);
}

/* ATAN.C
 * Calculates arctan(x)
 * Range: -infinite <= x <= infinite (Output -pi/2 to +pi/2)
 * Precision: +/- .000,000,04
 * Header: math.h
 * Author: Max R. D^Arsteler 9/15/83
 */

double atan(double x)
{
        double xi, q, q2, y;
        int sign;

        xi = (x < 0. ? -x : x);
        q = (xi - 1.0) / (xi + 1.0); q2 = q * q;
        y = ((((((( - .0040540580 * q2
                     + .0218612286) * q2
                     - .0559098861) * q2
                     + .0964200441) * q2
                     - .1390853351) * q2
                     + .1994653599) * q2
                     - .3332985605) * q2
                     + .9999993329) * q + 0.785398163397;
        return(x < 0. ? -y: y);
}


/* FLOOR.C
 * Returns largest integer not greater than x
 * Author: Max R. D^Arsteler 9/26/83
 */
double floor(double x)
{
        double y;
        int ix;

        y = 0.0;
        while (x >= 32768.0) {
                y += 32768.0;
                x -= 32768.0;
        }
        while (x <= -32768.0) {
                y -= 32768.0;
                x += 32768.0;
        }
        if (x > 0.0) ix = (int) x;
        else ix = (int)(x - 0.9999999999);
        return( y + (double) ix);
}


/* CEIL.C
 * Returns smallest integer not less than x
 * Author: Max R. D^Arsteler 9/26/83
 */
double ceil(double x)
{
        double y;
        int ix;

        y = 0.0;
        while (x >= 32768.0) {
                y += 32768.0;
                x -= 32768.0;
        }
        while (x <= -32768.0) {
                y -= 32768.0;
                x += 32768.0;
        }
        if (x > 0.0) ix = (int) (x + 0.999999999999999);
        else ix = (int) x;
        return( y + (double) ix);
}

/* EXP.C
 * Calculates exponens of x to base e
 * Range: +/- exp(88)
 * Precision: +/- .000,000,000,1
 * Author: Max R. D^Arsteler, 9/20/83
 */
 double exp(double xi)
 {
        double y, ex, px, nn, ds, in;

        if (xi > 88.0) return(1.7014117331926443e38);
        if (xi < -88.0) return(0.0);
        ex = 1.0;
        while( xi > 1.0) {
                ex *= 2.718281828459; /* const. e */
                xi--;
        }
        while( xi < -1.0) {
                ex *= .367879441171;  /* 1/e */
                xi++;
        }
/* Slow, but more precise Taylor expansion series */
        y = ds = 1.0; nn = 0.0;
        while ((ds < 0.0 ? -ds : ds) > 0.000000000001) {
                px = xi/++nn;	      /* above precision required */
                ds *= px;
                y += ds;
        }
       y *= ex;

/*  Chebyshev polynomials: fast, but less precise then expected!
 *	xi = -xi;
 *	y = (((((.0000006906  * xi
 *		+.0000054302) * xi
 *		+.0001715620) * xi
 *		+.0025913712) * xi
 *		+.0312575832) * xi
 *		+.2499986842) * xi
 *		+1.0;
 *     y = ex / (y * y * y * y);
 */
        return(y);
}


/* LOG10.C
 * Approximation for logarithm of basis 10
 * Range 0 < x < 1e+38
 * Precision: +/- 0.000,000,1
 * Header: math.h
 * Author: Max R. D^Arsteler 9/15/83
 */

/* Method of Chebyshev polynomials */
/* C. Hastings, jr. 1955 */

double log10(double x)
{
        double xi, y, q, q2;
        int ex;

        if (x <= 0.0) return(0.); /* Error!! */
        ex = 0.0; xi = x;
        while (xi < 1.0 ) {
                xi *= 10.0;
                ex--;
        }
        while (xi > 10.0) {
                xi *= 0.1;
                ex++;
        }
        q = (xi - 3.16227766) / (xi + 3.16227766); q2 = q * q;
        y = ((((.191337714 * q2
              + .094376476) * q2
              + .177522071) * q2
              + .289335524) * q2
              + .868591718) * q + .5;
        y += (double) ex;
        return(y);
}

/* PW10.C
 * Calculates 10 power x
 * Range: 0 <= x <= 1
 * Precision: +/- 0.000,000,005
 * Header: math.lib
 * Author: Max R. D^Arsteler 9/15/83
 */

double pw10(int x)
{
        double xi,ex,y;

        if (x > 38.0) return(1.7014117331926443e38);
        if (x < -38.0) return(0.0);
        xi = x; ex = 1.0;
        while (xi > 1.0) {
                ex *= 10.0;
                xi--;
        }
        while (xi < 0.0) {
                ex *= 0.1;
                xi++;
        }
        y = ((((((.00093264267	* xi
                + .00255491796) * xi
                + .01742111988) * xi
                + .07295173666) * xi
                + .25439357484) * xi
                + .66273088429) * xi
                +1.15129277603) * xi + 1.0;
        y *= y;
        return(y*ex);
}
