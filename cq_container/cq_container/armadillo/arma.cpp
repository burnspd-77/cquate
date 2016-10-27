#include "armadillo"
#include "matrix.h"

using namespace arma;
using std::vector;

// conversion helpers
//
void Arma2MatR(cMatrix& M, mat& am)
{
	const double *pq = am.memptr();
	M.DeleteCopyR( pq, am.n_rows, am.n_cols );
}

void Arma2MatC(cMatrix& M, Mat<complex<double> >& am)
{
	M.Delete();
	const complex<double> *pq = am.memptr();
	int size = am.n_elem;
	M.mr.reserve(size); M.mi.reserve(size);

	M.nr = am.n_rows;
	M.nc = am.n_cols;

	for (int i=0; i<size; ++i)
	{
		M.mr.push_back( am(i).real() );
		M.mi.push_back( am(i).imag() );
	}
}

void Arma2MatI(cMatrix& M, Mat<int>& am)
{
	M.Delete();
	const int *pq = am.memptr();
	int size = am.n_elem;
	M.mr.reserve(size);

	M.nr = am.n_rows;
	M.nc = am.n_cols;

	for (int i = 0; i<size; ++i)
	{
		M.mr.push_back(double(am(i)));
	}
}

void Arma2MatSt(cMatrix& M, Mat<size_t>& am)
{
	M.Delete();
	const size_t *pq = am.memptr();
	int size = am.n_elem;
	M.mr.reserve(size);

	M.nr = am.n_rows;
	M.nc = am.n_cols;

	for (int i = 0; i<size; ++i)
	{
		M.mr.push_back( double(am(i)) );
	}
}

void Mat2ArmaC(Mat<complex<double> >& am, cMatrix C)
{
	// construct armadillo mat
	vector<complex<double> > cx;
	complex<double> c;

	cx.reserve( C.GetSize() );

	for (int i=0; i<C.GetSize(); ++i)
	{
		cx.push_back( complex<double>(C.mr[i],C.mi[i]) );
	}
	am = cx;
	am.reshape( C.GetRows(), C.GetColumns() );
}

void ArmaVec2MatR(cMatrix& M, vec& vec)
{
	M.Delete();
	const double *pq = vec.memptr();
	int size = vec.n_elem;
	M.mr.reserve(size);

	M.nr = vec.n_rows;
	M.nc = vec.n_cols;

	for (int i=0; i<size; ++i)
	{
		M.mr.push_back( vec(i) );
	}
}

void ArmaVec2MatC(cMatrix& M, cx_vec& vec)
{
	M.Delete();
	const complex<double> *pq = vec.memptr();
	int size = vec.n_elem;
	M.mr.reserve(size); M.mi.reserve(size);

	M.nr = vec.n_rows;
	M.nc = vec.n_cols;

	for (int i=0; i<size; ++i)
	{
		M.mr.push_back( vec(i).real() );
		M.mi.push_back( vec(i).imag() );
	}
}

void Mat2ArmaVecR(cx_vec& vec, cMatrix& R)
{
	vector<complex<double> > cx;
	complex<double> c;

	cx.reserve( R.GetSize() );

	for (int i=0; i<R.GetSize(); ++i)
	{
		cx.push_back( complex<double>(R.mr[i],0.) );
	}
	vec = cx;
}

void Mat2ArmaVecC(cx_vec& vec, cMatrix& C)
{
	vector<complex<double> > cx;
	complex<double> c;

	cx.reserve( C.GetSize() );

	for (int i=0; i<C.GetSize(); ++i)
	{
		cx.push_back( complex<double>(C.mr[i],C.mi[i]) );
	}
	vec = cx;
}

void Mat2ArmaVecR(vec& vec, cMatrix& R)
{
	vec = R.mr;
}

void Mat2ArmaVecC(vec& vec, cMatrix& C)
{	// ignore imaginary part
	vec = C.mr;
}

// QR decomp
//
bool arma_qr_R(cMatrix& Q, cMatrix& R, cMatrix& X)
{
	// construct armadillo mat
	mat A(X.mr), aQ, aR;
	A.reshape( X.GetRows(), X.GetColumns() );

	if (!qr( aQ, aR, A )) { addError("QR decomposition failed"); return true; }

	// copy back to cMatrix
	Arma2MatR(Q, aQ);
	Arma2MatR(R, aR);

	return false;
}

