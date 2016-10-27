
#include "cube.h"
#include "my_errors.h"

//
// three args
//

bool cubeAccess(cMatrix& Result, double dvar, double darg1, double darg2, double darg3)
{
	int iarg1 = (int) darg1;
	int iarg2 = (int) darg2;
	int iarg3 = (int) darg3;
	if (iarg1==0 && iarg2==0 && iarg3==0) {
		if (Result.Create(1,1)) return true;
		Result.SetR(0,dvar);
		return false;
	}
	addError("index outside array bounds"); return true;
}

bool cubeAccess(cMatrix& Result, double dvar, double darg1, double darg2, cMatrix& arg3)
{
	int iarg1 = (int) darg1;
	int iarg2 = (int) darg2;
	if (iarg1!=0 || iarg2!=0) {
		addError("index outside array bounds"); return true;
	}

	if (!arg3.IsVector()) { addError("illegal argument"); return true; }

	if (Result.Create(arg3.GetRows(),arg3.GetColumns())) return true;

	double *pr = Result.GetR();
	const double *par = arg3.GetR();		// ignore imag part
	int n = arg3.GetSize();

	// the only valid arg is 0, anything else results in error!
	for (int i=0; i<n; ++i)
	{
		if (int(arg3.mr[i])==0)	Result.mr[i] = dvar;
		else { Result.Delete(); addError("index outside array bounds"); return true; }
	}
	return false;
}

bool cubeAccess(cMatrix& Result, double dvar, double darg1, cMatrix& arg2, double darg3)
{
	int iarg1 = (int) darg1;
	int iarg3 = (int) darg3;
	if (iarg1!=0 || iarg3!=0) {
		addError("index outside array bounds"); return true;
	}

	if (!arg2.IsVector()) { addError("illegal argument"); return true; }

	if (Result.Create(arg2.GetRows(),arg2.GetColumns())) return true;

	double *pr = Result.GetR();
	const double *par = arg2.GetR();		// ignore imag part
	int n = arg2.GetSize();

	// the only valid arg is 0, anything else results in error!
	for (int i=0; i<n; ++i)
	{
		if (int(arg2.mr[i])==0)	Result.mr[i] = dvar;
		else { Result.Delete(); addError("index outside array bounds"); return true; }
	}
	return false;
}

bool cubeAccess(cMatrix& Result, double dvar, double darg1, cMatrix& arg2, cMatrix& arg3)
{
	int iarg1 = (int) darg1;
	if (iarg1!=0) {
		addError("index outside array bounds"); return true;
	}

	if (!arg2.IsVector() || !arg3.IsVector()) { addError("illegal argument"); return true; }

	int iarg2, iarg3, i;

	int narg2 = arg2.GetSize();
	int narg3 = arg3.GetSize();

	if (Result.Create(narg2,narg3)) return true;

	for (i=0; i<narg2; ++i) {
		iarg2 = (int) arg2.mr[i];
		if (iarg2 != 0) { Result.Delete(); addError("index outside array bounds"); return true; }
	}

	for (i=0; i<narg3; ++i) {
		iarg3 = (int) arg3.mr[i];
		if (iarg3 != 0) { Result.Delete(); addError("index outside array bounds"); return true; }
	}

	Result.FillR(0, dvar, narg2*narg3);

	return false;
}

bool cubeAccess(cMatrix& Result, double dvar, cMatrix& arg1, double darg2, double darg3)
{
	int iarg2 = (int) darg2;
	int iarg3 = (int) darg3;
	if (iarg2!=0 || iarg3!=0) {
		addError("index outside array bounds"); return true;
	}

	if (!arg1.IsVector()) { addError("illegal argument"); return true; }

	if (Result.Create(arg1.GetRows(),arg1.GetColumns())) return true;

	double *pr = Result.GetR();
	const double *par = arg1.GetR();		// ignore imag part
	int n = arg1.GetSize();

	// the only valid arg is 0, anything else results in error!
	for (int i=0; i<n; ++i)
	{
		if (int(arg1.mr[i])==0)	Result.mr[i] = dvar;
		else { Result.Delete(); addError("index outside array bounds"); return true; }
	}
	return false;
}

