
#include "matrix.h"
#include "my_errors.h"
#include <cmath>


//
// vector/matrix norm
//
double FrobeniusNormR(cMatrix& A)
{
	int n = A.GetSize();
	double dSum = 0.;

	const double *pr = A.GetSafeR();
	for (int i=0; i<n; i++) dSum += pr[i]*pr[i];

	return sqrt(dSum);
}

double FrobeniusNorm(cMatrix& A)
{
	if (A.IsReal()) return FrobeniusNormR(A);

	int n = A.GetSize();
	double dSum = 0.;

	const double *pr = A.GetSafeR();
	const double *pi = A.GetSafeI();
	for (int i=0; i<n; i++) dSum += pr[i]*pr[i] + pi[i]*pi[i];

	return sqrt(dSum);
}

double InfinityNormR(cMatrix& R, bool bNegative)
//Return the max sum of magnitudes down the columns.
{
	cMatrix Res;
	return Max(Res, R);
	if (R.GetSize()==0) return 0.;

	double dSum = 0.;
	double dMaxSum(-DBL_MAX), dMinSum(DBL_MAX);
	const double *pr = R.GetSafeR();

	int nc = R.GetColumns();
	int nr = R.GetRows();

	for (int ic=0; ic<nc; ic++) {
		dSum = 0.;
		for (int ir=0; ir<nr; ir++) {
			dSum += fabs( pr[ir+nc*ic] );
		}

		if (ic>0) {
			if (bNegative) {
				// -Inf norm
				if (dMinSum>dSum) dMinSum = dSum;
			}
			else {
				// Inf norm
				if (dMaxSum<dSum) dMaxSum = dSum;
			}
		}
		else dMaxSum = dMinSum = dSum;
	}	// loop over columns

	return dMaxSum;
}

double InfinityNorm(cMatrix& A, bool bNegative)
{
	if (A.IsReal()) return InfinityNormR(A,bNegative);

	if (A.GetSize()==0) return 0.;

	double dSum = 0.;
	double dMaxSum(-DBL_MAX), dMinSum(DBL_MAX);
	const double *pr = A.GetSafeR();
	const double *pi = A.GetSafeI();

	int nc = A.GetColumns();
	int nr = A.GetRows();

	for (int ic=0; ic<nc; ic++) {
		dSum = 0.;
		for (int ir=0; ir<nr; ir++) {
			dSum += abs( complex<double>(pr[ir+nc*ic],pi[ir+nc*ic]) );
		}

		if (ic>0) {
			if (bNegative) {
				// -Inf norm
				if (dMinSum>dSum) dMinSum = dSum;
			}
			else {
				// Inf norm
				if (dMaxSum<dSum) dMaxSum = dSum;
			}
		}
		else dMaxSum = dMinSum = dSum;
	}	// loop over columns

	return dMaxSum;
}

double OneNormR(cMatrix& R)
{
	if (R.GetSize()==0) return 0.;

	double dSum = 0.;
	double dMaxSum(-DBL_MAX);
	const double *pr = R.GetSafeR();

	int nc = R.GetColumns();
	int nr = R.GetRows();

	for (int ir=0; ir<nr; ir++) {
		dSum = 0.;
		for (int ic=0; ic<nc; ic++) {
			dSum += fabs( pr[ir+nc*ic] );
		}
		if (dMaxSum<dSum) dMaxSum = dSum;
	}	// loop over rows

	return dMaxSum;
}

double OneNorm(cMatrix& A)
{
	if (A.IsReal()) return OneNormR(A);

	if (A.GetSize()==0) return 0.;

	double dSum = 0.;
	double dMaxSum(-DBL_MAX);
	const double *pr = A.GetSafeR();
	const double *pi = A.GetSafeI();

	int nc = A.GetColumns();
	int nr = A.GetRows();

	for (int ir=0; ir<nr; ir++) {
		dSum = 0.;
		for (int ic=0; ic<nc; ic++) {
			dSum += abs( complex<double>(pr[ir+nc*ic],pi[ir+nc*ic]) );
		}
		if (dMaxSum<dSum) dMaxSum = dSum;
	}	// loop over columns

	return dMaxSum;
}

double PNormR(cMatrix& R, double P)
{
	int n = R.GetSize();
	double dSum = 0.;

	const double *pr = R.GetSafeR();
	for (int i=0; i<n; i++) dSum += pow(fabs(pr[i]),P);

	return pow(dSum,1./P);
}

double PNorm(cMatrix& A, double P)
{
	if (A.IsReal()) return PNormR(A,P);

	int n = A.GetSize();
	double dSum = 0., term;

	const double *pr = A.GetSafeR();
	const double *pi = A.GetSafeI();
	for (int i=0; i<n; i++) {
		term = abs( complex<double>(pr[i],pi[i]) );
		dSum += pow(term,P);
	}

	return pow(dSum,1./P);
}

