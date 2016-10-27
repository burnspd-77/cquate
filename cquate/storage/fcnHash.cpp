
#include "fcnHash.h"
#include <string.h>

void cFcnHash::initTable()
{
	wipe();

	string s = "";
	sBlock b;
	b.fileName = b.pathName = s;

	sFileFcn f;
	f.fileName = f.pathName = s;

	int i;
	VectorFcnBlock vb;
	vb.push_back( b );

	VectorFileFcn vf;
	vf.push_back( f );

	// initialize primer
	for (i=0; i<FCN_HASH_SIZE; ++i) {
		m_tablePrimer.push_back( vb );
		m_filePrimer.push_back( vf );
	}

	// initialize table to primer
	m_table = new VectorFcnBlockP[FCN_HASH_SIZE];
	m_fileAssoc = new VectorFileFcnP[FCN_HASH_SIZE];

	for (i=0; i<FCN_HASH_SIZE; ++i) {
		m_table[i] = &m_tablePrimer[i];
		m_fileAssoc[i] = &m_filePrimer[i];
	}
}

// get ix when nargin, nargout are unknown
bool cFcnHash::getAddr(int& ix, const char *pchName) const
{
	if (strlen(pchName)==0)	return true;

	hashFunc(ix, pchName);
	int ixh = 0;
	return findNext(ix, ixh, pchName);
}

bool cFcnHash::getAddr(int& ix, string& name) const
{
	hashFunc(ix, name);
	int ixh = 0;
	return findNext(ix, ixh, name);
}

bool cFcnHash::findNext(int& ix, int& ixh, const char *pchName) const
{
	string entryName;
	for (int i=1; i<(int)m_table[ix]->size(); ++i)
	{
		entryName = m_table[ix]->at(i).name;
		if ( strcmp( pchName, entryName.c_str() )==0) { ixh = i; return false; }
	}

	return true;
}

bool cFcnHash::findNext(int& ix, int& ixh, string& fcnName) const
{
	string entryName;
	for (int i=1; i<(int)m_table[ix]->size(); ++i)
	{
		entryName = m_table[ix]->at(i).name;
		if ( fcnName.compare( entryName )==0) { ixh = i; return false; }
	}

	return true;
}

bool cFcnHash::getAddr(int& ix, int& ixh, const char *pchName, int nargin, int nargout) const
{
	if (strlen(pchName)==0)	return true;

	hashFunc(ix, pchName, nargin, nargout);
	return getIndex(ix, ixh, pchName, nargin, nargout);
}

bool cFcnHash::getAddr(int& ix, int& ixh, string& name, int nargin, int nargout) const
{
	if (name.empty())	return true;

	hashFunc(ix, name, nargin, nargout);
	return getIndex(ix, ixh, name, nargin, nargout);
}

bool cFcnHash::getAddrAny(int& ix, int& ixh, string& name) const
{
	if (name.empty())	return true;

	hashFunc(ix, name, 0, 0);
	return getIndexAny(ix, ixh, name);
}

bool cFcnHash::getIndex(int ix, int& ixh, const char *pchName, int nargin, int nargout) const
{
	if (m_table[ix]->empty())
		return true;		// no entry

	string entryName;
	for (int i=1; i<(int)m_table[ix]->size(); ++i)
	{
		entryName = m_table[ix]->at(i).name;
		if ( strcmp( pchName, entryName.c_str() )==0 &&
			m_table[ix]->at(i).inArgs.size()==nargin
			//m_table[ix]->at(i).outArgs.size()==nargout
			) { ixh = i; return false; }
	}

	return true;	// function 'pchName' not found
}

bool cFcnHash::getIndex(int ix, int& ixh, string& name, int nargin, int nargout) const
{
	if (m_table[ix]->empty())
		return true;		// no entry

	string entryName;
	for (int i=1; i<(int)m_table[ix]->size(); ++i)
	{
		entryName = m_table[ix]->at(i).name;
		if ( name.compare(entryName)==0 &&
			m_table[ix]->at(i).inArgs.size()==nargin
			//m_table[ix]->at(i).outArgs.size()==nargout
			) { ixh = i; return false; }
	}

	return true;	// function 'name' not found
}

