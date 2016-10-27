
#include "container.h"
#include "my_errors.h"
#include <string>

using std::string;
using std::map;


//
// File I/O
//
bool WriteData(map<string,cContainer>& m, FILE *pf)
{
	int sz = m.size();
	int len;
	map<string,cContainer>::iterator it;

	if (fwrite( &sz, sizeof(int), 1, pf ) != 1) return true;

	for (it = m.begin(); it != m.end(); ++it) {
		len = it->first.size();
		if (fwrite( &len, sizeof(int), 1, pf ) != 1) return true;		// key length
		if (fwrite( it->first.data(), sizeof(char), len, pf ) != len) return true;	// key
		if (WriteData(it->second, pf)) return true;						// container
	}
	return false;
}

bool ReadData(map<string,cContainer>& m, FILE *pf)
{
	cContainer c;
	int sz, len;
	char *pch;
	string key;
	m.clear();

	if (fread( &sz, sizeof(int), 1, pf ) != 1) return true;		// size of map

	for (int i=0; i<sz; ++i) {
		if (fread( &len, sizeof(int), 1, pf ) != 1) return true;		// key length
		pch = new char[len+1];
		if (fread( pch, sizeof(char), len, pf ) != len) return true;	// key
		pch[len] = '\0';
		key = pch;
		delete []pch;

		if (ReadContainer(c, pf)) return true;

		m[key] = c;		// set key
		c.Delete();
	}
	return false;
}

bool Get(cContainer& C, cContainer::cUserHash& hash, string& str)
{
	C.Delete();
	C = hash.h[str];
	return false;
}

bool Set(cContainer& C, cContainer::cUserHash& hash, string& key)
{
	hash.h[key] = C;
	return false;
}

bool DeleteKey(cContainer::cUserHash& hash, cContainer::cUserHash& in, cString& key)
{
	hash = in;
	string ks = key.GetString();
	return DeleteKey(hash, ks);
}

bool DeleteKey(cContainer::cUserHash& hash, string& key)
{
	hash.h.erase( key );
	return false;
}

bool DeleteHash(cContainer::cUserHash& hash)
{
	hash.h.clear();
	return false;
}

bool AddKey(cContainer::cUserHash& hash, cContainer::cUserHash& in, cString& key, cMatrix& value)
{
	cContainer cv;
	cv.SetTo(value);
	hash = in;
	hash.h[key.s] = cv;
	return false;
}

bool AddKey(cContainer::cUserHash& hash, cContainer::cUserHash& in, cString& key, cCube& value)
{
	cContainer cv;
	cv.SetTo(value);
	hash = in;
	hash.h[key.s] = cv;
	return false;
}

bool AddKey(cContainer::cUserHash& hash, cContainer::cUserHash& in, cString& key, cString& value)
{
	cContainer cv;
	cv.SetTo(value);
	hash = in;
	hash.h[key.s] = cv;
	return false;
}

bool AddKey(cContainer::cUserHash& hash, cContainer::cUserHash& in, cString& key, cContainer::cUserHash& value)
{
	cContainer cv;
	cv.SetTo(value);
	hash = in;
	hash.h[key.s] = cv;
	return false;
}

bool AddKey(cContainer& inout, cString& key, cContainer& value)
{
	if (inout.GetType() != VAR_TYPE_HASH && !inout.IsEmpty()) { addError("invalid operation: symbol not a hash"); return true; }

	return inout.AddKeyVal(key.s, value);
}

bool NextKey(cString& next, cContainer::cUserHash& hash, cString& start)
{
	string ret = start.s;
	next.Delete();

	if (hash.NextKey(ret)) {
		addError("key not found in hash: ", ret);
		return true;
	}

	next.s = ret;
	return false;
}

bool Retrieve(cContainer& res, cContainer& Chash, cContainer& Ckey)
{
	if (Ckey.GetType() != VAR_TYPE_STRING) { addError("hash argument must be a string"); return true; }
	if (Chash.GetType() != VAR_TYPE_HASH && !Chash.IsEmpty()) { addError("invalid operation: symbol not a hash"); return true; }

	if (Chash.h.Get(res,Ckey.s.s)) return true;	// nothing there

	return false;
}

bool Erase(cContainer& inout, cString& key)
{
	if (inout.GetType() != VAR_TYPE_HASH && !inout.IsEmpty()) { addError("cannot erase a key from a non-hash"); return true; }
	return inout.EraseKey(key.s);
}

bool ClearHash(cContainer& inout)
{
	if (inout.GetType() != VAR_TYPE_HASH && !inout.IsEmpty()) { addError("cannot clear a non-hash"); return true; }
	inout.h.Delete();
	return false;
}


bool Length(cMatrix& Result, cContainer::cUserHash& h)
{
	Result.Create(1,1);
	Result.mr[0] = (double) h.GetSize();

	return false;
}

bool Size(cMatrix& Result, cContainer::cUserHash& h)
{
	return Length(Result, h);
}

bool is_empty(cMatrix& res, cContainer::cUserHash& H)
{
	res.Delete();
	if (H.IsEmpty())
		res.SetTo(1.);
	else
		res.SetTo(0.);
	return false;
}

bool is_key(cMatrix& res, cContainer::cUserHash& H, cString& key)
{
	res.Delete();

	if (key.IsEmpty()) {	// invalid key, therefore never valid
		res.SetTo(0.);
		return false;
	}

	map<string,cContainer>::iterator it = H.h.find( key.s );
	if (it == H.h.end()) {	// not found
		res.SetTo(0.);
	}
	else {		// found
		res.SetTo(1.);
	}
	return false;
}

bool is_valid_field(cMatrix& res, cString& key)
{
	int i, nal;
	bool b_;
	res.Delete();

	if (key.IsEmpty()) {		// empty str is not valid field
		res.SetTo(0.);
		return false;
	}

	if (!isalpha(key.s.at(0))) {
		res.SetTo(0.);
		return false;
	}

	for (i=1; i<(int)key.s.length(); ++i)
	{
		nal = isalnum(key.s.at(i));
		b_ = key.s.at(i) == '_';
		if (nal == 0 && key.s.at(i)!='_') {
			res.SetTo(0.);
			return false;
		}
	}

	res.SetTo(1.);
	return false;
}

bool NewHash(cContainer::cUserHash& H)
{
	return H.Delete();
}