bool VectorNorm(cMatrix& Result, cMatrix& v, double P)
{
	int nType = std::fpclassify(P);
	double dv;
	cMatrix vm;

	if (nType==FP_NORMAL || nType==FP_ZERO) {
		dv = PNorm(v, P);
		Result.SetTo( dv );
	}
	else if (nType==FP_NAN) {
		addError("illegal argument"); return true;
	}
	else {
		if (std::signbit(P)) { // negative
			if (Abs(vm,v)) return true;
			return Min(Result,vm);
		} else { // positive
			if (Abs(vm,v)) return true;
			return Max(Result,vm);
		}
	}

	return false;
}

bool MatrixNorm(cMatrix& Result, cMatrix& v, double P)
{
	int nType = std::fpclassify(P);
	double dv;
	cMatrix vm;

	if (nType==FP_NORMAL || nType==FP_ZERO) {
		dv = PNorm(v, P);
		Result.SetTo( dv );
	}
	else if (nType==FP_NAN) {
		addError("illegal argument"); return true;
	}
	else {
		if (std::signbit(P)) { // negative
			if (Abs(vm,v)) return true;
			vm.Reshape( vm.GetSize(), 1 );
			return Min(Result,vm);
		} else { // positive
			if (Abs(vm,v)) return true;
			vm.Reshape( vm.GetSize(), 1 );
			return Max(Result,vm);
		}
	}

	return false;
}

// interface fcns
bool Matrix2Norm(cMatrix& Result, cMatrix& v)
{
	if (v.IsVector()) return VectorNorm(Result, v, 2.);
	else return MatrixNorm(Result, v, 2.);
}

bool MatrixPNorm(cMatrix& Result, cMatrix& v, cMatrix& p)
{
	if (!p.IsScalar()) {
		addError("illegal argument"); return true;
	}

	double dP = p.Get0R();
	if (v.IsVector()) return VectorNorm(Result, v, dP);
	else return MatrixNorm(Result, v, dP);
}

// inner product
double InnerProduct( vector<double>::iterator v1, vector<double>::iterator v2, int len )
{
	double sum = 0.;
	for (int i=0; i<len; ++i) {
		sum += (*v1) * (*v2);
		++v1; ++v2;
	}
	return sum;
}

complex<double> InnerProduct( vector<double>::iterator r1, vector<double>::iterator i1, vector<double>::iterator r2, vector<double>::iterator i2, int len )
{
	complex<double> csum;
	double dr(0.), di(0.);
	for (int i=0; i<len; ++i) {
		dr += (*r1)*(*r2) - (*i1)*(*i2);
		di += (*r1)*(*i2) + (*i1)*(*r2);
		++r1; ++r2;
		++i1; ++i2;
	}
	return complex<double>(dr,di);
}

complex<double> InnerProduct( vector<double>::iterator r, vector<double>::iterator r2, vector<double>::iterator i2, int len )
{
	complex<double> csum;
	double dr(0.), di(0.);
	for (int i=0; i<len; ++i) {
		dr += (*r)*(*r2);
		di += (*r)*(*i2);
		++r;
		++r2; ++i2;
	}
	return complex<double>(dr,di);
}

// outer product
bool OuterProductR( cMatrix *Result, cMatrix *a, cMatrix *b )
{	// a and and b MUST be vectors

	bool bCrossDimensions = false;
	bool bSameDimensions = false;
	bool bVectors = false;

	// check that data supplied is dimensioned correctly
	bVectors = a->IsVector() && b->IsVector();
	bCrossDimensions = (a->nr==b->nc) && (a->nc==b->nr) && (a->nc==1);
	bSameDimensions = (a->nr==b->nr) && (a->nc==b->nc) && ((a->nc==1)||(a->nr==1));
	if ( !(bVectors&&(bCrossDimensions||bSameDimensions)) )
		{ addError("incompatable matrix dimensions"); return true; }

	// dimensions are now valid
	int N = a->GetColumns();
	if (a->GetRows() > a->GetColumns()) N = a->GetRows();

	if (Result->Create(N,N)) return true;

	const double *par = a->GetSafeR();
	const double *pbr = b->GetSafeR();

	for (int r=0; r<N; r++) {
		for (int c=0; c<N; c++) {
			Result->SetR( r, c, par[r]*pbr[c] );
		}
	}

	return false;
}

bool OuterProductC( cMatrix *Result, cMatrix *a, cMatrix *b )
{	// a and and b MUST be vectors

	bool bCrossDimensions = false;
	bool bSameDimensions = false;
	bool bVectors = false;

	// check that data supplied is dimensioned correctly
	bVectors = a->IsVector() && b->IsVector();
	bCrossDimensions = (a->nr==b->nc) && (a->nc==b->nr) && (a->nc==1);
	bSameDimensions = (a->nr==b->nr) && (a->nc==b->nc) && ((a->nc==1)||(a->nr==1));
	if ( !(bVectors&&(bCrossDimensions||bSameDimensions)) )
		{ addError("incompatable matrix dimensions"); return true; }

	// dimensions are now valid
	int N = a->GetColumns();
	if (a->GetRows() > a->GetColumns()) N = a->GetRows();

	if (Result->CreateComplex(N,N)) return true;

	complex<double> ca, cb, cab;

	for (int r=0; r<N; r++) {
		for (int c=0; c<N; c++) {
			a->Get(ca,r);
			b->Get(cb,c);
			cab = ca*cb;
			Result->Set( r, c, cab );
		}
	}

	return false;
}


