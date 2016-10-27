
#include "matrix.h"
#include <float.h>
#include "my_errors.h"


/* @(#)s_erf.c 1.3 95/01/18 */
/*
 * ====================================================
 * Copyright (C) 1993 by Sun Microsystems, Inc. All rights reserved.
 *
 * Developed at SunSoft, a Sun Microsystems, Inc. business.
 * Permission to use, copy, modify, and distribute this
 * software is freely granted, provided that this notice 
 * is preserved.
 * ====================================================
 */

/* double erf(double x)
 * double erfc(double x)
 *			     x
 *		      2      |\
 *     erf(x)  =  ---------  | exp(-t*t)dt
 *	 	   sqrt(pi) \| 
 *			     0
 *
 *     erfc(x) =  1-erf(x)
 *  Note that 
 *		erf(-x) = -erf(x)
 *		erfc(-x) = 2 - erfc(x)
 *
 * Method:
 *	1. For |x| in [0, 0.84375]
 *	    erf(x)  = x + x*R(x^2)
 *          erfc(x) = 1 - erf(x)           if x in [-.84375,0.25]
 *                  = 0.5 + ((0.5-x)-x*R)  if x in [0.25,0.84375]
 *	   where R = P/Q where P is an odd poly of degree 8 and
 *	   Q is an odd poly of degree 10.
 *						 -57.90
 *			| R - (erf(x)-x)/x | <= 2
 *	
 *
 *	   Remark. The formula is derived by noting
 *          erf(x) = (2/sqrt(pi))*(x - x^3/3 + x^5/10 - x^7/42 + ....)
 *	   and that
 *          2/sqrt(pi) = 1.128379167095512573896158903121545171688
 *	   is close to one. The interval is chosen because the fix
 *	   point of erf(x) is near 0.6174 (i.e., erf(x)=x when x is
 *	   near 0.6174), and by some experiment, 0.84375 is chosen to
 * 	   guarantee the error is less than one ulp for erf.
 *
 *      2. For |x| in [0.84375,1.25], let s = |x| - 1, and
 *         c = 0.84506291151 rounded to single (24 bits)
 *         	erf(x)  = sign(x) * (c  + P1(s)/Q1(s))
 *         	erfc(x) = (1-c)  - P1(s)/Q1(s) if x > 0
 *			  1+(c+P1(s)/Q1(s))    if x < 0
 *         	|P1/Q1 - (erf(|x|)-c)| <= 2**-59.06
 *	   Remark: here we use the taylor series expansion at x=1.
 *		erf(1+s) = erf(1) + s*Poly(s)
 *			 = 0.845.. + P1(s)/Q1(s)
 *	   That is, we use rational approximation to approximate
 *			erf(1+s) - (c = (single)0.84506291151)
 *	   Note that |P1/Q1|< 0.078 for x in [0.84375,1.25]
 *	   where 
 *		P1(s) = degree 6 poly in s
 *		Q1(s) = degree 6 poly in s
 *
 *      3. For x in [1.25,1/0.35(~2.857143)], 
 *         	erfc(x) = (1/x)*exp(-x*x-0.5625+R1/S1)
 *         	erf(x)  = 1 - erfc(x)
 *	   where 
 *		R1(z) = degree 7 poly in z, (z=1/x^2)
 *		S1(z) = degree 8 poly in z
 *
 *      4. For x in [1/0.35,28]
 *         	erfc(x) = (1/x)*exp(-x*x-0.5625+R2/S2) if x > 0
 *			= 2.0 - (1/x)*exp(-x*x-0.5625+R2/S2) if -6<x<0
 *			= 2.0 - tiny		(if x <= -6)
 *         	erf(x)  = sign(x)*(1.0 - erfc(x)) if x < 6, else
 *         	erf(x)  = sign(x)*(1.0 - tiny)
 *	   where
 *		R2(z) = degree 6 poly in z, (z=1/x^2)
 *		S2(z) = degree 7 poly in z
 *
 *      Note1:
 *	   To compute exp(-x*x-0.5625+R/S), let s be a single
 *	   precision number and s := x; then
 *		-x*x = -s*s + (s-x)*(s+x)
 *	        exp(-x*x-0.5626+R/S) = 
 *			exp(-s*s-0.5625)*exp((s-x)*(s+x)+R/S);
 *      Note2:
 *	   Here 4 and 5 make use of the asymptotic series
 *			  exp(-x*x)
 *		erfc(x) ~ ---------- * ( 1 + Poly(1/x^2) )
 *			  x*sqrt(pi)
 *	   We use rational approximation to approximate
 *      	g(s)=f(1/x^2) = log(erfc(x)*x) - x*x + 0.5625
 *	   Here is the error bound for R1/S1 and R2/S2
 *      	|R1/S1 - f(x)|  < 2**(-62.57)
 *      	|R2/S2 - f(x)|  < 2**(-61.52)
 *
 *      5. For inf > x >= 28
 *         	erf(x)  = sign(x) *(1 - tiny)  (raise inexact)
 *         	erfc(x) = tiny*tiny (raise underflow) if x > 0
 *			= 2 - tiny if x<0
 *
 *      7. Special case:
 *         	erf(0)  = 0, erf(inf)  = 1, erf(-inf) = -1,
 *         	erfc(0) = 1, erfc(inf) = 0, erfc(-inf) = 2, 
 *	   	erfc/erf(NaN) is NaN
 */


