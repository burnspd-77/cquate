

#include "matrix.h"
#include "my_errors.h"
#include <vector>
#include <sys/timeb.h>

using std::vector;

int nLastTimeMilli(0);

bool Length(cMatrix& Result, cMatrix& v)
{
	int nr = v.GetRows();
	int nc = v.GetColumns();

	Result.Create(1,1);
	double *pr = Result.GetR();

	if (nr>nc)
		pr[0] = double(nr);
	else
		pr[0] = double(nc);

	return false;
}

bool Size(cMatrix& Result, cMatrix& v)
{
	Result.Create(1,2);
	double *pr = Result.GetR();

	pr[0] = v.GetRows();
	pr[1] = v.GetColumns();

	return false;
}

bool Rows(cMatrix& Result, cMatrix& v)
{
	Result.Create(1,1);
	double *pr = Result.GetR();

	pr[0] = (double) v.GetRows();

	return false;
}

bool Columns(cMatrix& Result, cMatrix& v)
{
	Result.Create(1,1);
	double *pr = Result.GetR();

	pr[0] = (double) v.GetColumns();

	return false;
}

bool TimeLapse(cMatrix& Result)
{
	timeb tb;
	ftime(&tb);

	int nMilli = 1000 * (tb.time & 0xfffff) + tb.millitm;
	int trel = nMilli - nLastTimeMilli;

	Result.Delete();
	Result.SetTo( double(trel) );

	// update current time
	nLastTimeMilli = nMilli;

	return false;
}

bool TimeSet()
{
	timeb tb;
	ftime(&tb);

	int nMilli = 1000 * (tb.time & 0xfffff) + tb.millitm;
	nLastTimeMilli = nMilli;

	return false;
}

bool RAddR(cMatrix& res, const double x, const cMatrix& R)	// real matrix add
{
	int i;

	int nr = R.GetRows();
	int nc = R.GetColumns();

	if (res.Create(nr,nc)) return true;

	const double *pa = R.GetSafeR();
	for (i=0; i<nr*nc; i++) {
		res.SetR(i, x + pa[i]);
	}

	return false;
}

bool RAddR(cMatrix& res, const cMatrix& R1, const cMatrix& R2)		// real and real matrix add
{
	int i;

	double d;
	if (R1.IsScalar()) { R1.Get0(d); return RAddR( res, d, R2 ); }
	if (R2.IsScalar()) { R2.Get0(d); return RAddR( res, d, R1 ); }

	int nr = R1.GetRows();
	int nc = R1.GetColumns();

	if (nr!=R2.GetRows() || nc!=R2.GetColumns()) { addError("incompatible matrix dimensions"); return true; }

	if (res.Create(nr,nc)) return true;

	const double *p1 = R1.GetSafeR(), *p2 = R2.GetSafeR();
	for (i=0; i<nr*nc; i++) {
		res.SetR(i, p1[i] + p2[i]);
	}

	return false;
}

bool RAddC(cMatrix& res, const double x, const cMatrix& C)	// complex matrix add
{
	int i;

	int nr = C.GetRows();
	int nc = C.GetColumns();

	if (res.CreateComplex(nr,nc)) return true;

	const double *pr = C.GetSafeR();
	const double *pi = C.GetSafeI();
	for (i=0; i<nr*nc; i++) {
		res.SetR(i, x + pr[i]);
		res.SetI(i, pi[i]);
	}

	return false;
}

bool CAddC(cMatrix& res, const complex<double>& x, const cMatrix& C)	// complex scalar plus complex matrix
{
	int i;

	int nr = C.GetRows();
	int nc = C.GetColumns();

	if (res.CreateComplex(nr,nc)) return true;

	const double *pr = C.GetSafeR();
	const double *pi = C.GetSafeI();
	for (i=0; i<nr*nc; i++) {
		res.SetR(i, x.real() + pr[i]);
		res.SetI(i, x.imag() + pi[i]);
	}

	return false;
}

bool CAddR(cMatrix& res, const complex<double>& x, const cMatrix& R)	// complex scalar matrix add
{
	int i;

	int nr = R.GetRows();
	int nc = R.GetColumns();

	bool bReal = R.IsReal();

	if (res.CreateComplex(nr,nc)) return true;

	const double *pr = R.GetSafeR();
	for (i=0; i<nr*nc; i++) {
		res.SetR(i, x.real() + pr[i]);
		res.SetI(i, x.imag());
	}

	return false;
}

bool CAddR(cMatrix& res, const cMatrix& C, const cMatrix& R)	// complex and real matrix add
{
	int i;

	double d;
	complex<double> c;
	if (C.IsScalar()) { C.Get0(c); return CAddR( res, c, R ); }
	if (R.IsScalar()) { R.Get0(d); return RAddR( res, d, R ); }

	int nr = R.GetRows();
	int nc = R.GetColumns();

	if (nr!=C.GetRows() || nc!=C.GetColumns()) { addError("incompatible matrix dimensions"); return true; }

	if (res.CreateComplex(nr,nc)) return true;

	const double *pr = R.GetSafeR(), *pcr = C.GetSafeR(), *pci = C.GetSafeI();
	for (i=0; i<nr*nc; i++) {
		res.SetR(i, pr[i] + pcr[i]);
		res.SetI(i, pci[i]);
	}

	return false;
}

bool RAddC(cMatrix& res, const cMatrix& R, const cMatrix& C)		// real and complex matrix add
{
	return CAddR(res,C,R);
}

bool CAddC(cMatrix& res, const cMatrix& C1, const cMatrix& C2)		// complex and complex matrix add
{
	int i;

	complex<double> c;
	if (C1.IsScalar()) { C1.Get0(c); return CAddC( res, c, C2 ); }
	if (C2.IsScalar()) { C2.Get0(c); return CAddC( res, c, C1 ); }

	int nr = C1.GetRows();
	int nc = C1.GetColumns();

	if (nr!=C2.GetRows() || nc!=C2.GetColumns()) { addError("incompatible matrix dimensions"); return true; }

	if (res.CreateComplex(nr,nc)) return true;

	const double *pr1 = C1.GetSafeR(), *pi1 = C2.GetSafeR();
	const double *pr2 = C2.GetSafeR(), *pi2 = C2.GetSafeI();
	for (i=0; i<nr*nc; i++) {
		res.SetR(i, pr1[i] + pr2[i]);
		res.SetI(i, pi1[i] + pi2[i]);
	}

	return false;
}


bool RSubR(cMatrix& res, const double x, const cMatrix& R)		// real matrix sub
{
	int i;

	double d;
	if (R.IsScalar()) {
		R.Get0(d);
		res.Create(1,1);
		res.SetR(0, x-d);
		return false;
	}

	int nr = R.GetRows();
	int nc = R.GetColumns();

	if (res.Create(nr,nc)) return true;

	const double *pr = R.GetSafeR();
	double *pt = res.GetR();
	for (i=0; i<nr*nc; i++) {
		pt[i] = x - pr[i];
	}

	return false;
}

bool RSubR(cMatrix& res, const cMatrix& R, const double x)
{
	int i;

	double d;
	if (R.IsScalar()) {
		R.Get0(d);
		res.Create(1,1);
		res.SetR(0, d-x);
		return false;
	}

	int nr = R.GetRows();
	int nc = R.GetColumns();

	if (res.Create(nr,nc)) return true;

	const double *pr = R.GetSafeR();
	double *pt = res.GetR();
	for (i=0; i<nr*nc; i++) {
		pt[i] = pr[i] - x;
	}

	return false;
}

bool RSubC(cMatrix& res, const cMatrix& R, complex<double>& x)	// real complex sub
{
	int i;

	int nr = R.GetRows();
	int nc = R.GetColumns();

	if (res.CreateComplex(nr,nc)) return true;

	const double *pr = R.GetSafeR();
	const double *pi = R.GetSafeI();
	for (i=0; i<nr*nc; i++) {
		res.SetR(i, pr[i] - x.real());
		res.SetI(i, pi[i] - x.imag());
	}

	return false;
}

bool RSubR(cMatrix& res, const cMatrix& R1, const cMatrix& R2)
{
	int i;

	double d1, d2;
	if (R1.IsScalar()) { R1.Get0(d1); return RSubR( res, d1, R2 ); }
	if (R2.IsScalar()) { R2.Get0(d2); return RSubR( res, R1, d2 ); }

	int nr = R1.GetRows();
	int nc = R1.GetColumns();

	if (nr!=R2.GetRows() || nc!=R2.GetColumns()) { addError("incompatible matrix dimensions"); return true; }

	if (res.Create(nr,nc)) return true;

	const double *pr1 = R1.GetSafeR(), *pr2 = R2.GetSafeR();
	double *pt = res.GetR();
	for (i=0; i<nr*nc; i++) {
		pt[i] = pr1[i] - pr2[i];
	}

	return false;
}

