
#include "variable.h"


bool Length(cVariable& Result, cVariable& v)				{ return true; }
bool Size(cVariable& Result, cVariable& v)					{ return true; }
bool Rows(cVariable& Result, cVariable& v)					{ return true; }
bool Columns(cVariable& Result, cVariable& v)				{ return true; }

bool sin(cVariable&,cVariable&)								{ return true; }
bool cos(cVariable&,cVariable&)								{ return true; }
bool tan(cVariable&,cVariable&)								{ return true; }
bool asin(cVariable& Cout,cVariable& R)						{ return true; }
bool acos(cVariable& Cout,cVariable& R)						{ return true; }
bool atan(cVariable& Rout,cVariable& R)						{ return true; }
bool csc(cVariable& Rout,cVariable& R)						{ return true; }
bool sec(cVariable& Rout,cVariable& R)						{ return true; }
bool cot(cVariable& Rout,cVariable& R)						{ return true; }
bool acsc(cVariable& Cout,cVariable& R)						{ return true; }
bool asec(cVariable& Cout,cVariable& R)						{ return true; }
bool acot(cVariable& Rout,cVariable& R)						{ return true; }
bool Atan2(cVariable& Aout, cVariable& A1, cVariable& A2)	{ return true; }

// hyperbolic trig
bool sinh(cVariable& Rout, cVariable& R)				{ return true; }
bool cosh(cVariable& Rout, cVariable& R)				{ return true; }
bool tanh(cVariable& Rout, cVariable& R)				{ return true; }
bool asinh(cVariable& Rout, cVariable& R)				{ return true; }
bool acosh(cVariable& Aout, cVariable& R)				{ return true; }
bool atanh(cVariable& Aout, cVariable& R)				{ return true; }
bool csch(cVariable& Rout, cVariable& R)				{ return true; }
bool sech(cVariable& Rout, cVariable& R)				{ return true; }
bool coth(cVariable& Rout, cVariable& R)				{ return true; }
bool acsch(cVariable& Rout, cVariable& R)				{ return true; }
bool asech(cVariable& Cout, cVariable& R)				{ return true; }
bool acoth(cVariable& Cout, cVariable& R)				{ return true; }

//// logarithms
bool log(cVariable& Cout, cVariable& v)				{ return true; }
bool log10(cVariable& Cout, cVariable& v)			{ return true; }
bool log2(cVariable& Cout, cVariable& v)			{ return true; }
bool sqrt(cVariable& Cout, cVariable& v)			{ return true; }
bool exp(cVariable& Cout, cVariable& v)				{ return true; }

//// special functions
bool erf(cVariable& Cout, cVariable& v)										{ return true; }
bool erfc(cVariable& Cout, cVariable& v)									{ return true; }
bool besseljn(cVariable& Cout, cVariable& norder, cVariable& v)				{ return true; }
bool besselyn(cVariable& Cout, cVariable& norder, cVariable& v)				{ return true; }
bool lgamma(cVariable& Cout, cVariable& v)									{ return true; }
bool gamma(cVariable& Cout, cVariable& v)									{ return true; }

//// misc
bool Real(cVariable& Result, cVariable& A)			{ return true; }
bool Imag(cVariable& Result, cVariable& A)			{ return true; }
bool Conj(cVariable& Result, cVariable& A)			{ return true; }
bool Trans(cVariable& Result, cVariable& A)			{ return true; }
bool Herm(cVariable& Result, cVariable& A)			{ return true; }
bool Fix(cVariable& Cout, cVariable& v)				{ return true; }
bool Floor(cVariable& Cout, cVariable& v)			{ return true; }
bool Ceil(cVariable& Cout, cVariable& v)			{ return true; }
bool Phase(cVariable& Cout, cVariable& v)			{ return true; }
bool Flipud(cVariable& Cout, cVariable& v)			{ return true; }
bool Fliplr(cVariable& Cout, cVariable& v)			{ return true; }

bool Not(cVariable& Result, cVariable& A)			{ return true; }

bool VertCat(cVariable& Result, cVariable& v1, cVariable& v2)			{ return true; }
bool HorzCat(cVariable& Result, cVariable& v1, cVariable& v2)			{ return true; }