/*
#ifdef __STDC__
static const double
#else
static double
#endif
*/
static const double
tiny	    = 1e-300,
dHuge    = 1e300,
zero = 0.00000000000000000000e+00,
half=  5.00000000000000000000e-01, /* 0x3FE00000, 0x00000000 */
one =  1.00000000000000000000e+00, /* 0x3FF00000, 0x00000000 */
two =  2.00000000000000000000e+00, /* 0x40000000, 0x00000000 */
	/* c = (float)0.84506291151 */
erx =  8.45062911510467529297e-01, /* 0x3FEB0AC1, 0x60000000 */
/*
 * Coefficients for approximation to  erf on [0,0.84375]
 */
efx =  1.28379167095512586316e-01, /* 0x3FC06EBA, 0x8214DB69 */
efx8=  1.02703333676410069053e+00, /* 0x3FF06EBA, 0x8214DB69 */
pp0  =  1.28379167095512558561e-01, /* 0x3FC06EBA, 0x8214DB68 */
pp1  = -3.25042107247001499370e-01, /* 0xBFD4CD7D, 0x691CB913 */
pp2  = -2.84817495755985104766e-02, /* 0xBF9D2A51, 0xDBD7194F */
pp3  = -5.77027029648944159157e-03, /* 0xBF77A291, 0x236668E4 */
pp4  = -2.37630166566501626084e-05, /* 0xBEF8EAD6, 0x120016AC */
qq1  =  3.97917223959155352819e-01, /* 0x3FD97779, 0xCDDADC09 */
qq2  =  6.50222499887672944485e-02, /* 0x3FB0A54C, 0x5536CEBA */
qq3  =  5.08130628187576562776e-03, /* 0x3F74D022, 0xC4D36B0F */
qq4  =  1.32494738004321644526e-04, /* 0x3F215DC9, 0x221C1A10 */
qq5  = -3.96022827877536812320e-06, /* 0xBED09C43, 0x42A26120 */
/*
 * Coefficients for approximation to  erf  in [0.84375,1.25] 
 */
pa0  = -2.36211856075265944077e-03, /* 0xBF6359B8, 0xBEF77538 */
pa1  =  4.14856118683748331666e-01, /* 0x3FDA8D00, 0xAD92B34D */
pa2  = -3.72207876035701323847e-01, /* 0xBFD7D240, 0xFBB8C3F1 */
pa3  =  3.18346619901161753674e-01, /* 0x3FD45FCA, 0x805120E4 */
pa4  = -1.10894694282396677476e-01, /* 0xBFBC6398, 0x3D3E28EC */
pa5  =  3.54783043256182359371e-02, /* 0x3FA22A36, 0x599795EB */
pa6  = -2.16637559486879084300e-03, /* 0xBF61BF38, 0x0A96073F */
qa1  =  1.06420880400844228286e-01, /* 0x3FBB3E66, 0x18EEE323 */
qa2  =  5.40397917702171048937e-01, /* 0x3FE14AF0, 0x92EB6F33 */
qa3  =  7.18286544141962662868e-02, /* 0x3FB2635C, 0xD99FE9A7 */
qa4  =  1.26171219808761642112e-01, /* 0x3FC02660, 0xE763351F */
qa5  =  1.36370839120290507362e-02, /* 0x3F8BEDC2, 0x6B51DD1C */
qa6  =  1.19844998467991074170e-02, /* 0x3F888B54, 0x5735151D */
/*
 * Coefficients for approximation to  erfc in [1.25,1/0.35]
 */