bool cubeAccess(cMatrix& Result, double dvar, cMatrix& arg1, double darg2, cMatrix& arg3)
{
	int iarg2 = (int) darg2;
	if (iarg2!=0) {
		addError("index outside array bounds"); return true;
	}

	if (!arg1.IsVector() || !arg3.IsVector()) { addError("illegal argument"); return true; }

	int iarg1, iarg3, i;

	int narg1 = arg1.GetSize();
	int narg3 = arg3.GetSize();

	if (Result.Create(narg1,narg3)) return true;

	for (i=0; i<narg1; ++i) {
		iarg1 = (int) arg1.mr[i];
		if (iarg1 != 0) { Result.Delete(); addError("index outside array bounds"); return true; }
	}

	for (i=0; i<narg3; ++i) {
		iarg3 = (int) arg3.mr[i];
		if (iarg3 != 0) { Result.Delete(); addError("index outside array bounds"); return true; }
	}

	Result.FillR(0, dvar, narg1*narg3);

	return false;
}

bool cubeAccess(cMatrix& Result, double dvar, cMatrix& arg1, cMatrix& arg2, double darg3)
{
	int iarg3 = (int) darg3;
	if (iarg3!=0) {
		addError("index outside array bounds"); return true;
	}

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

	Result.FillR(0, dvar, narg1*narg2);

	return false;
}

bool cubeAccess(cCube& Result, double dvar, cMatrix& arg1, cMatrix& arg2, cMatrix& arg3)
{
	if (!arg1.IsVector() || !arg2.IsVector() || !arg3.IsVector()) { addError("illegal argument"); return true; }

	int iarg1, iarg2, iarg3, i;

	int narg1 = arg1.GetSize();
	int narg2 = arg2.GetSize();
	int narg3 = arg3.GetSize();

	if (Result.Create(narg1,narg2,narg3)) return true;

	for (i=0; i<narg1; ++i) {
		iarg1 = (int) arg1.mr[i];
		if (iarg1 != 0) { Result.Delete(); addError("index outside array bounds"); return true; }
	}

	for (i=0; i<narg2; ++i) {
		iarg2 = (int) arg2.mr[i];
		if (iarg2 != 0) { Result.Delete(); addError("index outside array bounds"); return true; }
	}

	for (i=0; i<narg3; ++i) {
		iarg3 = (int) arg3.mr[i];
		if (iarg3 != 0) { Result.Delete(); addError("index outside array bounds"); return true; }
	}

	Result.mr.insert( Result.mr.begin(), narg1*narg2*narg3, dvar );

	return false;
}

