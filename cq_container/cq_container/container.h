
#pragma once

#include <vector>
#include <map>
#include "c_string.h"
#include "matrix.h"
#include "cube.h"
#include "variable.h"
#include <stdio.h>
#include <assert.h>

using std::vector;
using std::map;


class cMyPair
{
public:
	cMyPair() { ix=0; narg = 0; nUnkown = 0; }
	~cMyPair() { }

	int narg;
	int ix;
	int nUnkown;		// for input args, = to fixed # of args when variable # additional are allowed
						// for output args, = 1 when unknown # of outputs are allowed
						// = 0 for all other cases (fixed # inputs and outputs)

	bool operator< (const cMyPair& pr) const {
		if (narg != pr.narg)
			return narg < pr.narg;

		if (ix != pr.ix)
			return ix < pr.ix;
		
		return false;
	}
};

class cContainer
{
public:
	class cUserHash : public cVariable
	{
	public:
		cUserHash() { m_nType = VAR_TYPE_HASH; }
		~cUserHash() { Delete(); }
		cUserHash( const cUserHash& A ) { m_nType = VAR_TYPE_HASH; h = A.h; }

		// assignment operator
		bool operator=( const cUserHash& H ) {
			m_nType = VAR_TYPE_HASH;
			h = H.h;
			m_name = H.GetName();
			CopyHashes(h,H);
			return false;
		}

		void CopyHashes( map<string,cContainer>& m, const cUserHash& t ) {
			map<string,cContainer>::const_iterator it;
			std::pair<string,cContainer> pr;
			cContainer lc;
			//cContainer hash_cont;
			//hash_cont.SetType(VAR_TYPE_HASH);
			for (it = t.h.begin(); it != t.h.end(); ++it) {
				if (it->second.GetType()==VAR_TYPE_HASH) {		// recursion
					pr = std::make_pair(it->first, it->second);
					m[pr.first] = pr.second;
					CopyHashes( m[pr.first].h.h, pr.second.h );
				}
			}
			//m_pos = h.end();
		}

		//bool operator=( const cUserHash* pH ) { m_nType = VAR_TYPE_HASH; h = pH->h; return false; }

		bool Delete() { h.clear(); /*m_pos = h.end();*/ return false; }
		void Empty() { Delete(); }

		// Logical operators
		// (none)

		// get-set
		bool Get(cContainer& C, string& key) {
			C = h[key];
			return false;
		}

		int GetSize() const { return (int) h.size(); }

		int GetType(void) { return VAR_TYPE_HASH; }
		int GetSize(void) { return (int) h.size(); }

		vector<string> GetKeys() {
			vector<string> keys;
			map<string,cContainer>::iterator it;
			for (it = h.begin(); it != h.end(); ++it) {
				keys.push_back( it->first );
			}
		}

		bool NextKey(string& next)
		{	// replayce input key next with key pointed to by (++it)
			map<string,cContainer>::iterator pos;
			if (h.empty()) {
				return true;	// hash is empty
			}

			if (next.empty()) {	// set next to first key
				pos = h.begin();
				next = pos->first;
				return false;
			}

			// next is non-empty, and h has items
			pos = h.find(next);
			if (pos != h.end()) {
				++pos;
				if (pos != h.end()) {
					next = pos->first;
				}
				else {
					next = "";	// input is last key in hash
				}
			}
			else {
				return true;	// next is not a key in h
			}
			return false;
		}

		bool Set(cContainer& C, string& key) {
			if (key.empty()) return true;		// do not allow empty keys
			h[key] = C;
			return false;
		}

		bool SetTo(cUserHash& a) { h = a.h; return false; }
		bool SetTo(map<string,cContainer>& a) { h = a; return false; }

		bool Erase(string& key) {
			if (h.erase(key) == 1) {
				return false;
				//m_pos = h.end();
			}
			else return true;		// key not found
		}

		bool IsEntry(cContainer& C, string& key) {
			std::map<string,cContainer>::iterator it = h.find(key);
			if (it == h.end()) return true;
			return false;
		}

		bool IsEmpty() { return h.empty(); }

	public:
		map<string,cContainer> h;
	};

	cContainer();
	cContainer(const cContainer& C);
	~cContainer();

	bool operator=(const cContainer& C)
	{
		Delete();
		SetType( C.GetType() );
		switch (C.GetType()) {
		case VAR_TYPE_NUMERIC:
			m = C.m;
			c.Delete();
			s.Delete();
			h.Delete();
			break;

		case VAR_TYPE_CUBE:
			c = C.c;
			m.Delete();
			s.Delete();
			h.Delete();
			break;

		case VAR_TYPE_STRING:
			s = C.s;
			m.Delete();
			c.Delete();
			h.Delete();
			break;

		case VAR_TYPE_HASH:
			h = C.h;
			m.Delete();
			c.Delete();
			s.Delete();

		case VAR_TYPE_FCN_PTR:
			break;

		default:
			return true;
		};

		return false;
	}

