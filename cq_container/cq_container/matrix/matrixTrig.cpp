/* trigMathLib.cpp
Various math functions defined for the
classes defined in "matrix.h"
*/

#include <float.h>
#include "matrix.h"
#include "my_errors.h"

#define LN2 (log(2.0))
#define MAX_LOG2 30		// maximum expected log2(nfft), 20 corresponds to about 1e6 FFT points


const complex<double> J(0.,1.);

// trig functions
complex<double> sinC(complex<double>& x)
{
	complex<double> cr, arg1, arg2;
	arg1 = J*x;
	arg2 = -1.*arg1;
	cr = ( ExpC(arg1) - ExpC(arg2) ) / (2.*J);
	return cr;
}
bool sinR(cMatrix& Res, cMatrix& R)
{
	if (Res.Create(R.GetRows(),R.GetColumns())) return true;

	//const double *pr = R.GetR();
	for (int i=0; i<R.GetSize(); ++i)
		Res.SetR( i, sin(R.mr[i]) );

	return false;
}
bool sinC( cMatrix& Res, cMatrix& C)
{
	if (Res.CreateComplex(C.GetRows(),C.GetColumns())) return true;

	//const double *pr = C.GetR(), *pi = C.GetI();
	complex<double> c, ca;
	for (int i=0; i<C.GetSize(); ++i)
	{
		ca = complex<double>(C.mr[i],C.mi[i]);
		c = sinC( ca );
		Res.Set( i, c );
	}
	return false;
}

