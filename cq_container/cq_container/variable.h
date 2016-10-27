
#pragma once

#include <string>
#include <vector>
#include <math.h>
#include <complex>
#include "my_errors.h"
//#include "lexTypes.h"

using std::vector;
using std::string;
using std::complex;

enum { VAR_TYPE_NUMERIC = 0, VAR_TYPE_CUBE, VAR_TYPE_STRING, VAR_TYPE_HASH, VAR_TYPE_FCN_PTR, VAR_TYPE_CONTAINER };


// abstract container
//

class cVariable
{
public:
	cVariable() { };
	virtual ~cVariable() { };

	virtual bool operator=( const cVariable* pA ) { addError("= not defined!"); return true; }
	virtual bool operator=( vector<double>& ) { addError("= not defined!"); return true; }
	virtual bool operator=( double& ) { addError("= not defined!"); return true; }
	virtual bool operator=( complex<double>& ) { addError("= not defined!"); return true; }
	virtual bool operator=( const cVariable& ) { addError("= not defined!"); return true; }

	// creation, deletion
	virtual bool Delete(void) { addError("Delete not defined!"); return false; }
	virtual void Empty(void) { addError("Empty not defined!"); return; }

	virtual bool SetTo(double) { addError("SetTo not defined!"); return true; }
	virtual bool SetTo(complex<double>) { addError("SetTo not defined!"); return true; }
	virtual bool SetTo(cVariable&) { addError("SetTo not defined!"); return true; }

	virtual int GetType(void) { return -1; }
	virtual int GetSize(void) { addError("No size for virtual class!"); return -1; }
	string GetName(void) const { return m_name; }

	virtual string GetString(void) { string res; addError("No GetString for virtual class!"); return res; }
	string getName() { return m_name; }

public:
	int m_nType;
	string m_errorMsg;
	string m_name;
};

//#include "lexTypes.h"

//
// interface functions
//

bool Length(cVariable& Result, cVariable& v);
bool Size(cVariable& Result, cVariable& v);
bool Rows(cVariable& Result, cVariable& v);
bool Columns(cVariable& Result, cVariable& v);

bool sin(cVariable&,cVariable&);
bool cos(cVariable&,cVariable&);
bool tan(cVariable&,cVariable&);
bool asin(cVariable& Cout,cVariable& R);
bool acos(cVariable& Cout,cVariable& R);
bool atan(cVariable& Rout,cVariable& R);
bool csc(cVariable& Rout,cVariable& R);
bool sec(cVariable& Rout,cVariable& R);
bool cot(cVariable& Rout,cVariable& R);
bool acsc(cVariable& Cout,cVariable& R);
bool asec(cVariable& Cout,cVariable& R);
bool acot(cVariable& Rout,cVariable& R);
bool Atan2(cVariable& Aout, cVariable& A1, cVariable& A2);

// hyperbolic trig
bool sinh(cVariable& Rout, cVariable& R);
bool cosh(cVariable& Rout, cVariable& R);
bool tanh(cVariable& Rout, cVariable& R);
bool asinh(cVariable& Rout, cVariable& R);
bool acosh(cVariable& Aout, cVariable& R);
bool atanh(cVariable& Aout, cVariable& R);
bool csch(cVariable& Rout, cVariable& R);
bool sech(cVariable& Rout, cVariable& R);
bool coth(cVariable& Rout, cVariable& R);
bool acsch(cVariable& Rout, cVariable& R);
bool asech(cVariable& Cout, cVariable& R);
bool acoth(cVariable& Cout, cVariable& R);

//// logarithms
bool log(cVariable& Cout, cVariable& v);
bool log10(cVariable& Cout, cVariable& v);
bool log2(cVariable& Cout, cVariable& v);
bool sqrt(cVariable& Cout, cVariable& v);
bool exp(cVariable& Cout, cVariable& v);

//// special functions
bool erf(cVariable& Cout, cVariable& v);
bool erfc(cVariable& Cout, cVariable& v);
bool besseljn(cVariable& Cout, cVariable& norder, cVariable& v);
bool besselyn(cVariable& Cout, cVariable& norder, cVariable& v);
bool lgamma(cVariable& Cout, cVariable& v);
bool gamma(cVariable& Cout, cVariable& v);