/*
bool cubeAccess(cMatrix& Result, cCube& var, double darg1, double darg2, double darg3)
{
	int iarg1 = (int) darg1;
	int iarg2 = (int) darg2;
	int iarg3 = (int) darg3;
	if (iarg1<0 || iarg1>=var.GetRows() ||
		iarg2<0 || iarg2>=var.GetColumns() ||
		iarg3<0 || iarg3>=var.GetDepth() ) {
		addError("index outside array bounds"); return true;
	}

	double lf, lfi;

	if (var.IsReal()) {
		Result.Create(1,1);
		var.GetR(lf, iarg1, iarg2, iarg3);
		Result.mr[0] = lf;
	}
	else {
		Result.CreateComplex(1,1);
		var.GetR(lf, iarg1, iarg2, iarg3);
		var.GetI(lfi, iarg1, iarg2, iarg3);
		Result.mr[0] = lf;
		Result.mi[0] = lfi;
	}
	return false;
}

bool cubeAccess(cMatrix& Result, cCube& var, double darg1, double darg2, cMatrix& arg3)
{
	int iarg1 = (int) darg1;
	int iarg2 = (int) darg2;
	if (iarg1<0 || iarg1>=var.GetRows() ||
		iarg2<0 || iarg2>=var.GetColumns() ) {
		addError("index outside array bounds"); return true;
	}

	if (!arg3.IsVector()) { addError("illegal argument"); return true; }

	int i, iarg3, sz = arg3.GetSize();
	double lf, lfi, darg3;

	if (var.IsReal()) {
		Result.Create( sz, 1 );
		for (i=0; i<sz; ++i) {
			arg3.GetR(darg3, i);		// ignore imag part of arg3
			iarg3 = (int) darg3;
			if (iarg3 >=0 && iarg3 < var.nm) {
				var.GetR(lf, iarg1, iarg2, iarg3);
				Result.mr[i] = lf;
			}
			else { addError("index outside array bounds"); return true; }
		}
	}
	else {
		Result.CreateComplex( sz, 1 );
		for (i=0; i<sz; ++i) {
			arg3.GetR(darg3, i);		// ignore imag part of arg3
			iarg3 = (int) darg3;
			if (iarg3 >=0 && iarg3 < var.nm) {
				var.GetR(lf, iarg1, iarg2, iarg3);
				var.GetI(lfi, iarg1, iarg2, iarg3);
				Result.mr[i] = lf;
				Result.mi[i] = lfi;
			}
			else { addError("index outside array bounds"); return true; }
		}
	}
	return false;
}

bool cubeAccess(cMatrix& Result, cCube& var, double darg1, cMatrix& arg2, double darg3)
{
	int iarg1 = (int) darg1;
	int iarg3 = (int) darg3;
	if (iarg1<0 || iarg1>=var.GetRows() ||
		iarg3<0 || iarg3>=var.GetDepth() ) {
		addError("index outside array bounds"); return true;
	}

	if (!arg2.IsVector()) { addError("illegal argument"); return true; }

	int i, iarg2, sz = arg2.GetSize();
	double lf, lfi, darg2;

	if (var.IsReal()) {
		Result.Create( sz, 1 );
		for (i=0; i<sz; ++i) {
			arg2.GetR(darg2, i);		// ignore imag part of arg2
			iarg2 = (int) darg2;
			if (iarg2 >=0 && iarg2 < var.nc) {
				var.GetR(lf, iarg1, iarg2, iarg3);
				Result.mr[i] = lf;
			}
			else { addError("index outside array bounds"); return true; }
		}
	}
	else {
		Result.CreateComplex( sz, 1 );
		for (i=0; i<sz; ++i) {
			arg2.GetR(darg2, i);		// ignore imag part of arg2
			iarg2 = (int) darg2;
			if (iarg2 >=0 && iarg2 < var.nc) {
				var.GetR(lf, iarg1, iarg2, iarg3);
				var.GetI(lfi, iarg1, iarg2, iarg3);
				Result.mr[i] = lf;
				Result.mi[i] = lfi;
			}
			else { addError("index outside array bounds"); return true; }
		}
	}
	return false;
}

bool cubeAccess(cMatrix& Result, cCube& var, double darg1, cMatrix& arg2, cMatrix& arg3)
{
	int iarg1 = (int) darg1;
	if (iarg1<0 || iarg1>=var.GetRows()) {
		addError("index outside array bounds"); return true;
	}

	if (!arg2.IsVector() || !arg3.IsVector()) { addError("illegal argument"); return true; }

	int i, j, iarg2, iarg3, sz2 = arg2.GetSize(), sz3 = arg3.GetSize();
	double lf, lfi, darg2, darg3;

	if (var.IsReal()) {
		Result.Create( sz2, sz3 );
		for (i=0; i<sz2; ++i) {
			arg2.GetR(darg2, i);			// ignore imag part of arg2
			iarg2 = (int) darg2;
			if (iarg2 >=0 && iarg2 < var.nc) {
				for (j=0; j<sz3; ++j) {
					arg3.GetR(darg3, j);	// ignore imag part of arg3
					iarg3 = (int) darg3;
					if (iarg3 >=0 && iarg3 < var.nm) {
						var.GetR(lf, iarg1, iarg2, iarg3);
						Result.SetR(i, j, lf);
					}
					else { addError("index outside array bounds"); return true; }
				}
			}
			else { addError("index outside array bounds"); return true; }
		}
	}		// IsReal
	else {
		Result.CreateComplex( sz2, sz3 );
		for (i=0; i<sz2; ++i) {
			arg2.GetR(darg2, i);			// ignore imag part of arg2
			iarg2 = (int) darg2;
			if (iarg2 >=0 && iarg2 < var.nc) {
				for (j=0; j<sz3; ++j) {
					arg3.GetR(darg3, j);	// ignore imag part of arg3
					iarg3 = (int) darg3;
					if (iarg3 >=0 && iarg3 < var.nm) {
						var.GetR(lf, iarg1, iarg2, iarg3);
						var.GetI(lfi, iarg1, iarg2, iarg3);
						Result.SetR(i, j, lf);
						Result.SetI(i, j, lfi);
					}
					else { addError("index outside array bounds"); return true; }
				}
			}
			else { addError("index outside array bounds"); return true; }
		}
	}
	return false;
}

bool cubeAccess(cMatrix& Result, cCube& var, cMatrix& arg1, double darg2, double darg3)
{
	int iarg2 = (int) darg2;
	int iarg3 = (int) darg3;
	if (iarg2<0 || iarg2>=var.GetColumns() ||
		iarg3<0 || iarg3>=var.GetDepth() ) {
		addError("index outside array bounds"); return true;
	}

	if (!arg1.IsVector()) { addError("illegal argument"); return true; }

	int i, iarg1, sz = arg1.GetSize();
	double lf, lfi, darg1;

	if (var.IsReal()) {
		Result.Create( sz, 1 );
		for (i=0; i<sz; ++i) {
			arg1.GetR(darg1, i);		// ignore imag part of arg1
			iarg1 = (int) darg1;
			if (iarg1 >=0 && iarg1 < var.nr) {
				var.GetR(lf, iarg1, iarg2, iarg3);
				Result.mr[i] = lf;
			}
			else { addError("index outside array bounds"); return true; }
		}
	}
	else {
		Result.CreateComplex( sz, 1 );
		for (i=0; i<sz; ++i) {
			arg1.GetR(darg1, i);		// ignore imag part of arg1
			iarg1 = (int) darg1;
			if (iarg1 >=0 && iarg1 < var.nr) {
				var.GetR(lf, iarg1, iarg2, iarg3);
				var.GetI(lfi, iarg1, iarg2, iarg3);
				Result.mr[i] = lf;
				Result.mi[i] = lfi;
			}
			else { addError("index outside array bounds"); return true; }
		}
	}
	return false;
}

bool cubeAccess(cMatrix& Result, cCube& var, cMatrix& arg1, double darg2, cMatrix& arg3)
{
	int iarg2 = (int) darg2;
	if (iarg2<0 || iarg2>=var.GetColumns()) {
		addError("index outside array bounds"); return true;
	}

	if (!arg1.IsVector() || !arg3.IsVector()) { addError("illegal argument"); return true; }

	int i, j, iarg1, iarg3, sz1 = arg1.GetSize(), sz3 = arg3.GetSize();
	double lf, lfi, darg1, darg3;

	if (var.IsReal()) {
		Result.Create( sz1, sz3 );
		for (i=0; i<sz1; ++i) {
			arg1.GetR(darg1, i);			// ignore imag part of arg1
			iarg1 = (int) darg1;
			if (iarg1 >=0 && iarg1 < var.nr) {
				for (j=0; j<sz3; ++j) {
					arg3.GetR(darg3, j);	// ignore imag part of arg3
					iarg3 = (int) darg3;
					if (iarg3 >=0 && iarg3 < var.nm) {
						var.GetR(lf, iarg1, iarg2, iarg3);
						Result.SetR(i, j, lf);
					}
					else { addError("index outside array bounds"); return true; }
				}
			}
			else { addError("index outside array bounds"); return true; }
		}
	}		// IsReal
	else {
		Result.CreateComplex( sz1, sz3 );
		for (i=0; i<sz1; ++i) {
			arg1.GetR(darg1, i);			// ignore imag part of arg1
			iarg1 = (int) darg1;
			if (iarg1 >=0 && iarg1 < var.nr) {
				for (j=0; j<sz3; ++j) {
					arg3.GetR(darg3, j);	// ignore imag part of arg3
					iarg3 = (int) darg3;
					if (iarg3 >=0 && iarg3 < var.nm) {
						var.GetR(lf, iarg1, iarg2, iarg3);
						var.GetI(lfi, iarg1, iarg2, iarg3);
						Result.SetR(i, j, lf);
						Result.SetI(i, j, lfi);
					}
					else { addError("index outside array bounds"); return true; }
				}
			}
			else { addError("index outside array bounds"); return true; }
		}
	}
	return false;
}

bool cubeAccess(cMatrix& Result, cCube& var, cMatrix& arg1, cMatrix& arg2, double darg3)
{
	int iarg3 = (int) darg3;
	if (iarg3<0 || iarg3>=var.GetDepth()) {
		addError("index outside array bounds"); return true;
	}

	if (!arg1.IsVector() || !arg2.IsVector()) { addError("illegal argument"); return true; }

	int i, j, iarg1, iarg2, sz1 = arg1.GetSize(), sz2 = arg2.GetSize();
	double lf, lfi, darg1, darg2;

	if (var.IsReal()) {
		Result.Create( sz1, sz2 );
		for (i=0; i<sz1; ++i) {
			arg1.GetR(darg1, i);			// ignore imag part of arg1
			iarg1 = (int) darg1;
			if (iarg1 >=0 && iarg1 < var.nr) {
				for (j=0; j<sz2; ++j) {
					arg2.GetR(darg2, j);	// ignore imag part of arg2
					iarg2 = (int) darg2;
					if (iarg2 >=0 && iarg2 < var.nc) {
						var.GetR(lf, iarg1, iarg2, iarg3);
						Result.SetR(i, j, lf);
					}
					else { addError("index outside array bounds"); return true; }
				}
			}
			else { addError("index outside array bounds"); return true; }
		}
	}		// IsReal
	else {
		Result.CreateComplex( sz1, sz2 );
		for (i=0; i<sz1; ++i) {
			arg1.GetR(darg1, i);			// ignore imag part of arg1
			iarg1 = (int) darg1;
			if (iarg1 >=0 && iarg1 < var.nr) {
				for (j=0; j<sz2; ++j) {
					arg2.GetR(darg2, j);	// ignore imag part of arg2
					iarg2 = (int) darg2;
					if (iarg2 >=0 && iarg2 < var.nc) {
						var.GetR(lf, iarg1, iarg2, iarg3);
						var.GetI(lfi, iarg1, iarg2, iarg3);
						Result.SetR(i, j, lf);
						Result.SetI(i, j, lfi);
					}
					else { addError("index outside array bounds"); return true; }
				}
			}
			else { addError("index outside array bounds"); return true; }
		}
	}
	return false;
}

bool cubeAccess(cCube& Result, cCube& var, cMatrix& arg1, cMatrix& arg2, cMatrix& arg3)
{
	// NOTE: this fcn assumes none of the args are scalars! cContainer caller should check and call appropriate fcn.
	//
	if (!arg1.IsVector() || !arg2.IsVector() || !arg3.IsVector()) { addError("illegal argument"); return true; }

	int i, j, k;
	int iarg1, iarg2, iarg3, sz1 = arg1.GetSize(), sz2 = arg2.GetSize(), sz3 = arg3.GetSize();
	double lf, lfi, darg1, darg2, darg3;

	if (var.IsReal()) {
		Result.Create( sz1, sz2, sz3 );
		for (i=0; i<sz1; ++i) {
			arg1.GetR(darg1, i);			// ignore imag part of arg1
			iarg1 = (int) darg1;
			if (iarg1 >=0 && iarg1 < var.nr) {
				for (j=0; j<sz2; ++j) {
					arg2.GetR(darg2, j);	// ignore imag part of arg2
					iarg2 = (int) darg2;
					if (iarg2 >=0 && iarg2 < var.nc) {
						for (k=0; k<sz3; ++k) {
							arg3.GetR(darg3, k);	// ignore imag part of arg3
							iarg3 = (int) darg3;
							if (iarg3 >= 0 && iarg3 < var.nm) {
								var.GetR(lf, iarg1, iarg2, iarg3);
								Result.SetR(i, j, k, lf);
							}
							else { addError("index outside array bounds"); return true; }
						}
					}
					else { addError("index outside array bounds"); return true; }
				}
			}
			else { addError("index outside array bounds"); return true; }
		}
	}		// IsReal
	else {
		Result.CreateComplex( sz1, sz2, sz3 );
		for (i=0; i<sz1; ++i) {
			arg1.GetR(darg1, i);			// ignore imag part of arg1
			iarg1 = (int) darg1;
			if (iarg1 >=0 && iarg1 < var.nr) {
				for (j=0; j<sz2; ++j) {
					arg2.GetR(darg2, j);	// ignore imag part of arg2
					iarg2 = (int) darg2;
					if (iarg2 >=0 && iarg2 < var.nc) {
						for (k=0; k<sz3; ++k) {
							arg3.GetR(darg3, k);	// ignore imag part of arg3
							iarg3 = (int) darg3;
							if (iarg3 >= 0 && iarg3 < var.nm) {
								var.GetR(lf, iarg1, iarg2, iarg3);
								var.GetI(lfi, iarg1, iarg2, iarg3);
								Result.SetR(i, j, k, lf);
								Result.SetI(i, j, k, lfi);
							}
							else { addError("index outside array bounds"); return true; }
						}
					}
					else { addError("index outside array bounds"); return true; }
				}
			}
			else { addError("index outside array bounds"); return true; }
		}
	}
	return false;
}
*/