bool NewMat(cVariable& Cout, cVariable& vn)									{ return true; }
bool NewMat(cVariable& Cout, cVariable& vr, cVariable& vc)					{ return true; }
bool NewMat(cVariable& Cout, cVariable& v,  cVariable& vr, cVariable& vc)	{ return true; }
bool NewDiag(cVariable& Cout, cVariable& vn)								{ return true; }
bool NewDiag(cVariable& Cout, cVariable& vr, cVariable& vc)					{ return true; }
bool NewDiag(cVariable& Cout, cVariable& v, cVariable& vr, cVariable& vc)	{ return true; }
bool NewIdentity(cVariable& Cout, cVariable& vn)							{ return true; }
bool NewIdentity(cVariable& Cout, cVariable& vr, cVariable& vc)				{ return true; }

bool Repeat(cVariable& res, cVariable& v, cVariable& vr, cVariable& vc)	{ return true; }

bool Min(cVariable& Cout, cVariable& v)									{ return true; }
bool Min(cVariable& Cout, cVariable& v1, cVariable& v2)					{ return true; }
bool MinIndex(cVariable& Result, cVariable& ResIdx, cVariable& v)		{ return true; }
bool Max(cVariable& Cout, cVariable& v)									{ return true; }
bool Max(cVariable& Cout, cVariable& v1, cVariable& v2)					{ return true; }
bool MaxIndex(cVariable& Result, cVariable& ResIdx, cVariable& v)		{ return true; }

bool Abs(cVariable& Cout, cVariable& v)									{ return true; }
bool Factorial(cVariable& Cout, cVariable& v)							{ return true; }

bool Sum(cVariable& Cout, cVariable& v)									{ return true; }
bool CumSum(cVariable& Cout, cVariable& v)								{ return true; }
bool CumSum(cVariable& Cout, cVariable& v, cVariable& dim)				{ return true; }
bool Prod(cVariable& Cout, cVariable& v)								{ return true; }
bool Prod(cVariable& Cout, cVariable& v, cVariable& dim)				{ return true; }
bool CumProd(cVariable& Cout, cVariable& v)								{ return true; }
bool CumProd(cVariable& Cout, cVariable& v, cVariable& dim)				{ return true; }
bool Matrix2Norm(cVariable& Cout, cVariable& v)							{ return true; }
bool MatrixPNorm(cVariable& Result, cVariable& v, cVariable& vP)		{ return true; }

bool Find(cVariable& Result, cVariable& A)						{ return true; }
bool Resize(cVariable& Result, cVariable& v, cVariable& vRows, cVariable& vCols)	{ return true; }
bool Rotate(cVariable& Result, cVariable& v, cVariable& vRot)	{ return true; }
bool RotateCw(cVariable& Result, cVariable& v, cVariable& vRot)	{ return true; }

bool Rand(cVariable& Result, cVariable& vRows, cVariable& vCols)		{ return true; }
bool Rand(cVariable& Result, cVariable& vDim)							{ return true; }
bool Randn(cVariable& Result, cVariable& vRows, cVariable& vCols)		{ return true; }
bool Randn(cVariable& Result, cVariable& vDim)							{ return true; }

bool Sort(cVariable& Result, cVariable& v)								{ return true; }
bool SortIndex(cVariable& Result, cVariable& ResIndex, cVariable& v)	{ return true; }
bool Unique(cVariable& Result, cVariable& v)							{ return true; }

bool Cov(cVariable& Result, cVariable& v1, cVariable& v2)				{ return true; }
bool Cov(cVariable& Result, cVariable& v)								{ return true; }
bool Median(cVariable& Result, cVariable& v)							{ return true; }
bool Median(cVariable& Result, cVariable& v, cVariable& idim)			{ return true; }
bool Mean(cVariable& Result, cVariable& v)								{ return true; }
bool Mean(cVariable& Result, cVariable& v, cVariable& idim)				{ return true; }
bool Var(cVariable& Result, cVariable& v, cVariable& iDim)				{ return true; }
bool Var(cVariable& Result, cVariable& v)								{ return true; }
bool Std(cVariable& Result, cVariable& v)								{ return true; }
bool Std(cVariable& Result, cVariable& v, cVariable& idim)				{ return true; }
bool Hist(cVariable& Result, cVariable& v)								{ return true; }
bool Hist_2o(cVariable& Result, cVariable& vCenters, cVariable& v)		{ return true; }
bool Hist(cVariable& Result, cVariable& v, cVariable& centers)			{ return true; }
bool Histn(cVariable& Result, cVariable& v, cVariable& Ncenters)		{ return true; }
bool Histn_2o(cVariable& Result, cVariable& vCenters, cVariable& v, cVariable& Ncenters)		{ return true; }
bool Histe(cVariable& Result, cVariable& v, cVariable& edges)			{ return true; }

