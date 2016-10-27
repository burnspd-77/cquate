
#include "c_string.h"
#include <ctype.h>

// File I/O
//
bool WriteData(cString& v, FILE *pf)
{
	int nc = v.s.length();

	if (fwrite( &nc, sizeof(int), 1, pf ) != 1) return true;		// length
	if (fwrite( v.s.data(), sizeof(char), nc, pf ) != nc) return true;

	return false;
}

bool ReadData(cString& m, FILE *pf)
{
	int nc;
	char *pchString;

	// read string
	if (fread( &nc, sizeof(int), 1, pf ) != 1) {		// length
		return true;
	}

	pchString = new char[nc+1];
	if (pchString==NULL) return true;		// out of memory
	if (fread( pchString, sizeof(char), nc, pf ) != nc) {
		addError("Format error in input file"); delete []pchString; return true;
	}
	pchString[nc] = '\0';
	m.s = pchString;
	delete[] pchString;

	return false;
}


bool HorzCat(cString& Result, cString& v1, cString& v2)
{
	Result = v1;
	Result.s.append( v2.s );

	return false;
}

bool Length(cMatrix& res, cString& in)
{
	res.Delete();
	int len = in.GetSize();
	res.SetTo( double(len) );
	return false;
}

bool Size(cMatrix& Result, cString& in)
{
	Result.Create(1,2);
	double *pr = Result.GetR();

	pr[0] = 1;
	pr[1] = in.GetSize();

	return false;
}

bool FindMatch(cMatrix& res, cString& in, cString& pat)
{
	size_t pos = in.s.find(pat.s, 0);
	res.Delete();
	cMatrix tmp;

	while (pos != string::npos)
	{
		HorzCat(res, tmp, double(pos));
		pos = in.s.find(pat.s, pos+1);
		tmp = res;
	}
	return false;
}

bool FindNext(cMatrix& res, cString& in, cString& pat, cMatrix& beg)
{
	res.Delete();

	if (beg.GetSize() != 1) { addError("find_next: expected scalar argument"); return true; }

	int ibeg = (int) beg.Get0R();

	if (ibeg < 0 || ibeg >= in.GetSize()) { addError("find_next: illegal index"); return true; }

	size_t pos = in.s.find(pat.s, ibeg);

	if (pos != string::npos) {
		res.SetTo( double(pos) );
	}
	return false;
}

bool FindPrev(cMatrix& res, cString& in, cString& pat, cMatrix& beg)
{
	res.Delete();

	if (beg.GetSize() != 1) { addError("find_prev: expected scalar argument"); return true; }

	int ibeg = (int) beg.Get0R();

	if (ibeg < 0 || ibeg >= in.GetSize()) { addError("find_prev: illegal index"); return true; }

	size_t pos = in.s.rfind(pat.s, ibeg);

	if (pos != string::npos) {
		res.SetTo( double(pos) );
	}
	return false;
}


bool Replace(cString& res, cString& in, cString& pat, cString& rpat)
{
	res.Delete();

	size_t pos = in.s.find( pat.s );
	size_t last_pos = 0;
	if (pos == string::npos) {
		res.SetTo( in );
		return false;
	}

	while (pos != string::npos)
	{
		res.s.append( in.s.substr(last_pos, pos - last_pos) );
		res.s.append( rpat.s );
		pos += pat.s.length();
		// next pattern
		last_pos = pos;
		pos = in.s.find( pat.s, last_pos );
	}

	res.s.append( in.s.substr(last_pos) );
	return false;
}

bool Substr(cString& res, cString& in, cMatrix& beg, cMatrix& len)
{
	res.Delete();
	if (beg.GetSize() != 1 || len.GetSize() != 1) { addError("substr: expected scalar arguments"); return true; }

	int ibeg = (int) beg.Get0R();
	int ilen = (int) len.Get0R();

	if (ibeg < 0 || ibeg >= in.GetSize()) { addError("substr: illegal index"); return true; }
	if (ilen < 0 || ibeg+ilen >= in.GetSize()) { addError("substr: illegal index"); return true; }

	res.s = in.s.substr( ibeg, ilen );
	return false;
}

bool Substr(cString& res, cString& in, cMatrix& beg)
{
	res.Delete();
	if (beg.GetSize() != 1) { addError("substr: expected scalar arguments"); return true; }

	int ibeg = (int) beg.Get0R();
	if (ibeg < 0 || ibeg >= in.GetSize()) { addError("substr: illegal index"); return true; }

	res.s = in.s.substr( ibeg );
	return false;
}

bool Insert(cString& res, cString& in, cString& arg, cMatrix& pos)
{
	res.Delete();
	if (pos.GetSize() != 1) { addError("insert: expected scalar arguments"); return true; }

	int ipos = (int) pos.Get0R();
	if (ipos < 0 || ipos > in.GetSize()) { addError("insert: illegal index"); return true; }

	res.s = in.s;
	res.s.insert(ipos, arg.s);
	return false;
}

bool Append(cString& res, cString& s1, cString& s2)
{
	res = s1;
	res.s.append( s2.s );
	return false;
}

bool Append(cString& res, cString& s1, cString& s2, cString& s3)
{
	res = s1;
	res.s.append( s2.s );
	res.s.append( s3.s );
	return false;
}

bool Append(cString& res, vector<cString>& vars)
{
	res.Delete();
	if (vars.empty()) return false;

	res = vars[0];

	for (int i=1; i<(int)vars.size(); ++i) {
		res.s.append( vars[i].s );
	}

	return false;
}

bool ToLower(cString& res, cString& in)
{
	res.Delete();
	res.s = in.s;
	int sz = res.GetSize();

	for (int i=0; i<sz; ++i)
	{
		res.s[i] = tolower( res.s.at(i) );
	}
	return false;
}

bool ToUpper(cString& res, cString& in)
{
	res.Delete();
	res.s = in.s;
	int sz = res.GetSize();

	for (int i=0; i<sz; ++i)
	{
		res.s[i] = toupper( res.s.at(i) );
	}
	return false;
}

bool ToString(cString& res, cMatrix& in)
{
	res.Delete();
	string s = in.ToString();

	return res.SetTo( s );
}

bool ToScalar(cMatrix& res, cString& in)
{
	res.Delete();
	return res.ToScalar( in.s );
}

bool is_empty(cMatrix& res, cString& M)
{
	res.Create(1,1);
	res.mr[0] = (int) M.IsEmpty();
	return false;
}

//
// string access
//
/*
bool stringAccess(cString& res, cString& in, double darg)
{
	res.Delete();

	int ix = (int) darg;

	if (ix<0 || ix >= (int) in.s.length()) {
		addError("index outside string dimensions"); return true;
	}

	char ch = in.s.at(ix);
	res.SetTo( ch );
	return false;
}

bool stringAccess(cString& res, cString& in, cMatrix& arg)
{
	res.Delete();

	int ix, n = arg.GetSize(), len = (int) in.s.length();
	double lf;

	res.s.reserve( n );

	for (int i=0; i<n; ++i)
	{
		arg.GetR(lf,i);
		ix = (int) lf;

		if (ix<0 || ix >= len) {
			addError("index outside string dimensions"); return true;
		}

		res.s.append( 1, in.s.at(ix) );
	}

	return false;
}
*/