//// misc
bool Real(cVariable& Result, cVariable& A);
bool Imag(cVariable& Result, cVariable& A);
bool Conj(cVariable& Result, cVariable& A);
bool Trans(cVariable& Result, cVariable& A);
bool Herm(cVariable& Result, cVariable& A);
bool Fix(cVariable& Cout, cVariable& v);
bool Floor(cVariable& Cout, cVariable& v);
bool Ceil(cVariable& Cout, cVariable& v);
bool Phase(cVariable& Cout, cVariable& v);
bool Flipud(cVariable& Cout, cVariable& v);
bool Fliplr(cVariable& Cout, cVariable& v);

bool Not(cVariable& Result, cVariable& A);

bool VertCat(cVariable& Result, cVariable& v1, cVariable& v2);
bool HorzCat(cVariable& Result, cVariable& v1, cVariable& v2);

bool NewMat(cVariable& Cout, cVariable& vn);
bool NewMat(cVariable& Cout, cVariable& vr, cVariable& vc);
bool NewMat(cVariable& Cout, cVariable& v,  cVariable& vr, cVariable& vc);
bool NewDiag(cVariable& Cout, cVariable& vn);
bool NewDiag(cVariable& Cout, cVariable& vr, cVariable& vc);
bool NewDiag(cVariable& Cout, cVariable& v, cVariable& vr, cVariable& vc);
bool NewIdentity(cVariable& Cout, cVariable& vn);
bool NewIdentity(cVariable& Cout, cVariable& vr, cVariable& vc);

bool Repeat(cVariable& res, cVariable& v, cVariable& vr, cVariable& vc);

bool Min(cVariable& Cout, cVariable& v);
bool Min(cVariable& Cout, cVariable& v1, cVariable& v2);
bool MinIndex(cVariable& Result, cVariable& ResIdx, cVariable& v);
bool Max(cVariable& Cout, cVariable& v);
bool Max(cVariable& Cout, cVariable& v1, cVariable& v2);
bool MaxIndex(cVariable& Result, cVariable& ResIdx, cVariable& v);

bool Abs(cVariable& Cout, cVariable& v);
bool Factorial(cVariable& Cout, cVariable& v);

bool Sum(cVariable& Cout, cVariable& v);
bool CumSum(cVariable& Cout, cVariable& v);
bool CumSum(cVariable& Cout, cVariable& v, cVariable& dim);
bool Prod(cVariable& Cout, cVariable& v);
bool Prod(cVariable& Cout, cVariable& v, cVariable& dim);
bool CumProd(cVariable& Cout, cVariable& v);
bool CumProd(cVariable& Cout, cVariable& v, cVariable& dim);
bool Matrix2Norm(cVariable& Cout, cVariable& v);
bool MatrixPNorm(cVariable& Result, cVariable& v, cVariable& vP);

bool Find(cVariable& Result, cVariable& A);
bool Resize(cVariable& Result, cVariable& v, cVariable& vRows, cVariable& vCols);
bool Rotate(cVariable& Result, cVariable& v, cVariable& vRot);
bool RotateCw(cVariable& Result, cVariable& v, cVariable& vRot);

bool Rand(cVariable& Result, cVariable& vRows, cVariable& vCols);
bool Rand(cVariable& Result, cVariable& vDim);
bool Randn(cVariable& Result, cVariable& vRows, cVariable& vCols);
bool Randn(cVariable& Result, cVariable& vDim);

bool Sort(cVariable& Result, cVariable& v);
bool SortIndex(cVariable& Result, cVariable& ResIndex, cVariable& v);
bool Unique(cVariable& Result, cVariable& v);