bool cFcnHash::getIndexAny(int ix, int& ixh, string& name) const
{	// disregard nargin, nargout
	if (m_table[ix]->empty())
		return true;		// no entry

	string entryName;
	for (int i=1; i<(int)m_table[ix]->size(); ++i)
	{
		entryName = m_table[ix]->at(i).name;
		if ( name.compare(entryName)==0 ) {
			ixh = i; return false;
		}
	}

	return true;	// function 'name' not found
}

bool cFcnHash::get(sBlock& b, int ix, int ixh)
{
	if (int(m_table[ix]->size()) <= ixh)
		return true;

	b = m_table[ix]->at(ixh);
	return false;
}

bool cFcnHash::lookup(int& ix, int& ixh, sBlock& b, char *pch, int nargin, int nargout) const
{
	string s(pch);
	if ( getAddr(ix, ixh,  s, nargin, nargout) ) return true;
	b = m_table[ix]->at(ixh);
	return false;
}

bool cFcnHash::lookup(int& ix, int& ixh, sBlock& b, string& name, int nargin, int nargout) const
{
	if ( getAddr(ix, ixh,  name, nargin, nargout) ) return true;
	b = m_table[ix]->at(ixh);
	return false;
}

bool cFcnHash::lookup(sBlock& b, string& name, int nargin, int nargout) const
{
	int ix, ixh;
	if ( getAddr(ix, ixh,  name, nargin, nargout) ) return true;
	b = m_table[ix]->at(ixh);
	return false;
}

bool cFcnHash::lookup(sBlock& t) const
{
	return lookup(t, t.name, t.inArgs.size(), t.outArgs.size());
}

bool cFcnHash::lookupAny(sBlock& t) const		// disregard nargin
{
	return lookupAny(t, t.name);
}

bool cFcnHash::lookupAny(sBlock& b, string& name) const
{
	int ix, ixh;
	if ( getAddrAny(ix, ixh,  name) ) return true;
	b = m_table[ix]->at(ixh);
	return false;
}

bool cFcnHash::find(const char *pchName) const	// check for existence (return true if not found)
{
	sBlock t;
	string name = pchName;
	return lookup(t, name, 0, 0);
}

bool cFcnHash::get(sBlock& b, int ix, int ixh) const
{
	if (m_table[ix]->empty()) return true;
	if (ixh >= (int)m_table[ix]->size()) return true;

	b = m_table[ix]->at(ixh);
	return false;
}

bool cFcnHash::insert(sBlock& b)
{
	int ix, ixh;
	int nargin = b.inArgs.size(), nargout = b.outArgs.size();

	hashFunc(ix, b.name, nargin, nargout);

	if (getIndex(ix, ixh, b.name, nargin, nargout))
	{
		// new symbol
		m_table[ix]->push_back( b );
		++m_nCount;
		insertIx( ix );
	}
	else
	{
		// overwrite old symbol
		(m_table[ix])->at(ixh) = b;
	}

	insertFileAssoc( b );

	return false;
}

bool cFcnHash::insert(sBlock *pb)
{
	// NULL check
	if (pb==NULL) return true;

	int ix, ixh;
	int nargin = pb->inArgs.size(), nargout = pb->outArgs.size();

	hashFunc(ix, pb->name, nargin, nargout);

	if (getIndex(ix, ixh, pb->name, nargin, nargout))
	{
		// new symbol
		m_table[ix]->push_back( *pb );
		++m_nCount;
		insertIx( ix );
		return false;
	}
	else
	{
		// overwrite old symbol
		vector<sBlock>::iterator it = m_table[ix]->begin() + ixh;
		*it = *pb;
	}

	return false;
}

void cFcnHash::insertIx(int& ix)
{
	if (m_ix.empty()) { m_ix.push_back( ix ); return; }

	int i;

	for (i=0; i<(int)m_ix.size(); ++i)
	{
		if (ix==m_ix[i])
		{
			// already here, no change needed
			return;
		}
	}

	m_ix.push_back( ix );
}

bool cFcnHash::remove(sBlock& b)
{
	return remove(b.name, b.inArgs.size(), b.outArgs.size());
}

bool cFcnHash::remove(string& name, int nargin, int nargout)
{
	int ix, ixh;

	if (getAddr(ix, ixh, name, nargin, nargout)) return true;	// no such entry

	// remove entry and decrement m_nCount
	remove(ix, ixh);

	return false;
}

