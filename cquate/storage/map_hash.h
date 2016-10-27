
#pragma once

#include <vector>
#include <string>
#include <map>
#include "container.h"

using std::vector;
using std::string;
using std::map;

#define TOK_HASH_LENGTH 2
#define HASH_CONST 26
#define HASH_SIZE 128	// 26*26

typedef vector<cContainer> VV;
typedef VV* VVP;

//typedef struct tagHashNode
//{
//	variable v;
//	hashNode *prev, *next;
//} hashNode;

class cHash
{
public:
	cHash() {
		initTable();
		m_nCount = 0;
	}
	cHash( const cHash& hash )
	{
		initTable();
		m_table_type = hash.m_table_type;
		m_table_matrix = hash.m_table_matrix;
		m_table_string = hash.m_table_string;
		m_table_hash = hash.m_table_hash;
	}
	cHash operator=( const cHash& hash )
	{
		initTable();
		m_table_type = hash.m_table_type;
		m_table_matrix = hash.m_table_matrix;
		m_table_string = hash.m_table_string;
		m_table_hash = hash.m_table_hash;

		return *this;
	}
	~cHash() {
	};

	void CopyMap(map<string,cContainer>& res, const map<string,cContainer>& t) const {
		map<string,cContainer>::const_iterator it;
		for (it = t.begin(); it != t.end(); ++it) {
			res[it->first] = cContainer(it->second);
			if (it->second.GetType()==VAR_TYPE_HASH) {		// recursion
				CopyMap( res[it->first].h.h, it->second.h.h );
			}
		}
	}

	void initTable(void);
	void reset(void);
	void setDefaults(void);

	bool lookupEmpty(cContainer& t) const;		// lookup value of variable with empty name string. returns true if no such var found.
	bool lookup(char *) const;
	bool lookup(string&) const;
	bool lookup(int& type, string& name) const;
	bool lookup(cContainer&,string&) const;
	bool lookup(cContainer&) const;
	bool lookupMat(cMatrix& m, string& name) const;

	// direct lookup
	bool get(cContainer& t, int ix, int ixh) const;

	bool getSize(int& nRows, int& nCols, int& nDepth, string name) const;


	//bool lookup(cVariable *pt, string& name) const;
	//bool lookup(cVariable *pt) const;
	//bool lookup(cVariable *pt, unsigned int& ixh, unsigned int ix) const;
	//bool lookup(cVariable *pt, unsigned int& ixh, string& name, unsigned int ix) const;

	bool lookupInfo(string& name, int& type, int& rows, int& columns, int& depth) const;
	bool lookupInfo(int& type, string& name) const;
	bool getSymbolList( vector<string>& names ) const;

	bool insert(cContainer& t);
	bool insert(cContainer *pt);
	bool insert(string& name, cMatrix& m);
	bool insert(string& name, cCube& c);
	bool insert(string& name, cString& s);
	void insertIx(int& ix);
	bool indexSet(cContainer& v, cContainer& vIdx);
	bool hashAssign(cContainer& v, cContainer& arg);

	bool hashGet(cContainer& res, cContainer& in, vector<string>& strArgs);
	bool hashGet(cContainer& v, string& name, vector<string>& strArgs);

	bool hashSet(cContainer& in, vector<string>& strArgs, cContainer& v);
	bool hashSet(string& name, vector<string>& strArgs, cContainer& v);

	bool remove(string&);
	bool remove(cContainer&);

	bool set(string& name, cContainer& t);
	bool set(string& name, double& dv);
	bool set(string& name, complex<double>* pcv);
	bool set(string& name, cMatrix& m);
	bool set(string& name, string& s);

	bool getFirst(int& ix, int& ixh);		// first entry in the table
	bool getFirst(int& ix, int& ixh, int& ixPrev) const;
	bool getNext(int& ix, int& ixh);		// next table entry after (ix,ixh)
	bool getNext(int& ix, int& ixh, int& ixPrev) const;

	// access operator (one arg)
	//
	bool matAccess(cMatrix& Result, string& name, double darg);
	bool matAccess(cMatrix& Result, string& name, cMatrix& arg);
	bool cubeAccess(cMatrix& Result, string& name, double darg);
	bool cubeAccess(cMatrix& Result, string& name, cMatrix& arg);
	bool stringAccess(cString& Result, string& name, double darg);
	bool stringAccess(cString& Result, string& name, cMatrix& arg);

	bool containerAccess(cContainer& c, string& name, double darg);
	bool containerAccess(cContainer& c, string& name, string& name1);
	bool containerAccess(cContainer& c, string& name, cContainer& arg);

	bool containerAccessColon(cContainer& c, string& name);