//
// TO DO NEXT: CHANGE THIS TO ACCOMMODATE A CUBE !!
//
bool arg2ix( cCube& Result, cMatrix& arg1, cMatrix& arg2, cMatrix& arg3, int nRows, int nCols, int nDepth )
{	// convert values in arg1,arg2 to indices into a matrix of size (nRows,nCols)

	if (!arg1.IsVector() || !arg2.IsVector() || !arg3.IsVector()) { addError("illegal argument"); return true; }

	if (arg1.IsScalar()) { return arg2ix( Result, arg1.Get0R(), arg2, arg3, nRows, nCols, nDepth ); }
	if (arg2.IsScalar()) { return arg2ix( Result, arg1, arg2.Get0R(), arg3, nRows, nCols, nDepth ); }
	if (arg3.IsScalar()) { return arg2ix( Result, arg1, arg2, arg3.Get0R(), nRows, nCols, nDepth ); }

	int nr1, nr2, nr3;
	int i, j, k;

	nr1 = arg1.GetSize();
	nr2 = arg2.GetSize();
	nr3 = arg3.GetSize();
	if (Result.Create(nr1,nr2,nr3)) return true;

	for (i=0; i<nr1; ++i) {
		for (j=0; j<nr2; ++j) {
			for (k=0; k<nr3; ++k) {
				Result.SetR(i,j,k, floor( arg1.mr[i] + nRows*arg2.mr[j] + (nRows*nCols)*arg3.mr[k] ) );
			}
		}
	}

	return false;
}

