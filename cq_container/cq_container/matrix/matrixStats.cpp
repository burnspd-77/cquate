
#include "matrix.h"
#include "random.h"
#include "my_errors.h"


//
// helpers
//

// is i an even power of 2?
bool IsPow2(int i)
{
	bool flag=false;
	int k=0;
	int lval=1;

	// manually search for
	while (!flag) {
		if (lval==i) { flag=true; return true; }
		else if (k>=31) { lval=0; return false; }
		lval = lval*2;
		k++;
	}
	return false;
}

bool IsEven(int i)
{
	if (i==(2*(i/2))) { return true; }
	else { return false; }
}

bool IsOdd(int i)
{
	if (IsEven(i)) return false;
	else return true;
}


bool NandR(double val, double *pN, double *pr, int iGranularity)
{
	double sign_val, sign_N, N, NmodG;

	if (val!=0) {		// is data value non-zero?
		N = Fix( log10(fabs(val)) );
		//N = floor( log10(fabs(val)) );
		if (N==0.) sign_N = 1.;
		else sign_N = N/(fabs(N));

		if (iGranularity==1) {
			*pN = N;
		}
		else {
			NmodG = fmod(N, double(iGranularity) );
			*pN = N - NmodG;
		}

		sign_val = val/(fabs(val));
		*pr = sign_val * fabs(val)/pow(10.0,*pN);			// 1.0 < sign*r <= 10.0
	}
	else {				// data is zero
		*pN = *pr = 0.0;
		return false;
	}

	return false;
}

complex<double> Polar2Rect(const complex<double>& pol)
{
	return complex<double>(pol.real()*cos(pol.imag()), pol.real()*sin(pol.imag()));
}
complex<double> Rect2Polar(const complex<double>& rect)
{
	double r = sqrt( rect.real()*rect.real() + rect.imag()*rect.imag() );
	double a = atan2( rect.imag(), rect.real() );
	return complex<double>(r,a);
}


double Factorial(int n)
{
	double lf(1.);
	if (n<0) return 0.;

	for (int i=1; i<=n; ++i) {
		lf *= (double) i;
		if (std::fpclassify(lf)==FP_NAN) break;
	}
	return lf;
}

bool FactorialR(cMatrix& Result, cMatrix& R)
{
	if (Result.Create(R.GetRows(),R.GetColumns())) return true;

	//double *pr = Result.GetR();
	//const double *par = R.GetSafeR();
	int n = R.GetSize();
	for (int i=0; i<n; ++i)
		Result.mr[i] = Factorial( (int)R.mr[i] );

	return false;
}

bool Factorial(cMatrix& Result, cMatrix& A)
{	// ignore imag part
	return FactorialR(Result,A);
}

// Fix: round towards negative infinity
double Fix(double val)
{
	return double( int(val) );
}

bool Fix(cMatrix& Result, cMatrix& A)
{	// "truncate"
	if (Result.Create(A.GetRows(),A.GetColumns())) return true;

	// ignore imag part
	//double *pr = Result.GetR();
	//const double *par = A.GetSafeR();
	int n = A.GetSize();
	for (int i=0; i<n; ++i)
		Result.mr[i] = Fix( A.mr[i] );

	return false;
}

bool Round(cMatrix& Result, cMatrix& A)
{
	cMatrix B;
	Add(B, A, 0.5);
	return Floor(Result, B);
}

double Floor(double val)
{
	return floor(val);
}

bool Floor(cMatrix& Result, cMatrix& A)
{
	if (Result.Create(A.GetRows(),A.GetColumns())) return true;

	// ignore imag part
	//double *pr = Result.GetR();
	//const double *par = A.GetSafeR();
	int n = A.GetSize();
	for (int i=0; i<n; ++i)
		Result.mr[i] = floor( A.mr[i] );

	return false;
}

bool Ceil(cMatrix& Result, cMatrix& A)
{
	if (Result.Create(A.GetRows(),A.GetColumns())) return true;

	// ignore imag part
	//double *pr = Result.GetR();
	//const double *par = A.GetSafeR();
	int n = A.GetSize();
	for (int i=0; i<n; ++i)
		Result.mr[i] = ceil( A.mr[i] );

	return false;
}

bool Phase(cMatrix& Result, cMatrix& A)
{
	if (A.IsReal()) { return ZerosR(Result,A.GetRows(),A.GetColumns()); }

	if (Result.Create(A.GetRows(),A.GetColumns())) return true;

	// ignore imag part
	//double *pr = Result.GetR();
	//const double *par = A.GetSafeR();
	//const double *pai = A.GetSafeI();
	int n = A.GetSize();
	for (int i=0; i<n; ++i)
		Result.mr[i] = atan2( A.mi[i], A.mr[i] );

	return false;
}

// Sign: find the signum of the arg
int Sign(double val)
{
	if (val>0.) return 1;
	else if (val==0.) return 0;
	else return -1;
}

// Round: round to nearest integer
int Round(double val)
{
	double d = val + double(Sign(val)) * 0.5;
	return int(d);
}

// Mod:
// mod(x,y) - if x is complex, real and imag parts are both modded by y
//			  if y is complex, imag part is ignored (thus y treated as real)
//

bool Mod(cMatrix& Result, cMatrix& x, cMatrix& y)
{
	if (SameSize(x,y))
	{
		if (Result.Create(x.GetRows(),x.GetColumns())) return true;
		//pr = Result.GetR();
		//pxr = x.GetSafeR(); pyr = y.GetSafeR();
		for (int i=0; i<x.GetSize(); ++i) { Result.mr[i] = fmod( x.mr[i], y.mr[i] ); }
		return false;
	}
	else if (x.IsScalar())
	{
		if (Result.Create(y.GetRows(),y.GetColumns())) return true;
		//pr = Result.GetR();
		//x.GetSafe0(dv); pyr = y.GetSafeR();
		for (int i=0; i<x.GetSize(); ++i) { Result.mr[i] = fmod( x.mr[0], y.mr[i] ); }
		return false;
	}
	else if (y.IsScalar())
	{
		if (Result.Create(x.GetRows(),x.GetColumns())) return true;
		//pr = Result.GetR();
		//pxr = x.GetSafeR(); y.GetSafe0(dv);
		for (int i=0; i<x.GetSize(); ++i) { Result.mr[i] = fmod( x.mr[i], y.mr[0] ); }
		return false;
	}
	else
	{	// non-compatible sizes
		addError("incompatable matrix dimensions");
		return true;
	}
}

bool Mod(cMatrix& Result, double x, cMatrix& y)
{
	if (Result.Create(y.GetRows(),y.GetColumns())) return true;
	for (int i=0; i<y.GetSize(); ++i) { Result.mr[i] = fmod( x, y.mr[i] ); }

	return false;
}

bool Mod(cMatrix& Result, cMatrix& x, double y)
{
	if (Result.Create(x.GetRows(),x.GetColumns())) return true;
	for (int i=0; i<x.GetSize(); ++i) { Result.mr[i] = fmod( x.mr[i], y ); }

	return false;
}

////
//// sum/prod/cumulative
////
//bool Sum(cMatrix& Cout, cMatrix& v)
//{
//	v.Sum(Cout, 2);	// down columns by default
//	return false;
//}
//
//bool Sum(cMatrix& Cout, cMatrix& v, cMatrix& dim)
//{
//	int idim;
//	if (dim.IsScalar()) { idim = (int) dim.Get0R(); }
//	else { addError("scalar argument expected"); return true; }
//	return v.Sum(Cout, idim);
//}
//
//bool CumSum(cMatrix& Cout, cMatrix& v)
//{
//	return v.CumSum(Cout, 2);		// down columns by default
//}
//
//bool CumSum(cMatrix& Cout, cMatrix& v, cMatrix& dim)
//{
//	int idim;
//	if (dim.IsScalar()) { idim = (int) dim.Get0R(); }
//	else { addError("scalar argument expected"); return true; }
//	return v.CumSum(Cout, idim);
//}
//
//bool Prod(cMatrix& Cout, cMatrix& v)
//{
//	return v.Prod(Cout, 2);	// down columns by default
//}
//
//bool Prod(cMatrix& Cout, cMatrix& v, cMatrix& dim)
//{
//	int idim;
//	if (dim.IsScalar()) { idim = (int) dim.Get0R(); }
//	else { addError("scalar argument expected"); return true; }
//	return v.Prod(Cout, idim);
//}
//
//bool CumProd(cMatrix& Cout, cMatrix& v)
//{
//	return v.CumProd(Cout, 2);	// down columns by default
//}
//
//bool CumProd(cMatrix& Cout, cMatrix& v, cMatrix& dim)
//{
//	int idim;
//	if (dim.IsScalar()) { idim = (int) dim.Get0R(); }
//	else { addError("scalar argument expected"); return true; }
//	return v.CumProd(Cout, idim);
//}

// Min, Max, Abs, Phase
int Min(int i1, int i2)
{
	if (i1<i2) return i1;
	else return i2;
}

double Min(double d1, double d2)
{
	if (d1<d2) return d1;
	else return d2;
}

int Max(int i1, int i2)
{
	if (i1>i2) return i1;
	else return i2;
}

double Max(double d1, double d2)
{
	if (d1>d2) return d1;
	else return d2;
}


