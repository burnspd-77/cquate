/* matrix.h
cMatrix class definition.
Also, prototypes for several math functions, which
are implemented in mathlib.cpp, matrix.cpp, trigmathlib.cpp, and random.cpp.
The matrix class contains a pointer to an array of double
precision elements. The size of the array is simply
(nr*nc). The data in the matrix is ordered by columns;
that is, the first 'nr' entries in the matrix buffer correspond
to the first column in the matrix.

Created by Paul Burns in 2000-2003.
Revised in 2010.
Revised again in 2013.
*/

#pragma once

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits>
#include<limits.h>
#include <vector>
#include <string>
#include <algorithm>
#include <time.h>
#include <assert.h>
#include <cfloat>
#include <complex>
#include "my_errors.h"
#include "variable.h"
#include <cmath>

using std::vector;
using std::string;
using std::complex;

// matrix math error messages
#define __NO_ERRORS				0
#define SINGULAR_MATRIX			1
#define ILL_CONDITIONED			2
#define DIMENSION_MISMATCH		3
#define MEMORY_ERROR			4

#define __MATRIX_DEBUG		// uncomment to turn on debug checks

//#define __LITTLE_ENDIAN

#ifdef __LITTLE_ENDIAN
#define __HI(x) *(1+(int*)&x)
#define __LO(x) *(int*)&x
#define __HIp(x) *(1+(int*)x)
#define __LOp(x) *(int*)x
#else
#define __HI(x) *(int*)&x
#define __LO(x) *(1+(int*)&x)
#define __HIp(x) *(int*)x
#define __LOp(x) *(1+(int*)x)
#endif

#define PI 3.14159265358979323846L
#define D2R (PI/180.)
#define R2D (180./PI)

#define DCREATE(m,size) if ((m)!=NULL) {delete [](m);} if ((size)>0) { m = new double[(size)]; }
#define CCREATE(m,size) if ((m)!=NULL) {delete [](m);} if ((size)>0) { m = new Complex[(size)]; }
#define MDELETE(m) if ((m)!=NULL) { delete [](m); m = NULL; }

enum { LIMIT_SQUARE = 0, LIMIT_ROUND };		// is an interval inclusive on the end, or not?

// structure used for sorting (when ix is needed)
typedef struct tagSortElem {
	int ix;
	double dv;
} sortElem;


// sorting function
bool elemLessThan( sortElem ei, sortElem ej );

class cMatrix : public cVariable
{
public:
	cMatrix() {
		m_nType = VAR_TYPE_NUMERIC; nr = nc = 0;
		//if (!cMatrix::m_bLog) {
		//	cMatrix::m_bLog = true;
		//	pfLog = fopen("c:\\source\\cont_log.txt","w");
		//}
	}
	~cMatrix() { Delete(); }
	cMatrix( const cMatrix& A );

	bool operator=( const cMatrix& );  // assignment operator
	bool operator=( const cMatrix* pA );
	bool operator=( vector<double>& );  // assignment operator
	bool operator=( double& );  // assignment operator
	bool operator=( complex<double>& );  // assignment operator

	void Log(const char *pch, double *pd) {
		//fprintf(pfLog, "%s\t%x\n", pch, pd);
	}

	double * NewRealDouble(int size);
	double * NewImagDouble(int size);

	// creation, deletion
	bool Create(const int size);
	bool Create(const int r, const int c);
	bool Create(const int r, const int c, const double *pr);
	bool CreateComplex(const int size);
	bool CreateComplex(const int r, const int c);
	bool CreateComplex(vector<double>& re, vector<double>& im);
	bool Create(cMatrix& mat);			// create with same size as mat
	bool CreateComplex(cMatrix& mat);	// create complex with same size as mat	bool CreateComplex(vector<double>& re, vector<double>& im);
	bool CreateImaginary(const int r, const int c);
	bool CreateRow( vector<double>& v );
	bool CreateColumn( vector<double>& v );
	bool Delete(void) {
		//bool bRet(false);
		//if (mr!=NULL) { Log("delete_mat_real", mr); delete []mr; mr = NULL; bRet = true; } 
		//if (mi!=NULL) { Log("delete_mat_imag", mi); delete []mi; mi = NULL; bRet = true; }
		//nr = nc = 0;
		//return bRet;
		nr = nc = 0;
		mr.clear(); mi.clear(); return false;
	}
	bool Copy(const cMatrix&);
	bool CopyR(const cMatrix&);
	bool CopyI(const cMatrix&);
	bool DeleteCopy(const cMatrix&);
	//
	bool DeleteCopyR(const double *pd, int rows, int columns);
	bool DeleteCopyI(const double *pd, int rows, int columns);


	void Reset() {
		int sz = GetSize();
		resetReal(sz);
		if (!IsReal()) resetImag(sz);
	}
	void resetReal(int sz) {
		for (int i=0; i<sz; ++i) { mr[i] = 0; }
	}	
	void resetImag(int sz) {
		for (int i=0; i<sz; ++i) { mi[i] = 0; }
	}

	void Empty() { Delete(); }

	// Classification
	bool IsEmpty(void) const { return nr*nc==0; }
	bool IsScalar(void) const;
	bool IsReal(void) const { return mi.empty(); }
	bool IsVector(int*) const;		// if true, int arg is 0 for col vector, 1 for row vector
	bool IsVector(void) const;
	bool IsFinite(void) const;
	bool IsFinite(cMatrix& ret) const;
	bool IsNan(cMatrix& ret) const;
	bool IsSquare(void) const { return nr==nc; }
	// get functions
	//
	int GetType() { return m_nType; }
	int GetRows(void) const { return nr; }
	int GetColumns(void) const { return nc; }
	int GetSize(void) const { return nr*nc; }

	// ix helper
	int GetIndex(const int i, const int j) const { return i+nr*j; }
	// direct access to matrix
	double *GetR(void) { return mr.data(); }
	double *GetI(void) { return mi.data(); }
	const double *GetSafeR(void) const { return (const double *) mr.data(); }
	const double *GetSafeI(void) const { return (const double *) mi.data(); }

	// safe getting (with arg checking):
	bool GetSafe(complex<double>& c, int i, int j) const;		// with arg checking
	bool GetSafe(complex<double>& c, int i) const;
	bool GetSafeR(double& d, int i) const;
	bool GetSafeR(double& d, int i, int j) const;
	bool GetSafe0(complex<double>& c) const;
	bool GetSafe0(double& d) const;
	bool GetRz(double& d, int i, int j) const;
	bool GetIz(double& d, int i, int j) const;
	bool Getz(complex<double>& c, int i, int j) const;
	bool GetR(cMatrix&, const int, const int, const int, const int) const;	// returns (i,j)-(n,m) sub-matrix
	bool GetC(cMatrix& subMat, const int i, const int j, const int m, const int n) const;
	// no arg checking! (faster) use with caution
	void GetR(double&, const int ix) const;
	void GetR(double&, const int, const int) const;	// returns pointer to (row, column) element
	void GetI(double&, const int ix) const;
	void GetI(double&, const int, const int) const;
	void Get0(double&) const;
	void Get0(double& dr, double& di) const;
	void Get0(complex<double>& c) const;
	double Get0R(void) const { return mr[0]; }
	complex<double> Get0C(void) const { return complex<double>(mr[0],mi[0]); }
	void Get(complex<double>& c, const int ix) const;
	void Get(complex<double>& c, const int i, const int j) const;
	void GetC(complex<double>& c, int i, int j) const;		// redundant!
	void GetC(complex<double>& c, int i) const;
	void GetVectC(vector<complex<double> >& vc) const {
		vc.clear();
		complex<double> c;  int sz = GetSize();
		for (int i=0; i<sz; ++i) { c = complex<double>(mr[i],mi[i]); vc.push_back(c); }
	}

	bool GetFirstR(double& lf) const;
	bool GetLastR(double& lf) const;

	// set functions
	//
	bool SetTo(double d);				// set to real scalar
	bool SetTo(complex<double> c);		// set to complex scalar
	bool SetTo(cMatrix& m);				// copy from another matrix

	// with arg checking:
	bool SetSafeR(const int ix, double d);
	bool SetSafeR(const int, const int, double);
	bool SetSafeI(const int i, double x);
	bool SetSafe(const int ix, complex<double>& c);
	bool SetSafe(const int i, const int j, complex<double>& c);
	// no arg checking! (faster):
	bool SetR( std::vector<double>& A );
	bool SetR(const int, const int, const cMatrix&);	// sets (i,j)-(n,m)th sub-matrix. Size determined by size of arg													// (always safe)
	void SetR(const int ix, const double d);
	void SetR(const int, const int, const double);
	void SetI(const int i, const double x);
	void SetI(const int i, const int j, double x);
	void Set(const int ix, complex<double>& c);
	void Set(const int, const int, complex<double>& c);
	bool Set(const int, const int, const cMatrix&);		// sets (i,j)-(n,m)th sub-matrix. Size determined by size of arg
	void SetRows(int r) { nr = r; }
	void SetColumns(int c) { nc = c; }
	void SetVectC(vector<complex<double> >& vc) {
		Delete(); int sz = vc.size();
		mr.reserve(sz); mi.reserve(sz);
		for (int i=0; i<sz; ++i) { mr.push_back(vc[i].real()); mi.push_back(vc[i].imag()); }
		nr = sz;
		nc = 1;
	}