bool RSubC(cMatrix& res, const double x, const cMatrix& C)		// complex matrix sub
{
	int nr = C.GetRows();
	int nc = C.GetColumns();
	int sz = nr*nc;

	if (res.CreateComplex(nr,nc)) return true;

	const double *pr = C.GetSafeR(), *pi = C.GetSafeI();
	double *ptr = res.GetR();
	double *pti = res.GetI();

	for (int i=0; i<sz; i++) {
		ptr[i] = x - pr[i];
		pti[i] = -pi[i];
	}

	return false;
}

bool CSubR(cMatrix& res, const cMatrix& C, const cMatrix& R)	// complex and real matrix sub
{
	int i;

	double d;
	complex<double> c;
	if (C.IsScalar()) { C.Get0(c); return CSubR( res, c, R ); }
	if (R.IsScalar()) { R.Get0(d); return CSubR( res, C, d ); }

	int nr = R.GetRows();
	int nc = R.GetColumns();

	if (nr!=C.GetRows() || nc!=C.GetColumns()) { addError("incompatible matrix dimensions"); return true; }

	if (res.CreateComplex(nr,nc)) return true;

	const double *pr = R.GetSafeR(), *pcr = C.GetSafeR(), *pci = C.GetSafeI();
	for (i=0; i<nr*nc; i++) {
		res.SetR(i, pcr[i] - pr[i]);
		res.SetI(i, pci[i]);
	}

	return false;
}

bool RSubC(cMatrix& res, const cMatrix& R, const cMatrix& C)	// real and complex matrix add
{
	int i;

	double d;
	complex<double> c;
	if (R.IsScalar()) { R.Get0(d); return RSubC( res, d, C ); }
	if (C.IsScalar()) { C.Get0(c); return RSubC( res, R, c ); }

	int nr = R.GetRows();
	int nc = R.GetColumns();

	if (nr!=C.GetRows() || nc!=C.GetColumns()) { addError("incompatible matrix dimensions"); return true; }

	if (res.CreateComplex(nr,nc)) return true;

	const double *pr = R.GetSafeR(), *pcr = C.GetSafeR(), *pci = C.GetSafeI();
	for (i=0; i<nr*nc; i++) {
		res.SetR(i, pr[i] - pcr[i]);
		res.SetI(i, -pci[i]);
	}

	return false;
}

bool CSubC(cMatrix& res, const cMatrix& C1, const cMatrix& C2)	// complex and complex matrix add
{
	int i;

	complex<double> c;
	if (C1.IsScalar()) { C1.Get0(c); return CSubC( res, c, C2 ); }
	if (C2.IsScalar()) { C2.Get0(c); return CSubC( res, C1, c ); }

	int nr = C1.GetRows();
	int nc = C1.GetColumns();

	if (nr!=C2.GetRows() || nc!=C2.GetColumns()) { addError("incompatible matrix dimensions"); return true; }

	if (res.CreateComplex(nr,nc)) return true;

	const double *pr1 = C1.GetSafeR(), *pi1 = C2.GetSafeR();
	const double *pr2 = C2.GetSafeR(), *pi2 = C2.GetSafeI();
	for (i=0; i<nr*nc; i++) {
		res.SetR(i, pr1[i] - pr2[i]);
		res.SetI(i, pi1[i] - pi2[i]);
	}

	return false;
}

bool CSubR(cMatrix& res, const complex<double>& x, const cMatrix& R)	// complex and real matrix sub
{
	int i;

	int nr = R.GetRows();
	int nc = R.GetColumns();

	if (res.CreateComplex(nr,nc)) return true;

	const double *pr = R.GetSafeR();
	for (i=0; i<nr*nc; i++) {
		res.SetR(i, x.real() - pr[i]);
		res.SetI(i, x.imag());
	}

	return false;
}

bool CSubR(cMatrix& res, const cMatrix& C, const double x)
{
	int i;

	int nr = C.GetRows();
	int nc = C.GetColumns();

	if (res.CreateComplex(nr,nc)) return true;

	const double *pr = C.GetSafeR(), *pi = C.GetSafeI();
	for (i=0; i<nr*nc; i++) {
		res.SetR(i, pr[i] - x);
		res.SetI(i, pi[i]);
	}

	return false;
}

bool CSubC(cMatrix& res, const complex<double>& x, const cMatrix& C)	// complex and complex matrix sub
{
	int i;

	int nr = C.GetRows();
	int nc = C.GetColumns();

	if (res.CreateComplex(nr,nc)) return true;

	const double *pr = C.GetSafeR();
	const double *pi = C.GetSafeI();
	for (i=0; i<nr*nc; i++) {
		res.SetR(i, x.real() - pr[i]);
		res.SetI(i, x.imag() - pi[i]);
	}

	return false;
}

bool CSubC(cMatrix& res, const cMatrix& C, const complex<double>& x)	// complex and complex matrix add
{
	int i;

	int nr = C.GetRows();
	int nc = C.GetColumns();

	if (res.CreateComplex(nr,nc)) return true;

	const double *pr = C.GetSafeR();
	const double *pi = C.GetSafeI();
	for (i=0; i<nr*nc; i++) {
		res.SetR(i, pr[i] - x.real());
		res.SetI(i, pi[i] - x.imag());
	}

	return false;
}


bool RMultR(cMatrix& res, double x, cMatrix& R)		// real scalar, real matrix
{
	if (res.Create(R.GetRows(),R.GetColumns())) return true;

	double *pr = res.GetR();
	const double *parg = R.GetSafeR();
	int n = R.GetSize();
	for (int i=0; i<n; ++i) pr[i] = x * parg[i];
	return false;
}

bool RMultR(cMatrix& res, cMatrix& R1, cMatrix& R2)
{
	cMatrix va;
	if (res.Create(R1.GetRows(),R2.GetColumns())) return true;

	if (va.Create(R1.GetColumns(),1)) return true;

	int i, j, k;
	//double *pa = va.GetR();
	const double *p1 = R1.GetSafeR(), *p2 = R2.GetSafeR();
	double *pres = res.GetR();

	int nr1 = R1.GetRows(), nc1 = R1.GetColumns();
	int nr2 = R2.GetRows(), nc2 = R2.GetColumns();

	for (i=0; i<nr1; i++) {
		for (k=0; k<nc1; k++) { va.mr[k] = R1.mr[i+k*nr1]; }	// = A.Get(i,k);	// va is ith row of R1
		for (j=0; j<nc2; j++) {
			//pres[i+j*nr1] = InnerProduct( pa, p2+j*nr2, nc1 );
			pres[i+j*nr1] = InnerProduct( va.mr.begin(), R2.mr.begin()+j*nr2, nc1 );
		}
	}

	return false;
}

bool RMultC(cMatrix& res, cMatrix& R, cMatrix& C)
{
	cMatrix va;
	if (res.CreateComplex(R.GetRows(),C.GetColumns())) return true;

	if (va.Create(R.GetColumns(),1)) return true;

	int i, j, k;
	//double *pa = va.GetR();
	const double *pr = R.GetSafeR(), *pcr = C.GetSafeR(), *pci = C.GetSafeI();
	double *presr = res.GetR(), *presi = res.GetI();

	int nr1 = R.GetRows(), nc1 = R.GetColumns();
	int nr2 = C.GetRows(), nc2 = C.GetColumns();
	complex<double> c;

	for (i=0; i<nr1; i++) {
		for (k=0; k<nc1; k++) { va.mr[k] = pr[i+k*nr1]; }	// = A.Get(i,k);	// va is ith row of R1
		for (j=0; j<nc2; j++) {
			c = InnerProduct( va.mr.begin(), C.mr.begin()+j*nr2, C.mi.begin()+j*nr2, nc1 );
			presr[i+j*nr1] = c.real();
			presi[i+j*nr1] = c.imag();
		}
	}

	return false;
}

bool RMultC(cMatrix& res, double x, cMatrix& C)
{
	if (res.CreateComplex(C.GetRows(),C.GetColumns())) return true;

	double *pr = res.GetR(), *pi = res.GetI();
	const double *pargr = C.GetSafeR(), *pargi = C.GetSafeI();
	int n = C.GetSize();
	for (int i=0; i<n; ++i) { pr[i] = x * pargr[i]; pi[i] = x * pargi[i]; }
	return false;
}