ra0  = -9.86494403484714822705e-03, /* 0xBF843412, 0x600D6435 */
ra1  = -6.93858572707181764372e-01, /* 0xBFE63416, 0xE4BA7360 */
ra2  = -1.05586262253232909814e+01, /* 0xC0251E04, 0x41B0E726 */
ra3  = -6.23753324503260060396e+01, /* 0xC04F300A, 0xE4CBA38D */
ra4  = -1.62396669462573470355e+02, /* 0xC0644CB1, 0x84282266 */
ra5  = -1.84605092906711035994e+02, /* 0xC067135C, 0xEBCCABB2 */
ra6  = -8.12874355063065934246e+01, /* 0xC0545265, 0x57E4D2F2 */
ra7  = -9.81432934416914548592e+00, /* 0xC023A0EF, 0xC69AC25C */
sa1  =  1.96512716674392571292e+01, /* 0x4033A6B9, 0xBD707687 */
sa2  =  1.37657754143519042600e+02, /* 0x4061350C, 0x526AE721 */
sa3  =  4.34565877475229228821e+02, /* 0x407B290D, 0xD58A1A71 */
sa4  =  6.45387271733267880336e+02, /* 0x40842B19, 0x21EC2868 */
sa5  =  4.29008140027567833386e+02, /* 0x407AD021, 0x57700314 */
sa6  =  1.08635005541779435134e+02, /* 0x405B28A3, 0xEE48AE2C */
sa7  =  6.57024977031928170135e+00, /* 0x401A47EF, 0x8E484A93 */
sa8  = -6.04244152148580987438e-02, /* 0xBFAEEFF2, 0xEE749A62 */
/*
 * Coefficients for approximation to  erfc in [1/.35,28]
 */
rb0  = -9.86494292470009928597e-03, /* 0xBF843412, 0x39E86F4A */
rb1  = -7.99283237680523006574e-01, /* 0xBFE993BA, 0x70C285DE */
rb2  = -1.77579549177547519889e+01, /* 0xC031C209, 0x555F995A */
rb3  = -1.60636384855821916062e+02, /* 0xC064145D, 0x43C5ED98 */
rb4  = -6.37566443368389627722e+02, /* 0xC083EC88, 0x1375F228 */
rb5  = -1.02509513161107724954e+03, /* 0xC0900461, 0x6A2E5992 */
rb6  = -4.83519191608651397019e+02, /* 0xC07E384E, 0x9BDC383F */
sb1  =  3.03380607434824582924e+01, /* 0x403E568B, 0x261D5190 */
sb2  =  3.25792512996573918826e+02, /* 0x40745CAE, 0x221B9F0A */
sb3  =  1.53672958608443695994e+03, /* 0x409802EB, 0x189D5118 */
sb4  =  3.19985821950859553908e+03, /* 0x40A8FFB7, 0x688C246A */
sb5  =  2.55305040643316442583e+03, /* 0x40A3F219, 0xCEDF3BE6 */
sb6  =  4.74528541206955367215e+02, /* 0x407DA874, 0xE79FE763 */
sb7  = -2.24409524465858183362e+01; /* 0xC03670E2, 0x42712D62 */