bool MaxR(cMatrix& Result, cMatrix& mat)
{
	if (mat.IsEmpty()) return false;

	double dmax;
	mat.GetR(dmax,0,0);
	int n = mat.GetSize();
	//const double *pr = mat.GetSafeR();

	for (int i=0; i<n; i++) {
		if (dmax<mat.mr[i]) { dmax=mat.mr[i]; }
	}

	if (Result.SetTo(dmax)) return true;
	return false;
}

bool MaxR(cMatrix& Result, cMatrix& mat, double lf)
{
	if ( Result.Create(mat.GetRows(),mat.GetColumns()) ) { return true; }

	//double *pr = Result.GetR();
	//const double *pm = mat.GetSafeR();
	for (int i=0; i<mat.GetSize(); ++i)
	{
		Result.mr[i] = Max( mat.mr[i], lf );
	}

	return false;
}

bool RMatMaxR(cMatrix& Result, cMatrix& mat1, cMatrix& mat2)
{	// mat1 and mat2 are same size, assume checked before this call
	if ( Result.Create(mat1.GetRows(),mat1.GetColumns()) ) { return true; }

	//double *pr = Result.GetR();
	//const double *pm1 = mat1.GetSafeR();
	//const double *pm2 = mat2.GetSafeR();
	for (int i=0; i<mat1.GetSize(); ++i)
	{
		Result.mr[i] = Max( mat1.mr[i], mat2.mr[i] );
	}

	return false;
}

bool RMaxR(cMatrix& Result, cMatrix& mat1, cMatrix& mat2)
{
	double lf;

	if (mat1.IsScalar()) {
		lf = mat1.Get0R();
		return MaxR(Result, mat2, lf);
	}
	else if (mat2.IsScalar()) {
		lf = mat2.Get0R();
		return MaxR(Result, mat1, lf);
	}
	else {
		if ( mat1.GetRows()!=mat2.GetRows() || mat1.GetColumns()!=mat2.GetColumns() ) { addError("dimension mismatch"); return true; }		// size mismatch
		return RMatMaxR(Result, mat1, mat2);
	}

	return false;
}

bool MaxNumberR(cMatrix& Result, cMatrix& mat)
{
	if (mat.IsEmpty()) return false;

	double dmax;
	mat.GetR(dmax,0,0);
	int n = mat.GetSize();
	//const double *pr = mat.GetSafeR();

	double d = DBL_MIN;
	bool bCheck = true;

	for (int i=0; i<n; i++) {
		d = mat.mr[i];
		if ( std::fpclassify(d)==FP_NORMAL ) {
				bCheck = false;
				if (dmax<d) { dmax=d; }
		}
	}

	if (Result.SetTo(dmax)) return true;
	return bCheck;
}

bool MaxVectR(cMatrix& Result, cMatrix& mat)
{
	if (mat.IsEmpty()) return false;
	if (Result.Create(1,mat.GetColumns())) return true;

	double d, v;
	int i, j, nc = mat.GetColumns(), nr = mat.GetRows();
	//const double *pr = mat.GetSafeR();

	for (j=0; j<nc; j++) { mat.GetR(d,0,j); Result.SetR(0,j,d); }

	for (j=0; j<nc; j++) {
		Result.GetR(v,0,j);
		for (i=1; i<nr; i++) {
			d = mat.mr[i+j*nr];
			if (v<d) { v = d; }
		}
		Result.SetR(0,j,v);
	}
	return false;
}

bool MaxIndexR(cMatrix& Result, cMatrix& ResIdx, cMatrix& mat)
{	// index is in column major order (not a row index per se)
	if (Result.Create(1,1)) return true;
	if (ResIdx.Create(1,1)) return true;

	double dIdx;

	if (mat.IsEmpty()) return false;

	double dval;
	mat.GetR(dval,0,0);
	dIdx = 1.;
	int n = mat.GetSize();
	//const double *pr = mat.GetSafeR();

	for (int i=0; i<n; i++) {
		if (dval<mat.mr[i]) { dval=mat.mr[i]; dIdx = double(i); }
	}

	Result.SetR(0,dval);
	ResIdx.SetR(0,dIdx);
	return false;
}

bool MaxVectIndexR(cMatrix& Result, cMatrix& ResIdx, cMatrix& mat)
{	// result and indices are vectors
	if (mat.IsEmpty()) return false;

	int r;		// row of max value
	int i, j;
	double d, v;
	int nr = mat.GetRows(), nc = mat.GetColumns();

	if (Result.Create(1,nc)) return true;
	if (ResIdx.Create(1,nc)) return true;

	// initialize max values
	for (j=0; j<nc; j++) {
		mat.GetR(d,0,j);
		Result.SetR( 0, j, d );
		ResIdx.SetR( 0, j, 0 );
	}

	const double *pr = mat.GetSafeR();

	for (j=0; j<nc; j++) {
		Result.GetR(v,0,j);
		r = 0;
		for (i=1; i<nr; i++) {

			d = pr[i+j*nr];
			if (v<d) {
				v = d;
				r=i;
			}

		}
		ResIdx.SetR(0,j, double(r));
		Result.SetR(0,j, v);
	}
	return false;
}

bool MinR(cMatrix& Result, cMatrix& mat)
{
	if (mat.IsEmpty()) return false;

	double dmin;
	mat.GetR(dmin,0,0);
	int n = mat.GetSize();
	//const double *pr = mat.GetSafeR();

	for (int i=0; i<n; i++) {
		if (dmin>mat.mr[i]) { dmin=mat.mr[i]; }
	}

	if (Result.SetTo(dmin)) return true;
	return false;
}

bool MinR(cMatrix& Result, cMatrix& mat, double lf)
{
	if ( Result.Create(mat.GetRows(),mat.GetColumns()) ) { return true; }

	//double *pr = Result.GetR();
	//const double *pm = mat.GetSafeR();
	for (int i=0; i<mat.GetSize(); ++i)
	{
		Result.mr[i] = Min( mat.mr[i], lf );
	}

	return false;
}

bool RMatMinR(cMatrix& Result, cMatrix& mat1, cMatrix& mat2)
{	// mat1 and mat2 are same size, assume checked before this call
	if ( Result.Create(mat1.GetRows(),mat1.GetColumns()) ) { return true; }

	//double *pr = Result.GetR();
	//const double *pm1 = mat1.GetSafeR();
	//const double *pm2 = mat2.GetSafeR();
	for (int i=0; i<mat1.GetSize(); ++i)
	{
		Result.mr[i] = Min( mat1.mr[i], mat2.mr[i] );
	}

	return false;
}

bool RMinR(cMatrix& Result, cMatrix& mat1, cMatrix& mat2)
{
	double lf;

	if (mat1.IsScalar()) {
		lf = mat1.Get0R();
		return MinR(Result, mat2, lf);
	}
	else if (mat2.IsScalar()) {
		lf = mat2.Get0R();
		return MinR(Result, mat1, lf);
	}
	else {
		if ( mat1.GetRows()!=mat2.GetRows() || mat1.GetColumns()!=mat2.GetColumns() ) { addError("dimension mismatch"); return true; }		// size mismatch
		return RMatMinR(Result, mat1, mat2);
	}

	return false;
}

bool MinNumberR(cMatrix& Result, cMatrix& mat)
{
	if (mat.IsEmpty()) return false;

	double dmin;
	mat.GetR(dmin,0,0);
	int n = mat.GetSize();
	//const double *pr = mat.GetSafeR();

	double d = DBL_MAX;
	bool bCheck = true;

	for (int i=0; i<n; i++) {
		d = mat.mr[i];
		if ( std::fpclassify(d)==FP_NORMAL ) {
				bCheck = false;
				if (dmin>d) { dmin=d; }
		}
	}

	if (Result.SetTo(dmin)) return true;
	return bCheck;
}

bool MinVectR(cMatrix& Result, cMatrix& mat)
{
	if (mat.IsEmpty()) return false;
	if (Result.Create(1,mat.GetColumns())) return true;

	double d, v;
	int i, j, nc = mat.GetColumns(), nr = mat.GetRows();
	//const double *pr = mat.GetSafeR();

	for (j=0; j<nc; j++) { mat.GetR(d,0,j); Result.SetR(0,j,d); }

	for (j=0; j<nc; j++) {
		Result.GetR(v,0,j);
		for (i=1; i<nr; i++) {
			d = mat.mr[i+j*nr];
			if (v>d) { v = d; }
		}
		Result.SetR(0,j,v);
	}
	return false;
}

bool MinIndexR(cMatrix& Result, cMatrix& ResIdx, cMatrix& mat)
{	// index is in column major order (not a row index per se)
	if (Result.Create(1,1)) return true;
	if (ResIdx.Create(1,1)) return true;

	double dIdx;

	if (mat.IsEmpty()) return false;

	double dval;
	mat.GetR(dval,0,0);
	dIdx = 1.;
	int n = mat.GetSize();
	//const double *pr = mat.GetSafeR();

	for (int i=0; i<n; i++) {
		if (dval>mat.mr[i]) { dval=mat.mr[i]; dIdx = double(i); }
	}

	Result.SetR(0,dval);
	ResIdx.SetR(0,dIdx);
	return false;
}