	// access operator (two arg)
	//
	bool matAccess(cMatrix& Result, string& name, double darg1, double darg2);
	bool matAccess(cMatrix& Result, string& name, cMatrix& arg1, double darg2);
	bool matAccess(cMatrix& Result, string& name, double darg1, cMatrix& arg2);
	bool matAccess(cMatrix& Result, string& name, cMatrix& arg1, cMatrix& arg2);

	bool matAccess(cMatrix& Result, string& name, string& name1, double darg2);
	bool matAccess(cMatrix& Result, string& name, string& name1, cMatrix& arg2);
	bool matAccess(cMatrix& Result, string& name, double darg1, string& name2);
	bool matAccess(cMatrix& Result, string& name, cMatrix& arg1, string& name2);
	bool matAccess(cMatrix& Result, string& name, string& name1, string& name2);

	bool containerAccess(cContainer& c, string& name, cContainer& arg1, double darg2);
	bool containerAccess(cContainer& c, string& name, cContainer& arg1, string& name2);
	bool containerAccess(cContainer& c, string& name, double darg1, cContainer& arg2);
	bool containerAccess(cContainer& c, string& name, string& name1, cContainer& arg2);
	bool containerAccess(cContainer& c, string& name, cContainer& arg1, cContainer& arg2);

	bool containerAccessColon(cContainer& c, string& name, bool b1, double darg2);
	bool containerAccessColon(cContainer& c, string& name, bool b1, cContainer& arg2);
	bool containerAccessColon(cContainer& c, string& name, bool b1, string& name2);
	bool containerAccessColon(cContainer& c, string& name, double darg1, bool b2);
	bool containerAccessColon(cContainer& c, string& name, cContainer& arg1, bool b2);
	bool containerAccessColon(cContainer& c, string& name, string& name1, bool b2);
	bool containerAccessColon(cContainer& c, string& name, bool b1, bool b2);

	// access operator (three arg)
	//
	bool cubeAccess(cMatrix& Result, string& name, double darg1, double darg2, double darg3);
	bool cubeAccess(cMatrix& Result, string& name, double darg1, double darg2, cMatrix& arg3);
	bool cubeAccess(cMatrix& Result, string& name, double darg1, cMatrix& arg2, double darg3);
	bool cubeAccess(cMatrix& Result, string& name, double darg1, cMatrix& arg2, cMatrix& arg3);
	bool cubeAccess(cMatrix& Result, string& name, cMatrix& arg1, double darg2, double darg3);
	bool cubeAccess(cMatrix& Result, string& name, cMatrix& arg1, double darg2, cMatrix& arg3);
	bool cubeAccess(cMatrix& Result, string& name, cMatrix& arg1, cMatrix& arg2, double darg3);
	bool cubeAccess(cCube& Result, string& name, cMatrix& arg1, cMatrix& arg2, cMatrix& arg3);

	bool containerAccess(cContainer& c, string& name, double darg1, double darg2, cContainer& arg3);
	bool containerAccess(cContainer& c, string& name, double darg1, cContainer& arg2, double darg3);
	bool containerAccess(cContainer& c, string& name, double darg1, cContainer& arg2, cContainer& arg3);
	bool containerAccess(cContainer& c, string& name, cContainer& arg1, double darg2, double darg3);
	bool containerAccess(cContainer& c, string& name, cContainer& arg1, double darg2, cContainer& arg3);
	bool containerAccess(cContainer& c, string& name, cContainer& arg1, cContainer& arg2, double darg3);
	bool containerAccess(cContainer& c, string& name, cContainer& arg1, cContainer& arg2, cContainer& arg3);

	bool containerAccess(cContainer& c, string& name, double darg1, double darg2, string& name3);
	bool containerAccess(cContainer& c, string& name, double darg1, string& name2, double darg3);
	bool containerAccess(cContainer& c, string& name, double darg1, string& name2, string& name3);
	bool containerAccess(cContainer& c, string& name, string& name1, double darg2, double darg3);
	bool containerAccess(cContainer& c, string& name, string& name1, double darg2, string& name3);
	bool containerAccess(cContainer& c, string& name, string& name1, string& name2, double darg3);
	bool containerAccess(cContainer& c, string& name, string& name1, string& name2, string& name3);

	bool containerAccess(cContainer& c, string& name, double darg1, cContainer& arg2, string& name3);
	bool containerAccess(cContainer& c, string& name, double darg1, string& name2, cContainer& arg3);
	bool containerAccess(cContainer& c, string& name, cContainer& arg1, double darg2, string& name3);
	bool containerAccess(cContainer& c, string& name, string& name1, double darg2, cContainer& arg3);
	bool containerAccess(cContainer& c, string& name, cContainer& arg1, string& name2, double darg3);
	bool containerAccess(cContainer& c, string& name, string& name1, cContainer& arg2, double darg3);
	bool containerAccess(cContainer& c, string& name, cContainer& arg1, cContainer& arg2, string& name3);