static const double 
two52=  4.50359962737049600000e+15, /* 0x43300000, 0x00000000 */
pi  =  3.14159265358979311600e+00, /* 0x400921FB, 0x54442D18 */
a0  =  7.72156649015328655494e-02, /* 0x3FB3C467, 0xE37DB0C8 */
a1  =  3.22467033424113591611e-01, /* 0x3FD4A34C, 0xC4A60FAD */
a2  =  6.73523010531292681824e-02, /* 0x3FB13E00, 0x1A5562A7 */
a3  =  2.05808084325167332806e-02, /* 0x3F951322, 0xAC92547B */
a4  =  7.38555086081402883957e-03, /* 0x3F7E404F, 0xB68FEFE8 */
a5  =  2.89051383673415629091e-03, /* 0x3F67ADD8, 0xCCB7926B */
a6  =  1.19270763183362067845e-03, /* 0x3F538A94, 0x116F3F5D */
a7  =  5.10069792153511336608e-04, /* 0x3F40B6C6, 0x89B99C00 */
a8  =  2.20862790713908385557e-04, /* 0x3F2CF2EC, 0xED10E54D */
a9  =  1.08011567247583939954e-04, /* 0x3F1C5088, 0x987DFB07 */
a10 =  2.52144565451257326939e-05, /* 0x3EFA7074, 0x428CFA52 */
a11 =  4.48640949618915160150e-05, /* 0x3F07858E, 0x90A45837 */
tc  =  1.46163214496836224576e+00, /* 0x3FF762D8, 0x6356BE3F */
tf  = -1.21486290535849611461e-01, /* 0xBFBF19B9, 0xBCC38A42 */
/* tt = -(tail of tf) */
tt  = -3.63867699703950536541e-18, /* 0xBC50C7CA, 0xA48A971F */
t0  =  4.83836122723810047042e-01, /* 0x3FDEF72B, 0xC8EE38A2 */
t1  = -1.47587722994593911752e-01, /* 0xBFC2E427, 0x8DC6C509 */
t2  =  6.46249402391333854778e-02, /* 0x3FB08B42, 0x94D5419B */
t3  = -3.27885410759859649565e-02, /* 0xBFA0C9A8, 0xDF35B713 */
t4  =  1.79706750811820387126e-02, /* 0x3F9266E7, 0x970AF9EC */
t5  = -1.03142241298341437450e-02, /* 0xBF851F9F, 0xBA91EC6A */
t6  =  6.10053870246291332635e-03, /* 0x3F78FCE0, 0xE370E344 */
t7  = -3.68452016781138256760e-03, /* 0xBF6E2EFF, 0xB3E914D7 */
t8  =  2.25964780900612472250e-03, /* 0x3F6282D3, 0x2E15C915 */
t9  = -1.40346469989232843813e-03, /* 0xBF56FE8E, 0xBF2D1AF1 */
t10 =  8.81081882437654011382e-04, /* 0x3F4CDF0C, 0xEF61A8E9 */
t11 = -5.38595305356740546715e-04, /* 0xBF41A610, 0x9C73E0EC */
t12 =  3.15632070903625950361e-04, /* 0x3F34AF6D, 0x6C0EBBF7 */
t13 = -3.12754168375120860518e-04, /* 0xBF347F24, 0xECC38C38 */
t14 =  3.35529192635519073543e-04, /* 0x3F35FD3E, 0xE8C2D3F4 */
u0  = -7.72156649015328655494e-02, /* 0xBFB3C467, 0xE37DB0C8 */
u1  =  6.32827064025093366517e-01, /* 0x3FE4401E, 0x8B005DFF */
u2  =  1.45492250137234768737e+00, /* 0x3FF7475C, 0xD119BD6F */
u3  =  9.77717527963372745603e-01, /* 0x3FEF4976, 0x44EA8450 */
u4  =  2.28963728064692451092e-01, /* 0x3FCD4EAE, 0xF6010924 */
u5  =  1.33810918536787660377e-02, /* 0x3F8B678B, 0xBF2BAB09 */
v1  =  2.45597793713041134822e+00, /* 0x4003A5D7, 0xC2BD619C */
v2  =  2.12848976379893395361e+00, /* 0x40010725, 0xA42B18F5 */
v3  =  7.69285150456672783825e-01, /* 0x3FE89DFB, 0xE45050AF */
v4  =  1.04222645593369134254e-01, /* 0x3FBAAE55, 0xD6537C88 */
v5  =  3.21709242282423911810e-03, /* 0x3F6A5ABB, 0x57D0CF61 */
s0  = -7.72156649015328655494e-02, /* 0xBFB3C467, 0xE37DB0C8 */
s1  =  2.14982415960608852501e-01, /* 0x3FCB848B, 0x36E20878 */
s2  =  3.25778796408930981787e-01, /* 0x3FD4D98F, 0x4F139F59 */
s3  =  1.46350472652464452805e-01, /* 0x3FC2BB9C, 0xBEE5F2F7 */
s4  =  2.66422703033638609560e-02, /* 0x3F9B481C, 0x7E939961 */
s5  =  1.84028451407337715652e-03, /* 0x3F5E26B6, 0x7368F239 */
s6  =  3.19475326584100867617e-05, /* 0x3F00BFEC, 0xDD17E945 */
r1  =  1.39200533467621045958e+00, /* 0x3FF645A7, 0x62C4AB74 */
r2  =  7.21935547567138069525e-01, /* 0x3FE71A18, 0x93D3DCDC */
r3  =  1.71933865632803078993e-01, /* 0x3FC601ED, 0xCCFBDF27 */
r4  =  1.86459191715652901344e-02, /* 0x3F9317EA, 0x742ED475 */
r5  =  7.77942496381893596434e-04, /* 0x3F497DDA, 0xCA41A95B */
r6  =  7.32668430744625636189e-06, /* 0x3EDEBAF7, 0xA5B38140 */
w0  =  4.18938533204672725052e-01, /* 0x3FDACFE3, 0x90C97D69 */
w1  =  8.33333333333329678849e-02, /* 0x3FB55555, 0x5555553B */
w2  = -2.77777777728775536470e-03, /* 0xBF66C16C, 0x16B02E5C */
w3  =  7.93650558643019558500e-04, /* 0x3F4A019F, 0x98CF38B6 */
w4  = -5.95187557450339963135e-04, /* 0xBF4380CB, 0x8C0FE741 */
w5  =  8.36339918996282139126e-04, /* 0x3F4B67BA, 0x4CDAD5D1 */
w6  = -1.63092934096575273989e-03; /* 0xBF5AB89D, 0x0B9E43E4 */


