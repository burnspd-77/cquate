/* matrixLib.cpp
Various math functions defined for the
matrix classes defined in "matrix.h"
*/

#include "matrix.h"
#include "my_errors.h"
#include "float.h"
#include <stdio.h>
#include "time.h"
#include <stdlib.h>
#include <limits>
#include <string.h>
#include <assert.h>

//
// File I/O
//
bool WriteData(cMatrix& v, FILE *pf)
{
	int nr = v.GetRows();
	int nc = v.GetColumns();
	bool bReal = v.IsReal();
	if (fwrite( &nr, sizeof(int), 1, pf ) != 1) return true;		// rows
	if (fwrite( &nc, sizeof(int), 1, pf ) != 1) return true;		// cols
	if (fwrite( &bReal, sizeof(bool), 1, pf ) != 1) return true;	// is real?
	if (fwrite( v.GetR(), sizeof(double), nr*nc, pf ) != nr*nc) return true;
	if (!bReal) {
		if (fwrite( v.GetI(), sizeof(double), nr*nc, pf ) != nr*nc) return true;
	}

	return false;
}

bool ReadData(cMatrix& m, FILE *pf)
{
	int nr, nc;
	bool bReal;

	if (fread( &nr, sizeof(int), 1, pf ) != 1) return true;
	if (fread( &nc, sizeof(int), 1, pf ) != 1) return true;
	if (fread( &bReal, sizeof(bool), 1, pf ) != 1) return true;

	if (bReal) {
		if ( m.Create(nr,nc) ) { addError("Out of memory!"); return true; }				// out of memory
		if (fread( m.GetR(), sizeof(double), nr*nc, pf ) != nr*nc) return true;
	}
	else {
		if ( m.CreateComplex(nr,nc) ) { addError("Out of memory!"); return true; }		// out of memory
		if (fread( m.GetR(), sizeof(double), nr*nc, pf ) != nr*nc) return true;
		if (fread( m.GetI(), sizeof(double), nr*nc, pf ) != nr*nc) return true;
	}

	return false;
}

//
// Miscellaneous functs
//
void MemAllocErrorMessage()
{
	printf("Memory allocation failure.");
}

bool SameSize(const cMatrix& R1, const cMatrix& R2)
{
	if (R1.IsVector()) {
		if (R2.IsVector()) return R1.GetSize()==R2.GetSize();
		else return false;
	}

	return (R1.GetRows()==R2.GetRows() && R1.GetColumns()==R2.GetColumns());
}

bool AnyR(cMatrix& R)
{
	const double *pr = R.GetR();
	for (int i=0; i<R.GetSize(); ++i)
	{
		if (pr[i] != 0) return true;
	}
	return false;
}

bool checkRealDomain(double vL, int l_type, double vR, int r_type, cMatrix& M)
{
	// return false if any real value of M lies outside interval
	int i, N(M.GetSize());
bool bOK(true);
	double *pr = M.GetR();
	if (pr==NULL) return true;

	if (l_type==LIMIT_SQUARE && r_type==LIMIT_SQUARE) {
		for (i=0; i<N; ++i) {
			if (pr[i]<vL || pr[i]>vR) return false; }
	}
	else if (l_type==LIMIT_SQUARE && r_type==LIMIT_ROUND) {
		for (i=0; i<N; ++i) {
			if (pr[i]<vL || pr[i]>=vR) return false; }
	}
	else if (l_type==LIMIT_ROUND && r_type==LIMIT_SQUARE) {
		for (i=0; i<N; ++i) {
			if (pr[i]<=vL || pr[i]>vR) return false; }
	}
	else { //(l_type==LIMIT_ROUND && r_type==LIMIT_ROUND) {
		for (i=0; i<N; ++i) {
			if (pr[i]<=vL || pr[i]>=vR) return false; }
	}
	return true;
}

//
// shuffling rows and columns
//
vector<int> random_shuffle(int n)
{	// shuffle the set (0,...,n-1)
	sortElem item;
	vector<sortElem> set;

	int i;
	for (i=0; i<n; ++i) {
		item.ix = i;
		item.dv = Rand32();
		set.push_back( item );
	}

	std::sort( set.begin(), set.end(), elemLessThan );

	vector<int> shuf;
	for (i=0; i<n; ++i) {
		shuf.push_back( set[i].ix );
	}

	return shuf;
}

bool ShuffleRows(cMatrix& res, cMatrix& X)
{
	res.Delete();
	int n = X.GetRows();
	if (n==0)
		return false;

	if (X.IsReal()) {
		if (res.Create(X.GetRows(),X.GetColumns()))
			return true;
	}
	else {
		if (res.CreateComplex(X.GetRows(),X.GetColumns()))
			return true;
	}

	vector<int> ri = random_shuffle(n);
	cMatrix row;

	for (int r=0; r<n; ++r)
	{
		if (X.GetRow(&row, ri[r])) {
			addError("internal error: unexpected indexing condition");
			return true;
		}
		res.Set(r,0,row);
	}

	return false;
}

bool ShuffleCols(cMatrix& res, cMatrix& X)
{
	res.Delete();
	int n = X.GetColumns();
	if (n==0)
		return false;

	if (X.IsReal()) {
		if (res.Create(X.GetRows(),X.GetColumns()))
			return true;
	}
	else {
		if (res.CreateComplex(X.GetRows(),X.GetColumns()))
			return true;
	}

	vector<int> ri = random_shuffle(n);
	cMatrix col;

	for (int c=0; c<n; ++c)
	{
		if (X.GetColumn(&col, ri[c])) {
			addError("internal error: unexpected indexing condition");
			return true;
		}
		res.Set(0,c,col);
	}

	return false;
}

//
// find
//
bool FindR(cMatrix& Result, cMatrix& R)
{
	vector<double> v;

	int n = R.GetSize();
	const double *pr = R.GetSafeR();

	for (int i=0; i<n; ++i)
	{
		if (pr[i] != 0.) { v.push_back( double(i) ); }
	}

	if (R.IsVector()) {
		if (R.GetRows()==1) { if (Result.CreateRow( v )) return true; }
		else { if (Result.CreateColumn( v )) return true; }
	}
	else {		// matrix - put all result in a vector
		if (Result.CreateColumn( v )) return true;
	}

	return false;
}

bool FindC(cMatrix& Result, cMatrix& C)
{
	vector<double> v;

	int n = C.GetSize();
	const double *pr = C.GetSafeR();
	const double *pi = C.GetSafeI();

	for (int i=0; i<n; ++i)
	{
		if ( (pr[i]!=0.) || (pi[i]!=0.) ) { v.push_back( double(i) ); }
	}

	if (C.IsVector()) {
		if (C.GetRows()==1) { if (Result.CreateRow( v )) return true; }
		else { if (Result.CreateColumn( v )) return true; }
	}
	else {		// matrix - put all result in a vector
		if (Result.CreateColumn( v )) return true;
	}

	return false;
}