bool Fft(cVariable& Result, cVariable& v)		{ return true; }
bool Ifft(cVariable& Result, cVariable& v)		{ return true; }
bool Fftz(cVariable& Result, cVariable& v)		{ return true; }
bool Ifftz(cVariable& Result, cVariable& v)		{ return true; }
bool Dft(cVariable& Result, cVariable& v)		{ return true; }
bool Idft(cVariable& Result, cVariable& v)		{ return true; }
bool Fft2(cVariable& Result, cVariable& v)		{ return true; }
bool Ifft2(cVariable& Result, cVariable& v)		{ return true; }
bool Dft2(cVariable& Result, cVariable& v)		{ return true; }
bool Idft2(cVariable& Result, cVariable& v)		{ return true; }
bool Shfft(cVariable& Result, cVariable& v)		{ return true; }

bool MatrixInvert(cVariable& Result, cVariable& v)					{ return true; }
bool LUDecomposition(cVariable& LU, cVariable& v)					{ return true; }
bool LUDecomposition(cVariable& L, cVariable& U, cVariable& v)		{ return true; }

bool arma_qr(cVariable& Q, cVariable& R, cVariable& X)					{ return true; }
bool arma_lu(cVariable& L, cVariable& U, cVariable& X)					{ return true; }
bool arma_chol(cVariable& res, cVariable& X)							{ return true; }
//
bool arma_eig(cVariable& res, cVariable& X)								{ return true; }
bool arma_eig(cVariable& val, cVariable& vec, cVariable& X)				{ return true; }
bool arma_eigr(cVariable& res, cVariable& X)							{ return true; }
bool arma_eigr(cVariable& val, cVariable& vec, cVariable& X)			{ return true; }
bool arma_svd(cVariable& U, cVariable& S, cVariable& V, cVariable& X)	{ return true; }
bool arma_svd(cVariable& U, cVariable& S, cVariable& X)					{ return true; }
bool arma_svd(cVariable& S, cVariable& X)								{ return true; }
bool arma_rank(cVariable& res, cVariable& X)							{ return true; }
bool arma_solve(cVariable& X, cVariable& A, cVariable& B)				{ return true; }
bool arma_syl(cVariable& res, cVariable& A, cVariable& B, cVariable& C)	{ return true; }
//
bool arma_inv(cVariable& res, cVariable& X)								{ return true; }
bool arma_pinv(cVariable& res, cVariable& X)							{ return true; }
bool arma_det(cVariable& res, cVariable& X)								{ return true; }
bool arma_logdet(cVariable& res, cVariable& X)							{ return true; }
//
bool arma_trace(cVariable& res, cVariable& X)							{ return true; }
bool arma_conv(cVariable& res, cVariable& X, cVariable& Y)				{ return true; }
bool Conv2(cVariable& res, cVariable& X, cVariable& Y)					{ return true; }
//
bool arma_shuffle_rows(cVariable& res, cVariable& v)					{ return true; }
bool arma_shuffle_cols(cVariable& res, cVariable& v)					{ return true; }

bool is_empty(cVariable& res, cVariable& v)						{ return true; }
bool is_real(cVariable& res, cVariable& v)						{ return true; }
bool is_vector(cVariable& res, cVariable& v)					{ return true; }
bool is_scalar(cVariable& res, cVariable& v)					{ return true; }
bool is_square(cVariable& res, cVariable& v)					{ return true; }
bool is_finite(cVariable& res, cVariable& v)					{ return true; }
bool is_key(cVariable& res, cVariable& H, cVariable& key)		{ return true; }
bool is_valid_field(cVariable& res, cVariable& key)				{ return true; }

bool Append(cVariable& Result, cVariable& v1, cVariable& v2)					{ return true; }
bool Append(cVariable& Result, cVariable& v1, cVariable& v2, cVariable& v3)		{ return true; }
bool Append(cVariable& Result, vector<cVariable>& vars)							{ return true; }
bool FindMatch(cVariable& res, cVariable& in, cVariable& pat)					{ return true; }
bool FindNext(cVariable& res, cVariable& in, cVariable& pat, cVariable& beg)	{ return true; }
bool FindPrev(cVariable& res, cVariable& in, cVariable& pat, cVariable& beg)	{ return true; }
bool Replace(cVariable& res, cVariable& in, cVariable& pat, cVariable& rpat)	{ return true; }
bool Substr(cVariable& res, cVariable& in, cVariable& beg, cVariable& len)		{ return true; }
bool Substr(cVariable& res, cVariable& in, cVariable& beg)						{ return true; }
bool Insert(cVariable& res, cVariable& in, cVariable& arg, cVariable& pos)		{ return true; }
bool ToLower(cVariable& res, cVariable& in)										{ return true; }
bool ToUpper(cVariable& res, cVariable& in)										{ return true; }
bool GetVersion(cVariable& res)													{ return true; }