	void Delete() {
		m.Delete();
		c.Delete();
		s.Delete();
		h.Delete();
		SetType( -1 );

	}

	bool IsEmpty() {
		switch (GetType()) {
		case VAR_TYPE_NUMERIC:
			return m.IsEmpty();
		case VAR_TYPE_CUBE:
			return c.IsEmpty();
		case VAR_TYPE_STRING:
			return s.IsEmpty();
		case VAR_TYPE_HASH:
			return h.IsEmpty();
		default:
			return true;
		}
		return true;
	}

	bool IsScalar() {
		if (GetType()==VAR_TYPE_NUMERIC && !IsEmpty()) {
			return m.IsScalar();
		}
		else if (GetType()==VAR_TYPE_CUBE && !IsEmpty()) {
			return c.IsScalar();
		}
		return false;
	}

	bool IsReal() {
		if (GetType()==VAR_TYPE_NUMERIC) { return m.IsReal(); }
		else if (GetType()==VAR_TYPE_CUBE) { return c.IsReal(); }
		else return true;
	}

	void SetName(string& name) {
		m.m_name = name;
		c.m_name = name;
		s.m_name = name;
		h.m_name = name;
	}

	void SetName(const char *pch) {
		string name(pch);
		SetName(name);
	}

	void SetTo(cContainer& C) {

		SetType(C.GetType());

		switch (C.GetType()) {
		case VAR_TYPE_NUMERIC:
			m = C.m;
			c.Delete();
			s.Delete();
			h.Delete();
			break;

		case VAR_TYPE_CUBE:
			c = C.c;
			m.Delete();
			s.Delete();
			h.Delete();
			break;

		case VAR_TYPE_STRING:
			m.Delete();
			c.Delete();
			s = C.s;
			h.Delete();
			break;

		case VAR_TYPE_HASH:
			m.Delete();
			c.Delete();
			s.Delete();
			h = C.h;
			break;

		default:
			return;
		};
	}

	void SetTo(bool b) {
		m.SetTo( double(b) );
		c.Delete();
		s.Delete();
		h.Delete();
		SetType(VAR_TYPE_NUMERIC);
	}

	void SetTo(double f) {
		m.SetTo(f);
		c.Delete();
		s.Delete();
		h.Delete();
		SetType(VAR_TYPE_NUMERIC);
	}

	void SetTo(complex<double>& cx)
	{
		m.SetTo(cx);
		c.Delete();
		s.Delete();
		h.Delete();
		SetType(VAR_TYPE_NUMERIC);
	}

	void SetTo(cMatrix& mat)
	{
		m.SetTo(mat);
		c.Delete();
		s.Delete();
		h.Delete();
		SetType(VAR_TYPE_NUMERIC);
	}

	void SetTo(cCube& cbe)
	{
		c.SetTo(cbe);
		m.Delete();
		s.Delete();
		h.Delete();
		SetType(VAR_TYPE_CUBE);
	}

	void SetTo(string& ss) {
		m.Delete();
		c.Delete();
		s.SetTo(ss);
		h.Delete();
		SetType(VAR_TYPE_STRING);
	}
	
	void SetTo(cString& cs) {
		SetTo(cs.s); SetName(cs.m_name);
	}

	void SetTo(map<string,cContainer>& ah, string& name) {
		h.SetTo(ah); SetName(name);
		SetType(VAR_TYPE_HASH);
		m.Delete();
		c.Delete();
		s.Delete();
	}

	void SetTo(cUserHash& H) {
		h.SetTo(H.h); SetName(H.m_name);
		SetType(VAR_TYPE_HASH);
		m.Delete();
		c.Delete();
		s.Delete();
	}

	bool AddKeyVal(string& key, cContainer& value) {
		if (GetType()==VAR_TYPE_HASH) {
			h.h[key] = value;
		}
		else if (IsEmpty()) {
			Delete();
			SetType(VAR_TYPE_HASH);
			h.h[key] = value;
		}
		else return true;
		return false;
	}

	bool EraseKey(string& key) {
		if (GetType()==VAR_TYPE_HASH)
			h.h.erase(key);
		return false;
	}

	bool SetToRange( double dStart, double dEnd, double dIncr ) {
		s.Delete();
		c.Delete();
		h.Delete();
		SetType(VAR_TYPE_NUMERIC);
		return m.SetToRange(dStart, dEnd, dIncr);
	}

	int GetType() const {
		return m_nType;
	}

	void SetType(int type) {		// keep this fcn?
		m_nType = type;
	}

	int GetSize() const {
		switch (GetType()) {
		case VAR_TYPE_NUMERIC:
			return m.GetSize();

		case VAR_TYPE_CUBE:
			return c.GetSize();

		case VAR_TYPE_STRING:
			return s.GetSize();

		case VAR_TYPE_HASH:
			return h.GetSize();

		default:
			return 0;
		}
	}

