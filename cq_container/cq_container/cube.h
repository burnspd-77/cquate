
#pragma once

#include "matrix.h"

class cCube : public cMatrix
{
public:
	cCube() { m_nType = VAR_TYPE_CUBE; }
	~cCube() { }

	cCube(const cCube& c) {
		m_nType = VAR_TYPE_CUBE;
		nr = c.nr;
		nc = c.nc;
		nm = c.nm;
		//
		mr = c.mr;
		mi = c.mi;
	}

	bool Create(const int r, const int c, const int m);
	bool CreateComplex(const int r, const int c, const int m);
	bool CreateImaginary()
	{	// designed to be used for already-created real cubes
		int sz = GetSize();
		NewImagDouble(sz);
		return false;
	}
	bool Create(cMatrix& mat) {
		if (mat.IsReal()) return Create(mat.GetRows(), mat.GetColumns(), 1);
		else return CreateComplex(mat.GetRows(), mat.GetColumns(), 1);
	}

	bool Delete(void) {
		nr = nc = nm = 0;
		mr.clear(); mi.clear(); return false;
	}

	int GetType() { return m_nType; }
	int GetSize() const { return nr*nc*nm; }
	int GetDepth(void) const { return nm; }
	bool GetMatrix(cMatrix& m, const int k) const;
	void GetR(double& lf, const int ix) const { lf = mr[ix]; }
	void GetR(double&, const int, const int, const int) const;
	void GetI(double&, const int, const int, const int) const;
	void Get(complex<double>& c, const int i, const int j, const int k) const;
	bool GetR(vector<double>& vd, const int abeg, const int aend, const int bbeg, const int bend, const int c, const int dim);
	bool GetI(vector<double>& vd, const int abeg, const int aend, const int bbeg, const int bend, const int c, const int dim);
	bool Get(cMatrix& m, const int abeg, const int aend, const int bbeg, const int bend, const int c, const int dim);

	// direct access to matrix
	double *GetR(void) { return mr.data(); }
	double *GetI(void) { return mi.data(); }

	// set functions
	//
	bool SetTo(double d);				// set to real scalar
	bool SetTo(complex<double> c);		// set to complex scalar
	bool SetTo(cMatrix& m);				// copy from another matrix
	bool SetTo(cCube& c);

	void SetDepth(int d) { nm = d; }
	void SetR(const int ix, double lf) { mr[ix] = lf; }
	bool SetR(const int i, const int j, const int k, const double x);
	bool SetI(const int i, const int j, const int k, const double x);
	bool Set(const int i, const int j, const int k, complex<double>& c);
	bool Set(const int i, const int j, const int k, const cMatrix&);		// sets (i,j)-(n,m)th sub-matrix. Size determined by size of arg

	// file I/O
	bool SaveAscii(FILE *pf);

	bool IsEmpty(void) const { return GetSize()==0; }
	bool IsScalar(void) const;
	bool IsVector(int*) const;		// if true, int arg is 0 for col vector, 1 for row vector, 2 for depth vector
	bool IsVector(void) const;
	bool IsMatrix() { return nm==1; }
	bool IsFinite() const;

	bool AddMatrix(cMatrix& mat);

	// Logical operators
	//
	cCube operator==(const cCube& R) const;
	cCube operator==(const complex<double>& x) const;
	cCube operator==(const double d) const;
	friend cCube operator==(const double d, const cCube& R);
	friend cCube operator==(const complex<double>& x, const cCube& R);

	cCube operator!=(const cCube& R) const;
	cCube operator!=(const complex<double>& x) const;
	cCube operator!=(const double d) const;
	friend cCube operator!=(const double d, const cCube& R);
	friend cCube operator!=(const complex<double>& x, const cCube& R);

	cCube operator<(const cCube& R) const;
	cCube operator<(const complex<double>& x) const;
	cCube operator<(const double d) const;
	friend cCube operator<(const double d, const cCube& R);
	friend cCube operator<(const complex<double>& x, const cCube& R);

	cCube operator>(const cCube& R) const;
	cCube operator>(const complex<double>& x) const;
	cCube operator>(const double d) const;
	friend cCube operator>(const double d, const cCube& R);
	friend cCube operator>(const complex<double>& x, const cCube& R);

	cCube operator<=(const cCube& R) const;
	cCube operator<=(const complex<double>& x) const;
	cCube operator<=(const double d) const;
	friend cCube operator<=(const double d, const cCube& R);
	friend cCube operator<=(const complex<double>& x, const cCube& R);

	cCube operator>=(const cCube& R) const;
	cCube operator>=(const complex<double>& x) const;
	cCube operator>=(const double d) const;
	friend cCube operator>=(const double d, const cCube& R);
	friend cCube operator>=(const complex<double>& x, const cCube& R);