bool cFcnHash::remove(int ix, int ixh)
{
	// remove entry and decrement m_nCount
	vector<sBlock>::iterator it = m_table[ix]->begin();
	it += ixh;
	m_table[ix]->erase( it );

	// possibly remove entry from m_ix
	if (m_table[ix]->empty())	removeIx(ix);

	return false;
}

void cFcnHash::removeIx(int& ix)
{
	if (m_ix.empty())	return;

	int i;
	vector<int>::iterator it = m_ix.begin();

	for (i=0; i<(int)m_ix.size(); ++i)
	{
		if (ix==m_ix[i])
		{
			it += i;
			m_ix.erase( it );
			return;
		}
	}
}

bool cFcnHash::getFirst(int& ix, int& ixh)
{
	if (m_ix.empty())	return true;

	ix = m_ix[0];	m_ixix_last = ix;
	ixh = 0;

	return false;
}

bool cFcnHash::getFirst(int& ix, int& ixh, int& ixPrev) const
{
	if (m_ix.empty())	return true;

	ix = m_ix[0];	ixPrev = ix;
	ixh = 0;

	return false;
}

bool cFcnHash::getNext(int& ix, int& ixh)
{
	int n = m_table[ix]->size();
	if (ixh < (n-1))	{ ++ixh; return false; }

	int i;

	// go to next ix
	for (i=m_ixix_last; i<(int)m_ix.size(); ++i)
	{
		if (ix==m_ix[i])
		{
			if (i+1 < (int)m_ix.size())
			{
				m_ixix_last = i+1;
				ix = m_ix[i+1];
			}
			else
			{
				m_ixix_last = 0;
				ix = m_ix[0];
			}
			ixh = 0;

			return false;
		}
	}

	// did not find, keep looking
	for (i=0; i<=m_ixix_last; ++i)
	{
		if (ix==m_ix[i])
		{
			if (i+1 < (int)m_ix.size())
			{
				m_ixix_last = i+1;
				ix = m_ix[i+1];
			}
			else
			{
				m_ixix_last = 0;
				ix = m_ix[0];
			}
			ixh = 0;

			return false;
		}
	}

	// no symbols found!
	return true;
}

bool cFcnHash::getNext(int& ix, int& ixh, int& ixPrev) const
{
	int n = m_table[ix]->size();
	if (ixh < (n-1))	{ ++ixh; return false; }

	int i;

	// go to next ix
	for (i=ixPrev; i<(int)m_ix.size(); ++i)
	{
		if (ix==m_ix[i])
		{
			if (i+1 < (int)m_ix.size())
			{
				ixPrev = i+1;
				ix = m_ix[i+1];
			}
			else
			{
				ixPrev = 0;
				ix = m_ix[0];
			}
			ixh = 0;

			return false;
		}
	}

	// did not find, keep looking
	for (i=0; i<=ixPrev; ++i)
	{
		if (ix==m_ix[i])
		{
			if (i+1 < (int)m_ix.size())
			{
				ixPrev = i+1;
				ix = m_ix[i+1];
			}
			else
			{
				ixPrev = 0;
				ix = m_ix[0];
			}
			ixh = 0;

			return false;
		}
	}

	// no symbols found!
	return true;
}

void cFcnHash::clear()
{
	vector<sBlock>::iterator it;
	vector<sFileFcn>::iterator itf;

	for (int i=0; i<FCN_HASH_SIZE; ++i) {
		it = m_table[i]->begin() + 1;
		itf = m_fileAssoc[i]->begin() + 1;
		m_table[i]->erase(it, m_table[i]->end());
		m_fileAssoc[i]->erase(itf, m_fileAssoc[i]->end());
	}

	m_nCount = 0;
	m_ix.clear();
}

void cFcnHash::wipe()
{
	// clear entire hash table from memory, including primer
	if (m_table != NULL)
	{
		for (int i=0; i<FCN_HASH_SIZE; ++i) {
			m_table[i]->clear();
			m_fileAssoc[i]->clear();
		}
		delete []m_table;
		delete []m_fileAssoc;
		m_table = NULL;
		m_fileAssoc = NULL;
	}
	m_tablePrimer.clear();
	m_filePrimer.clear();

	m_nCount = 0;
	m_ix.clear();
}

int cFcnHash::count()
{
	return m_nCount;
}