bool arma_qr_C(cMatrix& Q, cMatrix& R, cMatrix& X)
{
	// construct armadillo mat
	Mat<complex<double> > A, aQ, aR;
	Mat2ArmaC(A, X);

	if (!qr( aQ, aR, A )) { addError("QR decomposition failed"); return true; }

	// copy back to cMatrix
	Arma2MatC( Q, aQ );
	Arma2MatC( R, aR );

	return false;
}

// interface fcn
//
bool arma_qr(cMatrix& Q, cMatrix& R, cMatrix& X)
{
	if (X.IsReal())
		return arma_qr_R(Q, R, X);
	else
		return arma_qr_C(Q, R, X);
}

// LU
//
bool arma_lu_R(cMatrix& L, cMatrix& U, cMatrix& X)
{
	// construct armadillo mat
	mat A(X.mr), aL, aU;
	A.reshape( X.GetRows(), X.GetColumns() );

	if (!lu( aL, aU, A )) { addError("LU decomposition failed"); return true; }

	// copy back to cMatrix
	Arma2MatR(L, aL);
	Arma2MatR(U, aU);

	return false;
}

bool arma_lu_C(cMatrix& L, cMatrix& U, cMatrix& X)
{
	// construct armadillo mat
	Mat<complex<double> > A, aL, aU;
	Mat2ArmaC(A, X);

	if (!lu( aL, aU, A )) { addError("LU decomposition failed"); return true; }

	// copy back to cMatrix
	Arma2MatC( L, aL );
	Arma2MatC( U, aU );

	return false;
}

bool arma_lu_R(cMatrix& L, cMatrix& U, cMatrix& P, cMatrix& X)
{
	// construct armadillo mat
	mat A(X.mr), aL, aU, aP;
	A.reshape( X.GetRows(), X.GetColumns() );

	if (!lu( aL, aU, aP, A )) { addError("LU decomposition failed"); return true; }

	// copy back to cMatrix
	Arma2MatR(L, aL);
	Arma2MatR(U, aU);
	Arma2MatR(P, aP);

	return false;
}


bool arma_lu_C(cMatrix& L, cMatrix& U, cMatrix& P, cMatrix& X)
{
	// construct armadillo mat
	Mat<complex<double> > A, aL, aU, aP;
	Mat2ArmaC(A, X);

	if (!lu( aL, aU, aP, A )) { addError("LU decomposition failed"); return true; }

	// copy back to cMatrix
	Arma2MatC( L, aL );
	Arma2MatC( U, aU );
	Arma2MatC( P, aP );

	return false;
}

// interface fcn
//
bool arma_lu(cMatrix& L, cMatrix& U, cMatrix& X)
{
	if (X.IsReal())
		return arma_lu_R(L, U, X);
	else
		return arma_lu_C(L, U, X);
}

bool arma_lu(cMatrix& L, cMatrix& U, cMatrix& P, cMatrix& X)
{
	if (X.IsReal())
		return arma_lu_R(L, U, P, X);
	else
		return arma_lu_C(L, U, P, X);
}

// Cholesky decomp
//
bool arma_chol_R(cMatrix& res, cMatrix& X)
{
	// construct armadillo mat
	mat A(X.mr), ar;
	A.reshape( X.GetRows(), X.GetColumns() );

	if (!chol( ar, A )) { addError("Cholesky decomposition failed"); return true; }

	// copy back to cMatrix
	Arma2MatR(res, ar);

	return false;
}

bool arma_chol_C(cMatrix& res, cMatrix& X)
{
	// construct armadillo mat
	Mat<complex<double> > A, ar;
	Mat2ArmaC(A, X);

	if (!chol( ar, A )) { addError("Cholesky decomposition failed"); return true; }

	// copy back to cMatrix
	Arma2MatC( res, ar );

	return false;
}

// interface fcn
//
bool arma_chol(cMatrix& res, cMatrix& X)
{
	if (X.IsReal())
		return arma_chol_R(res, X);
	else
		return arma_chol_C(res, X);
}

// left eigensystem
//
bool arma_eig_R(cMatrix& val, cMatrix& vec, cMatrix& X)
{
	// construct armadillo mat
	mat A(X.mr);
	Mat<complex<double> > avec;
	cx_vec aval;
	A.reshape( X.GetRows(), X.GetColumns() );

	if (!eig_gen( aval, avec, A )) { addError("Eigen decomposition failed"); return true; }

	// copy back to cMatrix
	Arma2MatC(vec, avec);
	ArmaVec2MatC(val, aval);

	return false;
}

