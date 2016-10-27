// matrix.cpp
//
// Global functions and member function implementations.

#include "matrix.h"
#include "my_errors.h"
#include <memory.h>
#include <float.h>
#include <stdio.h>
#include <assert.h>

//FILE *pfLog;

bool cMatrix::m_bLog = false;

//extern int CompareByRealValue(const void *elem1, const void *elem2);
//extern int CompareByComplexValue(const void *elem1, const void *elem2);
//extern bool elemLessThan( sortElem ei, sortElem ej );

int CompareByRealValue(const void *elem1, const void *elem2)
{
	double *p1 = (double*)elem1;
	double *p2 = (double*)elem2;

	if (*p1 < *p2) return -1;
	else if (*p1 == *p2) return 0;
	else return 1;
}

int CompareByRealValueIx(const void *elem1, const void *elem2)
{
	sortElem *p1 = (sortElem*)elem1;
	sortElem *p2 = (sortElem*)elem2;

	if (p1->dv < p2->dv) return -1;
	else if (p1->dv == p2->dv) return 0;
	else return 1;
}

int CompareByComplexValue(const void *elem1, const void *elem2)
{
    complex<double> *p1 = (complex<double>*)elem1;
    complex<double> *p2 = (complex<double>*)elem2;

	double abs_1 = std::abs(*p1);
	double abs_2 = std::abs(*p2);
	double ph_1, ph_2;

	if (abs_1 < abs_2) return -1;
	else if (abs_1 == abs_2) {
		ph_1 = arg(*p1);
		ph_2 = arg(*p2);
		if (ph_1 < ph_2) return -1;
		if (ph_1 == ph_2) return 0;
		else return 1;
	}
	else return 1;
}

// sorting function
//extern int CompareByRealValueIx(const void *elem1, const void *elem2);
bool elemLessThan( sortElem ei, sortElem ej ) { return (ei.dv<ej.dv); }


cMatrix::cMatrix( const cMatrix& A )  // assignment operator
{
	m_nType = VAR_TYPE_NUMERIC;

	m_name = A.m_name;
	nr = nc = 0;

	DeleteCopy(A);
}

bool cMatrix::operator=( const cMatrix& A )  // assignment operator
{
	m_nType = VAR_TYPE_NUMERIC;
	m_name = A.m_name;
	return DeleteCopy(A);
}
bool cMatrix::operator=( vector<double>& A )  // assignment operator
{
	m_nType = VAR_TYPE_NUMERIC;
	Delete();
	if (A.size()==0)
		return false;

	if (Create(1,A.size())) return true;
	// element-by-element copy... surely there is a better way?
	for (int i=0; i<(int)A.size(); ++i)
	{
		mr[i] = A[i];
	}
	return false;
}
bool cMatrix::operator=( double& d )			// assignment operator
{
	m_nType = VAR_TYPE_NUMERIC;
	if (Create(1,1)) return true;
	mr[0] = d;
	return false;
}
bool cMatrix::operator=( complex<double>& c )	// assignment operator
{
	m_nType = VAR_TYPE_NUMERIC;
	if (CreateComplex(1,1)) return true;
	mr[0] = c.real();
	mi[0] = c.imag();
	return false;
}

double * cMatrix::NewRealDouble(int size)
{
	mr.clear();
	mr.reserve(size);
	mr.insert(mr.begin(), size, 0.);
	return mr.data();
}

double * cMatrix::NewImagDouble(int size)
{
	mi.clear();
	mi.reserve(size);
	mi.insert(mi.begin(), size, 0.);
	return mi.data();
}

bool cMatrix::Create(const int size)
{
	if (size >= INT_MAX) {
		addError("matrix size exceeds limits");
		return true;
	}

	Delete();

	if (size<0)
		return false;

	// create column vector by default
	nr = size;
	nc = 1;

	NewRealDouble(size);
	//if (mr==NULL) { nr = nc = 0; addError("out of memory"); return true; }

	return false;
}

bool cMatrix::Create(const int rows, const int cols)
{
	if ( rows<0 || cols<0 ) {
		addError("illegal dimension (negative)");
		return true;
	}

	if (rows==0 || cols==0) {
		Delete();
		return false;
	}

	//if (cols >= INT_MAX / rows) {
	if ( cols >= (int) (double(INT_MAX) / double(rows)) ) {
		addError("matrix size exceeds limits");
		return true;
	}
	
	int size = rows*cols;
	if ( Create(size) ) return true;
	else {
		nr = rows;
		nc = cols;
	}
	return false;
}

bool cMatrix::Create(const int rows, const int cols, const double *pr)
{
	int size = rows*cols;
	if ( Create(size) ) return true;
	else {
		nr = rows;
		nc = cols;
		for (int i=0; i<size; ++i) {
			mr[i] = pr[i];
		}
//		memcpy( mr.data(), pr, size*sizeof(double) );
	}
	return false;
}

bool cMatrix::Create(cMatrix& mat)	// create with same size as mat
{
	nr = mat.GetRows();
	nc = mat.GetColumns();

	int size = nr*nc;
	if ( Create(size) ) return true;

	return false;
}

bool cMatrix::CreateComplex(cMatrix& mat)	// create complex with same size as mat
{
	nr = mat.GetRows();
	nc = mat.GetColumns();

	int size = nr*nc;
	if ( CreateComplex(size) ) return true;

	return false;
}

bool cMatrix::CreateComplex(const int size)
{
	if (size<0)
		return false;

	Delete();

	// create column vector by default
	nr = size;
	nc = 1;

	NewRealDouble(size);
	//if (mr==NULL) { nr = nc = 0; addError("out of memory"); return true; }
	NewImagDouble(size);
	//if (mi==NULL) { delete []mr; nr = nc = 0; addError("out of memory"); return true; }

	return false;
}

bool cMatrix::CreateComplex(const int rows, const int cols)
{
	if ( rows<0 || cols<0 ) {
		addError("illegal dimension (negative)");
		return true;
	}

	if (rows==0 || cols==0) {
		Delete();
		return false;
	}

	if ( cols >= (int) (double(INT_MAX) / double(rows)) ) {
		addError("matrix size exceeds limits");
		return true;
	}

	int size = rows*cols;
	if ( CreateComplex(size) ) return true;
	else {
		nr = rows;
		nc = cols;
	}
	return false;
}

bool cMatrix::CreateComplex(vector<double>& re, vector<double>& im)
{
	assert( re.size() == im.size() );

	mr.clear(); mi.clear();
	mr = re;
	mi = im;

	// column vector assumption
	nr = re.size();
	nc = 1;

	return false;
}

bool cMatrix::CreateImaginary(const int r, const int c)
{	// designed to be used for already-created real matrices

	if ( r<0 || c<0 ) {
		addError("illegal dimension (negative)");
		return true;
	}

	if (r==0 || c==0) {
		Delete();
		return false;
	}

	if ( c >= (int) (double(INT_MAX) / double(r)) ) {
		addError("matrix size exceeds limits");
		return true;
	}

	int size = r*c;

	NewImagDouble(size);
	//if (mi==NULL) { delete []mr; nr = nc = 0; addError("out of memory"); return true; }

	memset( mi.data(), 0, size*sizeof(double) );

	return false;
}

bool cMatrix::CreateRow( vector<double>& v )
{
	int n = v.size();
	if (Create(1,n)) return true;
	for (int i=0; i<n; ++i) { mr[i] = v[i]; }
	return false;
}

bool cMatrix::CreateColumn( vector<double>& v )
{
	int n = v.size();
	if (Create(n,1)) return true;
	for (int i=0; i<n; ++i) { mr[i] = v[i]; }
	return false;
}

bool cMatrix::Copy(const cMatrix& A)
{
	// WARNING: assumes memory has already been allocated!

	assert( GetSize()==A.GetSize() );

	//if (mr==NULL && A.GetSize()==0) return false;
	mr.clear(); mi.clear();
	mr = A.mr;
	mi = A.mi;
	//if ( memcpy( mr, A.GetSafeR(), A.GetSize()*sizeof(double) ) == NULL)
	//	return true;

	//if (!A.IsReal()) {
	//	if (mi==NULL && A.GetSize()==0) return false;
	//	if ( memcpy( mi, A.GetSafeI(), A.GetSize()*sizeof(double) ) == NULL)
	//		return true;
	//}

	return false;
}

bool cMatrix::CopyR(const cMatrix& A)
{
	assert( GetSize()==A.GetSize() );
	mr = A.mr;
	return false;
}

bool cMatrix::CopyI(const cMatrix& A)
{
	assert( GetSize()==A.GetSize() );
	mi = A.mi;
	return false;
}

bool cMatrix::DeleteCopy(const cMatrix& A)
{
	if (A.IsReal()) { if (Create( A.GetRows(), A.GetColumns() )) return true; }
	else { if (CreateComplex( A.GetRows(), A.GetColumns() )) return true; }

	return Copy(A);
}

bool cMatrix::DeleteCopyR(const double *pd, int rows, int columns)
{
	mr.clear();
	int size = rows*columns;
	mr.insert( mr.begin(), pd, &pd[size] );
	nr = rows;
	nc = columns;
	return false;
}