//protected:
//bool cFcnHash::hashFunc(int& ix, const char *pch, int n, int nargin, int nargout) const
//{
//	ix = (pch[0]-'A')*FCN_HASH_CONST;
//	for (int i=1; i<n; ++i)
//		ix += pch[i];
//
//	//ix *= (1 + nargin * nargout);		// removing nargin, nargout dependence makes it easier to keep track
//										//  of fcns when these are unknown (parsing stage)
//
//	ix = ix % FCN_HASH_SIZE;
//	return false;
//}

bool cFcnHash::hashFunc(int& ix, string& name, int nargin, int nargout) const
{
	ix = (name.at(0)-'A')*FCN_HASH_CONST;
	for (int i=1; i<(int)name.length(); ++i)
		ix += name.at(i);

	//ix *= (1 + nargin * nargout);

	ix = ix % FCN_HASH_SIZE;
	return false;
}

bool cFcnHash::hashFunc(int& ix, const char *pchName, int nargin, int nargout) const
{
	int n = strlen(pchName);
	ix = (pchName[0]-'A')*FCN_HASH_CONST;
	for (int i=1; i<n; ++i)
		ix += pchName[i];

	//ix *= (1 + nargin * nargout);

	ix = ix % FCN_HASH_SIZE;
	return false;
}

bool cFcnHash::hashFunc(int& ix, string& name) const
{
	return hashFunc(ix, name, 0, 0);
}

bool cFcnHash::hashFunc(int& ix, const char *pchName) const
{
	return hashFunc(ix, pchName, 0, 0);
}


//
// file association support
//
bool cFcnHash::getFileAddr(int& ix, int& ixh, sBlock& b) const
{
	string fullFile = b.pathName + '\\';
	fullFile.append( b.fileName );
	return getFileAddr(ix, ixh, fullFile);
}

bool cFcnHash::getFileAddr(int& ix, int& ixh, const char *pchFileName) const
{
	if (strlen(pchFileName)==0)	return true;

	hashFileFunc(ix, pchFileName);
	return getFileIndex(ix, ixh, pchFileName);
}

bool cFcnHash::getFileAddr(int& ix, int& ixh, string& fileName) const
{
	if (fileName.empty())	return true;

	hashFileFunc(ix, fileName);
	return getFileIndex(ix, ixh, fileName);
}

bool cFcnHash::getFileIndex(int ix, int& ixh, const char *pchName) const
{
	if (m_fileAssoc[ix]->empty())
		return true;		// no entry

	string fullFile;
	for (int i=1; i<(int)m_fileAssoc[ix]->size(); ++i)
	{
		// get full file name
		fullFile = m_fileAssoc[ix]->at(i).pathName + '\\';
		fullFile.append( m_fileAssoc[ix]->at(i).fileName );

		if ( strcmp( pchName, fullFile.c_str() )==0) { ixh = i; return false; }
	}

	return true;	// function 'pchName' not found
}

bool cFcnHash::getFileIndex(int ix, int& ixh, string& fullName) const
{
	if (m_fileAssoc[ix]->empty())
		return true;		// no entry

	string fullFile;
	for (int i=1; i<(int)m_fileAssoc[ix]->size(); ++i)
	{
		// get full file name
		fullFile = m_fileAssoc[ix]->at(i).pathName + '\\';
		fullFile.append( m_fileAssoc[ix]->at(i).fileName );

		if ( fullName.compare(fullFile)==0) { ixh = i; return false; }
	}

	return true;	// function 'name' not found
}


bool cFcnHash::getAddrByFile(vector<int>& vix, vector<int>& vixh, string& fullFile) const
{
	// init output
	vix.clear();	vixh.clear();

	int ix, ixh;

	if ( getFileAddr(ix, ixh, fullFile) ) return true;

	sFileFcn f = m_fileAssoc[ix]->at(ixh);
	for (int i=0; i<(int)f.fcns.size(); ++i)
	{
		if (!getAddr(ix, ixh, f.fcns[i].name, f.fcns[i].nargin, f.fcns[i].nargout)) { vix.push_back(ix); vixh.push_back(ixh); }
	}
	return false;
}

bool cFcnHash::getFile(sFileFcn& f, int ix, int ixh) const
{
	if (m_fileAssoc[ix]->empty()) return true;
	if (ixh >= (int)m_fileAssoc[ix]->size()) return true;

	f = m_fileAssoc[ix]->at(ixh);
	return false;
}