bool Find(cMatrix& Result, cMatrix& A)
{
	if (A.IsReal()) return FindR(Result, A);
	else return FindC(Result, A);
}

//
// resize
//
bool Resize(cMatrix& Result, cMatrix& v, cMatrix& vRows, cMatrix& vCols)
{
	if (!vRows.IsScalar() || !vCols.IsScalar()) { addError("illegal argument"); return true; }

	double drows = vRows.Get0R();
	double dcols = vCols.Get0R();

	int irows = int(drows);
	int icols = int(dcols);

	Result = v;
	if (Result.IsEmpty()) { addError("illegal argument"); return true; }	// memory error check

	if ( Result.Reshape(irows,icols) ) { addError("size mismatch"); Result.Delete(); return true; }	// size mismatch
	return false;	// success
}

//
// rotate
//
bool Rotate(cMatrix& Result, cMatrix& v, cMatrix& vRot)
{
	if (!vRot.IsScalar()) { addError("illegal argument"); return true; }

	double dRot = vRot.Get0R();

	return v.Rot90( Result, dRot );
}

bool RotateCw(cMatrix& Result, cMatrix& v, cMatrix& vRot)
{
	if (!vRot.IsScalar()) { addError("illegal argument"); return true; }

	double dRot = vRot.Get0R();

	return v.Rot90( Result, -dRot );
}

//
// Sort
//
bool Sort(cMatrix& Result, cMatrix& v)
{
	return v.Sort(Result);
}

bool SortIndex(cMatrix& Result, cMatrix& ResIndex, cMatrix& v)
{
	return v.SortIndex(Result, ResIndex);
}


//
// Unique
//
bool Unique(cMatrix& Result, cMatrix& X)
{	// imaginary part is ignored.

	//if (X.IsReal()) {
		Result.mr = X.mr;
		std::sort( Result.mr.begin(), Result.mr.end() );
		vector<double>::iterator it = std::unique( Result.mr.begin(), Result.mr.end() );
		Result.mr.resize( std::distance(Result.mr.begin(), it) );
		Result.nr = Result.mr.size();
		Result.nc = 1;
	//}
	//else {	// complex
	//	vector<complex<double>> vc;
	//	X.GetVectC( vc );
	//	std::sort( vc.begin(), vc.end() );
	//	vector<complex<double>>::iterator cit = std::unique( vc.begin(), vc.end() );
	//	vc.resize( std::distance(vc.begin(), cit) );
	//	Result.SetVectC( vc );
	//}
	return false;
}

//
// real/imag/conj/trans/herm
//
bool Conj(cMatrix& Result, cMatrix& A)
{
	if (A.IsReal()) return (Result = A);
	if ( Result.CreateComplex(A.GetRows(),A.GetColumns()) ) return true;

	//double *pr = Result.GetR();
	//double *pi = Result.GetI();
	//const double *par = A.GetSafeR();
	//const double *pai = A.GetSafeI();
	int n = A.GetSize();

	Result.FillR(0, A.GetR(), Result.GetSize());

	// direct copy of real part
	//if ( memcpy( pr, par, A.GetSize()*sizeof(double) )==NULL ) { addError("out of memory"); return true; }
	// flip sign of imag part
	for (int i=0; i<n; ++i) { Result.mi[i] = -A.mi[i]; }

	return false;
}

bool Real(cMatrix& Result, cMatrix& A)
{
	if (A.IsReal()) return (Result = A);
	if ( Result.Create(A.GetRows(),A.GetColumns()) ) return true;

	//double *pr = Result.GetR();
	//const double *par = A.GetSafeR();
	int n = A.GetSize();

	Result.FillR(0, A.GetR(), Result.GetSize());

	//if ( memcpy( pr, par, A.GetSize()*sizeof(double) )==NULL ) { addError("out of memory"); return true; }
	return false;
}

bool Imag(cMatrix& Result, cMatrix& A)
{
	if (A.IsReal()) return ZerosR(Result,A.GetRows(),A.GetColumns());

	if ( Result.Create(A.GetRows(),A.GetColumns()) ) return true;

	//double *pr = Result.GetR();
	//const double *pai = A.GetSafeI();
	int n = A.GetSize();

	Result.FillR(0, A.GetI(), Result.GetSize());

	//if ( memcpy( pr, pai, A.GetSize()*sizeof(double) )==NULL ) { addError("out of memory"); return true; }
	return false;
}

bool ZerosR(cMatrix& Result, int nrows, int ncols)
{
	if ( Result.Create(nrows,ncols) ) return true;
	double *pr = Result.GetR();
	if ( memset( pr, 0, nrows*ncols*sizeof(double) )==NULL ) { addError("out of memory"); return true; }
	return false;
}

bool ZerosC(cMatrix& Result, int nrows, int ncols)
{
	if ( Result.CreateComplex(nrows,ncols) ) return true;
	double *pr = Result.GetR();
	double *pi = Result.GetI();
	if ( memset( pr, 0, nrows*ncols*sizeof(double) )==NULL ) { addError("out of memory"); return true; }
	if ( memset( pi, 0, nrows*ncols*sizeof(double) )==NULL ) { addError("out of memory"); return true; }
	return false;
}

bool TransR(cMatrix& Result, cMatrix& R)
{	// transpose of a real matrix R
	if ( Result.Create(R.GetColumns(),R.GetRows()) ) return true;

	//double *pr = Result.GetR();
	//const double *prr = R.GetSafeR();
	int idx, i, j, nr = R.GetRows(), nc = R.GetColumns();

	for (i=0; i<nc; i++) {
		for (j=0; j<nr; j++) {
			idx = i+j*nc;
			assert( idx>=0 && idx<nr*nc );
			Result.mr[idx] = R.mr[j+i*nr];
		}
	}
	return false;
}

bool Trans(cMatrix& Result, cMatrix& A)
{
	if (A.IsReal()) return TransR(Result, A);

	if ( Result.CreateComplex(A.GetColumns(),A.GetRows()) ) return true;

	//double *pr = Result.GetR();
	//double *pi = Result.GetI();
	//const double *prr = A.GetSafeR();
	//const double *pri = A.GetSafeI();

	int idx, i, j, nr = A.GetRows(), nc = A.GetColumns();

	for (i=0; i<nc; i++) {
		for (j=0; j<nr; j++) {
			idx = i+j*nc;
			assert( idx>=0 && idx<nr*nc );
			Result.mr[idx] = A.mr[j+i*nr];
			Result.mi[idx] = A.mi[j+i*nr];
		}
	}
	return false;
}