bool Cov(cVariable& Result, cVariable& v1, cVariable& v2);
bool Cov(cVariable& Result, cVariable& v);
bool Median(cVariable& Result, cVariable& v);
bool Median(cVariable& Result, cVariable& v, cVariable& idim);
bool Mean(cVariable& Result, cVariable& v);
bool Mean(cVariable& Result, cVariable& v, cVariable& idim);
bool Var(cVariable& Result, cVariable& v, cVariable& iDim);
bool Var(cVariable& Result, cVariable& v);
bool Std(cVariable& Result, cVariable& v);
bool Std(cVariable& Result, cVariable& v, cVariable& idim);
bool Hist(cVariable& Result, cVariable& v);
bool Hist_2o(cVariable& Result, cVariable& vCenters, cVariable& v);
bool Hist(cVariable& Result, cVariable& v, cVariable& centers);
bool Histn(cVariable& Result, cVariable& v, cVariable& Ncenters);
bool Histn_2o(cVariable& Result, cVariable& vCenters, cVariable& v, cVariable& Ncenters);
bool Histe(cVariable& Result, cVariable& v, cVariable& edges);

bool Fft(cVariable& Result, cVariable& v);
bool Ifft(cVariable& Result, cVariable& v);
bool Fftz(cVariable& Result, cVariable& v);
bool Ifftz(cVariable& Result, cVariable& v);
bool Dft(cVariable& Result, cVariable& v);
bool Idft(cVariable& Result, cVariable& v);
bool Fft2(cVariable& Result, cVariable& v);
bool Ifft2(cVariable& Result, cVariable& v);
bool Dft2(cVariable& Result, cVariable& v);
bool Idft2(cVariable& Result, cVariable& v);
bool Shfft(cVariable& Result, cVariable& v);

bool MatrixInvert(cVariable& Result, cVariable& v);
bool LUDecomposition(cVariable& LU, cVariable& v);
bool LUDecomposition(cVariable& L, cVariable& U, cVariable& v);

bool arma_qr(cVariable& Q, cVariable& R, cVariable& X);
bool arma_lu(cVariable& L, cVariable& U, cVariable& X);
bool arma_chol(cVariable& res, cVariable& X);
//
bool arma_eig(cVariable& res, cVariable& X);
bool arma_eig(cVariable& val, cVariable& vec, cVariable& X);
bool arma_eigr(cVariable& res, cVariable& X);
bool arma_eigr(cVariable& val, cVariable& vec, cVariable& X);
bool arma_svd(cVariable& U, cVariable& S, cVariable& V, cVariable& X);
bool arma_svd(cVariable& U, cVariable& S, cVariable& X);
bool arma_svd(cVariable& S, cVariable& X);
bool arma_rank(cVariable& res, cVariable& X);
bool arma_solve(cVariable& X, cVariable& A, cVariable& B);
bool arma_syl(cVariable& res, cVariable& A, cVariable& B, cVariable& C);
//
bool arma_inv(cVariable& res, cVariable& X);
bool arma_pinv(cVariable& res, cVariable& X);
bool arma_det(cVariable& res, cVariable& X);
bool arma_logdet(cVariable& res, cVariable& X);
//
bool arma_trace(cVariable& res, cVariable& X);
bool arma_conv(cVariable& res, cVariable& X, cVariable& Y);
bool Conv2(cVariable& res, cVariable& X, cVariable& Y);
//
bool arma_shuffle_rows(cVariable& res, cVariable& v);
bool arma_shuffle_cols(cVariable& res, cVariable& v);

bool is_empty(cVariable& res, cVariable& v);
bool is_real(cVariable& res, cVariable& v);
bool is_vector(cVariable& res, cVariable& v);
bool is_scalar(cVariable& res, cVariable& v);
bool is_square(cVariable& res, cVariable& v);
bool is_finite(cVariable& res, cVariable& v);
bool is_key(cVariable& res, cVariable& H, cVariable& key);
bool is_valid_field(cVariable& res, cVariable& key);

