
#include "varFile.h"
#include "state.h"
#include "defines.h"
#include "varUtil.h"
#include "container.h"
#include "matrix.h"
#include "c_string.h"
#include "state.h"
#include "targetver.h"

#ifndef _WINDOWS_
	#include <sys/types.h>
	#include <sys/stat.h>
	#include <dirent.h>
#else
	#include <float.h>
#endif

extern cState prog_state;
extern cStorage T;

bool GetVersion(cString& res)
{
	res.Delete();
	res.s = VERSION_STRING;

	return false;
}

//
// file support
//
bool WriteWSHeader(FILE *pf, int nVars)
{
	int nVersion = __WORKSPACE_FILE_VERSION;

	if (pf==NULL) return true;

	if ( fwrite( &nVersion, sizeof(int), 1, pf ) != 1 ||
		 fwrite( &nVars, sizeof(int), 1, pf ) != 1 )
	{
		return true;
	}

	return false;
}

bool ReadWSHeader(int& nVer, int& nVars, FILE *pf)
{
	if (pf==NULL) return true;

	if ( fread( &nVer, sizeof(int), 1, pf ) != 1 ||
		 fread( &nVars, sizeof(int), 1, pf ) != 1 )
	{
		return true;
	}

	return false;
}

bool SaveAll()
{
	string fname = __PROGRAM_NAME;
	fname.append(".cwk");

	cString Name;
	Name.s = fname;

	return SaveAll(Name);
}

bool SaveAll(cString& file)
{
	string fname = file.s;

	vector<string> *pSymbols = T.GetSymbolList();

	FILE *pf = fopen( fname.c_str(), "wb" );
	if (pf==NULL) { addError("unable to open file: ", fname); return true; }

	// file header
	if ( WriteWSHeader(pf, int(pSymbols->size())) ) { fclose(pf); return true; }

	if (SaveSymbols(pf, pSymbols)) { fclose(pf); return true; }

	fclose(pf);
	return false;
}

//bool SaveSymbols(cString& file, cString& arg)
//{
//	string fname = file.s;
//
//	string symbol = arg.s;
//
//	vector<string> Symbols;
//	Symbols.push_back( symbol );
//
//	FILE *pf = fopen( fname.c_str(), "wb" );
//	if (pf==NULL) { addError("unable to open file: ", fname); return true; }
//
//	// file header
//	if ( WriteWSHeader(pf, int(Symbols.size())) ) { fclose(pf); return true; }
//
//	if (SaveSymbols(pf, &Symbols)) { fclose(pf); return true; }
//
//	fclose(pf);
//	return false;
//}
//
//bool SaveSymbols(cString& file, cString& arg1, cString& arg2)
//{
//	string fname = file.s;
//
//	string symbol1 = arg1.s;
//	string symbol2 = arg2.s;
//
//	vector<string> Symbols;
//	Symbols.push_back( symbol1 );
//	Symbols.push_back( symbol2 );
//
//	FILE *pf = fopen( fname.c_str(), "wb" );
//	if (pf==NULL) { addError("unable to open file: ", fname); return true; }
//
//	// file header
//	if ( WriteWSHeader(pf, int(Symbols.size())) ) { fclose(pf); return true; }
//
//	if (SaveSymbols(pf, &Symbols)) { fclose(pf); return true; }
//
//	fclose(pf);
//	return false;
//}
//
//bool SaveSymbols(cString& file, cString& arg1, cString& arg2, cString& arg3)
//{
//	string fname = file.s;
//
//	string symbol1 = arg1.s;
//	string symbol2 = arg2.s;
//	string symbol3 = arg3.s;
//
//	vector<string> Symbols;
//	Symbols.push_back( symbol1 );
//	Symbols.push_back( symbol2 );
//	Symbols.push_back( symbol3 );
//
//	FILE *pf = fopen( fname.c_str(), "wb" );
//	if (pf==NULL) { addError("unable to open file: ", fname); return true; }
//
//	// file header
//	if ( WriteWSHeader(pf, int(Symbols.size())) ) { fclose(pf); return true; }
//
//	if (SaveSymbols(pf, &Symbols)) { fclose(pf); return true; }
//
//	fclose(pf);
//	return false;
//}

bool SaveSymbols(cString& file, cContainer& arg)
{
	string fname = file.s;
	vector<string> Symbols;

	Symbols.push_back( arg.GetName() );

	FILE *pf = fopen( fname.c_str(), "wb" );
	if (pf==NULL) { addError("unable to open file: ", fname); return true; }

	// file header
	if ( WriteWSHeader(pf, int(Symbols.size())) ) { fclose(pf); return true; }

	if (SaveSymbols(pf, &Symbols)) { fclose(pf); return true; }

	fclose(pf);
	return false;
}

bool SaveSymbols(cString& file, cContainer& arg1, cContainer& arg2)
{
	string fname = file.s;
	vector<string> Symbols;

	Symbols.push_back( arg1.GetName() );
	Symbols.push_back( arg2.GetName() );

	FILE *pf = fopen( fname.c_str(), "wb" );
	if (pf==NULL) { addError("unable to open file: ", fname); return true; }

	// file header
	if ( WriteWSHeader(pf, int(Symbols.size())) ) { fclose(pf); return true; }

	if (SaveSymbols(pf, &Symbols)) { fclose(pf); return true; }

	fclose(pf);
	return false;
}

bool SaveSymbols(vector<cContainer>& args)
{
	if (args.size() < 2) { addError("insufficient arguments"); return true; }

	if (args[0].GetType() != VAR_TYPE_STRING) { addError("illegal argument"); return true; }

	string fname = args[0].s.s;
	vector<string> Symbols;

	for (int i=1; i<(int)args.size(); ++i) {
		Symbols.push_back( args[i].GetName() );
	}

	FILE *pf = fopen( fname.c_str(), "wb" );
	if (pf==NULL) { addError("unable to open file: ", fname); return true; }

	// file header
	if ( WriteWSHeader(pf, int(Symbols.size())) ) { fclose(pf); return true; }

	if (SaveSymbols(pf, &Symbols)) { fclose(pf); return true; }

	fclose(pf);
	return false;
}

bool SaveSymbols(FILE *pf, vector<string> *pVNames)
{
	if (pVNames==NULL) { addError("SaveSymbols: internal error"); return true; }

	cContainer v;

	int n, nType;
	char chByte = '_';	// arbitrary separator (verification) byte

	for (int i=0; i<(int)pVNames->size(); ++i)
	{
		if ( T.Lookup(v, pVNames->at(i)) ) { addError("undefined symbol: ", pVNames->at(i)); return true; }

		// variable name
		n = pVNames->at(i).length();
		if (fwrite( &n, sizeof(int), 1, pf ) != 1) { addError("error writing to file"); return true; }
		if (fwrite( pVNames->at(i).c_str(), sizeof(char), n, pf ) != n) { addError("error writing to file"); return true; }

		// variable type
		nType = v.GetType();
		if (fwrite( &nType, sizeof(int), 1, pf ) != 1) { addError("error writing to file"); return true; }

		// write variable
		if (WriteData( v, pf )) { addError("error writing to file"); return true; }

		// serparator byte
		if (i < int(pVNames->size()-1))
			if (fwrite( &chByte, sizeof(char), 1, pf ) != 1) { addError("error writing to file"); return true; }
	}
	return false;
}

bool Load(cString& vfile)
{
	string fname;
	fname = vfile.GetString();

	vector<cContainer> vars;
	if ( Load(fname, vars) ) return true;

	return (T.Insert( vars ));
}

bool Load(string& fname, vector<cContainer>& vars)
{
	vars.clear();
	cContainer var;
	cMatrix mat;
	cString str;

	char *pchString = NULL;

	char chByte = '_';	// arbitrary separator (verification) byte
	char ch;

	FILE *pf = fopen( fname.c_str(), "rb" );
	if (pf==NULL) { addError("unable to open file: ", fname); return true; }			// file not found

	int nVer, nVars;
	int n, nType;
	int iErr = 0, i;

	if ( ReadWSHeader(nVer, nVars, pf) ) {
		addError("Bad header in file: ", fname);
		return true;
	}

	if ( nVer<0 || nVars<0 ) {
		addError("Bad header in file: ", fname);
		return true;
	}

	for (i=0; i<nVars; ++i)
	{
		// variable name
		if ( fread( &n, sizeof(int), 1, pf ) != 1 ) {
			addError("Format error in file: ", fname); fclose(pf); return true;
		}
		pchString = new char[n+1];
		if (pchString==NULL) { iErr = 1; break; }		// out of memory
		if ( fread( pchString, sizeof(char), n, pf ) != n ) {
			addError("Format error in file: ", fname); delete []pchString; fclose(pf); return true;
		}
		pchString[n] = '\0';
		var.SetName( pchString );
		delete []pchString; pchString = NULL;
		
		// variable type
		if ( fread( &nType, sizeof(int), 1, pf ) != 1 ) {
			addError("Format error in file: ", fname); fclose(pf); return true;
		}

		// read the variable data
		var.SetType( nType );
		if (ReadData( var, pf )) {
			addError("Format error in file: ", fname); fclose(pf); return true;
		}

		vars.push_back( var );

		// serparator byte
		if (i == nVars-1) break;

		if (fread( &ch, sizeof(char), 1, pf ) != 1 || ch != chByte) {
			addError("Format error in file: ", fname); fclose(pf); return true;
		}
	}

	fclose( pf );
	return false;
}