bool MinVectIndexR(cMatrix& Result, cMatrix& ResIdx, cMatrix& mat)
{	// result and indices are vectors
	if (mat.IsEmpty()) return false;

	int r;		// row of max value
	int i, j;
	double d, v;
	int nr = mat.GetRows(), nc = mat.GetColumns();

	if (Result.Create(1,nc)) return true;
	if (ResIdx.Create(1,nc)) return true;

	// initialize min values
	for (j=0; j<nc; j++) {
		mat.GetR(d,0,j);
		Result.SetR( 0, j, d );
		ResIdx.SetR( 0, j, 0 );
	}

	//const double *pr = mat.GetSafeR();

	for (j=0; j<nc; j++) {
		Result.GetR(v,0,j);
		r = 0;
		for (i=1; i<nr; i++) {

			d = mat.mr[i+j*nr];
			if (v>d) {
				v = d;
				r=i;
			}

		}
		ResIdx.SetR(0,j, double(r));
		Result.SetR(0,j, v);
	}
	return false;
}

bool AbsR(cMatrix& Result, cMatrix& R)
{
	if (Result.Create(R.GetRows(),R.GetColumns())) return true;

	int n = R.GetSize();
	complex<double> c;
	//double *pr = Result.GetR();
	//const double *pargr = R.GetSafeR();
	for (int i=0; i<n; i++) {
		Result.mr[i] = fabs(R.mr[i]);
	}
	return false;
}
bool Abs(cMatrix& Result, cMatrix& A)
{
	if (A.IsReal()) return AbsR(Result,A);

	if (Result.Create(A.GetRows(),A.GetColumns())) return true;

	int n = A.GetSize();
	complex<double> c;
	//double *pr = Result.GetR();
	//const double *pargr = A.GetSafeR();
	//const double *pargi = A.GetSafeI();
	for (int i=0; i<n; i++) {
		c = complex<double>(A.mr[i],A.mi[i]);
		Result.mr[i] = abs(c);
	}
	return false;
}


//
// rand/randn
//
bool Rand(cMatrix& Result, int r, int c)
{
	// arg checking
	if (r<1 || c<1) { addError("dimensions must be positive"); return true; }

	if (r*c>1) return MatRandR(Result, r, c);
	else return Result.SetTo( DRand() );
}

bool Rand(cMatrix& Result, int d)
{
	// arg checking
	if (d<1) { addError("dimension must be positive"); return true; }

	if (d>1) return MatRandR(Result, d, d);
	else return Result.SetTo( DRand() );
}

bool Randn(cMatrix& Result, int r, int c)
{
	// arg checking
	if (r<1 || c<1) { addError("dimension must be positive"); return true; }

	int No2;
	bool bOdd = false;

	if (IsOdd(r*c)) {
		bOdd = true;
		No2 = int( (r*c)/2 ) + 1;
	}
	else No2 = int( (r*c)/2 );

	cMatrix U, V, Res1, Res2;
	cMatrix S, T, Tmp1, Tmp2, Tmp3;
	cMatrix arg;

	if (MatRandR(U,No2,1)) return true;
	if (MatRandR(V,No2,1)) return true;

	if (logR(Tmp1,V)) return true;
	if (Real(Tmp2,Tmp1)) return true;
	Mult(S, Tmp2 , -2.);
	if (S.IsEmpty()) { addError("grand: internal error"); return true; }

	V.Delete();		// free up memory

	if (RDotPowR(T,S,0.5)) return true;
	Mult(arg, U, (2.*PI));

	S.Delete();		// free up memory
	U.Delete();
	Tmp1.Delete();

	if (cosR(Tmp2,arg)) return true;
	if (sinR(Tmp3,arg)) return true;

	if ( RDotMultR(Res1, T, Tmp2) ) return true;
	if ( RDotMultR(Res2, T, Tmp3) ) return true;

	T.Delete();		// free up memory
	Tmp2.Delete();
	Tmp3.Delete();
	
	// initialize Result
	if (Result.Create(r,c)) return true;

	Result.FillR( 0, Res1.mr.begin(), No2 );
	//if (memcpy( Result.GetR(), Res1.GetR(), No2*sizeof(double) )==NULL) { addError("out of memory"); return true; }

	//int N_offset = No2*sizeof(double);
	//void *p = (void *)(&Result.mr[No2]);

	if (bOdd) { Result.FillR( No2, Res2.mr.begin(), No2-1 ); }
	else { Result.FillR( No2, Res2.mr.begin(), No2 ); }
	//if (bOdd) { memcpy( p, Res2.GetR(), (No2-1)*sizeof(double) ); }
	//else { memcpy( p, Res2.GetR(), No2*sizeof(double) ); }

	return false;
}

bool Randn(cMatrix& Result, int d)
{ return Randn(Result, d, d); }

// interface fcns
bool RandSeed(cMatrix& seed)
{
	if (!seed.IsScalar()) { addError("illegal argument"); return true; }

	unsigned int uiseed = (unsigned int) seed.Get0R();

	MersenneSeed(uiseed);
	return false;
}

bool Rand(cMatrix& Result, cMatrix& vRows, cMatrix& vCols)
{
	if (!vRows.IsScalar() || !vCols.IsScalar()) { addError("illegal argument"); return true; }

	int irows = (int) vRows.Get0R();
	int icols = (int) vCols.Get0R();

	return Rand(Result, irows, icols);
}

bool Rand(cMatrix& Result, cMatrix& vDim)
{
	if (!vDim.IsScalar()) { addError("illegal argument"); return true; }

	int idim = (int) vDim.Get0R();

	return Rand(Result, idim, idim);
}

bool Randn(cMatrix& Result, cMatrix& vRows, cMatrix& vCols)
{
	if (!vRows.IsScalar() || !vCols.IsScalar()) { addError("illegal argument"); return true; }

	int irows = (int) vRows.Get0R();
	int icols = (int) vCols.Get0R();

	return Randn(Result, irows, icols);
}

bool Randn(cMatrix& Result, cMatrix& vDim)
{
	if (!vDim.IsScalar()) { addError("illegal argument"); return true; }

	int idim = (int) vDim.Get0R();

	return Randn(Result, idim, idim);
}

//
// basic statistics
//
bool MedianR(double& Result, cMatrix& R)
{
	int n = R.GetSize();

	cMatrix sortedR;
	if (R.Sort(sortedR)) return true;
	//const double *prr = sortedR.GetR();

	// find the median value of SORTED VECTOR sortedR
	int ix, ix1;
	if (IsEven(n)) {	// nr is even
		ix = (int) n/2 - 1;
		ix1 = ix + 1;
		Result = (sortedR.mr[ix] + sortedR.mr[ix1]) / 2.;
	}
	else {				// nr is odd
		ix = (int) (n-1) / 2;
		Result = sortedR.mr[ix];
	}

	return false;
}

bool MedianC(complex<double>& Result, cMatrix& C)
{
	int n = C.GetSize();

	cMatrix sortedC;
	if (C.Sort(sortedC)) return true;
	//const double *prr = sortedC.GetR();
	//const double *pri = sortedC.GetI();

	// find the median value of SORTED VECTOR sortedC
	int ix, ix1;
	double dr, di;
	if (IsEven(n)) {	// nr is even
		ix = (int) n/2 - 1;
		ix1 = ix + 1;
		dr = (sortedC.mr[ix] + sortedC.mr[ix1]) / 2.;
		di = (sortedC.mi[ix] + sortedC.mi[ix1]) / 2.;
		Result = complex<double>( dr, di );
	}
	else {				// nr is odd
		ix = (int) (n-1) / 2;
		Result = complex<double>( sortedC.mr[ix], sortedC.mi[ix] );
	}

	return false;
}

bool MedianR(cMatrix& Result, cMatrix& R)
{	// median of real matrix down columns
	int nc = R.GetColumns();
	int nr = R.GetRows();
	double lf;

	if (R.IsVector()) {
		if (MedianR(lf,R)) return true;
		if (Result.Create(1,1)) return true;
		Result.SetR(0,0,lf);
		return false;
	}
	
	if (Result.Create(1,nc)) return true;

	cMatrix sortedR;
	if (R.Sort(sortedR)) return true;

	//double *pr = Result.GetR();
	//const double *prr = sortedR.GetR();

	// find the median value
	int ix, ix1;
	int i;
	if (IsEven(nr)) {	// nr is even
		ix = (int) nr/2 - 1;
		ix1 = ix + 1;
		for (i=0; i<nc; ++i) { Result.mr[i] = (sortedR.mr[ix+nr*i] + sortedR.mr[ix1+nr*i]) / 2.; }
	}
	else {				// nr is odd
		ix = (int) (nr-1) / 2;
		for (i=0; i<nc; ++i) { Result.mr[i] = sortedR.mr[ix+nr*i]; }
	}

	return false;
}