int LUDecompositionR(cMatrix& a, cMatrix& indx, double& d)
//*****************************************************************************
//Given a matrix a[0...n-1][0...n-1], this routine replaces it by the LU
//decomposition of a rowwise permutation of itself.  a and n are input.  a
//is output, arranged as in equation 2.3.14 in "Numerical Recipes in C";
//indx[0..n-1] is an output vector that records the row permutation
//effected by the partial pivoting; d is output as +- depending on whether
//the number of row interchanges was even or odd, respectively. This
//routine is used in combination with lubksb to solve linear equations
//or invert a matrix.
//******************************************************************************
{
	int iErr = __NO_ERRORS;

	int i,imax,j,k;
	double big,dum,sum,temp,*vv;
	const double TINY = 1.0e-300; // A small number

	int n = a.GetRows();				// n is the dimension of the input matrix

	vv = new double[n];
	memset( vv, 0, n*sizeof(double) );
	//for (i=0;i<n;i++) vv[i] = 0.0;

	if (indx.Create(n,1)) return true;

	double *par = a.GetR();
	double *pir = indx.GetR();

	d = 1.0;

	for (i=0; i<n; i++) {
		for (big=0.0,j=0; j<n; j++) {
			if ( (temp=fabs(par[i+j*n])) > big ) big = temp;
		}
		if(big==0.) {
			delete []vv;
			return SINGULAR_MATRIX;
		}
		vv[i] = 1.0 / big;
	}

	for (j=0; j<n; j++) {
		for (i=0; i<j; i++) {
			for (sum=par[i+j*n],k=0; k<i; k++) sum -= par[i+k*n]*par[k+j*n];		// a.Get(i,k)*a.Get(k,j);
			a.SetR(i,j, sum);
		}
		for (big=0.0,i=j; i<n; i++) {
			for (sum=par[i+j*n],k=0; k<j; k++) sum -= par[i+k*n]*par[k+j*n];		// a.Get(i,k)*a.Get(k,j);
			a.SetR(i,j, sum);
			if ((dum=vv[i]*fabs(sum)) >= big) {
				big = dum;
				imax = i;
			}
		}
		if (j != imax) {
			for (k=0; k<n; k++) {
				dum = par[imax+k*n];		// a.Get(imax,k);
				a.SetR(imax,k, par[j+k*n]);	// a.Get(j,k));
				a.SetR(j,k, dum);
			}
			d = -d;
//			*d = -(*d);
			vv[imax] = vv[j];
		}

		pir[j] = double( imax );
		if (par[j+j*n] == 0.0) a.SetR(j,j, TINY);

		if (j != n-1) {
			dum = 1.0 / par[j+j*n];
			for (i=j+1; i<n; i++) par[i+j*n] *= dum;	// a.SetR(i,j, par[i+j*n]*dum);
		}
	}

	delete []vv;

	return iErr;
}

int LUDecompositionC(cMatrix& a, cMatrix& indx, double& d)
//*****************************************************************************
//Given a matrix a[0...n-1][0...n-1], this routine replaces it by the LU
//decomposition of a rowwise permutation of itself.  a and n are input.  a
//is output, arranged as in equation 2.3.14 in "Numerical Recipes in C";
//indx[0..n-1] is an output vector that records the row permutation
//effected by the partial pivoting; d is output as +- depending on whether
//the number of row interchanges was even or odd, respectively. This
//routine is used in combination with lubksb to solve linear equations
//or invert a matrix.
//******************************************************************************
{
	int iErr = __NO_ERRORS;

	int i,imax,j,k;
	double big,temp,*vv;
	complex<double> TINY = complex<double>(1.0e-300,0.); // A small number

	int n = a.GetRows();				// n is the dimension of the input matrix
	vv = new double[n];
	memset( vv, 0, n*sizeof(double) );
	//for (i=0;i<n;i++) vv[i] = 0.0;

	if (indx.Create(n,1)) return true;

	double *par = a.GetR();
	double *pai = a.GetI();
	double *pir = indx.GetR();
	complex<double> cv, sum, dum1, dum2;

	d = 1.0;

	for (i=0; i<n; i++) {
		for (big=0.0,j=0; j<n; j++) {
			a.Get(cv, i, j);
			if ( (temp=abs(cv)) > big ) big = temp;
		}
		if(big==0.) {
			delete []vv;
			return SINGULAR_MATRIX;
		}
		vv[i] = 1.0 / big;
	}

	for (j=0; j<n; j++) {
		for (i=0; i<j; i++) {
			a.Get(sum,i,j);
			for (k=0; k<i; k++) {
				a.Get(dum1,i,k);
				a.Get(dum2,k,j);
				sum -= dum1 * dum2;		// a.Get(i,k)*a.Get(k,j);
			}
			a.Set(i,j, sum);
		}
		for (big=0.0,i=j; i<n; i++) {
			a.Get(sum,i,j);
			for (k=0; k<j; k++) {
				a.Get(dum1,i,k);
				a.Get(dum2,k,j);
				sum -= dum1 * dum2;		// a.Get(i,k)*a.Get(k,j);
			}
			a.Set(i,j, sum);
			if ((temp=vv[i]*abs(sum)) >= big) {
				big = temp;
				imax = i;
			}
		}
		if (j != imax) {
			for (k=0; k<n; k++) {
				a.Get(dum1,imax,k);			// a.Get(imax,k);
				a.Get(dum2,j,k);
				a.Set(imax,k,dum2);			// a.Get(j,k));
				a.Set(j,k, dum1);
			}
			d = -d;
			vv[imax] = vv[j];
		}

		pir[j] = double( imax );
		if (par[j+j*n] == 0.0) a.Set(j,j, TINY);

		if (j != n-1) {
			a.Get(dum1,j,j);
			dum2 = complex<double>(1.0,0.0) / dum1;
			for (i=j+1; i<n; i++) {
				a.Get(dum1,i,j);
				dum1 *= dum2;	// a.SetR(i,j, par[i+j*n]*dum);
				a.Set(i,j, dum1);
			}
		}
	}

	delete []vv;

	return iErr;
}

