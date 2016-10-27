#include "matrix.h"

//
// access operator (one arg), rvalue
//
bool matAccess(cMatrix& Result, double dvar, double darg)
{
	int iarg = (int) darg;
	if (iarg != 0) { addError("index outside array bounds"); return true; }

	if (Result.Create(1,1)) return true;
	//double *pr = Result.GetR();

	Result.mr[0] = dvar;

	return false;
}

bool matAccess(cMatrix& Result, double dvar, cMatrix& arg)
{
	if (Result.Create(arg.GetRows(),arg.GetColumns())) return true;

	double *pr = Result.GetR();
	const double *par = arg.GetR();		// ignore imag part
	int n = arg.GetSize();

	// the only valid arg is 0, anything else results in error!
	for (int i=0; i<n; ++i)
	{
		if (int(arg.mr[i])==0)	Result.mr[i] = dvar;
		else { Result.Delete(); addError("index outside array bounds"); return true; }
	}
	return false;
}

/*
bool matAccess(cMatrix& Result, cMatrix& var, double darg)
{
	int n = var.GetSize();
	int iarg = (int) darg;
	if (iarg < 0 || iarg >= n) { addError("index outside array bounds"); return true; }

	// get iarg^th entry of var
	if (var.IsReal()) {
		if (Result.Create(1,1)) return true;
		Result.mr[0] = var.mr[iarg];
	}
	else {	// complex
		if (Result.CreateComplex(1,1)) return true;
		Result.mr[0] = var.mr[iarg];
		Result.mi[0] = var.mi[iarg];
	}

	return false;
}

bool matAccess(cMatrix& Result, cMatrix& var, cMatrix& arg)
{
	if (!arg.IsVector()) { addError("non-vector argument"); return true; }

	int n = arg.GetSize(), nvar = var.GetSize();
	int iarg, i;

	if (var.IsReal()) {
		if (Result.Create(arg.GetRows(),arg.GetColumns())) return true;
		for (i=0; i<n; ++i) {
			iarg = (int) arg.mr[i];
			if (iarg < 0 || iarg >= nvar) { Result.Delete(); addError("index outside array bounds"); return true; }
			Result.mr[i] = var.mr[iarg];
		}
	}
	else {	// complex
		if (Result.CreateComplex(arg.GetRows(),arg.GetColumns())) return true;
		for (i=0; i<n; ++i) {
			iarg = (int) arg.mr[i];
			if (iarg < 0 || iarg >= nvar) { Result.Delete(); addError("index outside array bounds"); return true; }
			Result.mr[i] = var.mr[iarg];
			Result.mi[i] = var.mi[iarg];
		}
	}

	return false;
}
*/

bool arg2ix( cMatrix& Result, cMatrix& arg1, cMatrix& arg2, int nRows, int nCols )
{	// convert values in arg1,arg2 to indices into a matrix of size (nRows,nCols)

	if (arg1.IsScalar()) { return arg2ix( Result, arg1.Get0R(), arg2, nRows, nCols ); }
	if (arg2.IsScalar()) { return arg2ix( Result, arg1, arg2.Get0R(), nRows, nCols ); }

	if (arg1.IsVector() && !arg2.IsVector()) { addError("illegal argument"); return true; }
	if (!arg1.IsVector() && arg2.IsVector()) { addError("illegal argument"); return true; }
	
	int nr1, nr2;
	int i, j;

	if (arg1.IsVector()) {	// arg2 must also be vector
		nr1 = arg1.GetSize();
		nr2 = arg2.GetSize();
		if (Result.Create(nr1,nr2)) return true;
		//pr = Result.GetR();
		for (i=0; i<nr1; ++i) {
			for (j=0; j<nr2; ++j) {
				Result.mr[i+nr1*j] = floor( arg1.mr[i]+nRows*arg2.mr[j] );
			}
		}
	}
	else {
		// error, because if arg1 is non-vector matrix, arg2 should have been scalar (handled above)
		addError("illegal argument"); return true;
	}

	return false;
}

bool arg2ix( cMatrix& Result, double d1, cMatrix& arg2, int nRows, int nCols )
{	// result will be same size as arg2, which must be a vector

	if (!arg2.IsVector()) { addError("illegal argument"); return true; }

	int i, j, ix;
	int nr2 = arg2.GetRows();
	int nc2 = arg2.GetColumns();

	if (Result.Create(nr2,nc2)) return true;

	double *pr = Result.GetR();
	const double *par2 = arg2.GetR();

	for (i=0; i<nr2; ++i) {
		for (j=0; j<nc2; ++j) {
			ix = i+nr2*j;
			Result.mr[ix] = floor( d1+nRows*arg2.mr[ix] );
		}
	}

	return false;
}