bool Herm(cMatrix& Result, cMatrix& A)
{
	if (A.IsReal()) return TransR(Result,A);

	if ( Result.CreateComplex(A.GetColumns(),A.GetRows()) ) return true;

	int idx, i, j, nr = A.GetRows(), nc = A.GetColumns();

	for (i=0; i<nc; i++) {
		for (j=0; j<nr; j++) {
			idx = i+j*nc;
			assert( idx>=0 && idx<nr*nc );
			Result.mr[idx] = A.mr[j+i*nr];
			Result.mi[idx] = - A.mi[j+i*nr];
		}
	}
	return false;
}

bool Not(cMatrix& Result, cMatrix& A)
{
	if ( Result.Create(A.GetColumns(),A.GetRows()) ) return true;

	int idx, i, j, nr = A.GetRows(), nc = A.GetColumns();

	for (i=0; i<nc; i++) {
		for (j=0; j<nr; j++) {
			idx = i+j*nc;
			assert( idx>=0 && idx<nr*nc );
			Result.mr[idx] = (double) (A.mr[j+i*nr]==0.);
		}
	}
	return false;
}

bool VertCat(cMatrix& Result, double d1, double d2)
{
	if (Result.Create(2,1)) return true;
	//double *pr = Result.GetR();
	Result.mr[0] = d1;
	Result.mr[1] = d2;
	return false;
}

bool VertCatR(cMatrix& Result, double d1, cMatrix& R2)
{
	if (R2.GetColumns()!=1) { addError("incompatable matrix dimensions"); return true; }

	if (Result.Create(R2.GetRows()+1,1)) return true;

	double *pr = Result.GetR();
	const double *prr = R2.GetSafeR();
	
	Result.mr[0] = d1;
	for (int i=0; i<R2.GetRows(); ++i)
	{
		Result.mr[i+1] = R2.mr[i];
	}
	//memcpy( &pr[1], prr, R2.GetRows()*sizeof(double) );
	return false;
}

bool VertCat(cMatrix& Result, double d1, cMatrix& A2)
{
	if (A2.IsReal()) return VertCatR(Result, d1, A2);
	if (A2.GetColumns()!=1) { addError("incompatable matrix dimensions"); return true; }

	if (Result.CreateComplex(A2.GetRows()+1,1)) return true;

	//double *pr = Result.GetR();
	//double *pi = Result.GetI();
	//const double *par = A2.GetSafeR();
	//const double *pai = A2.GetSafeI();
	
	Result.mr[0] = d1; Result.mi[0] = 0.;
	for (int i=0; i<A2.GetRows(); ++i)
	{
		Result.mr[i+1] = A2.mr[i];
		Result.mi[i+1] = A2.mi[i];
	}
	//memcpy( &pr[1], par, A2.GetRows()*sizeof(double) );
	//memcpy( &pi[1], pai, A2.GetRows()*sizeof(double) );
	return false;
}

bool VertCatR(cMatrix& Result, cMatrix& R1, double d2)
{
	if (R1.GetColumns()!=1) { addError("incompatable matrix dimensions"); return true; }

	int n = R1.GetRows()+1;
	if (Result.Create(n,1)) return true;

	double *pr = Result.GetR();
	const double *par = R1.GetSafeR();
	
	memcpy( pr, par, R1.GetRows()*sizeof(double) );
	pr[n-1] = d2;
	return false;
}

bool VertCat(cMatrix& Result, cMatrix& A1, double d2)
{
	if (A1.IsReal()) return VertCatR(Result, A1, d2);
	if (A1.GetColumns()!=1) { addError("incompatable matrix dimensions"); return true; }

	int n = A1.GetRows()+1;
	if (Result.CreateComplex(n,1)) return true;

	double *pr = Result.GetR();
	double *pi = Result.GetI();
	const double *par = A1.GetSafeR();
	const double *pai = A1.GetSafeI();
	
	memcpy( pr, par, A1.GetRows()*sizeof(double) );
	memcpy( pi, pai, A1.GetRows()*sizeof(double) );
	pr[n-1] = d2; pi[n-1] = 0.;
	return false;
}

bool VertCatR(cMatrix& Result, cMatrix& A1, cMatrix& R2)
{
	Result = A1;
	return Result.AugmentRowsR(&R2);
}

bool RVertCat(cMatrix& Result, cMatrix& R1, cMatrix& A2)
{
	Result = R1;
	return Result.AugmentRows(&A2);
}

bool RVertCatR(cMatrix& Result, cMatrix& R1, cMatrix& R2)
{
	Result = R1;
	return Result.AugmentRowsR(&R2);
}

bool VertCat(cMatrix& Result, cMatrix& A1, cMatrix& A2)
{
	if (A1.IsReal()) {
		if (A2.IsReal()) return RVertCatR(Result, A1, A2);
		else return RVertCat(Result, A1, A2);
	}
	else {
		if (A2.IsReal()) return VertCatR(Result, A1, A2);
	}

	if (A1.GetColumns() != A2.GetColumns()) { addError("incompatable matrix dimensions"); return true; }

	Result = A1;
	return Result.AugmentRows(&A2);
}

// horzcat
bool HorzCat(cMatrix& Result, double d1, double d2)
{
	if (Result.Create(1,2)) return true;
	double *pr = Result.GetR();
	pr[0] = d1;
	pr[1] = d2;
	return false;
}

bool HorzCatR(cMatrix& Result, double d1, cMatrix& R2)
{
	if (R2.GetRows()!=1) { addError("incompatable matrix dimensions"); return true; }

	if (Result.Create(1,R2.GetColumns()+1)) return true;

	double *pr = Result.GetR();
	const double *prr = R2.GetSafeR();
	
	pr[0] = d1;
	memcpy( &pr[1], prr, R2.GetColumns()*sizeof(double) );
	return false;
}

bool HorzCat(cMatrix& Result, double d1, cMatrix& A2)
{
	if (A2.IsReal()) return HorzCatR(Result,d1,A2);
	if (A2.GetRows()!=1) { addError("incompatable matrix dimensions"); return true; }

	if (Result.CreateComplex(1,A2.GetColumns()+1)) return true;

	double *pr = Result.GetR();
	double *pi = Result.GetI();
	const double *par = A2.GetSafeR();
	const double *pai = A2.GetSafeI();
	
	pr[0] = d1; pi[0] = 0.;
	memcpy( &pr[1], par, A2.GetColumns()*sizeof(double) );
	memcpy( &pi[1], pai, A2.GetColumns()*sizeof(double) );
	return false;
}