//
complex<double> cosC(complex<double>& x)
{
	complex<double> cr, arg1, arg2;
	arg1 = J*x;
	arg2 = -1.*arg1;
	cr = ( ExpC(arg1) + ExpC(arg2) ) / 2.;
	return cr;
}
bool cosR(cMatrix& Res, cMatrix& R)
{
	if (Res.Create(R.GetRows(),R.GetColumns())) return true;

	//const double *pr = R.GetR();
	for (int i=0; i<R.GetSize(); ++i)
		Res.SetR( i, cos(R.mr[i]) );

	return false;
}
bool cosC(cMatrix& Res, cMatrix& C)
{
	if (Res.CreateComplex(C.GetRows(),C.GetColumns())) return true;

	//const double *pr = C.GetR(), *pi = C.GetI();
	complex<double> c, ca;
	for (int i=0; i<C.GetSize(); ++i)
	{
		ca = complex<double>(C.mr[i],C.mi[i]);
		c = cosC( ca );
		Res.Set( i, c );
	}
	return false;
}
//
complex<double> tanC(complex<double>& x) { return sinC(x)/cosC(x); }
bool tanR(cMatrix& Res, cMatrix& R)
{
	if (Res.Create(R.GetRows(),R.GetColumns())) return true;

	//const double *pr = R.GetR();
	for (int i=0; i<R.GetSize(); ++i)
		Res.SetR( i, tan(R.mr[i]) );

	return false;
}
bool tanC(cMatrix& Res, cMatrix& C)
{
	if (Res.CreateComplex(C.GetRows(),C.GetColumns())) return true;

	//const double *pr = C.GetR(), *pi = C.GetI();
	complex<double> c, ca;
	for (int i=0; i<C.GetSize(); ++i)
	{
		ca = complex<double>(C.mr[i],C.mi[i]);
		c = tanC( ca );
		Res.Set( i, c );
	}
	return false;
}
//
complex<double> asinR(double d)
{
	complex<double> x(d,0.);
	return asinC(x);
}
complex<double> asinC(complex<double>& x)
{
	complex<double> c;
	complex<double> arg = 1.-x*x;
	c = J*x + sqrtC(arg);
	return -1.*J*logC(c);
}
bool asinR(cMatrix& Cout, cMatrix& R)
{
	double lf;
	complex<double> cv;
	int i;

	if (checkRealDomain(-1,LIMIT_SQUARE,1,LIMIT_SQUARE,R)) {
		if (Cout.Create(R.GetRows(),R.GetColumns())) return true;
		//pr = R.GetR();
		for (i=0; i<R.GetSize(); ++i) {
			lf = asin(R.mr[i]);
			Cout.SetR( i, lf );
		}
	}
	else {
		if (Cout.CreateComplex(R.GetRows(),R.GetColumns())) return true;
		//pr = R.GetR();
		for (i=0; i<R.GetSize(); ++i) {
			cv = asinR(R.mr[i]);
			Cout.Set( i, cv );
		}
	}

	return false;
}
bool asinC(cMatrix& Cout, cMatrix& C)
{
	if (Cout.CreateComplex(C.GetRows(),C.GetColumns())) return true;

	complex<double> c, ca;
	for (int i=0; i<C.GetSize(); ++i)
	{
		ca = complex<double>(C.mr[i],C.mi[i]);
		c = asinC( ca );
		Cout.Set( i, c );
	}
	return false;
}
//
complex<double> acosR(double d)
{
	complex<double> c(d,0.);
	return acosC(c);
}
complex<double> acosC(complex<double>& x)
{
	complex<double> c;
	complex<double> arg = 1.-x*x;
	c = x + J*sqrtC(arg);
	return -1.*J*logC(c);
}
bool acosR(cMatrix& Cout,cMatrix& R)
{
	int i;
	double lf;
	complex<double> c;

	if (checkRealDomain(-1,LIMIT_SQUARE,1,LIMIT_SQUARE,R)) {
		if (Cout.Create(R.GetRows(),R.GetColumns())) return true;
		for (i=0; i<R.GetSize(); ++i) {
			lf = acos(R.mr[i]);
			Cout.SetR( i, lf );
		}
	}
	else {
		if (Cout.CreateComplex(R.GetRows(),R.GetColumns())) return true;

		for (i=0; i<R.GetSize(); ++i) {
			c = acosR(R.mr[i]);
			Cout.Set( i, c );
		}
	}

	return false;
}
bool acosC(cMatrix& Cout,cMatrix& C)
{
	if (Cout.CreateComplex(C.GetRows(),C.GetColumns())) return true;

	//const double *pr = C.GetR();
	//const double *pi = C.GetI();
	complex<double> c, ca;
	for (int i=0; i<C.GetSize(); ++i)
	{
		ca = complex<double>(C.mr[i],C.mi[i]);
		c = acosC( ca );
		Cout.Set( i, c );
	}
	return false;
}
//
complex<double> atanC(complex<double>& x)
{
	complex<double> arg = (J+x)/(J-x);
	return J/2.*logC( arg );
}
bool atanR(cMatrix& Rout, cMatrix& R)
{
	if (Rout.Create(R.GetRows(),R.GetColumns())) return true;

	double lf;
	for (int i=0; i<R.GetSize(); ++i) {
		lf = atan(R.mr[i]);
		Rout.SetR( i, lf );
	}

	return false;
}
bool atanC(cMatrix& Cout, cMatrix& C)
{
	if (Cout.CreateComplex(C.GetRows(),C.GetColumns())) return true;

	complex<double> c, ca;
	for (int i=0; i<C.GetSize(); ++i)
	{
		ca = complex<double>(C.mr[i],C.mi[i]);
		c = atanC( ca );
		Cout.Set( i, c );
	}
	return false;
}
//
complex<double> cscC(complex<double>& x)	{ return 1./sinC(x); }
bool cscR(cMatrix& Rout, cMatrix& R)
{
	if (Rout.Create(R.GetRows(),R.GetColumns())) return true;

	double lf;
	for (int i=0; i<R.GetSize(); ++i) {
		lf = csc(R.mr[i]);
		Rout.SetR( i, lf );
	}

	return false;
}
bool cscC(cMatrix& Cout, cMatrix& C)
{
	if (Cout.CreateComplex(C.GetRows(),C.GetColumns())) return true;

	complex<double> c, ca;
	for (int i=0; i<C.GetSize(); ++i)
	{
		ca = complex<double>(C.mr[i],C.mi[i]);
		c = cscC( ca );
		Cout.Set( i, c );
	}
	return false;
}
//
complex<double> secC(complex<double>& x)	{ return 1./cosC(x); }
bool secR(cMatrix& Rout, cMatrix& R)
{
	if (Rout.Create(R.GetRows(),R.GetColumns())) return true;

	double lf;
	for (int i=0; i<R.GetSize(); ++i) {
		lf = sec(R.mr[i]);
		Rout.SetR( i, lf );
	}

	return false;
}
bool secC(cMatrix& Cout, cMatrix& C)
{
	if (Cout.CreateComplex(C.GetRows(),C.GetColumns())) return true;

	complex<double> c, ca;
	for (int i=0; i<C.GetSize(); ++i)
	{
		ca = complex<double>(C.mr[i],C.mi[i]);
		c = secC( ca );
		Cout.Set( i, c );
	}
	return false;
}
//
complex<double> cotC(complex<double>& x)	{ return 1./tanC(x); }
bool cotR(cMatrix& Rout, cMatrix& R)
{
	if (Rout.Create(R.GetRows(),R.GetColumns())) return true;

	double lf;
	for (int i=0; i<R.GetSize(); ++i) {
		lf = cot(R.mr[i]);
		Rout.SetR( i, lf );
	}

	return false;
}
bool cotC(cMatrix& Cout, cMatrix& C)
{
	if (Cout.CreateComplex(C.GetRows(),C.GetColumns())) return true;

	complex<double> c, ca;
	for (int i=0; i<C.GetSize(); ++i)
	{
		ca = complex<double>(C.mr[i],C.mi[i]);
		c = cotC( ca );
		Cout.Set( i, c );
	}
	return false;
}
//
complex<double> acscR(double d)
{
	complex<double> c(d,0.);
	complex<double> arg = 1./c;
	return asinC(arg);
}
complex<double> acscC(complex<double>& c)
{
	complex<double> arg = 1./c;
	return asinC(arg);
}
bool acscR(cMatrix& Cout, cMatrix& R)
{
	if (Cout.CreateComplex(R.GetRows(),R.GetColumns())) return true;

	complex<double> c;
	for (int i=0; i<R.GetSize(); ++i) {
		c = acscR(R.mr[i]);
		Cout.Set( i, c );
	}

	return false;
}
bool acscC(cMatrix& Cout, cMatrix& C)
{
	if (Cout.CreateComplex(C.GetRows(),C.GetColumns())) return true;

	complex<double> c, ca;
	for (int i=0; i<C.GetSize(); ++i) {
		ca = complex<double>(C.mr[i],C.mi[i]);
		c = acscC(ca);
		Cout.Set( i, c );
	}
	return false;
}
//
complex<double> asecR(double d)
{
	return acosR(1./d);
}
complex<double> asecC(complex<double>& c)
{
	complex<double> arg = 1./c;
	return acosC(arg);
}
bool asecR(cMatrix& Cout, cMatrix& R)
{
	if (Cout.CreateComplex(R.GetRows(),R.GetColumns())) return true;

	complex<double> c;
	for (int i=0; i<R.GetSize(); ++i) {
		c = asecR(R.mr[i]);
		Cout.Set( i, c );
	}

	return false;
}
bool asecC(cMatrix& Cout, cMatrix& C)
{
	if (Cout.CreateComplex(C.GetRows(),C.GetColumns())) return true;


	complex<double> c, ca;
	for (int i=0; i<C.GetSize(); ++i) {
		ca = complex<double>(C.mr[i],C.mi[i]);
		c = acscC(ca);
		Cout.Set( i, c );
	}
	return false;
}
//
double acotR(double d)		{ return atan(1./d); }
complex<double> acotC(complex<double>& c)
{
	complex<double> arg = 1./c;
	return atanC(arg);
}
bool acotR(cMatrix& Rout, cMatrix& R)
{
	if (Rout.Create(R.GetRows(),R.GetColumns())) return true;

	double lf;
	for (int i=0; i<R.GetSize(); ++i) {
		lf = acotR(R.mr[i]);
		Rout.SetR( i, lf );
	}

	return false;
}
bool acotC(cMatrix& Cout, cMatrix& C)
{
	if (Cout.CreateComplex(C.GetRows(),C.GetColumns())) return true;

	complex<double> c, ca;
	for (int i=0; i<C.GetSize(); ++i) {
		ca = complex<double>(C.mr[i],C.mi[i]);
		c = acotC(ca);
		Cout.Set( i, c );
	}
	return false;
}
//
double csc(double d) { return 1./sin(d); }
double sec(double d) { return 1./cos(d); }
double cot(double d) { return 1./tan(d); }
double acsc(double d) { return asin(1./d); }
double asec(double d) { return acos(1./d); }
double acot(double d) { return atan(1./d); }
//
bool atan2(cMatrix& Rout, cMatrix& R1, cMatrix& R2)
{
	int i;
	if ( Rout.Create(R1.GetRows(),R1.GetColumns()) ) return true;

	double lf;
	for (i=0; i<R1.GetSize(); i++) {
		lf = atan2(R1.mr[i], R2.mr[i]);
		Rout.SetR( i, lf );
	}

	return false;
}
bool atan2(cMatrix& Rout, cMatrix& R, double d)
{
	int i;
	if ( Rout.Create(R.GetRows(),R.GetColumns()) ) return true;

	double lf;
	for (i=0; i<R.GetSize(); i++) {
		lf = atan2(R.mr[i], d);
		Rout.SetR( i, lf );
	}
	return false;
}
bool atan2(cMatrix& Rout, double d, cMatrix& R)
{
	int i;
	if ( Rout.Create(R.GetRows(),R.GetColumns()) ) return true;

	double lf;
	for (i=0; i<R.GetSize(); i++) {
		lf = atan2(d, R.mr[i]);
		Rout.SetR( i, lf );
	}
	return false;
}
bool Catan2(cMatrix& Rout, cMatrix& C, cMatrix& R)
{
	int i;
	if ( Rout.Create(C.GetRows(),C.GetColumns()) ) return true;

	double lf;
	for (i=0; i<C.GetSize(); i++) {
		lf = atan2(C.mr[i], R.mr[i]);
		Rout.SetR( i, lf );
	}
	return false;
}
bool Catan2(cMatrix& Rout, cMatrix& C, double d)
{
	int i;
	if ( Rout.Create(C.GetRows(),C.GetColumns()) ) return true;

	double lf;
	for (i=0; i<C.GetSize(); i++) {
		lf = atan2(C.mr[i], d);
		Rout.SetR( i, lf );
	}
	return false;
}
bool Catan2(cMatrix& Rout, complex<double>& c, cMatrix& R)
{
	int i;
	if ( Rout.Create(R.GetRows(),R.GetColumns()) ) return true;

	double dv = c.real();			// ignore imaginary part
	double lf;
	for (i=0; i<R.GetSize(); i++) {
		lf = atan2(dv, R.mr[i]);
		Rout.SetR( i, lf );
	}
	return false;
}
bool atan2C(cMatrix& Rout, cMatrix& R, cMatrix& C)
{
	int i;
	if ( Rout.Create(C.GetRows(),C.GetColumns()) ) return true;

	double lf;
	for (i=0; i<C.GetSize(); i++) {
		lf = atan2(R.mr[i], C.mr[i]);
		Rout.SetR( i, lf );
	}
	return false;
}
bool atan2C(cMatrix& Rout, double d, cMatrix& C)
{
	int i;
	if ( Rout.Create(C.GetRows(),C.GetColumns()) ) return true;

	double lf;
	for (i=0; i<C.GetSize(); i++) {
		lf = atan2(d, C.mr[i]);
		Rout.SetR( i, lf );
	}
	return false;
}
bool atan2C(cMatrix& Rout, cMatrix& R, complex<double>& c)
{
	int i;
	if ( Rout.Create(R.GetRows(),R.GetColumns()) ) return true;

	double d = c.real();
	double lf;
	for (i=0; i<R.GetSize(); i++) {
		lf = atan2(R.mr[i],d);
		Rout.SetR( i, lf );
	}
	return false;
}
bool Catan2C(cMatrix& Rout, cMatrix& C1, cMatrix& C2)
{
	int i;
	if ( Rout.Create(C1.GetRows(),C2.GetColumns()) ) return true;

	double lf;
	for (i=0; i<C1.GetSize(); i++) {
		lf = atan2(C1.mr[i], C2.mr[i]);
		Rout.SetR( i, lf );
	}
	return false;
}
bool Catan2C(cMatrix& Rout, complex<double>& c, cMatrix& C2)
{ return atan2C(Rout, c.real(), C2); }