	// file I/O
	bool SaveAscii(FILE *pf);
	bool Save(FILE *pf);
	bool Load(FILE *pf);

	// misc
	//
	void forceReal(void);
	bool forceComplex(void);

	string ToString() {
		char pch[128];
		string ret;
		double lf;
		complex<double> cx;
		int r, c;

		if (IsReal()) {
			for (r=0; r<nr; ++r) {
				for (c=0; c<nc-1; ++c) {
					GetR(lf,r,c);
					sprintf(pch,"%f\t",lf);
					ret.append( pch );
				}
				GetR(lf,r,nc-1);
				sprintf(pch,"%f",lf);
				ret.append( pch );
				if (r < nr-1) {
					ret.append("\n");
				}
			}
		}
		else {
			for (r=0; r<nr; ++r) {
				for (c=0; c<nc-1; ++c) {
					Get(cx,r,c);
					sprintf(pch,"(%f,%f)\t",cx.real(),cx.imag());
					ret.append( pch );
				}
				Get(cx,r,nc-1);
				sprintf(pch,"(%f,%f)",cx.real(),cx.imag());
				ret.append( pch );
				if (r < nr-1) {
					ret.append("\n");
				}
			}
		}
		return ret;
	}

	bool ToScalar(string& s)
	{
		double lf = atof( s.c_str() );
		SetTo( lf );

		return false;
	}

	// filling
	bool FillR(int pos, double *src, int len);
	bool FillI(int pos, double *src, int len);
	bool FillR(int pos, vector<double>::iterator src, int len);
	bool FillI(int pos, vector<double>::iterator src, int len);
	bool FillR(int pos, double val, int len);
	bool FillI(int pos, double val, int len);
	bool FillColumnR(int, double*);
	bool AugmentColumnsR(cMatrix *);
	bool AugmentColumnsI(cMatrix *);
	bool AugmentColumns(cMatrix *);
	bool AugmentRowsR(cMatrix *);
	bool AugmentRows(cMatrix *);
	bool GetColumnR(cMatrix *, int);
	bool GetColumnI(cMatrix *Result, int c);
	bool GetColumn(cMatrix *, int);
	bool GetRowR(cMatrix *Result, int r);
	bool GetRowI(cMatrix *Result, int r);
	bool GetRow(cMatrix *Result, int r);
	bool SwapRows(int,int);			// swap 2 rows

	// modulo (move this somewhere else?)
	bool Mod(double&);
	bool Mod(complex<double>&);

	bool Herm(void);
	bool Trans(void);
	bool Reshape(int rows, int columns);
	bool Rot90(double dRot);					// rotate this
	bool Rot90(cMatrix& Res, double dRot);		// return the rotated mat

	bool MinR(double&);
	bool MinNumberR(double&);			// min double that is not a NonNumber
	bool MinPosNumberR(double&);		// min double that is greater than zero
	bool MinVectR(cMatrix&);
	bool MinIndexR(double&,double&);
	bool MinVectIndexR(cMatrix&,cMatrix&);
	bool MaxR(double&);
	bool MaxNumberR(double&);			// max double that is not a NonNumber
	bool MaxVectR(cMatrix&);
	bool MaxIndexR(double&,double&);
	bool MaxVectIndexR(cMatrix&,cMatrix&);
	void SetColumnsToCount(void);			// Sets the columns to 1, 2, 3,... rows.
	void SetToCount(void);
	bool SetToCount(int n);
	bool SetToRange(double dStart, int n);
	bool SetToRange(double dStart, double dEnd, double dIncr);
	bool SetToRange(double&);
	bool SetToRange(double& dStart, double& dIncr);

	void FlipudR(void);
	void FliplrR(void);
	void Flipud(void);
	void Fliplr(void);

	int MaxDim(void) {
		return nr > nc ? nr : nc;
	}
	bool GetDiag(cMatrix& vect) {
		int len = MaxDim();
		int i;
		double lfr;
		complex<double> cx;
		if (IsReal()) {
			if (vect.Create(len,1))
				return true;
			for (i=0; i<len; ++i) {
				GetR(lfr,i,i);
				vect.SetR(i,lfr);
			}
		}
		else {
			if (vect.CreateComplex(len,1))
				return true;
			for (i=0; i<len; ++i) {
				Get(cx,i,i);
				vect.Set(i,cx);
			}
		}
		return false;
	}

	bool SumSquare(cMatrix& Result, int idim);
	bool Sum(cMatrix&,int);
	double StraightSumR(void);
	complex<double> StraightSumC(void);
	bool Prod(cMatrix&,int);
	bool CumSumR(cMatrix&,int);
	bool CumSum(cMatrix&,int);
	bool CumProdR(cMatrix& Result, int idim);
	bool CumProd(cMatrix&,int);
//	bool Find(cMatrix&);
	bool FindR(cMatrix&);
	bool Find(cMatrix& Result, cMatrix& iRows, cMatrix& iCols);

	bool Sort(cMatrix& Result);					// sort this
	bool SortReal(cMatrix& Res);				// return the sorted mat
	bool SortComplex(cMatrix& Result);
	bool SortIndex(cMatrix& Result, cMatrix& Ix);
	bool SortIndexReal(cMatrix&, cMatrix&);
	bool SortIndexComplex(cMatrix&, cMatrix&);
	bool column2elemR(vector<sortElem>& data, int icol);
	bool column2elemC(vector<sortElem>& data, int icol);
	bool elem2columnR(cMatrix& Result, cMatrix& Ix, vector<sortElem>& data, int icol);
	bool elem2columnC(cMatrix& Result, cMatrix& Ix, vector<sortElem>& data, int icol);

	// access operator (two args), lvalue
	bool matIxAccess(cMatrix& Result, double darg1, double darg2);
	bool matIxAccess(cMatrix& Result, cMatrix& arg1, double darg2);
	bool matIxAccess(cMatrix& Result, double darg1, cMatrix& arg2);
	bool matIxAccess(cMatrix& Result, cMatrix& arg1, cMatrix& arg2);

	// Logical operators
	//
	//friend cMatrix operator==(const cMatrix& R, const cMatrix& C);
	cMatrix operator==(const cMatrix& R) const;
	cMatrix operator==(const complex<double>& x) const;
	cMatrix operator==(const double d) const;
	friend cMatrix operator==(const double d, const cMatrix& R);
	friend cMatrix operator==(const complex<double>& x, const cMatrix& R);

	//friend cMatrix operator!=(const cMatrix& R, const cMatrix& C);
	cMatrix operator!=(const cMatrix& R) const;
	cMatrix operator!=(const complex<double>& x) const;
	cMatrix operator!=(const double d) const;
	friend cMatrix operator!=(const double d, const cMatrix& R);
	friend cMatrix operator!=(const complex<double>& x, const cMatrix& R);

	//friend cMatrix operator<(const cMatrix& R, const cMatrix& C);
	cMatrix operator<(const cMatrix& R) const;
	cMatrix operator<(const complex<double>& x) const;
	cMatrix operator<(const double d) const;
	friend cMatrix operator<(const double d, const cMatrix& R);
	friend cMatrix operator<(const complex<double>& x, const cMatrix& R);

	//friend cMatrix operator>(const cMatrix& R, const cMatrix& C);
	cMatrix operator>(const cMatrix& R) const;
	cMatrix operator>(const complex<double>& x) const;
	cMatrix operator>(const double d) const;
	friend cMatrix operator>(const double d, const cMatrix& R);
	friend cMatrix operator>(const complex<double>& x, const cMatrix& R);

	//friend cMatrix operator<=(const cMatrix& R, const cMatrix& C);
	cMatrix operator<=(const cMatrix& R) const;
	cMatrix operator<=(const complex<double>& x) const;
	cMatrix operator<=(const double d) const;
	friend cMatrix operator<=(const double d, const cMatrix& R);
	friend cMatrix operator<=(const complex<double>& x, const cMatrix& R);

	//friend cMatrix operator>=(const cMatrix& R, const cMatrix& C);
	cMatrix operator>=(const cMatrix& R) const;
	cMatrix operator>=(const complex<double>& x) const;
	cMatrix operator>=(const double d) const;
	friend cMatrix operator>=(const double d, const cMatrix& R);
	friend cMatrix operator>=(const complex<double>& x, const cMatrix& R);

	//friend cMatrix operator&&(const cMatrix& R, const cMatrix& C);
	cMatrix operator&&(const cMatrix& R) const;
	cMatrix operator&&(const complex<double>& x) const;
	cMatrix operator&&(const double d) const;
	friend cMatrix operator&&(const double d, const cMatrix& R);
	friend cMatrix operator&&(const complex<double>& x, const cMatrix& R);