static double sin_pi(double x)
{
	double y,z;
	int n,ix;

	ix = 0x7fffffff&__HI(x);

	if(ix<0x3fd00000) return sin(pi*x);					//__kernel_sin(pi*x,zero,0);
	y = -x;		/* x is assume negative */

    /*
     * argument reduction, make sure inexact flag not raised if input
     * is an integer
     */
	z = floor(y);
	if(z!=y) {				/* inexact anyway */
	    y  *= 0.5;
	    y   = 2.0*(y - floor(y));		/* y = |x| mod 2.0 */
	    n   = (int) (y*4.0);
	} else {
            if(ix>=0x43400000) {
                y = zero; n = 0;                 /* y must be even */
            } else {
                if(ix<0x43300000) z = y+two52;	/* exact */
                n   = __LO(z)&1;        /* lower word of z */
                y  = n;
                n<<= 2;
            }
        }
	switch (n) {
	    case 0:   y =  sin(pi*y);					//__kernel_sin(pi*y,zero,0); break;
	    case 1:   
	    case 2:   y =  cos(pi*(0.5-y)); break;		//__kernel_cos(pi*(0.5-y),zero); break;
	    case 3:  
	    case 4:   y =  sin(pi*(one-y)); break;		//__kernel_sin(pi*(one-y),zero,0); break;
	    case 5:
	    case 6:   y = -cos(pi*(y-1.5)); break;		//__kernel_cos(pi*(y-1.5),zero); break;
	    default:  y =  sin(pi*(y-2.0)); break;		//__kernel_sin(pi*(y-2.0),zero,0); break;
	}
	return -y;
}