bool HashGet(cVariable& Result, cVariable& hash, cVariable& key)			{ return true; }
bool HashSet(cVariable& hash, cVariable& key)								{ return true; }

bool DeleteKey(cVariable& hash, cVariable& in, cVariable& key)					{ return true; }
bool AddKey(cVariable& hash, cVariable& in, cVariable& key, cVariable& value)	{ return true; }

bool GetLine(cVariable& Result, cVariable& vid)								{ return true; }

bool Sprintf(cVariable& Result, cVariable& vformat)							{ return true; }
bool Sprintf(cVariable& Result, cVariable& vformat, cVariable& varg)		{ return true; }
bool Sprintf(cVariable& Result, cVariable& vformat, cVariable& varg1, cVariable& varg2)		{ return true; }
bool Sprintf(cVariable& Result, vector<cVariable>& vargs)					{ return true; }

bool Printf(cVariable& vformat)												{ return true; }
bool Printf(cVariable& vformat, cVariable& varg)							{ return true; }
bool Printf(cVariable& vformat, cVariable& varg1, cVariable& varg2)			{ return true; }
//bool Printf(vector<cVariable>& vargs)					{ return true; }

// file I/O
bool WriteData(cVariable& v, FILE *pf)										{ return true; }

bool FOpen(cVariable& Result, cVariable& vfile, cVariable& varg)			{ return true; }
bool FClose(cVariable& vid)													{ return true; }

bool FRead(cVariable& Result, cVariable& vid, cVariable& vsize, cVariable& vprecision)	{ return true; }
bool FWrite(cVariable& vid, cVariable& Mat, cVariable& vcount, cVariable& vprecision)	{ return true; }

bool SaveAll(cVariable& vfile)												{ return true; }
bool SaveSymbols(cVariable& Result, cVariable& fname, cVariable& varg)		{ return true; }
bool SaveSymbols(cVariable& Result, cVariable& fname, cVariable& varg1, cVariable& varg2)		{ return true; }
bool SaveSymbols(cVariable& Result, vector<cVariable>& vargs)				{ return true; }

bool Load(cVariable& vfile)													{ return true; }

bool LoadAscii(cVariable& Result, cVariable& vfile)							{ return true; }
bool SaveAscii(cVariable& vfile, vector<cVariable>& arg)					{ return true; }

bool FScanf(cVariable& Result, cVariable& vid, cVariable& vformat)			{ return true; }
bool FScanf(cVariable& Result_1, cVariable& Result_2, cVariable& vid, cVariable& vformat)		{ return true; }
bool FScanf(cVariable& Result_1, cVariable& Result_2, cVariable& Result_3, cVariable& vid, cVariable& vformat)		{ return true; }

bool FPrintf(cVariable& Result, cVariable& vid, cVariable& vformat)			{ return true; }
bool FPrintf(cVariable& Result, cVariable& vid, cVariable& vformat, cVariable& varg)		{ return true; }
bool FPrintf(cVariable& Result, vector<cVariable>& vargs)					{ return true; }

bool LoadImage(cVariable& Result, cVariable& vfile)						{ return true; }
bool SaveImage(cVariable& image, cVariable& vfile, cVariable& fmt)		{ return true; }

bool Show(cVariable& var)						{ return true; }

bool Edit(cVariable& file)						{ return true; }
bool CallHelp(cVariable& topic)					{ return true; }
bool NewPath(cVariable& dir)					{ return true; }
bool DelPath(cVariable& dir)					{ return true; }
bool Cd(cVariable& vdir)						{ return true; }
bool Pwd(cVariable& Result)						{ return true; }
bool Delete(cVariable& var)						{ return true; }
bool Delete(cVariable& var1, cVariable& var2)		{ return true; }
bool Delete(cVariable& var1, cVariable& var2, cVariable& var3)		{ return true; }
bool Delete(vector<cVariable>& vars)			{ return true; }
bool Help(cVariable& arg)						{ return true; }