	//friend cMatrix operator||(const cMatrix& R, const cMatrix& C);
	cMatrix operator||(const cMatrix& R) const;
	cMatrix operator||(const complex<double>& x) const;
	cMatrix operator||(const double d) const;
	friend cMatrix operator||(const double d, const cMatrix& R);
	friend cMatrix operator||(const complex<double>& x, const cMatrix& R);

public:
	// data
	vector<double> mr, mi;

	int nr, nc;	// no. rows, columns

	static bool m_bLog;
};


//
// prototypes
//

// access operator (one arg), rvalue
//
bool matAccess(cMatrix& Result, double dvar, double darg);
bool matAccess(cMatrix& Result, double dvar, cMatrix& arg);
//bool matAccess(cMatrix& Result, cMatrix& var, double darg);
//bool matAccess(cMatrix& Result, cMatrix& var, cMatrix& arg);
// access operator (two args), rvalue
//
bool matAccess(cMatrix& Result, double dvar, double darg1, double darg2);
bool matAccess(cMatrix& Result, double dvar, double darg1, cMatrix& arg2);
bool matAccess(cMatrix& Result, double dvar, cMatrix& arg1, double darg2);
bool matAccess(cMatrix& Result, double dvar, cMatrix& arg1, cMatrix& arg2);
//bool matAccess(cMatrix& Result, cMatrix& var, double darg1, cMatrix& arg2);
//bool matAccess(cMatrix& Result, cMatrix& var, cMatrix& arg1, double darg2);
//bool matAccess(cMatrix& Result, cMatrix& var, double darg1, double darg2);
//bool matAccess(cMatrix& Result, cMatrix& var, cMatrix& arg1, cMatrix& arg2);

bool arg2ix( cMatrix& Result, cMatrix& arg1, cMatrix& arg2, int nRows, int nCols );
bool arg2ix( cMatrix& Result, double d1, cMatrix& arg2, int nRows, int nCols );
bool arg2ix( cMatrix& Result, cMatrix& arg1, double d2, int nRows, int nCols );

//bool ixAssign( cMatrix& lvalue, cMatrix& rvalue, cMatrix& idx );
//bool ixAssign( cMatrix& lvalue, double drval, cMatrix& idx );
//bool ixAssign( cMatrix& lvalue, complex<double> cx, cMatrix& idx );

bool rm_rows(cMatrix& res, cMatrix& in, cMatrix& rows);
bool rm_cols(cMatrix& res, cMatrix& in, cMatrix& cols);

// File I/O
//
bool WriteData(cMatrix& v, FILE *pf);
bool ReadData(cMatrix& m, FILE *pf);

// Misc
//
void MemAllocErrorMessage(void);
bool SameSize(const cMatrix& R1, const cMatrix& R2);
bool AnyR(cMatrix& R);
bool checkRealDomain(double vL, int l_type, double vR, int r_type, cMatrix& M);
bool Length(cMatrix& Result, cMatrix& v);
bool Size(cMatrix& Result, cMatrix& v);
bool Rows(cMatrix& Result, cMatrix& v);
bool Columns(cMatrix& Result, cMatrix& v);

bool TimeLapse(cMatrix& Result);
bool TimeSet();

bool FindR(cMatrix& Result, cMatrix& R);
bool FindC(cMatrix& Result, cMatrix& C);
bool Find(cMatrix& Result, cMatrix& A);

bool Resize(cMatrix& Result, cMatrix& v, cMatrix& vRows, cMatrix& vCols);
bool Rotate(cMatrix& Result, cMatrix& v, cMatrix& vRot);
bool RotateCw(cMatrix& Result, cMatrix& v, cMatrix& vRot);

bool Sort(cMatrix& Result, cMatrix& v);
bool SortIndex(cMatrix& Result, cMatrix& ResIndex, cMatrix& v);
bool Unique(cMatrix& Result, cMatrix& X);

bool Real(cMatrix& Result, cMatrix& A);
bool Imag(cMatrix& Result, cMatrix& A);
bool ZerosR(cMatrix& Result, int nrows, int ncols);
bool ZerosC(cMatrix& Result, int nrows, int ncols);
bool Conj(cMatrix& Result, cMatrix& A);
bool TransR(cMatrix& Result, cMatrix& R);
bool Trans(cMatrix& Result, cMatrix& A);
bool Herm(cMatrix& Result, cMatrix& A);

bool Not(cMatrix& Result, cMatrix& A);		// unitary NOT operation


// horzcat/vertcat
//
bool VertCat(cMatrix& Result, double v1, double v2);
bool VertCatR(cMatrix& Result, double d1, cMatrix& R2);
bool VertCat(cMatrix& Result, double v1, cMatrix& v2);
bool VertCatR(cMatrix& Result, cMatrix& R1, double d2);
bool VertCat(cMatrix& Result, cMatrix& v1, double v2);
bool VertCatR(cMatrix& Result, cMatrix& A1, cMatrix& R2);
bool RVertCat(cMatrix& Result, cMatrix& R1, cMatrix& A2);
bool RVertCatR(cMatrix& Result, cMatrix& R1, cMatrix& R2);
bool VertCat(cMatrix& Result, cMatrix& v1, cMatrix& v2);

bool HorzCat(cMatrix& Result, double d1, double d2);
bool HorzCatR(cMatrix& Result, double d1, cMatrix& R2);
bool HorzCat(cMatrix& Result, double d1, cMatrix& A2);
bool RHorzCat(cMatrix& Result, cMatrix& R1, double d2);
bool HorzCat(cMatrix& Result, cMatrix& A1, double d2);
bool HorzCatR(cMatrix& Result, cMatrix& A1, cMatrix& R2);
bool RHorzCatR(cMatrix& Result, cMatrix& R1, cMatrix& R2);
bool HorzCat(cMatrix& Result, cMatrix& v1, cMatrix& v2);

// flipud/fliplr
//
bool FlipudR(cMatrix& Result, cMatrix& R);
bool FlipudC(cMatrix& Result, cMatrix& C);
bool FliplrR(cMatrix& Result, cMatrix& R);
bool FliplrC(cMatrix& Result, cMatrix& C);
// interface fcns
bool Fliplr(cMatrix& Result, cMatrix& A);
bool Flipud(cMatrix& Result, cMatrix& A);

bool FillMatrixR(cMatrix& Result, double dv, int nr, int nc);
bool FillMatrixC(cMatrix& Result, complex<double>& cv, int nr, int nc);
bool SetDiagR(cMatrix& Result, double dv);
bool SetDiagC(cMatrix& Result, complex<double>& cv);
int BlockDiagR(cMatrix& Result, cMatrix& A, cMatrix& B);
int BlockDiagC(cMatrix& Result, cMatrix& A, cMatrix& B);
bool FillDiagR(cMatrix& Result, double dv, int nr, int nc);
bool FillDiagC(cMatrix& Result, complex<double>& cv, int nr, int nc);
bool FillDiag(cMatrix& Result, cMatrix& diag);
bool FillIdentity(cMatrix& Result, int nr, int nc);
bool FillIdentity(cMatrix& Result, int ndim);
// interfaces
bool NewMat(cMatrix& Cout, cMatrix& vn);
bool NewMat(cMatrix& Cout, cMatrix& vr, cMatrix& vc);
//bool NewMat(cMatrix& Cout, cMatrix& v,  cMatrix& vr, cMatrix& vc);
bool NewDiag(cMatrix& Cout, cMatrix& vn);
bool NewDiag(cMatrix& Cout, cMatrix& v, cMatrix& vn);
bool NewDiag(cMatrix& Cout, cMatrix& v, cMatrix& vr, cMatrix& vc);
bool NewIdentity(cMatrix& Cout, cMatrix& vn);
bool NewIdentity(cMatrix& Cout, cMatrix& vr, cMatrix& vc);

bool GetDiagR(cMatrix& Result, cMatrix& mat);
bool GetDiagC(cMatrix& Result, cMatrix& mat);
// interface
bool GetDiag(cMatrix& Result, cMatrix& mat);

bool Repeat(cMatrix& res, cMatrix& in, cMatrix& vr, cMatrix& vc);

//
// access to elements
//

// access operator (one arg), rvalue
bool valAccess(cMatrix& Result, double dvar, double darg);
bool valAccess(cMatrix& Result, double dvar, cMatrix& arg);
bool valAccess(cMatrix& Result, cMatrix& var, double darg);
bool valAccess(cMatrix& Result, cMatrix& var, cMatrix& arg);
// access operator (two args), rvalue
bool valAccess(cMatrix& Result, double dvar, double darg1, double darg2);
bool valAccess(cMatrix& Result, double dvar, double darg1, cMatrix& arg2);
bool valAccess(cMatrix& Result, double dvar, cMatrix& arg1, double darg2);
bool valAccess(cMatrix& Result, double dvar, cMatrix& arg1, cMatrix& arg2);
bool valAccess(cMatrix& Result, cMatrix& var, double darg1, cMatrix& arg2);
bool valAccess(cMatrix& Result, cMatrix& var, cMatrix& arg1, double darg2);
bool valAccess(cMatrix& Result, cMatrix& var, double darg1, double darg2);
bool valAccess(cMatrix& Result, cMatrix& var, cMatrix& arg1, cMatrix& arg2);