bool FOpen(cMatrix& Result, cString& vfile, cString& varg)
{
	if (vfile.GetType() != VAR_TYPE_STRING) { addError("illegal argument"); return true; }
	string fname = vfile.GetString();

	if (varg.GetType() != VAR_TYPE_STRING) { addError("illegal argument"); return true; }
	string format = varg.GetString();

	int fid = FOpen(fname,format);
	if (fid == -1) { addError("unable to open file: ", fname); return true; }

	if (Result.Create(1,1)) return true;
	Result.mr[0] = double(fid);

	return false;
}

bool FClose(cMatrix& vid)
{
	if (!vid.IsScalar()) { addError("illegal argument"); return true; }

	return FClose( vid.mr[0] );
}


bool FRead(cMatrix& Result, cMatrix& vid, cMatrix& vsize, cString& vprecision)
{
	// check vid
	//if (vid.GetType() != VAR_TYPE_NUMERIC) { addError("illegal argument"); return true; }
	if (!vid.IsScalar()) { addError("illegal argument"); return true; }
	// check vsize
	//if (vsize.GetType() != VAR_TYPE_NUMERIC) { addError("illegal argument"); return true; }
	if (!vsize.IsScalar()) { addError("illegal argument"); return true; }
	// check vprecision
	//if (vprecision.GetType() != VAR_TYPE_STRING) { addError("illegal argument"); return true; }
	//if (vprecision.m_M.GetSize() != 1) { addError("illegal argument"); return true; }

	int fid = (int) vid.Get0R();
	int isize = (int) vsize.Get0R();
	string strPrecision = vprecision.GetString();

	return FRead( Result, fid, isize, strPrecision );
}

bool FWrite(cMatrix& Mat, cMatrix& vid, cMatrix& vcount, cString& vprecision)
{
	// check vid
	if (vid.GetSize() != 1) { addError("illegal argument"); return true; }
	// check vsize
	if (vcount.GetSize() != 1) { addError("illegal argument"); return true; }
	// check vprecision
	if (vprecision.GetSize() != 1) { addError("illegal argument"); return true; }

	int fid = (int) vid.Get0R();
	int icount = (int) vcount.Get0R();
	string strPrecision = vprecision.GetString();

	return FWrite( Mat, fid, icount, strPrecision );
}

bool FScanf(cContainer& Result, cMatrix& vid, cString& vformat)
{
	Result.Delete();

	// check vid
	if (vid.GetSize() != 1) { addError("illegal argument"); return true; }
	// check vformat
	if (vformat.s.empty()) { addError("illegal argument"); return true; }

	int fid = (int) vid.Get0R();

	vector<cContainer> vv;

	if (FScanf(vv, fid, vformat.s)) return true;

	return vv2vars(Result, vv);
}

bool FScanf(cContainer& Result_1, cContainer& Result_2, cMatrix& vid, cString& vformat)
{
	Result_1.Delete();
	Result_2.Delete();

	// check vid
	if (vid.GetSize() != 1) { addError("illegal argument"); return true; }
	// check vformat
	if (vformat.s.empty()) { addError("illegal argument"); return true; }

	int fid = (int) vid.Get0R();

	vector<cContainer> vv;

	if (FScanf(vv, fid, vformat.s)) return true;

	return vv2vars(Result_1, Result_2, vv);
}

bool FScanf(cContainer& Result_1, cContainer& Result_2, cContainer& Result_3, cMatrix& vid, cString& vformat)
{
	Result_1.Delete();
	Result_2.Delete();
	Result_3.Delete();

	// check vid
	if (vid.GetSize() != 1) { addError("illegal argument"); return true; }
	// check vformat
	if (vformat.s.empty()) { addError("illegal argument"); return true; }

	int fid = (int) vid.Get0R();

	vector<cContainer> vv;

	if (FScanf(vv, fid, vformat.s)) return true;

	return vv2vars(Result_1, Result_2, Result_3, vv);
}

bool FScanf(vector<cContainer>& Result, cMatrix& vid, cString& vformat)
{
	// check vid
	if (vid.GetSize() != 1) { addError("illegal argument"); return true; }
	// check vformat
	if (vformat.s.empty()) { addError("illegal argument"); return true; }

	int fid = (int) vid.Get0R();

	//vector<cContainer> vv;

	if (FScanf(Result, fid, vformat.s)) return true;

	//return vv2vars(Result, vv);
	return false;
}

bool stripCR(cString& clean, cString& dirty)
{
	int pos = dirty.s.find('\r',0);
	int last = -1;
	clean.s.clear();

	while (pos != string::npos)
	{
		clean.s.append( dirty.s.substr(last+1,pos-last-1) );
		last = pos;
		pos = dirty.s.find('\r',pos+1);
	}

	clean.s.append( dirty.s.substr(last+1) );

	return false;
}

bool FPrintf(cMatrix& Result, cMatrix& vid, cString& vformat)
{
	Result.Delete();
	cString clean;

	stripCR(clean, vformat);

	// check vid
	if (vid.GetSize() != 1) {
		addError("illegal argument"); return true; }
	// check vformat
	if (vformat.s.empty()) {
		addError("illegal argument"); return true; }

	int fid = (int) vid.Get0R();

	vector<cContainer> vargs;

	if (FPrintf(fid, clean.s))
		return true;

	Result.SetTo(1.);
	return false;
}

bool FPrintf(cMatrix& Result, cMatrix& vid, cString& vformat, cContainer& varg)
{
	Result.Delete();
	cString clean;

	stripCR(clean, vformat);

	// check vid
	if (vid.GetSize() != 1) { addError("illegal argument"); return true; }
	// check vformat
	if (vformat.s.empty()) { addError("illegal argument"); return true; }

	int fid = (int) vid.Get0R();

	vector<cContainer> vargs;
	vargs.push_back(varg);

	if (FPrintf(fid, clean.s, vargs)) return true;

	Result.SetTo(1.);
	return false;
}

bool FPrintf(cMatrix& Result, cMatrix& vid, cString& vformat, vector<cContainer>& vargs)
{
	cString clean;

	stripCR(clean, vformat);

	// check vid
	if (vid.GetSize() != 1) { addError("illegal argument"); return true; }
	// check vformat
	string fmt = vformat.GetString();
	if (fmt.empty()) { addError("illegal argument"); return true; }

	int fid = (int) vid.Get0R();

	if (FPrintf(fid, clean.s, vargs)) return true;
	Result.SetTo(1.);

	return false;
}

bool Sprintf(cString& Result, cString& vformat)
{
	Result.Empty();

	vector<cContainer> vv;

	return SPrintf(Result.s, vformat.s, vv);
}

bool Sprintf(cString& Result, cString& vformat, cContainer& varg)
{
	Result.Empty();

	vector<cContainer> vv;
	vv.push_back(varg);

	return SPrintf(Result.s, vformat.s, vv);
}

bool Sprintf(cString& Result, cString& vformat, cContainer& varg1, cContainer& varg2)
{
	Result.Empty();

	vector<cContainer> vv;
	vv.push_back(varg1);
	vv.push_back(varg2);

	return SPrintf(Result.s, vformat.s, vv);
}

bool Sprintf(cString& Result, cString& vformat, vector<cContainer>& vargs)
{
	Result.Empty();

	if (vargs.size()==0) { addError("syntax error: sprintf"); return true; }

	return SPrintf(Result.s, vformat.s, vargs);
}

bool Printf(cString& vformat)
{
	cString Result;

	// check vformat
	if (vformat.s.empty()) { addError("illegal argument"); return true; }

	vector<cContainer> vv;

	if (SPrintf(Result.s, vformat.s, vv)) return true;

	return QuietShow(Result);
}

bool Printf(cString& vformat, cContainer& varg)
{
	cString Result;

	// check vformat
	if (vformat.s.empty()) { addError("illegal argument"); return true; }

	vector<cContainer> vv;
	vv.push_back(varg);

	if (SPrintf(Result.s, vformat.s, vv)) return true;

	return QuietShow(Result);
}