bool LUDecomposition(cMatrix& LU, cMatrix& A)
{
	cMatrix idx;
	double det;
	LU = A;

	if (A.IsReal()) {
		if (LUDecompositionR(LU,idx,det) != __NO_ERRORS) return true;
	}
	else {
		if (LUDecompositionC(LU,idx,det) != __NO_ERRORS) return true;
	}
	return false;
}

bool LUDecomposition(cMatrix& L, cMatrix& U, cMatrix& A)
{
	cMatrix LU(A), idx;
	double det;
	LU = A;

	if (A.IsReal()) {
		if (LUDecompositionR(LU,idx,det) != __NO_ERRORS) return true;
		if (UpperTri(U,LU)) return true;
		if (LowerTri(L,idx,LU)) return true;
	}
	else {
		if (LUDecompositionC(LU,idx,det) != __NO_ERRORS) return true;
		if (UpperTri(U,LU)) return true;
		if (LowerTri(L,idx,LU)) return true;
	}
	return false;
}

bool UpperTri(cMatrix& U, cMatrix& A)
{
	if (A.IsReal()) { if (U.Create(A.GetRows(),A.GetColumns())) return true; }
	else  { if (U.CreateComplex(A.GetRows(),A.GetColumns())) return true; }

	U.Reset();
	int i, j, nr(A.GetRows()), nc(A.GetColumns());

	const double *par = A.GetR(), *pai = A.GetI();
	double *pur = U.GetR(), *pui = U.GetI();

	for (i=0; i<nr; ++i) {
		for (j=i; j<nc; ++j) {
			pur[i+j*nr] = par[i+j*nr];
		}
	}

	if (!A.IsReal()) {
		for (i=0; i<nr; ++i) {
			for (j=i; j<nc; ++j) {
				pui[i+j*nr] = pai[i+j*nr];
			}
		}
	}

	return false;
}

bool LowerTri(cMatrix& L, cMatrix& idx, cMatrix& A)
{
	if (A.IsReal()) { if (L.Create(A.GetRows(),A.GetColumns())) return true; }
	else  { if (L.CreateComplex(A.GetRows(),A.GetColumns())) return true; }

	L.Reset();
	int ir, i, j, nr(A.GetRows()), nc(A.GetColumns());

	const double *par = A.GetR(), *pai = A.GetI();
	double *plr = L.GetR(), *pli = L.GetI();
	double *idxr = idx.GetR();

	for (i=0; i<nr; ++i) {
		for (j=0; j<i; ++j) {
			plr[i+j*nr] = par[i+j*nr];
		}
		plr[i+i*nr] = 1.;
	}

	if (!A.IsReal()) {
		for (i=0; i<nr; ++i) {
			for (j=i; j<nc; ++j) {
				pli[i+j*nr] = pai[i+j*nr];
			}
		}
		pli[i+i*nr] = 1.;
	}

	// swap rows
	for (i=nr-1; i>=0; --i) {
		ir = int(idxr[i]);
		if (ir != i) {
			swapRows(L, i, ir);
		}
	}

	return false;
}

void swapRows(cMatrix& A, int i0, int i1)
{
	int j, nc = A.GetColumns(), nr = A.GetRows();
	double d;
	double *pr = A.GetR();
	double *pi = A.GetI();

	for (j=0; j<nc; ++j) {
		d = pr[i0+j*nr];
		pr[i0+j*nr] = pr[i1+j*nr];
		pr[i1+j*nr] = d;
	}

	if (!A.IsReal()) {
		for (j=0; j<nc; ++j) {
			d = pr[i0+j*nr];
			pr[i0+j*nr] = pr[i1+j*nr];
			pr[i1+j*nr] = d;
		}
	}
}


