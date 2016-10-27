

#pragma once

#include <vector>
#include <string>
#include "lexTypes.h"

using std::vector;
using std::string;

#define FCN_HASH_CONST 30
#define FCN_HASH_SIZE 1000
#define FCN_HASH_LENGTH 128

//// function block struct
//typedef struct tagFcnBlock
//{
//	vector<sStatement> stmts;	// fcn block is a vector of stmts with a name, plus in and out args
//	int nLineOffset;	// line number corresponding to first line of block (=first line of declare stmt)
//	time_t t_lastParsed;		// time of last file edit
//	string fileName;			// file name (if a fcn or script)
//	string pathName;			// path containing this file
//	string name;				// function name
//	vector<tokData> inArgs;		// input arguments
//	vector<tokData> outArgs;	// function outputs
//} sBlock;


typedef vector<sBlock> VectorFcnBlock;
typedef VectorFcnBlock* VectorFcnBlockP;

typedef vector<sFileFcn> VectorFileFcn;
typedef VectorFileFcn* VectorFileFcnP;


class cFcnHash
{
public:
	cFcnHash() {
		m_table = NULL;
		initTable();
		m_ixix_last = 0;
		m_nCount = 0;
	}
	cFcnHash( const cFcnHash& fcnHash )
	{
		m_table = NULL;
		m_fileAssoc = NULL;
		initTable();
		
		// fcn table
		int fix, fixh, ix(FCN_HASH_SIZE+1), ixh, ixRef;
		sBlock b;
		if (getFirst(fix, fixh, ixRef))
			return;

		fcnHash.get(b, ix, ixh);
		insert( b );
		
		while (ix!=fix && ixh!=fixh) {
			getNext(ix, ixh, ixRef);
			fcnHash.get(b, ix, ixh);
			insert( b );
		}

	}
	cFcnHash operator=( const cFcnHash& fcnHash )
	{
		m_table = NULL;
		initTable();
		
		int fix, fixh, ix(FCN_HASH_SIZE+1), ixh, ixRef;
		sBlock b;
		if (getFirst(fix, fixh, ixRef))
			return *this;

		fcnHash.get(b, ix, ixh);
		insert( b );
		
		while (ix!=fix && ixh!=fixh) {
			getNext(ix, ixh, ixRef);
			fcnHash.get(b, ix, ixh);
			insert( b );
		}

		return *this;
	}
	~cFcnHash() {
		if (m_table != NULL)
			delete []m_table;
		if (m_fileAssoc != NULL)
			delete []m_fileAssoc;
	};

	void initTable(void);

	bool getAddr(int& ix, const char *pchName) const;				// get ix when nargin, nargout are unknown
	bool getAddr(int& ix, string& nName) const;					// get ix when nargin, nargout are unknown
	bool getAddr(int& ix, int& ixh, const char *pchName, int nargin, int nargout) const;	// get ix, ixh
	bool getAddr(int& ix, int& ixh, string& name, int nargin, int nargout) const;			// get ix, ixh
	bool getIndex(int ix, int& ixh, const char *pchName, int nargin, int nargout) const;	// given ix, get ixh
	bool getIndex(int ix, int& ixh, string& name, int nargin, int nargout) const;			// given ix, get ixh

	bool get(sBlock& b, int ix, int ixh);

	bool getFileAddr(int& ix, int& ixh, sBlock& b) const;			// get ix, ixh
	bool getFileAddr(int& ix, int& ixh, const char *pchName) const;	// get ix, ixh
	bool getFileAddr(int& ix, int& ixh, string& name) const;			// get ix, ixh
	bool getFileIndex(int ix, int& ixh, const char *pchName) const;	// given ix, get ixh
	bool getFileIndex(int ix, int& ixh, string& name) const;			// given ix, get ixh
	bool getAddrByFile(vector<int>& vix, vector<int>& vixh, string& fname) const;	// use m_fileAssoc table to get addr of all fcns in file fname

	bool findNext(int& ix, int& ixh, const char *pchName) const;
	bool findNext(int& ix, int& ixh, string& fcnName) const;

	bool lookup(int& ix, int& ixh, sBlock& t, char *pch, int nargin, int nargout) const;		// return address and data
	bool lookup(int& ix, int& ixh, sBlock& t, string& name, int nargin, int nargout) const;	// return address and data
	bool lookup(sBlock& t, string& name, int nargin, int nargout) const;						// return data only
	bool lookup(sBlock& t) const;																// return data only

	bool find(const char *pchName) const;				// check for existence (return true if not found)

	bool get(sBlock& b, int ix, int ixh) const;
	bool getFile(sFileFcn& f, int ix, int ixh) const;

	bool insert(sBlock& t);
	bool insert(sBlock *pt);
	void insertIx(int& ix);

	bool remove(string&, int nargin, int nargout);
	bool remove(sBlock& t);
	bool remove(int ix, int ixh);
	void removeIx(int& ix);

	//bool insertFile(sFileFcn& f);
	void insertFileIx(int& ix);
	bool insertFileAssoc(sBlock& b);		// add fcn block b to file association table (no action if already there)
	bool removeFileAssoc(sBlock& b);		// remove fcn block b from file association table (no action if not there)
	bool removeByFile(string& file);		// use m_fileAssoc table to delete all fcn entries found in file fname

	bool getFirst(int& ix, int& ixh);		// first entry in the table
	bool getFirst(int& ix, int& ixh, int& ixPrev) const;
	bool getNext(int& ix, int& ixh);		// next table entry after (ix,ixh)
	bool getNext(int& ix, int& ixh, int& ixPrev) const;

	bool getFileFirst(int& ix, int& ixh);		// first entry in the table
	bool getFileFirst(int& ix, int& ixh, int& ixPrev) const;
	bool getFileNext(int& ix, int& ixh);		// next table entry after (ix,ixh)
	bool getFileNext(int& ix, int& ixh, int& ixPrev) const;

	vector<string> getAll();

	bool getIndexAny(int ix, int& ixh, string& name) const;
	bool getAddrAny(int& ix, int& ixh, string& name) const;
	bool lookupAny(sBlock& t) const;
	bool lookupAny(sBlock& b, string& name) const;

	bool which(string& fullFile, string& name);

	void clear(void);
	void wipe(void);
	int count(void);

protected:
	bool hashFunc(int& ix, string& name) const;
	bool hashFunc(int& ix, const char *pchName) const;
	//bool hashFunc(int& ix, const char *pch, int n, int nargin, int nargout) const;
	bool hashFunc(int& ix, string& t, int nargin, int nargout) const;
	bool hashFunc(int& ix, const char *pt, int nargin, int nargout) const;

	bool hashFileFunc(int& ix, const char *pch, int n) const;
	bool hashFileFunc(int& ix, string& t) const;
	bool hashFileFunc(int& ix, const char *pt) const;

protected:
	VectorFcnBlockP *m_table;
	VectorFileFcnP *m_fileAssoc;
	vector<int> m_ix, m_ixf;			// vector of indices of non-empty table vectors
	int m_ixix_last, m_ixixf_last;		// last accessed entry in m_ix array
	int m_nCount;						// keep track of no. symbols in table

private:
	vector<VectorFcnBlock> m_tablePrimer;
	vector<VectorFileFcn> m_filePrimer;
};