bool arma_eig_C(cMatrix& val, cMatrix& vec, cMatrix& X)
{
	// construct armadillo mat
	Mat<complex<double> > A;
	Mat<complex<double> > avec;
	cx_vec aval;

	Mat2ArmaC(A, X);
	A.reshape( X.GetRows(), X.GetColumns() );

	if (!eig_gen( aval, avec, A )) { addError("Eigen decomposition failed"); return true; }

	// copy back to cMatrix
	Arma2MatC(vec, avec);
	ArmaVec2MatC(val, aval);

	return false;
}

// interface fcns
//
bool arma_eig(cMatrix& val, cMatrix& vec, cMatrix& X)
{
	if (X.IsReal())
		return arma_eig_R(val, vec, X);
	else
		return arma_eig_C(val, vec, X);
}

bool arma_eig(cMatrix& val, cMatrix& X)
{
	cMatrix vec;
	if (X.IsReal())
		return arma_eig_R(val, vec, X);
	else
		return arma_eig_C(val, vec, X);
}

// right eigensystem
//
bool arma_eigr_R(cMatrix& val, cMatrix& vec, cMatrix& X)
{
	// construct armadillo mat
	mat A(X.mr);
	Mat<complex<double> > avec;
	cx_vec aval;
	A.reshape( X.GetRows(), X.GetColumns() );
	char r = 'r';

//	if (!eig_gen( aval, avec, A, r )) { addError("Eigen decomposition failed"); return true; }
	if (!eig_gen( aval, avec, A )) { addError("Eigen decomposition failed"); return true; }

	// copy back to cMatrix
	Arma2MatC(vec, avec);
	ArmaVec2MatC(val, aval);

	return false;
}

bool arma_eigr_C(cMatrix& val, cMatrix& vec, cMatrix& X)
{
	// construct armadillo mat
	Mat<complex<double> > A;
	Mat<complex<double> > avec;
	cx_vec aval;

	Mat2ArmaC(A, X);
	char r = 'r';

//	if (!eig_gen( aval, avec, A, r )) { addError("Eigen decomposition failed"); return true; }
	if (!eig_gen( aval, avec, A )) { addError("Eigen decomposition failed"); return true; }

	// copy back to cMatrix
	Arma2MatC(vec, avec);
	ArmaVec2MatC(val, aval);

	return false;
}

// interface fcns
//
bool arma_eigr(cMatrix& val, cMatrix& vec, cMatrix& X)
{
	if (X.IsReal())
		return arma_eigr_R(val, vec, X);
	else
		return arma_eigr_C(val, vec, X);
}

bool arma_eigr(cMatrix& val, cMatrix& X)
{
	cMatrix vec;
	return arma_eigr(val, vec, X);
}


bool arma_svd_R(cMatrix& U, cMatrix& S, cMatrix& V, cMatrix& X)
{
	// construct armadillo mat
	mat A(X.mr), aU, aV;
	vec aS;
	A.reshape( X.GetRows(), X.GetColumns() );

	if (!svd( aU, aS, aV, A, "dc" )) { addError("Singular value decomposition failed"); return true; }

	// copy back to cMatrix
	Arma2MatR(U, aU);
	ArmaVec2MatR(S, aS);
	Arma2MatR(V, aV);

	return false;
}

bool arma_svd_C(cMatrix& U, cMatrix& S, cMatrix& V, cMatrix& X)
{
	// construct armadillo mat
	cx_mat A, aU, aV;
	Mat2ArmaC(A, X);
	vec aS;

	if (!svd( aU, aS, aV, A, "standard" )) { addError("Singular value decomposition failed"); return true; }

	// copy back to cMatrix
	Arma2MatC(U, aU);
	ArmaVec2MatR(S, aS);
	Arma2MatC(V, aV);

	return false;
}

bool arma_svd(cMatrix& U, cMatrix& S, cMatrix& V, cMatrix& X)
{
	if (X.IsReal())
		return arma_svd_R(U, S, V, X);
	else
		return arma_svd_C(U, S, V, X);
}

bool arma_svd(cMatrix& U, cMatrix& S, cMatrix& X)
{
	cMatrix V;
	return arma_svd(U, S, V, X);
}

bool arma_svd(cMatrix& S, cMatrix& X)
{
	cMatrix U, V;
	return arma_svd(U, S, V, X);
}