bool arg2ix( cCube& Result, double d1, cMatrix& arg2, cMatrix& arg3, int nRows, int nCols, int nDepth )
{
	if (arg2.IsScalar()) { return arg2ix( Result, d1, arg2.Get0R(), arg3, nRows, nCols, nDepth ); }
	if (arg3.IsScalar()) { return arg2ix( Result, d1, arg2, arg3.Get0R(), nRows, nCols, nDepth ); }

	int i, j;
	int n2 = arg2.GetSize();
	int n3 = arg3.GetSize();

	if (Result.Create(1,n2,n3)) return true;

	for (i=0; i<n2; ++i) {
		for (j=0; j<n3; ++j) {
			Result.SetR(0,i,j, floor( d1 + nRows*arg2.mr[i] + nRows*nCols*arg3.mr[j] ) );
		}
	}

	return false;
}

bool arg2ix( cCube& Result, double d1, cMatrix& arg2, double d3, int nRows, int nCols, int nDepth )
{	// assumes arg2 has already been checked for scalar condition
	int i;
	int n2 = arg2.GetSize();

	if (Result.Create(1,n2,1)) return true;

	for (i=0; i<n2; ++i) {
		Result.SetR(0,i,0, floor( d1 + nRows*arg2.mr[i] + nRows*nCols*d3 ) );
	}

	return false;
}