	int GetRows() const {
		switch (GetType()) {
		case VAR_TYPE_NUMERIC:
			return m.GetRows();

		case VAR_TYPE_CUBE:
			return c.GetRows();

		case VAR_TYPE_STRING:
			return 1;

		default:
			return 0;
		}
	}

	int GetColumns() const {
		switch (GetType()) {
		case VAR_TYPE_NUMERIC:
			return m.GetColumns();

		case VAR_TYPE_CUBE:
			return c.GetColumns();

		case VAR_TYPE_STRING:
			return s.GetSize();

		default:
			return 0;
		}
	}

	int GetDepth() const {
		switch (GetType()) {
		case VAR_TYPE_NUMERIC:
			return 1;

		case VAR_TYPE_CUBE:
			return c.GetDepth();

		default:
			return 0;
		}
	}

	string GetName() {
		string nm;
		switch (GetType()) {
		case VAR_TYPE_NUMERIC:
			return m.getName();

		case VAR_TYPE_CUBE:
			return c.getName();

		case VAR_TYPE_STRING:
			return s.getName();

		case VAR_TYPE_HASH:
			return h.getName();

		default:
			return nm;
		}
	}

	bool GetSafe0(double& d) {
		if (GetType()==VAR_TYPE_NUMERIC || GetType()==VAR_TYPE_CUBE) {
			return m.GetSafe0(d);
		}
		return true;
	}

	bool GetSafeMat(cMatrix& mat) {
		if (GetType()==VAR_TYPE_NUMERIC) {
			mat = m;
		}
		else return true;
		return false;
	}
	bool GetSafeCube(cCube& cbe) {
		if (GetType()==VAR_TYPE_CUBE) {
			cbe = c;
		}
		else return true;
		return false;
	}
	bool GetSafeStr(cString& cstr) {
		if (GetType()==VAR_TYPE_STRING) {
			cstr = s;
		}
		else return true;
		return false;
	}
	bool GetSafeStr(string& str) {
		cString cs;
		if (GetSafeStr(cs)) { return true; }
		str = cs.s;
		return false;
	}
	bool GetSafeHash(map<string,cContainer>& hash) {
		if (GetType()==VAR_TYPE_HASH) {
			hash = h.h;
		}
		else return true;
		return false;
	}

	// carry out the indexed assignment from vright to vleft using indices vidx
	//
	bool ixAssign(cContainer& vright, cContainer& vidx)
	{
		SetType(VAR_TYPE_NUMERIC);
		if (vidx.GetType() != VAR_TYPE_NUMERIC || vright.GetType() != VAR_TYPE_NUMERIC) { addError("illegal argument"); return true; }
		if (!SameSize( vright.m, vidx.m )) { addError("size mismatch"); return true; }
		return ::ixAssign( m, vright.m, vidx.m );
	}

	bool operator==(cContainer& C) {
		if (IsEmpty() && C.IsEmpty()) return true;
		if (IsEmpty() || C.IsEmpty()) return false;
		if (GetType() != C.GetType()) return false;
		if (GetSize() != C.GetSize()) return false;

		// they are the same type and size...

		cMatrix mm;
		double lf;

		switch (GetType()) {
		case VAR_TYPE_NUMERIC:
			mm = m == C.m;
			lf = mm.StraightSumR();
			return lf == (double) m.GetSize();
			break;

		case VAR_TYPE_CUBE:
			mm = c == C.c;
			lf = mm.StraightSumR();
			return lf == (double) c.GetSize();
			break;

		case VAR_TYPE_STRING:
			return s == C.s;
			break;

		default:
			return true;
		};

		return false;
	}

public:
	int m_nType;
	cMatrix m;
	cCube c;
	cString s;
	cUserHash h;	// pointer to a hash
	//cFunctionPtr f;
};

