

#pragma once

#include <map>
#include <string>
#include "lexTypes.h"

using std::map;
using std::string;

#define SCRIPT_HASH_CONST 30
#define SCRIPT_HASH_SIZE 500
//#define SCRIPT_HASH_LENGTH 128

//// script block struct, from scriptHash.h
//typedef struct tagBlock
//{
//	vector<sStatement> stmts;	// a block is a vector of stmts
//	int nType;			// what kind of block (null (main), branch (if), loop (for, while), script, etc)?
//	int nLineOffset;	// line number corresponding to first line of block (=first line of block, if fcn or script)
//	time_t t_lastParsed;		// time of last file edit
//	string fileName;			// file name (if a fcn or script)
//	string pathName;			// path containing this file
//	string name;				// !!! this is string used as the source for the hash function !!!
//} sBlock;



class cScriptHash
{
public:
	cScriptHash() {
		initTable();
	}
	cScriptHash( const cScriptHash& scriptHash )
	{
		m_table = scriptHash.m_table;
		initTable();
	}
	cScriptHash operator=( const cScriptHash& scriptHash )
	{
		m_table = scriptHash.m_table;
		initTable();

		return *this;
	}
	~cScriptHash() {
	};

	void initTable(void);

	// get entry for a file name (return highest precedence)
	bool lookup(char *pchName) const;
	bool lookup(string& name) const;
	bool lookup(sBlock& t, char *pchName) const;
	bool lookup(sBlock& t, string& name) const;
	bool lookup(sBlock& t) const;

	bool insert(sBlock& t);				// will overwrite an entry if it has the SAME FILE NAME
	//bool insert(sBlock *pt);			// ..

	bool remove(string&);
	bool remove(sBlock& t);

	bool which(string& fullFile, string& name);

	vector<string> getAll();

	void clear(void);
	int count(void);

protected:
	map<string,sBlock> m_table;
};