bool ixAssign( cMatrix& lvalue, cMatrix& rvalue, cMatrix& idx );

//
// operator prototypes
//
cMatrix operator==(const double, const cMatrix&);
cMatrix operator==(const complex<double>&, const cMatrix&);

cMatrix operator!=(const double, const cMatrix&);
cMatrix operator!=(const complex<double>&, const cMatrix&);

cMatrix operator<(const double, const cMatrix&);
cMatrix operator<(const complex<double>&, const cMatrix&);

cMatrix operator>(const double, const cMatrix&);
cMatrix operator>(const complex<double>&, const cMatrix&);

cMatrix operator<=(const double, const cMatrix&);
cMatrix operator<=(const complex<double>&, const cMatrix&);

cMatrix operator>=(const double, const cMatrix&);
cMatrix operator>=(const complex<double>&, const cMatrix&);

cMatrix operator&&(const double, const cMatrix&);
cMatrix operator&&(const complex<double>&, const cMatrix&);

cMatrix operator||(const double, const cMatrix&);
cMatrix operator||(const complex<double>&, const cMatrix&);

// basic operation support
//
bool RAddR(cMatrix& res, const double x, const cMatrix& R);		// real matrix add
bool RAddR(cMatrix& res, const cMatrix& R1, const cMatrix& R2);	// real and real matrix add
bool RAddC(cMatrix& res, const double x, const cMatrix& C);		// complex matrix add
bool CAddR(cMatrix& res, const cMatrix& C, const cMatrix& R);	// complex and real matrix add
bool CAddR(cMatrix& res, const complex<double>& x, const cMatrix& R);	// complex scalar matrix add
bool RAddC(cMatrix& res, const cMatrix& R, const cMatrix& C);	// real and complex matrix add
bool CAddC(cMatrix& res, const cMatrix& C1, const cMatrix& C2);	// complex and complex matrix add
bool CAddC(cMatrix& res, const complex<double>& x, const cMatrix& C);	// complex scalar matrix add

bool RSubR(cMatrix& res, const double x, const cMatrix& R);		// real matrix sub
bool RSubR(cMatrix& res, const cMatrix& R, const double x);		// real matrix sub
bool RSubR(cMatrix& res, const cMatrix& R1, const cMatrix& R2);	// real and real matrix sub
bool RSubC(cMatrix& res, const double x, const cMatrix& C);		// complex matrix sub
bool RSubC(cMatrix& res, const cMatrix& R, complex<double>& x);	// real complex sub
bool CSubR(cMatrix& res, const cMatrix& C, const cMatrix& R);	// complex and real matrix sub
bool CSubR(cMatrix& res, const complex<double>& x, const cMatrix& R);	// complex and real matrix sub
bool RSubC(cMatrix& res, const cMatrix& R, const cMatrix& C);	// real and complex matrix add
bool CSubC(cMatrix& res, const cMatrix& C1, const cMatrix& C2);	// complex and complex matrix add
bool CSubC(cMatrix& res, const complex<double>& x, const cMatrix& C);	// complex and complex matrix add
bool CSubC(cMatrix& res, const cMatrix& C, const complex<double>& x);	// complex and complex matrix add
bool CSubR(cMatrix& res, const cMatrix& C, const double x);	// complex and complex matrix add

bool RMultR(cMatrix& res, double x, cMatrix& R);
bool RMultR(cMatrix& res, cMatrix& R1, cMatrix& R2);
bool RMultC(cMatrix& res, cMatrix& R, cMatrix& C);
bool RMultC(cMatrix& res, double x, cMatrix& C);
bool CMultR(cMatrix& res, complex<double>& c, cMatrix& R);
bool CMultR(cMatrix& res, cMatrix& C, cMatrix& R);
bool CMultC(cMatrix& res, complex<double>& c, cMatrix& C);
bool CMultC(cMatrix& res, cMatrix& C1, cMatrix& C2);

bool RDotMultR(cMatrix& Result, const cMatrix& R, const double d);
bool RDotMultR(cMatrix& Result, const double d, const cMatrix& R);
bool RDotMultR(cMatrix& Result, const cMatrix& R1, const cMatrix& R2);
bool RDotMultC(cMatrix& Result, const cMatrix& R, const complex<double>& c);
bool RDotMultC(cMatrix& Result, const double d, const cMatrix& C);
bool RDotMultC(cMatrix& Result, const cMatrix& R, const cMatrix& C);
bool CDotMultR(cMatrix& Result, const complex<double>& c, const cMatrix& R);
bool CDotMultR(cMatrix& Result, const cMatrix& C, const double d);
bool CDotMultR(cMatrix& Result, const cMatrix& C, const cMatrix& R);
bool CDotMultC(cMatrix& Result, const cMatrix& C, const complex<double>& c);
bool CDotMultC(cMatrix& Result, const complex<double>& c, const cMatrix& C);
bool CDotMultC(cMatrix& Result, const cMatrix& C1, const cMatrix& C2);
// interface fcn
bool DotMult(cMatrix& Result, cMatrix& A1, cMatrix& A2 );

bool RDotDivR(cMatrix& Result, const cMatrix& R, const double d);
bool RDotDivR(cMatrix& Result, const double d, const cMatrix& R);
bool RDotDivR(cMatrix& Result, const cMatrix& R1, const cMatrix& R2);
bool RDotDivC(cMatrix& Result, const cMatrix& R, const complex<double>& c);
bool RDotDivC(cMatrix& Result, const double d, const cMatrix& C);
bool RDotDivC(cMatrix& Result, const cMatrix& R, const cMatrix& C);
bool CDotDivR(cMatrix& Result, const complex<double>& c, const cMatrix& R);
bool CDotDivR(cMatrix& Result, const cMatrix& C, const double d);
bool CDotDivR(cMatrix& Result, const cMatrix& C, const cMatrix& R);
bool CDotDivC(cMatrix& Result, const cMatrix& C, const complex<double>& c);
bool CDotDivC(cMatrix& Result, const complex<double>& c, const cMatrix& C);
bool CDotDivC(cMatrix& Result, const cMatrix& C1, const cMatrix& C2);
// interface fcn
bool DotDiv(cMatrix& Result,  cMatrix& A1, cMatrix& A2 );
bool DotDiv(cMatrix& Result, cMatrix& A1, double d );
bool DotDiv(cMatrix& Result, double d, cMatrix& A2 );

complex<double> CPowC(complex<double>&, complex<double>&);			// x^y, x & y both complex
complex<double> CPow(complex<double>&, double);
complex<double> PowC(double, complex<double>&);
bool RDotPowR(cMatrix& Result, cMatrix& R, double d);
bool RDotPowR(cMatrix& Result, double d, cMatrix& R);
bool RDotPowR(cMatrix& Result, cMatrix& R1, cMatrix& R2);
bool RDotPowC(cMatrix& Result, double d, cMatrix& C);
bool RDotPowC(cMatrix& Result, cMatrix& R, std::complex<double>& c);
bool RDotPowC(cMatrix& Result, cMatrix& R, cMatrix& C);
bool CDotPowR(cMatrix& Result, std::complex<double>& c, cMatrix& R);
bool CDotPowR(cMatrix& Result, cMatrix& C, double d);
bool CDotPowR(cMatrix& Result, cMatrix& C, cMatrix& R);
bool CDotPowC(cMatrix& Result, std::complex<double>& c, cMatrix& C);
bool CDotPowC(cMatrix& Result, cMatrix& C, std::complex<double>& c);
bool CDotPowC(cMatrix& Result, cMatrix& C1, cMatrix& C2);
// interface fcn
bool DotPow(cMatrix& Result, cMatrix& A1, cMatrix& A2);
bool DotPow(cMatrix& Result, double A1, cMatrix& A2);
bool DotPow(cMatrix& Result, cMatrix& A1, double A2);

// sum/prod/cumulative
//
bool Sum(cMatrix& Cout, cMatrix& v);
bool Sum(cMatrix& res, cMatrix& v, int idim);
bool Sum(cMatrix& res, cMatrix& v, cMatrix& vDim);
bool CumSum(cMatrix& Cout, cMatrix& v);
bool CumSum(cMatrix& res, cMatrix& v, int idim);
bool CumSum(cMatrix& Cout, cMatrix& v, cMatrix& dim);
bool Prod(cMatrix& Cout, cMatrix& v);
bool Prod(cMatrix& res, cMatrix& v, int idim);
bool Prod(cMatrix& Cout, cMatrix& v, cMatrix& dim);
bool CumProd(cMatrix& Cout, cMatrix& v);
bool CumProd(cMatrix& res, cMatrix& v, int idim);
bool CumProd(cMatrix& Cout, cMatrix& v, cMatrix& dim);