typedef struct tagKeyData
{
	string name;
	string help;
	int nType;

	// fcns with cMatrix args
	//
	bool (*mat_0) (void);									// 0-argument functions
	bool (*mat_1) (cMatrix&);								// 1-argument functions
	bool (*mat_2) (cMatrix&,cMatrix&);						// 2-argument functions
	bool (*mat_3) (cMatrix&,cMatrix&,cMatrix&);				// 3-argument functions
	bool (*mat_4) (cMatrix&,cMatrix&,cMatrix&,cMatrix&);	// 4-argument functions

	bool (*mat_0i_no) (vector<cMatrix>&);							// n-argument functions (type 1)
	bool (*mat_1i_no) (vector<cMatrix>&,cMatrix&);
	bool (*mat_2i_no) (vector<cMatrix>&,cMatrix&,cMatrix&);
	bool (*mat_3i_no) (vector<cMatrix>&,cMatrix&,cMatrix&,cMatrix&);

	bool (*mat_ni_0o) (vector<cMatrix>&);
	bool (*mat_ni_1o) (cMatrix&, vector<cMatrix>&);					// n-argument functions (type 2)

	// fcns with cCube args (usually just output)
	//
	bool (*cbe_1) (cCube&);								// 1-argument functions
	bool (*cbe_2) (cCube&,cMatrix&);					// 2-argument functions
	bool (*cbe_3) (cCube&,cMatrix&,cMatrix&);			// 3-argument functions
	bool (*cbe_4) (cCube&,cMatrix&,cMatrix&,cMatrix&);	// 4-argument functions

	bool (*cbe_0i_no) (vector<cCube>&);							// n-argument functions (type 1)
	bool (*cbe_1i_no) (vector<cCube>&,cMatrix&);
	bool (*cbe_2i_no) (vector<cCube>&,cMatrix&,cMatrix&);
	bool (*cbe_3i_no) (vector<cCube>&,cMatrix&,cMatrix&,cMatrix&);

	bool (*cbe_ni_0o) (vector<cCube>&);
	bool (*cbe_ni_1o) (cCube&, vector<cMatrix>&);				// n-argument functions (type 2)

	bool (*cbe_cbe_2) (cCube&,cCube&);					// 2-argument functions
	bool (*cbe_cbe_3) (cCube&,cCube&,cCube&);			// 3-argument functions
	bool (*cbe_cbe_4) (cCube&,cCube&,cCube&,cCube&);	// 4-argument functions

	bool (*mat_cbe_2) (cMatrix&,cCube&);				// 2-argument functions
	bool (*mat_cbe_3) (cMatrix&,cCube&,cCube&);			// 3-argument functions

	// fcns with cString args
	//
	bool (*str_1) (cString&);								// 1-argument functions
	bool (*str_2) (cString&,cString&);						// 2-argument functions
	bool (*str_3) (cString&,cString&,cString&);				// 3-argument functions
	bool (*str_4) (cString&,cString&,cString&,cString&);	// 4-argument functions

	bool (*str_0i_no) (vector<cString>&);					// n-argument functions
	bool (*str_1i_no) (vector<cString>&,cString&);
	bool (*str_2i_no) (vector<cString>&,cString&,cString&);
	bool (*str_3i_no) (vector<cString>&,cString&,cString&,cString&);

	bool (*cbe_str) (cCube&, cString&);									//
	bool (*cbe_str2) (cCube&, cString&, cString&);
	bool (*cbe2_mat) (cCube&, cCube&, cMatrix&);						// rm_rows, rm_cols
	bool (*mat_str) (cMatrix&, cString&);								// length
	bool (*mat_str2) (cMatrix&, cString&, cString&);					// find functions
	bool (*mat2_str) (cMatrix&, cMatrix&, cString&);					// 
	bool (*mat_str2_mat) (cMatrix&, cString&, cString&, cMatrix&);		// find functions
	bool (*mat3_str) (cMatrix&, cMatrix&, cMatrix&, cString&);			// fread
	bool (*str_mat) (cString&, cMatrix&);								// to_string
	bool (*str2_mat2) (cString&, cString&, cMatrix&, cMatrix&);			// substr
	bool (*str2_mat) (cString&, cString&, cMatrix&);					// substr
	bool (*str3_mat) (cString&, cString&, cString&, cMatrix&);			// insert
	bool (*mat_hsh) (cMatrix&, cContainer::cUserHash&);
	bool (*hsh_str) (cContainer::cUserHash&, cString&);
	bool (*mat_hsh_str) (cMatrix&, cContainer::cUserHash&, cString&);
	bool (*str_hsh_str) (cString&, cContainer::cUserHash&, cString&);
	bool (*hsh2_str) (cContainer::cUserHash&, cContainer::cUserHash&, cString&);
	bool (*hsh2_str_mat) (cContainer::cUserHash&, cContainer::cUserHash&, cString&, cMatrix&);
	bool (*hsh2_str_cbe) (cContainer::cUserHash&, cContainer::cUserHash&, cString&, cCube&);
	bool (*hsh2_str_str) (cContainer::cUserHash&, cContainer::cUserHash&, cString&, cString&);
	bool (*hsh2_str_hsh) (cContainer::cUserHash&, cContainer::cUserHash&, cString&, cContainer::cUserHash&);

	bool (*str_ni_0o) (vector<cString>&);					// n-argument functions (type 1)
	bool (*str_ni_1o) (cString&, vector<cString>&);			// n-argument functions (type 2)

	// fcns with hash args
	//
	//bool (*hsh) (cContainer::cUserHash&);
	//bool (*hsh_2) (cContainer::cUserHash&, cContainer::cUserHash&);
	bool (*hsh_1) (cContainer::cUserHash&);

	bool (*c) (cContainer&);								// needed for show(.)
	bool (*cc) (cContainer&, cContainer&);					// needed for show(.)
	bool (*ccc) (cContainer&, cContainer&, cContainer&);	// needed for show(.)
	bool (*vc) (vector<cContainer>&);						// needed for show(.) and printf(.)

	bool (*sc) (cString&, cContainer&);							// needed for printf(.)
	bool (*scc) (cString&, cContainer&, cContainer&);			// needed for printf(.)

	bool (*svc) (cString&, vector<cContainer>&);

	// fcns to support FScanf (variable number of output args of undetermined type)
	bool (*cms) (cContainer&, cMatrix&, cString&);								// container, matrix, string
	bool (*ccms) (cContainer&, cContainer&, cMatrix&, cString&);				// c^2, matrix, string
	bool (*cccms) (cContainer&, cContainer&, cContainer&, cMatrix&, cString&);	// c^3, matrix, string
	bool (*vcms) (vector<cContainer>&, cMatrix&, cString&);	// c^3, matrix, string

	bool (*mmsvc) (cMatrix&, cMatrix&, cString&, vector<cContainer>&);	// c^3, matrix, string

	// fcns to support FPrintf (variable number of input args)
	bool (*msvc) (cMatrix&, cString&, vector<cContainer>&);

	// I/O map
	map<cMyPair,cMyPair> i2o;

} keyData;