bool Catan2C(cMatrix& Rout, cMatrix& C1, complex<double>& c)
{ return Catan2(Rout, C1, c.real()); }


// interface fcn
bool Atan2(cMatrix& Aout, cMatrix& A1, cMatrix& A2)
{
	if (A1.IsReal() && A2.IsReal()) { return Atan2_RR(Aout, A1, A2); }
	else if (A1.IsReal() && !A2.IsReal()) { return Atan2_RC(Aout, A1, A2); }
	else if (!A1.IsReal() && A2.IsReal()) { return Atan2_CR(Aout, A1, A2); }
	else if (!A1.IsReal() && !A2.IsReal()) { return Atan2_CC(Aout, A1, A2); }
	else { addError("Atan2: internal error"); return true; }
}

bool Atan2_RR(cMatrix& Aout, cMatrix& R1, cMatrix& R2)
{
	double dv;

	if (SameSize(R1,R2)) return atan2(Aout,R1,R2);
	else if (R1.IsScalar()) { R1.Get0(dv); return atan2(Aout,dv,R2); }
	else if (R2.IsScalar()) { R2.Get0(dv); return atan2(Aout,R1,dv); }
	else { addError("incompatible argument sizes"); return true; }	// incompatible sizes
}

bool Atan2_RC(cMatrix& Aout, cMatrix& R1, cMatrix& C2)
{
	double dv;
	complex<double> cv;

	if (SameSize(R1,C2)) return atan2C(Aout,R1,C2);
	else if (R1.IsScalar()) { R1.Get0(dv); return atan2C(Aout,dv,C2); }
	else if (C2.IsScalar()) { C2.Get0(cv); return atan2C(Aout,R1,cv); }
	else { addError("incompatible argument sizes"); return true; }	// incompatible sizes
}