bool cMatrix::DeleteCopyI(const double *pd, int rows, int columns)
{
	mi.clear();
	int size = rows*columns;
	mi.insert( mr.begin(), pd, &pd[size] );
	nr = rows;
	nc = columns;
	return false;
}

// Get & set operations
//

// "safe" getting (bounds checking)
bool cMatrix::GetSafeR(double& d, int i) const
{
	if (i<0 || i>=GetSize()) { addError("index outside array bounds"); return true; }

	d = mr[i];
	return false;
}

bool cMatrix::GetSafeR(double& d, int i, int j) const
{
	int ix = i + j*nc;
	if (ix<0 || ix>=GetRows()) { addError("index outside array bounds"); return true; }
	if (ix<0 || ix>=GetColumns()) { addError("index outside array bounds"); return true; }

	d = mr[ix];
	return false;
}

bool cMatrix::GetSafe(complex<double>& c, int i, int j) const
{
	if (i<0 || i>=GetRows()) { addError("index outside array bounds"); return true; }
	if (j<0 || j>=GetColumns()) { addError("index outside array bounds"); return true; }

	int ix = i + j*nc;
	if (IsReal()) { c = complex<double>(mr[ix],0.); }
	else { c = complex<double>(mr[ix],mi[ix]); }

	return false;
}

bool cMatrix::GetSafe(complex<double>& c, int i) const
{
	if (i<0 || i>=GetSize()) { addError("index outside array bounds"); return true; }

	if (IsReal()) { c = complex<double>(mr[i],0.); }
	else { c = complex<double>(mr[i],mi[i]); }

	return false;
}

bool cMatrix::GetSafe0(complex<double>& c) const
{
	if (GetSize()==0) { addError("index outside array bounds"); return true; }

	if (IsReal()) { c = complex<double>(mr[0],0.); }
	else { c = complex<double>(mr[0],mi[0]); }

	return false;
}

bool cMatrix::GetSafe0(double& d) const
{
	if (GetSize()==0) { addError("index outside array bounds"); return true; }

	d = mr[0];
	return false;
}

bool cMatrix::GetRz(double& d, int i, int j) const
{
	if (i<0 || i>=nr || j<0 || j>=nc) {		// boundary indication
		d = 0.;
		return true;
	}

	int ix = i + j*nc;
	d = mr[ix];
	return false;
}

bool cMatrix::GetIz(double& d, int i, int j) const
{
	if (i>=nr || j>=nc || IsReal()) {		// boundary indication
		d = 0.;
		return true;
	}

	int ix = i + j*nc;
	d = mi[ix];
	return false;
}

bool cMatrix::Getz(complex<double>& c, int i, int j) const
{
	if (i>=nr || j>=nc) {		// boundary indication
		c = complex<double>(0.,0.);
		return true;
	}

	int ix = i + j*nc;
	c= complex<double>(mr[ix],mi[ix]);
	return false;
}

// "unsafe" getting (faster)
void cMatrix::Get0(double& d) const
{ d = mr[0]; }

void cMatrix::Get0(double& dr, double& di) const
{
	if (IsReal()) { dr = mr[0]; di = 0.; }
	else { dr = mr[0]; di = mi[0]; }
}

void cMatrix::Get0(complex<double>& c) const
{
	if (IsReal()) { c = complex<double>(mr[0],0.); }
	else { c = complex<double>(mr[0],mi[0]); }
}

void cMatrix::GetC(complex<double>& c, int i, int j) const
{
	int ix = i + j*nr;
	if (IsReal()) { c = complex<double>(mr[ix],0.); }
	else { c = complex<double>(mr[ix],mi[ix]); }
}

void cMatrix::GetC(complex<double>& c, int i) const
{
	if (IsReal()) { c = complex<double>(mr[i],0.); }
	else { c = complex<double>(mr[i],mi[i]); }
}

void cMatrix::GetR(double& lf, const int i, const int j) const
{ lf = mr[i+nr*j]; }

void cMatrix::GetR(double& lf, const int ix) const
{ lf = mr[ix]; }

void cMatrix::GetI(double& lf, const int i, const int j) const
{ lf = mi[i+nr*j]; }

void cMatrix::GetI(double& lf, const int ix) const
{ lf = mi[ix]; }

void cMatrix::Get(complex<double>& c, const int ix) const
{
	double lfr, lfi;
	GetR(lfr,ix);

	if (IsReal()) {
		c = complex<double>(lfr,0.);
		return;
	}

	GetI(lfi,ix);
	c = complex<double>(lfr,lfi);
}

void cMatrix::Get(complex<double>& c, const int i, const int j) const
{ Get(c,i + j*nr); }

bool cMatrix::GetR(cMatrix& subMat, const int i, const int j, const int m, const int n) const
{
	if (subMat.Create(m-i+1,n-j+1)) { return true; }
	double lf;

	for (int k=0; k<subMat.GetRows(); k++)
	{
		for (int l=0; l<subMat.GetColumns(); l++)
		{
			GetR(lf,i+k,j+l);
			subMat.SetR(k,l,lf);
		}
	}

	return false;
}

bool cMatrix::GetC(cMatrix& subMat, const int i, const int j, const int m, const int n) const
{
	if (subMat.CreateComplex(m-i+1,n-j+1)) { return true; }
	complex<double> cv;

	for (int k=0; k<subMat.GetRows(); k++)
	{
		for (int l=0; l<subMat.GetColumns(); l++)
		{
			GetC(cv,i+k,j+l);
			subMat.Set(k,l,cv);
		}
	}

	return false;
}

bool cMatrix::GetFirstR(double& lf) const
{ return GetSafeR(lf,0); }

bool cMatrix::GetLastR(double& lf) const
{ return GetSafeR(lf,GetSize()-1); }

// set
//
bool cMatrix::SetSafeR(const int i, const int j, double x)
{
	int idx = i + j*nr;
	if ( idx<0 || idx>=GetSize() ) { addError("index outside array bounds"); return true; }

	mr[idx] = x;
	return false;
}

bool cMatrix::SetSafeR(const int i, double x)
{
	if ( i<0 || i>=GetSize() ) { addError("index outside array bounds"); return true; }

	mr[i] = x;
	return false;
}

bool cMatrix::SetSafeI(const int i, double x)
{
	if ( i<0 || i>=GetSize() || IsReal() ) { addError("index outside array bounds"); return true; }

	mi[i] = x;
	return false;
}

bool cMatrix::SetSafe(const int i, const int j, complex<double>& c)
{
	int idx = i + j*nr;
	if ( idx<0 || idx>=GetSize() ) { addError("index outside array bounds"); return true; }

	if (IsReal()) {
		NewImagDouble(GetSize());
		//if (mi==NULL) { addError("out of memory"); return true; }
		resetImag(GetSize());
	}

	mr[idx] = c.real();
	mi[idx] = c.imag();
	return false;
}

bool cMatrix::SetR(const int i, const int j, const cMatrix& A)
{
	int index;
	double dval;
	
	if ( (i+A.GetRows())>nr || (j+A.GetColumns())>nc )
		{ addError("index outside array bounds"); return true; }						// error

	const double *amr = A.GetSafeR();
	for (int k=i; k<(i+A.GetRows()); k++)
	{
		for (int l=j; l<(j+A.GetColumns()); l++)
		{
			index = (k-i) + (l-j)*A.GetRows();
			assert( index>=0 && index<A.GetSize() );
		
			dval = amr[index];
			SetR( k, l, dval );
		}
	}
	return false;
}

bool cMatrix::Set(const int i, const int j, const cMatrix& A)
{
	if (IsReal() && A.IsReal()) return SetR(i,j,A);

	if ( (i+A.GetRows())>nr || (j+A.GetColumns())>nc )
		{ addError("index outside array bounds"); return true; }						// error

	if (IsReal()) {
		NewImagDouble(A.GetSize());
		//if (mi==NULL) { addError("out of memory"); return true; }
		//memset( mi, 0, GetSize()*sizeof(double) );
		resetImag(GetSize());
	}

	int index;
	complex<double> cv;
	
	for (int k=i; k<(i+A.GetRows()); k++)
	{
		for (int l=j; l<(j+A.GetColumns()); l++)
		{
			index = (k-i) + (l-j)*A.GetRows();
			assert( index>=0 && index<A.GetSize() );
		
			A.Get(cv,index);
			Set( k, l, cv );
		}
	}
	return false;
}

bool cMatrix::SetR( std::vector<double>& A )  // assignment operator
{
	Delete();
	if (A.size()==0)
		return false;

	Create(1,A.size());

	// element-by-element copy... surely there is a better way?
	for (int i=0; i<(int)A.size(); ++i) mr[i] = A[i];

	return false;
}

bool cMatrix::SetTo(double d)		// set to real scalar
{
	if (Create(1,1)) return true;
	mr[0] = d;
	return false;
}

bool cMatrix::SetTo(complex<double> c)		// set to complex scalar
{
	if (CreateComplex(1,1)) return true;
	mr[0] = c.real();
	mi[0] = c.imag();
	return false;
}

