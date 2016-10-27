
#include "cube.h"

inline bool slice_and_dice(bool (*func) (cMatrix&,cMatrix&), cCube& output, cCube& input)
{
	output.Delete();
	cMatrix slice, sret;
	for (int i=0; i<input.GetDepth(); ++i) {
		input.GetMatrix(slice,i);
		if (func(sret,slice)) return true;
		output.AddMatrix(sret);
	}
	return false;
}

inline bool slice_and_dice(bool (*func) (cMatrix&,cMatrix&,cMatrix&), cCube& output, cCube& in1, cCube& in2)
{
	output.Delete();
	cMatrix slice1, slice2, sret;
	for (int i=0; i<in1.GetDepth(); ++i) {
		in1.GetMatrix(slice1,i);
		in2.GetMatrix(slice2,i);
		if (func(sret,slice1,slice2)) return true;
		output.AddMatrix(sret);
	}
	return false;
}

inline bool slice_and_dice(bool (*func) (cMatrix&,cMatrix&,cMatrix&), cCube& output, cMatrix& in1, cMatrix& in2, cMatrix& in3)
{
	output.Delete();
	if (!in3.IsScalar()) { addError("expected scalar argument"); return true; }
	int nDepth = (int) in3.Get0R();
	cMatrix sret;
	for (int i=0; i<nDepth; ++i) {
		if (func(sret,in1,in2)) return true;
		output.AddMatrix(sret);
	}
	return false;
}

inline bool slice_and_dice(bool (*func) (cMatrix&,cMatrix&,double), cCube& output, cCube& in1, double lf2)
{
	output.Delete();
	cMatrix slice1, sret;
	for (int i=0; i<in1.GetDepth(); ++i) {
		in1.GetMatrix(slice1,i);
		if (func(sret,slice1,lf2)) return true;
		output.AddMatrix(sret);
	}
	return false;
}

inline bool slice_and_dice(bool (*func) (cMatrix&,double,cMatrix&), cCube& output, double lf1, cCube& in2)
{
	output.Delete();
	cMatrix slice2, sret;
	for (int i=0; i<in2.GetDepth(); ++i) {
		in2.GetMatrix(slice2,i);
		if (func(sret,lf1,slice2)) return true;
		output.AddMatrix(sret);
	}
	return false;
}

inline bool slice_and_dice(bool (*func) (cMatrix&,cMatrix&,int), cCube& output, cCube& in1, int i2)
{
	output.Delete();
	cMatrix slice1, sret;
	for (int i=0; i<in1.GetDepth(); ++i) {
		in1.GetMatrix(slice1,i);
		if (func(sret,slice1,i2)) return true;
		output.AddMatrix(sret);
	}
	return false;
}

inline bool slice_and_dice(bool (*func) (cMatrix&,cMatrix&,complex<double>&), cCube& output, cCube& in1, complex<double>& cv)
{
	output.Delete();
	cMatrix slice1, sret;
	for (int i=0; i<in1.GetDepth(); ++i) {
		in1.GetMatrix(slice1,i);
		if (func(sret,slice1,cv)) return true;
		output.AddMatrix(sret);
	}
	return false;
}

inline bool slice_and_dice(bool (*func) (cMatrix&,complex<double>&,cMatrix&), cCube& output, complex<double>& cv, cCube& in1)
{
	output.Delete();
	cMatrix slice1, sret;
	for (int i=0; i<in1.GetDepth(); ++i) {
		in1.GetMatrix(slice1,i);
		if (func(sret,cv,slice1)) return true;
		output.AddMatrix(sret);
	}
	return false;
}

//
// misc
//
bool Length(cMatrix& Result, cCube& v)
{
	int mx = v.nr > v.nc ? v.nr : v.nc;
	int my = mx > v.nm ? mx : v.nm;

	Result.Create(1,1);
	Result.mr[0] = double(my);

	return false;
}

bool Size(cMatrix& Result, cCube& v)
{
	Result.Create(1,3);
	double *pr = Result.GetR();

	pr[0] = (double) v.GetRows();
	pr[1] = (double) v.GetColumns();
	pr[2] = (double) v.GetDepth();

	return false;
}

bool Rows(cMatrix& Result, cCube& v)
{
	Result.Create(1,1);
	double *pr = Result.GetR();

	pr[0] = (double) v.GetRows();

	return false;
}

bool Columns(cMatrix& Result, cCube& v)
{
	Result.Create(1,1);
	double *pr = Result.GetR();

	pr[0] = (double) v.GetColumns();

	return false;
}

