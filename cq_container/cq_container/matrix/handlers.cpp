
#include "matrix.h"
#include "variable.h"

// essential helpers
//
bool is_empty(cMatrix& res, cMatrix& M)
{
	res.Create(1,1);
	res.mr[0] = (int) M.IsEmpty();
	return false;
}

bool is_real(cMatrix& res, cMatrix& M)
{
	res.Create(1,1);
	res.mr[0] = (int) M.IsReal();
	return false;
}

bool is_vector(cMatrix& res, cMatrix& M)
{
	res.Create(1,1);
	res.mr[0] = (int) M.IsVector();
	return false;
}

bool is_scalar(cMatrix& res, cMatrix& M)
{
	res.Create(1,1);
	res.mr[0] = (int) M.IsScalar();
	return false;
}

bool is_square(cMatrix& res, cMatrix& M)
{
	res.Create(1,1);
	res.mr[0] = (int) M.IsSquare();
	return false;
}

bool is_finite(cMatrix& res, cMatrix& M)
{
	return M.IsFinite(res);
}

bool is_nan(cMatrix& res, cMatrix& M)
{
	return M.IsNan(res);
}

// basic operations
//
bool Add(cMatrix& res, cMatrix& M1, cMatrix& M2)
{
	if (M1.IsReal()) {
		if (M2.IsReal()) return RAddR(res,M1,M2);
		else return RAddC(res,M1,M2);
	}
	else {
		if (M2.IsReal()) return CAddR(res,M1,M2);
		else return CAddC(res,M1,M2);
	}
}

bool Add(cMatrix& res, cMatrix& M1, double d)
{
	if (M1.IsReal()) {
		return RAddR(res,d,M1);
	}
	else {
		return RAddC(res,d,M1);
	}
}

bool Add(cMatrix& res, double d, cMatrix& M2)
{
	return Add(res,M2,d);
}

bool Add(cMatrix& res, cMatrix& M1, complex<double>& cv)
{
	if (M1.IsReal()) {
		return CAddR(res,cv,M1);
	}
	else {
		return CAddC(res,cv,M1);
	}
}

bool Sub(cMatrix& res, cMatrix& M1, cMatrix& M2)
{
	if (M1.IsReal()) {
		if (M2.IsReal()) return RSubR(res,M1,M2);
		else return RSubC(res,M1,M2);
	}
	else {
		if (M2.IsReal()) return CSubR(res,M1,M2);
		else return CSubC(res,M1,M2);
	}
}

bool Sub(cMatrix& res, cMatrix& M1, double d)
{
	if (M1.IsReal()) {
		return RSubR(res,M1,d);
	}
	else {
		return CSubR(res,M1,d);
	}
}

bool Sub(cMatrix& res, double d, cMatrix& M2)
{
	if (M2.IsReal()) {
		return RSubR(res,d,M2);
	}
	else {
		return RSubC(res,d,M2);
	}
}

bool Sub(cMatrix& res, cMatrix& M1, complex<double>& cv)
{
	if (M1.IsReal()) {
		return RSubC(res,M1,cv);
	}
	else {
		return CSubC(res,M1,cv);
	}
}

bool Sub(cMatrix& res, complex<double>& cv, cMatrix& M2)
{
	if (M2.IsReal()) {
		return CSubR(res,cv,M2);
	}
	else {
		return CSubC(res,cv,M2);
	}
}

bool Mult(cMatrix& res, cMatrix& M1, cMatrix& M2)
{
	if (M1.IsScalar() || M2.IsScalar()) return DotMult(res, M1, M2);

	if (M1.GetColumns() != M2.GetRows()) {
		addError("dimension mismatch");
		return true;
	}

	if (M1.IsReal()) {
		if (M2.IsReal()) return RMultR(res,M1,M2);
		else return RMultC(res,M1,M2);
	}
	else {
		if (M2.IsReal()) return CMultR(res,M1,M2);
		else return CMultC(res,M1,M2);
	}
}

bool Mult(cMatrix& res, cMatrix& M, double d)
{
	if (M.IsReal()) {
		return RMultR(res,d,M);
	}
	else {
		return RMultC(res,d,M);
	}
}