bool cMatrix::SetTo(cMatrix& m)				// copy from another matrix
{
	return DeleteCopy(m);
}

// no arg checking here!
void cMatrix::SetR(const int i, const int j, double x)
{ mr[ i + j*nr] = x; }

void cMatrix::SetR(const int i, const double x)
{ mr[i] = x; }

void cMatrix::SetI(const int i, const int j, double x)
{ mi[ i + j*nr] = x; }

void cMatrix::SetI(const int i, double x)
{ mi[i] = x; }

void cMatrix::Set(const int i, const int j, complex<double>& c)
{
	int idx = i + j*nr;
	mr[idx] = c.real();
	mi[idx] = c.imag();
}

void cMatrix::Set(const int ix, complex<double>& c)
{
	mr[ix] = c.real();
	mi[ix] = c.imag();
}

// File I/O
//
bool cMatrix::SaveAscii(FILE *pf)
{
	int r, c;
	double lfr, lfi;

	if (IsReal()) {			// REAL values
		for (r=0; r<nr; ++r) {
			// first column
			GetR(lfr, r, 0);
			fprintf(pf, "%f", lfr);
			// additional columns
			for (c=1; c<nc; ++c) {
				GetR(lfr, r, c);
				fprintf(pf, ", %f", lfr);
			}
			fprintf(pf, "\n");		// end of line
		}
	}
	else {					// COMPLEX values
		for (r=0; r<nr; ++r) {
			// first column
			GetR(lfr, r, 0);
			GetI(lfi, r, 0);
			fprintf(pf, "%f+i%f", lfr, lfi);
			// additional columns
			for (c=1; c<nc; ++c) {
				GetR(lfr, r, c);
				GetI(lfi, r, c);
				fprintf(pf, ", %f+i%f", lfr, lfi);
			}
			fprintf(pf, "\n");		// end of line
		}
	}
	return false;
}

bool cMatrix::Save(FILE *pf)
{
	int r, c;
	double lfr, lfi;

	bool bReal = IsReal();

	fwrite(&bReal, sizeof(bool), 1, pf);
	fwrite(&nr, sizeof(int), 1, pf);
	fwrite(&nc, sizeof(int), 1, pf);

	if (IsReal()) {			// REAL values
		for (r=0; r<nr; ++r) {
			for (c=0; c<nc; ++c) {
				GetR(lfr, r, c);
				fwrite(&lfr, sizeof(double), 1, pf);
			}
		}
	}
	else {					// COMPLEX values
		for (r=0; r<nr; ++r) {
			for (c=1; c<nc; ++c) {
				GetR(lfr, r, c);
				GetR(lfi, r, c);
				fwrite(&lfr, sizeof(double), 1, pf);
				fwrite(&lfi, sizeof(double), 1, pf);
			}
		}
	}
	return false;
}

bool cMatrix::Load(FILE *pf)
{
	int r, c;
	double lfr, lfi;

	bool bReal = IsReal();

	fread(&bReal, sizeof(bool), 1, pf);
	fread(&nr, sizeof(int), 1, pf);
	fread(&nc, sizeof(int), 1, pf);

	if (bReal) {
		if (Create(nr, nc)) return true;
	}
	else {
		if (CreateComplex(nr, nc)) return true;
	}

	if (IsReal()) {			// REAL values
		for (r=0; r<nr; ++r) {
			for (c=0; c<nc; ++c) {
				fread(&lfr, sizeof(double), 1, pf);
				SetR(r, c, lfr);
			}
		}
	}
	else {					// COMPLEX values
		for (r=0; r<nr; ++r) {
			for (c=1; c<nc; ++c) {
				fread(&lfr, sizeof(double), 1, pf);
				fread(&lfi, sizeof(double), 1, pf);
				SetR(r, c, lfr);
				SetR(r, c, lfi);
			}
		}
	}
	return false;
}

// special set fcns
void cMatrix::forceReal()
{
	if (IsReal()) return;
	mi.clear();
}

bool cMatrix::forceComplex()
{
	if (!IsReal()) return false;
	NewImagDouble(GetSize());
	//if (mi==NULL) { addError("out of memory"); return true; }
	//if (memset( mi, 0, GetSize()*sizeof(double) )==NULL)
	//	return true;
	resetImag(GetSize());

	return false;
}

// manipulation, augmenting columns & rows
//
bool cMatrix::FillR(int pos, double *src, int len)
{
	int sz = GetSize();
	assert(pos+len <= sz);
	int end = pos+len-1;
	for (int i=pos; i<=end; ++i) mr[i] = src[i-pos];
	return false;
}

bool cMatrix::FillI(int pos, double *src, int len)
{
	int sz = mi.size();
	assert(pos+len <= sz);
	int end = pos+len-1;
	for (int i=pos; i<=end; ++i) mi[i] = src[i-pos];
	return false;
}

bool cMatrix::FillR(int pos, vector<double>::iterator src, int len)
{
	int sz = GetSize();
	assert(pos+len <= sz);
	int end = pos+len-1;
	for (int i=pos; i<=end; ++i, ++src)
		mr[i] = *(src);
	return false;
}

bool cMatrix::FillI(int pos, vector<double>::iterator src, int len)
{
	int sz = mi.size();
	assert(pos+len <= sz);
	int end = pos+len-1;
	for (int i=pos; i<=end; ++i, ++src)
		mi[i] = *(src);
	return false;
}

bool cMatrix::FillR(int pos, double val, int len)
{
	int sz = mr.size();
	assert(pos+len <= sz);
	int end = pos+len-1;
	for (int i=pos; i<=end; ++i) mr[i] = val;
	return false;
}

bool cMatrix::FillI(int pos, double val, int len)
{
	int sz = mi.size();
	assert(pos+len < sz);
	int end = pos+len-1;
	for (int i=pos; i<=end; ++i) mi[i] = val;
	return false;
}

bool cMatrix::FillColumnR(int icol, double *mat)
{
	if (icol<0 || icol>=nc)
		{ addError("index outside array bounds"); return true; }

	int idx = icol*nr;
	FillR(idx, mat, nr);
	//memcpy( mr+idx, mat, nr*sizeof(double) );

	return false;
}

bool cMatrix::AugmentColumnsR(cMatrix *A)			// add argument columns to *this
{
	// assumes *this and *A are both REAL valued only

	if (A->GetRows()==nr) {						// existing data is already aligned correctly!

		mr.insert( mr.end(), A->mr.begin(), A->mr.end() );

		//NewRealDouble( nr*(nc+A->GetColumns()) );
		//pbuf = new double[nr*(nc+A->GetColumns())];
		//if (pbuf==NULL) { addError("out of memory"); return true; }

		//memcpy( pbuf, mr, nr*nc*sizeof(double) );
		//offset = nr*nc;
		//nrows = nr; ncols = nc;
		//Delete();
		//mr = pbuf;
		//pbuf = NULL;

		nc += A->GetColumns();
		//if (memcpy( mr+offset, A->GetR(), A->GetSize()*sizeof(double) )==NULL)
		//	{ addError("out of memory"); return true; }
		//nc = ncols;
	}
	else if (nr==0) {
		if (Create(A->GetRows(),A->GetColumns())) return true;
		SetTo(*A);
		//SetR(0, 0, *A);
	}
	else { addError("size mismatch"); return true; }				// size mis-match

	return false;
}

bool cMatrix::AugmentColumnsI(cMatrix *A)			// add argument columns to *this
{
	if (A->mi.empty()) return false;

	if (A->GetRows()==nr) {						// existing data is already aligned correctly!
		if (IsReal()) {
			mi.reserve(GetSize());
			FillI(0, 0., A->GetSize());
		}
		mi.insert( mi.end(), A->mi.begin(), A->mi.end() );
		nc += A->GetColumns();
	}
	else if (nr==0) {
		SetTo(*A);
	}
	else { addError("size mismatch"); return true; }				// size mis-match

	return false;
}


bool cMatrix::AugmentColumns(cMatrix *A)			// add argument columns to *this
{
	if (IsReal() && A->IsReal()) return AugmentColumnsR(A);

	if (IsReal()) {
		NewImagDouble(GetSize());
		resetImag(GetSize());
		//if (mi==NULL) { addError("out of memory"); return true; }
		//memset( mi, 0, GetSize()*sizeof(double) );
	}

	if (A->GetRows()==nr) {						// existing data is already aligned correctly!
		NewRealDouble( nr*(nc+A->GetColumns()) );
		//if (pbufR==NULL) { addError("out of memory"); return true; }
	
		NewImagDouble( nr*(nc+A->GetColumns()) );
		//if (pbufI==NULL) { delete []pbufR; addError("out of memory"); return true; }

		//memcpy( pbufR, mr, nr*nc*sizeof(double) );
		//memcpy( pbufI, mi, nr*nc*sizeof(double) );

		//offset = nr*nc;
		//nrows = nr; ncols = nc;
		//Delete();
		//mr = pbufR;
		//mi = pbufI;
		//pbufR = pbufI = NULL;

		if (AugmentColumnsR( A )) return true;
		if (!IsReal() || !A->IsReal()) return AugmentColumnsI( A );

		//ncols += A->GetColumns();
		//if (memcpy( mr+offset, A->GetR(), A->GetSize()*sizeof(double) )==NULL)
		//	{ addError("out of memory"); return true; }

		//if (A->IsReal()) { memset( mi+offset, 0, A->GetSize()*sizeof(double) ); }
		//else { if (memcpy( mi+offset, A->GetI(), A->GetSize()*sizeof(double) )==NULL) { addError("out of memory"); return true; } }

		//nr = nrows;
		//nc = ncols;
	}
	else if (nr==0) {
		if (CreateComplex(A->GetRows(),A->GetColumns())) return true;
		Set(0, 0, *A);
	}
	else { addError("size mismatch"); return true; }				// size mis-match

	return false;
}



