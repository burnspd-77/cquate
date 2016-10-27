
#pragma once

#include "matrix.h"
#include "container.h"
#include "map_hash.h"
#include "defines.h"
#include <string>
#include "varFile.h"
#include<map>

using std::string;

// variable types
enum { ENUM_NUMERIC, ENUM_STRING };

static int GlobalFileIDCount;


class cVariableSpace
{
public:
	cVariableSpace()
	{
		setDefaults();
	};
	cVariableSpace( const cVariableSpace& varSpace )
	{
		// hash table of variables
		m_space = varSpace.m_space;

		// symbol list
		m_symbolList.clear();
		varSpace.GetSymbolList(m_symbolList);

		// hide list
		m_hideList = varSpace.m_hideList;
	}
	void operator=( const cVariableSpace& varSpace )
	{
		// hash table of variables
		m_space = varSpace.m_space;

		// symbol list
		m_symbolList.clear();
		varSpace.GetSymbolList(m_symbolList);

		// hide list
		m_hideList = varSpace.m_hideList;
	}
	~cVariableSpace() {
		m_space.clear();
	};

	void setDefaults(void);
	bool IsEmpty(void);
	void Clear(void);
	void Empty(void);
	//bool Lookup(unsigned int& Type, cMatrix *pM, string *pS, string& Symbol);
	bool lookupEmpty(cContainer& t) const;
	bool Lookup(cContainer& v, string& Symbol);
	bool LookupInfo(int &Type, int& rows, int& columns, string Symbol);
	bool LookupInfo(int& Type, int& rows, int& columns, int& depth, string Symbol);
	//bool LookupInfo(string Symbol);
	void Set(string& Symbol, double& A);
	void Set(string& Symbol, complex<double>& A);
	void Set(string& Symbol, cMatrix& A);
	void Set(string& Symbol, char *pch);
	void Set(string& Symbol, cString& str);
	void Set(string& Symbol, cContainer& A);
	bool Set(string& Dest, string& Src);		// set one symbol to another

	bool Remove(string& Symbol);
	bool IsDefinedSymbol(string& Symbol);
	int GetNumSymbols(void);

	vector<string>* GetSymbols(int nType);
	vector<string>* GetNumericSymbols(void);
	vector<string>* GetStringSymbols(void);

	int getFileIdCount(void) const		{ return GlobalFileIDCount; }
	vector<string>* GetSymbolList(void);
	void GetSymbolList(vector<string>& list) const;

	bool IsValidSymbolName(string& Str) const;

	bool Load(string& fname);
	bool LoadAscii(cMatrix& Result, string& fname);
	bool getFormatColumns(string& delim, int& ncols, const char *pch);
	bool str2cols( vector<string>& cols, string& delim, string& line );

	bool Save(string& fname);
	bool SaveVariable( FILE *pF, string& strVar );
	bool SaveVariableAscii( FILE *pF, string& strVar );
	bool SaveHashAscii( FILE *pF, string parentName, cContainer::cUserHash& h );
	bool Save(vector<string> *pVarArray, string& fname);
	bool SaveAscii(vector<string>& VarArray, string& fname);
	bool GetVarCounts( int *pN, int *iError, vector<string> *pVarArray );

	bool SavePreferences(string& fname);
	bool LoadPreferences(string& fname);
	void WriteBinaryHeader(FILE *pF);
	bool ReadBinaryHeader(FILE *pF, string& strApp, double& fVersion, int& iType);

	void TrimWhite( string& New, string Old );
	int TextLineLength(string& fname);
	int ScanLine( char *p_buffer );

	bool isHidden(string& name) const;

	cHash * getHashPtr(void);
	std::map<int,FILE*> * getFilesTable(void) { return m_pOpenFilesTable; }

public:
	vector<int> m_iFileIDs;
	vector<string> m_hideList;

	std::map<int,FILE*> * m_pOpenFilesTable;

protected:
	cHash m_space;
	vector<string> m_symbolList;

};