// exponentials
//
complex<double> ExpC(complex<double>& x);
bool ExpR(cMatrix& Result, cMatrix& R);
bool ExpC(cMatrix& Result, cMatrix& C);
// interface fcn
bool Exp(cMatrix& Result, cMatrix& A);

// logarithm
//
int Log2(int);
complex<double> Clog(double r);
complex<double> logC(complex<double>& cv);
bool logPosR(cMatrix& Rout, cMatrix& R);
bool logNegR(cMatrix& Cout, cMatrix& R);
bool logR(cMatrix& Aout, cMatrix& R);
bool logC(cMatrix& Cout, cMatrix& C);
double logBase(double x, const double base);
complex<double> logBaseC(complex<double>& c, const double base);
bool logBaseR(cMatrix& Aout, cMatrix& R, const double base);
bool logBaseC(cMatrix& Cout, cMatrix& C, const double base);

// sqrt
//
complex<double> sqrtC(complex<double>& x);
bool sqrtC(cMatrix& Res, const cMatrix& C);
complex<double> sqrtNegR(double d);
bool sqrtNegR(cMatrix& Res, const cMatrix& R);
bool sqrtR(cMatrix& Res, const cMatrix& R);
// interface fcn
bool Sqrt(cMatrix& Result, cMatrix& x);

complex<double> sinC(complex<double>&);
bool sinR(cMatrix&,cMatrix&);
bool sinC(cMatrix&,cMatrix&);
//
complex<double> cosC(complex<double>&);
bool cosR(cMatrix&,cMatrix&);
bool cosC(cMatrix&,cMatrix&);
//
complex<double> tanC(complex<double>&);
bool tanR(cMatrix&,cMatrix&);
bool tanC(cMatrix&,cMatrix&);
//
complex<double> asinR(double d);
complex<double> asinC(complex<double>& c);
bool asinR(cMatrix& Cout,cMatrix& R);
bool asinC(cMatrix& Cout,cMatrix& C);
//
complex<double> acosR(double d);
complex<double> acosC(complex<double>& c);
bool acosR(cMatrix& Cout,cMatrix& R);
bool acosC(cMatrix& Cout,cMatrix& C);
//
complex<double> atanC(complex<double>& c);
bool atanR(cMatrix& Rout,cMatrix& R);
bool atanC(cMatrix& Cout,cMatrix& C);
//
complex<double> cscC(complex<double>& c);
bool cscR(cMatrix& Rout,cMatrix& R);
bool cscC(cMatrix& Cout,cMatrix& C);
//
complex<double> secC(complex<double>& c);
bool secR(cMatrix& Rout,cMatrix& R);
bool secC(cMatrix& Cout,cMatrix& C);
//
complex<double> cotC(complex<double>& c);
bool cotR(cMatrix& Rout,cMatrix& R);
bool cotC(cMatrix& Cout,cMatrix& C);
//
complex<double> acscR(double d);
complex<double> acscC(complex<double>& c);
bool acscR(cMatrix& Cout,cMatrix& R);
bool acscC(cMatrix& Cout,cMatrix& C);
//
complex<double> asecR(double d);
complex<double> asecC(complex<double>& c);
bool asecR(cMatrix& Cout,cMatrix& R);
bool asecC(cMatrix& Cout,cMatrix& C);
//
double acotR(double d);
complex<double> acotC(complex<double>& c);
bool acotR(cMatrix& Rout,cMatrix& R);
bool acotC(cMatrix& Cout,cMatrix& C);
//
double csc(double);
double sec(double);
double cot(double);
//
bool Catan2(cMatrix& Rout, cMatrix& C, cMatrix& R);
bool Catan2(cMatrix& Rout, complex<double>& c, cMatrix& R);
bool Catan2(cMatrix& Rout, cMatrix& C, double d);
bool atan2C(cMatrix& Rout, cMatrix& R, cMatrix& C);
bool atan2C(cMatrix& Rout, double d, cMatrix& C);
bool atan2C(cMatrix& Rout, cMatrix& R, complex<double>& c);
bool Catan2C(cMatrix& Rout, cMatrix& C1, cMatrix& C2);
bool Catan2C(cMatrix& Rout, complex<double>& c, cMatrix& C2);
bool Catan2C(cMatrix& Rout, cMatrix& C1, complex<double>& c);
bool atan2(cMatrix& Rout, cMatrix& R, double d);
bool atan2(cMatrix& Rout, double d, cMatrix& R);
bool atan2(cMatrix& Rout, cMatrix& R1, cMatrix& R2);
// atan2 interface
bool Atan2(cMatrix& Aout, cMatrix& A1, cMatrix& A2);
bool Atan2_RR(cMatrix& Aout, cMatrix& R1, cMatrix& R2);
bool Atan2_RC(cMatrix& Aout, cMatrix& R1, cMatrix& C2);
bool Atan2_CR(cMatrix& Aout, cMatrix& C1, cMatrix& R2);
bool Atan2_CC(cMatrix& Aout, cMatrix& C1, cMatrix& C2);


// hyperbolic trig
complex<double> sinhC(complex<double>& c);
bool sinh(cMatrix& Rout, cMatrix& R);
bool sinhR(cMatrix& Rout, cMatrix& R);
bool sinhC(cMatrix& Rout, cMatrix& C);
//
complex<double> coshC(complex<double>& c);
bool cosh(cMatrix& Rout, cMatrix& R);
bool coshR(cMatrix& Cout, cMatrix& R);
bool coshC(cMatrix& Cout, cMatrix& C);
//
complex<double> tanhC(complex<double>& c);
bool tanh(cMatrix& Rout, cMatrix& R);
bool tanhR(cMatrix& Rout, cMatrix& R);
bool tanhC(cMatrix& Cout, cMatrix& C);
//
//double asinh(double d);
complex<double> asinhC(complex<double>& c);
bool asinh(cMatrix& Rout, cMatrix& R);
bool asinhR(cMatrix& Rout, cMatrix& R);
bool asinhC(cMatrix& Cout, cMatrix& C);
//
double acoshPosR(double d);					// d<=1
complex<double> acoshNegR(double d);		// d<1
complex<double> acoshC(complex<double>& c);
bool acoshPosR(cMatrix& Rout, cMatrix& R);	// d<=1
bool acoshNegR(cMatrix& Cout, cMatrix& R);	// d<1
bool acoshR(cMatrix& Aout, cMatrix& R);
bool acoshC(cMatrix& Cout, cMatrix& C);
//
double atanhPosR(double d);					// |d|<1
complex<double> atanhNegR(double d);		// |d|>1
complex<double> atanhC(complex<double>& c);
bool atanhPosR(cMatrix& Rout, cMatrix& R);	// d<=1
bool atanhNegR(cMatrix& Cout, cMatrix& R);	// d<1
bool atanhR(cMatrix& Aout, cMatrix& R);
bool atanhC(cMatrix& Cout, cMatrix& C);
//
double csch(double d);
complex<double> cschC(complex<double>& c);
bool csch(cMatrix& Rout, cMatrix& R);
bool cschR(cMatrix& Rout, cMatrix& R);
bool cschC(cMatrix& Cout, cMatrix& C);
//
double sech(double d);
complex<double> sechC(complex<double>& c);
bool sech(cMatrix& Rout, cMatrix& R);
bool sechR(cMatrix& Rout, cMatrix& R);
bool sechC(cMatrix& Cout, cMatrix& C);
//
double coth(double d);
complex<double> cothC(complex<double>& c);
bool coth(cMatrix& Rout, cMatrix& R);
bool cothR(cMatrix& Rout, cMatrix& R);
bool cothC(cMatrix& Cout, cMatrix& C);
//
double acsch(double d);
complex<double> acschC(complex<double>& c);
bool acsch(cMatrix& Rout, cMatrix& R);
bool acschR(cMatrix& Rout, cMatrix& R);
bool acschC(cMatrix& Cout, cMatrix& C);
//
complex<double> asech(double d);
complex<double> asechC(complex<double>& c);
bool asech(cMatrix& Cout, cMatrix& R);
bool asechR(cMatrix& Cout, cMatrix& R);
bool asechC(cMatrix& Cout, cMatrix& C);
//
complex<double> acoth(double d);
complex<double> acothC(complex<double>& c);
bool acoth(cMatrix& Cout, cMatrix& R);
bool acothR(cMatrix& Cout, cMatrix& R);
bool acothC(cMatrix& Cout, cMatrix& C);

//
// special functions
//

//double erf(double x);
//double erfc(double x);
bool erfR(cMatrix& Result, cMatrix& x);
bool erfcR(cMatrix& Result, cMatrix& x);
bool j0R(cMatrix& Result, cMatrix& R);			// Bessel fcn of first kind
bool j1R(cMatrix& Result, cMatrix& R);
bool jnR(cMatrix& Result, int iorder, cMatrix& R);
bool y0R(cMatrix& Result, cMatrix& R);			// Bessel fcn of second kind
bool y1R(cMatrix& Result, cMatrix& R);
bool ynR(cMatrix& Result, int iorder, cMatrix& R);