bool arg2ix( cMatrix& Result, cMatrix& arg1, double d2, int nRows, int nCols )
{	// result will be same size as arg1, which must be a vector

	if (!arg1.IsVector()) { addError("illegal argument"); return true; } 

	int i, j, ix;
	int nr1 = arg1.GetRows();
	int nc1 = arg1.GetColumns();

	if (Result.Create(nr1,nc1)) return true;

	//double *pr = Result.GetR();
	//const double *par1 = arg1.GetR();

	for (i=0; i<nr1; ++i) {
		for (j=0; j<nc1; ++j) {
			ix = i+nr1*j;
			Result.mr[ix] = floor( arg1.mr[ix]+nRows*d2 );
		}
	}

	return false;
}


//
// access operator (two args), rvalue
//
bool matAccess(cMatrix& Result, double dvar, double darg1, double darg2)
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

bool matAccess(cMatrix& Result, double dvar, double darg1, cMatrix& arg2)
{
	if (!arg2.IsVector()) { addError("illegal argument"); return true; }

	int iarg1 = (int) darg1;
	if (iarg1 != 0) { addError("index outside array bounds"); return true; }
	int iarg2;

	int narg = arg2.GetSize();
	if (Result.Create(1,narg)) return true;

	//double *pr = Result.GetR();
	//double *par = arg2.GetR();		// ignore imag part

	for (int i=0; i<narg; ++i) {
		iarg2 = (int) arg2.mr[i];
		if (iarg2 != 0) { Result.Delete(); addError("index outside array bounds"); return true; }
		Result.mr[i] = dvar;
	}

	return false;
}

bool matAccess(cMatrix& Result, double dvar, cMatrix& arg1, double darg2)
{
	if (!arg1.IsVector()) { addError("illegal argument"); return true; }

	int iarg2 = (int) darg2;
	if (iarg2 != 0) { addError("index outside array bounds"); return true; }
	int iarg1;

	int narg = arg1.GetSize();
	if (Result.Create(narg,1)) return true;

	//double *pr = Result.GetR();
	//double *par = arg1.GetR();		// ignore imag part

	for (int i=0; i<narg; ++i) {
		iarg1 = (int) arg1.mr[i];
		if (iarg1 != 0) { Result.Delete(); addError("index outside array bounds"); return true; }
		Result.mr[i] = dvar;
	}

	return false;
}

bool matAccess(cMatrix& Result, double dvar, cMatrix& arg1, cMatrix& arg2)
{
	if (!arg1.IsVector() || !arg2.IsVector()) { addError("illegal argument"); return true; }

	int iarg1, iarg2, i;

	int narg1 = arg1.GetSize();
	int narg2 = arg2.GetSize();

	if (Result.Create(narg1,narg2)) return true;

	for (i=0; i<narg1; ++i) {
		iarg1 = (int) arg1.mr[i];
		if (iarg1 != 0) { Result.Delete(); addError("index outside array bounds"); return true; }
	}

	for (i=0; i<narg2; ++i) {
		iarg2 = (int) arg2.mr[i];
		if (iarg2 != 0) { Result.Delete(); addError("index outside array bounds"); return true; }
	}

	Result.Reset();

	return false;
}