//
// File I/O
//
bool WriteContainer(cContainer& C, FILE *pf);
bool ReadContainer(cContainer& C, FILE *pf);
//
bool WriteData(cContainer& C, FILE *pf);
bool ReadData(cContainer& C, FILE *pf);
//
bool WriteData(map<string,cContainer>& m, FILE *pf);
bool ReadData(map<string,cContainer>& m, FILE *pf);

//
// operator prototypes
//
bool EqualTo(cContainer& res, cContainer& c1, cContainer& c2);
bool EqualTo(cContainer& res, cContainer& c1, const complex<double>& x);
bool EqualTo(cContainer& res, cContainer& c1, const double d);
bool EqualTo(cContainer& res, const double d, cContainer& c1);
bool EqualTo(cContainer& res, const complex<double>& x, cContainer& c1);
//
bool NotEqualTo(cContainer& res, cContainer& c1, cContainer& c2);
bool NotEqualTo(cContainer& res, cContainer& c1, const complex<double>& x);
bool NotEqualTo(cContainer& res, cContainer& c1, const double d);
bool NotEqualTo(cContainer& res, const double d, cContainer& c1);
bool NotEqualTo(cContainer& res, const complex<double>& x, cContainer& c1);
//
bool LessThan(cContainer& res, cContainer& c1, cContainer& c2);
bool LessThan(cContainer& res, cContainer& c1, const complex<double>& x);
bool LessThan(cContainer& res, cContainer& c1, const double d);
bool LessThan(cContainer& res, const double d, cContainer& c1);
bool LessThan(cContainer& res, const complex<double>& x, cContainer& c1);
//
bool GreaterThan(cContainer& res, cContainer& c1, cContainer& c2);
bool GreaterThan(cContainer& res, cContainer& c1, const complex<double>& x);
bool GreaterThan(cContainer& res, cContainer& c1, const double d);
bool GreaterThan(cContainer& res, const double d, cContainer& c1);
bool GreaterThan(cContainer& res, const complex<double>& x, cContainer& c1);
//
bool LessEqualTo(cContainer& res, cContainer& c1, cContainer& c2);
bool LessEqualTo(cContainer& res, cContainer& c1, const complex<double>& x);
bool LessEqualTo(cContainer& res, cContainer& c1, const double d);
bool LessEqualTo(cContainer& res, const double d, cContainer& c1);
bool LessEqualTo(cContainer& res, const complex<double>& x, cContainer& c1);
//
bool GreaterEqualTo(cContainer& res, cContainer& c1, cContainer& c2);
bool GreaterEqualTo(cContainer& res, cContainer& c1, const complex<double>& x);
bool GreaterEqualTo(cContainer& res, cContainer& c1, const double d);
bool GreaterEqualTo(cContainer& res, const double d, cContainer& c1);
bool GreaterEqualTo(cContainer& res, const complex<double>& x, cContainer& c1);
//
bool And(cContainer& res, cContainer& c1, cContainer& c2);
bool And(cContainer& res, cContainer& c1, const complex<double>& x);
bool And(cContainer& res, cContainer& c1, const double d);
bool And(cContainer& res, const double d, cContainer& c1);
bool And(cContainer& res, const complex<double>& x, cContainer& c1);
//
bool Or(cContainer& res, cContainer& c1, cContainer& c2);
bool Or(cContainer& res, cContainer& c1, const complex<double>& x);
bool Or(cContainer& res, cContainer& c1, const double d);
bool Or(cContainer& res, const double d, cContainer& c1);
bool Or(cContainer& res, const complex<double>& x, cContainer& c1);