double lgamma(double, int*);		// log-gamma function
bool lgamma(cMatrix& Res, cMatrix& R);
double gamma(double x);
bool gamma(cMatrix& Res, cMatrix& R);

//
// linear algebra
//

double FrobeniusNormR(cMatrix&);
double FrobeniusNorm(cMatrix&);
double InfinityNormR(cMatrix&,bool);
double InfinityNorm(cMatrix&,bool);
double OneNormR(cMatrix&);
double OneNorm(cMatrix&);
double PNormR(cMatrix&, double);
double PNorm(cMatrix&, double);
bool VectorNorm(cMatrix& Result, cMatrix& v, double P);
bool MatrixNorm(cMatrix& Result, cMatrix& v, double P);
// interface fcns
bool Matrix2Norm(cMatrix& Result, cMatrix& v);
bool MatrixPNorm(cMatrix& Result, cMatrix& v, cMatrix& p);

//double InnerProduct( const double *pv1, const double *pv2, int len );
//complex<double> InnerProduct( const double *pr, const double *pr2, const double *pi2, int len );
//complex<double> InnerProduct( const double *pr1, const double *pi1, const double *pr2, const double *pi2, int len );
double InnerProduct( vector<double>::iterator v1, vector<double>::iterator v2, int len );
complex<double> InnerProduct( vector<double>::iterator r, vector<double>::iterator r2, vector<double>::iterator i2, int len );
complex<double> InnerProduct( vector<double>::iterator r1, vector<double>::iterator i1, vector<double>::iterator r2, vector<double>::iterator i2, int len );
bool OuterProductR( cMatrix *Result, cMatrix *a, cMatrix *b );
bool OuterProductC( cMatrix *Result, cMatrix *a, cMatrix *b );

int LUDecompositionR(cMatrix& a, cMatrix& indx, double& d);
int LUDecompositionC(cMatrix& a, cMatrix& indx, double& d);
bool LUDecomposition(cMatrix& LU, cMatrix& A);
bool LUDecomposition(cMatrix& L, cMatrix& U, cMatrix& A);
bool UpperTri(cMatrix& U, cMatrix& A);
bool LowerTri(cMatrix& L, cMatrix& idx, cMatrix& A);
void swapRows(cMatrix& A, int i0, int i1);

void LUBackSubstitutionR(cMatrix& a, cMatrix& indx, cMatrix& b);
void LUBackSubstitutionC(cMatrix& a, cMatrix& indx, cMatrix& b);

int LUMatrixInvertR(cMatrix& A, cMatrix& AInvrs, double& Determinant);
int LUMatrixInvertC(cMatrix& A, cMatrix& AInvrs, double& Determinant);
bool LUMatrixInvert(cMatrix& Result, cMatrix& A);
bool MatrixInvert(cMatrix& Result, cMatrix& v);

int LUSolveR(cMatrix& A, cMatrix& X, cMatrix& B);
int LUSolveC(cMatrix& A, cMatrix& X, cMatrix& B);

int LUDeterminantR(cMatrix& A, double& Determinant);
int LUDeterminantC(cMatrix& A, complex<double>& Determinant);

int GetHouseR(cMatrix& v, cMatrix& x);
int GetHouseC(cMatrix& v, cMatrix& x);
int HouseMatR(cMatrix& H, cMatrix& v);
int HouseMatC(cMatrix& H, cMatrix& v);
int QRDecompositionR(cMatrix& Q, cMatrix& R, cMatrix& A);
int QRDecompositionC(cMatrix& Q, cMatrix& R, cMatrix& A);

bool RConv2R(cMatrix& res, cMatrix& A, cMatrix& B);
bool RConv2C(cMatrix& res, cMatrix& A, cMatrix& B);
bool CConv2R(cMatrix& res, cMatrix& A, cMatrix& B);
bool CConv2C(cMatrix& res, cMatrix& A, cMatrix& B);
bool Conv2(cMatrix& res, cMatrix& A, cMatrix& B);

// Armadillo functions
//
bool arma_qr_R(cMatrix& Q, cMatrix& R, cMatrix& X);
bool arma_qr_C(cMatrix& Q, cMatrix& R, cMatrix& X);
bool arma_lu_R(cMatrix& L, cMatrix& U, cMatrix& X);
bool arma_lu_C(cMatrix& L, cMatrix& U, cMatrix& X);
bool arma_lu_R(cMatrix& L, cMatrix& U, cMatrix& P, cMatrix& X);
bool arma_lu_C(cMatrix& L, cMatrix& U, cMatrix& P, cMatrix& X);
bool arma_chol_R(cMatrix& res, cMatrix& X);
bool arma_chol_C(cMatrix& res, cMatrix& X);
//
bool arma_eig_R(cMatrix& res, cMatrix& X);
bool arma_eig_C(cMatrix& res, cMatrix& X);
bool arma_eigr_R(cMatrix& res, cMatrix& X);
bool arma_eigr_C(cMatrix& res, cMatrix& X);
bool arma_svd_R(cMatrix& U, cMatrix& S, cMatrix& V, cMatrix& X);
bool arma_svd_C(cMatrix& U, cMatrix& S, cMatrix& V, cMatrix& X);
bool arma_rank_R(cMatrix& res, cMatrix& X);
bool arma_rank_C(cMatrix& res, cMatrix& X);
bool arma_rank_R(cMatrix& res, cMatrix& X, cMatrix& tol);
bool arma_rank_C(cMatrix& res, cMatrix& X, cMatrix& tol);
bool arma_eps_R(cMatrix& res, cMatrix& X);
bool arma_solve_R(cMatrix& X, cMatrix& A, cMatrix& B);
bool arma_solve_C(cMatrix& X, cMatrix& A, cMatrix& B);
bool arma_syl_R(cMatrix& res, cMatrix& A, cMatrix& B, cMatrix& C);
bool arma_syl_C(cMatrix& res, cMatrix& A, cMatrix& B, cMatrix& C);
//
bool arma_inv_R(cMatrix& res, cMatrix& X);
bool arma_inv_C(cMatrix& res, cMatrix& X);
bool arma_pinv_R(cMatrix& res, cMatrix& X);
bool arma_pinv_C(cMatrix& res, cMatrix& X);
bool arma_det_R(cMatrix& res, cMatrix& X);
bool arma_det_C(cMatrix& res, cMatrix& X);
bool arma_logdet_R(cMatrix& res, cMatrix& X);
bool arma_logdet_C(cMatrix& res, cMatrix& X);
//
bool arma_trace_R(cMatrix& res, cMatrix& X);
bool arma_trace_C(cMatrix& res, cMatrix& X);



// Armadillo helpers
//void Arma2MatR(cMatrix& M, mat& am);
//void Arma2MatC(cMatrix& M, Mat<complex<double>>& am);
//void Mat2ArmaC(Mat<complex<double>>& am, cMatrix C);
//void ArmaVec2MatC(cMatrix& M, cx_vec& vec);
//void Mat2ArmaVecR(cx_vec& vec, cMatrix& R);
//void Mat2ArmaVecC(cx_vec& vec, cMatrix& C);

//
// statistics
//

// stat helpers
bool IsPow2(int);
bool IsEven(int);
bool IsOdd(int);
double Fix(double);
bool Fix(cMatrix& Result, cMatrix& A);
bool Round(cMatrix& Result, cMatrix& A);
double Floor(double val);
bool Floor(cMatrix& Result, cMatrix& A);
bool Ceil(cMatrix& Result, cMatrix& A);
bool Phase(cMatrix& Result, cMatrix& A);
//
bool Mod(cMatrix& Result, cMatrix& x, cMatrix& y);
bool Mod(cMatrix& Result, double x, cMatrix& y);
bool Mod(cMatrix& Result, cMatrix& x, double y);

// random number generation
void Seed(unsigned int);
void TimeSeed(void);
void MersenneSeed(unsigned int);
void MersenneSeed(double);

double DRand(void);
double Rand32(void);
complex<double> CRand(void);
bool MatRandR(cMatrix& Result, int r, int c);
bool MatRandC(cMatrix& Result, int r, int c);
//

// very basics
int Max(int, int);
double Max(double, double);
bool MaxR(cMatrix& Result, cMatrix& mat);
bool RMaxR(cMatrix& Result, cMatrix& mat1, cMatrix& mat2);
bool RMatMaxR(cMatrix& Result, cMatrix& mat1, cMatrix& mat2);
bool RMaxR(cMatrix& Result, cMatrix& mat1, cMatrix& mat2);
bool MaxNumberR(cMatrix& Result, cMatrix& mat);
bool MaxVectR(cMatrix& Result, cMatrix& mat);
bool MaxIndexR(cMatrix& Result, cMatrix& ResIdx, cMatrix& mat);
bool MaxVectIndexR(cMatrix& Result, cMatrix& ResIdx, cMatrix& mat);
bool MaxIndex(cMatrix& Result, cMatrix& ResIdx, cMatrix& mat); // interface fcn