bool CMultR(cMatrix& res, complex<double>& c, cMatrix& R)
{
	if (res.CreateComplex(R.GetRows(),R.GetColumns())) return true;

	double *pr = res.GetR(), *pi = res.GetI();
	const double *parg = R.GetSafeR();
	int n = R.GetSize();
	double cr = c.real(), ci = c.imag();
	for (int i=0; i<n; ++i) { pr[i] = cr * parg[i]; pi[i] = ci * parg[i]; }
	return false;
}

bool CMultR(cMatrix& res, cMatrix& C, cMatrix& R)
{
	cMatrix va;
	if (res.CreateComplex(C.GetRows(),R.GetColumns())) return true;

	if (va.CreateComplex(C.GetColumns(),1)) return true;

	int i, j, k;
	double *par = va.GetR(), *pai = va.GetI();
	//const double *pcr = C.GetSafeR(), *pci = C.GetSafeI(), *pr = R.GetSafeR();
	double *presr = res.GetR(), *presi = res.GetI();

	int nr1 = C.GetRows(), nc1 = C.GetColumns();
	int nr2 = R.GetRows(), nc2 = R.GetColumns();
	complex<double> c;

	for (i=0; i<nr1; i++) {
		for (k=0; k<nc1; k++) { par[k] = C.mr[i+k*nr1]; pai[k] = C.mi[i+k*nr1]; }	// = A.Get(i,k);	// va is ith row of R1
		for (j=0; j<nc2; j++) {
			c = InnerProduct( R.mr.begin()+j*nr2, va.mr.begin(), va.mi.begin(), nc1 );
			presr[i+j*nr1] = c.real();
			presi[i+j*nr1] = c.imag();
		}
	}

	return false;
}

bool CMultC(cMatrix& res, complex<double>& c, cMatrix& C)
{
	if (res.CreateComplex(C.GetRows(),C.GetColumns())) return true;

	double *pr = res.GetR(), *pi = res.GetI();
	const double *pargr = C.GetSafeR(), *pargi = C.GetSafeI();
	int n = C.GetSize();
	double cr = c.real(), ci = c.imag();
	for (int i=0; i<n; ++i) {
		pr[i] = cr * pargr[i] - ci * pargi[i];
		pi[i] = cr * pargi[i] + ci * pargr[i];
	}
	return false;
}

bool CMultC(cMatrix& res, cMatrix& C1, cMatrix& C2)
{
	cMatrix va;
	if (res.CreateComplex(C1.GetRows(),C2.GetColumns())) return true;

	if (va.CreateComplex(C1.GetColumns(),1)) return true;

	int i, j, k;
	//double *pvr = va.GetR(), *pvi = va.GetI();
	//const double *pc1r = C1.GetSafeR(), *pc1i = C1.GetSafeI();
	//const double *pc2r = C2.GetSafeR(), *pc2i = C2.GetSafeI();
	//double *presr = res.GetR(), *presi = res.GetI();

	int nr1 = C1.GetRows(), nc1 = C1.GetColumns();
	int nr2 = C2.GetRows(), nc2 = C2.GetColumns();
	complex<double> c;

	for (i=0; i<nr1; i++) {
		for (k=0; k<nc1; k++) { va.mr[k] = C1.mr[i+k*nr1]; va.mi[k] = C1.mi[i+k*nr1]; }	// = A.Get(i,k);	// va is ith row of R1
		for (j=0; j<nc2; j++) {
			c = InnerProduct( va.mr.begin(), va.mi.begin(), C2.mr.begin()+j*nr2, C2.mi.begin()+j*nr2, nc1 );
			res.mr[i+j*nr1] = c.real();
			res.mi[i+j*nr1] = c.imag();
		}
	}

	return false;
}


//
// DOTMULT
//
bool RDotMultR(cMatrix& Result, const cMatrix& R, const double d)
{
	if (Result.Create(R.GetRows(),R.GetColumns())) return true;

	//double *pr = Result.GetR();
	//const double *parg = R.GetSafeR();
	int n = R.GetSize();
	for (int i=0; i<n; ++i) Result.mr[i] = R.mr[i] * d;
	return false;
}

bool RDotMultR(cMatrix& Result, const double d, const cMatrix& R)
{
	if (Result.Create(R.GetRows(),R.GetColumns())) return true;

	//double *pr = Result.GetR();
	//const double *parg = R.GetSafeR();
	int n = R.GetSize();
	for (int i=0; i<n; ++i) Result.mr[i] = d * R.mr[i];
	return false;
}

bool RDotMultR(cMatrix& Result, const cMatrix& R1, const cMatrix& R2)
{
	if (Result.Create(R1.GetRows(),R1.GetColumns())) return true;

	//double *pr = Result.GetR();
	//const double *parg1 = R1.GetSafeR();
	//const double *parg2 = R2.GetSafeR();
	int n = R1.GetSize();
	for (int i=0; i<n; ++i) Result.mr[i] = R1.mr[i] * R2.mr[i];
	return false;
}

bool RDotMultC(cMatrix& Result, const cMatrix& R, const complex<double>& c)
{
	if (Result.CreateComplex(R.GetRows(),R.GetColumns())) return true;

	//double *pr = Result.GetR();
	//double *pi = Result.GetI();
	//const double *parg = R.GetSafeR();
	double cr = c.real(), ci = c.imag();
	int n = R.GetSize();
	for (int i=0; i<n; ++i) { Result.mr[i] = R.mr[i]*cr; Result.mi[i] = R.mr[i]*ci; }
	return false;
}

bool RDotMultC(cMatrix& Result, const double d, const cMatrix& C)
{ return CDotMultR(Result, C, d); }

bool RDotMultC(cMatrix& Result, const cMatrix& R, const cMatrix& C)
{
	if (Result.CreateComplex(R.GetRows(),R.GetColumns())) return true;

	//double *pr = Result.GetR();
	//double *pi = Result.GetI();
	//const double *prr = R.GetSafeR();
	//const double *pcr = C.GetSafeR();
	//const double *pci = C.GetSafeI();

	int n = R.GetSize();
	for (int i=0; i<n; ++i) { Result.mr[i] = R.mr[i]*C.mr[i]; Result.mi[i] = R.mr[i]*C.mi[i]; }
	return false;
}

bool CDotMultR(cMatrix& Result, const complex<double>& c, const cMatrix& R)
{ return RDotMultC(Result,R,c); }

bool CDotMultR(cMatrix& Result, const cMatrix& C, const double d)
{
	if (Result.CreateComplex(C.GetRows(),C.GetColumns())) return true;

	//double *pr = Result.GetR();
	//double *pi = Result.GetI();
	//const double *pargr = C.GetSafeR();
	//const double *pargi = C.GetSafeI();
	int n = C.GetSize();
	for (int i=0; i<n; ++i) { Result.mr[i] = C.mr[i]*d; Result.mi[i] = C.mi[i]*d; }
	return false;
}

bool CDotMultR(cMatrix& Result, const cMatrix& C, const cMatrix& R)
{ return RDotMultC(Result, R, C); }

bool CDotMultC(cMatrix& Result, const cMatrix& C, const complex<double>& c)
{
	if (Result.CreateComplex(C.GetRows(),C.GetColumns())) return true;

	//double *pr = Result.GetR();
	//double *pi = Result.GetI();
	//const double *pargr = C.GetSafeR();
	//const double *pargi = C.GetSafeI();
	double cr = c.real(), ci = c.imag();
	int n = C.GetSize();
	for (int i=0; i<n; ++i) {
		Result.mr[i] = C.mr[i]*cr - C.mi[i]*ci;
		Result.mi[i] = C.mi[i]*cr + C.mr[i]*ci;
	}
	return false;
}

bool CDotMultC(cMatrix& Result, const complex<double>& c, const cMatrix& C)
{ return CDotMultC(Result,C,c); }

bool CDotMultC(cMatrix& Result, const cMatrix& C1, const cMatrix& C2)
{
	if (Result.CreateComplex(C1.GetRows(),C1.GetColumns())) return true;

	//double *pr = Result.GetR();
	//double *pi = Result.GetI();
	//const double *pr1 = C1.GetSafeR();
	//const double *pi1 = C1.GetSafeI();
	//const double *pr2 = C2.GetSafeR();
	//const double *pi2 = C2.GetSafeI();

	int n = C1.GetSize();
	for (int i=0; i<n; ++i) {
		Result.mr[i] = C1.mr[i]*C2.mr[i] - C1.mi[i]*C2.mi[i];
		Result.mi[i] = C1.mr[i]*C2.mi[i] + C1.mi[i]*C2.mr[i];
	}
	return false;
}