void LUBackSubstitutionR(cMatrix& a, cMatrix& indx, cMatrix& b)
//*****************************************************************************
//  Solves the set of n linear equations AX = B.  Here a[0...n-1][0...n-1] is
//  input, not as the matrix A but rather as its LU decomposition, determined by
//  the routine LUDecomposition.  indx[0...n-1] is input as the permutation vector re-
//  turned by LUDecomposition.  b[0...n-1] is input as the right-hand side vector B, and
//  returns with the solution vector X.  a, n, and indx are not modified by this
//  routine and can be left in place for successive calls with different right-
//  hand sides b.  This routine takes into account the possibility that b will
//  begin with many zero elements, so it is efficient for use in matrix inversion.
// 
//*****************************************************************************
{
	int i,ii=-1,ip,j;
	double sum;
	bool bSum(false);
	int n = a.GetRows();

	double *par = a.GetR();
	double *pbr = b.GetR();
	double *pir = indx.GetR();

	for (i=0;i<n;i++) {
		ip = (int) pir[i];
		sum = pbr[ip];
		pbr[ip] = pbr[i];

		if (sum != 0.) bSum = true;

		if (ii!=-1) {
			for (j=ii;j<=i-1;j++) sum -= par[i+j*n] * pbr[j];
		}
		else if (bSum) ii = i;

		pbr[i] = sum;
	}

	for (i=n-1;i>=0;i--) {
		for (sum=pbr[i],j=i+1; j<n; j++) sum -= par[i+j*n] * pbr[j];
		pbr[i] = sum / par[i+n*i];
	}
}

void LUBackSubstitutionC(cMatrix& a, cMatrix& indx, cMatrix& b)
//*****************************************************************************
//  Solves the set of n linear equations AX = B.  Here a[0...n-1][0...n-1] is
//  input, not as the matrix A but rather as its LU decomposition, determined by
//  the routine LUDecomposition.  indx[0...n-1] is input as the permutation vector re-
//  turned by LUDecomposition.  b[0...n-1] is input as the right-hand side vector B, and
//  returns with the solution vector X.  a, n, and indx are not modified by this
//  routine and can be left in place for successive calls with different right-
//  hand sides b.  This routine takes into account the possibility that b will
//  begin with many zero elements, so it is efficient for use in matrix inversion.
// 
//*****************************************************************************
{
	int i,ii=-1,ip,j;
	bool bSum(false);
	int n = a.GetRows();

	complex<double> ca, cb, csum, crat;
	double *pir = indx.GetR();

	for (i=0;i<n;i++) {
		ip = (int) pir[i];
		b.Get(csum, ip);
		b.Get(cb, i);
		b.Set(ip, cb);

		if (csum.real() != 0. || csum.imag() !=0.) bSum = true;

		if (ii!=-1) {
			for (j=ii;j<=i-1;j++) {
				a.Get(ca, i, j);
				b.Get(cb, j);
				csum -= ca * cb;
			}
		}
		else if (bSum) ii = i;

		b.Set(i, csum);
	}

	for (i=n-1;i>=0;i--) {
		b.Get(csum, i);
		for (j=i+1; j<n; j++) {
			a.Get(ca, i, j);
			b.Get(cb, j);
			csum -= ca * cb;
		}
		a.Get(ca, i, i);
		crat = csum / ca;
		b.Set(i, crat);
	}
}


int LUMatrixInvertR(cMatrix& AInvrs, // [0...N-1][0...N-1]Inverted matrix
					cMatrix& A, // [0...N-1][0...N-1] Matrix to invert
					double& Determinant) // The determinant of matrix a
{
	if (!A.IsSquare()) return DIMENSION_MISMATCH;

	int j;
	int N = A.GetRows();
	int iErr = __NO_ERRORS;
	cMatrix indx, col;

	if ( AInvrs.Create(N,N) ) return MEMORY_ERROR;
	if (col.Create(N,1)) return MEMORY_ERROR;

	double *pair = AInvrs.GetR();
	double *pcr = col.GetR();

	cMatrix y(A);
	iErr = LUDecompositionR(y, indx, Determinant);

	double *pyr = y.GetR();

	if (iErr==__NO_ERRORS) {

		for (j=0;j<N;j++) {
			Determinant *= pyr[j+N*j];				// y.Get(j,j);
			col.Reset();							// for (i=0;i<N;i++) col.m[i] = 0.0;
			pcr[j] = 1.0;
			LUBackSubstitutionR(y,indx,col);
			memcpy( pair+j*N, pcr, N*sizeof(double) );
		//	for (i=0;i<N;i++) pair[i+j*N] = pcr[i]);
		}

	}

	return iErr;
}