bool MedianC(cMatrix& Result, cMatrix& C)
{
	int nc = C.GetColumns();
	int nr = C.GetRows();
	complex<double> cv;

	if (C.IsVector()) {
		if (MedianC(cv,C)) return true;
		if (Result.Create(1,1)) return true;
		Result.Set(0,0,cv);
		return false;
	}

	if (Result.CreateComplex(1,nc)) return true;

	cMatrix sortedC;
	if (C.Sort(sortedC)) return true;

	//double *pr = Result.GetR();
	//double *pi = Result.GetI();
	//const double *pcr = sortedC.GetR();
	//const double *pci = sortedC.GetI();

	// find the median value
	int ix, ix1;
	int i;
	if (IsEven(nr)) {	// nr is even
		ix = (int) nr/2 - 1;
		ix1 = ix + 1;
		for (i=0; i<nc; ++i) {
			Result.mr[i] = (sortedC.mr[ix+nr*i] + sortedC.mr[ix1+nr*i]) / 2.;
			Result.mi[i] = (sortedC.mi[ix+nr*i] + sortedC.mi[ix1+nr*i]) / 2.;
		}
	}
	else {				// nr is odd
		ix = (int) (nr-1) / 2;
		for (i=0; i<nc; ++i) {
			Result.mr[i] = sortedC.mr[ix+nr*i];
			Result.mi[i] = sortedC.mi[ix+nr*i];
		}
	}

	return false;
}

bool Median(cMatrix& Result, cMatrix& A)
{
	if (A.IsReal()) return MedianR( Result, A );
	else return MedianC( Result, A );
}

bool MedianR(cMatrix& Result, cMatrix& R, int idim)
{
	cMatrix RT;

	if (idim==1) return MedianR( Result, R );
	else if (idim==2) {
		if (Trans(RT, R)) return true;	// transpose
		return MedianR( Result, RT );
	}

	addError("invalid dimension");
	return true;
}

bool MedianC(cMatrix& Result, cMatrix& C, int idim)
{
	cMatrix CT;

	if (idim==1) return MedianC( Result, C );
	else if (idim==2) {
		if (Trans(CT, C)) return true;	// transpose
		return MedianC( Result, CT );
	}

	addError("invalid dimension");
	return true;
}

bool Median(cMatrix& Result, cMatrix& A, cMatrix& dim)
{
	if (dim.IsEmpty()) { addError("invalid argument"); return true; }
	if (!dim.IsScalar()) { addError("invalid argument"); return true; }
	int idim = (int) dim.Get0R();

	if (A.IsReal()) return MedianR( Result, A, idim );
	else return MedianC( Result, A, idim );
}


bool MeanR(cMatrix& Result, cMatrix& R)
{
	cMatrix Rsum;
	if (R.Sum(Rsum, 1)) return true;

	if (R.IsVector()) {
		return RDotDivR( Result, Rsum, double(R.GetSize()) );
	}
	else {
		return RDotDivR( Result, Rsum, double(R.GetRows()) );
	}
}

bool MeanC(cMatrix& Result, cMatrix& C)
{
	cMatrix Csum;

	if (C.Sum(Csum, 1)) return true;

	if (C.IsVector()) {
		return CDotDivC(Result, Csum, double(C.GetSize()) );
	}
	else {
		return CDotDivC(Result, Csum, double(C.GetRows()) );
	}
}

bool Mean(cMatrix& Result, cMatrix& A)
{
	if (A.IsReal()) return MeanR(Result, A);
	else return MeanC(Result, A);
}

bool MeanR(cMatrix& Result, cMatrix& R, int idim)
{
	cMatrix RT, Res_RT;

	if (idim==1) return MeanR(Result, R);
	else if (idim==2) {
		if (Trans(RT, R)) return true;
		if (MeanR(Res_RT, RT)) return true;
		return Trans(Result, Res_RT);
	}
	else { addError("invalid dimension"); return true; }
}

bool MeanC(cMatrix& Result, cMatrix& C, int idim)
{
	cMatrix CT, Res_CT;

	if (idim==1) return MeanR(Result, C);
	else if (idim==2) {
		if (Trans(CT, C)) return true;
		if (MeanC(Res_CT, CT)) return true;
		return Trans(Result, Res_CT);
	}
	else { addError("invalid dimension"); return true; }
}

bool Mean(cMatrix& Result, cMatrix& A, cMatrix& dim)
{
	int idim;
	if (dim.IsEmpty()) { addError("invalid argument"); return true; }
	idim = (int) dim.Get0R();

	if (A.IsReal()) return MeanR(Result, A, idim);
	else return MeanC(Result, A, idim);
}


//bool Var(cMatrix& Result, cMatrix& v, int iFlag, cMatrix& iDim);
bool VarR(cMatrix& Result, cMatrix& R)
{
	cMatrix Rtemp, Rsqmean, Rmeansq;
	int n;

	if (R.IsVector()) { n = R.GetSize(); }
	else n = R.GetRows();

	// mean square
	if ( R.SumSquare(Rtemp, 1) ) return true;
	if ( RDotDivR(Rsqmean, Rtemp, double(n) ) ) return true;
	// mean
	if ( MeanR(Rtemp, R, 1) ) return true;
	// squared mean
	if ( RDotPowR(Rmeansq, Rtemp, 2.) ) return true;
	// variance
	Sub(Result, Rsqmean, Rmeansq );

	return false;
}

bool VarC(cMatrix& Result, cMatrix& C)
{
	cMatrix Ctemp, Csqmean, Cmeansq;
	int n;

	if (C.IsVector()) { n = C.GetSize(); }
	else n = C.GetRows();

	// mean square
	if ( C.SumSquare(Ctemp, 1) ) return true;
	if ( CDotDivR(Csqmean, Ctemp, double(n) ) ) return true;
	// mean
	if ( MeanC(Ctemp, C, 1) ) return true;
	// squared mean
	if ( CDotPowR(Cmeansq, Ctemp, 2.) ) return true;
	// variance
	CSubC(Result, Csqmean, Cmeansq );

	return false;
}

bool Var(cMatrix& Result, cMatrix& A)
{
	if (A.IsReal()) return VarR(Result, A);
	else return VarC(Result, A);
}

bool VarR(cMatrix& Result, cMatrix& R, int idim)
{
	cMatrix RT, Res_RT;

	if (idim==1) return VarR(Result, R);
	else if (idim==2) {
		if (Trans(RT, R)) return true;
		if (VarR(Res_RT, RT)) return true;
		return Trans(Result, Res_RT);
	}
	else { addError("invalid dimension"); return true; }
}

bool VarC(cMatrix& Result, cMatrix& C, int idim)
{
	cMatrix CT, Res_CT;

	if (idim==1) return VarC(Result, C);
	else if (idim==2) {
		if (Trans(CT, C)) return true;
		if (VarC(Res_CT, CT)) return true;
		return Trans(Result, Res_CT);
	}
	else { addError("invalid dimension"); return true; }
}

bool Var(cMatrix& Result, cMatrix& A, cMatrix& dim)
{
	if (dim.IsEmpty()) { addError("invalid argument"); return true; }
	if (!dim.IsScalar()) { addError("invalid argument"); return true; }

	int idim = (int) dim.Get0R();

	if (A.IsReal()) return VarR(Result, A, idim);
	else return VarC(Result, A, idim);
}


bool StdR(cMatrix& Result, cMatrix& R)
{
	cMatrix Rvar;
	if (VarR(Rvar, R, 1)) return true;
	if (sqrtR(Result, Rvar)) return true;
	return false;
}

bool StdC(cMatrix& Result, cMatrix& C)
{
	cMatrix Cvar;
	if (VarC(Cvar, C, 1)) return true;
	if (sqrtC(Result, Cvar)) return true;
	return false;
}

bool Std(cMatrix& Result, cMatrix& A)
{
	if (A.IsReal()) return StdR(Result, A);
	else return StdC(Result, A);
}

bool StdR(cMatrix& Result, cMatrix& R, int idim)
{
	cMatrix RT, Res_RT;

	if (idim==1) return StdR(Result, R);
	else if (idim==2) {
		if (Trans(RT, R)) return true;
		if (StdR(Res_RT, RT)) return true;
		return Trans(Result, Res_RT);
	}
	else { addError("invalid dimension"); return true; }
}

bool StdC(cMatrix& Result, cMatrix& C, int idim)
{
	cMatrix CT, Res_CT;

	if (idim==1) return StdC(Result, C);
	else if (idim==2) {
		if (Trans(CT, C)) return true;
		if (StdC(Res_CT, CT)) return true;
		return Trans(Result, Res_CT);
	}
	else { addError("invalid dimension"); return true; }
}

bool Std(cMatrix& Result, cMatrix& A, cMatrix& dim)
{
	if (dim.IsEmpty()) { addError("invalid argument"); return true; }
	if (!dim.IsScalar()) { addError("invalid argument"); return true; }

	int idim = (int) dim.Get0R();

	if (A.IsReal()) return StdR(Result, A, idim);
	else return StdC(Result, A, idim);
}


bool Hist(cMatrix& Result, cMatrix& Data)
{
	return Hist(Result, Data, 10+1);
}

bool Hist(cMatrix& Result, cMatrix& Data, int nBins)
{
	cMatrix edges, centers;

	if (nBins <= 0) { addError("number of bins must be greater than 0"); return true; }
	if ( GetHistEdges(edges, centers, Data, nBins-1) ) return true;
	if ( Hist(Result, Data, edges) ) return true;

	return false;
}