int Min(int, int);
double Min(double, double);
bool MinR(cMatrix& Result, cMatrix& mat);
bool RMatMinR(cMatrix& Result, cMatrix& mat1, cMatrix& mat2);
bool RMinR(cMatrix& Result, cMatrix& mat1, cMatrix& mat2);
bool MinNumberR(cMatrix& Result, cMatrix& mat);
bool MinVectR(cMatrix& Result, cMatrix& mat);
bool MinIndexR(cMatrix& Result, cMatrix& ResIdx, cMatrix& mat);
bool MinVectIndexR(cMatrix& Result, cMatrix& ResIdx, cMatrix& mat);
bool MinIndex(cMatrix& Result, cMatrix& ResIdx, cMatrix& mat); // interface fcn

bool AbsR(cMatrix& Result, cMatrix& R);
bool Abs(cMatrix& Result, cMatrix& A);
int Sign(double);
int Round(double);

bool NandR( double val, double *pN, double *pr, int iGranularity );
complex<double> Polar2Rect(const complex<double>&);
complex<double> Rect2Polar(const complex<double>&);

double Factorial(int n);
bool FactorialR(cMatrix& Result, cMatrix& R);
bool Factorial(cMatrix& Result, cMatrix& A);

// basic stats
bool MedianR(double& Result, cMatrix& R);				// for vectors
bool MedianC(complex<double>& Result, cMatrix& C);		// for vectors
bool MedianR(cMatrix& Result, cMatrix& R);
bool MedianC(cMatrix& Result, cMatrix& C);
bool Median(cMatrix& Result, cMatrix& A);
bool MedianR(cMatrix& Result, cMatrix& R, int idim);
bool MedianC(cMatrix& Result, cMatrix& C, int idim);
bool Median(cMatrix& Result, cMatrix& A, cMatrix& dim);

bool MeanR(cMatrix& Result, cMatrix& R);
bool MeanC(cMatrix& Result, cMatrix& C);
bool Mean(cMatrix& Result, cMatrix& A);
bool MeanR(cMatrix& Result, cMatrix& v, int idim);
bool MeanC(cMatrix& Result, cMatrix& v, int idim);
bool Mean(cMatrix& Result, cMatrix& v, cMatrix& dim);

bool VarR(cMatrix& Result, cMatrix& R);
bool VarC(cMatrix& Result, cMatrix& C);
bool Var(cMatrix& Result, cMatrix& A);
bool VarR(cMatrix& Result, cMatrix& R, int idim);
bool VarC(cMatrix& Result, cMatrix& C, int idim);
bool Var(cMatrix& Result, cMatrix& A, cMatrix& dim);

bool RCovR(cMatrix& Result, cMatrix& R1, cMatrix& R2);
bool RCovC(cMatrix& Result, cMatrix& R, cMatrix& C);
bool CCovR(cMatrix& Result, cMatrix& C, cMatrix& R);
bool CCovC(cMatrix& Result, cMatrix& C1, cMatrix& C2);
bool Cov(cMatrix& Result, cMatrix& A1, cMatrix& A2);
bool Cov(cMatrix& Result, cMatrix& A);

bool StdR(cMatrix& Result, cMatrix& R);
bool StdC(cMatrix& Result, cMatrix& C);
bool Std(cMatrix& Result, cMatrix& A);
bool StdR(cMatrix& Result, cMatrix& R, int idim);
bool StdC(cMatrix& Result, cMatrix& C, int idim);
bool Std(cMatrix& Result, cMatrix& A, cMatrix& dim);

bool Hist(cMatrix& Result, cMatrix& Data);
bool Hist(cMatrix& Result, cMatrix& Data, int nCenters);
bool Hist(cMatrix& Result, cMatrix& Data, cMatrix& Edges);
bool Hist_2o(cMatrix& Result, cMatrix& centers, cMatrix& Data);
bool Hist_2o(cMatrix& Result, cMatrix& centers, cMatrix& Data, int nBins);
bool Histe(cMatrix& Result, cMatrix& v, cMatrix& edges);
bool Histn(cMatrix& Result, cMatrix& v, cMatrix& Ncenters);
bool Histn_2o(cMatrix& Result, cMatrix& centers, cMatrix& v, cMatrix& Ncenters);

bool Histc(cMatrix& Result, cMatrix& Data, cMatrix& Centers);
bool GetHistEdges(cMatrix& Edges, cMatrix& Centers, cMatrix& Data, int N_edges);
bool GetHistCenters(cMatrix& Centers, cMatrix& Edges);
bool GetHistEdges(cMatrix& Edges, cMatrix& Centers);
bool GetHistEdges(cMatrix& Edges, cMatrix& Centers, double data_min, double data_max);

// fourier analysis
bool Radix2FFTR(cMatrix *y, cMatrix *x, int flag);
bool Radix2FFTC(cMatrix *y, cMatrix *x, int flag);
bool Radix2FFT(cMatrix& y, cMatrix& x, int iflag);

bool FFTZR(cMatrix& y, cMatrix& x, int iflag);
bool FFTZC(cMatrix& y, cMatrix& x, int iflag);
bool FFTZ(cMatrix& y, cMatrix& x, int iflag);

bool StraightDFTR(cMatrix *y, cMatrix *x, int flag);
bool StraightDFTC(cMatrix *y, cMatrix *x, int flag);
bool StraightDFT(cMatrix& y, cMatrix& x, int iflag);

bool StraightDFT2R(cMatrix *y, cMatrix *x, int flag);
bool StraightDFT2C(cMatrix *y, cMatrix *x, int flag);
bool StraightDFT2(cMatrix& y, cMatrix& x, int iflag);

bool DFTR(cMatrix *y, cMatrix *x, int iFlag);
bool DFTC(cMatrix *y, cMatrix *x, int iFlag);
bool DFT(cMatrix& y, cMatrix& x, int iflag);

bool DFT2R(cMatrix *y, cMatrix *x, int iFlag);
bool DFT2C(cMatrix *y, cMatrix *x, int iFlag);
bool DFT2(cMatrix& y, cMatrix& x, int iflag);

bool FFTShiftR(cMatrix *y, cMatrix *x);
bool FFTShiftC(cMatrix *y, cMatrix *x);
bool FFTShift(cMatrix& y, cMatrix& x);


// interface fcns
//
bool is_empty(cMatrix& res, cMatrix& M);
bool is_real(cMatrix& res, cMatrix& M);
bool is_vector(cMatrix& res, cMatrix& M);
bool is_scalar(cMatrix& res, cMatrix& M);
bool is_square(cMatrix& res, cMatrix& M);
bool is_finite(cMatrix& res, cMatrix& M);
bool is_nan(cMatrix& res, cMatrix& M);