double lgamma(double x, int *signgamp)
{
	double t,y,z,nadj,p,p1,p2,p3,q,r,w;
	int i,hx,lx,ix;
	bool bNeg = false;

	hx = __HI(x);
	lx = __LO(x);

    /* purge off +-inf, NaN, +-0, and negative arguments */
	*signgamp = 1;
	ix = hx&0x7fffffff;
	if(ix>=0x7ff00000) return x*x;
	if((ix|lx)==0) return one/zero;
//	if(ix<0x3b900000) {	/* |x|<2**-70, return -log(|x|) */
	if (fabs(x)<pow(2.,-70.)) {
	    if(hx<0) {
	        *signgamp = -1;
	        return -log(-x);
	    } else return -log(x);
	}
	if(x<0) {	// create the negative adustment factor
//	    if(ix>=0x43300000) 	/* |x|>=2**52, must be -integer */
//		if (fabs(x)>=pow(2,52))
		if (Round(x)==x) return one/zero;		// -integer
//	    t = sin_pi(x);
		t = sin(pi*x);
	    if(t==zero) return one/zero; /* -integer */
	    nadj = log(pi/fabs(t*x));
	    if(t<zero) *signgamp = -1;
	    x = -x;
		bNeg = true;
	}

    /* purge off 1 and 2 */
//	if((((ix-0x3ff00000)|lx)==0)||(((ix-0x40000000)|lx)==0)) r = 0;
	if (x==1.0 || x==2.0) r = 0;
	/* for x < 2.0 */
//	else if(ix<0x40000000) {
	else if (x < 2.0) {
//	    if (ix<=0x3feccccc) { 	/* lgamma(x) = lgamma(x+1)-log(x) */
		if (x <= 1.5) {
			r = -log(x);
			if (ix>=0x3FE76944) {y = one-x; i= 0;}
			else if (ix>=0x3FCDA661) {y= x-(tc-one); i=1;}
	  		else {y = x; i=2;}
	    } else {
			r = zero;
//	        if (ix>=0x3FFBB4C3) {y=2.0-x;i=0;} /* [1.7316,2] */
	        if (x>=1.73163) {y=2.0-x;i=0;} /* [1.7316,2] */
//	        else if (ix>=0x3FF3B4C4) {y=x-tc;i=1;} /* [1.23,1.73] */
	        else if (x>=1.23164) {y=x-tc;i=1;} /* [1.23,1.73] */
		else {y=x-one;i=2;}
	    }
	    switch(i) {
	      case 0:
		z = y*y;
		p1 = a0+z*(a2+z*(a4+z*(a6+z*(a8+z*a10))));
		p2 = z*(a1+z*(a3+z*(a5+z*(a7+z*(a9+z*a11)))));
		p  = y*p1+p2;
		r  += (p-0.5*y); break;
	      case 1:
		z = y*y;
		w = z*y;
		p1 = t0+w*(t3+w*(t6+w*(t9 +w*t12)));	/* parallel comp */
		p2 = t1+w*(t4+w*(t7+w*(t10+w*t13)));
		p3 = t2+w*(t5+w*(t8+w*(t11+w*t14)));
		p  = z*p1-(tt-w*(p2+y*p3));
		r += (tf + p); break;
	      case 2:	
		p1 = y*(u0+y*(u1+y*(u2+y*(u3+y*(u4+y*u5)))));
		p2 = one+y*(v1+y*(v2+y*(v3+y*(v4+y*v5))));
		r += (-0.5*y + p1/p2);
	    }
	}
//	else if(ix<0x40200000) { 			/* x < 8.0 */
	else if (x < 8.0) {
	    i = (int)x;
	    t = zero;
	    y = x-(double)i;
	    p = y*(s0+y*(s1+y*(s2+y*(s3+y*(s4+y*(s5+y*s6))))));
	    q = one+y*(r1+y*(r2+y*(r3+y*(r4+y*(r5+y*r6)))));
	    r = half*y+p/q;
	    z = one;	/* lgamma(1+s) = log(s) + lgamma(s) */
	    switch(i) {
	    case 7: z *= (y+6.0);	/* FALLTHRU */
	    case 6: z *= (y+5.0);	/* FALLTHRU */
	    case 5: z *= (y+4.0);	/* FALLTHRU */
	    case 4: z *= (y+3.0);	/* FALLTHRU */
	    case 3: z *= (y+2.0);	/* FALLTHRU */
		default:
		    r += log(z); break;
	    }
    /* 8.0 <= x < 2**58 */
//	} else if (ix < 0x43900000) {
	} else if (x >= 8.0 && x < pow(2.,58.)) {
	    t = log(x);
	    z = one/x;
	    y = z*z;
	    w = w0+z*(w1+y*(w2+y*(w3+y*(w4+y*(w5+y*w6)))));
	    r = (x-half)*(t-one)+w;
	} else 
    /* 2**58 <= x <= inf */
	    r =  x*(log(x)-one);
	if (bNeg) r = nadj - r;
	return r;
}