bool arma_rank_R(cMatrix& res, cMatrix& X)
{
	// construct armadillo mat
	mat A(X.mr), ar;
	A.reshape( X.GetRows(), X.GetColumns() );

	uword uw = rank( A );

	res.Create(1,1);
	res.mr[0] = (double) uw;

	return false;
}

bool arma_rank_C(cMatrix& res, cMatrix& X)
{
	// construct armadillo mat
	cx_mat A, ar;
	Mat2ArmaC(A, X);

	uword uw = rank( A );

	res.Create(1,1);
	res.mr[0] = (double) uw;

	return false;
}

bool arma_rank_R(cMatrix& res, cMatrix& X, cMatrix& tol)
{
	// construct armadillo mat
	mat A(X.mr), ar;
	A.reshape( X.GetRows(), X.GetColumns() );
	double lftol = tol.Get0R();

	uword uw = rank( A, lftol );

	res.Create(1,1);
	res.mr[0] = (double) uw;

	return false;
}

bool arma_rank_C(cMatrix& res, cMatrix& X, cMatrix& tol)
{
	// construct armadillo mat
	cx_mat A, ar;
	Mat2ArmaC(A, X);
	double lftol = tol.Get0R();

	uword uw = rank( A, lftol );

	res.Create(1,1);
	res.mr[0] = (double) uw;

	return false;
}

bool arma_rank(cMatrix& res, cMatrix& X)
{
	if (X.IsReal())
		return arma_rank_R(res, X);
	else
		return arma_rank_C(res, X);
}

bool arma_rank(cMatrix& res, cMatrix& X, cMatrix& tol)
{
	if (!tol.IsScalar()) {
		addError("expected scalar value for tolerance"); return true;
	}
	if (X.IsReal())
		return arma_rank_R(res, X, tol);
	else
		return arma_rank_C(res, X, tol);
}

bool arma_eps_R(cMatrix& res, cMatrix& X)
{
	// construct armadillo mat
	mat A(X.mr), ar;
	A.reshape( X.GetRows(), X.GetColumns() );

	mat B = eps( A );

	// copy back to cMatrix
	Arma2MatR(res, B);

	return false;
}

bool arma_eps(cMatrix& res, cMatrix& X)
{
	cMatrix Y;

	if (X.IsReal())
		return arma_eps_R(res, X);
	else {
		Real(Y, X);
		return arma_eps_R(res, Y);
	}
}

bool arma_solve_R(cMatrix& X, cMatrix& A, cMatrix& B)
{
	// construct armadillo mat
	mat aA(A.mr), aB(B.mr);
	vec ax;
	aA.reshape( A.GetRows(), A.GetColumns() );
	aB.reshape( B.GetRows(), B.GetColumns() );

	if (!solve(ax, aA, aB)) { addError("Solve failed"); return true; }

	// copy back to cMatrix
	ArmaVec2MatR(X, ax);

	return false;
}

bool arma_solve_C(cMatrix& X, cMatrix& A, cMatrix& B)
{
	// construct armadillo mat
	cx_mat aA, aB;
	cx_vec ax;
	Mat2ArmaC(aA, A);
	Mat2ArmaC(aB, B);

	if (!solve(ax, aA, aB)) { addError("Solve failed"); return true; }

	// copy back to cMatrix
	ArmaVec2MatC(X, ax);

	return false;
}

// interface fcn
//
bool arma_solve(cMatrix& X, cMatrix& A, cMatrix& B)
{
	if (A.IsReal() && B.IsReal())
		return arma_solve_R(X, A, B);
	else {
		A.forceComplex();
		B.forceComplex();
		return arma_solve_C(X, A, B);
	}
}

bool arma_syl_R(cMatrix& res, cMatrix& A, cMatrix& B, cMatrix& C)
{
	// construct armadillo mat
	mat aA(A.mr), aB(B.mr), aC(C.mr);
	mat ar;
	aA.reshape( A.GetRows(), A.GetColumns() );
	aB.reshape( B.GetRows(), B.GetColumns() );
	aC.reshape( C.GetRows(), C.GetColumns() );

	if (!syl(ar, aA, aB, aC)) { addError("Sylvester solve failed"); return true; }

	// copy back to cMatrix
	Arma2MatR(res, ar);

	return false;
}