// cov
//
bool RCovR(cMatrix& Result, cMatrix& R1, cMatrix& R2)
{	// assume R1 and R2 have same number of rows
	int nrows = R1.GetRows();
	double lf, avg1, avg2;

	if (Result.Create(R1.GetColumns(),R2.GetColumns())) return true;

	cMatrix R1trans, tmp;
	if (Trans(R1trans, R1)) return true;
	if (RMultR(tmp, R1trans, R2)) return true;

	cMatrix mean1, mean2;
	if (Mean(mean1, R1)) return true;
	if (Mean(mean2, R2)) return true;

	for (int i=0; i<R1.GetColumns(); ++i)
	{
		mean1.GetR(avg1, i);
		for (int j=0; j<R2.GetColumns(); ++j)
		{
			mean2.GetR(avg2, j);
			tmp.GetR(lf, i, j);
			Result.SetR(i, j, lf/double(nrows) - avg1*avg2);
		}
	}
	return false;
}

bool RCovC(cMatrix& Result, cMatrix& R, cMatrix& C)
{	// assume R and C have same number of rows
	int nrows = R.GetRows();
	double avg1;
	complex<double> lf, avg2, cv;

	if (Result.CreateComplex(R.GetColumns(),C.GetColumns())) return true;

	cMatrix Rtrans, tmp, tmpC;
	if (Trans(Rtrans, R)) return true;
	if (Conj(tmpC, C)) return true;
	if (RMultC(tmp, Rtrans, tmpC)) return true;

	cMatrix mean1, mean2;
	if (Mean(mean1, R)) return true;
	if (Mean(mean2, C)) return true;

	for (int i=0; i<R.GetColumns(); ++i)
	{
		mean1.GetR(avg1, i);
		for (int j=0; j<C.GetColumns(); ++j)
		{
			mean2.Get(avg2, j);
			tmp.Get(lf, i, j);
			cv = lf - double(nrows)*avg1*avg2;
			Result.Set(i, j, cv);
		}
	}
	return false;
}

bool CCovR(cMatrix& Result, cMatrix& C, cMatrix& R)
{	// assume R and C have same number of rows
	cMatrix tmp;
	if (RCovC(tmp, R, C)) return true;
	if (Herm(Result, tmp)) return true;
	return false;
}

bool CCovC(cMatrix& Result, cMatrix& C1, cMatrix& C2)
{	// assume R and C have same number of rows
	int nrows = C1.GetRows();
	complex<double> lf, avg1, avg2, cv;

	if (Result.CreateComplex(C1.GetColumns(),C2.GetColumns())) return true;

	cMatrix C1trans, tmp, tmpC2;
	if (Trans(C1trans, C1)) return true;
	if (Conj(tmpC2, C2)) return true;
	if (CMultC(tmp, C1trans, tmpC2)) return true;

	cMatrix mean1, mean2;
	if (Mean(mean1, C1)) return true;
	if (Mean(mean2, C2)) return true;

	for (int i=0; i<C1.GetColumns(); ++i)
	{
		mean1.Get(avg1, i);
		for (int j=0; j<C2.GetColumns(); ++j)
		{
			mean2.Get(avg2, j);
			tmp.Get(lf, i, j);
			cv = lf - double(nrows)*avg1*avg2;
			Result.Set(i, j, cv);
		}
	}
	return false;
}

bool Cov(cMatrix& Result, cMatrix& A1, cMatrix& A2)
{
	if (A1.IsVector()) { A1.nr = A1.GetSize(); A1.nc = 1; }	// force column
	if (A2.IsVector()) { A2.nr = A2.GetSize(); A2.nc = 1; }	// force column

	if (A1.GetRows() != A2.GetRows()) {
		addError("Number of rows must be equal");
		return true;
	}
	
	if (A1.IsReal() && A2.IsReal()) return RCovR(Result, A1, A2);
	else if (A1.IsReal()) return RCovC(Result, A1, A2);
	else if (A2.IsReal()) return CCovR(Result, A1, A2);
	else return CCovC(Result, A1, A2);
}

bool Cov(cMatrix& Result, cMatrix& A)
{
	cMatrix B = A;
	return Cov(Result, A, B);
}

// hist
//
bool Hist_2o(cMatrix& Result, cMatrix& centers, cMatrix& Data)
{
	cMatrix edges;

	if ( GetHistEdges(edges, centers, Data, 10-1) ) return true;
	if ( Hist(Result, Data, edges) ) return true;

	return false;
}

bool Hist_2o(cMatrix& Result, cMatrix& centers, cMatrix& Data, int nBins)
{
	cMatrix edges;

	if (nBins <= 0) { addError("number of bins must be greater than 0"); return true; }
	if ( GetHistEdges(edges, centers, Data, nBins-1) ) return true;
	if ( Hist(Result, Data, edges) ) return true;

	return false;
}

bool Histc(cMatrix& Result, cMatrix& Data, cMatrix& Centers)
{
	cMatrix edges;

	if ( GetHistEdges(edges, Centers) ) return true;
	if ( Hist(Result, Data, edges) ) return true;

	return false;
}

bool Histe(cMatrix& Result, cMatrix& v, cMatrix& edges)
{
	return Hist( Result, v, edges );
}

bool Histn(cMatrix& Result, cMatrix& v, cMatrix& Ncenters)
{
	if (!Ncenters.IsScalar()) { addError("illegal argument"); return true; }
	int nCenters = (int) Ncenters.Get0R();
	return Hist( Result, v, nCenters );
}

bool Histn_2o(cMatrix& Result, cMatrix& centers, cMatrix& v, cMatrix& Ncenters)
{
	if (!Ncenters.IsScalar()) { addError("illegal argument"); return true; }
	int nCenters = (int) Ncenters.Get0R();

	return Hist_2o(Result, centers, v, nCenters);
}

bool Hist(cMatrix& Result, cMatrix& Data, cMatrix& Edges)
{	// count number of Data entries inside bins defined by Edges vector.
	// infinite bins to left of Edges first entry, and bin to right of Edges last
	//  entry are counted as well. Thus Result generally is nEdges+1 -by- Data.GetColumns()
	//  matrix.

	int r, c, j;
	int iVectType;
	int nEdges = Edges.GetSize();

	if (Data.IsVector(&iVectType)) {
		if (Result.Create(nEdges+1, 1)) return true;
	}
	else {
		if (Result.Create(nEdges+1, Data.GetColumns())) return true;
	}
	// reset result to zeros
	//double *pr = Result.GetR();
	//memset( pr, 0, Result.GetSize()*sizeof(double) );
	Result.FillR(0, 0., Result.GetSize());

	int idx, nr(Data.GetRows()), nc(Data.GetColumns());
	double dval;
	const double *pdr = Data.GetR();	// ignore imag values (if any)
	const double *per = Edges.GetR();
	bool bFound;

	int icount(0);

	for (c=0; c<nc; c++) {
		for (r=0; r<nr; r++) {

			dval = pdr[r+nr*c];
			bFound = false;

			for (j=0; j<nEdges; j++) {
				if ( dval<=per[j] ) {
					switch (iVectType) {
					case 1:	 case 2:			// vector
						idx = j;
						break;
					default:
						idx = j + Result.GetRows()*c;
					};
					Result.mr[idx] += 1.;
					++icount;
					bFound = true;
					break;			// go to next data point
				}
			}

			// add to final bin, if no bin hit yet
			if (!bFound) {
				switch (iVectType) {
				case 1:	 case 2:			// vector
					idx = nEdges;
					break;
				default:
					idx = nEdges + Result.GetRows()*c;
				};
				Result.mr[idx] += 1;
				++icount;
			}

		}
	}

	return false;
}

bool GetHistEdges(cMatrix& Edges, cMatrix& Centers, cMatrix& Data, int N_edges)
{	// number of centers = number of edges + 1

	if (N_edges <= 0) { addError("number of bins must be greater than 0"); return true; }

	if (Edges.Create(N_edges,1)) { return true; }
	if (Centers.Create(N_edges+1,1)) { return true; }

	double dmax;
	double dmin;
	double dFirstCenter, dFirstEdge;

	if (Data.MaxNumberR( dmax )) return true;
	if (Data.MinNumberR( dmin )) return true;

	double edge_span = (dmax - dmin) / double(N_edges+1);
	int i;

	//double *per = Edges.GetR();
	//double *pcr = Centers.GetR();

	if (edge_span==0.) {
		for (i=0; i<N_edges; i++) {
			Edges.mr[i] = double(i)-double(N_edges) / 2.;
			Centers.mr[i] = Edges.mr[i];
		}
	}
	else {
		dFirstCenter = dmin + edge_span / 2.;
		dFirstEdge = dmin + edge_span;
		for (i=0; i<N_edges; i++) {
			Edges.mr[i] = dFirstEdge + double(i)*edge_span;
			Centers.mr[i] = dFirstCenter + double(i)*edge_span;
		}
		Centers.mr[N_edges] = dFirstCenter + double(N_edges)*edge_span;
	}

	return false;
}

bool GetHistCenters(cMatrix& Centers, cMatrix& Edges)
{	// determine centers (output) based on edges (input)

	int nEdges = Edges.GetSize();
	if (nEdges <= 1) { addError("number of bins must be greater than 0"); return true; }	// cannot derive centers unless we have more than one edge

	if (Centers.Create(nEdges+1,1)) { return true; }

	//const double *per = Edges.GetR();
	//double *pcr = Centers.GetR();

	double edge_span = Edges.mr[1] - Edges.mr[0];
	double dFirstCenter = Edges.mr[0] - edge_span / 2.;

	for (int i=0; i<=Edges.GetSize(); i++) {
		Centers.mr[i] = dFirstCenter + double(i) * edge_span;	// in the center, hence the name
	}

	return false;
}