// interface fcn
bool DotMult(cMatrix& Result, cMatrix& A1, cMatrix& A2 )
{
	if (SameSize(A1,A2)) {
		if (A1.IsReal() && A2.IsReal()) return RDotMultR(Result,A1,A2);
		else if (A1.IsReal() && !A2.IsReal()) return RDotMultC(Result,A1,A2);
		else if (!A1.IsReal() && A2.IsReal()) return CDotMultR(Result,A1,A2);
		else /* (!A1.IsReal() && !A2.IsReal()) */ return CDotMultC(Result,A1,A2);
	}
	else if (A1.IsScalar()) {
		if (A1.IsReal() && A2.IsReal()) return RDotMultR(Result,A1.Get0R(),A2);
		else if (A1.IsReal() && !A2.IsReal()) return RDotMultC(Result,A1.Get0R(),A2);
		else if (!A1.IsReal() && A2.IsReal()) return CDotMultR(Result,A1.Get0C(),A2);
		else /* (!A1.IsReal() && !A2.IsReal()) */ return CDotMultC(Result,A1.Get0C(),A2);
	}
	else if (A2.IsScalar()) {
		if (A1.IsReal() && A2.IsReal()) return RDotMultR(Result,A1,A2.Get0R());
		else if (A1.IsReal() && !A2.IsReal()) return RDotMultC(Result,A1,A2.Get0C());
		else if (!A1.IsReal() && A2.IsReal()) return CDotMultR(Result,A1,A2.Get0R());
		else /* (!A1.IsReal() && !A2.IsReal()) */ return CDotMultC(Result,A1,A2.Get0C());
	}
	else
		{ addError("incompatable matrix dimensions"); return true; }
}


//
// DOTDIV
//
bool RDotDivR(cMatrix& Result, const cMatrix& R, const double d)
{
	if (Result.Create(R.GetRows(),R.GetColumns())) return true;

	//double *pr = Result.GetR();
	//const double *parg = R.GetSafeR();
	int n = R.GetSize();
	for (int i=0; i<n; ++i) Result.mr[i] = R.mr[i] / d;
	return false;
}

bool RDotDivR(cMatrix& Result, const double d, const cMatrix& R)
{
	if (Result.Create(R.GetRows(),R.GetColumns())) return true;

	//double *pr = Result.GetR();
	//const double *parg = R.GetSafeR();
	int n = R.GetSize();
	for (int i=0; i<n; ++i) Result.mr[i] = d / R.mr[i];
	return false;
}
bool RDotDivR(cMatrix& Result, const cMatrix& R1, const cMatrix& R2)
{
	if (Result.Create(R1.GetRows(),R1.GetColumns())) return true;

	//double *pr = Result.GetR();
	//const double *parg1 = R1.GetSafeR();
	//const double *parg2 = R2.GetSafeR();
	int n = R1.GetSize();
	for (int i=0; i<n; ++i) Result.mr[i] = R1.mr[i] / R2.mr[i];
	return false;
}

bool RDotDivC(cMatrix& Result, const cMatrix& R, const complex<double>& c)
{
	if (Result.CreateComplex(R.GetRows(),R.GetColumns())) return true;

	//double *pr = Result.GetR();
	//double *pi = Result.GetI();
	//const double *prr = R.GetSafeR();
	complex<double> cv;

	int n = R.GetSize();
	double ang = arg(c);
	double mag = abs(c);
	double ca_m = cos(-ang) / mag;
	double sa_m = sin(-ang) / mag;

	for (int i=0; i<n; ++i) {
		Result.mr[i] = R.mr[i] * ca_m;
		Result.mi[i] = R.mr[i] * sa_m;
	}
	return false;
}

bool RDotDivC(cMatrix& Result, const double d, const cMatrix& C)
{
	if (Result.CreateComplex(C.GetRows(),C.GetColumns())) return true;

	//double *pr = Result.GetR();
	//double *pi = Result.GetI();
	//const double *pargr = C.GetSafeR();
	//const double *pargi = C.GetSafeI();
	int n = C.GetSize();
	double ang, mag;
	for (int i=0; i<n; ++i) {
		mag = sqrt(C.mr[i]*C.mr[i]+C.mi[i]*C.mi[i]);
		ang = atan2(C.mi[i],C.mr[i]);
		Result.mr[i] = d / mag * cos(-ang);
		Result.mi[i] = d / mag * sin(-ang);
	}
	return false;
}

bool RDotDivC(cMatrix& Result, const cMatrix& R, const cMatrix& C)
{
	if (Result.CreateComplex(C.GetRows(),C.GetColumns())) return true;

	//double *pr = Result.GetR();
	//double *pi = Result.GetI();
	//const double *prr = R.GetSafeR();
	//const double *pcr = C.GetSafeR();
	//const double *pci = C.GetSafeI();
	int n = C.GetSize();
	double ang, mag;
	for (int i=0; i<n; ++i) {
		mag = sqrt(C.mr[i]*C.mr[i]+C.mi[i]*C.mi[i]);
		ang = atan2(C.mi[i],C.mr[i]);
		Result.mr[i] = R.mr[i] / mag * cos(-ang);
		Result.mi[i] = R.mr[i] / mag * sin(-ang);
	}
	return false;
}

bool CDotDivR(cMatrix& Result, const complex<double>& c, const cMatrix& R)
{
	if (Result.CreateComplex(R.GetRows(),R.GetColumns())) return true;

	//double *pr = Result.GetR();
	//double *pi = Result.GetI();
	//const double *prr = R.GetSafeR();
	int n = R.GetSize();
	double ang = arg(c);
	double mag = abs(c);
	double sa = sin(ang), ca = cos(ang);
	double r;
	for (int i=0; i<n; ++i) {
		r = mag / R.mr[i];
		Result.mr[i] = r * ca;
		Result.mi[i] = r * sa;
	}
	return false;
}

bool CDotDivR(cMatrix& Result, const cMatrix& C, const double d)
{
	if (Result.CreateComplex(C.GetRows(),C.GetColumns())) return true;

	//double *pr = Result.GetR();
	//double *pi = Result.GetI();
	//const double *pargr = C.GetSafeR();
	//const double *pargi = C.GetSafeI();
	int n = C.GetSize();
	for (int i=0; i<n; ++i) {
		Result.mr[i] = C.mr[i] / d;
		Result.mi[i] = C.mi[i] / d;
	}
	return false;
}

bool CDotDivR(cMatrix& Result, const cMatrix& C, const cMatrix& R)
{
	if (Result.CreateComplex(C.GetRows(),C.GetColumns())) return true;

	//double *pr = Result.GetR();
	//double *pi = Result.GetI();
	//const double *prr = R.GetSafeR();
	//const double *pcr = C.GetSafeR();
	//const double *pci = C.GetSafeI();
	int n = C.GetSize();
	for (int i=0; i<n; ++i) {
		Result.mr[i] = C.mr[i] / R.mr[i];
		Result.mi[i] = C.mi[i] / R.mr[i];
	}
	return false;
}

bool CDotDivC(cMatrix& Result, const cMatrix& C, const complex<double>& c)
{
	if (Result.CreateComplex(C.GetRows(),C.GetColumns())) return true;

	//double *pr = Result.GetR();
	//double *pi = Result.GetI();
	//const double *pcr = C.GetSafeR();
	//const double *pci = C.GetSafeI();
	int n = C.GetSize();
	double ang = arg(c), angc;
	double mag = abs(c), magc;
	double r;
	for (int i=0; i<n; ++i) {
		magc = sqrt(C.mr[i]*C.mr[i]+C.mi[i]*C.mi[i]);
		angc = atan2(C.mr[i],C.mi[i]);
		r = magc / mag;
		Result.mr[i] = r * cos(angc-ang);
		Result.mi[i] = r * sin(angc-ang);
	}
	return false;
}

bool CDotDivC(cMatrix& Result, const complex<double>& c, const cMatrix& C)
{
	if (Result.CreateComplex(C.GetRows(),C.GetColumns())) return true;

	//double *pr = Result.GetR();
	//double *pi = Result.GetI();
	//const double *pcr = C.GetSafeR();
	//const double *pci = C.GetSafeR();
	int n = C.GetSize();
	double ang = arg(c), angc;
	double mag = abs(c), magc;
	double r;
	for (int i=0; i<n; ++i) {
		magc = sqrt(C.mr[i]*C.mr[i]+C.mi[i]*C.mi[i]);
		angc = atan2(C.mi[i],C.mr[i]);
		r = mag / magc;
		Result.mr[i] = r * cos(ang-angc);
		Result.mi[i] = r * sin(ang-angc);
	}
	return false;
}