bool arma_syl_C(cMatrix& res, cMatrix& A, cMatrix& B, cMatrix& C)
{
	// construct armadillo mat
	cx_mat aA, aB, aC;
	cx_mat ar;

	Mat2ArmaC( aA, A );
	Mat2ArmaC( aB, B );
	Mat2ArmaC( aC, C );

	if (!syl(ar, aA, aB, aC)) { addError("Sylvester solve failed"); return true; }

	// copy back to cMatrix
	Arma2MatC(res, ar);

	return false;
}

bool arma_syl(cMatrix& res, cMatrix& A, cMatrix& B, cMatrix& C)
{
	if (A.IsReal() && B.IsReal() && C.IsReal())
		return arma_syl_R(res, A, B, C);
	else {
		A.forceComplex();
		B.forceComplex();
		C.forceComplex();
		return arma_syl_C(res, A, B, C);
	}
}

//
bool arma_inv_R(cMatrix& res, cMatrix& X)
{
	// construct armadillo mat
	mat A(X.mr), ar;
	A.reshape( X.GetRows(), X.GetColumns() );

	if (!inv(ar, A)) { addError("Matrix inversion failed"); return true; }

	Arma2MatR(res, ar);

	return false;
}

bool arma_inv_C(cMatrix& res, cMatrix& X)
{
	// construct armadillo mat
	cx_mat A, ar;
	Mat2ArmaC( A, X );

	if (!inv(ar, A)) { addError("Matrix inversion failed"); return true; }

	Arma2MatC(res, ar);

	return false;
}

bool arma_inv(cMatrix& res, cMatrix& X)
{
	res.Delete();
	if (X.IsEmpty())
		return false;

	if (!X.IsSquare()) {
		addError("Error: cannot perform inverse of non-square matrix");
		return true;
	}

	if (X.IsReal())
		return arma_inv_R(res, X);
	else {
		return arma_inv_C(res, X);
	}
}

bool arma_pinv_R(cMatrix& res, cMatrix& X)
{
	// construct armadillo mat
	mat A(X.mr), ar;
	A.reshape( X.GetRows(), X.GetColumns() );

	if (!pinv(ar, A)) { addError("Pseudo-inversion failed"); return true; }

	Arma2MatR(res, ar);

	return false;
}

bool arma_pinv_C(cMatrix& res, cMatrix& X)
{
	// construct armadillo mat
	cx_mat A, ar;
	Mat2ArmaC( A, X );

	if (!pinv(ar, A)) { addError("Pseudo-inversion failed"); return true; }

	Arma2MatC(res, ar);

	return false;
}

bool arma_pinv(cMatrix& res, cMatrix& X)
{
	if (X.IsReal())
		return arma_pinv_R(res, X);
	else {
		return arma_pinv_C(res, X);
	}
}

bool arma_det_R(cMatrix& res, cMatrix& X)
{
	// construct armadillo mat
	mat A(X.mr), ar;
	A.reshape( X.GetRows(), X.GetColumns() );

	double lf = det( A );

	res.Create(1,1);
	res.mr[0] = lf;

	return false;
}

bool arma_det_C(cMatrix& res, cMatrix& X)
{
	// construct armadillo mat
	cx_mat A, ar;
	Mat2ArmaC( A, X );

	complex<double> c = det( A );

	res.CreateComplex(1,1);
	res.mr[0] = c.real();
	res.mi[0] = c.imag();

	return false;
}

bool arma_det(cMatrix& res, cMatrix& X)
{
	if (X.IsReal())
		return arma_det_R(res, X);
	else
		return arma_det_C(res, X);
}

bool arma_logdet_R(cMatrix& res, cMatrix& sign, cMatrix& X)
{
	// construct armadillo mat
	mat A(X.mr);
	A.reshape( X.GetRows(), X.GetColumns() );
	double lf, s;

	if (!log_det( lf, s, A )) { addError("log determinant failed"); return true; }

	res.Create(1,1);
	res.mr[0] = lf;

	sign.Create(1,1);
	sign.mr[0] = s;

	return false;
}

bool arma_logdet_C(cMatrix& res, cMatrix& sign, cMatrix& X)
{
	// construct armadillo mat
	cx_mat A;
	Mat2ArmaC( A, X );

	complex<double> c;
	double s;
	
	if (!log_det( c, s, A )) { addError("log determinant failed"); return true; }

	res.CreateComplex(1,1);
	res.mr[0] = c.real();
	res.mi[0] = c.imag();

	sign.Create(1,1);
	sign.mr[0] = s;

	return false;
}