int LUMatrixInvertC(cMatrix& AInvrs, // [0...N-1][0...N-1] Inverted matrix
					cMatrix& A, // [0...N-1][0...N-1] Matrix to invert
					complex<double>& Determinant) // The determinant of matrix a
{
	if (!A.IsSquare()) return DIMENSION_MISMATCH;

	int j;
	int N = A.GetRows();
	int iErr = __NO_ERRORS;
	cMatrix indx, col;

	if ( AInvrs.CreateComplex(N,N) ) return MEMORY_ERROR;
	if (col.CreateComplex(N,1)) return MEMORY_ERROR;

	double *pair = AInvrs.GetR();
	double *paii = AInvrs.GetI();
	double *pcr = col.GetR();
	double *pci = col.GetI();
	complex<double> cy;

	cMatrix y(A);
	double d;
	iErr = LUDecompositionC(y, indx, d);
	Determinant = complex<double>(d,0.);

	double *pyr = y.GetR();

	if (iErr==__NO_ERRORS) {

		for (j=0;j<N;j++) {
			y.GetC(cy, j, j);
			Determinant *= cy;
			col.Reset();							// for (i=0;i<N;i++) col.m[i] = 0.0;
			pcr[j] = 1.0;
			LUBackSubstitutionC(y,indx,col);
			memcpy( pair+j*N, pcr, N*sizeof(double) );
			memcpy( paii+j*N, pci, N*sizeof(double) );
		}

	}

	return iErr;
}

bool LUMatrixInvert(cMatrix& Result, cMatrix& A)
{
	complex<double> cdet;
	double det;

	if (A.IsReal()) return LUMatrixInvertR(Result, A, det) != __NO_ERRORS;
	else return LUMatrixInvertC(Result, A, cdet) != __NO_ERRORS;
}


int LUSolveR(cMatrix& A,
			cMatrix& X,
			cMatrix& B)
{
	if (!A.IsSquare()) return DIMENSION_MISMATCH;

	int i;
	int N = A.GetRows();
	int iErr = __NO_ERRORS;
	int nB;

	cMatrix indx, b;

	X.Delete();
	cMatrix LU(A);

	double d;
	iErr = LUDecompositionR(LU, indx, d);

	if (iErr==__NO_ERRORS) {

		nB = B.GetColumns();

		for (i=0; i<nB; i++) {
			B.GetColumn(&b, i);
			LUBackSubstitutionR(LU,indx,b);
			if (X.AugmentColumns(&b))
				return MEMORY_ERROR;
		}

	}

	return iErr;
}

int LUSolveC(cMatrix& A,
			cMatrix& X,
			cMatrix& B)
{
	if (!A.IsSquare()) return DIMENSION_MISMATCH;

	int i;
	int N = A.GetRows();
	int iErr = __NO_ERRORS;
	int nB;

	cMatrix indx, b;

	X.Delete();
	cMatrix LU(A);

	double d;
	iErr = LUDecompositionC(LU, indx, d);

	if (iErr==__NO_ERRORS) {

		nB = B.GetColumns();

		for (i=0; i<nB; i++) {
			B.GetColumn(&b, i);
			LUBackSubstitutionC(LU,indx,b);
			if (X.AugmentColumns(&b))
				return MEMORY_ERROR;
		}

	}

	return iErr;
}


//Calculate the determinant of A.

int LUDeterminantR(cMatrix& A, double& Determinant)
{
	if (!A.IsSquare()) return DIMENSION_MISMATCH;

	int j;
	int N = A.GetRows();
	int iErr = __NO_ERRORS;
	cMatrix indx;

	double d = 1;
	iErr = LUDecompositionR(A, indx, d);

	Determinant = d;
	double *par = A.GetR();

	if (iErr==__NO_ERRORS) {

		for (j=0; j<N; j++) {
			Determinant *= par[j+N*j];		// A.Get(j,j);
		}

	}

	return iErr;
}

int LUDeterminantC(cMatrix& A, complex<double>& Determinant)
{
	if (!A.IsSquare()) return DIMENSION_MISMATCH;

	int j;
	int N = A.GetRows();
	int iErr = __NO_ERRORS;
	cMatrix indx;

	double d;
	iErr = LUDecompositionC(A, indx, d);

	complex<double> cv = complex<double>(d,0.);
	Determinant = cv;

	if (iErr==__NO_ERRORS) {

		for (j=0; j<N; j++) {
			A.GetC(cv,j,j);
			Determinant *= cv;		// A.Get(j,j);
		}

	}

	return iErr;
}


// Householder transformation and QR decomposition
//
int GetHouseR(cMatrix& v, cMatrix& x)
{
	// get v so that (I-2(vv')/(v'v))x \in span(e1)
	int iErr = __NO_ERRORS;

	if (!x.IsVector()) return DIMENSION_MISMATCH;

	double u = FrobeniusNorm(x);
	if ( v.Create(x.GetRows(),x.GetColumns()) ) return MEMORY_ERROR;
	v.SetR(0,0,1.);

	if (u==0) return iErr;
	double *px = x.GetR();

	for (int i=1; i<x.GetSize(); ++i)
	{
		v.SetR(i,0, px[i]/u);
	}

	return iErr;
}