bool CDotDivC(cMatrix& Result, const cMatrix& C1, const cMatrix& C2)
{
	if (Result.CreateComplex(C1.GetRows(),C1.GetColumns())) return true;

	//double *pr = Result.GetR();
	//double *pi = Result.GetI();
	//const double *pcr1 = C1.GetSafeR();
	//const double *pci1 = C1.GetSafeI();
	//const double *pcr2 = C2.GetSafeR();
	//const double *pci2 = C2.GetSafeI();
	int n = C1.GetSize();
	double r;
	double ang1, mag1, ang2, mag2;
	for (int i=0; i<n; ++i) {
		mag1 = sqrt(C1.mr[i]*C1.mr[i]+C1.mi[i]*C1.mi[i]);
		ang1 = atan2(C1.mi[i],C1.mr[i]);
		mag2 = sqrt(C2.mr[i]*C2.mr[i]+C2.mi[i]*C2.mi[i]);
		ang2 = atan2(C2.mi[i],C2.mr[i]);
		r = mag1 / mag2;
		Result.mr[i] = r * cos(ang1-ang2);
		Result.mi[i] = r * sin(ang1-ang2);
	}
	return false;
}

// interface fcn
bool DotDiv(cMatrix& Result, cMatrix& A1, cMatrix& A2 )
{
	if (SameSize(A1,A2)) {
		if (A1.IsReal() && A2.IsReal()) return RDotDivR(Result,A1,A2);
		else if (A1.IsReal() && !A2.IsReal()) return RDotDivC(Result,A1,A2);
		else if (!A1.IsReal() && A2.IsReal()) return CDotDivR(Result,A1,A2);
		else /* (!A1.IsReal() && !A2.IsReal()) */ return CDotDivC(Result,A1,A2);
	}
	else if (A1.IsScalar()) {
		if (A1.IsReal() && A2.IsReal()) return RDotDivR(Result,A1.Get0R(),A2);
		else if (A1.IsReal() && !A2.IsReal()) return RDotDivC(Result,A1.Get0R(),A2);
		else if (!A1.IsReal() && A2.IsReal()) return CDotDivR(Result,A1.Get0C(),A2);
		else /* (!A1.IsReal() && !A2.IsReal()) */ return CDotDivC(Result,A1.Get0C(),A2);
	}
	else if (A2.IsScalar()) {
		if (A1.IsReal() && A2.IsReal()) return RDotDivR(Result,A1,A2.Get0R());
		else if (A1.IsReal() && !A2.IsReal()) return RDotDivC(Result,A1,A2.Get0C());
		else if (!A1.IsReal() && A2.IsReal()) return CDotDivR(Result,A1,A2.Get0R());
		else /* (!A1.IsReal() && !A2.IsReal()) */ return CDotDivC(Result,A1,A2.Get0C());
	}
	else { addError("incompatable matrix dimensions"); return true; }
}

bool DotDiv(cMatrix& Result, cMatrix& A1, double d )
{
	if (A1.IsReal())
		return RDotDivR(Result, A1, d);
	else
		return CDotDivR(Result, A1, d);
}

bool DotDiv(cMatrix& Result, double d, cMatrix& A2 )
{
	if (A2.IsReal())
		return RDotDivR(Result, d, A2);
	else
		return RDotDivC(Result, d, A2);
}

//
// POW
//
complex<double> CPowC(complex<double>& x, complex<double>& y)
{
	complex<double> term1, term2, temp, temp2;
	double phasex, val, val2, absx, dtemp, dtemp2;

	absx = abs(x);
	phasex = arg(x);

	val = pow(absx,y.real());
	val2 = y.real()*phasex;

	temp = complex<double>(0.,val2);
	term1 = val*ExpC(temp);

	dtemp = y.imag() * log(absx);
	dtemp = exp(dtemp);

	dtemp2 = exp(-1.*phasex*y.imag());
	
	return term1*dtemp*dtemp2;
}

complex<double> CPow(complex<double>& x, double d)
{
	double xarg = arg(x);

	double newmag = pow(abs(x), d);
	double newphase = arg(x)*d;
	return std::polar(newmag,newphase);
}

complex<double> PowC(double d, complex<double>& y)
{
	complex<double> x(d,0.);
	return CPowC(x, y);
}

bool RDotPowR(cMatrix& Result, cMatrix& R, double d)
{
	if (Result.Create(R.GetRows(),R.GetColumns())) return true;

	int n = R.GetSize();
	for (int i=0; i<n; ++i)
		Result.mr[i] = pow( R.mr[i], d );

	return false;
}

bool RDotPowR(cMatrix& Result, double d, cMatrix& R)
{
	if (Result.Create(R.GetRows(),R.GetColumns())) return true;

	int n = R.GetSize();
	for (int i=0; i<n; ++i)
		Result.mr[i] = pow( d, R.mr[i] );

	return false;
}

bool RDotPowR(cMatrix& Result, cMatrix& R1, cMatrix& R2)
{
	if (Result.Create(R1.GetRows(),R1.GetColumns())) return true;

	int n = R1.GetSize();
	for (int i=0; i<n; ++i)
		Result.mr[i] = pow( R1.mr[i], R2.mr[i] );

	return false;
}

bool RDotPowC(cMatrix& Result, double d, cMatrix& C)
{
	if (Result.CreateComplex(C.GetRows(),C.GetColumns())) return true;

	std::complex<double> cv, cv1;

	int n = C.GetSize();
	for (int i=0; i<n; ++i) {
		cv = complex<double>(C.mr[i],C.mi[i]);
		cv1 = PowC( d, cv );
		Result.mr[i] = cv1.real();
		Result.mi[i] = cv1.imag();
	}
	return false;
}

bool RDotPowC(cMatrix& Result, cMatrix& R, std::complex<double>& c)
{
	if (Result.CreateComplex(R.GetRows(),R.GetColumns())) return true;

	int n = R.GetSize();
	std::complex<double> cv;

	for (int i=0; i<n; ++i) {
		cv = PowC( R.mr[i], c );
		Result.mr[i] = cv.real();
		Result.mi[i] = cv.imag();
	}

	return false;
}

bool RDotPowC(cMatrix& Result, cMatrix& R, cMatrix& C)
{
	if (Result.CreateComplex(C.GetRows(),C.GetColumns())) return true;

	int n = C.GetSize();
	std::complex<double> cv, cv1;

	for (int i=0; i<n; ++i) {
		C.GetC(cv1,i);
		cv = PowC( R.mr[i], cv1 );
		Result.mr[i] = cv.real();
		Result.mi[i] = cv.imag();
	}

	return false;
}

bool CDotPowR(cMatrix& Result, std::complex<double>& c, cMatrix& R)
{
	if (Result.CreateComplex(R.GetRows(),R.GetColumns())) return true;

	int n = R.GetSize();
	std::complex<double> cv;

	for (int i=0; i<n; ++i) {
		cv = CPow( c, R.mr[i] );
		Result.mr[i] = cv.real();
		Result.mi[i] = cv.imag();
	}

	return false;
}

bool CDotPowR(cMatrix& Result, cMatrix& C, double d)
{
	if (Result.CreateComplex(C.GetRows(),C.GetColumns())) return true;

	int n = C.GetSize();
	std::complex<double> cv, cv1;

	for (int i=0; i<n; ++i) {
		C.GetC(cv1,i);
		cv = CPow( cv1, d );
		Result.mr[i] = cv.real();
		Result.mi[i] = cv.imag();
	}

	return false;
}

bool CDotPowR(cMatrix& Result, cMatrix& C, cMatrix& R)
{
	if (Result.CreateComplex(C.GetRows(),C.GetColumns())) return true;

	int n = C.GetSize();
	std::complex<double> cv, cv1, cv2;

	for (int i=0; i<n; ++i) {
		C.GetC(cv1,i);
		cv = CPow( cv1, R.mr[i] );
		Result.mr[i] = cv.real();
		Result.mi[i] = cv.imag();
	}

	return false;
}