bool RHorzCat(cMatrix& Result, cMatrix& R1, double d2)
{
	if (R1.GetRows()>1) { addError("incompatable matrix dimensions"); return true; }

	int n = R1.GetColumns()+1;
	if (Result.Create(1,n)) return true;

	double *pr = Result.GetR();
	const double *par = R1.GetSafeR();
	
	memcpy( pr, par, R1.GetColumns()*sizeof(double) );
	pr[n-1] = d2;
	return false;
}

bool HorzCat(cMatrix& Result, cMatrix& A1, double d2)
{
	if (A1.IsReal()) return RHorzCat(Result,A1,d2);
	if (A1.GetRows()!=1) { addError("incompatable matrix dimensions"); return true; }

	int n = A1.GetColumns()+1;
	if (Result.CreateComplex(1,n)) return true;

	double *pr = Result.GetR();
	double *pi = Result.GetI();
	const double *par = A1.GetSafeR();
	const double *pai = A1.GetSafeI();
	
	memcpy( pr, par, A1.GetColumns()*sizeof(double) );
	memcpy( pi, pai, A1.GetColumns()*sizeof(double) );
	pr[n-1] = d2; pi[n-1] = 0.;
	return false;
}

bool RHorzCatR(cMatrix& Result, cMatrix& R1, cMatrix& R2)
{
	if (R1.GetRows() != R2.GetRows()) { addError("incompatable matrix dimensions"); return true; }

	Result = R1;
	return Result.AugmentColumnsR(&R2);
}

bool HorzCatR(cMatrix& Result, cMatrix& A1, cMatrix& R2)
{
	if (A1.GetRows() != R2.GetRows()) { addError("incompatable matrix dimensions"); return true; }

	Result = A1;
	return Result.AugmentColumnsR(&R2);
}

bool HorzCat(cMatrix& Result, cMatrix& A1, cMatrix& A2)
{
	if (A1.IsReal()) {
		if (A2.IsReal()) return RHorzCatR(Result, A1, A2);
	}
	else {
		if (A2.IsReal()) return HorzCatR(Result, A1, A2);
	}

	if (A1.GetRows() != A2.GetRows()) { addError("incompatable matrix dimensions"); return true; }

	Result = A1;
	return Result.AugmentRows(&A2);
}

//
// flipud/fliplr
//
bool FlipudR(cMatrix& Result, cMatrix& R)
{
	if (Result.Create(R.GetRows(),R.GetColumns())) return true;

	int half_rows = int( R.GetRows()/2 );
	bool bOdd = IsOdd( R.GetRows() );
	double *pr = Result.GetR();
	const double *prr = R.GetSafeR();
	int ixU, ixL;
	int nr = R.GetRows();
	int nc = R.GetColumns();

	for (int c=0; c<nc; c++) {
		for (int r=0; r<half_rows; r++) {
			ixU = r + c*nr;				// upper part
			ixL = (nr-1-r) + c*nr;		// lower image
			pr[ixL] = prr[ixU];		// swap
			pr[ixU] = prr[ixL];
		}
	}
	if (bOdd) {
		for (int c=0; c<nc; c++) {
			ixU = half_rows + c*nr;				// upper part
			ixL = (nr-1-half_rows) + c*nr;		// lower image
			pr[ixL] = prr[ixU];		// swap
			pr[ixU] = prr[ixL];
		}
	}
	return false;
}

bool FlipudC(cMatrix& Result, cMatrix& C)
{
	if (Result.CreateComplex(C.GetRows(),C.GetColumns())) return true;

	int half_rows = int( C.GetRows()/2 );
	bool bOdd = IsOdd( C.GetRows() );
	double *pr = Result.GetR();
	double *pi = Result.GetI();
	const double *pcr = C.GetSafeR();
	const double *pci = C.GetSafeI();
	int ixU, ixL;
	int nr = C.GetRows();
	int nc = C.GetColumns();

	for (int c=0; c<nc; c++) {
		for (int r=0; r<half_rows; r++) {
			ixU = r + c*nr;				// upper part
			ixL = (nr-1-r) + c*nr;		// lower image
			pr[ixL] = pcr[ixU];		// swap
			pr[ixU] = pcr[ixL];
			pi[ixL] = pci[ixU];		// swap
			pi[ixU] = pci[ixL];
		}
	}

	if (bOdd) {
		for (int c=0; c<nc; c++) {
			ixU = half_rows + c*nr;				// upper part
			ixL = (nr-1-half_rows) + c*nr;		// lower image
			pr[ixL] = pcr[ixU];		// swap
			pr[ixU] = pcr[ixL];
			pi[ixL] = pci[ixU];		// swap
			pi[ixU] = pci[ixL];
		}
	}
	return false;
}

bool FliplrR(cMatrix& Result, cMatrix& R)
{
	if (Result.Create(R.GetRows(),R.GetColumns())) return true;

	int half_cols = int( R.GetColumns()/2 );
	bool bOdd = IsOdd( R.GetColumns() );
	double *pr = Result.GetR();
	const double *prr = R.GetSafeR();
	int ixL, ixR;
	int nr = R.GetRows();
	int nc = R.GetColumns();

	for (int r=0; r<nr; r++) {
		for (int c=0; c<half_cols; c++) {
			ixL = r + c*nr;				// left part
			ixR = r + (nc-1-c)*nr;		// right image
			pr[ixR] = prr[ixL];		// swap
			pr[ixL] = prr[ixR];
		}
	}

	if (bOdd) {
		for (int r=0; r<nr; r++) {
			ixL = r + half_cols*nr;				// left part
			ixR = r + (nc-1-half_cols)*nr;		// right image
			pr[ixR] = prr[ixL];		// swap
			pr[ixL] = prr[ixR];
		}
	}
	return false;
}

bool FliplrC(cMatrix& Result, cMatrix& C)
{
	if (Result.CreateComplex(C.GetRows(),C.GetColumns())) return true;

	int half_cols = int( C.GetColumns()/2 );
	bool bOdd = IsOdd( C.GetColumns() );
	double *pr = Result.GetR();
	double *pi = Result.GetI();
	const double *pcr = C.GetSafeR();
	const double *pci = C.GetSafeI();
	int ixL, ixR;
	int nr = C.GetRows();
	int nc = C.GetColumns();

	for (int r=0; r<nr; r++) {
		for (int c=0; c<half_cols; c++) {
			ixL = r + c*nr;				// left part
			ixR = r + (nc-1-c)*nr;		// right image
			pr[ixR] = pcr[ixL];		// swap
			pr[ixL] = pcr[ixR];
			pi[ixR] = pci[ixL];		// swap
			pi[ixL] = pci[ixR];
		}
	}

	if (bOdd) {
		for (int r=0; r<nr; r++) {
			ixL = r + half_cols*nr;				// left part
			ixR = r + (nc-1-half_cols)*nr;		// right image
			pr[ixR] = pcr[ixL];		// swap
			pr[ixL] = pcr[ixR];
			pi[ixR] = pci[ixL];		// swap
			pi[ixL] = pci[ixR];
		}
	}
	return false;
}