bool Add(cMatrix& res, cMatrix& M1, cMatrix& M2);
// called from cCube slice & dice:
bool Add(cMatrix& res, cMatrix& M1, double d);
bool Add(cMatrix& res, cMatrix& M1, complex<double>& cv);
bool Sub(cMatrix& res, cMatrix& M1, cMatrix& M2);
bool Sub(cMatrix& res, cMatrix& M1, double d);
bool Sub(cMatrix& res, double d, cMatrix& M2);
bool Sub(cMatrix& res, cMatrix& M1, complex<double>& cv);
bool Sub(cMatrix& res, complex<double>& cv, cMatrix& M2);
//
bool Mult(cMatrix& res, cMatrix& M1, cMatrix& M2);
bool Mult(cMatrix& res, cMatrix& M1, double d);
//
bool sin(cMatrix& res, cMatrix& m);
bool cos(cMatrix& res, cMatrix& m);
bool tan(cMatrix& res, cMatrix& m);
bool asin(cMatrix& res, cMatrix& m);
bool acos(cMatrix& res, cMatrix& m);
bool atan(cMatrix& res, cMatrix& m);
bool csc(cMatrix& res, cMatrix& m);
bool sec(cMatrix& res, cMatrix& m);
bool cot(cMatrix& res, cMatrix& m);
bool acsc(cMatrix& res, cMatrix& m);
bool asec(cMatrix& res, cMatrix& m);
bool acot(cMatrix& res, cMatrix& m);
//
bool sinh(cMatrix& res, cMatrix& m);
bool cosh(cMatrix& res, cMatrix& m);
bool tanh(cMatrix& res, cMatrix& m);
bool asinh(cMatrix& res, cMatrix& m);
bool acosh(cMatrix& res, cMatrix& m);
bool atanh(cMatrix& res, cMatrix& m);
bool csch(cMatrix& res, cMatrix& m);
bool sech(cMatrix& res, cMatrix& m);
bool coth(cMatrix& res, cMatrix& m);
bool acsch(cMatrix& res, cMatrix& m);
bool asech(cMatrix& res, cMatrix& m);
bool acoth(cMatrix& res, cMatrix& m);
//
bool log(cMatrix& res, cMatrix& m);
bool log10(cMatrix& res, cMatrix& m);
bool log2(cMatrix& res, cMatrix& m);
bool sqrt(cMatrix& res, cMatrix& m);
bool exp(cMatrix& res, cMatrix& m);
//
bool erf(cMatrix& res, cMatrix& m);
bool erfc(cMatrix& res, cMatrix& m);
bool besselj0(cMatrix& res, cMatrix& m);
bool besselj1(cMatrix& res, cMatrix& m);
bool besseljn(cMatrix& res, cMatrix& order, cMatrix& m);
bool bessely0(cMatrix& res, cMatrix& m);
bool bessely1(cMatrix& res, cMatrix& m);
bool besselyn(cMatrix& res, cMatrix& order, cMatrix& m);
//
bool Min(cMatrix& res, cMatrix& m);
bool Min(cMatrix& res, cMatrix& m1, cMatrix& m2);
bool MinIndex(cMatrix& res, cMatrix& ResIdx, cMatrix& v);
bool Max(cMatrix& res, cMatrix& v);
bool Max(cMatrix& res, cMatrix& v1, cMatrix& v2);
bool MaxIndex(cMatrix& res, cMatrix& ResIdx, cMatrix& v);
//
bool Sum(cMatrix& res, cMatrix& v);
bool Sum(cMatrix& res, cMatrix& v, int idim);
bool Sum(cMatrix& res, cMatrix& v, cMatrix& vDim);
bool CumSum(cMatrix& res, cMatrix& v);
bool CumSum(cMatrix& res, cMatrix& v, int idim);
bool CumSum(cMatrix& res, cMatrix& v, cMatrix& vDim);
bool StraightSum(cMatrix& res, cMatrix& v);
bool Prod(cMatrix& res, cMatrix& v);
bool Prod(cMatrix& res, cMatrix& v, int idim);
bool CumProd(cMatrix& res, cMatrix& v);
bool CumProd(cMatrix& res, cMatrix& v, int idim);
bool CumProd(cMatrix& res, cMatrix& v, cMatrix& vDim);
//
bool Fft(cMatrix& Result, cMatrix& v);
bool Ifft(cMatrix& Result, cMatrix& v);
bool Fftz(cMatrix& Result, cMatrix& v);
bool Ifftz(cMatrix& Result, cMatrix& v);
bool Dft(cMatrix& Result, cMatrix& v);
bool Idft(cMatrix& Result, cMatrix& v);
bool Fft2(cMatrix& Result, cMatrix& v);
bool Ifft2(cMatrix& Result, cMatrix& v);
bool Dft2(cMatrix& Result, cMatrix& v);
bool Idft2(cMatrix& Result, cMatrix& v);
bool Shfft(cMatrix& Result, cMatrix& v);
//
vector<int> random_shuffle(int n);	// shuffle the set (0,...,n-1)
bool ShuffleRowsR(cMatrix& res, cMatrix& X);
bool ShuffleRowsC(cMatrix& res, cMatrix& X);
bool ShuffleColsR(cMatrix& res, cMatrix& X);
bool ShuffleColsC(cMatrix& res, cMatrix& X);
bool ShuffleRows(cMatrix& res, cMatrix& X);
bool ShuffleCols(cMatrix& res, cMatrix& X);
//
bool RandSeed(cMatrix& seed);
bool Rand(cMatrix& Result, cMatrix& vRows, cMatrix& vCols);
bool Rand(cMatrix& Result, cMatrix& vDim);
bool Randn(cMatrix& Result, cMatrix& vRows, cMatrix& vCols);
bool Randn(cMatrix& Result, cMatrix& vDim);
//
bool arma_qr(cMatrix& Q, cMatrix& R, cMatrix& X);
bool arma_lu(cMatrix& L, cMatrix& U, cMatrix& X);
bool arma_lu(cMatrix& L, cMatrix& U, cMatrix& P, cMatrix& X);
bool arma_chol(cMatrix& res, cMatrix& X);
//
bool arma_eig(cMatrix& res, cMatrix& X);
bool arma_eig(cMatrix& val, cMatrix& vec, cMatrix& X);
bool arma_eigr(cMatrix& res, cMatrix& X);
bool arma_eigr(cMatrix& val, cMatrix& vec, cMatrix& X);
bool arma_svd(cMatrix& U, cMatrix& S, cMatrix& V, cMatrix& X);
bool arma_svd(cMatrix& U, cMatrix& S, cMatrix& X);
bool arma_svd(cMatrix& S, cMatrix& X);
bool arma_rank(cMatrix& res, cMatrix& X);
bool arma_rank(cMatrix& res, cMatrix& X, cMatrix& tol);
bool arma_solve(cMatrix& X, cMatrix& A, cMatrix& B);
bool arma_syl(cMatrix& res, cMatrix& A, cMatrix& B, cMatrix& C);
bool arma_eps(cMatrix& res, cMatrix& X);
//
bool arma_inv(cMatrix& res, cMatrix& X);
bool arma_pinv(cMatrix& res, cMatrix& X);
bool arma_det(cMatrix& res, cMatrix& X);
bool arma_logdet(cMatrix& res, cMatrix& X);
//
bool arma_trace(cMatrix& res, cMatrix& X);
bool arma_conv(cMatrix& res, cMatrix& X, cMatrix& Y);
//
bool arma_shuffle_rows(cMatrix& res, cMatrix& X);
bool arma_shuffle_cols(cMatrix& res, cMatrix& X);
//
bool get_inf(cMatrix& res);
bool get_pi(cMatrix& res);
bool get_i(cMatrix& res);
bool get_e(cMatrix& res);
bool get_light_speed(cMatrix& res);
//bool get_sound_speed(cMatrix& res);	// 340.29 m/s ?
bool get_boltzmann(cMatrix& res);

bool get_avagadro(cMatrix& res);	// Avagadro's number
bool get_goldenratio(cMatrix& res);	// golden ratio
bool get_eps(cMatrix& res);			// the difference between 1 and the least value greater than 1 that is representable 
bool get_atomicmass(cMatrix& res);	// atomic mass constant (in kg)
bool get_Z0(cMatrix& res);			// intrinsic impedance of vacuum (ohms)
bool get_G0(cMatrix& res);			// conductance quantum (in siemens)
bool get_G0i(cMatrix& res);			// conductance quantum inverse (in ohms)
//bool get_KE(cMatrix& res);			// Coulomb's constant (in meters per farad)
bool get_eps0(cMatrix& res);		// electric constant (in farads per meter)
bool get_me(cMatrix& res);			// electron mass
bool get_J2eV(cMatrix& res);		// electron volt (in joules)
bool get_ec(cMatrix& res);			// elementary charge (in coulombs)
bool get_ev(cMatrix& res);			// electron volt (in joules)
bool get_F(cMatrix& res);			// Faraday constant (in coulombs)
bool get_alpha(cMatrix& res);		// fine-structure constant
bool get_alpha_inv(cMatrix& res);	// inverse fine-structure constant
bool get_kj(cMatrix& res);			// Josephson constant
bool get_mu_0(cMatrix& res);		// magnetic constant (in henries per meter)
bool get_phi_0(cMatrix& res);		// magnetic flux quantum (in webers)
bool get_R(cMatrix& res);			// molar gas constant (in joules per mole kelvin)
bool get_G(cMatrix& res);			// Newtonian constant of gravitation (in newton square meters per kilogram squared
bool get_h(cMatrix& res);			// Planck constant (in joule seconds)
bool get_hbar(cMatrix& res);		// Planck constant over 2 pi, aka reduced Planck constant (in joule seconds)
bool get_mp(cMatrix& res);			// proton mass (in kg)
bool get_Rinf(cMatrix& res);		// Rydberg constant (in reciprocal meters)
bool get_sigma(cMatrix& res);		// Stefan-Boltzmann constant
bool get_Rk(cMatrix& res);			// von Klitzing constant (in ohms)
bool get_wien_b(cMatrix& res);		// Wien wavelength displacement law constant


// MLpack functions
//
bool knn_search(cMatrix& resN, cMatrix& resD, cMatrix& table, cMatrix& k);	// self-nn
bool knn_search(cMatrix& resN, cMatrix& resD, cMatrix& table, cMatrix& query, cMatrix& k);
//
bool kfn_search(cMatrix& resN, cMatrix& resD, cMatrix& table, cMatrix& k);
bool kfn_search(cMatrix& resN, cMatrix& resD, cMatrix& table, cMatrix& query, cMatrix& k);
//
bool krn_search(cMatrix& resN, cMatrix& resD, cMatrix& table, cMatrix& k, cMatrix& rmin, cMatrix& rmax);
bool krn_search(cMatrix& resN, cMatrix& resD, cMatrix& table, cMatrix& query, cMatrix& k, cMatrix& rmin, cMatrix& rmax);