bool CDotPowC(cMatrix& Result, std::complex<double>& c, cMatrix& C)
{
	if (Result.CreateComplex(C.GetRows(),C.GetColumns())) return true;

	int n = C.GetSize();
	std::complex<double> cv, cv1;

	for (int i=0; i<n; ++i) {
		C.GetC(cv1,i);
		cv = CPowC( c, cv1 );
		Result.mr[i] = cv.real();
		Result.mi[i] = cv.imag();
	}

	return false;
}

bool CDotPowC(cMatrix& Result, cMatrix& C, std::complex<double>& c)
{
	if (Result.CreateComplex(C.GetRows(),C.GetColumns())) return true;

	int n = C.GetSize();
	std::complex<double> cv, cv1;

	for (int i=0; i<n; ++i) {
		C.GetC(cv1,i);
		cv = CPowC( cv1, c );
		Result.mr[i] = cv.real();
		Result.mi[i] = cv.imag();
	}

	return false;
}

bool CDotPowC(cMatrix& Result, cMatrix& C1, cMatrix& C2)
{
	if (Result.CreateComplex(C1.GetRows(),C1.GetColumns())) return true;

	int n = C1.GetSize();
	std::complex<double> cv, cv1, cv2;

	for (int i=0; i<n; ++i) {
		C1.GetC(cv1,i);
		C2.GetC(cv2,i);
		cv = CPowC( cv1, cv2 );
		Result.mr[i] = cv.real();
		Result.mi[i] = cv.imag();
	}

	return false;
}

bool DotPow(cMatrix& Result, cMatrix& A1, cMatrix& A2)
{
	std::complex<double> clf;

	if (SameSize(A1,A2)) {
		if (A1.IsReal() && A2.IsReal()) return RDotPowR(Result,A1,A2);
		else if (A1.IsReal() && !A2.IsReal()) return RDotPowC(Result,A1,A2);
		else if (!A1.IsReal() && A2.IsReal()) return CDotPowR(Result,A1,A2);
		else if (!A1.IsReal() && !A2.IsReal()) return CDotPowC(Result,A1,A2);
	}
	else if (A1.IsScalar()) {
		if (A1.IsReal() && A2.IsReal()) return RDotPowR(Result,A1.Get0R(),A2);
		else if (A1.IsReal() && !A2.IsReal()) return RDotPowC(Result,A1.Get0R(),A2);
		else if (!A1.IsReal() && A2.IsReal()) {
			clf = A1.Get0C();
			return CDotPowR(Result,clf,A2);
		}
		else if (!A1.IsReal() && !A2.IsReal()) {
			clf = A1.Get0C();
			return CDotPowC(Result,clf,A2);
		}
	}
	else if (A2.IsScalar()) {
		if (A1.IsReal() && A2.IsReal()) return RDotPowR(Result,A1,A2.Get0R());
		else if (A1.IsReal() && !A2.IsReal()) {
			clf = A2.Get0C();
			return RDotPowC(Result,A1,clf);
		}
		else if (!A1.IsReal() && A2.IsReal()) return CDotPowR(Result,A1,A2.Get0R());
		else if (!A1.IsReal() && !A2.IsReal()) {
			clf = A2.Get0C();
			return CDotPowC(Result,A1,clf);
		}
	}
	else { addError("incompatable matrix dimensions"); return true; }

	addError("reached an illogical point!");
	return true;
}

bool DotPow(cMatrix& Result, double A1, cMatrix& A2)
{
	if (A2.IsReal()) return RDotPowR(Result,A1,A2);
	else return RDotPowC(Result,A1,A2);
}

bool DotPow(cMatrix& Result, cMatrix& A1, double A2)
{
	if (A1.IsReal()) return RDotPowR(Result,A1,A2);
	else return CDotPowR(Result,A1,A2);
}


//
// Overloaded operators
//

//
// "+"
//

// Important helpers:
//cMatrix Add(double x, cMatrix& R);	// real matrix add
//cMatrix Add(cMatrix& R, cMatrix& R);	// real and real matrix add
//cMatrix AddC(double x, cMatrix& C);	// complex matrix add
//cMatrix CAdd(cMatrix& C, cMatrix& R);	// complex and real matrix add
//cMatrix AddC(cMatrix& R, cMatrix& C);	// real and complex matrix add
//cMatrix CAddC(cMatrix& C, cMatrix& C);	// complex and complex matrix add
//
//cMatrix cMatrix::operator+(const cMatrix& A)
//{
//	cMatrix res;
//	Add(res,*this,A);
//	return res;
//}
//
//cMatrix cMatrix::operator+(const double x) const
//{
//	cMatrix res;
//	if (IsReal()) { RAddR(res,x,*this); }
//	else { RAddC(res,x,*this); }
//
//	return res;
//}
//
//cMatrix cMatrix::operator+(const complex<double>& x) const
//{
//	cMatrix res;
//	if (IsReal()) { CAddR(res,x,*this); }
//	else { CAddC(res,x,*this); }
//
//	return res;
//}
//
//cMatrix operator+(const double d, const cMatrix& A)
//{
//	cMatrix res;
//	if (A.IsReal()) { RAddR(res,d,A); }
//	else { RAddC(res,d,A); }
//
//	return res;
//}
//
//cMatrix operator+(const complex<double>& c, const cMatrix& A)
//{
//	cMatrix res;
//	if (A.IsReal()) { CAddR(res,c,A); }
//	else { CAddC(res,c,A); }
//	
//	return res;
//}

//
// "-"
//

//cMatrix Sub(const double x, const cMatrix& R);		// real matrix sub
//cMatrix Sub(const cMatrix& R1, const cMatrix& R2);	// real and real matrix sub
//cMatrix SubC(const double x, const cMatrix& C);		// complex matrix sub
//cMatrix CSub(const cMatrix& C, const cMatrix& R);	// complex and real matrix sub
//cMatrix CSub(const complex<double>& x, const cMatrix& R);	// complex and real matrix sub
//cMatrix SubC(const cMatrix& R, const cMatrix& C);	// real and complex matrix sub
//cMatrix CSubC(const cMatrix& C1, const cMatrix& C2);	// complex and complex matrix sub
//cMatrix CSubC(const complex<double>& x, const cMatrix& C);	// complex and complex matrix sub
//cMatrix CSubC(const cMatrix& C, const complex<double>& x);	// complex and complex matrix sub

//cMatrix cMatrix::operator-(const cMatrix& A) const
//{
//	cMatrix res;
//	if (IsReal()) {
//		if (A.IsReal()) RSubR(res,*this,A);
//		else RSubC(res,*this,A);
//	}
//	else {
//		if (A.IsReal()) CSubR(res,*this,A);
//		else CSubC(res,*this,A);
//	}
//
//	return res;
//}
//
//cMatrix cMatrix::operator-(const double x) const
//{
//	cMatrix res = *this;
//	int n = GetSize();
//	for (int i=0; i<n; ++i) { res.mr[i] -= x; }
//	return res;
//}
//
//cMatrix cMatrix::operator-(const complex<double>& x) const
//{
//	cMatrix res;
//	if (IsReal()) { CSubR(res,x,*this); }
//	else { CSubC(res,x,*this); }
//
//	return res;
//}
//
//cMatrix operator-(const double d, const cMatrix& R)
//{
//	cMatrix res;
//	if (R.IsReal()) RSubR(res,d,R);
//	else RSubC(res,d,R);
//	return res;
//}
//
//cMatrix operator-(const complex<double>& c, const cMatrix& A)
//{
//	cMatrix res;
//	if (A.IsReal()) { CSubR(res,c,A); }
//	else { CSubC(res,c,A); }
//	return res;
//}

//
// "*"
//

//cMatrix Mult(const double x, const cMatrix& R);
//cMatrix Mult(const cMatrix& R1, const cMatrix& R2);
//cMatrix MultC(const cMatrix& R, const cMatrix& C);
//cMatrix MultC(const double x, const cMatrix& C);
//cMatrix CMult(const complex<double>& c, const cMatrix& R);
//cMatrix CMult(const cMatrix& R, const cMatrix& C);
//cMatrix CMultC(const complex<double>& c, const cMatrix& C);
//cMatrix CMultC(const cMatrix& C1, const cMatrix& C2);