bool GetHistEdges(cMatrix& Edges, cMatrix& Centers, double data_min, double data_max)
{
	int nCenters = Centers.GetSize();

	if (std::fpclassify(data_min)==FP_INFINITE || std::fpclassify(data_max)==FP_INFINITE) { addError("infinite values in matrix"); return true; }

	int N_edges = nCenters - 1;
	double edge_span = (data_max - data_min) / double(N_edges+1);

	if (N_edges==0) { Edges.Delete(); return false; }

	if (Edges.Create(N_edges,1)) return true;

	//double *per = Edges.GetR();
	double dFirstEdge = data_min + edge_span;

	for (int i=1; i<nCenters; i++) {
		Edges.mr[i-1] = dFirstEdge + double(i-1) * edge_span;
	}

	return false;
}

bool GetHistEdges(cMatrix& Edges, cMatrix& Centers)
{
	int nCenters = Centers.GetSize();
	if (nCenters <= 1) { addError("number of bins must be greater than 0"); return true; }	// cannot derive centers unless we have more than one edge

	if (Edges.Create(nCenters-1,1)) return true;

	//double *per = Edges.GetR();
	//const double *pcr = Centers.GetR();

	for (int i=1; i<nCenters; i++) {
		Edges.mr[i-1] = (Centers.mr[i] + Centers.mr[i-1]) / 2.;
	}

	return false;
}


//
// FFT
//

// Radix2FFT:
// Input matrix = x, output = y, flag==1 --> fft, flag==-1 --> ifft.
bool Radix2FFTR(cMatrix *y, cMatrix *x, int flag)
{
	int i, j, k, l;
	int L;
	complex<double> arg, jpi, cval1, cval2, cv;
	double temp_val;
	cMatrix xbr, tmpMat;
	cMatrix cxbr;

	if (!IsPow2(x->GetRows())) { addError("size must be power of 2"); return true; }
	L = Log2(x->GetRows());

	if (xbr.Create(x->GetRows(),1)) { return true; }
	if (cxbr.CreateComplex(x->GetRows(),1)) { return true; }

	// Initialize result matrix
	if ( y->CreateComplex(x->GetRows(),x->GetColumns()) ) { return true; }

	//double *pxbr = xbr.GetR();
	//double *pxr = x->GetR();

	int nr = x->GetRows();

	for (int c=0; c<x->GetColumns(); c++) {		// do FFT for all columns of *x;

		// Initialize xbr to the contents of x
		if ( x->GetR(tmpMat, 0,c,x->GetRows()-1,c) ) { addError("Radix2FFTR: internal error"); return true; }
		xbr.Set( 0, 0, tmpMat );

		// arrange x in bit reversed order (put int xbr)
		// Taken from FORTRAN code in Oppenheim & Schafer (pg. 608)
		long nv2 = nr/2;
		j = 1;
		for (i=1; i<nr; i++) {
			if (i>=j) { goto K_stmt; }
			temp_val = xbr.mr[j-1];
			xbr.mr[j-1] = xbr.mr[i-1];
			xbr.mr[i-1] = temp_val;
	K_stmt: k = nv2;
	IF:		if (k>=j) { goto J_stmt; }
			j = j-k;
			k = k/2;
			goto IF;
	J_stmt:	j = j+k;
		}

		cxbr.Set(0, 0, xbr);

		int IP;
		double LE, LE1;
		complex<double> W, U;
		
		// main radix-2 fft algorithm
		for (i=1; i<=L; i++) {				// loop over sets

			LE = pow(2.,i);
			LE1 = LE / 2.;
			W = complex<double>( cos(PI/LE1), -sin(PI/LE1) );
			U = complex<double>( 1., 0. );

			for (k=1; k<=LE1; k++) {	// loop within sets

				for (l=k; l<=nr; l+=int(LE)) {

					IP = l + int(LE1);

					cxbr.Get(cval1, IP-1, 0);
					cxbr.Get(cval2, l-1, 0);

					cval1 = cval1 * U;
					cv = cval2-cval1;
					cxbr.Set(IP-1, 0, cv);

					cval2 = cval2 + cval1;
					cxbr.Set(l-1, 0, cval2);
				}

				U = U * W;
			}

		}

		// set c^th column of *y to FFT output 
		y->Set(0,c,cxbr);

	}	// end loop over columns of *x

	// all is well
	return false;
}

bool Radix2FFTC(cMatrix *y, cMatrix *x, int flag)
{
	int i, j, k, l;
	int L;
	complex<double> temp_val, arg, jpi, cval1, cval2, cv;
	cMatrix xbr, cxbr;
	cMatrix tmpMat;

	if (!IsPow2(x->GetRows())) {	return 0; }
	L = Log2(x->GetRows());

	if (xbr.CreateComplex(x->GetRows(),1)) { return true; }
	if (cxbr.CreateComplex(x->GetRows(),1)) { return true; }

	// Initialize result matrix
	if (y->CreateComplex(x->GetRows(),x->GetColumns())) { return true; }

	//double *pxbr = xbr.GetR(), *pxbi = xbr.GetI();
	//double *pxr = x->GetR(), *pxi = x->GetI();

	int nr = x->GetRows();

	for (int c=0; c<x->GetColumns(); c++) {		// do FFT for all columns of *x;

		// Initialize xbr to the contents of x
		if ( x->GetR(tmpMat, 0,c,nr-1,c) ) { addError("Radix2FFTC: internal error"); return true; }
		xbr.Set( 0, 0, tmpMat );

		// arrange x in bit reversed order (put int xbr)
		// Taken from FORTRAN code in Oppenheim & Schafer (pg. 608)
		long nv2 = x->nr/2;
		j = 1;
		for (i=1; i<nr; i++) {
			if (i>=j) { goto K_stmt; }
			temp_val = complex<double>(xbr.mr[j-1], xbr.mi[j-1]);
			xbr.mr[j-1] = xbr.mr[i-1];
			xbr.mi[j-1] = xbr.mi[i-1];
			xbr.mr[i-1] = temp_val.real();
			xbr.mi[i-1] = temp_val.imag();
	K_stmt: k = nv2;
	IF:		if (k>=j) { goto J_stmt; }
			j = j-k;
			k = k/2;
			goto IF;
	J_stmt:	j = j+k;
		}

		cxbr.Set(0, 0, xbr);

		int IP;
		double LE, LE1;
		complex<double> W, U;
		
		// main radix-2 fft algorithm
		for (i=1; i<=L; i++) {				// loop over sets

			LE = pow(2.,i);
			LE1 = LE / 2.;
			W = complex<double>( cos(PI/LE1), -sin(PI/LE1) );
			U = complex<double>( 1., 0. );

			for (k=1; k<=LE1; k++) {	// loop within sets

				for (l=k; l<=nr; l+=int(LE)) {

					IP = l + int(LE1);

					cxbr.Get(cval1, IP-1, 0);
					cxbr.Get(cval2, l-1, 0);

					cval1 = cval1 * U;
					cv = cval2-cval1;
					cxbr.Set(IP-1, 0, cv);

					cval2 = cval2 + cval1;
					cxbr.Set(l-1, 0, cval2);
				}

				U = U * W;
			}

		}

		// set c^th column of *y to FFT output 
		y->Set(0,c,cxbr);

	}	// end loop over columns of *x

	// all is well
	return false;
}

bool Radix2FFT(cMatrix& y, cMatrix& x, int iflag)
{
	if (x.IsReal()) return Radix2FFTR(&y, &x, iflag);
	else return Radix2FFTC(&y, &x, iflag);
}

bool FFTZR(cMatrix& y, cMatrix& x, int iflag)
{
	int iw, inum, nrz;
	bool bSwapFlag = false;
	bool bResult;
	double dlog, dlogz;
	cMatrix z;

	int nr = x.GetRows();
	int nc = x.GetColumns();

	if (x.IsVector(&iw)) {
		switch (iw) {
		case 1:					// column vector
			break;
		case 2:					// row vector, make it a column vect (bookkeeping)
			inum = nr;
			nr = nc;
			nc = inum;
			x.nc = nc;
			x.nr = nr;
			bSwapFlag = true;
			break;
		case 3:
			//*y = *x;
			break;
		};
	}

	if (IsPow2(nr)) {
		bResult = Radix2FFTR(&y,&x,iflag);
	}
	else {
		dlog = log(double(nr))/log(2.);
		dlogz = double( (int)dlog + 1 );
		nrz = int( pow(2., (int)dlogz) );
		if (z.Create(nrz,nc)) return true;
		z.Reset();
		z.Set(0, 0, x);
		bResult = Radix2FFTR(&y,&z,iflag);
	}

	if (bSwapFlag) {
		// swap x back
		inum = x.nr;
		x.nr = x.nc;
		x.nc = inum;
		// now fix y
		inum = y.nr;
		y.nr = y.nc;
		y.nc = inum;
	}
	return bResult;
}