// trig fcns
//
bool sin(cMatrix& res, cMatrix& m)
{
	if (m.IsReal()) return sinR(res, m);
	else return sinC(res, m);
}
bool cos(cMatrix& res, cMatrix& m)
{
	if (m.IsReal()) return cosR(res, m);
	else return cosC(res,m);
}
bool tan(cMatrix& res, cMatrix& m)
{
	if (m.IsReal()) return tanR(res, m);
	else return tanC(res, m);
}
bool asin(cMatrix& res, cMatrix& m)
{
	if (m.IsReal()) return asinR(res, m);
	else return asinC(res, m);
}
bool acos(cMatrix& res, cMatrix& m)
{
	if (m.IsReal()) return acosR(res, m);
	else return acosC(res, m);
}
bool atan(cMatrix& res, cMatrix& m)
{
	if (m.IsReal()) return atanR(res, m);
	else return atanC(res, m);
}
bool csc(cMatrix& res, cMatrix& m)
{
	if (m.IsReal()) return cscR(res, m);
	else return cscC(res, m);
}
bool sec(cMatrix& res, cMatrix& m)
{
	if (m.IsReal()) return secR(res, m);
	else return secC(res, m);
}
bool cot(cMatrix& res, cMatrix& m)
{
	if (m.IsReal()) return cotR(res, m);
	else return cotC(res, m);
}
bool acsc(cMatrix& res, cMatrix& m)
{
	if (m.IsReal()) return acscR(res, m);
	else return acscC(res, m);
}
bool asec(cMatrix& res, cMatrix& m)
{
	if (m.IsReal()) return asecR(res, m);
	else return asecC(res, m);
}
bool acot(cMatrix& res, cMatrix& m)
{
	if (m.IsReal()) return acotR(res, m);
	else return acotC(res, m);
}


// hyperbolic trig
//
bool sinh(cMatrix& res, cMatrix& m)
{
	if (m.IsReal()) return sinhR(res, m);
	else return sinhC(res, m);
}
bool cosh(cMatrix& res, cMatrix& m)
{
	if (m.IsReal()) return coshR(res, m);
	else return coshC(res, m);
}
bool tanh(cMatrix& res, cMatrix& m)
{
	if (m.IsReal()) return tanhR(res, m);
	else return tanhC(res, m);
}
bool asinh(cMatrix& res, cMatrix& m)
{
	if (m.IsReal()) return asinhR(res, m);
	else return asinhC(res, m);
}
bool acosh(cMatrix& res, cMatrix& m)
{
	if (m.IsReal()) return acoshR(res, m);
	else return acoshC(res, m);
}
bool atanh(cMatrix& res, cMatrix& m)
{
	if (m.IsReal()) return atanhR(res, m);
	else return atanhC(res, m);
}
bool csch(cMatrix& res, cMatrix& m)
{
	if (m.IsReal()) return cschR(res, m);
	else return cschC(res, m);
}
bool sech(cMatrix& res, cMatrix& m)
{
	if (m.IsReal()) return sech(res, m);
	else return sechC(res, m);
}
bool coth(cMatrix& res, cMatrix& m)
{
	if (m.IsReal()) return cothR(res, m);
	else return cothC(res, m);
}
bool acsch(cMatrix& res, cMatrix& m)
{
	if (m.IsReal()) return acschR(res, m);
	else return acschC(res, m);
}
bool asech(cMatrix& res, cMatrix& m)
{
	if (m.IsReal()) return asechR(res, m);
	else return asechC(res, m);
}
bool acoth(cMatrix& res, cMatrix& m)
{
	if (m.IsReal()) return acothR(res, m);
	else return acothC(res, m);
}


// logarithms
//
bool log(cMatrix& res, cMatrix& m)
{
	if (m.IsReal()) return logR(res, m);
	else return logC(res, m);
}
bool log10(cMatrix& res, cMatrix& m)
{
	if (m.IsReal()) return logBaseR(res, m, 10.);
	else return logBaseC(res, m, 10.);
}
bool log2(cMatrix& res, cMatrix& m)
{
	if (m.IsReal()) return logBaseR(res, m, 2.);
	else return logBaseC(res, m, 2.);
}
bool sqrt(cMatrix& res, cMatrix& m)
{
	return Sqrt(res, m);
}
bool exp(cMatrix& res, cMatrix& m)
{
	return Exp(res, m);
}

// Special funtions
//
bool erf(cMatrix& res, cMatrix& m)
{
	return erfR(res, m);
}