bool arma_logdet(cMatrix& res, cMatrix& sign, cMatrix& X)
{
	if (X.IsReal())
		return arma_logdet_R(res, sign, X);
	else
		return arma_logdet_C(res, sign, X);
}

//
// conv
//
bool conv_vec_vec_R(cMatrix& res, cMatrix& X, cMatrix& Y)
{
	vec A(X.mr);
	vec B(Y.mr);

	vec C = conv(A,B);

	ArmaVec2MatR(res, C);
	return false;
}

bool conv_vec_mat_R(cMatrix& res, cMatrix& v, cMatrix& Y)
{	// one vector v convolved with each column of Y
	int cols = Y.nc;
	vec av(v.mr);
	cMatrix c;
	res.Delete();

	for (int i=0; i<cols; ++i)
	{
		Y.GetColumn(&c,i);
		vec y(c.mr);
		vec r = conv(av,y);

		ArmaVec2MatR(c, r);
		res.AugmentColumnsR(&c);
	}
	return false;
}

bool conv_mat_mat_R(cMatrix& res, cMatrix& X, cMatrix& Y)
{	// each column of X convolved with each column of Y
	res.Delete();

	if (X.nc != Y.nc)
		return false;

	int cols = Y.nc;
	cMatrix x, y, c;

	for (int i=0; i<cols; ++i)
	{
		X.GetColumn(&x,i);
		Y.GetColumn(&y,i);
		vec ax(x.mr);
		vec ay(y.mr);

		vec r = conv(ax,ay);

		ArmaVec2MatR(c, r);
		res.AugmentColumnsR(&c);
	}
	return false;
}

bool arma_conv_R(cMatrix& res, cMatrix& X, cMatrix& Y)
{
	if (X.IsVector() && Y.IsVector()) {
		return conv_vec_vec_R(res, X, Y);
	}
	else if (X.IsVector())
		return conv_vec_mat_R(res, X, Y);
	else if (Y.IsVector())
		return conv_vec_mat_R(res, Y, X);
	else
		return conv_mat_mat_R(res, X, Y);
}


bool conv_vec_vec_C(cMatrix& res, cMatrix& X, cMatrix& Y)
{
	cx_vec ax, ay;
	Mat2ArmaVecC(ax, X);
	Mat2ArmaVecC(ay, Y);

	cx_vec c = conv(ax,ay);

	ArmaVec2MatC(res, c);
	return false;
}

bool conv_vec_mat_C(cMatrix& res, cMatrix& v, cMatrix& Y)
{	// one vector v convolved with each column of Y
	int cols = Y.nc;
	cx_vec av, ay;
	cMatrix c;
	res.Delete();

	Mat2ArmaVecC(av, v);

	for (int i=0; i<cols; ++i)
	{
		Y.GetColumn(&c,i);
		Mat2ArmaVecC(ay, c);

		cx_vec r = conv(av,ay);

		ArmaVec2MatC(c, r);
		res.AugmentColumnsR(&c);
	}
	return false;
}

bool conv_mat_mat_C(cMatrix& res, cMatrix& X, cMatrix& Y)
{	// each column of X convolved with each column of Y
	res.Delete();

	if (X.nc != Y.nc)
		return false;

	int cols = Y.nc;
	cMatrix x, y, c;
	cx_vec ax, ay;

	for (int i=0; i<cols; ++i)
	{
		X.GetColumn(&x,i);
		Y.GetColumn(&y,i);

		Mat2ArmaVecC(ax, X);
		Mat2ArmaVecC(ay, Y);

		cx_vec r = conv(ax,ay);

		ArmaVec2MatC(c, r);
		res.AugmentColumnsR(&c);
	}
	return false;
}

bool arma_conv_C(cMatrix& res, cMatrix& X, cMatrix& Y)
{
	if (X.IsVector() && Y.IsVector()) {
		return conv_vec_vec_C(res, X, Y);
	}
	else if (X.IsVector() || Y.IsVector())
		return conv_vec_mat_C(res, X, Y);
	else
		return conv_mat_mat_C(res, X, Y);
}

bool arma_conv(cMatrix& res, cMatrix& X, cMatrix& Y)
{
	if (X.IsReal() && Y.IsReal())
		return arma_conv_R(res, X, Y);
	else {
		X.forceComplex();
		Y.forceComplex();
		return arma_conv_C(res, X, Y);
	}
}

