#pragma once

#include <string>
#include "variable.h"
#include "matrix.h"

using std::string;

class cString : public cVariable
{
public:
	cString() { m_nType = VAR_TYPE_STRING; }
	~cString() { Delete(); }
	cString( const cString& A ) { m_nType = VAR_TYPE_STRING; s = A.s; }

	// assignment operator
	bool operator=( const cString& S ) { m_name = S.m_name; m_nType = VAR_TYPE_STRING; s = S.s; return false; }
	bool operator=( const cString* pA ) { m_name = pA->m_name; m_nType = VAR_TYPE_STRING; s = pA->s; return false; }

	// Logical operators
	//
	bool operator==( const cString& S ) const { return s.compare(S.s) == 0; }
	bool operator!=(const cString& S) const { return s.compare(S.s) != 0; }
	bool operator<(const cString& S) const { return s.compare(S.s) < 0; }
	bool operator>(const cString& S) const { return s.compare(S.s) > 0; }
	bool operator<=(const cString& S) const { return s.compare(S.s) <= 0; }
	bool operator>=(const cString& S) const { return s.compare(S.s) >= 0; }

	// creation, deletion
	bool Delete(void) { bool bRet(false); if (s.size()!=0) {bRet = true;} s.clear(); return bRet; }
	void Empty(void) { s.clear(); }

	bool SetTo(cString& a) { s = a.s; return false; }
	bool SetTo(string& a) { s = a; return false; }
	bool SetTo(char ch) { s = ch; return false; }
	char Get(int i) {
		char ch = '\0';
		if (i>=0 && i<(int)s.length()) ch = s[i];
		return ch;
	}
	int GetType(void) { return VAR_TYPE_STRING; }
	int GetSize(void) const { return (int) s.length(); }

	// query
	bool IsEmpty() { return s.empty(); }

	string GetString(void) { return s; }

public:
	string s;
};


//
// prototypes
//

// File I/O
//
bool WriteData(cString& v, FILE *pf);
bool ReadData(cString& m, FILE *pf);

bool HorzCat(cString& Result, cString& v1, cString& v2);

//
// basic string search and manipulation
//
bool Length(cMatrix& res, cString& in);
bool Size(cMatrix& Result, cString& in);

bool FindMatch(cMatrix& res, cString& in, cString& pat);
bool FindNext(cMatrix& res, cString& in, cString& pat, cMatrix& beg);
bool FindPrev(cMatrix& res, cString& in, cString& pat, cMatrix& beg);

bool Replace(cString& res, cString& in, cString& pat, cString& rpat);

bool Substr(cString& res, cString& in, cMatrix& beg, cMatrix& len);
bool Substr(cString& res, cString& in, cMatrix& beg);

bool Insert(cString& res, cString& in, cString& arg, cMatrix& pos);

bool Append(cString& res, cString& s1, cString& s2);
bool Append(cString& res, cString& s1, cString& s2, cString& s3);
bool Append(cString& res, vector<cString>& vars);

bool ToLower(cString& res, cString& in);
bool ToUpper(cString& res, cString& in);

bool ToString(cString& res, cMatrix& in);
bool ToScalar(cMatrix& res, cString& in);

bool is_empty(cMatrix& res, cString& M);

//// string access functions
//bool stringAccess(cString& res, cString& in, double darg);
//bool stringAccess(cString& res, cString& in, cMatrix& arg);