// delete all fcn entries found in file fname
bool cFcnHash::removeByFile(string& fullName)
{
	// get address of all fcns in file
	vector<int> vix, vixh;
	if ( getAddrByFile(vix, vixh, fullName) ) return true;

	// delete fcn entries
	for (int i=0; i<(int)vix.size(); ++i) { remove(vix[i], vixh[i]); }

	return false;
}

bool cFcnHash::insertFileAssoc(sBlock& b)
{
	int ix, ixh;
	sFileFcn f, *pf;
	sFcnInfo fi;
	bool bFound = false;

	if (getFileAddr(ix, ixh, b))
	{	// new file
		f.fileName = b.fileName;
		f.pathName = b.pathName;
		fi.name = b.name;
		fi.nargin = b.inArgs.size();
		fi.nargout = b.outArgs.size();
		f.fcns.push_back( fi );
		m_fileAssoc[ix]->push_back( f );
	}
	else	// found the file
	{
		pf = &m_fileAssoc[ix]->at(ixh);
		for (int i=0; i<(int)pf->fcns.size(); ++i)
		{
			if (b.name.compare( pf->fcns[i].name )==0 &&
				b.inArgs.size()==pf->fcns[i].nargin &&
				b.outArgs.size()==pf->fcns[i].nargout ) {
					bFound = true; break; // fcn is already there, take no action
			}
		}
		if (!bFound) {	// new fcn found in file - add to list
			fi.name = b.name;
			fi.nargin = b.inArgs.size();
			fi.nargout = b.outArgs.size();
			pf->fcns.push_back( fi );
		}
	}
	return false;
}

//bool cFcnHash::insertFile(sFileFcn& f)
//{
//	int ix, ixh;
//	string fullFile = f.pathName + '\\';
//	fullFile.append( f.fileName );
//
//	hashFileFunc(ix, fullFile);
//
//	if (getFileIndex(ix, ixh, fullFile))
//	{
//		// new symbol
//		m_fileAssoc[ix]->push_back( f );
//		insertFileIx( ix );
//	}
//	else
//	{
//		// overwrite old symbol
//		(m_fileAssoc[ix])->at(ixh) = f;
//	}
//
//	insertFileAssoc( f );
//
//	return false;
//}

bool cFcnHash::removeFileAssoc(sBlock& b)
{
	int ix, ixh;
	sFileFcn f, *pf;
	sFcnInfo fi;
	bool bFound = false;
	int i;
	sBlock fb;
	int ixf, ixhf;

	vector<sFcnInfo>::iterator fit;

	if (!getFileAddr(ix, ixh, b))
	{	// found the file
		pf = &m_fileAssoc[ix]->at(ixh);
		for (i=0; i<(int)pf->fcns.size(); ++i)
		{
			if (lookup(ixf, ixhf, fb, pf->fcns[i].name, pf->fcns[i].nargin, pf->fcns[i].nargout))
				continue;

			remove(ixf, ixhf);
			//if (b.fileName == pf->fcns[i].
			//if (b.name.compare( pf->fcns[i].name )==0 &&
			//	b.inArgs.size()==pf->fcns[i].nargin &&
			//	b.outArgs.size()==pf->fcns[i].nargout ) {
			//		bFound = true; break; // fcn is already there, take no action
			//}
		}

		m_fileAssoc[ix]->erase( m_fileAssoc[ix]->begin() + ixh );
		//if (bFound) {	// fcn found in file - remove from list
			//fit = pf->fcns.begin() + i;
			//pf->fcns.erase( fit, fit+1 );	// just remove 1 record
		//}
	}
	return false;
}

// first entry in the table
bool cFcnHash::getFileFirst(int& ix, int& ixh)
{
	if (m_ixf.empty())	return true;

	ix = m_ixf[0];	m_ixixf_last = ix;
	ixh = 0;

	return false;
}

bool cFcnHash::getFileFirst(int& ix, int& ixh, int& ixPrev) const
{
	if (m_ixf.empty())	return true;

	ix = m_ixf[0];	ixPrev = ix;
	ixh = 0;

	return false;
}