bool erfc(cMatrix& res, cMatrix& m)
{
	return erfcR(res, m);
}

bool besselj0(cMatrix& res, cMatrix& m)
{
	return j0R(res, m);
}

bool besselj1(cMatrix& res, cMatrix& m)
{
	return j1R(res, m);
}

bool besseljn(cMatrix& res, cMatrix& order, cMatrix& m)
{
	if (order.IsEmpty()) { addError("illegal argument"); return true; }
	int iorder = (int) order.Get0R();
	if (iorder<0) { addError("order must be non-negative"); return true; }

	return jnR(res, iorder, m);
}

bool bessely0(cMatrix& res, cMatrix& m)
{
	return erfR(res, m);
}

bool bessely1(cMatrix& res, cMatrix& m)
{
	return erfR(res, m);
}

bool besselyn(cMatrix& res, cMatrix& order, cMatrix& m)
{
	if (order.IsEmpty()) { addError("illegal argument"); return true; }
	int iorder = (int) order.Get0R();
	if (iorder<0) { addError("order must be non-negative"); return true; }

	return ynR(res, iorder, m);
}


//
// Min/Max
//
bool Min(cMatrix& res, cMatrix& m)
{	// ignore imag part
	if (m.IsVector()) return MinR(res, m);
	else return MinVectR(res, m);
}

bool Min(cMatrix& res, cMatrix& m1, cMatrix& m2)
{	// ignore imag part
	return RMinR(res, m1, m2);
}

bool MinIndex(cMatrix& res, cMatrix& ResIdx, cMatrix& v)
{	// ignore imag part
	if (v.IsVector()) return MinIndexR(res, ResIdx, v);
	else return MinVectIndexR(res, ResIdx, v);
}

bool Max(cMatrix& res, cMatrix& v)
{	// ignore imag part
	if (v.IsVector()) return MaxR(res, v);
	else return MaxVectR(res, v);
}

bool Max(cMatrix& res, cMatrix& v1, cMatrix& v2)
{	// ignore imag part
	return RMaxR(res, v1, v2);
}

bool MaxIndex(cMatrix& res, cMatrix& ResIdx, cMatrix& v)
{	// ignore imag part
	if (v.IsVector()) return MaxIndexR(res, ResIdx, v);
	else return MaxVectIndexR(res, ResIdx, v);
}


//
// Sum, Prod
//
bool Sum(cMatrix& res, cMatrix& v)
{
	v.Sum(res, 1);	// down columns by default

	return false;
}

bool Sum(cMatrix& res, cMatrix& v, int idim)
{
	return v.Sum(res, idim);
}

bool Sum(cMatrix& res, cMatrix& v, cMatrix& vDim)
{
	int idim;
	if (!vDim.IsEmpty()) { idim = (int) vDim.Get0R(); }
	else return true;

	return Sum(res, v, idim);
}

bool CumSum(cMatrix& res, cMatrix& v)
{
	return v.CumSum(res, 2);		// down columns by default
}

bool CumSum(cMatrix& res, cMatrix& v, int idim)
{
	return v.CumSum(res, idim);
}

bool CumSum(cMatrix& res, cMatrix& v, cMatrix& vDim)
{
	int idim;
	if (!vDim.IsEmpty()) { idim = (int) vDim.Get0R(); }
	else return true;

	return CumSum(res, v, idim);
}

bool StraightSum(cMatrix& res, cMatrix& v)
{
	if (v.IsReal()) return res.SetTo( v.StraightSumR() );
	else return res.SetTo( v.StraightSumC() );
}


bool Prod(cMatrix& res, cMatrix& v)
{
	return v.Prod(res, 2);	// down columns by default
}

bool Prod(cMatrix& res, cMatrix& v, int idim)
{
	return v.Prod(res, idim);
}

bool Prod(cMatrix& res, cMatrix& v, cMatrix& vdim)
{
	int idim;
	if (!vdim.IsEmpty()) { idim = (int) vdim.Get0R(); }
	else return true;

	return CumProd(res, v, idim);
}

bool CumProd(cMatrix& res, cMatrix& v)
{
	v.CumProd(res, 2);	// down columns by default
	return false;
}


bool CumProd(cMatrix& res, cMatrix& v, int idim)
{
	return v.CumProd(res, idim);
}