int GetHouseC(cMatrix& v, cMatrix& x)
{
	// get v so that (I-2(vv')/(v'v))x \in span(e1)
	int iErr = __NO_ERRORS;

	if (!x.IsVector()) return DIMENSION_MISMATCH;

	double u = FrobeniusNorm(x);
	if ( v.CreateComplex(x.GetRows(),x.GetColumns()) ) return MEMORY_ERROR;
	v.SetR(0,0,1.);

	if (u==0) return iErr;
	complex<double> cx, crat;

	double *px = x.GetR();

	for (int i=1; i<x.GetSize(); ++i)
	{
		x.GetC(cx, i);
		crat = cx/u;
		v.Set(i, crat);
	}

	return iErr;
}

int HouseMatR(cMatrix& H, cMatrix& v)
{
	// return Householder reflection transformation
	int iErr = __NO_ERRORS;

	if (!v.IsVector()) return DIMENSION_MISMATCH;

	int n = v.GetSize();
	H.Create(n,n);

	cMatrix vv, I;
	OuterProductR(&vv, &v, &v);

	if (FillIdentity(I,n)) return MEMORY_ERROR;
	cMatrix vv2, r;
	Mult(vv2,vv,2.);
	double lf = InnerProduct(v.mr.begin(), v.mr.begin(), n);
	DotDiv(r,vv2,lf);

	//H = I - DotDivR(r,vv2,lf);
	Sub(H,I,r);

	return iErr;
}

int HouseMatC(cMatrix& H, cMatrix& v)
{
	// return Householder reflection transformation
	int iErr = __NO_ERRORS;

	if (!v.IsVector()) return DIMENSION_MISMATCH;

	int n = v.GetSize();
	H.CreateComplex(n,n);

	cMatrix vv, I;
	OuterProductC(&vv, &v, &v);

	if (FillIdentity(I,n)) return MEMORY_ERROR;

	cMatrix vv2, r;
	Mult(vv2,vv,2.);
	
	double lf = FrobeniusNorm(v);
	DotDiv(r,vv2,lf);

	//H = I - 2.*vv/InnerProduct(v.mr, v.mr, n);
	Sub(H,I,r);

	return iErr;
}

int QRDecompositionR(cMatrix& Q, cMatrix& R, cMatrix& A)
{
	// apply Householder transformations on A (m-by-n, m>=n)
	int iErr = __NO_ERRORS;

	cMatrix H, Hh, I, v, hq;
	int n = A.GetColumns();
	int m = A.GetRows();

	if (FillIdentity(Q,n)) return MEMORY_ERROR;
	//Q = Eye(n);

	for (int i=0; i<n; ++i)
	{
		A.GetR(v,i,i, m-1,i);

		iErr = HouseMatR(H,v);
		if (iErr!=__NO_ERRORS) return iErr;

		if (i>0) {
			if (FillIdentity(I,i)) return MEMORY_ERROR;
			iErr = BlockDiagR( Hh, I, H );
		}
		else
			Hh = H;

		Mult(hq,Hh,Q);
		Q = hq;
	}

	//R = Q*A;
	Mult(R,Q,A);

	return iErr;
}

int QRDecompositionC(cMatrix& Q, cMatrix& R, cMatrix& A)
{
	// apply Householder transformations on A (m-by-n, m>=n)
	int iErr = __NO_ERRORS;

	cMatrix H, Hh, I, v, hq;
	int n = A.GetColumns();
	int m = A.GetRows();

	if (FillIdentity(Q,n)) return MEMORY_ERROR;
	//Q = Eye(n);

	for (int i=0; i<n; ++i)
	{
		A.GetC(v,i,i, m-1,i);

		iErr = HouseMatC(H,v);
		if (iErr!=__NO_ERRORS) return iErr;

		if (i>0) {
			if (FillIdentity(I,i)) return MEMORY_ERROR;
			iErr = BlockDiagC( Hh, I, H );
			if (iErr!=__NO_ERRORS) return iErr;
		}
		else
			Hh = H;

		//Q = Hh*Q;
		Mult(hq,Hh,Q);
		Q = hq;
	}

	//R = Q*A;
	Mult(R,Q,A);

	return iErr;
}


bool MatrixInvert(cMatrix& Result, cMatrix& v)
{
	return LUMatrixInvert( Result, v );
}


bool RConv2R(cMatrix& res, cMatrix& A, cMatrix& B)
{
	res.Delete();

	int nro = A.GetRows() + B.GetRows() - 1;
	int nco = A.GetColumns() + B.GetColumns() - 1;

	int nra = A.GetRows();
	int nca = A.GetColumns();
	int nrb = B.GetRows();
	int ncb = B.GetColumns();

	if (nro*nco==0)
		return false;

	if (res.Create(nro, nco)) {
		addError("memory error");
		return true;
	}

	int r, c, k, l;
	double lf, lfa, lfb;

	// conv down columns
	for (r=0; r<nro; ++r) {
		for (c=0; c<nco; ++c) {

			// reset cumulative result for (r,c)
			lf = 0.;
			// 2-d convolution at (r,c)
			for (k=0; k<nrb; ++k) {
				for (l=0; l<ncb; ++l) {
					B.GetR(lfb,k,l);
					A.GetRz(lfa,r-k,c-l);		// returns zero (0) if index outside dimensions

					lf += lfa * lfb;
				}
			}

			res.SetR(r, c, lf);
		}
	}

	return false;
}