// interface fcns
bool Fliplr(cMatrix& Result, cMatrix& A)
{
	if (A.IsReal()) return FliplrR(Result, A);
	else return FliplrC(Result, A);
}
bool Flipud(cMatrix& Result, cMatrix& A)
{
	if (A.IsReal()) return FlipudR(Result, A);
	else return FlipudC(Result, A);
}


//
// fill matrix
//
bool FillMatrixR(cMatrix& Result, double dv, int nr, int nc)
{
	// create new matrix of dimension nr-by-nc, fill with value dv
	if (Result.Create(nr,nc)) return true;

	int n = nr*nc;
	double *pr = Result.GetR();

	for (int i=0; i<n; ++i) { pr[i] = dv; }
	return false;
}

bool FillMatrixC(cMatrix& Result, complex<double>& cv, int nr, int nc)
{
	// create new matrix of dimension nr-by-nc, fill with value dv
	if (Result.CreateComplex(nr,nc)) return true;

	int n = nr*nc;
	double *pr = Result.GetR();
	double *pi = Result.GetI();
	double dr = cv.real();
	double di = cv.imag();

	for (int i=0; i<n; ++i) { pr[i] = dr; pi[i] = di; }
	return false;
}

bool SetDiagR(cMatrix& Result, double dv)
{
	int n = Min(Result.GetRows(),Result.GetColumns());
	int nr = Result.GetRows();
	double *pr = Result.GetR();

	for (int i=0; i<n; ++i) { pr[i+i*nr] = dv; }
	return false;
}

bool SetDiagC(cMatrix& Result, complex<double>& cv)
{
	int n = Min(Result.GetRows(),Result.GetColumns());
	int nr = Result.GetRows();
	double *pr = Result.GetR();
	double *pi = Result.GetI();
	double dr = cv.real();
	double di = cv.imag();

	for (int i=0; i<n; ++i) { pr[i+i*nr] = dr;  pi[i+i*nr] = di; }
	return false;
}

bool FillDiagR(cMatrix& Result, double dv, int nr, int nc)
{
	if (FillMatrixR(Result, 0., nr, nc)) return true;
	return SetDiagR(Result, dv);
}

bool FillDiagC(cMatrix& Result, complex<double>& cv, int nr, int nc)
{
	if (FillMatrixR(Result, 0., nr, nc)) return true;
	return SetDiagC(Result, cv);
}

bool FillDiag(cMatrix& Result, cMatrix& diag)
{
	if (!diag.IsVector()) { addError("non-vector argument is illegal"); return true; }

	int i;
	const double *pdr, *pdi;
	double *pr, *pi;

	int n = diag.GetSize();
	if (diag.IsReal()) {	// real matrix
		if (Result.Create(n,n)) return true;
		pr = Result.GetR();
		pdr = diag.GetR();
		memset( pr, 0, n*n*sizeof(double) );
		for (i=0; i<n; ++i) { pr[i+n*i] = pdr[i]; }
	}
	else {					// complex matrix
		if (Result.CreateComplex(n,n)) return true;
		pr = Result.GetR();
		pi = Result.GetI();
		pdr = diag.GetR();
		pdi = diag.GetI();
		memset( pr, 0, n*n*sizeof(double) );
		for (i=0; i<n; ++i) { pr[i+n*i] = pdr[i]; pi[i+n*i] = pdi[i]; }
	}

	return false;
}


// interface
bool GetDiag(cMatrix& Result, cMatrix& mat)
{
	return mat.GetDiag(Result);
}


bool FillIdentity(cMatrix& Result, int nr, int nc)
{
	return FillDiagR(Result, 1., nr, nc);
}

bool FillIdentity(cMatrix& Result, int ndim)
{
	return FillDiagR(Result, 1., ndim, ndim);
}

int BlockDiagR(cMatrix& Result, cMatrix& A, cMatrix& B)
{
	int iErr = __NO_ERRORS;
	int n = A.GetRows() + B.GetRows();
	int m = A.GetColumns() + B.GetColumns();

	if (Result.Create(n,m)) return MEMORY_ERROR;

	Result.Reset();
	Result.Set(0,0, A);
	Result.Set(A.GetRows(),A.GetColumns(), B);

	return iErr;
}

int BlockDiagC(cMatrix& Result, cMatrix& A, cMatrix& B)
{
	int iErr = __NO_ERRORS;
	int n = A.GetRows() + B.GetRows();
	int m = A.GetColumns() + B.GetColumns();

	if (Result.CreateComplex(n,m)) return MEMORY_ERROR;

	Result.Reset();
	Result.Set(0,0, A);
	Result.Set(A.GetRows(),A.GetColumns(), B);

	return iErr;
}

// interfaces

bool NewMat(cMatrix& Cout, cMatrix& vn)
{
	return NewMat(Cout, vn, vn);
}

bool NewMat(cMatrix& Cout, cMatrix& vr, cMatrix& vc)
{
	if (!vr.IsScalar() || !vc.IsScalar()) { addError("illegal argument"); return true; }

	// convert args to nr, nc integer values
	int nr = (int) vr.Get0R();
	int nc = (int) vc.Get0R();

	// fill Result with zeros
	return FillMatrixR(Cout, 0., nr, nc);
}

//bool NewMat(cMatrix& Cout, cMatrix& v,  cMatrix& vr, cMatrix& vc)
//{
//	if (!vr.IsScalar() || !vc.IsScalar() || !v.IsScalar()) { addError("illegal argument"); return true; }
//
//	// convert args to nr, nc integer values
//	int nr = (int) vr.Get0R();
//	int nc = (int) vc.Get0R();
//	double dv = v.Get0R();
//
//	// fill Result with value dv
//	return FillMatrixR(Cout, dv, nr, nc);
//}

bool NewDiag(cMatrix& Cout, cMatrix& v)
{
	if (!v.IsVector()) { addError("illegal argument"); return true; }

	return FillDiag(Cout, v);
}

bool NewDiag(cMatrix& Cout, cMatrix& v, cMatrix& vn)
{
	return NewDiag(Cout, v, vn, vn);
}

bool NewDiag(cMatrix& Cout, cMatrix& v, cMatrix& vr, cMatrix& vc)
{
	if (!vr.IsScalar() || !vc.IsScalar() || !v.IsScalar()) { addError("illegal argument"); return true; }

	// convert args to nr, nc integer values
	int nr = (int) vr.Get0R();
	int nc = (int) vc.Get0R();
	double dv;
	complex<double> cv;

	if (v.IsReal()) {
		dv = v.Get0R();
		return FillDiagR(Cout, dv, nr, nc);
	}
	else {
		cv = v.Get0C();
		return FillDiagC(Cout, cv, nr, nc);
	}
}