	cCube operator&&(const cCube& R) const;
	cCube operator&&(const complex<double>& x) const;
	cCube operator&&(const double d) const;
	friend cCube operator&&(const double d, const cCube& R);
	friend cCube operator&&(const complex<double>& x, const cCube& R);

	cCube operator||(const cCube& R) const;
	cCube operator||(const complex<double>& x) const;
	cCube operator||(const double d) const;
	friend cCube operator||(const double d, const cCube& R);
	friend cCube operator||(const complex<double>& x, const cCube& R);

protected:
	int getIx(const int i, const int j, const int k) const {
		return i + nr*j + nr*nc*k;
	}

public:
	int nm;
};

// File I/O
//
bool WriteData(cMatrix& v, FILE *pf);
bool ReadData(cMatrix& m, FILE *pf);


bool NewCube(cCube& Cout, cMatrix& vr,  cMatrix& vc, cMatrix& vd);
bool FillR(cCube& Cout, int r, int c, int d);
bool SameSize(const cCube& R1, const cMatrix& R2);
bool SameSize(const cCube& R1, const cCube& R2);

bool Length(cMatrix& Result, cCube& v);
bool Size(cMatrix& Result, cCube& v);
bool Rows(cMatrix& Result, cCube& v);
bool Columns(cMatrix& Result, cCube& v);

// access operator
//
bool cubeAccess(cMatrix& Result, double dvar, double darg1, double darg2, double darg3);
bool cubeAccess(cMatrix& Result, double dvar, double darg1, double darg2, cMatrix& arg3);
bool cubeAccess(cMatrix& Result, double dvar, double darg1, cMatrix& arg2, double darg3);
bool cubeAccess(cMatrix& Result, double dvar, double darg1, cMatrix& arg2, cMatrix& arg3);
bool cubeAccess(cMatrix& Result, double dvar, cMatrix& arg1, double darg2, double darg3);
bool cubeAccess(cMatrix& Result, double dvar, cMatrix& arg1, double darg2, cMatrix& arg3);
bool cubeAccess(cMatrix& Result, double dvar, cMatrix& arg1, cMatrix& arg2, double darg3);
//
bool cubeAccess(cCube& Result, double dvar, cMatrix& arg1, cMatrix& arg2, cMatrix& arg3);
//
bool cubeAccess(cMatrix& Result, cCube& var, double darg1, double darg2, double darg3);
bool cubeAccess(cMatrix& Result, cCube& var, double darg1, double darg2, cMatrix& arg3);
bool cubeAccess(cMatrix& Result, cCube& var, double darg1, cMatrix& arg2, double darg3);
bool cubeAccess(cMatrix& Result, cCube& var, double darg1, cMatrix& arg2, cMatrix& arg3);
bool cubeAccess(cMatrix& Result, cCube& var, cMatrix& arg1, double darg2, double darg3);
bool cubeAccess(cMatrix& Result, cCube& var, cMatrix& arg1, double darg2, cMatrix& arg3);
bool cubeAccess(cMatrix& Result, cCube& var, cMatrix& arg1, cMatrix& arg2, double darg3);
//
bool cubeAccess(cCube& Result, cCube& var, cMatrix& arg1, cMatrix& arg2, cMatrix& arg3);


bool arg2ix( cCube& Result, cMatrix& arg1, cMatrix& arg2, cMatrix& arg3, int nRows, int nCols, int nDepth );
//bool arg2ix( cCube& Result, cMatrix& arg1, cMatrix& arg2, int nRows, int nCols );
bool arg2ix( cCube& Result, double d1, cMatrix& arg2, cMatrix& arg3, int nRows, int nCols, int nDepth );
bool arg2ix( cCube& Result, double d1, double d2, cMatrix& arg3, int nRows, int nCols, int nDepth );
bool arg2ix( cCube& Result, double d1, cMatrix& arg2, double d3, int nRows, int nCols, int nDepth );
bool arg2ix( cCube& Result, cMatrix& arg1, double d2, cMatrix& arg3, int nRows, int nCols, int nDepth );
bool arg2ix( cCube& Result, cMatrix& arg1, double d2, double d3, int nRows, int nCols, int nDepth );
bool arg2ix( cCube& Result, cMatrix& arg1, cMatrix& arg2, double d3, int nRows, int nCols, int nDepth );

//bool ixAssign( cCube& lvalue, cCube& rvalue, cCube& idx );
//bool ixAssign( cCube& lvalue, cMatrix& rvalue, cCube& idx );
//bool ixAssign( cCube& lvalue, double drval, cCube& idx );
//bool ixAssign( cCube& lvalue, complex<double> cx, cCube& idx );