//
bool arma_trace_R(cMatrix& res, cMatrix& X)
{
	// construct armadillo mat
	mat A(X.mr);
	A.reshape( X.GetRows(), X.GetColumns() );

	double lf = trace( A );

	res.Create(1,1);
	res.mr[0] = lf;

	return false;
}

bool arma_trace_C(cMatrix& res, cMatrix& X)
{
	// construct armadillo mat
	cx_mat A;
	Mat2ArmaC( A, X );

	complex<double> c = trace( A );

	res.CreateComplex(1,1);
	res.mr[0] = c.real();
	res.mi[0] = c.imag();

	return false;
}

bool arma_trace(cMatrix& res, cMatrix& X)
{
	if (X.IsReal())
		return arma_trace_R(res, X);
	else
		return arma_trace_C(res, X);
}


//
bool arma_shuffle_R(cMatrix& res, cMatrix& X, int dim)
{
	// construct armadillo mat
	mat A(X.mr);
	A.reshape( X.GetRows(), X.GetColumns() );

	mat B = shuffle( A, dim );
	Arma2MatR(res, B);
	
	return false;
}

bool arma_shuffle_C(cMatrix& res, cMatrix& X, int dim)
{
	// construct armadillo mat
	cx_mat A;
	Mat2ArmaC(A, X);

	cx_mat B = shuffle( A, dim );
	Arma2MatC(res, B);
	
	return false;
}

bool arma_shuffle(cMatrix& res, cMatrix& X, int dim)
{
	if (X.IsReal())
		return arma_shuffle_R(res, X, dim);
	else
		return arma_shuffle_C(res, X, dim);
}

bool arma_shuffle_rows(cMatrix& res, cMatrix& X)
{
	if (X.IsReal())
		return arma_shuffle_R(res, X, 0);
	else
		return arma_shuffle_C(res, X, 0);
}

bool arma_shuffle_cols(cMatrix& res, cMatrix& X)
{
	if (X.IsReal())
		return arma_shuffle_R(res, X, 1);
	else
		return arma_shuffle_C(res, X, 1);
}

//
// constants
//
// most values taken from NIST web site (http://physics.nist.gov/cgi-bin/cuu/Value?r)
//

bool get_inf(cMatrix& res)
{
	double inf = datum::inf;
	res.SetTo(inf);
	return false;
}

bool get_pi(cMatrix& res)
{
	res.SetTo(PI);
	return false;
}

bool get_i(cMatrix& res)
{
	res.SetTo(complex<double>(0.,1.));
	return false;
}

bool get_e(cMatrix& res)
{
	double inf = datum::e;
	res.SetTo(inf);
	return false;
}

bool get_light_speed(cMatrix& res)
{	// light speed in vacuum
	//double c = datum::c_0;
	double lf = 299792458.;
	res.SetTo(lf);
	return false;
}

bool get_boltzmann(cMatrix& res)
{	// Boltzmann constant in J/kelvin
	//double lf = datum::k;
	double lf = 1.3806488e-23;
	res.SetTo(lf);
	return false;
}

bool get_avagadro(cMatrix& res)
{	// Avagadro's number
	//double lf = datum::N_A;
	double lf = 6.02214129e23;
	res.SetTo(lf);
	return false;
}

bool get_goldenratio(cMatrix& res)
{	// golden ratio
	double lf = datum::gratio;
	res.SetTo(lf);
	return false;
}

bool get_eps(cMatrix& res)
{	// the difference between 1 and the least value greater than 1 that is representable
	double lf = datum::eps;
	res.SetTo(lf);
	return false;
}

bool get_atomicmass(cMatrix& res)
{	// atomic mass constant (in kg)
	//double lf = datum::m_u;
	double lf = 1.660538921e-27;
	res.SetTo(lf);
	return false;
}

bool get_Z0(cMatrix& res)
{	// intrinsic impedance of vacuum (ohms)
	//double lf = datum::Z_0;
	double lf = 376.730313461;
	res.SetTo(lf);
	return false;
}

bool get_G0(cMatrix& res)
{	// conductance quantum (in siemens)
	//double lf = datum::G_0;
	double lf = 7.7480917346e-5;
	res.SetTo(lf);
	return false;
}