//
// single arg access (rvalue)
//
bool containerAccess(cContainer& c, double dvar, double darg);
//bool containerAccess(cContainer& c, cContainer& var, double darg);
bool containerAccess(cContainer& c, double darg, cContainer& arg);
//bool containerAccess(cContainer& c, cContainer& var, cContainer& arg);

bool containerAccessColon(cContainer& c, double dvar);
bool containerAccessColon(cContainer& c, cContainer& var);

//
// two arg access (rvalue)
//
bool containerAccess(cContainer& c, double dvar, double darg1, double darg2);
bool containerAccess(cContainer& c, double dvar, cContainer& arg1, double darg2);
bool containerAccess(cContainer& c, double dvar, double darg1, cContainer& arg2);
bool containerAccess(cContainer& c, double dvar, cContainer& arg1, cContainer& arg2);
//bool containerAccess(cContainer& c, cContainer& var, double darg1, double darg2);
//bool containerAccess(cContainer& c, cContainer& var, double darg1, cContainer& arg2);
//bool containerAccess(cContainer& c, cContainer& var, cContainer& arg1, double darg2);
//bool containerAccess(cContainer& c, cContainer& var, cContainer& arg1, cContainer& arg2);

bool containerAccessColon(cContainer& c, double dvar, bool b1, double darg2);
bool containerAccessColon(cContainer& c, double dvar, bool b1, cContainer& arg2);
bool containerAccessColon(cContainer& c, double dvar, double darg1, bool b2);
bool containerAccessColon(cContainer& c, double dvar, cContainer& arg1, bool b2);
//bool containerAccessColon(cContainer& c, cContainer& var, bool b1, double darg2);
//bool containerAccessColon(cContainer& c, cContainer& var, bool b1, cContainer& arg2);
//bool containerAccessColon(cContainer& c, cContainer& var, double darg1, bool b2);
//bool containerAccessColon(cContainer& c, cContainer& var, cContainer& arg1, bool b2);

bool containerAccessColon(cContainer& c, double dvar, bool b1, bool b2);
//bool containerAccessColon(cContainer& c, cContainer& var, bool b1, bool b2);

//
// three arg access
//
bool containerAccess(cContainer& v, double dvar, double darg1, double darg2, double darg3);
bool containerAccess(cContainer& v, double dvar, double darg1, double darg2, cContainer& arg3);
bool containerAccess(cContainer& v, double dvar, double darg1, cContainer& arg2, double darg3);
bool containerAccess(cContainer& v, double dvar, double darg1, cContainer& arg2, cContainer& arg3);
bool containerAccess(cContainer& v, double dvar, cContainer& arg1, double darg2, double darg3);
bool containerAccess(cContainer& v, double dvar, cContainer& arg1, double darg2, cContainer& arg3);
bool containerAccess(cContainer& v, double dvar, cContainer& arg1, cContainer& arg2, double darg3);
bool containerAccess(cContainer& v, double dvar, cContainer& arg1, cContainer& arg2, cContainer& arg3);
//bool containerAccess(cContainer& v, cContainer& var, double darg1, double darg2, double darg3);
//bool containerAccess(cContainer& v, cContainer& var, double darg1, double darg2, cContainer& arg3);
//bool containerAccess(cContainer& v, cContainer& var, double darg1, cContainer& arg2, double darg3);
//bool containerAccess(cContainer& v, cContainer& var, double darg1, cContainer& arg2, cContainer& arg3);
//bool containerAccess(cContainer& v, cContainer& var, cContainer& arg1, double darg2, double darg3);
//bool containerAccess(cContainer& v, cContainer& var, cContainer& arg1, double darg2, cContainer& arg3);
//bool containerAccess(cContainer& v, cContainer& var, cContainer& arg1, cContainer& arg2, double darg3);
//bool containerAccess(cContainer& v, cContainer& var, cContainer& arg1, cContainer& arg2, cContainer& arg3);

bool arg2ix( cContainer& Result, cContainer& arg1, cContainer& arg2, int nRows, int nCols );
bool arg2ix( cContainer& Result, cContainer& arg1, cContainer& arg2, cContainer& arg3, int nRows, int nCols, int nDepth );

// single colon
bool containerAccessColon(cContainer& c, double dvar, bool b1, double darg2, double darg3);
bool containerAccessColon(cContainer& c, double dvar, bool b1, double darg2, cContainer& arg3);
bool containerAccessColon(cContainer& c, double dvar, bool b1, cContainer& arg2, double darg3);
bool containerAccessColon(cContainer& c, double dvar, bool b1, cContainer& arg2, cContainer& arg3);

bool containerAccessColon(cContainer& c, double dvar, double darg1, bool b2, double darg3);
bool containerAccessColon(cContainer& c, double dvar, double darg1, bool b2, cContainer& arg3);
bool containerAccessColon(cContainer& c, double dvar, cContainer& arg1, bool b2, double darg3);
bool containerAccessColon(cContainer& c, double dvar, cContainer& arg1, bool b2, cContainer& arg3);