bool erfR(cMatrix& Result, cMatrix& R)
{
	// ignore imaginary part
	if (Result.Create(R.GetRows(), R.GetColumns())) return true;

	double *pr = Result.GetR();
	const double *prr = R.GetSafeR();
	int n = R.GetSize();
	for (int i=0; i<n; i++)
		pr[i] = erf(prr[i]);

	return false;
}

bool erfcR(cMatrix& Result, cMatrix& R)
{
	// ignore imaginary part
	if (Result.Create(R.GetRows(), R.GetColumns())) return true;

	double *pr = Result.GetR();
	const double *prr = R.GetSafeR();
	int n = R.GetSize();
	for (int i=0; i<n; i++)
		pr[i] = erfc(prr[i]);

	return false;
}


bool lgamma(cMatrix& Result, cMatrix& R)
{
	// ignore imaginary part
	if (Result.Create(R.GetRows(), R.GetColumns())) return true;

	double *pr = Result.GetR();
	const double *prr = R.GetSafeR();
	int n = R.GetSize();
	for (int i=0; i<n; i++)
		pr[i] = lgamma(prr[i]);

	return false;
}

double gamma(double x)
{
	int isign = 1;

	double gln = lgamma(x, &isign);
	return ( double(isign) * exp( gln ) );
}

bool gamma(cMatrix& Result, cMatrix& R)
{
	// ignore imaginary part
	if (Result.Create(R.GetRows(), R.GetColumns())) return true;

	double *pr = Result.GetR();
	const double *prr = R.GetSafeR();
	int n = R.GetSize();
	for (int i=0; i<n; i++)
		pr[i] = gamma(prr[i]);

	return false;
}


bool j0R(cMatrix& Result, cMatrix& R)
{
	// ignore imaginary part
	if (Result.Create(R.GetRows(), R.GetColumns())) return true;

	double *pr = Result.GetR();
	const double *prr = R.GetSafeR();
	int n = R.GetSize();
	for (int i=0; i<n; i++)
		pr[i] = j0(prr[i]);

	return false;
}

bool j1R(cMatrix& Result, cMatrix& R)
{
	// ignore imaginary part
	if (Result.Create(R.GetRows(), R.GetColumns())) return true;

	double *pr = Result.GetR();
	const double *prr = R.GetSafeR();
	int n = R.GetSize();
	for (int i=0; i<n; i++)
		pr[i] = j1(prr[i]);

	return false;
}

bool jnR(cMatrix& Result, int iorder, cMatrix& R)
{
	if (iorder<0) { addError("order must be greater than 0"); return true; }

	// ignore imaginary part
	if (Result.Create(R.GetRows(), R.GetColumns())) return true;

	double *pr = Result.GetR();
	const double *prr = R.GetSafeR();
	int n = R.GetSize();
	for (int i=0; i<n; i++)
		pr[i] = jn(iorder, prr[i]);

	return false;
}

bool y0R(cMatrix& Result, cMatrix& R)
{
	// ignore imaginary part
	if (Result.Create(R.GetRows(), R.GetColumns())) return true;

	double *pr = Result.GetR();
	const double *prr = R.GetSafeR();
	int n = R.GetSize();
	for (int i=0; i<n; i++)
		pr[i] = y0(prr[i]);

	return false;
}

bool y1R(cMatrix& Result, cMatrix& R)
{
	// ignore imaginary part
	if (Result.Create(R.GetRows(), R.GetColumns())) return true;

	double *pr = Result.GetR();
	const double *prr = R.GetSafeR();
	int n = R.GetSize();
	for (int i=0; i<n; i++)
		pr[i] = y1(prr[i]);

	return false;
}

bool ynR(cMatrix& Result, int iorder, cMatrix& R)
{
	if (iorder<0) { addError("order must be greater than 0"); return true; }

	// ignore imaginary part
	if (Result.Create(R.GetRows(), R.GetColumns())) return true;

	double *pr = Result.GetR();
	const double *prr = R.GetSafeR();
	int n = R.GetSize();
	for (int i=0; i<n; i++)
		pr[i] = yn(iorder, prr[i]);

	return false;
}