// string
bool Append(cVariable& Result, cVariable& v1, cVariable& v2);
bool Append(cVariable& Result, cVariable& v1, cVariable& v2, cVariable& v3);
bool Append(cVariable& Result, vector<cVariable>& vars);
bool Length(cVariable& res, cVariable& in);
bool FindMatch(cVariable& res, cVariable& in, cVariable& pat);
bool FindNext(cVariable& res, cVariable& in, cVariable& pat, cVariable& beg);
bool FindPrev(cVariable& res, cVariable& in, cVariable& pat, cVariable& beg);
bool Replace(cVariable& res, cVariable& in, cVariable& pat, cVariable& rpat);
bool Substr(cVariable& res, cVariable& in, cVariable& beg, cVariable& len);
bool Substr(cVariable& res, cVariable& in, cVariable& beg);
bool Insert(cVariable& res, cVariable& in, cVariable& arg, cVariable& pos);
bool ToLower(cVariable& res, cVariable& in);
bool ToUpper(cVariable& res, cVariable& in);
//
bool GetVersion(cVariable& res);

// hash
bool HashGet(cVariable& Result, cVariable& hash, cVariable& key);
bool HashSet(cVariable& hash, cVariable& key);

bool DeleteKey(cVariable& hash, cVariable& in, cVariable& key);
bool AddKey(cVariable& hash, cVariable& in, cVariable& key, cVariable& value);

bool GetLine(cVariable& Result, cVariable& vid);

bool Sprintf(cVariable& Result, cVariable& vformat);
bool Sprintf(cVariable& Result, cVariable& vformat, cVariable& varg);
bool Sprintf(cVariable& Result, cVariable& vformat, cVariable& varg1, cVariable& varg2);
bool Sprintf(cVariable& Result, vector<cVariable>& vargs);

bool Printf(cVariable& vformat);
bool Printf(cVariable& vformat, cVariable& varg);
bool Printf(cVariable& vformat, cVariable& varg1, cVariable& varg2);
//bool Printf(vector<cVariable>& vargs);

// file I/O
bool WriteData(cVariable& v, FILE *pf);
bool ReadData(cVariable& v, FILE *pf);
bool FRead(cVariable& Result, cVariable& vid, cVariable& vsize, cVariable& vprecision);
bool FWrite(cVariable& vid, cVariable& Mat, cVariable& vcount, cVariable& vprecision);

bool FOpen(cVariable& Result, cVariable& vfile, cVariable& varg);
bool FClose(cVariable& vid);

bool Load(cVariable& vfile);
bool SaveAll(cVariable& vfile);
bool SaveSymbols(cVariable& Result, cVariable& fname, cVariable& varg);
bool SaveSymbols(cVariable& Result, cVariable& fname, cVariable& varg1, cVariable& varg2);
bool SaveSymbols(cVariable& Result, vector<cVariable>& vargs);

bool LoadAscii(cVariable& Result, cVariable& vfile);
bool SaveAscii(cVariable& vfile, vector<cVariable>& arg);

bool FScanf(cVariable& Result, cVariable& vid, cVariable& vformat);
bool FScanf(cVariable& Result_1, cVariable& Result_2, cVariable& vid, cVariable& vformat);
bool FScanf(cVariable& Result_1, cVariable& Result_2, cVariable& Result_3, cVariable& vid, cVariable& vformat);
//bool FScanf(vector<valueBlob>& Result, cVariable& vid, cVariable& vformat);

bool FPrintf(cVariable& Result, cVariable& vid, cVariable& vformat);
bool FPrintf(cVariable& Result, cVariable& vid, cVariable& vformat, cVariable& varg);
bool FPrintf(cVariable& Result, vector<cVariable>& vargs);

bool LoadImage(cVariable& Result, cVariable& vfile);
bool SaveImage(cVariable& image, cVariable& vfile, cVariable& fmt);
//
//bool FileStr(cVariable& Result, cVariable& fname)							{ return true; }

bool Pwd(cVariable& Result);
bool Show(cVariable& var);

bool Rundir();

bool Edit(cVariable& file);
bool CallHelp(cVariable& topic);
bool NewPath(cVariable& dir);
bool DelPath(cVariable& dir);
bool Cd(cVariable& vdir);
bool Pwd(cVariable& Result);
bool Delete(cVariable& var);
bool Delete(cVariable& var1, cVariable& var2);
bool Delete(cVariable& var1, cVariable& var2, cVariable& var3);
bool Delete(vector<cVariable>& vars);
bool Help(cVariable& arg);