bool containerAccessColon(cContainer& c, double dvar, double darg1, double darg2, bool b3);
bool containerAccessColon(cContainer& c, double dvar, double darg1, cContainer& arg2, bool b3);
bool containerAccessColon(cContainer& c, double dvar, cContainer& arg1, double darg2, bool b3);
bool containerAccessColon(cContainer& c, double dvar, cContainer& arg1, cContainer& arg2, bool b3);

//bool containerAccessColon(cContainer& c, cContainer& var, bool b1, double darg2, double darg3);
//bool containerAccessColon(cContainer& c, cContainer& var, bool b1, double darg2, cContainer& arg3);
//bool containerAccessColon(cContainer& c, cContainer& var, bool b1, cContainer& arg2, double darg3);
//bool containerAccessColon(cContainer& c, cContainer& var, bool b1, cContainer& arg2, cContainer& arg3);
//
//bool containerAccessColon(cContainer& c, cContainer& var, double darg1, bool b2, double darg3);
//bool containerAccessColon(cContainer& c, cContainer& var, double darg1, bool b2, cContainer& arg3);
//bool containerAccessColon(cContainer& c, cContainer& var, cContainer& arg1, bool b2, double darg3);
//bool containerAccessColon(cContainer& c, cContainer& var, cContainer& arg1, bool b2, cContainer& arg3);
//
//bool containerAccessColon(cContainer& c, cContainer& var, double darg1, double darg2, bool b3);
//bool containerAccessColon(cContainer& c, cContainer& var, double darg1, cContainer& arg2, bool b3);
//bool containerAccessColon(cContainer& c, cContainer& var, cContainer& arg1, double darg2, bool b3);
//bool containerAccessColon(cContainer& c, cContainer& var, cContainer& arg1, cContainer& arg2, bool b3);

// two colons
bool containerAccessColon(cContainer& c, double dvar, bool b1, bool b2, double darg3);
bool containerAccessColon(cContainer& c, double dvar, bool b1, bool b2, cContainer& arg3);

bool containerAccessColon(cContainer& c, double dvar, bool b1, double darg2, bool b3);
bool containerAccessColon(cContainer& c, double dvar, bool b1, cContainer& arg2, bool b3);

bool containerAccessColon(cContainer& c, double dvar, double darg1, bool b2, bool b3);
bool containerAccessColon(cContainer& c, double dvar, cContainer& arg1, bool b2, bool b3);

//bool containerAccessColon(cContainer& c, cContainer& var, bool b1, bool b2, double darg3);
//bool containerAccessColon(cContainer& c, cContainer& var, bool b1, bool b2, cContainer& arg3);
//
//bool containerAccessColon(cContainer& c, cContainer& var, bool b1, double darg2, bool b3);
//bool containerAccessColon(cContainer& c, cContainer& var, bool b1, cContainer& arg2, bool b3);
//
//bool containerAccessColon(cContainer& c, cContainer& var, double darg1, bool b2, bool b3);
//bool containerAccessColon(cContainer& c, cContainer& var, cContainer& arg1, bool b2, bool b3);

// three colons
bool containerAccessColon(cContainer& c, double dvar, bool b1, bool b2, bool b3);
//bool containerAccessColon(cContainer& c, cContainer& var, bool b1, bool b2, bool b3);


bool SetToRange( cContainer& v, double dStart, double dIncr, double dEnd );
bool SetToRange( cContainer& v, double dStart, double dEnd );
bool SetToRange( cContainer& v, double d1, cContainer& v2 );
bool SetToRange( cContainer& v, cContainer& v1, double d2 );
bool SetToRange( cContainer& v, cContainer& v1, cContainer& v2 );
bool SetToRange( cContainer& v, double d1, double d2, cContainer& vinc );
bool SetToRange( cContainer& v, double d1, cContainer& v2, cContainer& vinc );
bool SetToRange( cContainer& v, cContainer& v1, double d2, cContainer& vinc );
bool SetToRange( cContainer& v, cContainer& v1, cContainer& v2, cContainer& vinc );
bool SetToRange( cContainer& v, double d1, cContainer& v2, double dinc );
bool SetToRange( cContainer& v, cContainer& v1, double d2, double dinc );
bool SetToRange( cContainer& v, cContainer& v1, cContainer& v2, double dinc );