bool NewIdentity(cMatrix& Cout, cMatrix& vn)
{
	if (!vn.IsScalar()) { addError("illegal argument"); return true; }

	// convert args to nr, nc integer values
	int nd = (int) vn.Get0R();

	// fill Result with zeros
	return FillDiagR(Cout, 1., nd, nd);
}

bool NewIdentity(cMatrix& Cout, cMatrix& vr, cMatrix& vc)
{
	if (!vr.IsScalar() || !vc.IsScalar()) { addError("illegal argument"); return true; }

	// convert args to nr, nc integer values
	int nr = (int) vr.Get0R();
	int nc = (int) vc.Get0R();

	// fill Result with zeros
	return FillDiagR(Cout, 1., nr, nc);
}

bool Repeat(cMatrix& res, cMatrix& in, cMatrix& vr, cMatrix& vc)
{
	res.Delete();

	if (!vr.IsScalar() || !vc.IsScalar()) { addError("illegal argument"); return true; }

	// convert args to nr, nc integer values
	int nrm = (int) vr.Get0R();
	int ncm = (int) vc.Get0R();

	if (nrm < 0 || ncm < 0) {
		addError("illegal argument"); return true;
	}

	int nr = in.GetRows() * nrm;
	int nc = in.GetColumns() * ncm;

	if (in.IsReal()) {
		if (res.Create(nr,nc)) {
			addError("memory error");
			return true;
		}
	}
	else {
		if (res.CreateComplex(nr,nc)) {
			addError("memory error");
			return true;
		}
	}

	vector<double>::iterator src;
	int pos = 0, r, c, j;
	int sz = in.GetSize();

	for (r=0; r<nrm; ++r) {
		for (c=0; c<ncm; ++c) {
			pos = r * in.GetRows() + (c * in.GetColumns())*nr;
			src = in.mr.begin();
			for (j=0; j<in.GetColumns(); ++j) {
				res.FillR(pos, src, in.GetRows());
				pos += nr;
				src += in.GetRows();
			}
		}
	}

	if (!in.IsReal()) {		// do imaginary part
		for (r=0; r<nrm; ++r) {
			for (c=0; c<ncm; ++c) {
				pos = r * in.GetRows() + (c * in.GetColumns())*nr;
				src = in.mi.begin();
				for (j=0; j<in.GetColumns(); ++j) {
					res.FillI(pos, src, in.GetRows());
					pos += nr;
					src += in.GetRows();
				}
			}
		}
	}

	return false;
}

//
// arg access
//
// access operator (one arg), rvalue
bool valAccess(cMatrix& Result, double dvar, double darg)
{
	int iarg = (int) darg;
	if (iarg != 0) { addError("index outside array bounds"); return true; }

	if (Result.Create(1,1)) return true;
	double *pr = Result.GetR();

	pr[0] = dvar;

	return false;
}

bool valAccess(cMatrix& Result, double dvar, cMatrix& arg)
{
	if (Result.Create(arg.GetRows(),arg.GetColumns())) return true;

	double *pr = Result.GetR();
	const double *par = arg.GetR();		// ignore imag part
	int n = arg.GetSize();

	// the only valid arg is 0, anything else results in error!
	for (int i=0; i<n; ++i)
	{
		if (int(par[i])==0)	pr[i] = dvar;
		else { Result.Delete(); addError("index outside array bounds"); return true; }
	}
	return false;
}

bool valAccess(cMatrix& Result, cMatrix& var, double darg)
{
	int n = var.GetSize();
	int iarg = (int) darg;
	if (iarg < 0 || iarg >= n) { addError("index outside array bounds"); return true; }

	double *pr, *pi;
	double *par, *pai;

	// get iarg^th entry of var
	if (var.IsReal()) {
		if (Result.Create(1,1)) return true;
		pr = Result.GetR();
		par = var.GetR();
		pr[0] = par[iarg];
	}
	else {	// complex
		if (Result.CreateComplex(1,1)) return true;
		pr = Result.GetR();
		pi = Result.GetI();
		par = var.GetR();
		pai = var.GetI();
		pr[0] = par[iarg];
		pi[0] = pai[iarg];
	}

	return false;
}

bool valAccess(cMatrix& Result, cMatrix& var, cMatrix& arg)
{
	if (!arg.IsVector()) { addError("non-vector argument"); return true; }

	int n = arg.GetSize(), nvar = var.GetSize();
	int iarg, i;

	double *pr, *pi;
	double *pvr, *pvi;
	double *par;

	if (var.IsReal()) {
		if (Result.Create(arg.GetRows(),arg.GetColumns())) return true;
		pr = Result.GetR();
		pvr = var.GetR();
		par = arg.GetR();		// ignore imag part of arg
		for (i=0; i<n; ++i) {
			iarg = (int) par[i];
			if (iarg < 0 || iarg >= nvar) { Result.Delete(); addError("index outside array bounds"); return true; }
			pr[i] = pvr[iarg];
		}
	}
	else {	// complex
		if (Result.CreateComplex(arg.GetRows(),arg.GetColumns())) return true;
		pr = Result.GetR();
		pi = Result.GetI();
		pvr = var.GetR();
		pvi = var.GetI();
		par = arg.GetR();		// ignore imag part of arg
		for (i=0; i<n; ++i) {
			iarg = (int) par[i];
			if (iarg < 0 || iarg >= nvar) { Result.Delete(); addError("index outside array bounds"); return true; }
			pr[i] = pvr[iarg];
			pi[i] = pvi[iarg];
		}
	}

	return false;
}