//
// File I/O
//
bool WriteData(cCube& v, FILE *pf)
{
	int nr = v.GetRows();
	int nc = v.GetColumns();
	int nm = v.GetDepth();
	bool bReal = v.IsReal();
	if (fwrite( &nr, sizeof(int), 1, pf ) != 1) return true;		// rows
	if (fwrite( &nc, sizeof(int), 1, pf ) != 1) return true;		// cols
	if (fwrite( &nm, sizeof(int), 1, pf ) != 1) return true;		// depth
	//
	if (fwrite( &bReal, sizeof(bool), 1, pf ) != 1) return true;	// is real?
	//
	if (fwrite( v.GetR(), sizeof(double), nr*nc, pf ) != nr*nc) return true;
	if (!bReal) {
		if (fwrite( v.GetI(), sizeof(double), nr*nc, pf ) != nr*nc) return true;
	}

	return false;
}

bool ReadData(cCube& m, FILE *pf)
{
	int nr, nc, nm;
	bool bReal;

	if (fread( &nr, sizeof(int), 1, pf ) != 1) return true;
	if (fread( &nc, sizeof(int), 1, pf ) != 1) return true;
	if (fread( &nm, sizeof(int), 1, pf ) != 1) return true;

	if (fread( &bReal, sizeof(bool), 1, pf ) != 1) return true;

	if (bReal) {
		if ( m.Create(nr,nc,nm) ) { addError("Out of memory!"); return true; }				// out of memory
		if (fread( m.GetR(), sizeof(double), nr*nc*nm, pf ) != nr*nc*nm) return true;
	}
	else {
		if ( m.CreateComplex(nr,nc,nm) ) { addError("Out of memory!"); return true; }		// out of memory
		if (fread( m.GetR(), sizeof(double), nr*nc*nm, pf ) != nr*nc*nm) return true;
		if (fread( m.GetI(), sizeof(double), nr*nc*nm, pf ) != nr*nc*nm) return true;
	}

	return false;
}


bool FillR(cCube& Cout, int r, int c, int d)
{
	Cout.Create(r,c,d);
	return false;
}

bool NewCube(cCube& Cout, cMatrix& vr,  cMatrix& vc, cMatrix& vd)
{
	if (!vr.IsScalar() || !vc.IsScalar() || !vr.IsScalar()) { addError("illegal argument"); return true; }

	// convert args to nr, nc integer values
	int nr = (int) vr.Get0R();
	int nc = (int) vc.Get0R();
	int nd = (int) vd.Get0R();

	// fill Result with value dv
	return FillR(Cout, nr, nc, nd);
}

bool SameSize(const cCube& R1, const cMatrix& R2)
{
	if (R1.IsVector()) {
		if (R2.IsVector()) return R1.GetSize()==R2.GetSize();
		else return false;
	}

	bool bSingletonDim = R1.GetRows()==1 || R1.GetColumns()==1 || R1.GetDepth()==1;
	int nSingleton(-1);

	if (bSingletonDim) {
		if (R1.GetRows()==1) nSingleton = 0;
		else if (R1.GetColumns()==1) nSingleton = 1;
		else if (R1.GetDepth()==1) nSingleton = 2;
	}

	switch (nSingleton) {
	case 0:
		return R1.GetColumns()==R2.GetRows() && R1.GetDepth()==R2.GetColumns();
	case 1:
		return R1.GetRows()==R2.GetRows() && R1.GetDepth()==R2.GetColumns();
	case 2:
		return R1.GetRows()==R2.GetRows() && R1.GetColumns()==R2.GetColumns();
	default:
		return false;
	};
}

bool SameSize(const cCube& R1, const cCube& R2)
{
	if (R1.IsVector()) {
		if (R2.IsVector()) return R1.GetSize()==R2.GetSize();
		else return false;
	}

	return (R1.GetRows()==R2.GetRows() && R1.GetColumns()==R2.GetColumns() && R1.GetDepth()==R2.GetDepth());
}

bool Real(cCube& Result, cCube& A)
{
	Result.Delete();
	Result.nr = A.nr;
	Result.nc = A.nc;
	Result.nm = A.nm;
	Result.mr = A.mr;
	return false;
}

bool Imag(cCube& Result, cCube& A)
{
	Result.Delete();
	Result.nr = A.nr;
	Result.nc = A.nc;
	Result.nm = A.nm;

	if (A.IsReal())
		Result.mi.insert( Result.mi.begin(), A.GetSize(), 0. );
	else
		Result.mi = A.mi;

	return false;
}