bool Atan2_CR(cMatrix& Aout, cMatrix& C1, cMatrix& R2)
{
	double dv;
	complex<double> cv;

	if (SameSize(C1,R2)) return Catan2(Aout,C1,R2);
	else if (C1.IsScalar()) { C1.Get0(cv); return Catan2(Aout,cv,R2); }
	else if (R2.IsScalar()) { R2.Get0(dv); return Catan2(Aout,C1,dv); }
	else { addError("incompatible argument sizes"); return true; }	// incompatible sizes
}

bool Atan2_CC(cMatrix& Aout, cMatrix& C1, cMatrix& C2)
{
	complex<double> cv;

	if (SameSize(C1,C2)) return Catan2C(Aout,C1,C2);
	else if (C1.IsScalar()) { C1.Get0(cv); return Catan2C(Aout,cv,C2); }
	else if (C2.IsScalar()) { C2.Get0(cv); return Catan2C(Aout,C1,cv); }
	else { addError("incompatible argument sizes"); return true; }	// incompatible sizes
}

//
// hyperbolic trig
//
complex<double> sinhC(complex<double>& c) {
	complex<double> ca = -1.*c;
	return (ExpC(c)-ExpC(ca))/2.;
}
bool sinhR(cMatrix& Rout, cMatrix& R)
{
	int i;
	if ( Rout.Create(R.GetRows(),R.GetColumns()) ) return true;

	double lf;
	for (i=0; i<R.GetSize(); i++) {
		lf = sinh(R.mr[i]);
		Rout.SetR( i, lf );
	}

	return false;
}
bool sinhC(cMatrix& Cout, cMatrix& C)
{
	int i;
	if ( Cout.Create(C.GetRows(),C.GetColumns()) ) return true;

	complex<double> c, ca;
	for (i=0; i<C.GetSize(); i++) {
		ca = complex<double>(C.mr[i],C.mi[i]);
		c = sinhC(ca);
		Cout.Set( i, c );
	}
	return false;
}
//
complex<double> coshC(complex<double>& c) {
	complex<double> ca = -1.*c;
	return ( (ExpC(c)-ExpC(ca))/2. );
}
bool coshR(cMatrix& Rout, cMatrix& R)
{
	int i;
	if ( Rout.Create(R.GetRows(),R.GetColumns()) ) return true;

	double lf;
	for (i=0; i<R.GetSize(); i++) {
		lf = cosh(R.mr[i]);
		Rout.SetR( i, lf );
	}

	return false;
}
bool coshC(cMatrix& Cout, cMatrix& C)
{
	int i;
	if ( Cout.CreateComplex(C.GetRows(),C.GetColumns()) ) return true;

	complex<double> c, ca;
	for (i=0; i<C.GetSize(); i++) {
		ca = complex<double>(C.mr[i],C.mi[i]);
		c = coshC(ca);
		Cout.Set( i, c );
	}
	return false;
}
//
complex<double> tanhC(complex<double>& c)	{ return sinhC(c)/coshC(c); }
bool tanhR(cMatrix& Rout, cMatrix& R)
{
	int i;
	if ( Rout.Create(R.GetRows(),R.GetColumns()) ) return true;

	double lf;
	for (i=0; i<R.GetSize(); i++) {
		lf = tanh(R.mr[i]);
		Rout.SetR( i, lf );
	}

	return false;
}
bool tanhC(cMatrix& Cout, cMatrix& C)
{
	int i;
	if ( Cout.CreateComplex(C.GetRows(),C.GetColumns()) ) return true;

	complex<double> c, ca;
	for (i=0; i<C.GetSize(); i++) {
		ca = complex<double>(C.mr[i],C.mi[i]);
		c = tanhC(ca);
		Cout.Set( i, c );
	}
	return false;
}
//
complex<double> asinhC(complex<double>& c)
{
	complex<double> ca = c*c+1.;
	complex<double> arg = c+CPow(ca,0.5);
	return logC( arg );
}
bool asinhR(cMatrix& Rout, cMatrix& R)
{
	int i;
	if ( Rout.Create(R.GetRows(),R.GetColumns()) ) return true;

	double lf;
	for (i=0; i<R.GetSize(); i++) {
		lf = asinh(R.mr[i]);
		Rout.SetR( i, lf );
	}

	return false;
}
bool asinhC(cMatrix& Cout, cMatrix& C)
{
	int i;
	if ( Cout.CreateComplex(C.GetRows(),C.GetColumns()) ) return true;

	complex<double> c, ca;
	for (i=0; i<C.GetSize(); i++) {
		ca = complex<double>(C.mr[i],C.mi[i]);
		c = asinhC(ca);
		Cout.Set( i, c );
	}
	return false;
}
//
double acoshPosR(double d)			{ return log( d+sqrt(d*d-1.) ); }			// d<=1
complex<double> acoshNegR(double d)	{
	complex<double> ca = d+sqrtNegR(d*d-1.);
	return logC( ca );
}		// d<1
complex<double> acoshC(complex<double>& c)
{
	complex<double> ca = c*c-complex<double>(1.,0);
	complex<double> arg = c+CPow(ca,0.5);
	return logC( arg );
}
bool acoshPosR(cMatrix& Rout, cMatrix& R)	// d<=1
{
	int i;
	if ( Rout.Create(R.GetRows(),R.GetColumns()) ) return true;

	const double *pr = R.GetR();
	double lf;
	for (i=0; i<R.GetSize(); i++) {
		lf = acoshPosR(pr[i]);
		Rout.SetR( i, lf );
	}

	return false;
}
bool acoshNegR(cMatrix& Cout, cMatrix& R)	// d<1
{
	int i;
	if ( Cout.CreateComplex(R.GetRows(),R.GetColumns()) ) return true;

	const double *pr = R.GetR();
	complex<double> c;
	for (i=0; i<R.GetSize(); i++) {
		c = acoshNegR(pr[i]);
		Cout.Set( i, c );
	}

	return false;
}
bool acoshR(cMatrix& Aout, cMatrix& R)
{
	cMatrix rn(R<1.);
	if (AnyR(rn)) return acoshNegR(Aout, R);
	else return acoshPosR(Aout, R);
}
bool acoshC(cMatrix& Cout, cMatrix& C)
{
	int i;
	if ( Cout.CreateComplex(C.GetRows(),C.GetColumns()) ) return true;

	complex<double> c, ca;
	for (i=0; i<C.GetSize(); i++) {
		ca = complex<double>(C.mr[i],C.mi[i]);
		c = acoshC(ca);
		Cout.Set( i, c );
	}
	return false;
}
//
double atanhPosR(double d)		{ return log( (1.+d)/(1.-d) )/2.; }		// |d|<1
complex<double> atanhNegR(double d)		{ return Clog( (1.+d)/(1.-d) )/complex<double>(2.,0.); }		// |d|>1
complex<double> atanhC(complex<double>& c)
{
	complex<double> arg = (1.+c)/(1.-c);
	return logC( arg );
}
bool atanhPosR(cMatrix& Rout, cMatrix& R)	// d<=1
{
	int i;
	if ( Rout.Create(R.GetRows(),R.GetColumns()) ) return true;

	const double *pr = R.GetR();
	for (i=0; i<R.GetSize(); i++)
		Rout.SetR( i, atanhPosR(pr[i]) );

	return false;
}
bool atanhNegR(cMatrix& Cout, cMatrix& R)	// d<1
{
	int i;
	if ( Cout.CreateComplex(R.GetRows(),R.GetColumns()) ) return true;

	complex<double> c;
	for (i=0; i<R.GetSize(); i++) {
		c = atanhNegR(R.mr[i]);
		Cout.Set( i, c );
	}

	return false;
}
bool atanhR(cMatrix& Aout, cMatrix& R)
{
	cMatrix M;
	if (AbsR(M,R)) return true;

	cMatrix mg(M>1.);
	if (AnyR(mg)) return acoshNegR(Aout, R);
	else return acoshPosR(Aout, R);
}
bool atanhC(cMatrix& Cout, cMatrix& C)
{
	int i;
	if ( Cout.CreateComplex(C.GetRows(),C.GetColumns()) ) return true;

	complex<double> c, ca;
	for (i=0; i<C.GetSize(); i++) {
		ca = complex<double>(C.mr[i],C.mi[i]);
		c = atanhC(ca);
		Cout.Set( i, c );
	}
	return false;
}
//
double csch(double d)						{ return (1./sinh(d)); }
complex<double> cschC(complex<double>& c)	{ return (1./sinhC(c)); }
bool cschR(cMatrix& Rout, cMatrix& R)
{
	int i;
	if ( Rout.Create(R.GetRows(),R.GetColumns()) ) return true;

	const double *pr = R.GetR();
	for (i=0; i<R.GetSize(); i++)
		Rout.SetR( i, csch(R.mr[i]) );

	return false;
}
bool cschC(cMatrix& Cout, cMatrix& C)
{
	int i;
	if ( Cout.CreateComplex(C.GetRows(),C.GetColumns()) ) return true;

	complex<double> c, ca;
	for (i=0; i<C.GetSize(); i++) {
		ca = complex<double>(C.mr[i],C.mi[i]);
		c = cschC(ca);
		Cout.Set( i, c );
	}
	return false;
}
//
double sech(double d)						{ return (1./cosh(d)); }
complex<double> sechC(complex<double>& c)	{ return (1./coshC(c)); }
bool sechR(cMatrix& Rout, cMatrix& R)
{
	int i;
	if ( Rout.Create(R.GetRows(),R.GetColumns()) ) return true;

	//const double *pr = R.GetR();
	for (i=0; i<R.GetSize(); i++)
		Rout.SetR( i, sech(R.mr[i]) );

	return false;
}
bool sechC(cMatrix& Cout, cMatrix& C)
{
	int i;
	if ( Cout.CreateComplex(C.GetRows(),C.GetColumns()) ) return true;

	complex<double> c, ca;
	for (i=0; i<C.GetSize(); i++) {
		ca = complex<double>(C.mr[i],C.mi[i]);
		c = sechC(ca);
		Cout.Set( i, c );
	}
	return false;
}
//
double coth(double d)						{ return (1./tanh(d)); }
complex<double> cothC(complex<double>& c)	{ return (1./tanhC(c)); }
bool cothR(cMatrix& Rout, cMatrix& R)
{
	int i;
	if ( Rout.Create(R.GetRows(),R.GetColumns()) ) return true;

	for (i=0; i<R.GetSize(); i++)
		Rout.SetR( i, coth(R.mr[i]) );

	return false;
}
bool cothC(cMatrix& Cout, cMatrix& C)
{
	int i;
	if ( Cout.CreateComplex(C.GetRows(),C.GetColumns()) ) return true;

	complex<double> c, ca;
	for (i=0; i<C.GetSize(); i++) {
		ca = complex<double>(C.mr[i],C.mi[i]);
		c = cothC(ca);
		Cout.Set( i, c );
	}
	return false;
}
//
double acsch(double d)						{ return (asinh(1./d)); }
complex<double> acschC(complex<double>& c) {
	complex<double> ca = 1./c;
	return asinhC(ca);
}
bool acschR(cMatrix& Rout, cMatrix& R)
{
	int i;
	if ( Rout.Create(R.GetRows(),R.GetColumns()) ) return true;

	for (i=0; i<R.GetSize(); i++)
		Rout.SetR( i, acsch(R.mr[i]) );

	return false;
}
bool acschC(cMatrix& Cout, cMatrix& C)
{
	int i;
	if ( Cout.CreateComplex(C.GetRows(),C.GetColumns()) ) return true;

	complex<double> c, ca;
	for (i=0; i<C.GetSize(); i++) {
		ca = complex<double>(C.mr[i],C.mi[i]);
		c = acschC(ca);
		Cout.Set( i, c );
	}
	return false;
}
//
complex<double> asech(double d)
{
	complex<double> c(1./d,0.);
	return acoshC(c);
}
complex<double> asechC(complex<double>& c)
{
	return acoshC(c);
}
bool asechR(cMatrix& Cout, cMatrix& R)
{
	int i;
	if ( Cout.CreateComplex(R.GetRows(),R.GetColumns()) ) return true;

	complex<double> c;
	for (i=0; i<R.GetSize(); i++) {
		c = asech(R.mr[i]);
		Cout.Set( i, c );
	}

	return false;
}
bool asechC(cMatrix& Cout, cMatrix& C)
{
	int i;
	if ( Cout.CreateComplex(C.GetRows(),C.GetColumns()) ) return true;

	complex<double> c, ca;
	for (i=0; i<C.GetSize(); i++) {
		ca = complex<double>(C.mr[i],C.mi[i]);
		c = asechC(ca);
		Cout.Set( i, c );
	}
	return false;
}
//
complex<double> acoth(double d)
{
	complex<double> c(1./d,0.);
	return atanhC(c);
}
complex<double> acothC(complex<double>& c) {
	complex<double> ca = 1./c;
	return atanhC(ca);
}
bool acothR(cMatrix& Cout, cMatrix& R)
{
	int i;
	if ( Cout.CreateComplex(R.GetRows(),R.GetColumns()) ) return true;

	complex<double> c;
	for (i=0; i<R.GetSize(); i++) {
		c = acoth(R.mr[i]);
		Cout.Set( i, c );
	}

	return false;
}
bool acothC(cMatrix& Cout, cMatrix& C)
{
	int i;
	if ( Cout.CreateComplex(C.GetRows(),C.GetColumns()) ) return true;

	complex<double> c, ca;
	for (i=0; i<C.GetSize(); i++) {
		ca = complex<double>(C.mr[i],C.mi[i]);
		c = acothC(ca);
		Cout.Set( i, c );
	}
	return false;
}