bool arg2ix( cCube& Result, double d1, double d2, cMatrix& arg3, int nRows, int nCols, int nDepth )
{
	int i;
	int n3 = arg3.GetSize();

	if (Result.Create(1,1,n3)) return true;

	for (i=0; i<n3; ++i) {
		Result.SetR(0,0,i, floor( d1 + nRows*d2 + nRows*nCols*arg3.mr[i] ) );
	}

	return false;
}

bool arg2ix( cCube& Result, cMatrix& arg1, double d2, cMatrix& arg3, int nRows, int nCols, int nDepth )
{
	if (arg3.IsScalar()) { return arg2ix( Result, arg1, d2, arg3.Get0R(), nRows, nCols, nDepth ); }

	int i, j;
	int n1 = arg1.GetSize();
	int n3 = arg3.GetSize();

	if (Result.Create(n1,1,n3)) return true;

	for (i=0; i<n1; ++i) {
		for (j=0; j<n3; ++j) {
			Result.SetR(i,0,j, floor( arg1.mr[i] + nRows*d2 + nRows*nCols*arg3.mr[j] ) );
		}
	}

	return false;
}

bool arg2ix( cCube& Result, cMatrix& arg1, double d2, double d3, int nRows, int nCols, int nDepth )
{
	int i;
	int n1 = arg1.GetSize();

	if (Result.Create(n1,1,1)) return true;

	for (i=0; i<n1; ++i) {
		Result.SetR(i,0,0, floor( arg1.mr[i] + nRows*d2 + nRows*nCols*d3 ) );
	}

	return false;
}