//cMatrix cMatrix::operator*(const cMatrix& A) const
//{
//	cMatrix res;
//	if (IsReal()) {
//		if (A.IsReal()) RMultR(res,*this,A);
//		else RMultC(res,*this,A);
//	}
//	else {
//		if (A.IsReal()) CMultR(res,*this,A);
//		else CMultC(res,*this,A);
//	}
//	return res;
//}
//
//cMatrix cMatrix::operator*(const double d) const
//{
//	cMatrix res;
//	if (IsReal()) RMultR(res,d,*this);
//	else RMultC(res,d,*this);
//	return res;
//}
//
//cMatrix cMatrix::operator*(const complex<double>& x) const
//{
//	cMatrix res;
//	if (IsReal()) { CMultR(res,x,*this); }
//	else { CMultC(res,x,*this); }
//	return res;
//}
//
//cMatrix operator*(const double d, const cMatrix& R)
//{
//	cMatrix res;
//	if (R.IsReal()) RMultR(res,d,R);
//	else RMultC(res,d,R);
//	return res;
//}
//
//cMatrix operator*(const complex<double>& c, const cMatrix& A)
//{
//	cMatrix res;
//	if (A.IsReal()) { CMultR(res,c,A); }
//	else { CMultC(res,c,A); }
//	return res;
//}

//
// "/"
//
//cMatrix cMatrix::operator/(const double d) const
//{
//	cMatrix Result;
//	if (IsReal()) RDotDivR(Result,*this,d);
//	else CDotDivR(Result,*this,d);
//	return Result;
//}
//
//cMatrix cMatrix::operator/(const complex<double>& x) const
//{
//	cMatrix Result;
//	if (IsReal()) { RDotDivC(Result,*this,x); }
//	else { CDotDivC(Result,*this,x); }
//	return Result;
//}
//
//cMatrix cMatrix::operator/(const cMatrix& A) const
//{
//	cMatrix Result;
//	DotDiv(Result, *this, A);
//	return Result;
//}
//
//cMatrix operator/(const double d, const cMatrix& R)
//{
//	cMatrix Result;
//	if (R.IsReal()) RDotDivR(Result, d, R);
//	else RDotDivC(Result, d, R);
//	return Result;
//}
//
//cMatrix operator/(const complex<double>& c, const cMatrix& A)
//{
//	cMatrix Result;
//	if (A.IsReal()) CDotDivR(Result, c, A);
//	else CDotDivC(Result, c, A);
//	return Result;
//}


//
// "=="
//
//cMatrix operator==(const cMatrix& R, const cMatrix& C)
//{
//	cMatrix Result;
//	if (Result.Create(R.GetRows(),R.GetColumns())) return Result;
//	int i;
//	complex<double> c1, c2;
//
//	for (i=0; i<R.GetSize(); i++) {
//		R.GetC(c1,i);
//		C.GetC(c2,i);
//		Result.SetR(i, c1==c2 );
//	}
//	return Result;
//}

cMatrix cMatrix::operator==(const cMatrix& R) const
{
	cMatrix Result;
	if (Result.Create(GetRows(),GetColumns())) return Result;
	int i;
	complex<double> c1, c2;

	for (i=0; i<GetSize(); i++) {
		GetC(c1,i);
		R.GetC(c2,i);
		Result.SetR(i, c1==c2 );
	}
	return Result;
}

cMatrix cMatrix::operator==(const complex<double>& x) const
{
	cMatrix Result;
	if (Result.Create(GetRows(),GetColumns())) return Result;
	int i;
	complex<double> c1;

	for (i=0; i<GetSize(); i++) {
		GetC(c1,i);
		Result.SetR(i, c1==x );
	}
	return Result;
}

cMatrix cMatrix::operator==(const double d) const
{
	cMatrix Result;
	if (Result.Create(GetRows(),GetColumns())) return Result;
	int i;
	complex<double> c1;

	for (i=0; i<GetSize(); i++) {
		GetC(c1,i);
		Result.SetR(i, c1==d );
	}
	return Result;
}

cMatrix operator==(const double d, const cMatrix R)
{
	cMatrix Result;
	if (Result.Create(R.GetRows(),R.GetColumns())) return Result;
	int i;
	complex<double> c1;

	for (i=0; i<R.GetSize(); i++) {
		R.GetC(c1,i);
		Result.SetR(i, c1==d );
	}
	return Result;
}

cMatrix operator==(const complex<double>& x, const cMatrix R)
{
	cMatrix Result;
	if (Result.Create(R.GetRows(),R.GetColumns())) return Result;
	int i;
	complex<double> c1;

	for (i=0; i<R.GetSize(); i++) {
		R.GetC(c1,i);
		Result.SetR(i, c1==x );
	}
	return Result;
}


//
// "!="
//

cMatrix cMatrix::operator!=(const cMatrix& R) const
{
	cMatrix Result;
	if (Result.Create(GetRows(),GetColumns())) return Result;
	int i;
	complex<double> c1, c2;

	for (i=0; i<GetSize(); i++) {
		GetC(c1,i);
		R.GetC(c2,i);
		Result.SetR(i, c1!=c2 );
	}
	return Result;
}

cMatrix cMatrix::operator!=(const complex<double>& x) const
{
	cMatrix Result;
	if (Result.Create(GetRows(),GetColumns())) return Result;
	int i;
	complex<double> c1;

	for (i=0; i<GetSize(); i++) {
		GetC(c1,i);
		Result.SetR(i, c1!=x );
	}
	return Result;
}

cMatrix cMatrix::operator!=(const double d) const
{
	cMatrix Result;
	if (Result.Create(GetRows(),GetColumns())) return Result;
	int i;
	complex<double> c1;

	for (i=0; i<GetSize(); i++) {
		GetC(c1,i);
		Result.SetR(i, c1!=d );
	}
	return Result;
}

cMatrix operator!=(const double d, const cMatrix& R)
{
	cMatrix Result;
	if (Result.Create(R.GetRows(),R.GetColumns())) return Result;
	int i;
	complex<double> c1;

	for (i=0; i<R.GetSize(); i++) {
		R.GetC(c1,i);
		Result.SetR(i, c1!=d );
	}
	return Result;
}

cMatrix operator!=(const complex<double>& x, const cMatrix& R)
{
	cMatrix Result;
	if (Result.Create(R.GetRows(),R.GetColumns())) return Result;
	int i;
	complex<double> c1;

	for (i=0; i<R.GetSize(); i++) {
		R.GetC(c1,i);
		Result.SetR(i, c1!=x );
	}
	return Result;
}


//
// "<"
//

cMatrix cMatrix::operator<(const cMatrix& R) const
{
	cMatrix Result;
	if (Result.Create(GetRows(),GetColumns())) return Result;
	int i;
	double d1, d2;

	for (i=0; i<GetSize(); ++i) {
		GetR(d1,i);
		R.GetR(d2,i);
		Result.SetR(i, d1<d2);
	}

	return Result;
}

cMatrix cMatrix::operator<(const complex<double>& x) const
{
	cMatrix Result;
	if (Result.Create(GetRows(),GetColumns())) return Result;
	int i;
	double d1, d2 = x.real();

	for (i=0; i<GetSize(); ++i) {
		GetR(d1,i);
		Result.SetR(i, d1<d2);
	}

	return Result;
}

cMatrix cMatrix::operator<(const double d) const
{
	cMatrix Result;
	if (Result.Create(GetRows(),GetColumns())) return Result;
	int i;
	double d1;

	for (i=0; i<GetSize(); ++i) {
		GetR(d1,i);
		Result.SetR(i, d1<d);
	}

	return Result;
}

cMatrix operator<(const double d, const cMatrix& R)
{
	cMatrix Result;
	if (Result.Create(R.GetRows(),R.GetColumns())) return Result;
	int i;
	double d1;

	for (i=0; i<R.GetSize(); ++i) {
		R.GetR(d1,i);
		Result.SetR(i, d<d1);
	}

	return Result;
}

cMatrix operator<(const complex<double>& x, const cMatrix& R)
{
	cMatrix Result;
	if (Result.Create(R.GetRows(),R.GetColumns())) return Result;
	int i;
	double d1, d2;
	d1 = x.real();

	for (i=0; i<R.GetSize(); ++i) {
		R.GetR(d2,i);
		Result.SetR(i, d1<d2);
	}

	return Result;
}


//
// ">"
//

cMatrix cMatrix::operator>(const cMatrix& R) const
{
	cMatrix Result;
	if (Result.Create(GetRows(),GetColumns())) return Result;
	int i;
	double d1, d2;

	for (i=0; i<GetSize(); ++i) {
		GetR(d1,i);
		R.GetR(d2,i);
		Result.SetR(i, d1>d2);
	}

	return Result;
}

cMatrix cMatrix::operator>(const complex<double>& x) const
{
	cMatrix Result;
	if (Result.Create(GetRows(),GetColumns())) return Result;
	int i;
	double d1, d2 = x.real();

	for (i=0; i<GetSize(); ++i) {
		GetR(d1,i);
		Result.SetR(i, d1>d2);
	}

	return Result;
}