bool cMatrix::AugmentRowsR(cMatrix *A)			// add argument columns to *this
{
	// assumes *this and *A are both REAL valued only

	int k, offset;
	int new_rows = nr + A->GetRows();
	vector<double> aug_mat;

	if (A->GetColumns()==nc) {

		aug_mat.reserve( new_rows*nc );

		//pbuf = new double[new_rows*nc];
		//if (pbuf==NULL) { addError("out of memory"); return true; }

		offset = nr;
		for (k=0; k<nc; k++) {				// fill zeros below, if necessary

			aug_mat.insert( aug_mat.begin()+(k*new_rows), mr.begin()+(k*nr), mr.begin()+(k+1)*nr );
			//src = (void *)(mr+k*nr);
			//dest = (void *)(pbuf+k*new_rows);
			//memcpy( dest, src, nr*sizeof(double) );		// copy current matrix values
			aug_mat.insert( aug_mat.begin()+(k*new_rows+nr), A->mr.begin()+(k*A->nr), A->mr.begin()+(k+1)*A->nr );
			//src = (void *)(A->mr+k*A->nr);
			//dest = (void *)(pbuf+k*new_rows+nr);
			//memcpy( dest, src, A->GetRows()*sizeof(double) );				// copy augmented rows
		}
		nr += A->GetRows();
		mr = aug_mat;
		//delete []mr;
		//mr = pbuf;
		//pbuf = NULL;
	}
	else if (nc==0) {			// if *this is empty, just set to arg
		if (Create(A->GetRows(),A->GetColumns())) return true;
		if (Copy(*A)) return true;
	}
	else { addError("size mismatch"); return true; }				// size mis-match

	return false;				// no error
}

bool cMatrix::AugmentRows(cMatrix *A)			// add argument columns to *this
{
	if (IsReal() && A->IsReal()) return AugmentRowsR(A);

	int k, offset;

	if (IsReal()) {
		mi.reserve( GetSize() );
		for (k=0; k<GetSize(); ++k) mi[k] = 0.;
		//mi = new double[GetSize()];
		//if (mi==NULL) { addError("out of memory"); return true; }
		//memset( mi, 0, GetSize()*sizeof(double) );
	}

	int new_rows = nr + A->GetRows();
	vector<double> aug_mat_r, aug_mat_i;

	if (A->GetColumns()==nc) {

		aug_mat_r.reserve( new_rows*nc );
		aug_mat_i.reserve( new_rows*nc );

		offset = nr;
		for (k=0; k<nc; k++) {				// fill zeros below, if necessary
			aug_mat_r.insert( aug_mat_r.begin()+k*new_rows, mr.begin()+k*nr, mr.begin()+(k+1)*nr-1 );
			aug_mat_r.insert( aug_mat_r.begin()+k*new_rows+nr, A->mr.begin()+k*A->nr, A->mr.begin()+(k+1)*A->nr-1 );

			if (A->IsReal()) {
				FillI(k*new_rows+nr,0.,A->nr);
			}
			else {
				aug_mat_i.insert( aug_mat_i.begin()+k*new_rows, mi.begin()+k*nr, mi.begin()+(k+1)*nr-1 );
				aug_mat_i.insert( aug_mat_i.begin()+k*new_rows+nr, A->mi.begin()+k*A->nr, A->mi.begin()+(k+1)*A->nr-1 );
			}

			//src = (void *)(mr+k*nr);
			//dest = (void *)(pbufR+k*new_rows);
			//memcpy( dest, src, nr*sizeof(double) );		// copy current matrix values
			//src = (void *)(mi+k*nr);
			//dest = (void *)(pbufI+k*new_rows);
			//memcpy( dest, src, nr*sizeof(double) );		// copy current matrix values

			//src = (void *)(A->mr+k*A->nr);
			//dest = (void *)(pbufR+k*new_rows+nr);
			//memcpy( dest, src, A->GetRows()*sizeof(double) );				// copy augmented rows
			//if (A->IsReal()) {
			//	memset( dest, 0, A->GetRows()*sizeof(double) );
			//}
			//else {
			//	src = (void *)(A->mr+k*A->nr);
			//	dest = (void *)(pbufR+k*new_rows+nr);
			//	memcpy( dest, src, A->GetRows()*sizeof(double) );				// copy augmented rows
			//}
		}
		nr += A->GetRows();
		//delete []mr;
		//delete []mi;
		//mr = pbufR;
		//mi = pbufI;
		//pbufR = pbufI = NULL;
	}
	else if (nc==0) {			// if *this is empty, just set to arg
		if (CreateComplex(A->GetRows(),A->GetColumns())) return true;
		if (Copy(*A)) return true;
	}
	else { addError("size mismatch"); return true; }				// size mis-match

	return false;				// no error
}

bool cMatrix::GetColumnR(cMatrix *Result, int c)
{
	if ( (c<0)||(c>=nc) ) { addError("index outside array bounds"); return true; }

	if (Result->Create(nr,1)) return true;

	int pos = c*nr;
	Result->nr = nr;
	Result->nc = 1;
	Result->mr.clear();
	Result->mr.insert( Result->mr.begin(), mr.begin()+pos, mr.begin()+pos+nr );
	return false;
}

bool cMatrix::GetColumnI(cMatrix *Result, int c)
{
	if ( (c<0)||(c>=nc) ) { addError("index outside array bounds"); return true; }

	int pos = c*nr;

	if (IsReal()) {
		return Result->FillI(0, 0., nr);
	}
	else {
		Result->nr = nr;
		Result->nc = 1;
		Result->mi.clear();
		Result->mi.insert( Result->mi.begin(), mi.begin()+pos, mi.begin()+pos+nr );
		return false;
	}
}

bool cMatrix::GetColumn(cMatrix *Result, int c)
{
	if ( (c<0)||(c>=nc) ) { addError("index outside array bounds"); return true; }

	if (GetColumnR(Result,c)) return true;

	if (IsReal()) {	return false; }
	else { return GetColumnI(Result, c); }
}

bool cMatrix::GetRowR(cMatrix *Result, int r)
{
	if ( (r<0)||(r>=nr) ) { addError("index outside array bounds"); return true; }

	if (Result->Create(1,nc)) return true;

	for (int i=0; i<nc; ++i)
	{
		Result->mr[i] = mr[nr*i + r];
	}
	return false;
}

bool cMatrix::GetRowI(cMatrix *Result, int r)
{
	if ( (r<0)||(r>=nr) ) { addError("index outside array bounds"); return true; }

	if (IsReal()) {
		return Result->FillI(0, 0., nc);
	}
	else {
		Result->nr = 1;
		Result->nc = nc;
		Result->mi.clear();
		Result->mi.insert( Result->mi.begin(), nc, 0. );
		for (int i=0; i<nc; ++i)
		{
			Result->mi[i] = mi[nr*i + r];
		}
		return false;
	}
}

bool cMatrix::GetRow(cMatrix *Result, int r)
{
	if (GetRowR(Result,r)) return true;

	if (IsReal()) {	return false; }
	else { return GetRowI(Result, r); }
}

bool cMatrix::SwapRows(int r1, int r2)
{
	if ( r1<0 || r1>=nr || r2<0 || r2>=nr ) { addError("index outside array bounds"); return true; }

	double lf;
	int ic, ix1, ix2;
	complex<double> cc1, cc2;

	if (IsReal())
	{
		for (ic=0; ic<nc; ic++) {
			ix1 = r1 + ic*nr;
			ix2 = r2 + ic*nr;
			lf = mr[ix1];
			SetR( ix1, mr[ix2] );
			SetR( ix2, lf );
		}
	}
	else {
		for (ic=0; ic<nc; ic++) {
			ix1 = r1 + ic*nr;
			ix2 = r2 + ic*nr;
			cc1 = complex<double>(mr[ix1],mi[ix1]);
			cc2 = complex<double>(mr[ix2],mi[ix2]);
			Set( ix1, cc2 );
			Set( ix2, cc1 );
		}
	}

	return false;
}

//
// Classification
//
bool cMatrix::IsVector(int *iWhich) const	// if true, int arg is 0 for scalar, 1 col vector, 2 for row vector
{
	if (nr*nc==1) {
		*iWhich = 0;
		return true;
	}
	else if (nc==1) {
		*iWhich = 1;		// column vector
		return true;
	}
	else if (nr==1) {		// row vector
		*iWhich = 2;
		return true;
	}
	else {
		*iWhich = 3;		// non-vector matrix
		return false;
	}
}