bool Conj(cCube& Result, cCube& A) { return slice_and_dice(Conj, Result, A); }

bool Add(cCube& res, cCube& M1, cCube& M2) { return slice_and_dice(Add, res, M1, M2); }
bool Add(cCube& res, cCube& M1, cMatrix& M2) {
	double d;
	complex<double> cv;

	if (M2.IsScalar()) {
		if (M2.IsReal()) {
			d = M2.Get0R();
			return slice_and_dice(Add, res, M1, d);
		}
		else {
			cv = M2.Get0C();
			return slice_and_dice(Add, res, M1, cv);
		}
	}
	else {
		addError("illegal operation");
		return true;
	}
}
bool Add(cCube& res, cCube& M1, double d) { return slice_and_dice(Add, res, M1, d); }
bool Add(cCube& res, cCube& M1, complex<double>& cv) { return slice_and_dice(Add, res, M1, cv); }
bool Add(cCube& res, complex<double>& cv, cCube& M1) { return slice_and_dice(Add, res, M1, cv); }
bool Sub(cCube& res, cCube& M1, cCube& M2) { return slice_and_dice(Sub, res, M1, M2); }
bool Sub(cCube& res, cCube& M1, double d) { return slice_and_dice(Sub, res, M1, d); }
bool Sub(cCube& res, double d, cCube& M2) { return slice_and_dice(Sub, res, d, M2); }
bool Sub(cCube& res, cCube& M1, complex<double>& cv) { return slice_and_dice(Sub, res, M1, cv); }
bool Sub(cCube& res, complex<double>& cv, cCube& M2) { return slice_and_dice(Sub, res, cv, M2); }
//
bool Mult(cCube& Result, cCube& A1, double d) { return slice_and_dice(Mult, Result, A1, d); }
bool DotMult(cCube& Result, cCube& A1, cCube& A2 ) { return slice_and_dice(DotMult, Result, A1, A2); }
//
bool DotDiv(cCube& Result, cCube& A1, cCube& A2 ) { return slice_and_dice(DotDiv, Result, A1, A2); }
bool DotDiv(cCube& Result, cCube& A1, double d ) { return slice_and_dice(DotDiv, Result, A1, d); }
bool DotDiv(cCube& Result, double d, cCube& A2 ) { return slice_and_dice(DotDiv, Result, d, A2); }
//
bool DotPow(cCube& Result, cCube& A1, cCube& A2) { return slice_and_dice(DotPow, Result, A1, A2); }
bool DotPow(cCube& Result, double d, cCube& A2) { return slice_and_dice(DotPow, Result, d, A2); }
bool DotPow(cCube& Result, cCube& A1, double d) { return slice_and_dice(DotPow, Result, A1, d); }
//
bool Fix(cCube& Result, cCube& A) { return slice_and_dice(Fix, Result, A); }
bool Floor(cCube& Result, cCube& A) { return slice_and_dice(Floor, Result, A); }
bool Round(cCube& Result, cCube& A) { return slice_and_dice(Round, Result, A); }
bool Ceil(cCube& Result, cCube& A) { return slice_and_dice(Ceil, Result, A); }
bool Phase(cCube& Result, cCube& A) { return slice_and_dice(Phase, Result, A); }
//
bool Not(cCube& Result, cCube& A) { return slice_and_dice(Not, Result, A); }
//
bool Mod(cCube& Result, cCube& x, cCube& y) { return slice_and_dice(Mod, Result, x, y); }
bool Mod(cCube& Result, double x, cCube& y) { return slice_and_dice(Mod, Result, x, y); }
bool Mod(cCube& Result, cCube& x, double y) { return slice_and_dice(Mod, Result, x, y); }
//
bool Factorial(cCube& Result, cCube& A) { return slice_and_dice(Factorial, Result, A); }
//
bool is_empty(cMatrix& res, cCube& M) { res.Create(1,1); res.mr[0] = (int) M.IsEmpty(); return false; }
bool is_real(cMatrix& res, cCube& M) { res.Create(1,1); res.mr[0] = (int) M.IsReal(); return false; }
bool is_vector(cMatrix& res, cCube& M) { res.Create(1,1); res.mr[0] = (int) M.IsVector(); return false; }
bool is_scalar(cMatrix& res, cCube& M) { res.Create(1,1); res.mr[0] = (int) M.IsScalar(); return false; }
bool is_finite(cCube& res, cCube& M) { return slice_and_dice(is_finite, res, M); }
bool is_nan(cCube& res, cCube& M) { return slice_and_dice(is_nan, res, M); }
//
bool sin(cCube& res, cCube& m) { return slice_and_dice(sin, res, m); }
bool cos(cCube& res, cCube& m) { return slice_and_dice(cos, res, m); }
bool tan(cCube& res, cCube& m) { return slice_and_dice(tan, res, m); }
bool asin(cCube& res, cCube& m) { return slice_and_dice(asin, res, m); }
bool acos(cCube& res, cCube& m) { return slice_and_dice(acos, res, m); }
bool atan(cCube& res, cCube& m) { return slice_and_dice(atan, res, m); }
bool csc(cCube& res, cCube& m) { return slice_and_dice(csc, res, m); }
bool sec(cCube& res, cCube& m) { return slice_and_dice(sec, res, m); }
bool cot(cCube& res, cCube& m) { return slice_and_dice(cot, res, m); }
bool acsc(cCube& res, cCube& m) { return slice_and_dice(acsc, res, m); }
bool asec(cCube& res, cCube& m) { return slice_and_dice(asec, res, m); }
bool acot(cCube& res, cCube& m) { return slice_and_dice(acot, res, m); }
bool Atan2(cCube& res, cCube& A1, cCube& A2) { return slice_and_dice(Atan2, res, A1, A2); }
//
bool sinh(cCube& res, cCube& m) { return slice_and_dice(sinh, res, m); }
bool cosh(cCube& res, cCube& m) { return slice_and_dice(cosh, res, m); }
bool tanh(cCube& res, cCube& m) { return slice_and_dice(tanh, res, m); }
bool asinh(cCube& res, cCube& m) { return slice_and_dice(asinh, res, m); }
bool acosh(cCube& res, cCube& m) { return slice_and_dice(acosh, res, m); }
bool atanh(cCube& res, cCube& m) { return slice_and_dice(atanh, res, m); }
bool csch(cCube& res, cCube& m) { return slice_and_dice(csch, res, m); }
bool sech(cCube& res, cCube& m) { return slice_and_dice(sech, res, m); }
bool coth(cCube& res, cCube& m) { return slice_and_dice(coth, res, m); }
bool acsch(cCube& res, cCube& m) { return slice_and_dice(acsch, res, m); }
bool asech(cCube& res, cCube& m) { return slice_and_dice(asech, res, m); }
bool acoth(cCube& res, cCube& m) { return slice_and_dice(acoth, res, m); }
//
bool log(cCube& res, cCube& m) { return slice_and_dice(log, res, m); }
bool log10(cCube& res, cCube& m) { return slice_and_dice(log10, res, m); }
bool log2(cCube& res, cCube& m) { return slice_and_dice(log2, res, m); }
bool sqrt(cCube& res, cCube& m) { return slice_and_dice(sqrt, res, m); }
bool exp(cCube& res, cCube& m) { return slice_and_dice(exp, res, m); }
//
bool erf(cCube& res, cCube& m) { return slice_and_dice(erf, res, m); }
bool erfc(cCube& res, cCube& m) { return slice_and_dice(erfc, res, m); }
bool besselj0(cCube& res, cCube& m) { return slice_and_dice(besselj0, res, m); }
bool besselj1(cCube& res, cCube& m) { return slice_and_dice(besselj1, res, m); }
bool besseljn(cCube& res, cCube& order, cCube& m) { return slice_and_dice(besseljn, res, order, m); }
bool bessely0(cCube& res, cCube& m) { return slice_and_dice(bessely0, res, m); }
bool bessely1(cCube& res, cCube& m) { return slice_and_dice(bessely1, res, m); }
bool besselyn(cCube& res, cCube& order, cCube& m) { return slice_and_dice(besselyn, res, order, m); }
//
bool lgamma(cCube& Res, cCube& R) { return slice_and_dice(lgamma, Res, R); }
bool gamma(cCube& Res, cCube& R) { return slice_and_dice(gamma, Res, R); }
//
bool Rand(cCube& Result, cMatrix& vRows, cMatrix& vCols, cMatrix& vDepth) { return slice_and_dice(Rand, Result, vRows, vCols, vDepth); }
bool Randn(cCube& Result, cMatrix& vRows, cMatrix& vCols, cMatrix& vDepth) { return slice_and_dice(Randn, Result, vRows, vCols, vDepth); }
