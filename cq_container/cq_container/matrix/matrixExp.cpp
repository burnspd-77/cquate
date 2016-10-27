
#include "matrix.h"
#include "my_errors.h"


//
// logarithm
//

complex<double> ExpC(complex<double>& x)
{
	double rm = exp(x.real());
	double ph = x.imag();
	return std::polar( rm, ph );
}

bool ExpR(cMatrix& Result, cMatrix& R)
{
	if ( Result.Create(R.GetRows(),R.GetColumns()) ) return true;

	//double *pr = Result.GetR();
	//const double *prr = R.GetSafeR();
	int n = R.GetSize();

	for (int i=0; i<n; i++) {
		Result.mr[i] = exp( R.mr[i] );
	}
	return false;
}

bool ExpC(cMatrix& Result, cMatrix& C)
{
	if ( Result.CreateComplex(C.GetRows(),C.GetColumns()) ) return true;

	//double *pr = Result.GetR();
	//double *pi = Result.GetI();
	//const double *pcr = C.GetSafeR();
	//const double *pci = C.GetSafeI();
	complex<double> cv, carg;
	int n = C.GetSize();

	for (int i=0; i<n; i++) {
		carg = complex<double>( C.mr[i], C.mi[i] );
		cv = ExpC( carg );
		Result.mr[i] = cv.real();
		Result.mi[i] = cv.imag();
	}
	return false;
}

// interface fcn
bool Exp(cMatrix& Result, cMatrix& A)
{
	if (A.IsReal()) return ExpR(Result, A);
	else return ExpC(Result,A);
}


complex<double> Clog(double r)
{
	complex<double> result;
	double dv;

	dv = log(fabs(r));

	if (r>0) result = complex<double>(dv,0.);
	// r<0: result is complex
	else if (r<0) result = complex<double>(dv,PI);	// check this...
	// r==0
	else result = complex<double>( std::numeric_limits<double>::quiet_NaN(), 0. );

	return result;
}

complex<double> logC(complex<double>& cv)
{
	complex<double> result, pol;

	double la = log( abs(cv) );
	double t = arg(cv);
	result = complex<double>( la, arg(cv) );		// polar to rect

	return result;
}

bool logPosR(cMatrix& Rout, cMatrix& R)
{	// log for matrix with all non-negative real values
	int i;
	if ( Rout.Create(R.GetRows(),R.GetColumns()) ) return true;

	//const double *pr = R.GetR();
	for (i=0; i<R.GetSize(); i++)
		Rout.SetR( i, log(R.mr[i]) );

	return false;
}

bool logNegR(cMatrix& Cout, cMatrix& R)
{	// log for matrix with some negative real values
	int i;
	if ( Cout.CreateComplex(R.GetRows(),R.GetColumns()) ) return true;

	complex<double> cx;
	//const double *pr = R.GetR();
	for (i=0; i<R.GetSize(); i++) {
		cx = Clog(R.mr[i]);
		Cout.Set( i, cx );
	}

	return false;
}

bool logR(cMatrix& Aout, cMatrix& R)
{
	cMatrix rn(R<0);
	if (AnyR(rn))
		return logNegR(Aout,R);
	else
		return logPosR(Aout,R);
}

bool logC(cMatrix& Cout, cMatrix& C)
{
	int i;
	if ( Cout.CreateComplex(C.GetRows(),C.GetColumns()) ) return true;

	//const double *pr = C.GetR();
	//const double *pi = C.GetI();
	complex<double> c, cx;
	for (i=0; i<C.GetSize(); i++) {
		c = complex<double>(C.mr[i],C.mi[i]);
		cx = logC(c);
		Cout.Set( i, cx );
	}

	return false;
}

double logBase(double x, const double base) { return log(x)/log(base); }

complex<double> logBaseC(complex<double>& c, const double base) { return logC(c)/log(base); }

bool logBaseR(cMatrix& Aout, cMatrix& R, const double base)
{
	if (base<=0) { addError("log base must be greater than 0"); return true; }

	cMatrix A, rn(R<0);
	if (AnyR(rn)) {
		if (logNegR(A,R)) return true;
	}
	else {
		if (logPosR(A,R)) return true;
	}

	//Aout = A/log(base);
	DotDiv(Aout,A,log(base));
	return false;
}
bool logBaseC(cMatrix& Cout, cMatrix& C, const double base)
{
	if (base<=0) { addError("log base must be greater than 0"); return true; }

	cMatrix A;
	if (logC(A,C)) return true;

	//Cout = A/log(base);
	DotDiv(Cout,A,log(base));
	return false;
}

// base 2 logarithm of an integer, which is an even power of 2
int Log2(int i)
{
	double t = (double)i;
	double l = log(t)/log(2.);
	double d1 = l-floor(l);
	double d2 = ceil(l)-l;
	if (d1<=d2) { return (int)floor(l); }
	else { return (int)ceil(l); }
}

//
// sqrt
//
bool sqrtR(cMatrix& Res, const cMatrix& R)
{
	if (Res.Create(R.GetRows(),R.GetColumns())) return true;

	int n = R.GetSize();
	//double *pr = Res.GetR();
	//const double *prr = R.GetSafeR();

	for (int i=0; i<n; ++i) {
		Res.mr[i] = sqrt( R.mr[i] );
	}
	return false;
}

complex<double> sqrtNegR(double d)
{	// square root which can handle pos or neg numbers
	double ang;
	if (d>=0) ang = 0.;
	else ang = PI/2.;
	return std::polar( sqrt(fabs(d)), ang );
}

bool sqrtNegR(cMatrix& Res, const cMatrix& R)
{
	if (Res.CreateComplex(R.GetRows(),R.GetColumns())) return true;

	int n = R.GetSize();
	//double *pr = Res.GetR(), *pi = Res.GetI();
	//const double *prr = R.GetSafeR();
	complex<double> cv;

	for (int i=0; i<n; ++i) {
		cv = sqrtNegR(R.mr[i]);
		Res.mr[i] = cv.real();
		Res.mi[i] = cv.imag();
	}
	return false;
}

complex<double> sqrtC(complex<double>& x)
{
	//double mag = sqrt(abs(x));
	//double ang = arg(x);
	//Complex Result = Polar2Rect(Complex(mag,ang/2.));
	return std::polar( sqrt(abs(x)), arg(x)/2. );
}

bool sqrtC(cMatrix& Res, const cMatrix& C)
{
	if (Res.CreateComplex(C.GetRows(),C.GetColumns())) return true;

	int n = C.GetSize();
	//double *pr = Res.GetR(), *pi = Res.GetI();
	//const double *pcr = C.GetSafeR(), *pci = C.GetSafeI();
	complex<double> cv;

	for (int i=0; i<n; ++i) {
		cv = complex<double>(C.mr[i],C.mi[i]);
		cv = sqrtC(cv);
		Res.mr[i] = cv.real();
		Res.mi[i] = cv.imag();
	}
	return false;
}

// interface fcn
bool Sqrt(cMatrix& Result, cMatrix& x)
{
	cMatrix xn;
	if (x.IsReal()) {
		xn = x<0.;
		if (AnyR(xn)) return sqrtNegR(Result,x);
		else return sqrtR(Result,x);
	}
	else return sqrtC(Result,x);
}


//// is i an even power of 2?
//bool IsPow2(int i)
//{
//	bool flag=false;
//	int k=0;
//	int lval=1;
//
//	// manually search for
//	while (!flag) {
//		if (lval==i) { flag=true; return true; }
//		else if (k>=MAX_LOG2) { lval=0; return false; }
//		lval = lval*2;
//		k++;
//	}
//	return false;
//}