/*
bool matAccess(cMatrix& Result, cMatrix& var, double darg1, cMatrix& arg2)
{
	if (!arg2.IsVector()) { addError("illegal argument"); return true; }

	int iarg1 = (int) darg1;
	int nvar = var.GetSize();
	int nv_rows = var.GetRows(), nv_cols = var.GetColumns();

	if (iarg1 < 0 || iarg1 >= nv_rows) { addError("index outside array bounds"); return true; }
	int iarg2;

	int narg = arg2.GetSize();
	double dval;
	complex<double> cval;

	if (var.IsReal()) {
		if (Result.Create(1,narg)) return true;

		for (int i=0; i<narg; ++i) {
			iarg2 = (int) arg2.mr[i];
			if (iarg2 < 0 || iarg2 >= nv_cols) { Result.Delete(); addError("index outside array bounds"); return true; }
			var.GetR(dval,iarg1,iarg2);
			Result.mr[i] = dval;
		}
	}
	else {
		if (Result.CreateComplex(1,narg)) return true;

		for (int i=0; i<narg; ++i) {
			iarg2 = (int) arg2.mr[i];
			if (iarg2 < 0 || iarg2 >= nv_cols) { Result.Delete(); addError("index outside array bounds"); return true; }
			var.GetC(cval,iarg1,iarg2);
			Result.mr[i] = cval.real();
			Result.mi[i] = cval.imag();
		}
	}

	return false;
}

bool matAccess(cMatrix& Result, cMatrix& var, cMatrix& arg1, double darg2)
{
	if (!arg1.IsVector()) { addError("illegal argument"); return true; }

	int iarg2 = (int) darg2;
	int nvar = var.GetSize();
	int nv_rows = var.GetRows(), nv_cols = var.GetColumns();

	if (iarg2 < 0 || iarg2 >= nv_cols) { addError("index outside array bounds"); return true; }
	int iarg1;

	int narg = arg1.GetSize();
	double dval;
	complex<double> cval;

	if (var.IsReal()) {
		if (Result.Create(narg,1)) return true;

		for (int i=0; i<narg; ++i) {
			iarg1 = (int) arg1.mr[i];
			if (iarg1 < 0 || iarg1 >= nv_rows) { Result.Delete(); addError("index outside array bounds"); return true; }
			var.GetR(dval,iarg1,iarg2);
			Result.mr[i] = dval;
		}
	}
	else {
		if (Result.CreateComplex(narg,1)) return true;

		for (int i=0; i<narg; ++i) {
			iarg1 = (int) arg1.mr[i];
			if (iarg1 < 0 || iarg1 >= nv_rows) { Result.Delete(); addError("index outside array bounds"); return true; }
			var.GetC(cval,iarg1,iarg2);
			Result.mr[i] = cval.real();
			Result.mi[i] = cval.imag();
		}
	}

	return false;
}

bool matAccess(cMatrix& Result, cMatrix& var, double darg1, double darg2)
{
	int iarg1 = (int) darg1;
	int iarg2 = (int) darg2;
	int nv_rows = var.GetRows(), nv_cols = var.GetColumns();

	if (iarg1 < 0 || iarg1 >= nv_rows) { addError("index outside array bounds"); return true; }
	if (iarg2 < 0 || iarg2 >= nv_cols) { addError("index outside array bounds"); return true; }

	//double *pr, *pi;
	double dval;
	complex<double> cval;

	if (var.IsReal()) {
		if (Result.Create(1,1)) return true;
		var.GetR(dval,iarg1,iarg2);
		Result.mr[0] = dval;
	}
	else {
		if (Result.CreateComplex(1,1)) return true;
		var.GetC(cval,iarg1,iarg2);
		Result.mr[0] = cval.real();
		Result.mi[0] = cval.imag();
	}

	return false;
}

bool matAccess(cMatrix& Result, cMatrix& var, cMatrix& arg1, cMatrix& arg2)
{
	// both arguments must be vectors
	if (!arg1.IsVector() || !arg2.IsVector()) { addError("illegal argument"); return true; }

	int nvar = var.GetSize();
	int nv_rows = var.GetRows(), nv_cols = var.GetColumns();

	int iarg1, iarg2;
	int i, j, ix;

	int narg1 = arg1.GetSize();
	int narg2 = arg2.GetSize();

	double dval;
	complex<double> cval;

	if (var.IsReal()) {
		if (Result.Create(narg1,narg2)) return true;

		for (i=0; i<narg1; ++i) {

			iarg1 = (int) arg1.mr[i];
			if (iarg1 < 0 || iarg1 >= nv_rows) { Result.Delete(); addError("index outside array bounds"); return true; }

			for (j=0; j<narg2; ++j) {
				iarg2 = (int) arg2.mr[j];
				if (iarg2 < 0 || iarg2 >= nv_cols) { Result.Delete(); addError("index outside array bounds"); return true; }
				var.GetR(dval,iarg1,iarg2);

				//ix = var.GetIndex(iarg1,iarg2);
				Result.SetR(i,j,dval);	//mr[ix] = dval;
			}

		}
	}
	else {
		if (Result.CreateComplex(narg1,narg2)) return true;

		for (i=0; i<narg1; ++i) {

			iarg1 = (int) arg2.mr[i];
			if (iarg1 < 0 || iarg1 >= nv_rows) { Result.Delete(); addError("index outside array bounds"); return true; }

			for (j=0; j<narg2; ++j) {
				iarg2 = (int) arg2.mr[j];
				if (iarg2 < 0 || iarg2 >= nv_cols) { Result.Delete(); addError("index outside array bounds"); return true; }
				var.GetC(cval,iarg1,iarg2);
				ix = var.GetIndex(iarg1,iarg2);
				Result.SetR(i,j,cval.real());	//mr[ix] = cval.real();
				Result.SetI(i,j,cval.imag());	//mi[ix] = cval.imag();
			}

		}

	}

	return false;
}
*/