//
// accessor helpers
//
bool rm_rows(cCube& res, cCube& in, cMatrix& rows);
bool rm_cols(cCube& res, cCube& in, cMatrix& cols);
bool rm_depths(cCube& res, cCube& in, cMatrix& deps);

// interface fcns
//
bool Real(cCube& Result, cCube& A);
bool Imag(cCube& Result, cCube& A);
bool Conj(cCube& Result, cCube& A);
bool Fix(cCube& Result, cCube& A);
bool Floor(cCube& Result, cCube& A);
bool Round(cCube& Result, cCube& A);
bool Ceil(cCube& Result, cCube& A);
bool Phase(cCube& Result, cCube& A);
//
bool Not(cCube& Result, cCube& A);		// unitary NOT operation
//
bool Add(cCube& res, cCube& M1, cCube& M2);
bool Add(cCube& res, cCube& M1, cMatrix& M2);
bool Add(cCube& res, cCube& M1, double d);
bool Add(cCube& res, cCube& M1, complex<double>& cv);
bool Sub(cCube& res, cCube& M1, cCube& M2);
bool Sub(cCube& res, cCube& M1, double d);
bool Sub(cCube& res, double d, cCube& M2);
//
bool Mult(cCube& Result, cCube& A1, double d);
bool DotMult(cCube& Result, cCube& A1, cCube& A2 );
//
bool DotDiv(cCube& Result, cCube& A1, cCube& A2 );
bool DotDiv(cCube& Result, cCube& A1, double d );
bool DotDiv(cCube& Result, double d, cCube& A2 );
//
bool DotPow(cCube& Result, cCube& A1, cCube& A2);
bool DotPow(cCube& Result, double A1, cCube& A2);
bool DotPow(cCube& Result, cCube& A1, double A2);
//
bool Mod(cCube& Result, cCube& x, cCube& y);
bool Mod(cCube& Result, double x, cCube& y);
bool Mod(cCube& Result, cCube& x, double y);
//
bool Factorial(cCube& Result, cCube& A);
//
bool is_empty(cMatrix& res, cCube& M);
bool is_real(cMatrix& res, cCube& M);
bool is_vector(cMatrix& res, cCube& M);
bool is_scalar(cMatrix& res, cCube& M);
//bool is_square(cMatrix& res, cCube& M);
bool is_finite(cCube& res, cCube& M);
bool is_nan(cCube& res, cCube& M);
//
bool sin(cCube& res, cCube& m);
bool cos(cCube& res, cCube& m);
bool tan(cCube& res, cCube& m);
bool asin(cCube& res, cCube& m);
bool acos(cCube& res, cCube& m);
bool atan(cCube& res, cCube& m);
bool csc(cCube& res, cCube& m);
bool sec(cCube& res, cCube& m);
bool cot(cCube& res, cCube& m);
bool acsc(cCube& res, cCube& m);
bool asec(cCube& res, cCube& m);
bool acot(cCube& res, cCube& m);
bool Atan2(cCube& res, cCube& A1, cCube& A2);
//
bool sinh(cCube& res, cCube& m);
bool cosh(cCube& res, cCube& m);
bool tanh(cCube& res, cCube& m);
bool asinh(cCube& res, cCube& m);
bool acosh(cCube& res, cCube& m);
bool atanh(cCube& res, cCube& m);
bool csch(cCube& res, cCube& m);
bool sech(cCube& res, cCube& m);
bool coth(cCube& res, cCube& m);
bool acsch(cCube& res, cCube& m);
bool asech(cCube& res, cCube& m);
bool acoth(cCube& res, cCube& m);
//
bool log(cCube& res, cCube& m);
bool log10(cCube& res, cCube& m);
bool log2(cCube& res, cCube& m);
bool sqrt(cCube& res, cCube& m);
bool exp(cCube& res, cCube& m);
//
bool erf(cCube& res, cCube& m);
bool erfc(cCube& res, cCube& m);
bool besselj0(cCube& res, cCube& m);
bool besselj1(cCube& res, cCube& m);
bool besseljn(cCube& res, cCube& order, cCube& m);
bool bessely0(cCube& res, cCube& m);
bool bessely1(cCube& res, cCube& m);
bool besselyn(cCube& res, cCube& order, cCube& m);
//
bool lgamma(cCube& Res, cCube& R);
bool gamma(cCube& Res, cCube& R);
//
bool Rand(cCube& Result, cMatrix& vRows, cMatrix& vCols, cMatrix& vDepth);
bool Randn(cCube& Result, cMatrix& vRows, cMatrix& vCols, cMatrix& vDepth);