// next table entry after (ix,ixh)
bool cFcnHash::getFileNext(int& ix, int& ixh)
{
	int n = m_fileAssoc[ix]->size();
	if (ixh < (n-1))	{ ++ixh; return false; }

	int i;

	// go to next ix
	for (i=m_ixixf_last; i<(int)m_ixf.size(); ++i)
	{
		if (ix==m_ixf[i])
		{
			if (i+1 < (int)m_ixf.size())
			{
				m_ixixf_last = i+1;
				ix = m_ixf[i+1];
			}
			else
			{
				m_ixixf_last = 0;
				ix = m_ixf[0];
			}
			ixh = 0;

			return false;
		}
	}

	// did not find, keep looking
	for (i=0; i<=m_ixixf_last; ++i)
	{
		if (ix==m_ixf[i])
		{
			if (i+1 < (int)m_ixf.size())
			{
				m_ixixf_last = i+1;
				ix = m_ixf[i+1];
			}
			else
			{
				m_ixixf_last = 0;
				ix = m_ixf[0];
			}
			ixh = 0;

			return false;
		}
	}

	// no symbols found!
	return true;
}

bool cFcnHash::getFileNext(int& ix, int& ixh, int& ixPrev) const
{
	int n = m_fileAssoc[ix]->size();
	if (ixh < (n-1))	{ ++ixh; return false; }

	int i;

	// go to next ix
	for (i=ixPrev; i<(int)m_ixf.size(); ++i)
	{
		if (ix==m_ixf[i])
		{
			if (i+1 < (int)m_ixf.size())
			{
				ixPrev = i+1;
				ix = m_ixf[i+1];
			}
			else
			{
				ixPrev = 0;
				ix = m_ixf[0];
			}
			ixh = 0;

			return false;
		}
	}

	// did not find, keep looking
	for (i=0; i<=ixPrev; ++i)
	{
		if (ix==m_ixf[i])
		{
			if (i+1 < (int)m_ixf.size())
			{
				ixPrev = i+1;
				ix = m_ixf[i+1];
			}
			else
			{
				ixPrev = 0;
				ix = m_ixf[0];
			}
			ixh = 0;

			return false;
		}
	}

	// no symbols found!
	return true;
}

bool cFcnHash::which(string& fullFile, string& name)
{
	fullFile.clear();
	sBlock b;
	b.name = name;

	char pch[64];

	if (!lookupAny(b)) {
		fullFile = b.pathName;
		fullFile.append("\\");
		fullFile.append( b.fileName );
		sprintf(pch,", line %d", b.nLineOffset+1);
		fullFile.append( pch );
		return false;
	}
	return true;
}

vector<string> cFcnHash::getAll()
{
	sBlock b;
	int ix, ixh;
	vector<string> fcns;

	char pch[64];

	if (getFirst(ix, ixh))
		return fcns;

	int ixFirst = ix;
	int ixhFirst = ixh;

	//get(b, ix, ixh);

	//fcns.push_back( b.name );
	//fcns.back().append(" (file ");
	//fcns.back().append( b.fileName );
	//fcns.back().append(")");

	while (!getNext(ix,ixh))
	{
		if (ix == ixFirst && ixh == ixhFirst)
			break;

		if (ixh == 0)
			continue;

		get(b, ix, ixh);

		fcns.push_back( b.name );
		fcns.back().append(" (file ");
		fcns.back().append( b.fileName );
		fcns.back().append(", line ");
		sprintf(pch,"%d", b.nLineOffset+1);
		fcns.back().append( pch );
		fcns.back().append(")");
	}

	return fcns;
}

bool cFcnHash::hashFileFunc(int& ix, const char *pch, int n) const
{
	ix = (pch[0]-'A')*FCN_HASH_CONST;
	for (int i=1; i<n; ++i)
		ix += pch[i];

	ix = ix % FCN_HASH_SIZE;
	return false;
}

bool cFcnHash::hashFileFunc(int& ix, string& fname) const
{
	ix = (fname.at(0)-'A')*FCN_HASH_CONST;
	for (int i=1; i<(int)fname.length(); ++i)
		ix += fname.at(i);

	ix = ix % FCN_HASH_SIZE;
	return false;
}

bool cFcnHash::hashFileFunc(int& ix, const char *pchName) const
{
	int n = strlen(pchName);
	ix = (pchName[0]-'A')*FCN_HASH_CONST;
	for (int i=1; i<n; ++i)
		ix += pchName[i];

	ix = ix % FCN_HASH_SIZE;
	return false;
}