cMatrix cMatrix::operator>(const double d) const
{
	cMatrix Result;
	if (Result.Create(GetRows(),GetColumns())) return Result;
	int i;
	double d1;

	for (i=0; i<GetSize(); ++i) {
		GetR(d1,i);
		Result.SetR(i, d1>d);
	}

	return Result;
}

cMatrix operator>(const double d, const cMatrix& R)
{
	cMatrix Result;
	if (Result.Create(R.GetRows(),R.GetColumns())) return Result;
	int i;
	double d1;

	for (i=0; i<R.GetSize(); ++i) {
		R.GetR(d1,i);
		Result.SetR(i, d>d1);
	}

	return Result;
}

cMatrix operator>(const complex<double>& x, const cMatrix& R)
{
	cMatrix Result;
	if (Result.Create(R.GetRows(),R.GetColumns())) return Result;
	int i;
	double d1, d2;
	d1 = x.real();

	for (i=0; i<R.GetSize(); ++i) {
		R.GetR(d2,i);
		Result.SetR(i, d1>d2);
	}

	return Result;
}


//
// "<="
//

cMatrix cMatrix::operator<=(const cMatrix& R) const
{
	cMatrix Result;
	if (Result.Create(GetRows(),GetColumns())) return Result;
	int i;
	double d1, d2;

	for (i=0; i<GetSize(); ++i) {
		GetR(d1,i);
		R.GetR(d2,i);
		Result.SetR(i, d1<=d2);
	}

	return Result;
}

cMatrix cMatrix::operator<=(const complex<double>& x) const
{
	cMatrix Result;
	if (Result.Create(GetRows(),GetColumns())) return Result;
	int i;
	double d1, d2 = x.real();

	for (i=0; i<GetSize(); ++i) {
		GetR(d1,i);
		Result.SetR(i, d1<=d2);
	}

	return Result;
}

cMatrix cMatrix::operator<=(const double d) const
{
	cMatrix Result;
	if (Result.Create(GetRows(),GetColumns())) return Result;
	int i;
	double d1;

	for (i=0; i<GetSize(); ++i) {
		GetR(d1,i);
		Result.SetR(i, d1<=d);
	}

	return Result;
}

cMatrix operator<=(const double d, const cMatrix& R)
{
	cMatrix Result;
	if (Result.Create(R.GetRows(),R.GetColumns())) return Result;
	int i;
	double d1;

	for (i=0; i<R.GetSize(); ++i) {
		R.GetR(d1,i);
		Result.SetR(i, d<=d1);
	}

	return Result;
}

cMatrix operator<=(const complex<double>& x, const cMatrix& R)
{
	cMatrix Result;
	if (Result.Create(R.GetRows(),R.GetColumns())) return Result;
	int i;
	double d1, d2;
	d1 = x.real();

	for (i=0; i<R.GetSize(); ++i) {
		R.GetR(d2,i);
		Result.SetR(i, d1<=d2);
	}

	return Result;
}


//
// ">="
//

cMatrix cMatrix::operator>=(const cMatrix& R) const
{
	cMatrix Result;
	if (Result.Create(GetRows(),GetColumns())) return Result;
	int i;
	double d1, d2;

	for (i=0; i<GetSize(); ++i) {
		GetR(d1,i);
		R.GetR(d2,i);
		Result.SetR(i, d1>=d2);
	}

	return Result;
}

cMatrix cMatrix::operator>=(const complex<double>& x) const
{
	cMatrix Result;
	if (Result.Create(GetRows(),GetColumns())) return Result;
	int i;
	double d1, d2 = x.real();

	for (i=0; i<GetSize(); ++i) {
		GetR(d1,i);
		Result.SetR(i, d1>=d2);
	}

	return Result;
}

cMatrix cMatrix::operator>=(const double d) const
{
	cMatrix Result;
	if (Result.Create(GetRows(),GetColumns())) return Result;
	int i;
	double d1;

	for (i=0; i<GetSize(); ++i) {
		GetR(d1,i);
		Result.SetR(i, d1>=d);
	}

	return Result;
}

cMatrix operator>=(const double d, const cMatrix& R)
{
	cMatrix Result;
	if (Result.Create(R.GetRows(),R.GetColumns())) return Result;
	int i;
	double d1;

	for (i=0; i<R.GetSize(); ++i) {
		R.GetR(d1,i);
		Result.SetR(i, d>=d1);
	}

	return Result;
}

cMatrix operator>=(const complex<double>& x, const cMatrix& R)
{
	cMatrix Result;
	if (Result.Create(R.GetRows(),R.GetColumns())) return Result;
	int i;
	double d1, d2;
	d1 = x.real();

	for (i=0; i<R.GetSize(); ++i) {
		R.GetR(d2,i);
		Result.SetR(i, d1>=d2);
	}

	return Result;
}


//
// "&&" ("&")
//

cMatrix cMatrix::operator&&(const cMatrix& R) const
{
	cMatrix Result;
	if (Result.Create(GetRows(),GetColumns())) return Result;
	int i;
	double d1, d2;

	for (i=0; i<GetSize(); ++i) {
		GetR(d1,i);
		R.GetR(d2,i);
		Result.SetR(i, d1&&d2);
	}

	return Result;
}

cMatrix cMatrix::operator&&(const complex<double>& x) const
{
	cMatrix Result;
	if (Result.Create(GetRows(),GetColumns())) return Result;
	int i;
	double d1, d2 = x.real();

	for (i=0; i<GetSize(); ++i) {
		GetR(d1,i);
		Result.SetR(i, d1&&d2);
	}

	return Result;
}

cMatrix cMatrix::operator&&(const double d) const
{
	cMatrix Result;
	if (Result.Create(GetRows(),GetColumns())) return Result;
	int i;
	double d1;

	for (i=0; i<GetSize(); ++i) {
		GetR(d1,i);
		Result.SetR(i, d1&&d);
	}

	return Result;
}

cMatrix operator&&(const double d, const cMatrix& R)
{
	cMatrix Result;
	if (Result.Create(R.GetRows(),R.GetColumns())) return Result;
	int i;
	double d1;

	for (i=0; i<R.GetSize(); ++i) {
		R.GetR(d1,i);
		Result.SetR(i, d&&d1);
	}

	return Result;
}

cMatrix operator&&(const complex<double>& x, const cMatrix& R)
{
	cMatrix Result;
	if (Result.Create(R.GetRows(),R.GetColumns())) return Result;
	int i;
	double d1, d2;
	d1 = x.real();

	for (i=0; i<R.GetSize(); ++i) {
		R.GetR(d2,i);
		Result.SetR(i, d1&&d2);
	}

	return Result;
}


//
// "||" ("|")
//

cMatrix cMatrix::operator||(const cMatrix& R) const
{
	cMatrix Result;
	if (Result.Create(GetRows(),GetColumns())) return Result;
	int i;
	double d1, d2;

	for (i=0; i<GetSize(); ++i) {
		GetR(d1,i);
		R.GetR(d2,i);
		Result.SetR(i, d1||d2);
	}

	return Result;
}

cMatrix cMatrix::operator||(const complex<double>& x) const
{
	cMatrix Result;
	if (Result.Create(GetRows(),GetColumns())) return Result;
	int i;
	double d1, d2 = x.real();

	for (i=0; i<GetSize(); ++i) {
		GetR(d1,i);
		Result.SetR(i, d1||d2);
	}

	return Result;
}

cMatrix cMatrix::operator||(const double d) const
{
	cMatrix Result;
	if (Result.Create(GetRows(),GetColumns())) return Result;
	int i;
	double d1;

	for (i=0; i<GetSize(); ++i) {
		GetR(d1,i);
		Result.SetR(i, d1||d);
	}

	return Result;
}

cMatrix operator||(const double d, const cMatrix& R)
{
	cMatrix Result;
	if (Result.Create(R.GetRows(),R.GetColumns())) return Result;
	int i;
	double d1;

	for (i=0; i<R.GetSize(); ++i) {
		R.GetR(d1,i);
		Result.SetR(i, d||d1);
	}

	return Result;
}

cMatrix operator||(const complex<double>& x, const cMatrix& R)
{
	cMatrix Result;
	if (Result.Create(R.GetRows(),R.GetColumns())) return Result;
	int i;
	double d1, d2;
	d1 = x.real();

	for (i=0; i<R.GetSize(); ++i) {
		R.GetR(d2,i);
		Result.SetR(i, d1||d2);
	}

	return Result;
}