//
//bool arg2ix( cMatrix& Result, cMatrix& arg1, cMatrix& arg2, int nRows, int nCols )
//{	// convert values in arg1,arg2 to indices into a matrix of size (nRows,nCols)
//
//	if (arg1.IsScalar()) { return arg2ix( Result, arg1.Get0R(), arg2, nRows, nCols ); }
//	if (arg2.IsScalar()) { return arg2ix( Result, arg1, arg2.Get0R(), nRows, nCols ); }
//
//	if (arg1.IsVector() && !arg2.IsVector()) { addError("illegal argument"); return true; }
//	if (!arg1.IsVector() && arg2.IsVector()) { addError("illegal argument"); return true; }
//	
//	int nr1, nr2;
//	int i, j;
//
//	double *pr;
//	const double *par1 = arg1.GetR();
//	const double *par2 = arg2.GetR();
//
//	if (arg1.IsVector()) {	// arg2 must also be vector
//		nr1 = arg1.GetSize();
//		nr2 = arg2.GetSize();
//		if (Result.Create(nr1,nr2)) return true;
//		pr = Result.GetR();
//		for (i=0; i<nr1; ++i) {
//			for (j=0; j<nr2; ++j) {
//				pr[i+nr1*j] = floor( par1[i]+nRows*par2[j] );
//			}
//		}
//	}
//	else {
//		// error, because if arg1 is non-vector matrix, arg2 should have been scalar (handled above)
//		addError("illegal argument"); return true;
//	}
//
//	return false;
//}
//
//bool arg2ix( cMatrix& Result, double d1, cMatrix& arg2, int nRows, int nCols )
//{	// result will be same size as arg2
//
//	int i, j, ix;
//	int nr2 = arg2.GetRows();
//	int nc2 = arg2.GetColumns();
//
//	if (Result.Create(nr2,nc2)) return true;
//
//	double *pr = Result.GetR();
//	const double *par2 = arg2.GetR();
//
//	for (i=0; i<nr2; ++i) {
//		for (j=0; j<nc2; ++j) {
//			ix = i+nc2*j;
//			pr[ix] = floor( d1+nRows*par2[ix] );
//		}
//	}
//
//	return false;
//}
//
//bool arg2ix( cMatrix& Result, cMatrix& arg1, double d2, int nRows, int nCols )
//{	// result will be same size as arg1
//
//	int i, j, ix;
//	int nr1 = arg1.GetRows();
//	int nc1 = arg1.GetColumns();
//
//	if (Result.Create(nr1,nc1)) return true;
//
//	double *pr = Result.GetR();
//	const double *par1 = arg1.GetR();
//
//	for (i=0; i<nr1; ++i) {
//		for (j=0; j<nc1; ++j) {
//			ix = i+nc1*j;
//			pr[ix] = floor( d2+nRows*par1[ix] );
//		}
//	}
//
//	return false;
//}


// access operator (two args), rvalue
bool valAccess(cMatrix& Result, double dvar, double darg1, double darg2)
{
	int iarg1 = (int) darg1;
	int iarg2 = (int) darg2;
	if (iarg1==0 && iarg2==0) {
		if (Result.Create(1,1)) return true;
		Result.SetR(0,dvar);
		return false;
	}
	addError("index outside array bounds"); return true;
}

bool valAccess(cMatrix& Result, double dvar, double darg1, cMatrix& arg2)
{
	if (!arg2.IsVector()) { addError("illegal argument"); return true; }

	int iarg1 = (int) darg1;
	if (iarg1 != 0) { addError("index outside array bounds"); return true; }
	int iarg2;

	int narg = arg2.GetSize();
	if (Result.Create(1,narg)) return true;

	double *pr = Result.GetR();
	double *par = arg2.GetR();		// ignore imag part

	for (int i=0; i<narg; ++i) {
		iarg2 = (int) par[i];
		if (iarg2 != 0) { Result.Delete(); addError("index outside array bounds"); return true; }
		pr[i] = dvar;
	}

	return false;
}

bool valAccess(cMatrix& Result, double dvar, cMatrix& arg1, double darg2)
{
	if (!arg1.IsVector()) { addError("illegal argument"); return true; }

	int iarg2 = (int) darg2;
	if (iarg2 != 0) { addError("index outside array bounds"); return true; }
	int iarg1;

	int narg = arg1.GetSize();
	if (Result.Create(narg,1)) return true;

	double *pr = Result.GetR();
	double *par = arg1.GetR();		// ignore imag part

	for (int i=0; i<narg; ++i) {
		iarg1 = (int) par[i];
		if (iarg1 != 0) { Result.Delete(); addError("index outside array bounds"); return true; }
		pr[i] = dvar;
	}

	return false;
}

bool valAccess(cMatrix& Result, double dvar, cMatrix& arg1, cMatrix& arg2)
{
	if (!arg1.IsVector() || !arg2.IsVector()) { addError("illegal argument"); return true; }

	int iarg1, iarg2, i;

	int narg1 = arg1.GetSize();
	int narg2 = arg2.GetSize();

	if (Result.Create(narg1,narg2)) return true;

	double *pr = Result.GetR();
	double *par1 = arg1.GetR();		// ignore imag part
	double *par2 = arg2.GetR();		// ignore imag part

	for (i=0; i<narg1; ++i) {
		iarg1 = (int) par1[i];
		if (iarg1 != 0) { Result.Delete(); addError("index outside array bounds"); return true; }
	}

	for (i=0; i<narg2; ++i) {
		iarg2 = (int) par2[i];
		if (iarg2 != 0) { Result.Delete(); addError("index outside array bounds"); return true; }
	}

	memset( pr, 0, narg1*narg2*sizeof(double) );

	return false;
}

bool valAccess(cMatrix& Result, cMatrix& var, double darg1, cMatrix& arg2)
{
	if (!arg2.IsVector()) { addError("illegal argument"); return true; }

	int iarg1 = (int) darg1;
	int nvar = var.GetSize();
	int nv_rows = var.GetRows(), nv_cols = var.GetColumns();

	if (iarg1 < 0 || iarg1 >= nv_rows) { addError("index outside array bounds"); return true; }
	int iarg2;

	int narg = arg2.GetSize();
	double *pr, *pi;
	const double *pvr, *pvi, *par;
	double dval;
	complex<double> cval;

	if (var.IsReal()) {
		if (Result.Create(1,narg)) return true;

		pr = Result.GetR();
		pvr = var.GetR();
		par = arg2.GetR();		// ignore imag part

		for (int i=0; i<narg; ++i) {
			iarg2 = (int) par[i];
			if (iarg2 < 0 || iarg2 >= nv_cols) { Result.Delete(); addError("index outside array bounds"); return true; }
			var.GetR(dval,iarg1,iarg2);
			pr[i] = dval;
		}
	}
	else {
		if (Result.CreateComplex(1,narg)) return true;

		pr = Result.GetR();
		pi = Result.GetI();
		pvr = var.GetR();
		pvi = var.GetI();
		par = arg2.GetR();		// ignore imag part

		for (int i=0; i<narg; ++i) {
			iarg2 = (int) par[i];
			if (iarg2 < 0 || iarg2 >= nv_cols) { Result.Delete(); addError("index outside array bounds"); return true; }
			var.GetC(cval,iarg1,iarg2);
			pr[i] = cval.real();
			pi[i] = cval.imag();
		}
	}

	return false;
}