bool cMatrix::IsVector() const
{
	int iw;
	return IsVector(&iw);
}

bool cMatrix::IsScalar(void) const
{
	return (nr*nc==1);
}

bool cMatrix::IsFinite(void) const
{
	int i;
	int n = GetSize();

	for (i=0; i<n; i++) {
		if ( !( std::isfinite(mr[i]) ) ) return false;
	}
	if (!IsReal()) {	// check imag
		for (i=0; i<n; i++) {
			if ( !( std::isfinite(mi[i]) ) ) return false;
		}
	}
	return true;		// everything finite
}

bool cMatrix::IsFinite(cMatrix& ret) const
{
	int i;
	int n = GetSize();

	ret.Delete();
	if (ret.Create(nr,nc)) {
		addError("memory error");
		return true;
	}

	if (IsReal()) {
		for (i=0; i<n; i++) {
			ret.SetR(i, (double)std::isfinite(mr[i]));
		}
	}
	else {
		for (i=0; i<n; i++) {
			ret.SetR(i, (double)( std::isfinite(mr[i]) && std::isfinite(mi[i])) );
		}
	}

	return false;
}

bool cMatrix::IsNan(cMatrix& ret) const
{
	int i;
	int n = GetSize();

	ret.Delete();
	if (ret.Create(nr,nc)) {
		addError("memory error");
		return true;
	}

	bool b(false);
	double bval(0.);

	if (IsReal()) {
		for (i=0; i<n; i++) {
			b = std::isnan<double>(mr[i]);
			bval = (double) b;
			ret.SetR(i, bval);
		}
	}
	else {
		for (i=0; i<n; i++) {
			b = std::isnan<double>(mr[i]) || std::isnan<double>(mi[i]);
			bval = (double)b;
			ret.SetR(i, bval);
		}
	}

	return false;
}
//
// modulo
//
bool cMatrix::Mod(double& d)
{
	if (IsEmpty()) { addError("empty matrix"); return true; }

	for (int i=0; i<nr*nc; ++i)
		mr[i] = fmod( mr[i], d );

	return false;
}

bool cMatrix::Mod(complex<double>& c)
{
	if (IsEmpty()) { addError("empty matrix"); return true; }

	for (int i=0; i<nr*nc; ++i)
		mr[i] = fmod( mr[i], c.real() );

	return false;
}

//
// transpose, hermitian, reshape, rot90
//
bool cMatrix::Herm(void) {
	int i, j, idx, temp;
	vector<double> mh, mih;
	bool bComplex;

	// check for empty matrix
	if (GetSize()==0) return false;

	mh.reserve( GetSize() );
	bComplex = !IsReal();

	if ( bComplex  )
	{
		mih.reserve( GetSize() );
	}

	if (bComplex)
	{
		for (i=0; i<nc; i++) {
			for (j=0; j<nr; j++) {
				idx = i+j*nc;
				assert( idx>=0 && idx<nr*nc );
				mh[idx] = mr[j+i*nr];
				mih[idx] = - mi[j+i*nr];
			}
		}
	}
	else
	{
		// just real matrix (transpose)
		for (i=0; i<nc; i++) {
			for (j=0; j<nr; j++) {
				idx = i+j*nc;
				assert( idx>=0 && idx<nr*nc );
				mh[idx] = mr[j+i*nr];
			}
		}
	}

	mr = mh;
	if (bComplex) mi = mih;

	// swap rows/cols
	temp = nr;
	nr = nc;
	nc = temp;

	return false;
}

bool cMatrix::Trans(void) {
	int i, j, idx, temp;
	vector<double> mh, mih;
	bool bComplex;
	int sz = nr*nc;

	// check for empty matrix
	if (sz==0) return false;

	mh.reserve( sz );
	mh.insert( mh.begin(), sz, 0. );

	if ( bComplex = !IsReal() )
	{
		mih.reserve( nr*nc );
		mih.insert( mih.begin(), sz, 0. );
	}

	if (bComplex)
	{
		for (i=0; i<nc; i++) {
			for (j=0; j<nr; j++) {
				idx = i+j*nc;
				assert( idx>=0 && idx<nr*nc );
				mh[idx] = mr[j+i*nr];
				mih[idx] = mi[j+i*nr];
			}
		}
	}
	else
	{
		// just real matrix (transpose)
		for (i=0; i<nc; i++) {
			for (j=0; j<nr; j++) {
				idx = i+j*nc;
				assert( idx>=0 && idx<nr*nc );
				mh[idx] = mr[j+i*nr];
			}
		}
	}

	mr = mh;
	if (bComplex) mi = mih;

	// swap rows/cols
	temp = nr;
	nr = nc;
	nc = temp;

	return false;
}


bool cMatrix::Reshape(int Nr, int Nc)
{
	if (nr*nc != Nr*Nc) { addError("incompatable matrix dimensions"); return true; }		// incompatable matrix dimensions

	nr = Nr;
	nc = Nc;

	return false;
}

bool cMatrix::Rot90(double dRot)
{	// rotate R by dRot 90 deg CCW turns
	return Rot90(*this, dRot);
}

bool cMatrix::Rot90(cMatrix& R, double dRot)
{	// rotate R by dRot 90 deg CCW turns
	double dNew = fmod(dRot, 4.);		// dRot Mod 4, rounded to nearest integer
	double dd;

	if (dNew<0.) dNew += 4.;			// dNew is now a positive remainder
	
	int iRot = int(dNew);
	int iCount = 0, r, c, itemp;

	if (R.Create(nr,nc)) return true;

	if (IsOdd(iRot)) {
		itemp = R.GetRows();
		R.SetRows( R.GetColumns() );
		R.SetColumns( itemp );
	}

	switch (iRot) {
	case 0:
		R = (*this); return false;		// NOP
	case 1:
		for (r=0; r<nr; r++) {
			for (c=nc-1; c>=0; c--) {
				GetR(dd,r,c);
				R.SetR(iCount,dd);
				iCount++;
			}
		}
		return false;
	case 2:
		for (c=nc-1; c>=0; c--) {
			for (r=nr-1; r>=0; r--) {
				GetR(dd,r,c);
				R.SetR(iCount,dd);
				iCount++;
			}
		}
		return false;
	case 3:
		for (r=nr-1; r>=0; r--) {
			for (c=0; c<nc; c++) {
				GetR(dd,r,c);
				R.SetR(iCount,dd);
				iCount++;
			}
		}
		return false;
	case 4:
		R = (*this); return false;
	default:
		{ addError("rot90: internal error"); return true; }	// should never happen
	};
}

// get maximum value of matrix (farthest right on real line)
bool cMatrix::MaxR(double& dmax)
{
	if (IsEmpty()) { addError("empty matrix"); return true; }

	double d;
	GetR(dmax,0,0);
	int n = nr*nc;

	for (int i=0; i<n; i++) {
		d = mr[i];
		if (dmax<d) { dmax=d; }
	}

	return false;
}

bool cMatrix::MaxNumberR(double& dmax)
{
	if (IsEmpty()) return false;

	// initialize max to most negative number
	dmax = -DBL_MAX;

	double d;
	bool bCheck = true;
	int n = nr*nc;

	for (int i=0; i<n; i++) {
		d = mr[i];
		if ( std::isfinite(d) ) {
				bCheck = false;
				if (dmax<d) { dmax=d; }
		}
	}

	return bCheck;
}

bool cMatrix::MaxVectR(cMatrix& val)
{
	if (val.Create(1,nc)) return true;

	double d, v;
	int i, j;

	for (j=0; j<nc; j++) { GetR(d,0,j); val.SetR(0,j,d); }

	for (j=0; j<nc; j++) {
		val.GetR(v,0,j);
		for (i=1; i<nr; i++) {
			d = mr[i+j*nr];
			if (v<d) { v = d; }
		}
		val.SetR(0,j,v);
	}
	return false;
}

bool cMatrix::MaxIndexR(double& dmax, double& Idx)
{
	if (IsEmpty()) { addError("index outside array bounds"); return true; }

	double dval;
	GetR(dval,0,0);
	Idx = 1.;

	for (int i=0; i<nr*nc; i++) {
		if (dval<mr[i]) { dval=mr[i]; Idx = double(i); }
	}
	return false;
}

bool cMatrix::MaxVectIndexR(cMatrix& dmax, cMatrix& Idx)
{
	if (IsEmpty()) { addError("index outside array bounds"); return true; }

	int r;		// row of max value
	int i, j;
	double d, v;

	if (Idx.Create(1,nc)) return true;
	if (dmax.Create(1,nc)) return true;

	// initialize max values
	for (j=0; j<nc; j++) {
		GetR(d,0,j);
		dmax.SetR( 0, j, d );
		Idx.SetR( 0, j, 0 );
	}

	for (j=0; j<nc; j++) {
		dmax.GetR(v,0,j);
		for (i=1; i<nr; i++) {

			d = mr[i+j*nc];
			if (v<d) {
				v = d;
				r=i;
			}

		}
		Idx.SetR(0,j, double(r));
		dmax.SetR(0,j, v);
	}
	return false;
}