bool Printf(cString& vformat, cContainer& varg1, cContainer& varg2)
{
	cString Result;

	// check vformat
	if (vformat.s.empty()) { addError("illegal argument"); return true; }

	vector<cContainer> vv;
	vv.push_back(varg1);
	vv.push_back(varg2);

	if (SPrintf(Result.s, vformat.s, vv)) return true;

	return QuietShow(Result);
}

bool Printf(vector<cContainer>& vargs)
{
	cString Result;

	if (vargs.size()==0) { addError("syntax error: printf"); return true; }

	cContainer fmt = vargs[0];

	// check vformat
	if (fmt.GetType() != VAR_TYPE_STRING ||
		fmt.s.s.empty())
		{ addError("illegal argument"); return true; }

	vargs.erase( vargs.begin() );

	if (SPrintf(Result.s, fmt.s.s, vargs)) return true;

	return QuietShow(Result);
}

bool vv2vars(cContainer& Result, vector<cContainer>& vv)
{
	if (vv.size()<1) { addError("internal error: expected 1 or more arguments"); return true; }
	if (vv.size()==1) { Result = vv[0]; return false; }

	if (HorzCat( Result, vv[0], vv[1] )) return true;
	
	cContainer vtemp;

	for (int i=2; i<(int)vv.size(); ++i)
	{
		if (HorzCat(vtemp, Result, vv[i])) { Result.Delete(); return true; }
		Result = vtemp;
	}
	return false;
}

bool vv2vars(cContainer& Result_1, cContainer& Result_2, vector<cContainer>& vv)
{
	if (vv.size()<2) { addError("internal error: expected 2 or more arguments"); return true; }
	if (vv.size()==2) { Result_1 = vv[0]; Result_2 = vv[1]; return false; }

	Result_1 = vv[0];
	if (HorzCat( Result_2, vv[1], vv[2] )) return true;
	
	cContainer vtemp;

	for (int i=3; i<(int)vv.size(); ++i)
	{
		if (HorzCat(vtemp, Result_2, vv[i])) { Result_1.Delete(); Result_2.Delete(); return true; }
		Result_2 = vtemp;
	}
	return false;
}

bool vv2vars(cContainer& Result_1, cContainer& Result_2, cContainer& Result_3, vector<cContainer>& vv)
{
	if (vv.size()<3) { addError("internal error: expected 3 or more arguments"); return true; }
	if (vv.size()==3) { Result_1 = vv[0]; Result_2 = vv[1]; Result_3 = vv[2]; return false; }

	return true;
/*
	Result_1 = vv[0];
	Result_2 = vv[1];
	if (HorzCat( Result_3, vv[2], vv[3] )) return true;
	
	cVariable vtemp;

	for (int i=4; i<vv.size(); ++i)
	{
		if (HorzCat(vtemp, Result_3, vv[i])) { Result_1.Empty(); Result_2.Empty(); Result_3.Empty(); return true; }
		Result_3 = vtemp;
	}
	return false;
	*/
}

bool vv2vars(vector<valueBlob>& Result, vector<cContainer>& vv)
{
	int nargout = Result.size();
	int i;

	if ((int)vv.size()<nargout) return true;

	for (i=0; i<nargout; ++i) { Result[i].vResult = vv[i]; }
	if (vv.size()==nargout) return false;

	// vv.size() > Results.size()
	int nlast = nargout - 1;
	cContainer vtmp;
	//cString stemp;
	//cMatrix mtemp;

	int nType = vv[nargout-1].GetType();

	for (int i=nargout; i<(int)vv.size(); ++i)
	{
		if ( HorzCat(vtmp, Result[nlast].vResult, vv[i])) { return true; }
		//if (nType == VAR_TYPE_NUMERIC) {
		//	if (HorzCat(mtemp, Result[nlast].vResult, vv[i])) { return true; }
		//}
		//else if (nType == VAR_TYPE_STRING) {
		//	if (HorzCat(stemp, Result[nlast].vResult, vv[i])) { return true; }
		//}
	}

	Result[nlast].vResult = vtmp;

	//if (nType == VAR_TYPE_NUMERIC) {
	//	Result[nlast].vResult = mtemp;
	//}
	//else if (nType == VAR_TYPE_STRING) {
	//	Result[nlast].vResult = stemp;
	//}
	return false;
}

bool LoadAscii(cMatrix& Result, cString& vfile)
{
	Result.Empty();	// clear the result first

	string fname;
	fname = vfile.GetString();

	return T.LoadAscii(Result, fname);
}

bool SaveAscii(cString& vfile, cContainer& carg)
{
	cContainer c;
	c.SetTo(vfile);

	vector<cContainer> args;
	args.push_back( c );
	args.push_back( carg );

	return SaveAscii( args );
}

bool SaveAscii(cString& vfile, cContainer& carg_1, cContainer& carg_2)
{
	cContainer c;
	c.SetTo(vfile);

	vector<cContainer> args;
	args.push_back( c );
	args.push_back( carg_1 );
	args.push_back( carg_2 );

	return SaveAscii( args );
}

bool SaveAscii(vector<cContainer>& carg)
{
	if (carg.empty()) {
		addError("insufficient arguments provided");
		return true;
	}
	else if (carg[0].GetType() != VAR_TYPE_STRING) {
		addError("missing file name");
		return true;
	}

	string fname = carg[0].s.GetString();

	vector<string> vsarg;

	for (int i=1; i<(int)carg.size(); ++i)
		vsarg.push_back(carg[i].GetName());

	if (T.SaveAscii(vsarg, fname))
		return true;

	return false;
}


//
// File I/O helpers
//
int FOpen(string& FName)
{
	int fid;
	FILE *pF = fopen( FName.c_str(), "r" );

	if (pF==NULL) return -1;

	fid = prog_state.GetNextFID();
	prog_state.AddFID( fid, pF );

	return fid;
}

int FOpen(string& FName, string& strSpec)
{
	int fid;
	FILE *pF = fopen( FName.c_str(), strSpec.c_str() );

	if (pF==NULL) return -1;

	fid = prog_state.GetNextFID();
	prog_state.AddFID( fid, pF );

	return fid;
}

bool FClose(double fid)
{
	FILE *pFile(NULL);
	int ifid = Round(fid);

	if (prog_state.fileLookup(pFile, ifid)) {
		fclose(pFile);
		prog_state.RmFID(ifid);
	}
	else {
		addError("invalid file ID: ", ifid); return true;
	}

	return false;
}