bool CumProd(cMatrix& res, cMatrix& v, cMatrix& vDim)
{
	int idim;
	if (!vDim.IsEmpty()) { idim = (int) vDim.Get0R(); }
	else return true;

	return CumProd(res, v, idim);
}


/*
//
// rand/randn
//
bool Rand(cMatrix& res, int r, int c)
{
	// arg checking
	if (r<1 || c<1) { addError("dimensions must be positive"); return true; }

	if (r*c>1) return MatRandR(res, r, c);
	else return res.SetTo( DRand() );
}

bool Rand(cVariable& Result, int d)
{
	Result.Empty();

	// arg checking
	if (d<1) { addError("dimension must be positive"); return true; }

	if (d>1) return MatRandR(Result.m_M, d, d);
	else return Result.m_M.SetTo( DRand() );
}

bool Randn(cVariable& Result, int r, int c)
{
	Result.Empty();

	// arg checking
	if (r<1 || c<1) { addError("dimension must be positive"); return true; }

	int No2;
	bool bOdd = false;

	if (IsOdd(r*c)) {
		bOdd = true;
		No2 = int( r*c/2 ) + 1;
	}
	else No2 = int( (r*c)/2 );

	cMatrix U, V, Res1, Res2;
	cMatrix S, T, Tmp1, Tmp2, Tmp3;
	cMatrix arg;

	if (MatRandR(U,No2,1)) return true;
	if (MatRandR(V,No2,1)) return true;

	if (logR(Tmp1,V)) return true;
	if (Real(Tmp2,Tmp1)) return true;
	S = Tmp2 * -2.;
	if (S.IsEmpty()) { addError("grand: internal error"); return true; }

	V.Delete();		// free up memory

	if (RDotPowR(T,S,0.5)) return true;
	arg = U*(2.*PI);

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
	if (Result.m_M.Create(r,c)) return true;

	if (memcpy( Result.m_M.GetR(), Res1.GetR(), No2*sizeof(double) )==NULL) { addError("out of memory"); return true; }

	int N_offset = No2*sizeof(double);
	void *p = (void *)(&Result.m_M.mr[No2]);

	if (bOdd) { memcpy( p, Res2.GetR(), (No2-1)*sizeof(double) ); }
	else { memcpy( p, Res2.GetR(), No2*sizeof(double) ); }

	return false;
}

bool Randn(cVariable& Result, int d)
{ return Randn(Result, d, d); }

// interface fcns
bool Rand(cVariable& Result, cVariable& vRows, cVariable& vCols)
{
	Result.Empty();
	if (vRows.Type!=VAR_TYPE_NUMERIC || vCols.Type!=VAR_TYPE_NUMERIC) { addError("illegal argument"); return true; }
	if (!vRows.IsScalar() || !vCols.IsScalar()) { addError("illegal argument"); return true; }

	int irows = (int) vRows.m_M.Get0R();
	int icols = (int) vCols.m_M.Get0R();

	return Rand(Result, irows, icols);
}

bool Rand(cVariable& Result, cVariable& vDim)
{
	Result.Empty();
	if (vDim.Type!=VAR_TYPE_NUMERIC) { addError("illegal argument"); return true; }
	if (!vDim.IsScalar()) { addError("illegal argument"); return true; }

	int idim = (int) vDim.m_M.Get0R();

	return Rand(Result, idim, idim);
}

bool Randn(cVariable& Result, cVariable& vRows, cVariable& vCols)
{
	Result.Empty();
	if (vRows.Type!=VAR_TYPE_NUMERIC || vCols.Type!=VAR_TYPE_NUMERIC) { addError("illegal argument"); return true; }
	if (!vRows.IsScalar() || !vCols.IsScalar()) { addError("illegal argument"); return true; }

	int irows = (int) vRows.m_M.Get0R();
	int icols = (int) vCols.m_M.Get0R();

	return Randn(Result, irows, icols);
}

bool Randn(cVariable& Result, cVariable& vDim)
{
	Result.Empty();
	if (vDim.Type!=VAR_TYPE_NUMERIC) { addError("illegal argument"); return true; }
	if (!vDim.IsScalar()) { addError("illegal argument"); return true; }

	int idim = (int) vDim.m_M.Get0R();

	return Randn(Result, idim, idim);
}
*/