// get minimum value of matrix (farthest left on real line)
bool cMatrix::MinR(double& dmin)
{
	if (IsEmpty()) { addError("empty matrix"); return true; }

	double d = Get0R();
	int n = nr*nc;

	for (int i=0; i<n; i++) {
		d = mr[i];
		if (dmin>d) { dmin=d; }
	}
	return false;
}

bool cMatrix::MinPosNumberR(double& dmin)
{
	if (IsEmpty()) return false;

	dmin = DBL_MAX;
	bool bCheck = true;
	double d;
	int n = nr*nc;

	for (int i=0; i<n; i++) {
		d = mr[i];
		if ( std::isfinite(d) && d>0. ) {
				bCheck = false;
				if (dmin>d) { dmin=d; }
		}
	}

	return bCheck;
}

bool cMatrix::MinNumberR(double& dmin)
{
	if (IsEmpty()) return false;

	dmin = DBL_MAX;
	bool bCheck = true;
	double d;
	int n = nr*nc;

	for (int i=0; i<n; i++) {
		d = mr[i];
		if ( std::isfinite(d)!=0 ) {
				bCheck = false;
				if (dmin>d) { dmin=d; }
		}
	}

	return bCheck;
}

bool cMatrix::MinVectR(cMatrix& val)
{
	if (val.Create(1,nc)) return true;

	double d, v;
	int i, j;

	for (j=0; j<nc; j++) { GetR(d,0,j); val.SetR(0,j,d); }

	for (j=0; j<nc; j++) {
		val.GetR(v,0,j);
		for (i=1; i<nr; i++) {
			d = mr[i+j*nc];
			if (v>d) { v = d; }
		}
		val.SetR(0,j,v);
	}
	return false;
}

bool cMatrix::MinIndexR(double& dmin, double& Idx)
{
	if (IsEmpty()) { addError("index outside array bounds"); return true; }

	double dval;
	GetR(dval,0,0);
	Idx = 1.;

	for (int i=0; i<nr*nc; i++) {
		if (dval>mr[i]) { dval=mr[i]; Idx = double(i); }
	}
	return false;
}

bool cMatrix::MinVectIndexR(cMatrix& dmin, cMatrix& Idx)
{
	if (IsEmpty()) { addError("index outside array bounds"); return true; }

	int r;			// row of max value
	int i, j;
	double d, v;

	if (Idx.Create(1,nc)) return true;
	if (dmin.Create(1,nc)) return true;

	// initialize max values
	for (j=0; j<nc; j++) {
		d = mr[j*nc];
		dmin.SetR( 0, j, d );
		Idx.SetR( 0, j, 0 );
	}

	for (i=1; i<nr; i++) {
		dmin.GetR(v,0,j);
		for (j=0; j<nc; j++) {
			d = mr[i+j*nc];
			if (v>d) {
				v = d;
				r=i;
			}
		}
		Idx.SetR(0,j, double(r));
		dmin.SetR(0,j, v);
	}
	return false;
}

// set i^th row to (double)i.
void cMatrix::SetColumnsToCount(void)
{
	if (!IsReal()) { mi.clear(); }

	int i, j;
	for (i=0; i<nr; i++) {
		for (j=0; j<nc; j++) {
			mr[i+j*nr] = double(i);
		}
	}
	return;
}

bool cMatrix::SetToCount(int n)
{
	if (Create(1,n)) return true;

	for (int i=0; i<n; i++) { mr[i] = double(i); }
	return false;
}

void cMatrix::SetToCount()
{
	if (!IsReal()) { mi.clear(); }

	int n = GetSize();
	for (int i=0; i<n; i++) { mr[i] = double(i); }
}

bool cMatrix::SetToRange(double dStart, int n)
{
	if (Create(1,n)) return true;

	double lf = dStart;
	for (int i=0; i<n; i++) { mr[i] = lf + double(i); }
	return false;
}

bool cMatrix::SetToRange(double dStart, double dEnd, double dIncr)
{
	int n = (int) ((dEnd-dStart)/dIncr) + 1;
	if (n<0) { addError("illegal range operands"); return true; }

	if (Create(1,n)) return true;

	double lf = dStart;
	for (int i=0; i<n; i++) { mr[i] = lf; lf += dIncr; }
	return false;
}

//
// flip
//
void cMatrix::FlipudR()
{
	int half_rows = int( nr/2 );
	double dv1, dv2;

	for (int c=0; c<nc; c++) {
		for (int r=0; r<half_rows; r++) {
			GetR(dv1,nr-1-r,c);
			GetR(dv2,r,c);
			SetR( nr-1-r, c, dv2 );		// swap
			SetR( r, c, dv1 ); 
		}
	}
}

void cMatrix::Flipud()
{
	if (IsReal()) return FlipudR();

	int half_rows = int( nr/2 );
	complex<double> cv1, cv2;

	for (int c=0; c<nc; c++) {
		for (int r=0; r<half_rows; r++) {
			Get(cv1,nr-1-r,c);
			Get(cv2,r,c);
			Set( nr-1-r, c, cv2 );		// swap
			Set( r, c, cv1 ); 
		}
	}
}

void cMatrix::FliplrR()
{
	double dv1, dv2;
	int half_cols = int( nc/2 );

	for (int r=0; r<nr; r++) {
		for (int c=0; c<half_cols; c++) {
			GetR( dv1, r, nc-1-c);
			GetR( dv2, r, c);
			SetR( r, nc-1-c, dv2 );		// swap
			SetR( r, c, dv1 );
		}
	}
}

void cMatrix::Fliplr()
{
	if (IsReal()) return FliplrR();

	complex<double> cv1, cv2;
	int half_cols = int( nc/2 );

	for (int r=0; r<nr; r++) {
		for (int c=0; c<half_cols; c++) {
			Get( cv1, r, nc-1-c);
			Get( cv2, r, c);
			Set( r, nc-1-c, cv2 );		// swap
			Set( r, c, cv1 );
		}
	}
}

//
// sum, prod
//

bool cMatrix::SumSquare(cMatrix& Result, int idim)
{
	if (IsEmpty()) { addError("empty matrix"); return true; }

	int r, c;
	double rsum = 0.;

	// check for vector case
	if (IsVector()) {
		if (IsReal()) {
			Result.Create(1,1);
			for (c=0; c<nr*nc; c++) rsum += mr[c] * mr[c];
			Result.SetR(0, rsum);
		}
		else {
			Result.Create(1,1);
			for (c=0; c<nr*nc; c++) { rsum += mr[c]*mr[c] + mi[c]*mi[c]; }
			Result.SetR(0, rsum);
		}
		return false;
	}
	
	int ix;

	if (idim==2) {		// sum across rows

		if (IsReal())
		{
			if (Result.Create(nr,1)) return true;
			for (r=0; r<nr; r++) {
				rsum = 0.;
				for (c=0; c<nc; c++) {
					ix = r+c*nr;
					rsum += mr[ix] * mr[ix];
				}
				Result.SetR(r, rsum);
			}
		}
		else {
			if (Result.Create(nr,1)) return true;
			for (r=0; r<nr; r++) {
				rsum = 0.;
				for (c=0; c<nc; c++) {
					ix = r+c*nr;
					rsum += mr[ix] * mr[ix] + mi[ix] * mi[ix];
				}
				Result.SetR(r, rsum);
			}
		}

	}
	else if (idim==1) {	// sum down columns

		if (IsReal())
		{
			if (Result.Create(1,nc)) return true;

			for (c=0; c<nc; c++) {
 				rsum = 0.;
				for (r=0; r<nr; r++) { ix = r+c*nr; rsum += mr[ix] * mr[ix]; }
				Result.SetR(0, c, rsum);
			}
		}
		else {
			if (Result.Create(1,nc)) return true;
			
			for (c=0; c<nc; c++) {
 				rsum = 0.;
				for (r=0; r<nr; r++) {
					ix = r+c*nr;
					rsum += mr[ix] * mr[ix] + mi[ix] * mi[ix];
				}
				Result.SetR(0, c, rsum);
			}
		}	// if IsReal()

	}
	else { addError("invalid dimension indicator"); return true; }		// invalid idim

	return false;
}