bool FRead( cMatrix& ReadMat, int& fid, int iSize, string& strPrecision )
{
	/*
		MATLAB documentation snippet:
		============================
	    'uchar'   'unsigned char'  unsigned character,  8 bits.
        'schar'   'signed char'    signed character,  8 bits.
        'int8'    'integer*1'      integer, 8 bits.
        'int16'   'integer*2'      integer, 16 bits.
        'int32'   'integer*4'      integer, 32 bits.
        'int64'   'integer*8'      integer, 64 bits.
        'uint8'   'integer*1'      unsigned integer, 8 bits.
        'uint16'  'integer*2'      unsigned integer, 16 bits.
        'uint32'  'integer*4'      unsigned integer, 32 bits.
        'uint64'  'integer*8'      unsigned integer, 64 bits.
        'single'  'real*4'         floating point, 32 bits.
        'float32' 'real*4'         floating point, 32 bits.
        'double'  'real*8'         floating point, 64 bits.
        'float64' 'real*8'         floating point, 64 bits.
	*/

	int i;
	FILE *pF(NULL);
	ReadMat.Delete();	// clear the input matrix

	if (prog_state.fileLookup(pF, fid)) {
		return true;
	}

	if (strPrecision.compare("uchar")==0 || strPrecision.compare("uint8")==0) {

		vector<unsigned char> uchArr;
		unsigned char uch;
		if (iSize==-1) {
			// read entire file
			while ( fread( &uch, sizeof(unsigned char), 1, pF )==1 ) {
				uchArr.push_back( uch );
			}
		}
		else {
			// read iSize elements
			for (i=0; i<iSize; i++) {
				if (fread( &uch, sizeof(unsigned char), 1, pF )!=1) {
					addError("reached EOF before reading requested elements");
					return true;
				}
				uchArr.push_back( uch );
			}
		}

		// read error?
		if (ferror(pF)) {
			uchArr.clear();
			addError("file read error"); return true;
		}

		// X-Fer values
		if (uchArr.size()>0) {
			if (ReadMat.Create( int(uchArr.size()), 1 )) return true;		// out of memory
			for (i=0; i<(int)uchArr.size(); i++) {
				ReadMat.SetR( i, double( uchArr[i] ) );
			}
		}

	}
	else if (strPrecision.compare("schar")==0 || strPrecision.compare("int8")==0) {

		vector <signed char> schArr;
		signed char sch;
		if (iSize==-1) {
			// read entire file
			while ( fread( &sch, sizeof(char), 1, pF )==1 ) {
				schArr.push_back( sch );
			}
		}
		else {
			// read iSize elements
			for (i=0; i<iSize; i++) {
				if (fread( &sch, sizeof(char), 1, pF )!=1)  {
					addError("reached EOF before reading requested elements");
					return true;
				}
				schArr.push_back( sch );
			}
		}

		// read error?
		if (ferror(pF)) {
			schArr.clear();
			addError("file read error"); return true;
		}

		// X-Fer values
		if (schArr.size()>0) {
			if (ReadMat.Create( int(schArr.size()), 1 )) return true;		// out of memory
			for (i=0; i<(int)schArr.size(); i++) {
				ReadMat.SetR( i, double( schArr[i] ) );
			}
		}

	}
	else if (strPrecision.compare("int16")==0) {

		vector <short> shArr;
		short sh;
		if (iSize==-1) {
			// read entire file
			while ( fread( &sh, sizeof(short), 1, pF )==1 ) {
				shArr.push_back( sh );
			}
		}
		else {
			// read iSize elements
			for (i=0; i<iSize; i++) {
				if (fread( &sh, sizeof(short), 1, pF )!=1)  {
					addError("reached EOF before reading requested elements");
					return true;
				}
				shArr.push_back( sh );
			}
		}

		// read error?
		if (ferror(pF)) {
			shArr.clear();
			addError("file read error"); return true;
		}

		// X-Fer values
		if (shArr.size()>0) {
			if (ReadMat.Create( int(shArr.size()), 1 )) return true;		// out of memory
			for (i=0; i<(int)shArr.size(); i++) {
				ReadMat.SetR( i, double( shArr[i] ) );
			}
		}

	}
	else if (strPrecision.compare("int32")==0) {

		vector <int> iArr;
		int it;
		if (iSize==-1) {
			// read entire file
			while ( fread( &it, sizeof(int), 1, pF )==1 ) {
				iArr.push_back( it );
			}
		}
		else {
			// read iSize elements
			for (i=0; i<iSize; i++) {
				if (fread( &it, sizeof(int), 1, pF )!=1)  {
					addError("reached EOF before reading requested elements");
					return true;
				}
				iArr.push_back( it );
			}
		}

		// read error?
		if (ferror(pF)) {
			iArr.clear();
			addError("file read error"); return true;
		}

		// X-Fer values
		if (iArr.size()>0) {
			if (ReadMat.Create( int(iArr.size()), 1 )) return true;		// out of memory
			for (i=0; i<(int)iArr.size(); i++) {
				ReadMat.SetR( i, double( iArr[i] ) );
			}
		}

	}
	else if (strPrecision.compare("int64")==0) {

		vector <long long> lArr;
		long long li;
		if (iSize==-1) {
			// read entire file
			while ( fread( &li, sizeof(long long), 1, pF )==1 ) {
				lArr.push_back( li );
			}
		}
		else {
			// read iSize elements
			for (i=0; i<iSize; i++) {
				if (fread( &li, sizeof(long long), 1, pF )!=1)  {
					addError("reached EOF before reading requested elements");
					return true;
				}
				lArr.push_back( li );
			}
		}

		// read error?
		if (ferror(pF)) {
			lArr.clear();
			addError("file read error"); return true;
		}

		// X-Fer values
		if (lArr.size()>0) {
			if (ReadMat.Create( int(lArr.size()), 1 )) return true;		// out of memory
			for (i=0; i<int(lArr.size()); i++) {
				ReadMat.SetR( i, double( lArr[i] ) );
			}
		}

	}
	else if (strPrecision.compare("uint16")==0) {

		vector <unsigned short> ushArr;
		unsigned short ush;
		if (iSize==-1) {
			// read entire file
			while ( fread( &ush, sizeof(unsigned short), 1, pF )==1 ) {
				ushArr.push_back( ush );
			}
		}
		else {
			// read iSize elements
			for (i=0; i<iSize; i++) {
				if (fread( &ush, sizeof(unsigned short), 1, pF )!=1)  {
					addError("reached EOF before reading requested elements");
					return true;
				}
				ushArr.push_back( ush );
			}
		}

		// read error?
		if (ferror(pF)) {
			ushArr.clear();
			addError("file read error"); return true;
		}

		// X-Fer values
		if (ushArr.size()>0) {
			if (ReadMat.Create( int(ushArr.size()), 1 )) return true;		// out of memory
			for (i=0; i<(int)ushArr.size(); i++) {
				ReadMat.SetR( i, double( ushArr[i] ) );
			}
		}

	}
	else if (strPrecision.compare("uint32")==0) {

		vector <unsigned int> uiArr;
		unsigned int ui;
		if (iSize==-1) {
			// read entire file
			while ( fread( &ui, sizeof(unsigned int), 1, pF )==1 ) {
				uiArr.push_back( ui );
			}
		}
		else {
			// read iSize elements
			for (i=0; i<iSize; i++) {
				if (fread( &ui, sizeof(unsigned int), 1, pF )!=1)  {
					addError("reached EOF before reading requested elements");
					return true;
				}
				uiArr.push_back( ui );
			}
		}

		// read error?
		if (ferror(pF)) {
			uiArr.clear();
			addError("file read error"); return true;
		}

		// X-Fer values
		if (uiArr.size()>0) {
			if (ReadMat.Create( int(uiArr.size()), 1 )) return true;
			for (i=0; i<(int)uiArr.size(); i++) {
				ReadMat.SetR( i, double( uiArr[i] ) );
			}
		}

	}
	else if (strPrecision.compare("uint64")==0) {

		vector <unsigned long long> ulArr;
		unsigned long long ul;
		if (iSize==-1) {
			// read entire file
			while ( fread( &ul, sizeof(unsigned long long), 1, pF )==1 ) {
				ulArr.push_back( ul );
			}
		}
		else {
			// read iSize elements
			for (i=0; i<iSize; i++) {
				if (fread( &ul, sizeof(unsigned long long), 1, pF )!=1) {
					addError("reached EOF before reading requested elements");
					return true;
				}
				ulArr.push_back( ul );
			}
		}

		// read error?
		if (ferror(pF)) {
			ulArr.clear();
			addError("file read error"); return true;
		}

		// X-Fer values
		if (ulArr.size()>0) {
			if (ReadMat.Create( int(ulArr.size()), 1 )) return true;		// out of memory
			for (i=0; i<(int)ulArr.size(); i++) {
				ReadMat.SetR( i, double( ulArr[i] ) );
			}
		}

	}
	else if (strPrecision.compare("single")==0 || strPrecision.compare("float32")==0) {

		vector <float> fArr;
		float f;
		if (iSize==-1) {
			// read entire file
			while ( fread( &f, sizeof(float), 1, pF )==1 ) {
				fArr.push_back( f );
			}
		}
		else {
			// read iSize elements
			for (i=0; i<iSize; i++) {
				if (fread( &f, sizeof(float), 1, pF )!=1) {
					addError("reached EOF before reading requested elements");
					return true;
				}
				fArr.push_back( f );
			}
		}

		// read error?
		if (ferror(pF)) {
			fArr.clear();
			addError("file read error"); return true;
		}

		// X-Fer values
		if (fArr.size()>0) {
			if (ReadMat.Create( int(fArr.size()), 1 )) return true;		// out of memory
			for (i=0; i<(int)fArr.size(); i++) {
				ReadMat.SetR( i, double( fArr[i] ) );
			}
		}

	}
	else if (strPrecision.compare("double")==0 || strPrecision.compare("float64")==0) {

		vector <double> dArr;
		double d;
		if (iSize==-1) {
			// read entire file
			while ( fread( &d, sizeof(double), 1, pF )==1 ) {
				dArr.push_back( d );
			}
		}
		else {
			// read iSize elements
			for (i=0; i<iSize; i++) {
				if (fread( &d, sizeof(double), 1, pF )!=1)  {
					addError("reached EOF before reading requested elements");
					return true;
				}
				dArr.push_back( d );
			}
		}

		// read error?
		if (ferror(pF)) {
			dArr.clear();
			addError("file read error"); return true;
		}

		// X-Fer values
		if (dArr.size()>0) {
			if (ReadMat.Create( int(dArr.size()), 1 )) return true;		// out of memory
			for (i=0; i<(int)dArr.size(); i++) {
				ReadMat.SetR( i, dArr[i] );
			}
		}

	}
	else
		{ addError("invalid precision specifier"); return true; }		// invalid strPrecision string

	// return fid as the count of elements read
	fid = ReadMat.GetSize();

	return false;
}