	bool containerAccess(cContainer& c, string& name, string& name1, string& name2, cContainer& arg3);

	// single colon
	bool containerAccessColon(cContainer& c, string& name, bool b1, double darg2, double darg3);
	bool containerAccessColon(cContainer& c, string& name, bool b1, double darg2, cContainer& arg3);
	bool containerAccessColon(cContainer& c, string& name, bool b1, cContainer& arg2, double darg3);
	bool containerAccessColon(cContainer& c, string& name, bool b1, cContainer& arg2, cContainer& arg3);
	bool containerAccessColon(cContainer& c, string& name, bool b1, double darg2, string& argName);
	bool containerAccessColon(cContainer& c, string& name, bool b1, cContainer arg2, string& argName);
	bool containerAccessColon(cContainer& c, string& name, bool b1, string& argName, double darg3);
	bool containerAccessColon(cContainer& c, string& name, bool b1, string& argName2, string& argName3);
	
	bool containerAccessColon(cContainer& c, string& name, double darg1, bool b2, double darg3);
	bool containerAccessColon(cContainer& c, string& name, double darg1, bool b2, cContainer& arg3);
	bool containerAccessColon(cContainer& c, string& name, cContainer& arg1, bool b2, double darg3);
	bool containerAccessColon(cContainer& c, string& name, cContainer& arg1, bool b2, cContainer& arg3);
	bool containerAccessColon(cContainer& c, string& name, double darg1, bool b2, string& argName);
	bool containerAccessColon(cContainer& c, string& name, string& argName, bool b2, double darg3);
	bool containerAccessColon(cContainer& c, string& name, string& argName1, bool b2, string& argName3);
	
	bool containerAccessColon(cContainer& c, string& name, double darg1, double darg2, bool b3);
	bool containerAccessColon(cContainer& c, string& name, double darg1, cContainer& arg2, bool b3);
	bool containerAccessColon(cContainer& c, string& name, cContainer& arg1, double darg2, bool b3);
	bool containerAccessColon(cContainer& c, string& name, cContainer& arg1, cContainer& arg2, bool b3);
	bool containerAccessColon(cContainer& c, string& name, double darg1, string& argName, bool b3);
	bool containerAccessColon(cContainer& c, string& name, string& argName, double darg2, bool b3);
	bool containerAccessColon(cContainer& c, string& name, string& argName1, string& argName2, bool b3);

	// two colons
	bool containerAccessColon(cContainer& c, string& name, bool b1, bool b2, double darg3);
	bool containerAccessColon(cContainer& c, string& name, bool b1, bool b2, cContainer& arg3);
	bool containerAccessColon(cContainer& c, string& name, bool b1, bool b2, string& name3);
	
	bool containerAccessColon(cContainer& c, string& name, bool b1, double darg2, bool b3);
	bool containerAccessColon(cContainer& c, string& name, bool b1, cContainer& arg2, bool b3);
	bool containerAccessColon(cContainer& c, string& name, bool b1, string& name2, bool b3);
	
	bool containerAccessColon(cContainer& c, string& name, double darg1, bool b2, bool b3);
	bool containerAccessColon(cContainer& c, string& name, cContainer& arg1, bool b2, bool b3);
	bool containerAccessColon(cContainer& c, string& name, string& name1, bool b2, bool b3);

	// three colons
	bool containerAccessColon(cContainer& c, string& name, bool b1, bool b2, bool b3);

	bool ixMatAssign( string& name, cMatrix& rvalue, cMatrix& idx );
	bool ixMatAssign( string& name, double drval, cMatrix& idx );
	bool ixMatAssign( string& name, complex<double> cx, cMatrix& idx );

	bool ixCubeAssign( string& name, cCube& rvalue, cCube& idx );
	bool ixCubeAssign( string& name, cMatrix& rvalue, cCube& idx );
	bool ixCubeAssign( string& name, double drval, cCube& idx );
	bool ixCubeAssign( string& name, complex<double> cx, cCube& idx );

void clear(void);
	int count(void);

protected:
	int m_nCount;		// keep track of no. symbols in table
	map<string, int> m_table_type;
	map<string, cMatrix> m_table_matrix;
	map<string, cCube> m_table_cube;
	map<string, cString> m_table_string;
	map<string, map<string,cContainer> > m_table_hash;
};
