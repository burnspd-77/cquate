
#include "cube.h"
#include<limits.h>

bool cCube::Create(const int r, const int c, const int m)
{
	if ( r<0 || c<0 || m<0 ) {
		addError("illegal dimension (negative)");
		return true;
	}

	if ( c >= (int) (double(INT_MAX) / double(r)) ) {
		addError("cube size exceeds limits");
		return true;
	}

	if ( m >= (int) (double(INT_MAX) / double(r*c)) ) {
		addError("cube size exceeds limits");
		return true;
	}

	int sz = r*c*m;
	nr = r;
	nc = c;
	nm = m;
	mr.clear(); mi.clear();
	mr.insert( mr.begin(), sz, 0. );
	return false;
}

bool cCube::CreateComplex(const int r, const int c, const int m)
{
	if ( r<0 || c<0 || m < 0 ) {
		addError("illegal dimension (negative)");
		return true;
	}

	if ( c >= (int) (double(INT_MAX) / double(r)) ) {
		addError("cube size exceeds limits");
		return true;
	}

	if ( m >= (int) (double(INT_MAX) / double(r*c)) ) {
		addError("cube size exceeds limits");
		return true;
	}

	int sz = r*c*m;
	nr = r;
	nc = c;
	nm = m;
	mr.clear(); mi.clear();
	mr.insert( mr.begin(), sz, 0. );
	mi.insert( mi.begin(), sz, 0. );
	return false;
}

bool cCube::GetMatrix(cMatrix& m, const int k) const
{
	if (k<0 || k>= GetDepth()) return true;

	int i, j;
	double lf, lfi;

	if (IsReal()) {
		m.Create(nr, nc);
		for (i=0; i<nr; ++i) {
			for (j=0; j<nc; ++j) {
				GetR(lf,i,j,k);
				m.SetR(i,j, lf);
			}
		}
	}
	else {
		m.CreateComplex(nr, nc);
		for (i=0; i<nr; ++i) {
			for (j=0; j<nc; ++j) {
				GetR(lf,i,j,k);
				GetI(lfi,i,j,k);
				m.SetR(i,j, lf);
				m.SetI(i,j, lfi);
			}
		}
	}
	return false;
}

void cCube::GetR(double& lf, const int i, const int j, const int k) const
{
	int ix = getIx(i,j,k);
	lf = mr[ix];
}

void cCube::GetI(double& lf, const int i, const int j, const int k) const
{
	int ix = getIx(i,j,k);
	lf = mi[ix];
}

void cCube::Get(complex<double>& c, const int i, const int j, const int k) const
{
	int ix = getIx(i,j,k);
	if (IsReal()) c = complex<double>(mr[ix],0.);
	else c = complex<double>(mr[ix],mi[ix]);
}

bool cCube::GetR(vector<double>& vd, const int abeg, const int aend, const int bbeg, const int bend, const int c, const int dim)
{
	int i, j;
	int na = aend - abeg + 1;
	int nb = bend - bbeg + 1;
	//m.Create(na, nb);
	int sz = na*nb;
	vd.reserve(sz);
	double lf;
	switch (dim) {
	case 0:			// get along plane of constant depth
		assert(abeg>=0 && aend <nr);
		assert(bbeg>=0 && bend <nc);
		for (j=bbeg; j<bend; ++j) {
			for (i=abeg; i<=aend; ++i) {
				GetR(lf,i,j,c);
				vd.push_back(lf);	//m.SetR(i-abeg,j-bbeg,lf);
			}
		}
		break;

	case 1:			// get along plane of constant row
		assert(abeg>=0 && aend <nc);
		assert(bbeg>=0 && bend <nm);
		for (j=bbeg; j<bend; ++j) {
			for (i=abeg; i<=aend; ++i) {
				GetR(lf,c,i,j);
				vd.push_back(lf);	//m.SetR(i-abeg,j-bbeg,lf);
			}
		}
		break;

	case 2:			// get along plane of constant column
		assert(abeg>=0 && aend <nr);
		assert(bbeg>=0 && bend <nm);
		for (j=bbeg; j<bend; ++j) {
			for (i=abeg; i<=aend; ++i) {
				GetR(lf,i,c,j);
				vd.push_back(lf);	//m.SetR(i-abeg,j-bbeg,lf);
			}
		}
		break;

	default:
		return true;
	}
	return false;
}

bool cCube::GetI(vector<double>& vd, const int abeg, const int aend, const int bbeg, const int bend, const int c, const int dim)
{
	int i, j;
	int na = aend - abeg + 1;
	int nb = bend - bbeg + 1;
	//m.Create(na, nb);
	int sz = na*nb;
	vd.reserve(sz);
	double lf;
	switch (dim) {
	case 0:			// get along plane of constant depth
		assert(abeg>=0 && aend <nr);
		assert(bbeg>=0 && bend <nc);
		for (j=bbeg; j<bend; ++j) {
			for (i=abeg; i<=aend; ++i) {
				GetI(lf,i,j,c);
				vd.push_back(lf);	//m.SetR(i-abeg,j-bbeg,lf);
			}
		}
		break;

	case 1:			// get along plane of constant row
		assert(abeg>=0 && aend <nc);
		assert(bbeg>=0 && bend <nm);
		for (j=bbeg; j<bend; ++j) {
			for (i=abeg; i<=aend; ++i) {
				GetI(lf,c,i,j);
				vd.push_back(lf);	//m.SetR(i-abeg,j-bbeg,lf);
			}
		}
		break;

	case 2:			// get along plane of constant column
		assert(abeg>=0 && aend <nr);
		assert(bbeg>=0 && bend <nm);
		for (j=bbeg; j<bend; ++j) {
			for (i=abeg; i<=aend; ++i) {
				GetI(lf,i,c,j);
				vd.push_back(lf);	//m.SetR(i-abeg,j-bbeg,lf);
			}
		}
		break;

	default:
		return true;
	}
	return false;
}