bool FWrite( cMatrix& WriteMat, int& fid, int& iCount, string& strPrecision )
{
	/*
		MATLAB documentation snippet:
		============================
	    'uchar'   'unsigned char'  unsigned character,  8 bits.
        'schar'   'signed char'    signed character,  8 bits.
        'int8'    'integer*1'      integer, 8 bits.
        'int16'   'integer*2'      integer, 16 bits.
        'int32'   'integer*4'      integer, 32 bits.
        'int64'   'integer*8'      integer, 64 bits.
        'uint8'   'integer*1'      unsigned integer, 8 bits.
        'uint16'  'integer*2'      unsigned integer, 16 bits.
        'uint32'  'integer*4'      unsigned integer, 32 bits.
        'uint64'  'integer*8'      unsigned integer, 64 bits.
        'single'  'real*4'         floating point, 32 bits.
        'float32' 'real*4'         floating point, 32 bits.
        'double'  'real*8'         floating point, 64 bits.
        'float64' 'real*8'         floating point, 64 bits.
	*/

	int i;
	double *pr = NULL;
	FILE *pF(NULL);
	if (prog_state.fileLookup( pF, fid )==0) { addError("unable to find file associated with ID=", fid); return true; }

	if (strPrecision.compare("uchar")==0 || strPrecision.compare("uint8")==0) {

		unsigned char *uchar_mat;
		uchar_mat = new unsigned char[WriteMat.GetSize()];

		// write elements
		pr = WriteMat.GetR();
		for (i=0; i<WriteMat.GetSize(); i++) {
			uchar_mat[i] = (unsigned char) pr[i];
		}
		iCount = fwrite( uchar_mat, sizeof(unsigned char), WriteMat.GetSize(), pF );

		delete []uchar_mat;

		// read error?
		if (ferror(pF)) {
			addError("file write error"); return true;
		}

	}
	else if (strPrecision.compare("schar")==0 || strPrecision.compare("int8")==0) {

		signed char *schar_mat;
		schar_mat = new signed char[WriteMat.GetSize()];

		// write elements
		pr = WriteMat.GetR();
		for (i=0; i<WriteMat.GetSize(); i++) {
			schar_mat[i] = (signed char)(pr[i]);
		}
		iCount = fwrite( schar_mat, sizeof(signed char), WriteMat.GetSize(), pF );

		delete []schar_mat;

		// read error?
		if (ferror(pF)) {
			addError("file write error"); return true;
		}

	}
	else if (strPrecision.compare("int16")==0) {

		short *sh_mat;
		sh_mat = new short[WriteMat.GetSize()];

		// write elements
		pr = WriteMat.GetR();
		for (i=0; i<WriteMat.GetSize(); i++) {
			sh_mat[i] = (short)(pr[i]);
		}
		iCount = fwrite( sh_mat, sizeof(short), WriteMat.GetSize(), pF );

		delete []sh_mat;

		// read error?
		if (ferror(pF)) {
			addError("file write error"); return true;
		}

	}
	else if (strPrecision.compare("int32")==0) {

		int *i_mat;
		i_mat = new int[WriteMat.GetSize()];

		// write elements
		pr = WriteMat.GetR();
		for (i=0; i<WriteMat.GetSize(); i++) {
			i_mat[i] = (int)(pr[i]);
		}
		iCount = fwrite( i_mat, sizeof(int), WriteMat.GetSize(), pF );

		delete []i_mat;

		// read error?
		if (ferror(pF)) {
			addError("file write error"); return true;
		}

	}
	else if (strPrecision.compare("int64")==0) {

		long long *l_mat;
		l_mat = new long long[WriteMat.GetSize()];

		// write elements
		pr = WriteMat.GetR();
		for (i=0; i<WriteMat.GetSize(); i++) {
			l_mat[i] = (long long)(pr[i]);
		}
		iCount = fwrite( l_mat, sizeof(long long), WriteMat.GetSize(), pF );

		delete []l_mat;

		// read error?
		if (ferror(pF)) {
			addError("file write error"); return true;
		}

	}
	else if (strPrecision.compare("uint16")==0) {

		unsigned short *ush_mat;
		ush_mat = new unsigned short[WriteMat.GetSize()];

		// write elements
		pr = WriteMat.GetR();
		for (i=0; i<WriteMat.GetSize(); i++) {
			ush_mat[i] = (unsigned short)(pr[i]);
		}
		iCount = fwrite( ush_mat, sizeof(unsigned short), WriteMat.GetSize(), pF );

		delete []ush_mat;

		// read error?
		if (ferror(pF)) {
			addError("file write error"); return true;
		}

	}
	else if (strPrecision.compare("uint32")==0) {

		unsigned int *ui_mat;
		ui_mat = new unsigned int[WriteMat.GetSize()];

		// write elements
		pr = WriteMat.GetR();
		for (i=0; i<WriteMat.GetSize(); i++) {
			ui_mat[i] = (unsigned int)(pr[i]);
		}
		iCount = fwrite( ui_mat, sizeof(unsigned int), WriteMat.GetSize(), pF );

		delete []ui_mat;

		// read error?
		if (ferror(pF)) {
			addError("file write error"); return true;
		}

	}
	else if (strPrecision.compare("uint64")==0) {

		unsigned long long *ul_mat;
		ul_mat = new unsigned long long[WriteMat.GetSize()];

		// write elements
		pr = WriteMat.GetR();
		for (i=0; i<WriteMat.GetSize(); i++) {
			ul_mat[i] = (unsigned long long)(pr[i]);
		}
		iCount = fwrite( ul_mat, sizeof(unsigned long long), WriteMat.GetSize(), pF );

		delete []ul_mat;

		// read error?
		if (ferror(pF)) {
			addError("file write error"); return true;
		}

	}
	else if (strPrecision.compare("single")==0 || strPrecision.compare("float32")==0) {

		float *f_mat;
		f_mat = new float[WriteMat.GetSize()];

		// write elements
		pr = WriteMat.GetR();
		for (i=0; i<WriteMat.GetSize(); i++) {
			f_mat[i] = (float)(pr[i]);
		}
		iCount = fwrite( f_mat, sizeof(float), WriteMat.GetSize(), pF );

		delete []f_mat;

		// read error?
		if (ferror(pF)) {
			addError("file write error"); return true;
		}

	}
	else if (strPrecision.compare("double")==0 || strPrecision.compare("float64")==0) {

		double *d_mat;
		d_mat = new double[WriteMat.GetSize()];

		// write elements
		pr = WriteMat.GetR();
		for (i=0; i<WriteMat.GetSize(); i++) {
			d_mat[i] = (double)(pr[i]);
		}
		iCount = fwrite( d_mat, sizeof(double), WriteMat.GetSize(), pF );

		delete []d_mat;

		// read error?
		if (ferror(pF)) {
			addError("file write error"); return true;
		}

	}
	else
	{ addError("invalid precision specifier"); return true; }		// invalid strPrecision string

	return false;
}

bool FScanf( vector<cContainer>& ReadVars, int& fid, string& format)
{
	ReadVars.clear();

	FILE *pF(NULL);
	if (prog_state.fileLookup( pF, fid )==0) { addError("unable to find file associated with ID=", fid); return true; }

	int n = countFmtArgs(format);
	double lf;
	char ch, chFmt;
	int ival, ipos(-1);
	unsigned int ui;
	string s, fsub;
	cMatrix mat;
	cString str;
	cContainer cont;

	for (int i=0; i<n; ++i)
	{
		if (nextFormat(ipos,fsub,format)) break;
		chFmt = format[ipos];

		//var.Empty();

		switch (chFmt) {
		case 'c':
			if (fscanf(pF,fsub.c_str(),&ch) == EOF) { addError("reached EOF"); return false; }
			str.SetTo(ch);
			cont.SetTo(str);
			ReadVars.push_back( cont );
			break;

		case 'd':
			if (fscanf(pF,fsub.c_str(),&ival) == EOF) { addError("reached EOF"); return false; }
			lf = (double) ival;
			mat.SetTo(lf);
			cont.SetTo(mat);
			ReadVars.push_back( cont );
			break;

		case 'e': case 'E': case 'f': case 'g': case 'G':
			if (fscanf(pF,fsub.c_str(),&lf) == EOF) { addError("reached EOF"); return false; }
			mat.SetTo(lf);
			cont.SetTo(mat);
			ReadVars.push_back( cont );
			break;

		case 's':
			if (fscanf(pF,"%c",&ch) == EOF) { addError("reached EOF"); return false; }
			s = ch;
			while (fscanf(pF,"%c",&ch) != EOF) {
				if (ch==' ' || ch=='\t' || ch=='\0' || ch=='\r' || ch=='\n') break;
				s.append( 1, ch );
			}
			str.SetTo( s );
			cont.SetTo(str);
			ReadVars.push_back( cont );
			break;

		case 'u':
			if (fscanf(pF,"%u",&ui) == EOF) { addError("reached EOF"); return false; }
			lf = (double) ui;
			mat.SetTo(lf);
			cont.SetTo(mat);
			ReadVars.push_back( cont );
			break;

		default:	// illegal format character
			addError("illegal format character"); return true;
		};

	}

	return false;
}