//
// accessor helpers
//
bool rm_rows(cMatrix& res, cMatrix& in, cMatrix& rows)
{
	res.Delete();

	vector<int> virows;
	int i, ix, r, c;
	int inrows = in.GetRows();

	if (!rows.IsVector()) {
		addError("illegal argument");
		return true;
	}

	for (i=0; i<rows.GetSize(); ++i) {
		ix = (int) rows.mr[i];
		if (ix < 0 || ix >= inrows) {
			addError("illegal argument");
			return true;
		}
		virows.push_back( ix );
	}

	// sort & unique cleaning operations on the indices
	std::sort( virows.begin(), virows.end() );
	std::vector<int>::iterator it = std::unique( virows.begin(), virows.end() );
	virows.resize( std::distance(virows.begin(),it) );

	int nrows = inrows - (int)virows.size();
	if (nrows < 0) {
		addError("illegal argument");		// this should never happen if the logic is sound, but just in case
		return true;
	}
	else if (nrows == 0) {
		return false;		// empty matrix is returned as result
	}

	double lf;
	complex<double> cv;
	ix = 0;

	if (in.IsReal()) {
		if (res.Create( nrows, in.GetColumns() )) { addError("memory error"); return true; }

		for (r=0; r<inrows; ++r) {
			if (ix < (int)virows.size() && r == virows[ix]) {
				++ix;
				continue;
			}
			for (c=0; c<in.GetColumns(); ++c) {
				in.GetR(lf, r, c);
				res.SetR(r-ix, c, lf);
			}
		}
	}
	else {
		if (res.CreateComplex( nrows, in.GetColumns() )) { addError("memory error"); return true; }

		for (r=0; r<inrows; ++r) {
			if (ix < (int)virows.size() && r == virows[ix]) {
				++ix;
				continue;
			}
			for (c=0; c<in.GetColumns(); ++c) {
				in.Get(cv, r, c);
				res.Set(r-ix, c, cv);
			}
		}
	}

	return false;
}

bool rm_cols(cMatrix& res, cMatrix& in, cMatrix& cols)
{
	res.Delete();

	vector<int> vicols;
	int i, ix, r, c;
	int incols = in.GetColumns();

	if (!cols.IsVector()) {
		addError("illegal argument");
		return true;
	}

	for (i=0; i<cols.GetSize(); ++i) {
		ix = (int) cols.mr[i];
		if (ix < 0 || ix >= incols) {
			addError("illegal argument");
			return true;
		}
		vicols.push_back( ix );
	}

	// sort & unique cleaning operations on the indices
	std::sort( vicols.begin(), vicols.end() );
	std::vector<int>::iterator it = std::unique( vicols.begin(), vicols.end() );
	vicols.resize( std::distance(vicols.begin(),it) );

	int ncols = incols - (int)vicols.size();
	if (ncols < 0) {
		addError("illegal argument");		// this should never happen if the logic is sound, but just in case
		return true;
	}
	else if (ncols == 0) {
		return false;		// empty matrix is returned as result
	}

	double lf;
	complex<double> cv;
	ix = 0;

	if (in.IsReal()) {
		if (res.Create( in.GetRows(), ncols )) { addError("memory error"); return true; }

		for (r=0; r<in.GetRows(); ++r) {
			ix = 0;
			for (c=0; c<incols; ++c) {
				if (ix < (int)vicols.size() && c == vicols[ix]) {
					++ix;
					continue;
				}
				in.GetR(lf, r, c);
				res.SetR(r, c-ix, lf);
			}
		}
	}
	else {
		if (res.CreateComplex( in.GetRows(), ncols )) { addError("memory error"); return true; }

		for (r=0; r<in.GetRows(); ++r) {
			ix = 0;
			for (c=0; c<incols; ++c) {
				if (ix < (int)vicols.size() && c == vicols[ix]) {
					++ix;
					continue;
				}
				in.Get(cv, r, c);
				res.Set(r, c-ix, cv);
			}
		}
	}

	return false;
}