//
// simple math
//
bool Add(cContainer& res, cContainer& a, cContainer& b);
bool Add(cContainer& res, cContainer& a, double b);
bool Sub(cContainer& res, cContainer& a, cContainer& b);
bool Sub(cContainer& res, cContainer& a, double b);
bool Sub(cContainer& res, double a, cContainer& b);
bool Mult(cContainer& res, cContainer& a, cContainer& b);
bool Mult(cContainer& res, cContainer& a, double b);
bool DotMult(cContainer& res, cContainer& a, cContainer& b);
bool DotMult(cContainer& res, cContainer& a, double b);
bool DotDiv(cContainer& res, cContainer& a, cContainer& b);
bool DotDiv(cContainer& res, cContainer& a, double b);
bool DotDiv(cContainer& res, double a, cContainer& b);
bool Mod(cContainer& res, cContainer& a, cContainer& b);
bool Mod(cContainer& res, double a, cContainer& b);
bool Mod(cContainer& res, cContainer& a, double b);
bool DotPow(cContainer& res, cContainer& a, cContainer& b);
bool DotPow(cContainer& res, double a, cContainer& b);
bool DotPow(cContainer& res, cContainer& a, double b);
//
bool Herm(cContainer& res, cContainer& a);
bool Not(cContainer& res, cContainer& a);
//
bool VertCat(cContainer& res, cContainer& a, cContainer& b);
bool VertCat(cContainer& res, double a, cContainer& b);
bool VertCat(cContainer& res, cContainer& a, double b);
bool VertCat(cContainer& res, double a, double b);
bool HorzCat(cContainer& res, cContainer& a, cContainer& b);
bool HorzCat(cContainer& res, double a, cContainer& b);
bool HorzCat(cContainer& res, cContainer& a, double b);
bool HorzCat(cContainer& res, double a, double b);

// hash functions
bool Get(cContainer& C, cContainer::cUserHash& hash, string& str);
bool Set(cContainer& C, cContainer::cUserHash& hash, string& key);
bool DeleteKey(cContainer::cUserHash& hash, cContainer::cUserHash& in, cString& key);
bool DeleteKey(cContainer::cUserHash& hash, string& key);
bool DeleteHash(cContainer::cUserHash& hash);
bool AddKey(cContainer::cUserHash& hash, cContainer::cUserHash& in, cString& key, cMatrix& value);
bool AddKey(cContainer::cUserHash& hash, cContainer::cUserHash& in, cString& key, cCube& value);
bool AddKey(cContainer::cUserHash& hash, cContainer::cUserHash& in, cString& key, cString& value);
bool AddKey(cContainer::cUserHash& hash, cContainer::cUserHash& in, cString& key, cContainer::cUserHash& value);
bool AddKey(cContainer& inout, cString& key, cContainer& value);
bool NextKey(cString& next, cContainer::cUserHash& hash, cString& start);
bool Retrieve(cContainer& res, cContainer& Chash, cContainer& Ckey);
bool Erase(cContainer& inout, cString& key);
bool ClearHash(cContainer& inout);

bool Length(cMatrix& Result, cContainer::cUserHash& h);
bool Size(cMatrix& Result, cContainer::cUserHash& h);

bool is_empty(cMatrix& res, cContainer::cUserHash& H);
bool is_key(cMatrix& res, cContainer::cUserHash& H, cString& key);
bool is_valid_field(cMatrix& res, cString& key);
bool NewHash(cContainer::cUserHash& H);


// call decoys (implemented in container.cpp)
bool fcall(cVariable& v, cContainer& c);
bool fcall(cVariable& v, cContainer& c1, cContainer& c2);

//
// machine learning package
//
bool knn_self(cContainer& resN, cMatrix& table, cMatrix& k);
bool knn_self(cContainer& resN, cContainer& resD, cMatrix& table, cMatrix& k);

bool knn_query(cContainer& resN, cMatrix& table, cMatrix& query, cMatrix& k);
bool knn_query(cContainer& resN, cContainer& resD, cMatrix& table, cMatrix& query, cMatrix& k);

bool kfn_self(cContainer& resN, cMatrix& table, cMatrix& k);
bool kfn_self(cContainer& resN, cContainer& resD, cMatrix& table, cMatrix& k);

bool kfn_query(cContainer& resN, cMatrix& table, cMatrix& query, cMatrix& k);
bool kfn_query(cContainer& resN, cContainer& resD, cMatrix& table, cMatrix& query, cMatrix& k);

bool krn_self(cContainer& resN, cMatrix& table, cMatrix& k, cMatrix& rmin, cMatrix& rmax);
bool krn_self(cContainer& resN, cContainer& resD, cMatrix& table, cMatrix& k, cMatrix& rmin, cMatrix& rmax);

bool krn_query(cContainer& resN, cMatrix& table, cMatrix& query, cMatrix& k, cMatrix& rmin, cMatrix& rmax);
bool krn_query(cContainer& resN, cContainer& resD, cMatrix& table, cMatrix& query, cMatrix& k, cMatrix& rmin, cMatrix& rmax);
// k-RN handlers
bool krn_self(cContainer& resN, vector<cMatrix>& inArgs);
bool krn_self(cContainer& resN, cContainer& resD, vector<cMatrix>& inArgs);
bool krn_query(cContainer& resN, vector<cMatrix>& inArgs);
bool krn_query(cContainer& resN, cContainer& resD, vector<cMatrix>& inArgs);