bool FPrintf( int& fid, string& format )
{
	vector<cContainer> vargs;		// no args (empty vector)
	return FPrintf( fid, format, vargs );
}

bool FPrintf( int& fid, string& format, vector<cContainer>& vargs )
{
	FILE *pF(NULL);
	if (prog_state.fileLookup( pF, fid )==0) {
		addError("unable to find file associated with ID=", fid); return true; }

	//int n = countPercents(format);
	double lf;
	char ch, chFmt;
	int ival, ipos(-1), ilast(0), i;
	unsigned int ui;
	string s, fsub, chunk;

	for (i=0; i<(int)vargs.size(); ++i)
	{
		if (nextFormat(ipos,fsub,format)) {
			addError("invalid format string"); return true; }		// invalid format string
		if (ipos<0) { addError("too many arguments given for format string"); return true; }

		chunk = format.substr(ilast, (ipos-fsub.length()+1)-ilast);
		ilast = ipos + 1;
		chFmt = format[ipos];

		// print chunk between this fmt thing and last
		if (fprintf(pF, chunk.c_str()) < 0) { addError("fprintf failed, improper format string?"); return true; }

		switch (chFmt) {
		case 'c':
			if (vargs[i].GetType()==VAR_TYPE_STRING) {	// get 1st char of string
				if (vargs[i].GetSafeStr(s)) { addError("bad argument"); return true; }
				ch = s.at(0);
			}
			else {	// cast from double to char
				if (vargs[i].IsEmpty()) { addError("empty variable"); return true; }
				vargs[i].GetSafe0(lf);
				ch = (char) lf;
			}
			if (fprintf(pF,fsub.c_str(),ch) < 0) { addError("fprintf failed, improper format string?"); return true; }
			break;

		case 'd':
			if (vargs[i].GetType()!=VAR_TYPE_NUMERIC) { addError("numeric data expected"); return true; }
			if (vargs[i].GetSafe0(lf)) { addError("bad argument"); return true; }
			ival = (int) lf;

			if (fprintf(pF,fsub.c_str(),ival) < 0) { addError("fprintf failed, improper format string?"); return true; }
			break;

		case 'e': case 'E': case 'f': case 'g': case 'G':
			if (vargs[i].GetType()!=VAR_TYPE_NUMERIC) { addError("numeric data expected"); return true; }
			if (vargs[i].GetSafe0(lf)) { addError("bad argument"); return true; }

			if (fprintf(pF,fsub.c_str(),lf) < 0) { addError("fprintf failed, improper format string?"); return true; }
			break;

		case 's':
			if (vargs[i].GetType()!=VAR_TYPE_STRING) { addError("string data expected"); return true; }
			if (vargs[i].GetSafeStr(s)) { addError("bad argument"); return true; }

			if (fprintf(pF,fsub.c_str(),s.c_str()) < 0) { addError("fprintf failed, improper format string?"); return true; }
			break;

		case 'u':
			if (vargs[i].GetType()!=VAR_TYPE_NUMERIC) { addError("numeric data expected"); return true; }
			if (vargs[i].GetSafe0(lf)) { addError("empty variable"); return true; }
			ui = (unsigned int) lf;

			if (fprintf(pF,"%u",ui) < 0) { addError("fprintf failed, improper format string?"); return true; }
			break;

		default:	// illegal format character
			addError("illegal format character"); return true;
		};

	}	// loop over args

	ilast = ipos;
	if (nextFormat(ilast,fsub,format)) { addError("invalid format string"); return true; }		// invalid format string
	if (ilast>=0) {
		addError("too few arguments given for format string"); return true;
	}

	// add any remaining stuff after args are finished
	if ( ipos < int(format.length())-1 ) {
		fsub = format.substr(ipos+1,string::npos);
		if (fprintf(pF, fsub.c_str()) < 0) { addError("fprintf failed, improper format string?"); return true; }
	}

	return false;
}

bool SPrintf( string& Result, string& format, vector<cContainer>& vargs )
{
	Result.clear();

	//int n = countPercents(format);
	double lf;
	char ch, chFmt;
	int ival, ipos(-1), ilast(0), i;
	unsigned int ui;
	string s, chunk, fsub;
	bool bErr(false);

	int nSize = 64;
	char *pch = new char[nSize];

	for (i=0; i<(int)vargs.size(); ++i)
	{
		if (nextFormat(ipos,fsub,format)) { addError("invalid format string"); bErr = true; goto sprintfDone; }		// invalid format string

		if (ipos<0) { addError("too many arguments given for format string"); bErr = true; goto sprintfDone; }

		chunk = format.substr(ilast, (ipos-fsub.length()+1)-ilast);
		ilast = ipos + 1;
		chFmt = format[ipos];

		switch (chFmt) {
		case 'c':
			if (vargs[i].GetType()==VAR_TYPE_STRING) {	// get 1st char of string
				if (vargs[i].IsEmpty()) { addError("empty argument"); bErr = true; goto sprintfDone; }
				if (vargs[i].GetSafeStr(s)) { addError("bad argument"); bErr = true; goto sprintfDone; }
				ch = s.at(0);
			}
			else {	// cast from double to char
				if (vargs[i].IsEmpty()) { addError("empty variable"); bErr = true; goto sprintfDone; }
				if (vargs[i].GetSafe0(lf)) { addError("bad argument"); bErr = true; goto sprintfDone; }
				ch = (char) lf;
			}
			if (sprintf(pch, fsub.c_str(), ch) < 0) { bErr = true; goto sprintfDone; }
			break;

		case 'd':
			if (vargs[i].GetType()!=VAR_TYPE_NUMERIC) { addError("numeric data expected"); bErr = true; goto sprintfDone; }
			if (vargs[i].IsEmpty()) { addError("empty variable"); bErr = true; goto sprintfDone; }
			if (vargs[i].GetSafe0(lf)) { addError("bad argument"); bErr = true; goto sprintfDone; }
			ival = (int) lf;

			if (sprintf(pch, fsub.c_str(), ival) < 0) { bErr = true; goto sprintfDone; }
			break;

		case 'e': case 'E': case 'f': case 'g': case 'G':
			if (vargs[i].GetType()!=VAR_TYPE_NUMERIC) { addError("numeric data expected"); bErr = true; goto sprintfDone; }
			if (vargs[i].IsEmpty()) { addError("empty variable"); bErr = true; goto sprintfDone; }
			if (vargs[i].GetSafe0(lf)) { addError("bad argument"); bErr = true; goto sprintfDone; }

			if (sprintf(pch, fsub.c_str(), lf) < 0) { bErr = true; goto sprintfDone; }
			break;

		case 's':
			if (vargs[i].GetType()==VAR_TYPE_STRING) {
				//if (vargs[i].Str.length()==0) { addError("empty variable"); bErr = true; goto sprintfDone; }
				if (vargs[i].GetSafeStr(s)) { addError("bad argument"); bErr = true; goto sprintfDone; }
			}
			else { bErr = true; goto sprintfDone; }		// type mismatch

			//if (sprintf(pch, format.c_str(), s.c_str()) < 0) { bErr = true; goto sprintfDone; }
			if (sprintf(pch, fsub.c_str(), s.c_str()) < 0) { bErr = true; goto sprintfDone; }
			break;

		case 'u':
			if (vargs[i].GetType()!=VAR_TYPE_NUMERIC) { addError("numeric data expected"); bErr = true; goto sprintfDone; }
			if (vargs[i].IsEmpty()) { addError("empty variable"); bErr = true; goto sprintfDone; }
			if (vargs[i].GetSafe0(lf)) { addError("bad argument"); bErr = true; goto sprintfDone; }
			ui = (unsigned int) lf;

			if (sprintf(pch, "%u", ui) < 0) { bErr = true; goto sprintfDone; }
			break;

		default:	// illegal format character
			addError("illegal format character"); { bErr = true; goto sprintfDone; }
		};

		Result.append( chunk );
		Result.append( pch );

	}	// loop over args

	ilast = ipos;
	if (nextFormat(ilast,fsub,format)) { addError("invalid format string"); bErr = true; goto sprintfDone; }		// invalid format string
	if (ilast>=0) {
		addError("too few arguments given for format string"); bErr = true; goto sprintfDone;
	}

	// add any remaining stuff after args are finished
	if (ipos<int(format.length())-1) {
		fsub = format.substr(ipos+1,string::npos);
		Result.append( fsub );
	}

sprintfDone:
	delete []pch;
	return bErr;
}


