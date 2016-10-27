// storage.h: interface for the storage class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include "matrix.h"
#include "variable.h"
#include "varFile.h"
#include "map_hash.h"
#include "scriptHash.h"
#include "fcnHash.h"
#include "defines.h"
//#include "plotTypes.h"
#include <string>
#include <vector>
#include "variableSpace.h"
#include "help_db.h"

using std::string;
using std::vector;

typedef struct tagFILE_ID {
	int iID;
	FILE *pFile;
} FILE_ID;




class cStorage  
{
private:

	bool m_bSafeMode;

	vector<cVariableSpace> m_Spaces;
	int m_ixStackPos;

	// keywords
	std::map<string, keyData> m_KeywordTable;				// keyword table (stored as a map)

	// system commands
	std::map<string, keyData> m_CommandTable;				// system commands

	// scripts (parsed code)
	cScriptHash m_ScriptTable;

	// functions (parsed code)
	cFcnHash m_FcnTable;

	// scratch space
	vector<string> SymbolList;

	// help system
	cHelpDb m_help;


public:
	cStorage();
	virtual ~cStorage();

	bool helpQuery(string& help_page, string& query);

	// version
	int m_iVersion;

	// let colormap be a public data member for now
	//ColorMap m_ColorMap;
	string m_ColorMapName;

	// program run directory
	string m_RunDir;

	void resetFcn( keyData& kd );
	void resetCmd( keyData& kd );

	int getIx(void);
	bool setIx(int ix);

	void SetRunDir(string& dir) { m_RunDir = dir; }

	void Empty();
	bool LookupEmpty(cContainer& v);
	bool Lookup(cContainer& v, string& Symbol);
	bool LookupInfo(int& Type, int& rows, int& columns, string& Symbol);
	bool LookupInfo(int& Type, int& rows, int& columns, int& depth, string& Symbol);
	bool LookupInfo(string& Symbol);
	bool LookupSize(int& Type, int& size, string& Symbol);
	void Set(string& Symbol, cMatrix& A);
	void Set(string& Symbol, char *pch);
	void Set(string& Symbol, cString& str);
	void Set(string& Symbol, cContainer& A);
	bool Set(string& Dest, string& Src);		// set one symbol to another
	bool Insert( vector<cContainer>& vars );
	bool Remove(string& Symbol);
	void Clear(void);
	bool IsDefinedSymbol(string& Symbol);
	int GetNumSymbols(void);

	cHash * getHashPtr(void);			// return pointer to hash of active space
	std::map<string, keyData> * getKeywordPtr(void);
	std::map<string, keyData> * getCommandPtr(void);
	cScriptHash * getScriptPtr(void);
	cFcnHash * getFcnPtr(void);

	void insertFcns( vector<keyData>& kds );
	void insertCmds( vector<keyData>& kds );
	void insertKeywords( vector<keyData>& kds );

	vector<string>* GetNumericSymbols(void);
	vector<string>* GetStringSymbols(void);

	vector<string>* GetSymbolList(void);

	bool Load(string& fname);
	bool LoadDataVis(string& fname);
	bool LoadAscii(cMatrix& Result, string& fname);
	bool Save(string& fname);
	bool Save(vector<string> *pVarArray, string& fname);
	bool SaveAscii(vector<string>& VarArray, string& fname);

	bool SavePreferences(string& fname);
	//bool LoadPreferences(string& fname);

	void TrimWhite( string& New, string Old );		// another rendition of CLemmaDoc's TrimWhite funct
	int TextLineLength(string& fname);
	int ScanLine( char *p_buffer );


	// m_Spaces array management
	bool Push(cVariableSpace& aSpace) {
		if (!IsFull()) {
			int iTop = GetUpperBound();
			m_Spaces.push_back( aSpace );
			m_ixStackPos = int(m_Spaces.size()) - 1;
			return false;
		}
		else {
			return true;
		}
	}
	void Pop() {
		assert( !IsEmpty() );
		int iTop = GetUpperBound();
		vector<cVariableSpace>::iterator it = m_Spaces.begin() + iTop;

		// pop stack
		m_Spaces.erase( it );
		m_ixStackPos = int(m_Spaces.size()) - 1;

	}
	cVariableSpace GetTop() {
		assert(!IsEmpty());
		int iTop = getIx();
		return m_Spaces[iTop];
	}
	bool IsEmpty() const { return (m_Spaces.size()==0); }
	bool IsFull() const {
		return false;		// infinite recursion loops will probably crash the application, hopefully not the whole computer (this is 2011)
	}
	cVariableSpace GetAt(int i) { return m_Spaces[i]; }
	int GetSize() { return (m_Spaces.size()); }
	int GetUpperBound() { return (m_Spaces.size()-1); }
};