bool cCube::Get(cMatrix& m, const int abeg, const int aend, const int bbeg, const int bend, const int c, const int dim)
{
	int na = aend - abeg + 1;
	int nb = bend - bbeg + 1;

	if (IsReal()) {
		m.Create(na,nb);
		return GetR(m.mr, abeg, aend, bbeg, bend, c, dim);
	}
	
	m.CreateComplex(na,nb);
	return GetI(m.mi, abeg, aend, bbeg, bend, c, dim);
}

bool cCube::SetTo(double d)		// set to complex scalar
{
	Create(1,1,1);
	mr[0] = d;
	return false;
}

bool cCube::SetTo(complex<double> c)		// set to complex scalar
{
	CreateComplex(1,1,1);
	mr[0] = c.real();
	mi[0] = c.imag();
	return false;
}

bool cCube::SetTo(cMatrix& m)				// copy from another matrix
{
	if (m.IsReal()) {
		Create(m.GetRows(),m.GetColumns(),1);
		mr = m.mr;
	}
	else {
		CreateComplex(m.GetRows(),m.GetColumns(),1);
		mr = m.mr;
		mi = m.mi;
	}

	return false;
}

bool cCube::SetTo(cCube& c)				// copy from another matrix
{
	if (c.IsReal()) {
		Create(c.GetRows(),c.GetColumns(),c.GetDepth());
		mr = c.mr;
	}
	else {
		CreateComplex(c.GetRows(),c.GetColumns(),c.GetDepth());
		mr = c.mr;
		mi = c.mi;
	}

	return false;
}


bool cCube::SetR(const int i, const int j, const int k, const double x)
{
	int ix = getIx(i,j,k);
	if (ix>=0 && i<GetSize()) {
		mr[ix] = x;
		return false;
	}
	return true;
}

bool cCube::SetI(const int i, const int j, const int k, const double x)
{
	if (IsReal()) return true;

	int ix = getIx(i,j,k);
	if (ix>=0 && i<GetSize()) {
		mi[ix] = x;
		return false;
	}
	return true;
}

bool cCube::Set(const int i, const int j, const int k, complex<double>& c)
{
	int ix = getIx(i,j,k);
	if (ix>=0 && i<GetSize()) {
		mr[ix] = c.real();
		mi[ix] = c.imag();
		return false;
	}
	return true;
}

// File I/O
//
bool cCube::SaveAscii(FILE *pf)
{
	int r, c, d;
	double lfr, lfi;

	if (IsReal()) {			// REAL values
		for (d=0; d<nm; ++d) {
			fprintf(pf, "# depth=%d\n", d);
			for (r=0; r<nr; ++r) {
				// first column
				GetR(lfr, r, 0, d);
				fprintf(pf, "%f", lfr);
				// additional columns
				for (c=1; c<nc; ++c) {
					GetR(lfr, r, c, d);
					fprintf(pf, ", %f", lfr);
				}
				fprintf(pf, "\n");		// end of line
			}
		}
	}
	else {					// COMPLEX values
		for (d=0; d<nm; ++d) {
			fprintf(pf, "# depth=%d\n", d);
			for (r=0; r<nr; ++r) {
				// first column
				GetR(lfr, r, 0, d);
				GetI(lfi, r, 0, d);
				fprintf(pf, "%f+i%f", lfr, lfi);
				// additional columns
				for (c=1; c<nc; ++c) {
					GetR(lfr, r, c, d);
					fprintf(pf, ", %f+i%f", lfr, lfi);
				}
				fprintf(pf, "\n");		// end of line
			}
		}
	}
	return false;
}

bool cCube::IsScalar(void) const
{
	return GetSize()==1;
}

bool cCube::IsVector(int *pi) const		// if true, int arg is 0 for col vector, 1 for row vector, 2 for depth vector
{
	if (nm==1) {
		if (nc==1) { *pi = 0; return true; }
		if (nr==1) { *pi = 1; return true; }
	}
	else if (nr==1) {
		if (nc==1) { *pi = 2; return true; }
	}
	return false;
}

bool cCube::IsVector(void) const
{
	int dc;
	return IsVector(&dc);
}

bool cCube::IsFinite() const
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



bool cCube::AddMatrix(cMatrix& mat)
{
	if (IsReal() && !mat.IsReal())
		forceComplex();

	if (IsEmpty()) { nr = mat.GetRows(); nc = mat.GetColumns(); nm = 0;	}
	else if (nr != mat.GetRows() || nc != mat.GetColumns()) return true;		// size mismatch

	int sz = mat.GetSize();

	mr.insert( mr.end(), mat.mr.begin(), mat.mr.end() );

	if (!IsReal() && mat.IsReal())
		mi.insert( mi.end(), sz, 0. );
	else if (!IsReal() && !mat.IsReal() || nm==0 && !mat.IsReal())		// mat is complex
		mi.insert( mi.end(), mat.mi.begin(), mat.mi.end() );

	++nm;

	return false;
}