bool FGetline( string& line, int& fid )
{
	FILE *pF(NULL);
	if (prog_state.fileLookup( pF, fid )==0) { addError("unable to find file associated with ID=", fid); return true; }

	char ch;
	line.clear();

	if (fscanf(pF,"%c",&ch) == EOF) return false;
	line = ch;
	while (fscanf(pF,"%c",&ch) != EOF) {
		if (ch=='\n') break;
		line.append( 1, ch );
	}

	return false;
}


int countFmtArgs(string& s)
{
	int n = s.length(), np(0);
	int i(0);

	while (i<n) {
		if (s[i]=='%') {
			if (i>0) {
				if (s[i-1] != '\\') ++np;
			}
			else ++np;
		}
		++i;
	}
	return np;
}

bool nextFormat(int& ipos, string& fsub, string& s)
{
	// "result = %7.2f" --> ipos=9, fsub="7.2f"
	// "%d" --> ipos=0, fsub="d"
	// "%% increase" --> "% increase"

	++ipos;
	string strChars = "cdeEfgGsu";
	int ibeg = ipos;

	int n = s.length();

	if (ipos<0) ipos = 0;
	if (ipos>=n) { ipos = -1; return false; }

	while (ipos<n)
	{
		if (s[ipos]=='%') {
			if (ipos<n-1) {
				if (s[ipos+1] != '%')			// ignore %% instances
					break;
				else
					++ipos;
			}
			else
				return true;		// illegal format
		}
		++ipos;
		if (ipos==n) { ipos = -1; return false; }
	}

	// ipos points to valid '%', now look for format type char
	size_t stChar = s.find_first_of(strChars, ipos);
	if (stChar==string::npos)
		return true;	// format error, could not find fmt char

	fsub = s.substr(ipos, stChar - ipos + 1);
	ipos = stChar;
	
	return false;
}


void GetFileIDList()			// populate m_iFileIDs with current open FIDs
{
	GetFileIDList( prog_state.m_iFileIDs );
}

void GetFileIDList(vector<int>& fileIds)			// populate m_iFileIDs with current open FIDs
{
	prog_state.GetFileIds(fileIds);
}


bool Quit(void)
{
	prog_state.bQuitFlag = true;
	return false;
}

bool Rundir()
{
	sReportEvent re;
	re.bReport = true;
	re.iType = RPT_LIST;
	re.list.push_back( prog_state.m_RunDir );
	prog_state.pushReport(re);

	return false;
}


bool ForceCrash()
{
		int* p = NULL;
		*p = 13;

	return false;
}


bool Pause(void)
{
	prog_state.Pause();
	return false;
}

bool ShowPwd(void)
{
	sReportEvent re;
	re.bReport = true;
	re.iType = RPT_LIST;
	re.list.push_back( prog_state.getWorkingDir() );
	prog_state.pushReport( re );

	return false;
}

bool ShowPath(void)
{
	vector<string> path = prog_state.m_lang.getPath();

	//CChildView *pv = (CChildView *) CChildView::GetView();
	//pv->WindowPrintList( path );

	sReportEvent re;
	re.bReport = true;
	re.iType = RPT_LIST;
	re.list = path;
	prog_state.pushReport( re );

	return false;
}

bool NewPath(cString& vdir)
{
	if ( prog_state.m_lang.addPath(vdir.s) ) return true;

	//vector<string> newFiles;
	//if (prog_state.m_lang.updateAllScripts(newFiles)) return true;

	// send corrected debug stops back to editor
	//prog_state.FlushDebugStops(newFiles);

	prog_state.m_bUpdateScripts = true;

	return false;
}

bool DelPath(cString& vdir)
{
	if ( prog_state.m_lang.delPath(vdir.s) ) return true;

	prog_state.m_bUpdateScripts = true;

	////pDoc->SendPathToEditor();

	return false;
}

bool Cd(cString& vdir)
{
	// attempt to change directory
	if (prog_state.setWorkingDir( vdir.s )) { return true;	}

	prog_state.m_bUpdateScripts = true;

	return false;
}

bool Pwd(cString& Result)
{
	Result.Empty();
	Result.s = prog_state.getWorkingDir();

	return false;
}

bool Ls(void)		// file list
{
	vector<string> files, subdirs;
	string dir = prog_state.getWorkingDir();

	if (getFilesInDir(files, subdirs, dir)) return true;

	sReportEvent re;
	re.bReport = true;
	re.iType = RPT_BRACKET_LIST;
	re.list = subdirs;
	prog_state.pushReport( re );

	re.list = files;
	re.iType = RPT_LIST;
	prog_state.pushReport( re );

	return false;
}

bool Ls(cString& arg)		// file list
{
	vector<string> files, subdirs;
	string dir = prog_state.getWorkingDir();

	if (handleDotDot(dir, arg.s)) {
		addError("location not found: ", arg.s);
		return true;
	}

	if (arg.s.empty()) {
			if (getFilesInDir(files, subdirs, dir)) return true;
	}
	else {
		if (getFilesInDir(files, subdirs, dir, arg.s)) return true;
	}

	sReportEvent re;
	re.bReport = true;
	re.iType = RPT_BRACKET_LIST;
	re.list = subdirs;
	prog_state.pushReport( re );

	re.list = files;
	re.iType = RPT_LIST;
	prog_state.pushReport( re );

	return false;
}

bool handleDotDot(string& absDir, string& arg)
{ // if the leading 2 chars of arg equal "..", then process all leading "../" or "..\"
	// by modifying absDir as appropriate, and appending any leftover stuff from arg
	string leading = arg.substr(0,2);

	int ct(0);
	string suffix = arg;

	if (leading != "..")
		return false;		// nothing to do

	while (leading == "..")
	{
		suffix = suffix.substr(2,string::npos);
		if (suffix.empty()) {
			++ct;
			break;
		}
		else if (suffix.at(0)=='/' || suffix.at(0)=='\\') {
			++ct;
			suffix = suffix.substr(1,string::npos);
			leading = suffix.substr(0,2);
		}
		else {
			return true;	// error
		}
	}

	size_t pos = absDir.find_last_of("/\\", string::npos);
	size_t posLast;

	while (ct > 0)
	{
		if (pos != string::npos) {
			--ct;
			posLast = pos;
			pos = absDir.find_last_of("/\\", pos-1);
		}
		else
			return true;		// error
	}

	if (posLast != string::npos) {
		absDir = absDir.substr(0,posLast);
		if (!suffix.empty()) {
			absDir.append("\\");
			absDir.append( suffix );
		}
		arg.clear();
	}
	else {	// go to base drive
		pos = absDir.find_first_of("/\\",0);
		if (pos == string::npos) return true;
		absDir = absDir.substr(0,pos);
		arg.clear();
	}

	return false;
}

bool Vls(void)		// variable list
{
	vector<string> *pSymbols = T.GetSymbolList();

	sReportEvent re;
	re.bReport = true;
	re.iType = RPT_LIST;
	re.list = *pSymbols;
	prog_state.pushReport( re );

	//CChildView *pv = CChildView::GetView();
	//pv->WindowPrintList(*pSymbols);
	return false;
}

bool Delete(cString& var)
{
	return T.Remove(var.s);
}

bool Delete(cString& var1, cString& var2)
{
	if (T.Remove(var1.s)) return true;
	return T.Remove(var2.s);
}

bool Delete(cString& var1, cString& var2, cString& var3)
{
	if (T.Remove(var1.s)) return true;
	if (T.Remove(var2.s)) return true;
	return T.Remove(var3.s);
}

bool Delete(vector<cString>& vars)
{
	for (int i=0; i<(int)vars.size(); ++i) {
		if (Delete(vars[i])) return true;
	}
	return false;
}

bool DeleteAll(void)
{
	T.Clear();
	return false;
}

bool Cleanup(void)
{
	//close_all_figs();
	DeleteAll();
	return false;
}

bool Help(cString& arg)
{
	return false;
}

bool Help()
{
	return false;
}

bool getFilesInDir(vector<string>& files, string& dir)
{
	vector<string> subdirs;
	return getFilesInDir(files, subdirs, dir);
}