bool arg2ix( cCube& Result, cMatrix& arg1, cMatrix& arg2, double d3, int nRows, int nCols, int nDepth )
{
	if (arg2.IsScalar()) { return arg2ix( Result, arg1, arg2.Get0R(), d3, nRows, nCols, nDepth ); }

	int i, j;
	int n1 = arg1.GetSize();
	int n2 = arg2.GetSize();

	if (Result.Create(n1,n2,1)) return true;

	for (i=0; i<n1; ++i) {
		for (j=0; j<n2; ++j) {
			Result.SetR(i,j,0, floor( arg1.mr[i] + nRows*arg2.mr[j] + nRows*nCols*d3 ) );
		}
	}

	return false;
}



//
// accessor helpers
//
bool rm_rows(cCube& res, cCube& in, cMatrix& rows)
{
	res.Delete();

	vector<int> virows;
	int i, ix, r, c, m;
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
		if (res.Create( nrows, in.GetColumns(), in.GetDepth() )) { addError("memory error"); return true; }

		for (m=0; m<in.GetDepth(); ++m) {
			for (r=0; r<inrows; ++r) {
				if (ix < (int)virows.size() && r == virows[ix]) {
					++ix;
					continue;
				}
				for (c=0; c<in.GetColumns(); ++c) {
					in.GetR(lf, r, c, m);
					res.SetR(r-ix, c, m, lf);
				}
			}
		}
	}
	else {
		if (res.CreateComplex( nrows, in.GetColumns(), in.GetDepth() )) { addError("memory error"); return true; }

		for (m=0; m<in.GetDepth(); ++m) {
			for (r=0; r<inrows; ++r) {
				if (ix < (int)virows.size() && r == virows[ix]) {
					++ix;
					continue;
				}
				for (c=0; c<in.GetColumns(); ++c) {
					in.Get(cv, r, c, m);
					res.Set(r-ix, c, m, cv);
				}
			}
		}
	}

	return false;
}