// single arg access (rvalue)
bool valAccess(cVariable& v, double dvar, double darg)							{ return true; }
bool valAccess(cVariable& v, cVariable& var, double darg)						{ return true; }
bool valAccess(cVariable& v, double dvar, cVariable& arg)						{ return true; }
bool valAccess(cVariable& v, cVariable& var, cVariable& arg)					{ return true; }
// two arg access (rvalue)
bool valAccess(cVariable& v, double dvar, double darg1, double darg2)			{ return true; }
bool valAccess(cVariable& v, double dvar, cVariable& arg1, double darg2)		{ return true; }
bool valAccess(cVariable& v, double dvar, double darg1, cVariable& arg2)		{ return true; }
bool valAccess(cVariable& v, double dvar, cVariable& arg1, cVariable& arg2)		{ return true; }
bool valAccess(cVariable& v, cVariable& var, double darg1, double darg2)		{ return true; }
bool valAccess(cVariable& v, cVariable& var, double darg1, cVariable& arg2)		{ return true; }
bool valAccess(cVariable& v, cVariable& var, cVariable& arg1, double darg2)		{ return true; }
bool valAccess(cVariable& v, cVariable& var, cVariable& arg1, cVariable& arg2)	{ return true; }


// plot support
bool plot(cVariable& y)									{ return true; }
bool plot(cVariable& x, cVariable& y)					{ return true; }
bool bar(cVariable& v)									{ return true; }
bool bar(cVariable& v1, cVariable& v2)					{ return true; }
bool spike(cVariable& v)								{ return true; }
bool spike(cVariable& v1, cVariable& v2)				{ return true; }
bool plot_scatter(cVariable& v)							{ return true; }
bool plot_scatter(cVariable& x, cVariable& y)			{ return true; }
bool plot_scatter(cVariable& x, cVariable& y, cVariable& z)		{ return true; }
bool plot_fill(cVariable& v)							{ return true; }
bool plot_fill(cVariable& v1, cVariable& v2)			{ return true; }
bool polar(cVariable& v)								{ return true; }
bool polar(cVariable& v1, cVariable& v2)				{ return true; }
bool plot_triangles(cVariable& v)						{ return true; }
bool plot_triangles(cVariable& x, cVariable& y)			{ return true; }
bool plot_triangles(cVariable& x, cVariable& y, cVariable& z) { return true; }
bool plot3(cVariable& x, cVariable& y, cVariable& z)	{ return true; }
bool surf(cVariable& z)									{ return true; }
bool surf(cVariable& x, cVariable& y, cVariable& z)		{ return true; }
bool hmap(cVariable& z)									{ return true; }
bool hmap(cVariable& x, cVariable& y, cVariable& z)		{ return true; }
bool contour(cVariable& z)								{ return true; }
bool contour(cVariable& x, cVariable& y, cVariable& z)	{ return true; }
bool contour3(cVariable& z)								{ return true; }
bool contour3(cVariable& x, cVariable& y, cVariable& z)	{ return true; }
bool surf_contour(cVariable& z)							{ return true; }
bool surf_contour(cVariable& x, cVariable& y, cVariable& z)	{ return true; }
bool plot_image(cVariable& z)							{ return true; }

bool plot_log_y(cVariable& y)							{ return true; }
bool plot_log_x(cVariable& x, cVariable& y)				{ return true; }
bool plot_log_y(cVariable& x, cVariable& y)				{ return true; }
bool plot_log_xy(cVariable& x, cVariable& y)			{ return true; }

bool add_plot(cVariable& y)						{ return true; }
bool add_plot(cVariable& x, cVariable& y)		{ return true; }
bool add_bar(cVariable& v)						{ return true; }
bool add_bar(cVariable& v1, cVariable& v2)		{ return true; }
bool add_triangles(cVariable& v)				{ return true; }
bool add_triangles(cVariable& x, cVariable& y)	{ return true; }
bool add_triangles(cVariable& x, cVariable& y, cVariable& z){ return true; }
bool add_plot3(cVariable& x, cVariable& y, cVariable& z)	{ return true; }
bool add_surf(cVariable& z)									{ return true; }
bool add_surf(cVariable& x, cVariable& y, cVariable& z)		{ return true; }
bool add_contour(cVariable& z)								{ return true; }
bool add_contour(cVariable& x, cVariable& y, cVariable& z)	{ return true; }

bool NextKey(cVariable& next, cVariable& hash, cVariable& start) { return true; }

// utilities
bool Figure(cVariable& v)				{ return true; }
bool CurrentFigure(cVariable& v)		{ return true; }
bool Which(cVariable& v)				{ return true; }