bool getFilesInDir(vector<string>& files, vector<string>& subdirs, string& dir)
{
	files.clear();
	subdirs.clear();
	string fname;

#ifdef _WINDOWS_
	// windows directory interrogation library

	int ANY_FILE =  _A_NORMAL | /*_A_RDONLY |*/ _A_HIDDEN | _A_SYSTEM | _A_ARCH;

	struct _finddatai64_t data;
	// First create the filename that will be use to initialize the find.
	// "*.*" are wild card characters that tells the find function to return a 
	// list of all the files and directories.  You can limit this if you wish
	// to just file with specific extensions, for example "*.txt".  If you do that
	// then finder will not return any directory names.

	// start the finder -- on error _findfirsti64() will return -1, otherwise if no
	// error it returns a handle greater than -1.

	fname = dir + "\\*";

	long h = _findfirsti64(fname.c_str(),&data);
	if (h >= 0)
	{
		do {
			if( (data.attrib & ANY_FILE) )
			{
				// its a file
				fname = data.name;
				files.push_back( fname );
			}
			else if (data.attrib & _A_SUBDIR)
			{
				// its a subdir
				fname = data.name;
				if (fname.compare(".") != 0)		// ignore pwd
					subdirs.push_back( fname );
			}
		} while ( _findnexti64(h,&data) == 0 );
		// close the find handle.  
		_findclose(h);

	}
#else
	// POSIX directory interrogation library

    DIR *dp;
    struct dirent *dirp;
    if((dp = opendir(dir.c_str())) == NULL) {
		return true;
    }

	string str;
    while ((dirp = readdir(dp)) != NULL)
	{
		str = string(dirp->d_name);

		if (str.size()>0)
		{
			if (str.at(0)!='.')
			{
				struct stat st;
				size_t size;
				if ( !stat( dirp->d_name, &st ) )
					size = st.st_size;

				if (dirp->d_type == 0x8)
					files.push_back( dirp->d_name );
				else if (dirp->d_type == 0x4)
					subdirs.push_back( dirp->d_name );
			}
		}
    }
    closedir(dp);

#endif
    return false;
}


bool getFilesInDir(vector<string>& files, vector<string>& subdirs, string& dir, string& arg)
{
	files.clear();
	subdirs.clear();
	string fname = arg;
	vector<string> subsubdirs;
	int ix(0);

#ifdef _WINDOWS_
	// windows directory interrogation library

	int ANY_FILE =  _A_NORMAL | _A_RDONLY | _A_HIDDEN | _A_SYSTEM | _A_ARCH;

	struct _finddatai64_t data;
	// First create the filename that will be use to initialize the find.
	// "*.*" are wild card characters that tells the find function to return a 
	// list of all the files and directories.  You can limit this if you wish
	// to just file with specific extensions, for example "*.txt".  If you do that
	// then finder will not return any directory names.

	// start the finder -- on error _findfirsti64() will return -1, otherwise if no
	// error it returns a handle greater than -1.

	long h = _findfirsti64(fname.c_str(),&data);
	if (h >= 0)
	{
		do {
			if( (data.attrib & ANY_FILE) )
			{
				// its a file
				fname = data.name;
				files.push_back( fname );
			}
			else if (data.attrib & _A_SUBDIR)
			{
				// its a subdir
				fname = data.name;
				if (fname.compare(".") != 0)		// ignore pwd
					subdirs.push_back( fname );
			}
		} while ( _findnexti64(h,&data) == 0 );

		if (files.size() == 0 && subdirs.size() == 1)
		{	// ls the dir(s)
			fname = subdirs[ix];
			fname.append("\\*");

			h = _findfirsti64(fname.c_str(),&data);
			if (h >= 0)
			{
				do {
					if( (data.attrib & ANY_FILE) )
					{
						// its a file
						fname = data.name;
						files.push_back( fname );
					}
					else if (data.attrib & _A_SUBDIR)
					{
						// its a subdir
						fname = data.name;
						if (fname.compare(".") != 0)		// ignore pwd
							subsubdirs.push_back( fname );
					}
				} while ( _findnexti64(h,&data) == 0 );
			}

			// replace with those in the subdirs
			subdirs = subsubdirs;
		}

		// close the find handle.  
		_findclose(h);

	}
#else
	// POSIX directory interrogation library

    DIR *dp;
    struct dirent *dirp;
    if((dp = opendir(dir.c_str())) == NULL)
		return true;

	string str;
    while ((dirp = readdir(dp)) != NULL)
	{
		str = string(dirp->d_name);

		if (str.size()>0)
		{
			if (str.at(0)!='.')
			{
				struct stat st;
				size_t size;
				if ( !stat( dirp->d_name, &st ) )
					size = st.st_size;

				if (dirp->d_type == 0x8)
					files.push_back( dirp->d_name );
				else if (dirp->d_type == 0x4)
					subsubdirs.push_back( dirp->d_name );
			}
		}
    }

	if (files.size() == 0 && subdirs.size() == 1)
	{	// ls the dir(s)
		fname = subdirs[ix];
		fname.append("\\*");

	    if((dp = opendir(dir.c_str())) == NULL)
			return true;

		while ((dirp = readdir(dp)) != NULL)
		{
			str = string(dirp->d_name);

			if (str.size()>0)
			{
				if (str.at(0)!='.')
				{
					struct stat st;
					size_t size;
					if ( !stat( dirp->d_name, &st ) )
						size = st.st_size;

					if (dirp->d_type == 0x8)
						files.push_back( dirp->d_name );
					else if (dirp->d_type == 0x4)
						subdirs.push_back( dirp->d_name );
				}
			}
		}
		// replace with those in the subdirs
		subdirs = subsubdirs;

    }

    closedir(dp);

#endif
    return false;
}


bool getFileExtension(string& ext, string& fname)
{
	ext.clear();

	if (fname.empty()) return false;

	size_t ix = fname.find_last_of('.');
	if (ix==string::npos) {
		return false;	// no extension
	}

	string raw_ext = fname.substr(ix+1);
	char ch;
	for (int i=0; i<(int)raw_ext.size(); ++i)
	{
		ch = tolower( raw_ext[i] );
		ext.append(1,ch);
	}
	return false;
}

bool isRelativeDir(string& dir)
{
	size_t pos = dir.find_first_of(":",0);
	if (pos==string::npos) return true;
	return false;
}

bool getAbsoluteDir(string& absdir, string& reldir)
{
	sReportEvent re;
	string backtrack, forward, current, str;
	size_t pos = reldir.find_last_of("..");
	size_t pos_cut;

	char *pch;

	if (pos != string::npos) {
		// cut reldir into two pieces: 1/ "../../.." (backward) part and 2/ "/dir_a/dir_b/dir_c" (forward) part
		pos_cut = pos + 1;
		backtrack = reldir.substr(0,pos_cut);
		forward = reldir.substr(pos_cut,string::npos);
		// attempt to go back
		if (prog_state.setWorkingDir( backtrack )) {
			return true;
		}
		pch = __GETCWD(NULL,MAX_PATH_BUFFER);
		current = pch;
		free( pch );
	}
	else {
		current = prog_state.getWorkingDir();
		forward = reldir;
	}

	// check for empty forward path
	if (forward.length()==0)
		return false;

	// remove leading slash
	if (forward.at(0)=='/' || forward.at(0)=='\\') {
		forward = forward.substr(1,string::npos);
	}

	// check again for empty forward path
	if (forward.length()==0)
		return false;

	size_t pos1 = forward.find_last_of("/",string::npos);
	size_t pos2 = forward.find_last_of("\\",string::npos);
	if (pos1==reldir.length()-1 || pos2==reldir.length()-1) { reldir = reldir.substr(0,reldir.length()-1); }

	absdir = current + '\\';

	absdir.append( forward );

	return false;
}

string getParentDir(string& fulldir)
{	// assumes fulldir is appropriately formatted (no trailing slashes, and all slashes are back (Windows))
	string parentdir, testdir;

	size_t pos = fulldir.find_last_of("\\",string::npos);
	size_t pos2;

	if (pos != string::npos) {
		
		testdir = fulldir.substr(0, pos);				// top-most level looks like "n:\" on windows systems
		pos2 = testdir.find_last_of("\\",string::npos);

		if (pos2==string::npos) parentdir = fulldir;	// we are already at top level
		else parentdir = testdir;
	}
	else
		parentdir = fulldir;	// no change - should never happen though!

	return parentdir;
}

string forwardToBackSlash(string& dir)
{
	string newdir = dir;
	size_t pos = newdir.find_first_of("/",0);
	while (pos != string::npos) {
		newdir[pos] = '\\';
		newdir.insert( pos, "\\" );
		++pos;
		pos = newdir.find_first_of("/",pos);
	}
	return newdir;
}