bool rm_cols(cCube& res, cCube& in, cMatrix& cols)
{
	res.Delete();

	vector<int> vicols;
	int i, ix, r, c, m;
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
		if (res.Create( in.GetRows(), ncols, in.GetDepth() )) { addError("memory error"); return true; }

		for (m=0; m<in.GetDepth(); ++m) {
			for (r=0; r<in.GetRows(); ++r) {
				ix = 0;
				for (c=0; c<incols; ++c) {
					if (ix < (int)vicols.size() && c == vicols[ix]) {
						++ix;
						continue;
					}
					in.GetR(lf, r, c, m);
					res.SetR(r, c-ix, m, lf);
				}
			}
		}
	}
	else {
		if (res.CreateComplex( in.GetRows(), ncols, in.GetDepth() )) { addError("memory error"); return true; }

		for (m=0; m<in.GetDepth(); ++m) {
			for (r=0; r<in.GetRows(); ++r) {
				ix = 0;
				for (c=0; c<incols; ++c) {
					if (ix < (int)vicols.size() && c == vicols[ix]) {
						++ix;
						continue;
					}
					in.Get(cv, r, c, m);
					res.Set(r, c-ix, m, cv);
				}
			}
		}
	}

	return false;
}

bool rm_depths(cCube& res, cCube& in, cMatrix& deps)
{
	res.Delete();

	vector<int> videps;
	int i, ix, r, c, m;
	int indeps = in.GetDepth();

	if (!deps.IsVector()) {
		addError("illegal argument");
		return true;
	}

	for (i=0; i<deps.GetSize(); ++i) {
		ix = (int) deps.mr[i];
		if (ix < 0 || ix >= indeps) {
			addError("illegal argument");
			return true;
		}
		videps.push_back( ix );
	}

	// sort & unique cleaning operations on the indices
	std::sort( videps.begin(), videps.end() );
	std::vector<int>::iterator it = std::unique( videps.begin(), videps.end() );
	videps.resize( std::distance(videps.begin(),it) );

	int ndeps = indeps - (int)videps.size();
	if (ndeps < 0) {
		addError("illegal argument");		// this should never happen if the logic is sound, but just in case
		return true;
	}
	else if (ndeps == 0) {
		return false;		// empty matrix is returned as result
	}

	double lf;
	complex<double> cv;
	ix = 0;

	if (in.IsReal()) {
		if (res.Create( in.GetRows(), in.GetColumns(), ndeps )) { addError("memory error"); return true; }

		for (m=0; m<indeps; ++m) {
			if (ix < (int)videps.size() && m == videps[ix]) {
				++ix;
				continue;
			}
			for (r=0; r<in.GetRows(); ++r) {
				for (c=0; c<in.GetColumns(); ++c) {
					in.GetR(lf, r, c, m);
					res.SetR(r, c, m-ix, lf);
				}
			}
		}
	}
	else {
		if (res.CreateComplex( in.GetRows(), in.GetColumns(), ndeps )) { addError("memory error"); return true; }

		for (m=0; m<indeps; ++m) {
			if (ix < (int)videps.size() && m == videps[ix]) {
				++ix;
				continue;
			}
			for (r=0; r<in.GetRows(); ++r) {
				ix = 0;
				for (c=0; c<in.GetColumns(); ++c) {
					in.Get(cv, r, c, m);
					res.Set(r, c, m-ix, cv);
				}
			}
		}
	}

	return false;
}