bool cMatrix::Sum(cMatrix& Result, int idim)
{
	if (IsEmpty()) { addError("empty matrix"); return true; }

	int r, c;
	double rsum = 0., isum = 0.;
	complex<double> cval;

	// check for vector case
	if (IsVector()) {
		if (IsReal()) {
			Result.Create(1,1);
			for (c=0; c<nr*nc; c++) rsum += mr[c];
			Result.SetR(0, rsum);
		}
		else {
			Result.CreateComplex(1,1);
			for (c=0; c<nr*nc; c++) { rsum += mr[c]; isum += mi[c]; }
			cval = complex<double>(rsum,isum);
			Result.Set(0, cval);
		}
		return false;
	}
	
	int ix;

	if (idim==2) {		// sum across rows

		if (IsReal())
		{
			if (Result.Create(nr,1)) return true;
			for (r=0; r<nr; r++) {
				rsum = 0.;
				for (c=0; c<nc; c++) { rsum += mr[r+c*nr]; }
				Result.SetR(r, rsum);
			}
		}
		else {
			if (Result.CreateComplex(nr,1)) return true;
			for (r=0; r<nr; r++) {
				rsum = isum = 0.;
				for (c=0; c<nc; c++) { ix = r+c*nr; rsum += mr[ix]; isum += mi[ix]; }
				cval = complex<double>(rsum,isum);
				Result.Set(r, cval);
			}
		}

	}
	else if (idim==1) {	// sum down columns

		if (IsReal())
		{
			if (Result.Create(1,nc)) return true;

			for (c=0; c<nc; c++) {
 				rsum = 0.;
				for (r=0; r<nr; r++) { rsum += mr[r+c*nr]; }
				Result.SetR(0, c, rsum);
			}
		}
		else {
			if (Result.CreateComplex(1,nc)) return true;
			
			for (c=0; c<nc; c++) {
 				rsum = isum = 0.;
				for (r=0; r<nr; r++) {
					ix = r+c*nr;
					rsum += mr[ix];
					isum += mi[ix];
				}
				cval = complex<double>(rsum,isum);
				Result.Set(0, c, cval);
			}
		}	// if IsReal()

	}
	else { addError("invalid dimension indicator"); return true; }		// invalid idim

	return false;
}

double cMatrix::StraightSumR(void)
{	// sum total matrix
	double dsum = 0.;
	int n = nr*nc;

	for (int i=0; i<n; ++i) { dsum += mr[i]; }
	return dsum;
}

complex<double> cMatrix::StraightSumC(void)
{	// sum total matrix
	complex<double> dsum(0.,0.);
	int n = nr*nc;

	for (int i=0; i<n; ++i) { dsum += complex<double>(mr[i],mi[i]); }
	return dsum;
}

bool cMatrix::Prod(cMatrix& Result, int idim)
{
	int r, c;
	double dprod = 1., dd;
	complex<double> cval;

	if (IsReal())
	{
		// check for vector case
		if (IsVector()) {	// vector
			Result.Create(1,1);
			for (c=0; c<nr*nc; c++) { dprod *= mr[c]; }
			Result.SetR(0,0, dprod);
			return false;
		}
	}
	else {
		// check for vector case
		if (IsVector()) {	// vector
			Result.CreateComplex(1,1);
			cval = complex<double>(1.,0.);
			for (c=0; c<nr*nc; c++) { cval *= complex<double>(mr[c],mi[c]); }
			Result.Set(0,0, cval);
			return false;
		}	}

	if (idim==1) {	// prod across rows

		if (Result.Create(nr,1)) return true;
		for (r=0; r<nr; r++) {
			dprod = 1.;
			for (c=0; c<nc; c++) { GetR(dd,r,c); dprod *= dd; }
			Result.SetR(r, 0, dprod);
		}

	}
	else {			// prod down columns

		if (Result.Create(1,nc)) return true;
		for (c=0; c<nc; c++) {
			dprod = 1.;
			for (r=0; r<nr; r++) { GetR(dd,r,c); dprod *= dd; }
			Result.SetR(0, c, dprod);
		}

	}

	return false;
}

bool cMatrix::CumSumR(cMatrix& Result, int idim)
{
	int r, c, ix;
	if (Result.Create(nr,nc)) return true;

	double dsum = 0.;

	// check for vector case
	if (IsVector()) {	// vector
		for (c=0; c<nr*nc; c++) {
			dsum += mr[c];
			Result.SetR(c,dsum);
		}
		return false;
	}

	if (idim==1) {		// sum across rows

		for (r=0; r<nr; r++) {
			dsum = 0.;
			for (c=0; c<nc; c++) {
				ix = r + c*nr;
				dsum += mr[ix];
				Result.SetR(ix, dsum);
			}
		}

	}
	else {				// sum down columns

		for (c=0; c<nc; c++) {
			dsum = 0.;
			for (r=0; r<nr; r++) {
				ix = r + c*nr;
				dsum += mr[ix];
				Result.SetR(ix, dsum);
			}
		}

	}

	return false;
}

bool cMatrix::CumSum(cMatrix& Result, int idim)
{
	if (IsReal()) return CumSumR(Result, idim);

	int r, c, ix;
	if (Result.CreateComplex(nr,nc)) return true;

	complex<double> csum(0.,0.), dd;

	// check for vector case
	if (IsVector()) {	// vector
		for (c=0; c<nr*nc; c++) {
			Get(dd, c);
			csum += dd;
			Result.Set(c,csum);
		}
		return false;
	}

	if (idim==1) {		// sum across rows

		for (r=0; r<nr; r++) {
			csum = complex<double>(0.,0.);
			for (c=0; c<nc; c++) {
				ix = r + c*nr;
				Get(dd, ix);
				csum += dd;
				Result.Set(ix, csum);
			}
		}

	}
	else {				// sum down columns

		for (c=0; c<nc; c++) {
			csum = complex<double>(0.,0.);
			for (r=0; r<nr; r++) {
				ix = r + c*nr;
				Get(dd, ix);
				csum += dd;
				Result.Set(ix, csum);
			}
		}

	}

	return false;
}

bool cMatrix::CumProdR(cMatrix& Result, int idim)
{
	int r, c, ix;
	if (Result.Create(nr,nc)) return true;

	double dprod = 1.;

	// check for vector case
	if (IsVector()) {	// vector
		for (c=0; c<nr*nc; c++) {
			dprod *= mr[c];
			Result.SetR(c,dprod);
		}
		return false;
	}

	if (idim==1) {		// mult across rows

		for (r=0; r<nr; r++) {
			dprod = 1.;
			for (c=0; c<nc; c++) {
				ix = r + c*nr;
				dprod *= mr[ix];
				Result.SetR(ix, dprod);
			}
		}

	}
	else {				// mult down columns

		for (c=0; c<nc; c++) {
			dprod = 1.;
			for (r=0; r<nr; r++) {
				ix = r + c*nr;
				dprod *= mr[ix];
				Result.SetR(ix, dprod);
			}
		}

	}

	return false;
}

bool cMatrix::CumProd(cMatrix& Result, int idim)
{
	if (IsReal()) return CumProdR(Result,idim);

	int r, c, ix;
	if (Result.CreateComplex(nr,nc)) return true;

	complex<double> cprod(1.,0.), dd;

	// check for vector case
	if (IsVector()) {	// vector
		for (c=0; c<nr*nc; c++) {
			Get(dd,c);
			cprod *= dd;
			Result.Set(c,cprod);
		}
		return false;
	}

	if (idim==1) {		// mult across rows

		for (r=0; r<nr; r++) {
			cprod = 1.;
			for (c=0; c<nc; c++) {
				ix = r + c*nr;
				Get(dd,ix);
				cprod *= dd;
				Result.Set(ix, cprod);
			}
		}

	}
	else {				// mult down columns

		for (c=0; c<nc; c++) {
			cprod = complex<double>(1.,0.);
			for (r=0; r<nr; r++) {
				ix = r + c*nr;
				Get(dd,ix);
				cprod *= dd;
				Result.Set(ix, cprod);
			}
		}

	}

	return false;
}

//
// find
//

bool cMatrix::FindR(cMatrix& Result)
{
	vector<double> v;
	vector<int> vix;

	int n = GetSize();

	for (int i=0; i<n; ++i)
	{
		if (mr[i] != 0.) { v.push_back( double(i) ); vix.push_back(i); }
	}

	if (IsVector()) {

		if (nr==1) {	// row vector (or scalar)
			if (Result.CreateRow( v )) return true;
		}
		else {			// column vector
			if (Result.CreateColumn( v )) return true;
		}

	}
	else {		// nr-by-nc matrix
		if (Result.CreateColumn( v )) return true;
	}

	return false;
}

bool cMatrix::Find(cMatrix& Result, cMatrix& iRows, cMatrix& iCols)
{
	cMatrix Temp;
	Temp.Create(1,1);

	cMatrix NewIndex;
	NewIndex.Create(1,1);

	double dd;

	for (int r=0; r<nr; r++) {
		for (int c=0; c<nc; c++) {
			GetR(dd,r,c);
			if (dd!=0.) {
				Temp.SetR(0,0,dd);
				Result.AugmentRows(&Temp);
				NewIndex.SetR(0,0, double(r) );
				if (iRows.AugmentRows(&NewIndex)) return true;
				NewIndex.SetR(0,0, double(c) );
				if (iCols.AugmentRows(&NewIndex)) return true;
			}
		}
	}

	return false;
}

//
// sort
//
bool cMatrix::Sort(cMatrix& Result)
{
	if (IsReal()) {
		return SortReal(Result);
	}
	else return SortComplex(Result);
}