bool FFTZC(cMatrix& y, cMatrix& x, int iflag)
{
	int iw, inum, nrz;
	bool bSwapFlag = false;
	bool bResult;
	double dlog, dlogz;
	cMatrix z;

	int nr = x.GetRows();
	int nc = x.GetColumns();

	if (x.IsVector(&iw)) {
		switch (iw) {
		case 1:					// column vector
			break;
		case 2:					// row vector, make it a column vect (bookkeeping)
			inum = nr;
			nr = nc;
			nc = inum;
			x.nc = nc;
			x.nr = nr;
			bSwapFlag = true;
			break;
		case 3:
			//*y = *x;
			break;
		};
	}

	if (IsPow2(nr)) {
		bResult = Radix2FFTC(&y,&x,iflag);
	}
	else {
		dlog = log(double(nr))/log(2.);
		dlogz = double( (int)dlog + 1 );
		nrz = int( pow(2., (int)dlogz) );
		if (z.CreateComplex(nrz,nc)) return true;
		z.Reset();
		z.Set(0, 0, x);
		bResult = Radix2FFTC(&y,&z,iflag);
	}

	if (bSwapFlag) {
		// swap x back
		inum = x.nr;
		x.nr = x.nc;
		x.nc = inum;
		// now fix y
		inum = y.nr;
		y.nr = y.nc;
		y.nc = inum;
	}
	return bResult;
}

bool FFTZ(cMatrix& y, cMatrix& x, int iflag)
{
	if (x.IsReal()) return FFTZR(y, x, iflag);
	else return FFTZC(y, x, iflag);
}

bool StraightDFTR(cMatrix *y, cMatrix *x, int flag)
{
	complex<double> carg, tmp_val, tmp_exp;
	double dv, dfrac;

	carg = complex<double>(0.,0.);
	tmp_val = tmp_val = complex<double>(0.,0.);

	int nr = x->GetRows();
	int nc = x->GetColumns();

	if (y->CreateComplex(nr,nc)) return true;

	for (int c=0; c<nc; c++) {		// perform DFT on all columns

		for (int k=0; k<nr; k++) {		// main DFT loop

			dfrac = (double)(-flag)*2*PI*(double)k/((double)nr);

			for (int l=0; l<nr; l++) {

				dv = (double)l*dfrac;
				tmp_exp = complex<double>( cos(dv), sin(dv) );
				x->GetR(dv,l,c);
				tmp_val = tmp_val + dv * tmp_exp;

			}
			y->Set(k,c,tmp_val);
			tmp_val = complex<double>(0.,0.);	// zero out tmp value
		}

	}
	return false;
}

bool StraightDFTC(cMatrix *y, cMatrix *x, int flag)
{
	complex<double> carg, tmp_val, tmp_exp;
	complex<double> cv;
	double dv, dfrac;

	carg = complex<double>(0.,0.);
	tmp_val = tmp_val = complex<double>(0.,0.);

	int nr = x->GetRows();
	int nc = x->GetColumns();

	if (y->CreateComplex(nr,nc)) return true;

	for (int c=0; c<nc; c++) {		// perform DFT on all columns

		for (int k=0; k<nr; k++) {		// main DFT loop

			dfrac = (double)(-flag)*2*PI*(double)k/((double)nr);

			for (int l=0; l<nr; l++) {

				dv = (double)l*dfrac;
				tmp_exp = complex<double>( cos(dv), sin(dv) );
				x->GetC(cv,l,c);
				tmp_val = tmp_val + cv * tmp_exp;

			}
			y->Set(k,c,tmp_val);
			tmp_val = complex<double>(0.,0.);	// zero out tmp value
		}

	}
	return false;
}

bool StraightDFT(cMatrix& y, cMatrix& x, int iflag)
{
	int iw, inum;
	bool bSwapFlag = false;
	bool bResult;

	int nr = x.GetRows();
	int nc = x.GetColumns();

	if (x.IsVector(&iw)) {
		switch (iw) {
		case 1:					// column vector
			break;
		case 2:					// row vector, make it a column vect (bookkeeping)
			inum = nr;
			nr = nc;
			nc = inum;
			x.nc = nc;
			x.nr = nr;
			bSwapFlag = true;
			break;
		case 3:
			//*y = *x;
			break;
		};
	}

	if (x.IsReal()) bResult = StraightDFTR(&y, &x, iflag);
	else bResult = StraightDFTC(&y, &x, iflag);

	if (bSwapFlag) {	// for vectors
		// swap x back
		inum = x.nr;
		x.nr = x.nc;
		x.nc = inum;
		// now fix y
		y.nr = x.nr;
		y.nc = x.nc;
	}
	return bResult;
}

bool StraightDFT2R(cMatrix *y, cMatrix *x, int flag)
{
	complex<double> carg, tmp_val, tmp_exp;
	double dv;

	carg = complex<double>(0.,0.);
	tmp_val = tmp_val = complex<double>(0.,0.);

	int nr = x->GetRows();
	int nc = x->GetColumns();

	if (y->CreateComplex(nr,nc)) return true;

	cMatrix Mat1;							// intermediate calculation
	if (Mat1.CreateComplex(nr,nc)) return true;

	int c;

	for (c=0; c<nc; c++) {		// perform DFT on all columns

		for (int k=0; k<nr; k++) {		// main DFT loop
			for (int l=0; l<nr; l++) {

				carg = complex<double>( 0., (double)(-flag)*2.*PI*(double)l*(double)k/((double)nr) );
				tmp_exp = ExpC( carg );
				x->GetR(dv,l,c);
				tmp_val = tmp_val + dv * tmp_exp;

			}
			Mat1.Set(k,c,tmp_val);
			tmp_val = complex<double>(0.,0.);	// zero out tmp value
		}

	}

	for (int r=0; r<nr; r++) {		// perform DFT on all rows

		for (int k=0; k<nc; k++) {		// main DFT loop
			for (int l=0; l<nc; l++) {

				carg = complex<double>(0., (double)(-flag)*2.*PI*(double)l*(double)k/((double)nc) );
				tmp_exp = ExpC( carg );
				x->GetR(dv,l,c);
				tmp_val = tmp_val + dv * tmp_exp;

			}
			y->Set(k,c,tmp_val);
			tmp_val = complex<double>(0.,0.);	// zero out tmp value
		}

	}

	return false;
}

bool StraightDFT2C(cMatrix *y, cMatrix *x, int flag)
{
	complex<double> carg, tmp_val, tmp_exp;
	complex<double> cv;

	carg = complex<double>(0.,0.);
	tmp_val = tmp_val = complex<double>(0.,0.);

	int nr = x->GetRows();
	int nc = x->GetColumns();

	if (y->CreateComplex(nr,nc)) return true;

	cMatrix Mat1;							// intermediate calculation
	if (Mat1.CreateComplex(nr,nc)) return true;

	int c;

	for (c=0; c<nc; c++) {		// perform DFT on all columns

		for (int k=0; k<nr; k++) {		// main DFT loop
			for (int l=0; l<nr; l++) {

				carg = complex<double>( 0., (double)(-flag)*2.*PI*(double)l*(double)k/((double)nr) );
				tmp_exp = ExpC( carg );
				x->GetC(cv,l,c);
				tmp_val = tmp_val + cv * tmp_exp;

			}
			Mat1.Set(k,c,tmp_val);
			tmp_val = complex<double>(0.,0.);	// zero out tmp value
		}

	}

	for (int r=0; r<nr; r++) {		// perform DFT on all rows

		for (int k=0; k<nc; k++) {		// main DFT loop
			for (int l=0; l<nc; l++) {

				carg = complex<double>(0., (double)(-flag)*2.*PI*(double)l*(double)k/((double)nc) );
				tmp_exp = ExpC( carg );
				x->GetC(cv,l,c);
				tmp_val = tmp_val + cv * tmp_exp;

			}
			y->Set(k,c,tmp_val);
			tmp_val = complex<double>(0.,0.);	// zero out tmp value
		}

	}

	return false;
}

bool StraightDFT2(cMatrix& y, cMatrix& x, int iflag)
{
	if (x.IsReal()) return StraightDFT2R(&y, &x, iflag);
	else return StraightDFT2C(&y, &x, iflag);
}

// x is ptr to input matrix, y is output dft
bool DFTR(cMatrix *y, cMatrix *x, int iFlag)
{
	int iw, inum;
	bool bSwapFlag = false;
	bool bResult;

	int nr = x->GetRows();
	int nc = x->GetColumns();

	if (x->IsVector(&iw)) {
		switch (iw) {
		case 1:					// column vector
			break;
		case 2:					// row vector, make it a column vect (bookkeeping)
			inum = nr;
			nr = nc;
			nc = inum;
			x->nc = nc;
			x->nr = nr;
			bSwapFlag = true;
			break;
		case 3:
			//*y = *x;
			break;
		};
	}

	if (IsPow2(nr)) {
		bResult = Radix2FFTR(y,x,iFlag);
	}
	else bResult = StraightDFTR(y,x,iFlag);

	if (bSwapFlag) {
		// swap x back
		inum = x->nr;
		x->nr = x->nc;
		x->nc = inum;
		// now fix y
		y->nr = x->nr;
		y->nc = x->nc;
	}
	return bResult;
}

bool DFTC(cMatrix *y, cMatrix *x, int iFlag)
{
	int iw, inum;
	bool bSwapFlag = false;
	bool bResult;

	int nr = x->GetRows();
	int nc = x->GetColumns();

	if (x->IsVector(&iw)) {
		switch (iw) {
		case 1:					// column vector
			break;
		case 2:					// row vector, make it a column vect (bookkeeping)
			inum = nr;
			nr = nc;
			nc = inum;
			bSwapFlag = true;
			break;
		case 3:
			*y = *x;
			break;
		};
	}

	if (IsPow2(nr)) {
		bResult = Radix2FFTC(y,x,iFlag);
	}
	else bResult = StraightDFTC(y,x,iFlag);

	if (bSwapFlag) {
		inum = y->nr;
		y->nr = y->nc;
		y->nc = inum;
	}
	return bResult;
}