bool get_G0i(cMatrix& res)
{	// conductance quantum inverse (in ohmes)
	//double lf = datum::G_0;
	double lf = 12906.4037217;
	res.SetTo(lf);
	return false;
}

//bool get_KE(cMatrix& res)
//{	// Coulomb's constant (in meters per farad)
//	double lf = datum::k_e;
//	res.SetTo(lf);
//	return false;
//}

bool get_eps0(cMatrix& res)
{	// electric constant (in farads per meter)
	//double lf = datum::eps_0;
	double lf = 8.854187817e-12;
	res.SetTo(lf);
	return false;
}

bool get_me(cMatrix& res)
{	// electron mass
	//double lf = datum::m_e;
	double lf = 9.10938291e-31;
	res.SetTo(lf);
	return false;
}

//bool get_J2eV(cMatrix& res)
//{	// electron volt (in joules)
//	double lf = datum::eV;
//	res.SetTo(lf);
//}

bool get_ec(cMatrix& res)
{	// elementary charge (in coulombs)
	//double lf = datum::ec;
	double lf = 1.602176565e-19;
	res.SetTo(lf);
	return false;
}

bool get_ev(cMatrix& res)
{	// electron volt (in joules)
	//double lf = datum::eV;
	double lf = 1.602176565e-19;
	res.SetTo(lf);
	return false;
}

bool get_F(cMatrix& res)
{	// Faraday constant (in coulombs)
	//double lf = datum::F;
	double lf = 96485.3365;
	res.SetTo(lf);
	return false;
}

bool get_alpha(cMatrix& res)
{	// fine-structure constant
	//double lf = datum::alpha;
	double lf = 7.2973525698e-3;
	res.SetTo(lf);
	return false;
}

bool get_alpha_inv(cMatrix& res)
{	// inverse fine-structure constant
	//double lf = datum::alpha_inv;
	double lf = 137.035999074;
	res.SetTo(lf);
	return false;
}

bool get_kj(cMatrix& res)
{	// Josephson constant
	//double lf = datum::K_J;
	double lf = 483597.870e9;
	res.SetTo(lf);
	return false;
}

bool get_mu_0(cMatrix& res)
{	// magnetic constant (in henries per meter)
	//double lf = datum::mu_0;
	double lf = 12.566370614e-7;
	res.SetTo(lf);
	return false;
}

bool get_phi_0(cMatrix& res)
{	// magnetic flux quantum (in webers)
	//double lf = datum::phi_0;
	double lf = 2.067833758e-15;
	res.SetTo(lf);
	return false;
}

bool get_R(cMatrix& res)
{	// molar gas constant (in joules per mole kelvin)
	//double lf = datum::R;
	double lf = 8.3144621;
	res.SetTo(lf);
	return false;
}

bool get_G(cMatrix& res)
{	// Newtonian constant of gravitation (in newton square meters per kilogram squared
	//double lf = datum::G;
	double lf = 6.67384e-11;
	res.SetTo(lf);
	return false;
}

bool get_h(cMatrix& res)
{	// Planck constant (in joule seconds)
	//double lf = datum::h;
	double lf = 6.62606957e-34;
	res.SetTo(lf);
	return false;
}

bool get_hbar(cMatrix& res)
{	// Planck constant over 2 pi, aka reduced Planck constant (in joule seconds)
	//double lf = datum::h_bar;
	double lf = 1.054571726e-34;
	res.SetTo(lf);
	return false;
}

bool get_mp(cMatrix& res)
{	// proton mass (in kg)
	//double lf = datum::m_p;
	double lf = 1.672621777e-27;
	res.SetTo(lf);
	return false;
}

bool get_Rinf(cMatrix& res)
{	// Rydberg constant (in reciprocal meters)
	//double lf = datum::R_inf;
	double lf = 10973731.568539;
	res.SetTo(lf);
	return false;
}

bool get_sigma(cMatrix& res)
{	// Stefan-Boltzmann constant
	//double lf = datum::sigma;
	double lf = 5.670373e-8;
	res.SetTo(lf);
	return false;
}

bool get_Rk(cMatrix& res)
{	// von Klitzing constant (in ohms)
	//double lf = datum::R_k;
	double lf = 25812.8074434;
	res.SetTo(lf);
	return false;
}

bool get_wien_b(cMatrix& res)
{	// Wien wavelength displacement law constant
	//double lf = datum::b;
	double lf = 2.8977721e-3;
	res.SetTo(lf);
	return false;
}