// single arg access (rvalue)
bool valAccess(cVariable& v, double dvar, double darg);
bool valAccess(cVariable& v, cVariable& var, double darg);
bool valAccess(cVariable& v, double dvar, cVariable& arg);
bool valAccess(cVariable& v, cVariable& var, cVariable& arg);
// two arg access (rvalue)
bool valAccess(cVariable& v, double dvar, double darg1, double darg2);
bool valAccess(cVariable& v, double dvar, cVariable& arg1, double darg2);
bool valAccess(cVariable& v, double dvar, double darg1, cVariable& arg2);
bool valAccess(cVariable& v, double dvar, cVariable& arg1, cVariable& arg2);
bool valAccess(cVariable& v, cVariable& var, double darg1, double darg2);
bool valAccess(cVariable& v, cVariable& var, double darg1, cVariable& arg2);
bool valAccess(cVariable& v, cVariable& var, cVariable& arg1, double darg2);
bool valAccess(cVariable& v, cVariable& var, cVariable& arg1, cVariable& arg2);


// plot support
bool plot(cVariable& y);
bool plot(cVariable& x, cVariable& y);
bool bar(cVariable& v);
bool bar(cVariable& v1, cVariable& v2);
bool spike(cVariable& v);
bool spike(cVariable& v1, cVariable& v2);
bool plot_scatter(cVariable& v);
bool plot_scatter(cVariable& x, cVariable& y);
bool plot_scatter(cVariable& x, cVariable& y, cVariable& z);
bool plot_fill(cVariable& v);
bool plot_fill(cVariable& v1, cVariable& v2);
bool polar(cVariable& v);
bool polar(cVariable& v1, cVariable& v2);
bool plot_triangles(cVariable& v);
bool plot_triangles(cVariable& x, cVariable& y);
bool plot_triangles(cVariable& x, cVariable& y, cVariable& z);
bool plot3(cVariable& x, cVariable& y, cVariable& z);
bool surf(cVariable& z);
bool surf(cVariable& x, cVariable& y, cVariable& z);
bool hmap(cVariable& z);
bool hmap(cVariable& x, cVariable& y, cVariable& z);
bool contour(cVariable& z);
bool contour(cVariable& x, cVariable& y, cVariable& z);
bool contour3(cVariable& z);
bool contour3(cVariable& x, cVariable& y, cVariable& z);
bool surf_contour(cVariable& z);
bool surf_contour(cVariable& x, cVariable& y, cVariable& z);
bool plot_image(cVariable& z);

bool plot_log_y(cVariable& y);
bool plot_log_x(cVariable& x, cVariable& y);
bool plot_log_y(cVariable& x, cVariable& y);
bool plot_log_xy(cVariable& x, cVariable& y);

bool add_plot(cVariable& y);
bool add_plot(cVariable& x, cVariable& y);
bool add_bar(cVariable& v);
bool add_bar(cVariable& v1, cVariable& v2);
bool add_triangles(cVariable& v);
bool add_triangles(cVariable& x, cVariable& y);
bool add_triangles(cVariable& x, cVariable& y, cVariable& z);
bool add_plot3(cVariable& x, cVariable& y, cVariable& z);
bool add_surf(cVariable& z);
bool add_surf(cVariable& x, cVariable& y, cVariable& z);
bool add_contour(cVariable& z);
bool add_contour(cVariable& x, cVariable& y, cVariable& z);

bool set_xlabel(cVariable& fig, cVariable& label);
bool set_ylabel(cVariable& fig, cVariable& label);
bool set_zlabel(cVariable& fig, cVariable& label);
bool set_plot_title(cVariable& fig, cVariable& title);
bool set_plot_colormap(cVariable& fig, cVariable& name);
bool set_limits(cVariable& fig, cVariable& limits);

bool NextKey(cVariable& next, cVariable& hash, cVariable& start);

// utilities
bool Figure(cVariable& v);
bool CurrentFigure(cVariable& v);
bool Which(cVariable& v);