bool RConv2C(cMatrix& res, cMatrix& A, cMatrix& B)
{	// A is real, B is complex
	res.Delete();

	int nro = A.GetRows() + B.GetRows() - 1;
	int nco = A.GetColumns() + B.GetColumns() - 1;

	int nra = A.GetRows();
	int nca = A.GetColumns();
	int nrb = B.GetRows();
	int ncb = B.GetColumns();

	if (nro*nco==0)
		return false;

	if (res.CreateComplex(nro, nco)) {
		addError("memory error");
		return true;
	}

	int r, c, k, l;
	double lfr, lfi, lfb, lfar, lfai;

	// conv down columns
	for (r=0; r<nro; ++r) {
		for (c=0; c<nco; ++c) {

			// reset cumulative result for (r,c)
			lfr = lfi = 0.;
			// 2-d convolution at (r,c)
			for (k=0; k<nrb; ++k) {
				for (l=0; l<ncb; ++l) {
					B.GetR(lfb,k,l);
					A.GetRz(lfar,r-k,c-l);		// returns zero (0) if index outside dimensions
					A.GetIz(lfai,r-k,c-l);		// returns zero (0) if index outside dimensions

					lfr += lfb * lfar;
					lfi += lfb * lfai;
				}
			}

			res.SetR(r, c, lfr);
			res.SetI(r, c, lfi);
		}
	}

	return false;
}

bool CConv2R(cMatrix& res, cMatrix& A, cMatrix& B)
{	// A is real, B is complex
	res.Delete();

	int nro = A.GetRows() + B.GetRows() - 1;
	int nco = A.GetColumns() + B.GetColumns() - 1;

	int nra = A.GetRows();
	int nca = A.GetColumns();
	int nrb = B.GetRows();
	int ncb = B.GetColumns();

	if (nro*nco==0)
		return false;

	if (res.CreateComplex(nro, nco)) {
		addError("memory error");
		return true;
	}

	int r, c, k, l;
	double lfr, lfi, lfa, lfbr, lfbi;

	// conv down columns
	for (r=0; r<nro; ++r) {
		for (c=0; c<nco; ++c) {

			// reset cumulative result for (r,c)
			lfr = lfi = 0.;
			// 2-d convolution at (r,c)
			for (k=0; k<nrb; ++k) {
				for (l=0; l<ncb; ++l) {
					B.GetR(lfbr,k,l);
					B.GetI(lfbi,k,l);
					A.GetRz(lfa,r-k,c-l);		// returns zero (0) if index outside dimensions

					lfr += lfa * lfbr;
					lfi += lfa * lfbi;
				}
			}

			res.SetR(r, c, lfr);
			res.SetI(r, c, lfi);
		}
	}

	return false;
}

bool CConv2C(cMatrix& res, cMatrix& A, cMatrix& B)
{	// both A and B are complex
	res.Delete();

	int nro = A.GetRows() + B.GetRows() - 1;
	int nco = A.GetColumns() + B.GetColumns() - 1;

	int nra = A.GetRows();
	int nca = A.GetColumns();
	int nrb = B.GetRows();
	int ncb = B.GetColumns();

	if (nro*nco==0)
		return false;

	if (res.CreateComplex(nro, nco)) {
		addError("memory error");
		return true;
	}

	int r, c, k, l;
	double lfr, lfi, lfar, lfai, lfbr, lfbi;

	// conv down columns
	for (r=0; r<nro; ++r) {
		for (c=0; c<nco; ++c) {

			// reset cumulative result for (r,c)
			lfr = lfi = 0.;
			// 2-d convolution at (r,c)
			for (k=0; k<nrb; ++k) {
				for (l=0; l<ncb; ++l) {
					B.GetR(lfbr,k,l);
					B.GetI(lfbi,k,l);
					A.GetRz(lfar,r-k,c-l);		// returns zero (0) if index outside dimensions
					A.GetIz(lfai,r-k,c-l);		// returns zero (0) if index outside dimensions

					lfr += lfar * lfbr;
					lfi += lfai * lfbi;
				}
			}

			res.SetR(r, c, lfr);
			res.SetI(r, c, lfi);
		}
	}

	return false;
}

bool Conv2(cMatrix& res, cMatrix& A, cMatrix& B)
{
	if (A.IsReal() && B.IsReal()) {
		if (A.GetSize() > B.GetSize())
			return RConv2R(res, A, B);
		else
			return RConv2R(res, B, A);
	}
	else if (A.IsReal() && !B.IsReal()) {
		if (A.GetSize() > B.GetSize())
			return RConv2C(res, A, B);
		else
			return CConv2R(res, B, A);
	}
	else if (!A.IsReal() && B.IsReal()) {
		if (A.GetSize() > B.GetSize())
			return CConv2R(res, A, B);
		else
			return RConv2C(res, B, A);
	}
	else {
		if (A.GetSize() > B.GetSize())
			return CConv2C(res, A, B);
		else
			return CConv2C(res, B, A);
	}
	return false;
}