bool valAccess(cMatrix& Result, cMatrix& var, cMatrix& arg1, double darg2)
{
	if (!arg1.IsVector()) { addError("illegal argument"); return true; }

	int iarg2 = (int) darg2;
	int nvar = var.GetSize();
	int nv_rows = var.GetRows(), nv_cols = var.GetColumns();

	if (iarg2 < 0 || iarg2 >= nv_cols) { addError("index outside array bounds"); return true; }
	int iarg1;

	int narg = arg1.GetSize();
	double *pr, *pi;
	const double *pvr, *pvi, *par;
	double dval;
	complex<double> cval;

	if (var.IsReal()) {
		if (Result.Create(narg,1)) return true;

		pr = Result.GetR();
		pvr = var.GetR();
		par = arg1.GetR();		// ignore imag part

		for (int i=0; i<narg; ++i) {
			iarg1 = (int) par[i];
			if (iarg1 < 0 || iarg1 >= nv_rows) { Result.Delete(); addError("index outside array bounds"); return true; }
			var.GetR(dval,iarg1,iarg2);
			pr[i] = dval;
		}
	}
	else {
		if (Result.CreateComplex(narg,1)) return true;

		pr = Result.GetR();
		pi = Result.GetI();
		pvr = var.GetR();
		pvi = var.GetI();
		par = arg1.GetR();		// ignore imag part

		for (int i=0; i<narg; ++i) {
			iarg1 = (int) par[i];
			if (iarg1 < 0 || iarg1 >= nv_rows) { Result.Delete(); addError("index outside array bounds"); return true; }
			var.GetC(cval,iarg1,iarg2);
			pr[i] = cval.real();
			pi[i] = cval.imag();
		}
	}

	return false;
}

bool valAccess(cMatrix& Result, cMatrix& var, double darg1, double darg2)
{
	int iarg1 = (int) darg1;
	int iarg2 = (int) darg2;
	int nv_rows = var.GetRows(), nv_cols = var.GetColumns();

	if (iarg1 < 0 || iarg1 >= nv_rows) { addError("index outside array bounds"); return true; }
	if (iarg2 < 0 || iarg2 >= nv_cols) { addError("index outside array bounds"); return true; }

	double *pr, *pi;
	double dval;
	complex<double> cval;

	if (var.IsReal()) {
		if (Result.Create(1,1)) return true;
		pr = Result.GetR();
		var.GetR(dval,iarg1,iarg2);
		pr[0] = dval;
	}
	else {
		if (Result.CreateComplex(1,1)) return true;
		pr = Result.GetR();
		pi = Result.GetI();
		var.GetC(cval,iarg1,iarg2);
		pr[0] = cval.real();
		pi[0] = cval.imag();
	}

	return false;
}

bool valAccess(cMatrix& Result, cMatrix& var, cMatrix& arg1, cMatrix& arg2)
{
	// both arguments must be vectors
	if (!arg1.IsVector() || !arg2.IsVector()) { addError("illegal argument"); return true; }

	int nvar = var.GetSize();
	int nv_rows = var.GetRows(), nv_cols = var.GetColumns();

	int iarg1, iarg2;
	int i, j, ix;

	int narg1 = arg1.GetSize();
	int narg2 = arg2.GetSize();

	double *pr, *pi;
	const double *pvr, *pvi, *par1, *par2;
	double dval;
	complex<double> cval;

	if (var.IsReal()) {
		if (Result.Create(narg1,narg2)) return true;

		pr = Result.GetR();
		pvr = var.GetR();
		par1 = arg1.GetR();		// ignore imag part
		par2 = arg2.GetR();		// ignore imag part

		for (i=0; i<narg1; ++i) {

			iarg1 = (int) par1[i];
			if (iarg1 < 0 || iarg1 >= nv_rows) { Result.Delete(); addError("index outside array bounds"); return true; }

			for (j=0; j<narg2; ++j) {
				iarg2 = (int) par2[j];
				if (iarg2 < 0 || iarg2 >= nv_cols) { Result.Delete(); addError("index outside array bounds"); return true; }
				var.GetR(dval,iarg1,iarg2);
				ix = var.GetIndex(iarg1,iarg2);
				pr[ix] = dval;
			}

		}
	}
	else {
		if (Result.CreateComplex(narg1,narg2)) return true;

		pr = Result.GetR();
		pi = Result.GetI();
		pvr = var.GetR();
		pvi = var.GetI();
		par1 = arg1.GetR();		// ignore imag part
		par2 = arg2.GetR();		// ignore imag part

		for (i=0; i<narg1; ++i) {

			iarg1 = (int) par1[i];
			if (iarg1 < 0 || iarg1 >= nv_rows) { Result.Delete(); addError("index outside array bounds"); return true; }

			for (j=0; j<narg2; ++j) {
				iarg2 = (int) par2[j];
				if (iarg2 < 0 || iarg2 >= nv_cols) { Result.Delete(); addError("index outside array bounds"); return true; }
				var.GetC(cval,iarg1,iarg2);
				ix = var.GetIndex(iarg1,iarg2);
				pr[ix] = cval.real();
				pi[ix] = cval.imag();
			}

		}

	}

	return false;
}


//bool ixAssign( cMatrix& lvalue, cMatrix& rvalue, cMatrix& idx )
//{
//	// rvalue and idx must be the same size (assumed already checked)
//	// indices indicated by idx MUST be valid for lvalue (assumed already converted
//	//  to absolute column-wise coodinate)
//
//	int lsize = lvalue.GetSize();
//	int n = idx.GetSize();
//	const double *pix = idx.GetR();
//	const double *prr = rvalue.GetR(), *pri;
//	double *plr = lvalue.GetR(), *pli;
//
//	int nleft = lvalue.GetSize();
//	bool brr(false), bcr(false), bcc(false);
//
//	// check complex-ness
//	if (!rvalue.IsReal() && lvalue.IsReal()) {
//		if ( lvalue.CreateImaginary(lvalue.GetRows(), lvalue.GetColumns()) ) return true;	// out of memory
//		pri = rvalue.GetI();
//		pli = lvalue.GetI();
//		bcc = true;
//	}
//	else if (!rvalue.IsReal() && !lvalue.IsReal()) {
//		pri = rvalue.GetI();
//		pli = lvalue.GetI();
//		bcc = true;
//	}
//	else if (rvalue.IsReal() && lvalue.IsReal())
//		brr = true;
//	else if (rvalue.IsReal() && !lvalue.IsReal())
//		bcr = true;
//
//	int i, ix;
//
//	for (i=0; i<n; ++i) {
//		// get index
//		ix = (int) pix[i];
//		// check bounds
//		if (ix<0 || ix>=lsize) return true;
//		// assign to lvalue
//		plr[ix] = prr[i];
//		if (bcc) pli[ix] = pri[i];
//	}
//
//	return false;
//}