bool DFT(cMatrix& y, cMatrix& x, int iflag)
{
	if (x.IsReal()) return DFTR(&y, &x, iflag);
	else return DFTC(&y, &x, iflag);
}

bool DFT2R(cMatrix *y, cMatrix *x, int iFlag)
{
	cMatrix Mat1, Mat2;
	if (Mat1.Create(x->GetRows(),x->GetColumns())) return true;
	if (Mat2.Create(x->GetColumns(),x->GetRows())) return true;

	// DFT of columns of x
	DFTR(&Mat1, x, iFlag);

	Mat1.Trans();

	// row-wise DFT
	if ( DFTR(&Mat2, &Mat1, iFlag) ) return true;

	// copy result into y
	Mat2.Trans();
	*y = Mat2;

	return false;
}

bool DFT2C(cMatrix *y, cMatrix *x, int iFlag)
{
	cMatrix Mat1, Mat2;
	if (Mat1.CreateComplex(x->GetRows(),x->GetColumns())) return true;
	if (Mat2.CreateComplex(x->GetColumns(),x->GetRows())) return true;

	// DFT of columns of x
	DFTC(&Mat1, x, iFlag);

	Mat1.Trans();

	// row-wise DFT
	if ( DFTC(&Mat2, &Mat1, iFlag) ) return true;

	// copy result into y
	Mat2.Trans();
	*y = Mat2;

	return false;
}

bool DFT2(cMatrix& y, cMatrix& x, int iflag)
{
	if (x.IsReal()) return DFT2R(&y, &x, iflag);
	else return DFT2C(&y, &x, iflag);
}

// puts lower half of matrix x input upper of y, and
// upper half of x into lower of y.
bool FFTShiftR(cMatrix *y, cMatrix *x)
{
	// Initialize result matrix
	if ( y->Create(x->GetRows(),x->GetColumns()) ) return true;

	// handle the scalar case
	if (x->IsScalar()) {
		*y = *x;
		return false;
	}

	int iw, r_offset = 0, c_offset = 0;
	int ro2 = int( (x->GetRows())/2 );
	int co2 = int( (x->GetColumns())/2 );

	if (!IsEven(x->GetRows())) r_offset = 1;
	if (!IsEven(x->GetColumns())) c_offset = 1;

	cMatrix tcv1, tcv2;
	cMatrix trv1, trv2;

	//if (tcv1.Create(ro2,1))				return true;
	//if (tcv2.Create(ro2+r_offset,1))	return true;
	//if (trv1.Create(1,co2))				return true;
	//if (trv2.Create(1,co2+c_offset))	return true;

	if (x->IsVector(&iw)) {
		switch (iw) {
		case 1:					// column vector
			x->GetR(tcv1, ro2+r_offset, 0, x->GetRows()-1, 0);
			y->Set(0, 0, tcv1);
			x->GetR(tcv2, 0, 0, ro2-1, 0);
			y->Set(ro2, 0, tcv2);
			break;
		case 2:
			x->GetR(trv1, 0, co2+c_offset, 0, x->GetColumns()-1);
			y->Set(0, 0, trv1);
			x->GetR(trv2, 0, 0, 0, co2-1);
			y->Set(0, co2, trv2);
			break;
		case 3:
			*y = *x;
			break;
		};
		trv1.Delete();
		trv2.Delete();
		return false;
	}

	tcv1.Delete();
	tcv2.Delete();
	trv1.Delete();
	trv2.Delete();

	cMatrix tx1, tx2;
	cMatrix tx3, tx4;
	if (tx1.Create(ro2, co2+c_offset)) 			return true;
	if (tx2.Create(ro2, co2)) 					return true;
	if (tx3.Create(ro2+r_offset, co2)) 			return true;
	if (tx4.Create(ro2+r_offset, co2+c_offset)) return true;

	x->GetR(tx2, ro2+r_offset, co2+c_offset, x->GetRows()-1, x->GetColumns()-1);	// 4th quadrant --> 2nd
	y->Set(0, 0, tx2);
	x->GetR(tx4, 0, 0, ro2-1+r_offset, co2-1+c_offset);								// 2nd quadrant --> 4th
	y->Set(ro2, co2, tx4);
	x->GetR(tx3, 0, co2+c_offset, ro2-1+r_offset, x->GetColumns()-1);				// 1st quadrant --> 3rd
	y->Set(ro2, 0, tx3);
	x->GetR(tx1, ro2+r_offset, 0, x->GetRows()-1, co2-1+c_offset);					// 3rd quadrant --> 1st
	y->Set(0, co2, tx1);

	return false;
}

bool FFTShiftC(cMatrix *y, cMatrix *x)
{
	// Initialize result matrix
	if ( y->CreateComplex(x->GetRows(),x->GetColumns()) ) return true;

	// handle the scalar case
	if (x->IsScalar()) {
		*y = *x;
		return false;
	}

	int iw, r_offset = 0, c_offset = 0;
	int ro2 = int( (x->GetRows())/2 );
	int co2 = int( (x->GetColumns())/2 );

	if (!IsEven(x->GetRows())) r_offset = 1;
	if (!IsEven(x->GetColumns())) c_offset = 1;

	cMatrix tcv1, tcv2;
	cMatrix trv1, trv2;

	if (x->IsVector(&iw)) {
		switch (iw) {
		case 1:					// column vector
			x->GetC(tcv1, ro2+r_offset, 0, x->GetRows()-1, 0);
			y->Set(0, 0, tcv1);
			x->GetC(tcv2, 0, 0, ro2-1, 0);
			y->Set(ro2, 0, tcv2);
			break;
		case 2:
			x->GetC(trv1, 0, co2+c_offset, 0, x->GetColumns()-1);
			y->Set(0, 0, trv1);
			x->GetC(trv2, 0, 0, 0, co2-1);
			y->Set(0, co2, trv2);
			break;
		case 3:
			*y = *x;
			break;
		};
		return false;
	}

	//if (tcv1.CreateComplex(ro2,1))				return true;
	//if (tcv2.CreateComplex(ro2+r_offset,1))		return true;
	//if (trv1.CreateComplex(1,co2))				return true;
	//if (trv2.CreateComplex(1,co2+c_offset))		return true;

	//tcv1.Delete();
	//tcv2.Delete();
	//trv1.Delete();
	//trv2.Delete();

	cMatrix tx1, tx2;
	cMatrix tx3, tx4;
	if (tx1.CreateComplex(ro2, co2+c_offset)) 			return true;
	if (tx2.CreateComplex(ro2, co2)) 					return true;
	if (tx3.CreateComplex(ro2+r_offset, co2)) 			return true;
	if (tx4.CreateComplex(ro2+r_offset, co2+c_offset)) return true;

	x->GetC(tx2, ro2+r_offset, co2+c_offset, x->GetRows()-1, x->GetColumns()-1);	// 4th quadrant --> 2nd
	y->Set(0, 0, tx2);
	x->GetC(tx4, 0, 0, ro2-1+r_offset, co2-1+c_offset);								// 2nd quadrant --> 4th
	y->Set(ro2, co2, tx4);
	x->GetC(tx3, 0, co2+c_offset, ro2-1+r_offset, x->GetColumns()-1);				// 1st quadrant --> 3rd
	y->Set(ro2, 0, tx3);
	x->GetC(tx1, ro2+r_offset, 0, x->GetRows()-1, co2-1+c_offset);					// 3rd quadrant --> 1st
	y->Set(0, co2, tx1);

	return false;
}

bool FFTShift(cMatrix& y, cMatrix& x)
{
	if (x.IsReal()) return FFTShiftR(&y, &x);
	else return FFTShiftC(&y, &x);
}

// interface fcns

bool Fft(cMatrix& Result, cMatrix& v)
{
	return DFT( Result, v, 1 );
}

bool Ifft(cMatrix& Result, cMatrix& v)
{
	return DFT( Result, v, -1 );
}

bool Fftz(cMatrix& Result, cMatrix& v)
{
	return FFTZ( Result, v, 1 );
}

bool Ifftz(cMatrix& Result, cMatrix& v)
{
	return FFTZ( Result, v, -1 );
}

bool Dft(cMatrix& Result, cMatrix& v)
{
	return StraightDFT( Result, v, 1 );
}

bool Idft(cMatrix& Result, cMatrix& v)
{
	return StraightDFT( Result, v, -1 );
}

bool Fft2(cMatrix& Result, cMatrix& v)
{
	return DFT2( Result, v, 1 );
}

bool Ifft2(cMatrix& Result, cMatrix& v)
{
	return DFT2( Result, v, -1 );
}

bool Dft2(cMatrix& Result, cMatrix& v)
{
	return StraightDFT2( Result, v, 1 );
}

bool Idft2(cMatrix& Result, cMatrix& v)
{
	return StraightDFT2( Result, v, -1 );
}

bool Shfft(cMatrix& Result, cMatrix& v)
{
	return FFTShift( Result, v );
}