bool cMatrix::SortReal(cMatrix& Result)
{
	if (Result.Create(nr,nc)) return true;

	// copy the contents of *this to Result
	Result = *this;
	int pos;

	if (IsVector()) {
		//pv = static_cast<void*>(Result.GetR());
		//qsort( pv, nr*nc, sizeof(double), CompareByRealValue );
		std::sort( Result.mr.begin(), Result.mr.end() );
	}
	else {
		for (int c=0; c<nc; c++) {
			//pv = static_cast<void*>(Result.GetR() + c*nr);
			//qsort( static_cast<void*>(Result.m), nr, sizeof(double), CompareByRealValue );
			pos = c*nr;
			std::sort( Result.mr.begin() + pos, Result.mr.begin() + pos + nr );
		}
	}

	return false;
}

bool cMatrix::SortComplex(cMatrix& Result)
{
	if (IsReal()) return SortReal(Result);

	// sort on absulute value
	if (Abs(Result,*this)) return true;
	return SortReal(Result);

	return false;
}

bool cMatrix::SortIndex(cMatrix& Result, cMatrix& Ix)
{
	if (IsReal()) {
		return SortIndexReal(Result,Ix);
	}
	else return SortIndexComplex(Result,Ix);
}

bool cMatrix::SortIndexReal(cMatrix& Result, cMatrix& Indices)
{
	if (Result.Create(nr,nc)) return true;
	if (Indices.Create(nr,nc)) return true;

	Result.Copy(*this);
	Indices.Reset();

	int n=nr*nc;

	vector<sortElem> data;
	data.reserve(nr);

	if (IsVector()) {
		if (column2elemR(data,0)) return true;
		sort( data.begin(), data.end(), elemLessThan );
		elem2columnR(Result, Indices, data, 0);
	}
	else {
		for (int c=0; c<nc; c++) {
			if (column2elemR(data,c)) return true;
			sort( data.begin(), data.end(), elemLessThan );
			elem2columnR(Result, Indices, data, c);
		}
	}

	return false;
}

bool cMatrix::SortIndexComplex(cMatrix& Result, cMatrix& Indices)
{
	if (Result.CreateComplex(nr,nc)) return true;
	if (Indices.Create(nr,nc)) return true;

	Result.Copy(*this);
	Indices.Reset();

	int n=nr*nc;

	vector<sortElem> data;

	if (IsVector()) {
		if (column2elemC(data,0)) return true;
		sort( data.begin(), data.end(), elemLessThan );
		elem2columnC(Result, Indices, data, 0);
	}
	else {
		for (int c=0; c<nc; c++) {
			if (column2elemC(data,c)) return true;
			sort( data.begin(), data.end(), elemLessThan );
			elem2columnC(Result, Indices, data, c);
		}
	}

	return false;
}

bool cMatrix::column2elemR(vector<sortElem>& data, int icol)
{
	if (icol<0 || icol>=nc) { addError("index outside array bounds"); return true; }

	data.clear();
	data.reserve(nr);
	sortElem elem;
	int i;

	// copy data to vector
	if (IsVector()) {
		for (i=0; i<nr*nc; ++i) {
			elem.ix = i;
			elem.dv = mr[i];
			data.push_back( elem );	// assumes vector has already been 'reserve'd
		}	
	}
	else {
		for (i=0; i<nr; ++i) {
			elem.ix = i;
			elem.dv = mr[i+icol*nr];
			data.push_back( elem );	// assumes vector has already been 'reserve'd
		}
	}
	return false;
}

bool cMatrix::column2elemC(vector<sortElem>& data, int icol)
{
	if (icol<0 || icol>=nc) { addError("index outside array bounds"); return true; }

	sortElem elem;
	int i;

	// copy data to vector
	if (IsVector()) {
		for (i=0; i<nr*nc; ++i) {
			elem.ix = i;
			elem.dv = mr[i]*mr[i] + mi[i]*mi[i];
			data.push_back( elem );	// assumes vector has already been 'reserve'd
		}	
	}
	else {
		for (i=0; i<nr; ++i) {
			elem.ix = i;
			elem.dv = mr[i+icol*nr]*mr[i+icol*nr] + mi[i+icol*nr]*mi[i+icol*nr];
			data[i] = elem;	// assumes vector has already been 'reserve'd
		}
	}
	return false;
}

bool cMatrix::elem2columnR(cMatrix& Result, cMatrix& Ix, vector<sortElem>& data, int icol)
{
	if (icol<0 || icol>=nc) { addError("index outside array bounds"); return true; }
	int i;

	// copy data to vector
	if (IsVector()) {
		for (i=0; i<nr*nc; ++i) {
			Ix.SetR(i, data[i].ix);
			Result.SetR(i, data[i].dv);
		}
	}
	else {
		for (i=0; i<nr; ++i) {
			Ix.SetR(i, icol, data[i].ix);
			Result.SetR(i, icol, data[i].dv);
		}
	}
	return false;
}

bool cMatrix::elem2columnC(cMatrix& Result, cMatrix& Ix, vector<sortElem>& data, int icol)
{
	if (icol<0 || icol>=nc) { addError("index outside array bounds"); return true; }

	int ix;
	int i;
	complex<double> cc;

	// copy data to vector
	if (IsVector()) {
		for (i=0; i<nr*nc; ++i) {
			Ix.SetR(i, double(data[i].ix));
			cc = complex<double>(mr[i],mi[i]);
			Result.Set(i, cc);
		}
	}
	else {
		for (i=0; i<nr; ++i) {
			Ix.SetR(i, icol, double(data[i].ix));
			ix = i + icol*nr;
			cc = complex<double>(mr[ix],mi[ix]);
			Result.Set(i, icol, cc);
		}
	}
	return false;
}

//
// access operator (two args), lvalue
//
bool cMatrix::matIxAccess( cMatrix& Result, double darg1, double darg2 )
{
	if (Result.Create(1,1)) return true;

	int ir = (int) darg1;
	int ic = (int) darg2;

	if (ir < 0 || ir >= nr || ic < 0 || ic >= nc) {
		Result.Delete();
		addError("index outside array bounds");
		return true;
	}		// indexing error

	double lf;
	int ix = ic*nr + ir;

	GetR(lf,ix);
	Result.SetR(0, lf);

	return false;
}

bool cMatrix::matIxAccess(cMatrix& Result, cMatrix& arg1, double darg2)
{
	if (Result.Create(arg1.GetRows(),arg1.GetColumns())) return true;

	int lc = (int) darg2;

	if (lc < 0 || lc >= nc) {	// attempting to index outside matrix
		Result.Delete();
		addError("index outside array bounds"); return true;
	}

	double *pr = Result.GetR();
	const double *par = arg1.GetR();
	int nar = arg1.GetRows(), nac = arg1.GetColumns();

	int r,c;
	int ar;

	for (r=0; r<arg1.GetRows(); ++r) {
		for (c=0; c<arg1.GetColumns(); ++c) {
			ar = (int) floor( arg1.mr[c*nar+r] );
			// check for indexing outside matrix
			if (ar < 0 || ar >= nr) { Result.Delete(); addError("index outside array bounds"); return true; }
			// store absolute index in Result
			Result.mr[c*nr+r] = ar + nr * lc;
		}
	}

	return false;
}

bool cMatrix::matIxAccess(cMatrix& Result, double darg1, cMatrix& arg2)
{
	if (Result.Create(arg2.GetRows(),arg2.GetColumns())) return true;

	int lr = (int) darg1;

	if (lr < 0 || lr >= nr) {	// attempting to index outside matrix
		Result.Delete();
		addError("index outside array bounds"); return true;
	}

	//double *pr = Result.GetR();
	//const double *par = arg2.GetR();
	int nar = arg2.GetRows(), nac = arg2.GetColumns();

	int r,c;
	int ac;

	for (r=0; r<arg2.GetRows(); ++r) {
		for (c=0; c<arg2.GetColumns(); ++c) {
			ac = (int) floor( arg2.mr[c*nar+r] );
			// check for indexing outside matrix
			if (ac < 0 || ac >= nc) { Result.Delete(); addError("index outside array bounds"); return true; }
			// store absolute index in Result
			Result.mr[c*nr+r] = lr + nr * ac;
		}
	}

	return false;
}

bool cMatrix::matIxAccess(cMatrix& Result, cMatrix& arg1, cMatrix& arg2)
{
	int nar = arg1.GetRows(), nac = arg1.GetColumns();

	// make sure arguments are same size
	if (nar != arg2.GetRows() || nac != arg2.GetColumns()) { addError("matrix size mismatch"); return true; }

	if (Result.Create(nar, nac)) return true;

	//double *pr = Result.GetR();
	//const double *par1 = arg1.GetR();
	//const double *par2 = arg2.GetR();

	int r,c;
	int ar, ac;

	for (r=0; r<nar; ++r) {
		for (c=0; c<nac; ++c) {
			ar = (int) floor( arg1.mr[c*nar+r] );
			ac = (int) floor( arg2.mr[c*nar+r] );
			// check for indexing outside matrix
			if (ar < 0 || ar >= nr || ac < 0 || ac >= nc) { Result.Delete(); addError("index outside array bounds"); return true; }
			// store absolute index in Result
			Result.mr[c*nr+r] = ar + nr * ac;
		}
	}

	return false;
}
