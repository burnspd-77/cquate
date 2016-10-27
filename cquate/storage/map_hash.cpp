
#include "map_hash.h"
#include <string.h>

void cHash::initTable()
{
	//wipe();
}

void cHash::setDefaults()
{
	//// Set pi, i, j
	//double dv = PI;
	//cContainer c;
	//c.SetTo(dv);		c.SetName("pi");
	//insert( c );
	////
	//complex<double> i(0.,1.);
	//c.SetTo( i );		c.SetName("i");
	//insert( c );
	////
	//c.SetName("j");
	//insert( c );

	//// true, false
	//dv = 1.;
	//c.SetTo( dv );	c.SetName("true");
	//insert( c );
	////
	//dv = 0.;
	//c.SetTo( dv );	c.SetName("false");
	//insert( c );
}

void cHash::reset()
{
	initTable();
	setDefaults();
}

bool cHash::lookupEmpty(cContainer& t) const
{
	string name;	// empty name string

	map<string,int>::const_iterator it = m_table_type.find(name);
	if (it==m_table_type.end())
		return true; // key not found

	cMatrix m;
	cString s;
	cCube c;
	map<string,cContainer> h;
	map<string,cMatrix>::const_iterator itm;
	map<string,cCube>::const_iterator itc;
	map<string,cString>::const_iterator its;
	map<string, map<string,cContainer> >::const_iterator ith;

	switch (it->second) {
	case VAR_TYPE_NUMERIC:
		itm = m_table_matrix.find(name);
		if (itm==m_table_matrix.end()) {
			addError("internal error: missing matrix");
			return true; // key not found
		}
		m = itm->second;
		t.SetTo( m );
		t.SetName(name);
		break;

	case VAR_TYPE_CUBE:
		itc = m_table_cube.find(name);
		if (itc==m_table_cube.end()) {
			addError("internal error: missing matrix");
			return true; // key not found
		}
		c = itc->second;
		t.SetTo( c );
		t.SetName(name);
		break;

	case VAR_TYPE_STRING:
		its = m_table_string.find(name);
		if (its==m_table_string.end()) {
			addError("internal error: missing string");
			return true; // key not found
		}
		s = its->second;
		t.SetTo( s );
		t.SetName(name);
		break;

	case VAR_TYPE_HASH:
		ith = m_table_hash.find(name);
		if (ith==m_table_hash.end()) {
			addError("internal error: missing hash");
			return true; // key not found
		}
		h = ith->second;
		t.SetTo( h, name );
		CopyMap(t.h.h, ith->second );
		t.SetName(name);
		break;

	default:
		addError("internal error: unkown var type");
		return true;		// unkown type!
	}

	return false;
}

bool cHash::lookup(char *pchName) const
{
	// check existence
	int len = strlen(pchName);
	if (len==0) return true;

	string name = pchName;

	if (m_table_type.find(name)==m_table_type.end())
		return true; // key not found

	return false;
}

bool cHash::lookup(string& name) const
{
	// check existence
	if (name.empty())	return true;

	const map<string,int>::const_iterator it = m_table_type.find(name);
	if (it==m_table_type.end())
		return true; // key not found

	return false;
}

bool cHash::lookup(int& type, string& name) const
{
	type = -1;

	// check existence
	if (name.empty())	return true;

	const map<string,int>::const_iterator it = m_table_type.find(name);
	if (it==m_table_type.end())
		return true; // key not found

	type = it->second;

	return false;
}

bool cHash::lookup(cContainer& t, string& name) const
{
	if (name.empty())	return true;

	map<string,int>::const_iterator it = m_table_type.find(name);
	if (it==m_table_type.end())
		return true; // key not found

	cMatrix m;
	cString s;
	cCube c;
	map<string,cContainer> h;
	map<string,cMatrix>::const_iterator itm;
	map<string,cCube>::const_iterator itc;
	map<string,cString>::const_iterator its;
	map<string, map<string,cContainer> >::const_iterator ith;

	switch (it->second) {
	case VAR_TYPE_NUMERIC:
		itm = m_table_matrix.find(name);
		if (itm==m_table_matrix.end()) {
			addError("internal error: missing matrix");
			return true; // key not found
		}
		m = itm->second;
		t.SetTo( m );
		t.SetName(name);
		break;

	case VAR_TYPE_CUBE:
		itc = m_table_cube.find(name);
		if (itc==m_table_cube.end()) {
			addError("internal error: missing matrix");
			return true; // key not found
		}
		c = itc->second;
		t.SetTo( c );
		t.SetName(name);
		break;

	case VAR_TYPE_STRING:
		its = m_table_string.find(name);
		if (its==m_table_string.end()) {
			addError("internal error: missing string");
			return true; // key not found
		}
		s = its->second;
		t.SetTo( s );
		t.SetName(name);
		break;

	case VAR_TYPE_HASH:
		ith = m_table_hash.find(name);
		if (ith==m_table_hash.end()) {
			addError("internal error: missing hash");
			return true; // key not found
		}
		h = ith->second;
		t.SetTo( h, name );
		CopyMap(t.h.h, ith->second );
		t.SetName(name);
		break;

	default:
		addError("internal error: unkown var type");
		return true;		// unkown type!
	}

	return false;
}

bool cHash::lookup(cContainer& t) const
{
	string entryName = t.GetName();
	return lookup(t, entryName);
}

bool cHash::lookupMat(cMatrix& m, string& name) const
{
	if (name.empty())	return true;

	map<string,cMatrix>::const_iterator it = m_table_matrix.find(name);
	if (it==m_table_matrix.end())
		return true; // key not found

	m = it->second;
	return false;
}

bool cHash::lookupInfo(int& type, string& name) const
{
	if (name.empty())	return true;

	map<string,int>::const_iterator it = m_table_type.find(name);
	if (it==m_table_type.end())
		return true; // key not found

	type = it->second;

	return false;
}

bool cHash::lookupInfo(string& name, int& nType, int& rows, int& columns, int& depth) const
{
	depth = 1;

	if (lookupInfo(nType,name)) {
		return true;
	}
	
	if (nType==VAR_TYPE_STRING) {
		map<string, cString>::const_iterator its = m_table_string.find(name);
		if (its != m_table_string.end()) {
			rows = 1;
			columns = its->second.GetSize();
		}
	}
	else if (nType==VAR_TYPE_HASH) {
		map<string, map<string,cContainer> >::const_iterator ith = m_table_hash.find(name);
		if (ith != m_table_hash.end()) {
			columns = depth = 1;
			rows = ith->second.size();
		}
	}
	else if (nType==VAR_TYPE_NUMERIC) {
		map<string, cMatrix>::const_iterator itm = m_table_matrix.find(name);
		if (itm != m_table_matrix.end()) {
			rows = itm->second.GetRows();
			columns = itm->second.GetColumns();
		}
	}
	else if (nType==VAR_TYPE_CUBE) {
		map<string, cCube>::const_iterator itc = m_table_cube.find(name);
		rows = itc->second.GetRows();
		columns = itc->second.GetColumns();
		depth = itc->second.GetDepth();
	}

	return false;
}

bool cHash::getSymbolList( vector<string>& names ) const
{
	names.clear();
	map<string,int>::const_iterator it = m_table_type.begin();

	while (it != m_table_type.end())
	{
		if (!it->first.empty())
			names.push_back( it->first );

		++it;
	}
	return false;
}

bool cHash::insert(cContainer& t)
{
	string name = t.GetName();
	if (name.empty()) return true;

	int type = t.GetType();
	m_table_type[t.GetName()] = type;

	cMatrix m;
	cCube c;
	cString s;
	map<string,cContainer> h;

	switch (type) {
	case VAR_TYPE_NUMERIC:
		t.GetSafeMat(m);
		m_table_matrix[name] = m;
		break;

	case VAR_TYPE_CUBE:
		t.GetSafeCube(c);
		m_table_cube[name] = c;
		break;

	case VAR_TYPE_STRING:
		t.GetSafeStr(s);
		m_table_string[name] = s;
		break;
		
	case VAR_TYPE_HASH:
		t.GetSafeHash(h);
		m_table_hash[name] = h;
		break;

	default:
		addError("internal error: unknown var type");
		return true;
	}

	return false;
}

bool cHash::insert(cContainer *pt)
{
	// NULL check
	if (pt==NULL) return true;

	return insert(*pt);
}

bool cHash::insert(string& name, cMatrix& m)
{
	remove(name);
	m_table_type[name] = VAR_TYPE_NUMERIC;
	m_table_matrix[name] = m;
	return false;
}

bool cHash::insert(string& name, cCube& c)
{
	remove(name);
	m_table_type[name] = VAR_TYPE_CUBE;
	m_table_cube[name] = c;
	return false;
}

bool cHash::insert(string& name, cString& s)
{
	remove(name);
	m_table_type[name] = VAR_TYPE_STRING;
	m_table_string[name] = s;
	return false;
}


bool cHash::indexSet(cContainer& v, cContainer& vIdx)
{	// set elements of variable v.m_name pointed to by indices vIdx
	// equal to the values in v.

	cMatrix mres;
	cCube cbe;
	cContainer ctn;
	int nType;
	string varName = v.GetName();

	if ( vIdx.GetType() == VAR_TYPE_NUMERIC && v.GetType() == VAR_TYPE_NUMERIC )
	{
		if (!v.IsScalar() && !vIdx.IsScalar() && !SameSize( v.m, vIdx.m )) { addError("size mismatch"); return true; }

		if (lookupInfo(nType, varName)) { addError("bad argument"); return true; }

		if (nType == VAR_TYPE_NUMERIC) {
			if ( ixMatAssign( varName, v.m, vIdx.m ) ) return true;
		}
		else {
			if ( ixCubeAssign( varName, v.c, vIdx.c ) ) return true;
			m_table_cube[v.GetName()] = ctn.c;
		}
	}
	else if  (vIdx.GetType() == VAR_TYPE_CUBE && v.GetType() == VAR_TYPE_NUMERIC)
	{
		if (!v.IsScalar() && !vIdx.IsScalar() && !SameSize( vIdx.c, v.m )) { addError("size mismatch"); return true; }

		if (lookupInfo(nType, varName)) { addError("bad argument"); return true; }

		if (nType == VAR_TYPE_CUBE) {
			if ( ixCubeAssign( varName, v.m, vIdx.c ) ) return true;
		}
		else {
			addError("bad argument"); return true;
			//if ( ixCubeAssign( varName, v.m, vIdx.c ) ) return true;
		}
	}
	else if  (vIdx.GetType() == VAR_TYPE_CUBE && v.GetType() == VAR_TYPE_CUBE)
	{
		if (!v.IsScalar() && !vIdx.IsScalar() && !SameSize( v.c, vIdx.c )) { addError("size mismatch"); return true; }

		if (lookupInfo(nType, varName)) { addError("bad argument"); return true; }

		if (nType != VAR_TYPE_CUBE) { addError("bad argument"); return true; }

		if ( ixCubeAssign( varName, v.c, vIdx.c ) ) return true;
	}
	else { addError("illegal argument"); return true; }

	return false;
}


bool cHash::ixMatAssign( string& name, cMatrix& rvalue, cMatrix& idx )
{
	// rvalue and idx must be the same size (assumed already checked)
	// indices indicated by idx MUST be valid for lvalue (assumed already converted
	//  to absolute column-wise coodinate)

	complex<double> cx;
	if (rvalue.IsScalar()) {		// copy scalar value to every position idx
		if (rvalue.IsReal()) return ixMatAssign( name, rvalue.Get0R(), idx );
		else { rvalue.Get0(cx); return ixMatAssign( name, cx, idx ); }
	}

	map<string, cMatrix>::iterator it = m_table_matrix.find(name);
	if (it == m_table_matrix.end()) {		// not found
		return true;
	}

	int lsize = it->second.GetSize();
	int n = idx.GetSize();

	//int lsize = lvalue.GetSize();
	//int n = idx.GetSize();

	bool bcc(false);
	int rows = it->second.GetRows();
	int cols = it->second.GetColumns();

	// check complex-ness
	if (!rvalue.IsReal() && it->second.IsReal()) {
		if ( it->second.CreateImaginary(rows, cols) ) return true;	// out of memory
		bcc = true;
	}
	else if (!rvalue.IsReal() && !it->second.IsReal()) {
		bcc = true;
	}

	int i, ix;

	for (i=0; i<n; ++i) {
		// get index
		//ix = (int) pix[i];
		ix = (int) idx.mr[i];
		// check bounds
		if (ix<0 || ix>=lsize) return true;
		// assign to lvalue
		it->second.mr[ix] = rvalue.mr[i];
		if (bcc) it->second.mi[ix] = rvalue.mi[i];
	}

	return false;
}

bool cHash::ixMatAssign( string& name, double drval, cMatrix& idx )
{
	map<string, cMatrix>::iterator it = m_table_matrix.find(name);
	if (it == m_table_matrix.end()) {		// not found
		return true;
	}

	int lsize = it->second.GetSize();
	int n = idx.GetSize();
	int i, ix;

	bool bc = !it->second.IsReal();

	for (i=0; i<n; ++i) {
		// get index
		ix = (int) idx.mr[i];
		// check bounds
		if (ix<0 || ix>=lsize) return true;
		// assign to lvalue
		it->second.mr[ix] = drval;
		if (bc) it->second.mi[ix] = 0.;
	}

	return false;
}

bool cHash::ixMatAssign( string& name, complex<double> cx, cMatrix& idx )
{
	map<string, cMatrix>::iterator it = m_table_matrix.find(name);
	if (it == m_table_matrix.end()) {		// not found
		return true;
	}

	int lsize = it->second.GetSize();
	int n = idx.GetSize();
	int i, ix;

	int rows = it->second.GetRows();
	int cols = it->second.GetColumns();

	if ( it->second.IsReal() ) {
		if ( it->second.CreateImaginary(rows, cols) ) return true;
	}

	for (i=0; i<n; ++i) {
		// get index
		ix = (int) idx.mr[i];
		// check bounds
		if (ix<0 || ix>=lsize) return true;
		// assign to lvalue
		it->second.mr[ix] = cx.real();
		it->second.mi[ix] = cx.imag();
	}

	return false;
}


bool cHash::ixCubeAssign( string& name, cCube& rvalue, cCube& idx )
{
	// rvalue and idx must be the same size (assumed already checked)
	// indices indicated by idx MUST be valid for lvalue (assumed already converted
	//  to absolute column-wise coodinate)

	complex<double> cx;
	if (rvalue.IsScalar()) {		// copy scalar value to every position idx
		if (rvalue.IsReal()) return ixCubeAssign( name, rvalue.Get0R(), idx );
		else { rvalue.Get0(cx); return ixCubeAssign( name, cx, idx ); }
	}

	map<string, cCube>::iterator it = m_table_cube.find(name);
	if (it == m_table_cube.end()) {		// not found
		return true;
	}

	int lsize = it->second.GetSize();
	int n = idx.GetSize();

	int nleft = it->second.GetSize();
	bool bcc(false);

	// check complex-ness
	if (!rvalue.IsReal() && it->second.IsReal()) {
		if ( it->second.CreateImaginary() ) return true;	// out of memory
		bcc = true;
	}
	else if (!rvalue.IsReal() && !it->second.IsReal()) {
		bcc = true;
	}

	int i, ix;

	for (i=0; i<n; ++i) {
		// get index
		ix = (int) idx.mr[i];
		// check bounds
		if (ix<0 || ix>=lsize) return true;
		// assign to lvalue
		it->second.mr[ix] = rvalue.mr[i];
		if (bcc) it->second.mi[ix] = rvalue.mi[i];
	}

	return false;
}

bool cHash::ixCubeAssign( string& name, cMatrix& rvalue, cCube& idx )
{
	complex<double> cx;
	if (rvalue.IsScalar()) {		// copy scalar value to every position idx
		if (rvalue.IsReal()) return ixCubeAssign( name, rvalue.Get0R(), idx );
		else { rvalue.Get0(cx); return ixCubeAssign( name, cx, idx ); }
	}

	map<string, cCube>::iterator it = m_table_cube.find(name);
	if (it == m_table_cube.end()) {		// not found
		return true;
	}

	int lsize = it->second.GetSize();
	int n = idx.GetSize();
	int i, ix;

	bool bc = !it->second.IsReal();

	for (i=0; i<n; ++i) {
		// get index
		ix = (int) idx.mr[i];
		// check bounds
		if (ix<0 || ix>=lsize) return true;
		// assign to lvalue
		it->second.mr[ix] = rvalue.mr[i];
		if (bc) it->second.mi[ix] = rvalue.mi[i];
	}

	return false;
}

bool cHash::ixCubeAssign( string& name, double drval, cCube& idx )
{
	map<string, cCube>::iterator it = m_table_cube.find(name);
	if (it == m_table_cube.end()) {		// not found
		return true;
	}

	int lsize = it->second.GetSize();
	int n = idx.GetSize();
	int i, ix;

	bool bc = !it->second.IsReal();

	for (i=0; i<n; ++i) {
		// get index
		ix = (int) idx.mr[i];
		// check bounds
		if (ix<0 || ix>=lsize) return true;
		// assign to lvalue
		it->second.mr[ix] = drval;
		if (bc) it->second.mi[ix] = 0.;
	}

	return false;
}

bool cHash::ixCubeAssign( string& name, complex<double> cx, cCube& idx )
{
	map<string, cCube>::iterator it = m_table_cube.find(name);
	if (it == m_table_cube.end()) {		// not found
		return true;
	}

	int lsize = it->second.GetSize();
	int n = idx.GetSize();
	int i, ix;

	if ( it->second.IsReal() ) {
		if ( it->second.CreateImaginary() ) return true;
	}

	for (i=0; i<n; ++i) {
		// get index
		ix = (int) idx.mr[i];
		// check bounds
		if (ix<0 || ix>=lsize) return true;
		// assign to lvalue
		it->second.mr[ix] = cx.real();
		it->second.mi[ix] = cx.imag();
	}

	return false;
}

bool cHash::hashAssign(cContainer& v, cContainer& arg)
{
	int type = m_table_type[v.GetName()];
	if (type == VAR_TYPE_HASH)
		m_table_hash[v.GetName()][arg.s.s] = v;
	else if (type == -1) {
		remove(v);
		m_table_hash[v.GetName()][arg.s.s] = v;
	}
	else return true;		// illegal assignment

	return false;
}

bool cHash::hashGet(cContainer& res, cContainer& in, vector<string>& strArgs)
{
	int type = in.GetType();
	string str;
	map<string,cContainer>::iterator it;

	if (type == VAR_TYPE_HASH && strArgs.size() > 1) {
		strArgs.erase( strArgs.begin() );
		str = strArgs[0];
		if (in.h.h.find(str)==in.h.h.end()) {
			addError("key not found: ", str);
			return true;
		}
		return hashGet(res, in.h.h[str], strArgs);
	}
	else if (strArgs.size() == 1) {
		res = in;
	}
	else return true;		// illegal access

	return false;
}

bool cHash::hashGet(cContainer& v, string& name, vector<string>& strArgs)
{
	if (strArgs.empty()) return false;
	string str = strArgs[0];
	//strArgs.erase( strArgs.begin() );

	map<string,cContainer>::iterator it;

	if (strArgs.size() == 1) {
		if (m_table_hash[name].find(str)==m_table_hash[name].end()) {
			addError("key not found");
			return true;
		}
		v = m_table_hash[name][str];
		return false;
	}
	else if (m_table_hash[name][str].GetType() != VAR_TYPE_HASH) { addError("invalid hash key"); return true; }
	
	return hashGet(v, m_table_hash[name][strArgs[0]], strArgs);
}

bool cHash::hashSet(cContainer& in, vector<string>& strArgs, cContainer& v)
{
	int type = in.GetType();
	if (type == VAR_TYPE_HASH && strArgs.size() > 1) {
		strArgs.erase( strArgs.begin() );
		return hashSet(in.h.h[strArgs[0]], strArgs, v);
	}
	else if (strArgs.size() == 1) {
		in = v;
	}
	else if (type == -1) {
		// do something else
		in.SetType(VAR_TYPE_HASH);
		strArgs.erase( strArgs.begin() );
		return hashSet(in.h.h[strArgs[0]], strArgs, v);
	}
	else {
		addError("illegal access");
		return true;
	}

	return false;
}

bool cHash::hashSet(string& name, vector<string>& strArgs, cContainer& v)
{
	int type, r, c, d;
	bool bUpdateType(false);

	if (strArgs.empty()) return false;
	string str = strArgs[0];

	lookupInfo( name, type, r, c, d );
	if (type != VAR_TYPE_HASH) { bUpdateType = true; remove( name ); }

	if (strArgs.size() == 1) {
		m_table_hash[name][str] = v;
		if (bUpdateType) m_table_type[name] = VAR_TYPE_HASH;
		return false;
	}
	//else if (m_table_hash[name][str].GetType() != VAR_TYPE_HASH) { addError("illegal access"); return true; }
	
	if (hashSet(m_table_hash[name][strArgs[0]], strArgs, v)) return true;
	
	if (bUpdateType) m_table_type[name] = VAR_TYPE_HASH;
	return false;
}

bool cHash::remove(cContainer& t)
{
	string name = t.GetName();
	return remove(name);
}

bool cHash::remove(string& name)
{
	int type;
	if (lookup(type,name))
		return true;

	switch (type) {
	case VAR_TYPE_NUMERIC:
		m_table_matrix.erase(name);
		break;

	case VAR_TYPE_CUBE:
		m_table_cube.erase(name);
		break;

	case VAR_TYPE_STRING:
		m_table_string.erase(name);
		break;
	}

	m_table_type.erase(name);

	return false;
}

bool cHash::set(string& name, cContainer& t)
{
	t.SetName( name );
	insert( t );

	return false;
}

bool cHash::set(string& name, double& dv)
{
	cMatrix m;
	m.SetTo(dv);

	return insert(name,m);
}

bool cHash::set(string& name, complex<double>* pcv)
{
	cMatrix m;
	m.SetTo(*pcv);

	return insert(name,m);
}

bool cHash::set(string& name, cMatrix& m)
{
	return insert(name, m);
}

bool cHash::set(string& name, string& s)
{
	cString S;
	S.SetTo(s);

	return insert(name, S);
}

void cHash::clear()
{
	m_table_type.clear();
	m_table_matrix.clear();
	m_table_string.clear();

	initTable();
	setDefaults();
	m_nCount = 0;
}

int cHash::count()
{
	return m_nCount;
}


bool cHash::getSize(int& nRows, int& nCols, int& nDepth, string name) const
{
	int type(-1);

	return lookupInfo(name, type, nRows, nCols, nDepth);
}


// access operator (one arg)
//
bool cHash::matAccess(cMatrix& Result, string& name, double darg)
{
	map<string, cMatrix>::iterator it = m_table_matrix.find(name);
	if (it == m_table_matrix.end()) {
		return true;
	}

	int n = it->second.GetSize();
	int iarg = (int) darg;
	if (iarg < 0 || iarg >= n) { addError("index outside array bounds"); return true; }

	// get iarg^th entry of var
	if (it->second.IsReal()) {
		if (Result.Create(1,1)) return true;
		Result.mr[0] = it->second.mr[iarg];
	}
	else {	// complex
		if (Result.CreateComplex(1,1)) return true;
		Result.mr[0] = it->second.mr[iarg];
		Result.mi[0] = it->second.mi[iarg];
	}

	return false;
}

bool cHash::matAccess(cMatrix& Result, string& name, cMatrix& arg)
{
	map<string, cMatrix>::iterator it = m_table_matrix.find(name);
	if (it == m_table_matrix.end()) {
		return true;
	}

	int n = arg.GetSize(), nvar = it->second.GetSize();
	int iarg, i;

	if (it->second.IsReal()) {
		if (Result.Create(arg.GetRows(),arg.GetColumns())) return true;
		for (i=0; i<n; ++i) {
			iarg = (int) arg.mr[i];
			if (iarg < 0 || iarg >= nvar) { Result.Delete(); addError("index outside array bounds"); return true; }
			Result.mr[i] = it->second.mr[iarg];
		}
	}
	else {	// complex
		if (Result.CreateComplex(arg.GetRows(),arg.GetColumns())) return true;
		for (i=0; i<n; ++i) {
			iarg = (int) arg.mr[i];
			if (iarg < 0 || iarg >= nvar) { Result.Delete(); addError("index outside array bounds"); return true; }
			Result.mr[i] = it->second.mr[iarg];
			Result.mi[i] = it->second.mi[iarg];
		}
	}

	return false;
}

bool cHash::cubeAccess(cMatrix& Result, string& name, double darg)
{
	map<string, cCube>::iterator it = m_table_cube.find(name);
	if (it == m_table_cube.end()) {
		return true;
	}

	int n = it->second.GetSize();
	int iarg = (int) darg;
	if (iarg < 0 || iarg >= n) { addError("index outside array bounds"); return true; }

	// get iarg^th entry of var
	if (it->second.IsReal()) {
		if (Result.Create(1,1)) return true;
		Result.mr[0] = it->second.mr[iarg];
	}
	else {	// complex
		if (Result.CreateComplex(1,1)) return true;
		Result.mr[0] = it->second.mr[iarg];
		Result.mi[0] = it->second.mi[iarg];
	}

	return false;
}

bool cHash::cubeAccess(cMatrix& Result, string& name, cMatrix& arg)
{
	map<string, cCube>::iterator it = m_table_cube.find(name);
	if (it == m_table_cube.end()) {
		return true;
	}

	int n = arg.GetSize(), nvar = it->second.GetSize();
	int iarg, i;

	if (it->second.IsReal()) {
		if (Result.Create(arg.GetRows(),arg.GetColumns())) return true;
		for (i=0; i<n; ++i) {
			iarg = (int) arg.mr[i];
			if (iarg < 0 || iarg >= nvar) { Result.Delete(); addError("index outside array bounds"); return true; }
			Result.mr[i] = it->second.mr[iarg];
		}
	}
	else {	// complex
		if (Result.CreateComplex(arg.GetRows(),arg.GetColumns())) return true;
		for (i=0; i<n; ++i) {
			iarg = (int) arg.mr[i];
			if (iarg < 0 || iarg >= nvar) { Result.Delete(); addError("index outside array bounds"); return true; }
			Result.mr[i] = it->second.mr[iarg];
			Result.mi[i] = it->second.mi[iarg];
		}
	}

	return false;
}

bool cHash::stringAccess(cString& Result, string& name, double darg)
{
	map<string, cString>::iterator it = m_table_string.find(name);
	if (it == m_table_string.end()) {
		return true;
	}

	Result.Delete();

	int ix = (int) darg;

	if (ix<0 || ix >= (int) it->second.s.length()) {
		addError("index outside string dimensions"); return true;
	}

	char ch = it->second.s.at(ix);
	Result.SetTo( ch );
	return false;
}

bool cHash::stringAccess(cString& Result, string& name, cMatrix& arg)
{
	map<string, cString>::iterator it = m_table_string.find(name);
	if (it == m_table_string.end()) {
		return true;
	}

	Result.Delete();

	int ix, n = arg.GetSize(), len = (int) it->second.s.length();
	double lf;

	it->second.s.reserve( n );

	for (int i=0; i<n; ++i)
	{
		arg.GetR(lf,i);
		ix = (int) lf;

		if (ix<0 || ix >= len) {
			addError("index outside string dimensions"); return true;
		}

		Result.s.append( 1, it->second.s.at(ix) );
	}

	return false;
}

bool cHash::containerAccess(cContainer& c, string& name, double darg)
{
	int nType;

	if (lookupInfo(nType,name)) {
		addError("undefined symbol: ", name);
		return true;
	}

	switch (nType) {
	case VAR_TYPE_NUMERIC:
		c.SetType(VAR_TYPE_NUMERIC);
		return matAccess(c.m, name, darg);

	case VAR_TYPE_CUBE:
		c.SetType(VAR_TYPE_CUBE);
		return cubeAccess(c.m, name, darg);

	case VAR_TYPE_STRING:
		c.SetType(VAR_TYPE_STRING);
		return stringAccess(c.s, name, darg);

	default:
		addError("illegal indexing: ", name);
		return true;
	}

	return false;
}

bool cHash::containerAccess(cContainer& c, string& name, string& name1)
{
	int nType, nType1;

	if (lookupInfo(nType,name)) {
		addError("undefined symbol: ", name);
		return true;
	}

	if (lookupInfo(nType1,name1)) {
		addError("undefined symbol: ", name1);
		return true;
	}

	if (nType1 != VAR_TYPE_NUMERIC) {
		addError("illegal indexing: ", name1);
		return true;
	}

	map<string, cMatrix>::iterator it = m_table_matrix.find(name1);
	if (it == m_table_matrix.end()) {
		return true;
	}

	switch (nType) {
	case VAR_TYPE_NUMERIC:
		c.SetType(VAR_TYPE_NUMERIC);
		return matAccess(c.m, name, it->second);

	case VAR_TYPE_CUBE:
		c.SetType(VAR_TYPE_CUBE);
		return cubeAccess(c.c, name, it->second);

	case VAR_TYPE_STRING:
		c.SetType(VAR_TYPE_STRING);
		return stringAccess(c.s, name, it->second);

	default:
		addError("illegal indexing: ", name);
		return true;
	}

	return false;
}

bool cHash::containerAccess(cContainer& c, string& name, cContainer& arg)
{
	int nType;

	if (lookupInfo(nType,name)) {
		addError("undefined symbol: ", name);
		return true;
	}

	if (arg.GetType() != VAR_TYPE_NUMERIC) {
		addError("illegal indexing: ", name);
		return true;
	}

	switch (nType) {
	case VAR_TYPE_NUMERIC:
		c.SetType(VAR_TYPE_NUMERIC);
		return matAccess(c.m, name, arg.m);

	case VAR_TYPE_CUBE:
		c.SetType(VAR_TYPE_CUBE);
		return cubeAccess(c.c, name, arg.m);

	case VAR_TYPE_STRING:
		c.SetType(VAR_TYPE_STRING);
		return stringAccess(c.s, name, arg.m);

	default:
		addError("illegal indexing: ", name);
		return true;
	}

	return false;
}

//
// access operator (two arg)
//
bool cHash::matAccess(cMatrix& Result, string& name, double darg1, double darg2)
{
	map<string, cMatrix>::iterator it = m_table_matrix.find(name);
	if (it == m_table_matrix.end()) {
		addError("no matrix found: ", name);
		return true;
	}

	int iarg1 = (int) darg1;
	int iarg2 = (int) darg2;
	int nv_rows = it->second.GetRows(), nv_cols = it->second.GetColumns();

	if (iarg1 < 0 || iarg1 >= nv_rows) { addError("index outside array bounds"); return true; }
	if (iarg2 < 0 || iarg2 >= nv_cols) { addError("index outside array bounds"); return true; }

	//double *pr, *pi;
	double dval;
	complex<double> cval;

	if (it->second.IsReal()) {
		if (Result.Create(1,1)) return true;
		it->second.GetR(dval,iarg1,iarg2);
		Result.mr[0] = dval;
	}
	else {
		if (Result.CreateComplex(1,1)) return true;
		it->second.GetC(cval,iarg1,iarg2);
		Result.mr[0] = cval.real();
		Result.mi[0] = cval.imag();
	}

	return false;
}

bool cHash::matAccess(cMatrix& Result, string& name, cMatrix& arg1, double darg2)
{
	map<string, cMatrix>::iterator it = m_table_matrix.find(name);
	if (it == m_table_matrix.end()) {
		addError("no matrix found: ", name);
		return true;
	}

	if (!arg1.IsVector()) { addError("illegal argument"); return true; }

	int iarg2 = (int) darg2;
	int nvar = it->second.GetSize();
	int nv_rows = it->second.GetRows(), nv_cols = it->second.GetColumns();

	if (iarg2 < 0 || iarg2 >= nv_cols) { addError("index outside array bounds"); return true; }
	int iarg1;

	int narg = arg1.GetSize();
	double dval;
	complex<double> cval;

	if (it->second.IsReal()) {
		if (Result.Create(narg,1)) return true;

		for (int i=0; i<narg; ++i) {
			iarg1 = (int) arg1.mr[i];
			if (iarg1 < 0 || iarg1 >= nv_rows) { Result.Delete(); addError("index outside array bounds"); return true; }
			it->second.GetR(dval,iarg1,iarg2);
			Result.mr[i] = dval;
		}
	}
	else {
		if (Result.CreateComplex(narg,1)) return true;

		for (int i=0; i<narg; ++i) {
			iarg1 = (int) arg1.mr[i];
			if (iarg1 < 0 || iarg1 >= nv_rows) { Result.Delete(); addError("index outside array bounds"); return true; }
			it->second.GetC(cval,iarg1,iarg2);
			Result.mr[i] = cval.real();
			Result.mi[i] = cval.imag();
		}
	}

	return false;
}

bool cHash::matAccess(cMatrix& Result, string& name, double darg1, cMatrix& arg2)
{
	map<string, cMatrix>::iterator it = m_table_matrix.find(name);
	if (it == m_table_matrix.end()) {
		addError("no matrix found: ", name);
		return true;
	}

	if (!arg2.IsVector()) { addError("illegal argument"); return true; }

	int iarg1 = (int) darg1;
	int nvar = it->second.GetSize();
	int nv_rows = it->second.GetRows(), nv_cols = it->second.GetColumns();

	if (iarg1 < 0 || iarg1 >= nv_rows) { addError("index outside array bounds"); return true; }
	int iarg2;

	int narg = arg2.GetSize();
	double dval;
	complex<double> cval;

	if (it->second.IsReal()) {
		if (Result.Create(1,narg)) return true;

		for (int i=0; i<narg; ++i) {
			iarg2 = (int) arg2.mr[i];
			if (iarg2 < 0 || iarg2 >= nv_cols) { Result.Delete(); addError("index outside array bounds"); return true; }
			it->second.GetR(dval,iarg1,iarg2);
			Result.mr[i] = dval;
		}
	}
	else {
		if (Result.CreateComplex(1,narg)) return true;

		for (int i=0; i<narg; ++i) {
			iarg2 = (int) arg2.mr[i];
			if (iarg2 < 0 || iarg2 >= nv_cols) { Result.Delete(); addError("index outside array bounds"); return true; }
			it->second.GetC(cval,iarg1,iarg2);
			Result.mr[i] = cval.real();
			Result.mi[i] = cval.imag();
		}
	}

	return false;
}

bool cHash::matAccess(cMatrix& Result, string& name, cMatrix& arg1, cMatrix& arg2)
{
	map<string, cMatrix>::iterator it = m_table_matrix.find(name);
	if (it == m_table_matrix.end()) {
		addError("no matrix found: ", name);
		return true;
	}

	// both arguments must be vectors
	if (!arg1.IsVector() || !arg2.IsVector()) { addError("illegal argument"); return true; }

	int nvar = it->second.GetSize();
	int nv_rows = it->second.GetRows(), nv_cols = it->second.GetColumns();

	int iarg1, iarg2;
	int i, j, ix;

	int narg1 = arg1.GetSize();
	int narg2 = arg2.GetSize();

	double dval;
	complex<double> cval;

	if (it->second.IsReal()) {
		if (Result.Create(narg1,narg2)) return true;

		for (i=0; i<narg1; ++i) {

			iarg1 = (int) arg1.mr[i];
			if (iarg1 < 0 || iarg1 >= nv_rows) { Result.Delete(); addError("index outside array bounds"); return true; }

			for (j=0; j<narg2; ++j) {
				iarg2 = (int) arg2.mr[j];
				if (iarg2 < 0 || iarg2 >= nv_cols) { Result.Delete(); addError("index outside array bounds"); return true; }
				it->second.GetR(dval,iarg1,iarg2);

				//ix = var.GetIndex(iarg1,iarg2);
				Result.SetR(i,j,dval);	//mr[ix] = dval;
			}

		}
	}
	else {
		if (Result.CreateComplex(narg1,narg2)) return true;

		for (i=0; i<narg1; ++i) {

			iarg1 = (int) arg2.mr[i];
			if (iarg1 < 0 || iarg1 >= nv_rows) { Result.Delete(); addError("index outside array bounds"); return true; }

			for (j=0; j<narg2; ++j) {
				iarg2 = (int) arg2.mr[j];
				if (iarg2 < 0 || iarg2 >= nv_cols) { Result.Delete(); addError("index outside array bounds"); return true; }
				it->second.GetC(cval,iarg1,iarg2);
				ix = it->second.GetIndex(iarg1,iarg2);
				Result.SetR(i,j,cval.real());	//mr[ix] = cval.real();
				Result.SetI(i,j,cval.imag());	//mi[ix] = cval.imag();
			}

		}

	}

	return false;
}

bool cHash::matAccess(cMatrix& Result, string& name, string& name1, double darg2)
{
	int nType1;

	if (lookupInfo(nType1,name1)) {
		addError("undefined symbol: ", name1);
		return true;
	}

	if (nType1!=VAR_TYPE_NUMERIC) {
		addError("illegal indexing: ", name);
		return true;
	}

	map<string, cMatrix>::iterator it = m_table_matrix.find(name1);
	if (it == m_table_matrix.end()) {
		addError("database error!");
		return true;
	}

	return matAccess(Result, name, it->second, darg2);
}

bool cHash::matAccess(cMatrix& Result, string& name, string& name1, cMatrix& arg2)
{
	int nType1;

	if (lookupInfo(nType1,name1)) {
		addError("undefined symbol: ", name1);
		return true;
	}

	if (nType1!=VAR_TYPE_NUMERIC) {
		addError("illegal indexing: ", name);
		return true;
	}

	map<string, cMatrix>::iterator it = m_table_matrix.find(name1);
	if (it == m_table_matrix.end()) {
		addError("database error!");
		return true;
	}

	return matAccess(Result, name, it->second, arg2);
}

bool cHash::matAccess(cMatrix& Result, string& name, double darg1, string& name2)
{
	int nType2;

	if (lookupInfo(nType2,name2)) {
		addError("undefined symbol: ", name2);
		return true;
	}

	if (nType2!=VAR_TYPE_NUMERIC) {
		addError("illegal indexing: ", name);
		return true;
	}

	map<string, cMatrix>::iterator it = m_table_matrix.find(name2);
	if (it == m_table_matrix.end()) {
		addError("database error!");
		return true;
	}

	return matAccess(Result, name, darg1, it->second);
}

bool cHash::matAccess(cMatrix& Result, string& name, cMatrix& arg1, string& name2)
{
	int nType2;

	if (lookupInfo(nType2,name2)) {
		addError("undefined symbol: ", name2);
		return true;
	}

	if (nType2!=VAR_TYPE_NUMERIC) {
		addError("illegal indexing: ", name);
		return true;
	}

	map<string, cMatrix>::iterator it = m_table_matrix.find(name2);
	if (it == m_table_matrix.end()) {
		addError("database error!");
		return true;
	}

	return matAccess(Result, name, arg1, it->second);
}

bool cHash::matAccess(cMatrix& Result, string& name, string& name1, string& name2)
{
	int nType1, nType2;

	if (lookupInfo(nType1,name1)) {
		addError("undefined symbol: ", name1);
		return true;
	}

	if (lookupInfo(nType2,name2)) {
		addError("undefined symbol: ", name2);
		return true;
	}

	if (nType1!=VAR_TYPE_NUMERIC || nType2!=VAR_TYPE_NUMERIC) {
		addError("illegal indexing: ", name);
		return true;
	}

	map<string, cMatrix>::iterator it1 = m_table_matrix.find(name1);
	if (it1 == m_table_matrix.end()) {
		addError("database error!");
		return true;
	}
	map<string, cMatrix>::iterator it2 = m_table_matrix.find(name2);
	if (it2 == m_table_matrix.end()) {
		addError("database error!");
		return true;
	}

	return matAccess(Result, name, it1->second, it2->second);
}


bool cHash::containerAccess(cContainer& c, string& name, cContainer& arg1, double darg2)
{
	switch (arg1.GetType()) {
	case VAR_TYPE_NUMERIC:
		c.SetType(VAR_TYPE_NUMERIC);
		return matAccess(c.m, name, arg1.m, darg2);

	default:
		addError("illegal indexing: ", name);
		return true;
	}

	return true;
}

bool cHash::containerAccess(cContainer& c, string& name, cContainer& arg1, string& name2)
{
	int nType2;

	if (lookupInfo(nType2,name2)) {
		addError("undefined symbol: ", name2);
		return true;
	}

	if (nType2!=VAR_TYPE_NUMERIC) {
		addError("illegal indexing: ", name);
		return true;
	}

	switch (arg1.GetType()) {
	case VAR_TYPE_NUMERIC:
		c.SetType(VAR_TYPE_NUMERIC);
		return matAccess(c.m, name, arg1.m, name2);

	default:
		addError("illegal indexing: ", name);
		return true;
	}

	return true;
}

bool cHash::containerAccess(cContainer& c, string& name, double darg1, cContainer& arg2)
{
	switch (arg2.GetType()) {
	case VAR_TYPE_NUMERIC:
		c.SetType(VAR_TYPE_NUMERIC);
		return matAccess(c.m, name, darg1, arg2.m);

	default:
		addError("illegal indexing: ", name);
		return true;
	}

	return true;
}

bool cHash::containerAccess(cContainer& c, string& name, string& name1, cContainer& arg2)
{
	int nType1;

	if (lookupInfo(nType1,name1)) {
		addError("undefined symbol: ", name1);
		return true;
	}

	if (nType1!=VAR_TYPE_NUMERIC) {
		addError("illegal indexing: ", name);
		return true;
	}

	switch (arg2.GetType()) {
	case VAR_TYPE_NUMERIC:
		c.SetType(VAR_TYPE_NUMERIC);
		return matAccess(c.m, name, name1, arg2.m);

	default:
		addError("illegal indexing: ", name);
		return true;
	}

	return true;
}

bool cHash::containerAccess(cContainer& c, string& name, cContainer& arg1, cContainer& arg2)
{
	if (arg1.GetType() != VAR_TYPE_NUMERIC || arg2.GetType() != VAR_TYPE_NUMERIC) {
		addError("illegal indexing: ", name);
		return true;
	}

	c.SetType(VAR_TYPE_NUMERIC);
	return matAccess(c.m, name, arg1.m, arg2.m);
}


//
// three arg
//
bool cHash::cubeAccess(cMatrix& Result, string& name, double darg1, double darg2, double darg3)
{
	map<string, cCube>::iterator it = m_table_cube.find(name);
	if (it == m_table_cube.end()) {
		return true;
	}

	int iarg1 = (int) darg1;
	int iarg2 = (int) darg2;
	int iarg3 = (int) darg3;
	if (iarg1<0 || iarg1>=it->second.GetRows() ||
		iarg2<0 || iarg2>=it->second.GetColumns() ||
		iarg3<0 || iarg3>=it->second.GetDepth() ) {
		addError("index outside array bounds"); return true;
	}

	double lf, lfi;

	if (it->second.IsReal()) {
		Result.Create(1,1);
		it->second.GetR(lf, iarg1, iarg2, iarg3);
		Result.mr[0] = lf;
	}
	else {
		Result.CreateComplex(1,1);
		it->second.GetR(lf, iarg1, iarg2, iarg3);
		it->second.GetI(lfi, iarg1, iarg2, iarg3);
		Result.mr[0] = lf;
		Result.mi[0] = lfi;
	}
	return false;
}

bool cHash::cubeAccess(cMatrix& Result, string& name, double darg1, double darg2, cMatrix& arg3)
{
	map<string, cCube>::iterator it = m_table_cube.find(name);
	if (it == m_table_cube.end()) {
		return true;
	}

	int iarg1 = (int) darg1;
	int iarg2 = (int) darg2;
	if (iarg1<0 || iarg1>=it->second.GetRows() ||
		iarg2<0 || iarg2>=it->second.GetColumns() ) {
		addError("index outside array bounds"); return true;
	}

	if (!arg3.IsVector()) { addError("illegal argument"); return true; }

	int i, iarg3, sz = arg3.GetSize();
	double lf, lfi, darg3;

	if (it->second.IsReal()) {
		Result.Create( sz, 1 );
		for (i=0; i<sz; ++i) {
			arg3.GetR(darg3, i);		// ignore imag part of arg3
			iarg3 = (int) darg3;
			if (iarg3 >=0 && iarg3 < it->second.nm) {
				it->second.GetR(lf, iarg1, iarg2, iarg3);
				Result.mr[i] = lf;
			}
			else { addError("index outside array bounds"); return true; }
		}
	}
	else {
		Result.CreateComplex( sz, 1 );
		for (i=0; i<sz; ++i) {
			arg3.GetR(darg3, i);		// ignore imag part of arg3
			iarg3 = (int) darg3;
			if (iarg3 >=0 && iarg3 < it->second.nm) {
				it->second.GetR(lf, iarg1, iarg2, iarg3);
				it->second.GetI(lfi, iarg1, iarg2, iarg3);
				Result.mr[i] = lf;
				Result.mi[i] = lfi;
			}
			else { addError("index outside array bounds"); return true; }
		}
	}
	return false;
}

bool cHash::cubeAccess(cMatrix& Result, string& name, double darg1, cMatrix& arg2, double darg3)
{
	map<string, cCube>::iterator it = m_table_cube.find(name);
	if (it == m_table_cube.end()) {
		return true;
	}

	int iarg1 = (int) darg1;
	int iarg3 = (int) darg3;
	if (iarg1<0 || iarg1>=it->second.GetRows() ||
		iarg3<0 || iarg3>=it->second.GetDepth() ) {
		addError("index outside array bounds"); return true;
	}

	if (!arg2.IsVector()) { addError("illegal argument"); return true; }

	int i, iarg2, sz = arg2.GetSize();
	double lf, lfi, darg2;

	if (it->second.IsReal()) {
		Result.Create( sz, 1 );
		for (i=0; i<sz; ++i) {
			arg2.GetR(darg2, i);		// ignore imag part of arg2
			iarg2 = (int) darg2;
			if (iarg2 >=0 && iarg2 < it->second.nc) {
				it->second.GetR(lf, iarg1, iarg2, iarg3);
				Result.mr[i] = lf;
			}
			else { addError("index outside array bounds"); return true; }
		}
	}
	else {
		Result.CreateComplex( sz, 1 );
		for (i=0; i<sz; ++i) {
			arg2.GetR(darg2, i);		// ignore imag part of arg2
			iarg2 = (int) darg2;
			if (iarg2 >=0 && iarg2 < it->second.nc) {
				it->second.GetR(lf, iarg1, iarg2, iarg3);
				it->second.GetI(lfi, iarg1, iarg2, iarg3);
				Result.mr[i] = lf;
				Result.mi[i] = lfi;
			}
			else { addError("index outside array bounds"); return true; }
		}
	}
	return false;
}

bool cHash::cubeAccess(cMatrix& Result, string& name, double darg1, cMatrix& arg2, cMatrix& arg3)
{
	map<string, cCube>::iterator it = m_table_cube.find(name);
	if (it == m_table_cube.end()) {
		return true;
	}

	int iarg1 = (int) darg1;
	if (iarg1<0 || iarg1>=it->second.GetRows()) {
		addError("index outside array bounds"); return true;
	}

	if (!arg2.IsVector() || !arg3.IsVector()) { addError("illegal argument"); return true; }

	int i, j, iarg2, iarg3, sz2 = arg2.GetSize(), sz3 = arg3.GetSize();
	double lf, lfi, darg2, darg3;

	if (it->second.IsReal()) {
		Result.Create( sz2, sz3 );
		for (i=0; i<sz2; ++i) {
			arg2.GetR(darg2, i);			// ignore imag part of arg2
			iarg2 = (int) darg2;
			if (iarg2 >=0 && iarg2 < it->second.nc) {
				for (j=0; j<sz3; ++j) {
					arg3.GetR(darg3, j);	// ignore imag part of arg3
					iarg3 = (int) darg3;
					if (iarg3 >=0 && iarg3 < it->second.nm) {
						it->second.GetR(lf, iarg1, iarg2, iarg3);
						Result.SetR(i, j, lf);
					}
					else { addError("index outside array bounds"); return true; }
				}
			}
			else { addError("index outside array bounds"); return true; }
		}
	}		// IsReal
	else {
		Result.CreateComplex( sz2, sz3 );
		for (i=0; i<sz2; ++i) {
			arg2.GetR(darg2, i);			// ignore imag part of arg2
			iarg2 = (int) darg2;
			if (iarg2 >=0 && iarg2 < it->second.nc) {
				for (j=0; j<sz3; ++j) {
					arg3.GetR(darg3, j);	// ignore imag part of arg3
					iarg3 = (int) darg3;
					if (iarg3 >=0 && iarg3 < it->second.nm) {
						it->second.GetR(lf, iarg1, iarg2, iarg3);
						it->second.GetI(lfi, iarg1, iarg2, iarg3);
						Result.SetR(i, j, lf);
						Result.SetI(i, j, lfi);
					}
					else { addError("index outside array bounds"); return true; }
				}
			}
			else { addError("index outside array bounds"); return true; }
		}
	}
	return false;
}

bool cHash::cubeAccess(cMatrix& Result, string& name, cMatrix& arg1, double darg2, double darg3)
{
	map<string, cCube>::iterator it = m_table_cube.find(name);
	if (it == m_table_cube.end()) {
		return true;
	}

	int iarg2 = (int) darg2;
	int iarg3 = (int) darg3;
	if (iarg2<0 || iarg2>=it->second.GetColumns() ||
		iarg3<0 || iarg3>=it->second.GetDepth() ) {
		addError("index outside array bounds"); return true;
	}

	if (!arg1.IsVector()) { addError("illegal argument"); return true; }

	int i, iarg1, sz = arg1.GetSize();
	double lf, lfi, darg1;

	if (it->second.IsReal()) {
		Result.Create( sz, 1 );
		for (i=0; i<sz; ++i) {
			arg1.GetR(darg1, i);		// ignore imag part of arg1
			iarg1 = (int) darg1;
			if (iarg1 >=0 && iarg1 < it->second.nr) {
				it->second.GetR(lf, iarg1, iarg2, iarg3);
				Result.mr[i] = lf;
			}
			else { addError("index outside array bounds"); return true; }
		}
	}
	else {
		Result.CreateComplex( sz, 1 );
		for (i=0; i<sz; ++i) {
			arg1.GetR(darg1, i);		// ignore imag part of arg1
			iarg1 = (int) darg1;
			if (iarg1 >=0 && iarg1 < it->second.nr) {
				it->second.GetR(lf, iarg1, iarg2, iarg3);
				it->second.GetI(lfi, iarg1, iarg2, iarg3);
				Result.mr[i] = lf;
				Result.mi[i] = lfi;
			}
			else { addError("index outside array bounds"); return true; }
		}
	}
	return false;
}

bool cHash::cubeAccess(cMatrix& Result, string& name, cMatrix& arg1, double darg2, cMatrix& arg3)
{
	map<string, cCube>::iterator it = m_table_cube.find(name);
	if (it == m_table_cube.end()) {
		return true;
	}

	int iarg2 = (int) darg2;
	if (iarg2<0 || iarg2>=it->second.GetColumns()) {
		addError("index outside array bounds"); return true;
	}

	if (!arg1.IsVector() || !arg3.IsVector()) { addError("illegal argument"); return true; }

	int i, j, iarg1, iarg3, sz1 = arg1.GetSize(), sz3 = arg3.GetSize();
	double lf, lfi, darg1, darg3;

	if (it->second.IsReal()) {
		Result.Create( sz1, sz3 );
		for (i=0; i<sz1; ++i) {
			arg1.GetR(darg1, i);			// ignore imag part of arg1
			iarg1 = (int) darg1;
			if (iarg1 >=0 && iarg1 < it->second.nr) {
				for (j=0; j<sz3; ++j) {
					arg3.GetR(darg3, j);	// ignore imag part of arg3
					iarg3 = (int) darg3;
					if (iarg3 >=0 && iarg3 < it->second.nm) {
						it->second.GetR(lf, iarg1, iarg2, iarg3);
						Result.SetR(i, j, lf);
					}
					else { addError("index outside array bounds"); return true; }
				}
			}
			else { addError("index outside array bounds"); return true; }
		}
	}		// IsReal
	else {
		Result.CreateComplex( sz1, sz3 );
		for (i=0; i<sz1; ++i) {
			arg1.GetR(darg1, i);			// ignore imag part of arg1
			iarg1 = (int) darg1;
			if (iarg1 >=0 && iarg1 < it->second.nr) {
				for (j=0; j<sz3; ++j) {
					arg3.GetR(darg3, j);	// ignore imag part of arg3
					iarg3 = (int) darg3;
					if (iarg3 >=0 && iarg3 < it->second.nm) {
						it->second.GetR(lf, iarg1, iarg2, iarg3);
						it->second.GetI(lfi, iarg1, iarg2, iarg3);
						Result.SetR(i, j, lf);
						Result.SetI(i, j, lfi);
					}
					else { addError("index outside array bounds"); return true; }
				}
			}
			else { addError("index outside array bounds"); return true; }
		}
	}
	return false;
}

bool cHash::cubeAccess(cMatrix& Result, string& name, cMatrix& arg1, cMatrix& arg2, double darg3)
{
	map<string, cCube>::iterator it = m_table_cube.find(name);
	if (it == m_table_cube.end()) {
		return true;
	}

	int iarg3 = (int) darg3;
	if (iarg3<0 || iarg3>=it->second.GetDepth()) {
		addError("index outside array bounds"); return true;
	}

	if (!arg1.IsVector() || !arg2.IsVector()) { addError("illegal argument"); return true; }

	int i, j, iarg1, iarg2, sz1 = arg1.GetSize(), sz2 = arg2.GetSize();
	double lf, lfi, darg1, darg2;

	if (it->second.IsReal()) {
		Result.Create( sz1, sz2 );
		for (i=0; i<sz1; ++i) {
			arg1.GetR(darg1, i);			// ignore imag part of arg1
			iarg1 = (int) darg1;
			if (iarg1 >=0 && iarg1 < it->second.nr) {
				for (j=0; j<sz2; ++j) {
					arg2.GetR(darg2, j);	// ignore imag part of arg2
					iarg2 = (int) darg2;
					if (iarg2 >=0 && iarg2 < it->second.nc) {
						it->second.GetR(lf, iarg1, iarg2, iarg3);
						Result.SetR(i, j, lf);
					}
					else { addError("index outside array bounds"); return true; }
				}
			}
			else { addError("index outside array bounds"); return true; }
		}
	}		// IsReal
	else {
		Result.CreateComplex( sz1, sz2 );
		for (i=0; i<sz1; ++i) {
			arg1.GetR(darg1, i);			// ignore imag part of arg1
			iarg1 = (int) darg1;
			if (iarg1 >=0 && iarg1 < it->second.nr) {
				for (j=0; j<sz2; ++j) {
					arg2.GetR(darg2, j);	// ignore imag part of arg2
					iarg2 = (int) darg2;
					if (iarg2 >=0 && iarg2 < it->second.nc) {
						it->second.GetR(lf, iarg1, iarg2, iarg3);
						it->second.GetI(lfi, iarg1, iarg2, iarg3);
						Result.SetR(i, j, lf);
						Result.SetI(i, j, lfi);
					}
					else { addError("index outside array bounds"); return true; }
				}
			}
			else { addError("index outside array bounds"); return true; }
		}
	}
	return false;
}

//
bool cHash::cubeAccess(cCube& Result, string& name, cMatrix& arg1, cMatrix& arg2, cMatrix& arg3)
{
	// NOTE: this fcn assumes none of the args are scalars! cContainer caller should check and call appropriate fcn.
	//
	map<string, cCube>::iterator it = m_table_cube.find(name);
	if (it == m_table_cube.end()) {
		return true;
	}

	if (!arg1.IsVector() || !arg2.IsVector() || !arg3.IsVector()) { addError("illegal argument"); return true; }

	int i, j, k;
	int iarg1, iarg2, iarg3, sz1 = arg1.GetSize(), sz2 = arg2.GetSize(), sz3 = arg3.GetSize();
	double lf, lfi, darg1, darg2, darg3;

	if (it->second.IsReal()) {
		Result.Create( sz1, sz2, sz3 );
		for (i=0; i<sz1; ++i) {
			arg1.GetR(darg1, i);			// ignore imag part of arg1
			iarg1 = (int) darg1;
			if (iarg1 >=0 && iarg1 < it->second.nr) {
				for (j=0; j<sz2; ++j) {
					arg2.GetR(darg2, j);	// ignore imag part of arg2
					iarg2 = (int) darg2;
					if (iarg2 >=0 && iarg2 < it->second.nc) {
						for (k=0; k<sz3; ++k) {
							arg3.GetR(darg3, k);	// ignore imag part of arg3
							iarg3 = (int) darg3;
							if (iarg3 >= 0 && iarg3 < it->second.nm) {
								it->second.GetR(lf, iarg1, iarg2, iarg3);
								Result.SetR(i, j, k, lf);
							}
							else { addError("index outside array bounds"); return true; }
						}
					}
					else { addError("index outside array bounds"); return true; }
				}
			}
			else { addError("index outside array bounds"); return true; }
		}
	}		// IsReal
	else {
		Result.CreateComplex( sz1, sz2, sz3 );
		for (i=0; i<sz1; ++i) {
			arg1.GetR(darg1, i);			// ignore imag part of arg1
			iarg1 = (int) darg1;
			if (iarg1 >=0 && iarg1 < it->second.nr) {
				for (j=0; j<sz2; ++j) {
					arg2.GetR(darg2, j);	// ignore imag part of arg2
					iarg2 = (int) darg2;
					if (iarg2 >=0 && iarg2 < it->second.nc) {
						for (k=0; k<sz3; ++k) {
							arg3.GetR(darg3, k);	// ignore imag part of arg3
							iarg3 = (int) darg3;
							if (iarg3 >= 0 && iarg3 < it->second.nm) {
								it->second.GetR(lf, iarg1, iarg2, iarg3);
								it->second.GetI(lfi, iarg1, iarg2, iarg3);
								Result.SetR(i, j, k, lf);
								Result.SetI(i, j, k, lfi);
							}
							else { addError("index outside array bounds"); return true; }
						}
					}
					else { addError("index outside array bounds"); return true; }
				}
			}
			else { addError("index outside array bounds"); return true; }
		}
	}
	return false;
}

bool cHash::containerAccess(cContainer& c, string& name, double darg1, double darg2, cContainer& arg3)
{
	if (arg3.GetType()!=VAR_TYPE_NUMERIC) {
		addError("illegal indexing: ", name);
		return true;
	}
	
	c.SetType(VAR_TYPE_NUMERIC);
	return cubeAccess(c.m, name, darg1, darg2, arg3.m);
}

bool cHash::containerAccess(cContainer& c, string& name, double darg1, cContainer& arg2, double darg3)
{
	if (arg2.GetType()!=VAR_TYPE_NUMERIC) {
		addError("illegal indexing: ", name);
		return true;
	}

	c.SetType(VAR_TYPE_NUMERIC);
	return cubeAccess(c.m, name, darg1, arg2.m, darg3);
}

bool cHash::containerAccess(cContainer& c, string& name, double darg1, cContainer& arg2, cContainer& arg3)
{
	if (arg2.GetType()!=VAR_TYPE_NUMERIC || arg2.GetType()!=VAR_TYPE_NUMERIC) {
		addError("illegal indexing: ", name);
		return true;
	}

	c.SetType(VAR_TYPE_NUMERIC);
	return cubeAccess(c.m, name, darg1, arg2.m, arg3.m);
}

bool cHash::containerAccess(cContainer& c, string& name, cContainer& arg1, double darg2, double darg3)
{
	if (arg1.GetType()!=VAR_TYPE_NUMERIC) {
		addError("illegal indexing: ", name);
		return true;
	}

	c.SetType(VAR_TYPE_NUMERIC);
	return cubeAccess(c.m, name, arg1.m, darg2, darg3);
}

bool cHash::containerAccess(cContainer& c, string& name, cContainer& arg1, double darg2, cContainer& arg3)
{
	if (arg1.GetType()!=VAR_TYPE_NUMERIC || arg3.GetType()!=VAR_TYPE_NUMERIC) {
		addError("illegal indexing: ", name);
		return true;
	}

	c.SetType(VAR_TYPE_NUMERIC);
	return cubeAccess(c.m, name, arg1.m, darg2, arg3.m);
}

bool cHash::containerAccess(cContainer& c, string& name, cContainer& arg1, cContainer& arg2, double darg3)
{
	if (arg1.GetType()!=VAR_TYPE_NUMERIC || arg2.GetType()!=VAR_TYPE_NUMERIC) {
		addError("illegal indexing: ", name);
		return true;
	}

	c.SetType(VAR_TYPE_NUMERIC);
	return cubeAccess(c.m, name, arg1.m, arg2.m, darg3);
}

bool cHash::containerAccess(cContainer& c, string& name, cContainer& arg1, cContainer& arg2, cContainer& arg3)
{
	if (arg1.GetType()!=VAR_TYPE_NUMERIC || arg2.GetType()!=VAR_TYPE_NUMERIC || arg3.GetType()!=VAR_TYPE_NUMERIC) {
		addError("illegal indexing: ", name);
		return true;
	}

	c.SetType(VAR_TYPE_CUBE);
	return cubeAccess(c.c, name, arg1.m, arg2.m, arg3.m);
}

bool cHash::containerAccess(cContainer& c, string& name, double darg1, double darg2, string& name3)
{
	cContainer t;

	if (lookup(t,name3)) {
		addError("undefined symbol: ", name3);
		return true;
	}

	if (t.GetType()!=VAR_TYPE_NUMERIC) {
		addError("illegal indexing: ", name3);
		return true;
	}

	return containerAccess(c, name, darg1, darg2, t);
}

bool cHash::containerAccess(cContainer& c, string& name, double darg1, string& name2, double darg3)
{
	cContainer t;

	if (lookup(t,name2)) {
		addError("undefined symbol: ", name2);
		return true;
	}

	if (t.GetType()!=VAR_TYPE_NUMERIC) {
		addError("illegal indexing: ", name2);
		return true;
	}

	return containerAccess(c, name, darg1, t, darg3);
}

bool cHash::containerAccess(cContainer& c, string& name, double darg1, string& name2, string& name3)
{
	cContainer t2, t3;

	if (lookup(t2,name2)) {
		addError("undefined symbol: ", name2);
		return true;
	}
	if (lookup(t3,name3)) {
		addError("undefined symbol: ", name3);
		return true;
	}

	if (t2.GetType()!=VAR_TYPE_NUMERIC) {
		addError("illegal indexing: ", name2);
		return true;
	}
	if (t3.GetType()!=VAR_TYPE_NUMERIC) {
		addError("illegal indexing: ", name3);
		return true;
	}

	return containerAccess(c, name, darg1, t2, t3);
}

bool cHash::containerAccess(cContainer& c, string& name, string& name1, double darg2, double darg3)
{
	cContainer t;

	if (lookup(t,name1)) {
		addError("undefined symbol: ", name1);
		return true;
	}

	if (t.GetType()!=VAR_TYPE_NUMERIC) {
		addError("illegal indexing: ", name1);
		return true;
	}

	return containerAccess(c, name, t, darg2, darg3);
}

bool cHash::containerAccess(cContainer& c, string& name, string& name1, double darg2, string& name3)
{
	cContainer t1, t3;

	if (lookup(t1,name1)) {
		addError("undefined symbol: ", name1);
		return true;
	}
	if (lookup(t3,name3)) {
		addError("undefined symbol: ", name3);
		return true;
	}

	if (t1.GetType()!=VAR_TYPE_NUMERIC) {
		addError("illegal indexing: ", name1);
		return true;
	}
	if (t3.GetType()!=VAR_TYPE_NUMERIC) {
		addError("illegal indexing: ", name3);
		return true;
	}

	return containerAccess(c, name, t1, darg2, t3);
}

bool cHash::containerAccess(cContainer& c, string& name, string& name1, string& name2, double darg3)
{
	cContainer t1, t2;

	if (lookup(t1,name1)) {
		addError("undefined symbol: ", name1);
		return true;
	}
	if (lookup(t2,name2)) {
		addError("undefined symbol: ", name2);
		return true;
	}

	if (t1.GetType()!=VAR_TYPE_NUMERIC) {
		addError("illegal indexing: ", name1);
		return true;
	}
	if (t2.GetType()!=VAR_TYPE_NUMERIC) {
		addError("illegal indexing: ", name2);
		return true;
	}

	return containerAccess(c, name, t1, t2, darg3);
}

bool cHash::containerAccess(cContainer& c, string& name, string& name1, string& name2, string& name3)
{
	cContainer t1, t2, t3;

	if (lookup(t1,name1)) {
		addError("undefined symbol: ", name1);
		return true;
	}
	if (lookup(t2,name2)) {
		addError("undefined symbol: ", name2);
		return true;
	}
	if (lookup(t3,name3)) {
		addError("undefined symbol: ", name3);
		return true;
	}

	if (t1.GetType()!=VAR_TYPE_NUMERIC) {
		addError("illegal indexing: ", name1);
		return true;
	}
	if (t2.GetType()!=VAR_TYPE_NUMERIC) {
		addError("illegal indexing: ", name2);
		return true;
	}
	if (t3.GetType()!=VAR_TYPE_NUMERIC) {
		addError("illegal indexing: ", name3);
		return true;
	}

	return containerAccess(c, name, t1, t2, t3);
}


bool cHash::containerAccess(cContainer& c, string& name, double darg1, cContainer& arg2, string& name3)
{
	cContainer t;

	if (lookup(t,name3)) {
		addError("undefined symbol: ", name3);
		return true;
	}

	if (t.GetType()!=VAR_TYPE_NUMERIC) {
		addError("illegal indexing: ", name3);
		return true;
	}

	return containerAccess(c, name, darg1, arg2, t);
}

bool cHash::containerAccess(cContainer& c, string& name, double darg1, string& name2, cContainer& arg3)
{
	cContainer t;

	if (lookup(t,name2)) {
		addError("undefined symbol: ", name2);
		return true;
	}

	if (t.GetType()!=VAR_TYPE_NUMERIC) {
		addError("illegal indexing: ", name2);
		return true;
	}

	return containerAccess(c, name, darg1, t, arg3);
}

bool cHash::containerAccess(cContainer& c, string& name, cContainer& arg1, double darg2, string& name3)
{
	cContainer t;

	if (lookup(t,name3)) {
		addError("undefined symbol: ", name3);
		return true;
	}

	if (t.GetType()!=VAR_TYPE_NUMERIC) {
		addError("illegal indexing: ", name3);
		return true;
	}

	return containerAccess(c, name, arg1, darg2, t);
}

bool cHash::containerAccess(cContainer& c, string& name, string& name1, double darg2, cContainer& arg3)
{
	cContainer t;

	if (lookup(t,name1)) {
		addError("undefined symbol: ", name1);
		return true;
	}

	if (t.GetType()!=VAR_TYPE_NUMERIC) {
		addError("illegal indexing: ", name1);
		return true;
	}

	return containerAccess(c, name, t, darg2, arg3);
}

bool cHash::containerAccess(cContainer& c, string& name, cContainer& arg1, string& name2, double darg3)
{
	cContainer t;

	if (lookup(t,name2)) {
		addError("undefined symbol: ", name2);
		return true;
	}

	if (t.GetType()!=VAR_TYPE_NUMERIC) {
		addError("illegal indexing: ", name2);
		return true;
	}

	return containerAccess(c, name, arg1, t, darg3);
}

bool cHash::containerAccess(cContainer& c, string& name, string& name1, cContainer& arg2, double darg3)
{
	cContainer t;

	if (lookup(t,name1)) {
		addError("undefined symbol: ", name1);
		return true;
	}

	if (t.GetType()!=VAR_TYPE_NUMERIC) {
		addError("illegal indexing: ", name1);
		return true;
	}

	return containerAccess(c, name, t, arg2, darg3);
}

bool cHash::containerAccess(cContainer& c, string& name, cContainer& arg1, cContainer& arg2, string& name3)
{
	cContainer t;

	if (lookup(t,name3)) {
		addError("undefined symbol: ", name3);
		return true;
	}

	if (t.GetType()!=VAR_TYPE_NUMERIC) {
		addError("illegal indexing: ", name3);
		return true;
	}

	return containerAccess(c, name, arg1, arg2, t);
}


bool cHash::containerAccess(cContainer& c, string& name, string& name1, string& name2, cContainer& arg3)
{
	cContainer t1, t2;

	if (lookup(t1,name1)) {
		addError("undefined symbol: ", name1);
		return true;
	}
	if (lookup(t2,name2)) {
		addError("undefined symbol: ", name2);
		return true;
	}

	if (t1.GetType()!=VAR_TYPE_NUMERIC) {
		addError("illegal indexing: ", name1);
		return true;
	}
	if (t2.GetType()!=VAR_TYPE_NUMERIC) {
		addError("illegal indexing: ", name2);
		return true;
	}

	return containerAccess(c, name, t1, t2, arg3);
}


//
// wild access (colon operator) - single arg
//
bool cHash::containerAccessColon(cContainer& c, string& name)
{
	c.Delete();

	int nType, nrows, ncols, ndeep;

	if (lookupInfo(name, nType, nrows, ncols, ndeep)) {
		addError("undefined symbol: ", name);
		return true;
	}

	int sz = nrows * ncols * ndeep;

	map<string, cMatrix>::iterator itm;
	map<string, cCube>::iterator itc;
	map<string, cString>::iterator its;

	switch (nType) {

	case VAR_TYPE_NUMERIC:
		itm = m_table_matrix.find(name);
		c.SetType( VAR_TYPE_NUMERIC );
		if (itm->second.IsReal()) {
			c.m.Create(sz,1);
			c.m.mr = itm->second.mr;
		}
		else {
			c.m.CreateComplex(sz,1);
			c.m.mr = itm->second.mr; c.m.mi = itm->second.mi;
		}
		break;

	case VAR_TYPE_CUBE:
		itc = m_table_cube.find(name);
		c.SetType( VAR_TYPE_CUBE );
		if (itc->second.IsReal()) {
			c.m.Create(sz,1);
			c.m.mr = itc->second.mr;
		}
		else {
			c.m.CreateComplex(sz,1);
			c.m.mr = itc->second.mr; c.m.mi = itc->second.mi;
		}
		break;

	case VAR_TYPE_STRING:
		its = m_table_string.find(name);
		c.SetType( VAR_TYPE_STRING );
		c.s = its->second;
		break;

	default:
		addError("illegal indexing"); return true;
	}

	return false;
}

//
// wild access - two arg
//
bool cHash::containerAccessColon(cContainer& c, string& name, bool b1, double darg2)
{
	int nType, nrows, ncols, ndeep;

	if (lookupInfo(name, nType, nrows, ncols, ndeep)) {
		addError("undefined symbol :", name); return true;
	}

	cMatrix m;
	m.SetToRange(0,nrows);
	cContainer arg1;
	arg1.SetTo(m);

	switch (nType) {
	case VAR_TYPE_NUMERIC:
		c.SetType(VAR_TYPE_NUMERIC);
		return matAccess(c.m, name, m, darg2);

	default:
		addError("illegal indexing");
		return true;
	}

	return false;
}

bool cHash::containerAccessColon(cContainer& c, string& name, bool b1, cContainer& arg2)
{
	int nType, nrows, ncols, ndeep;

	if (lookupInfo(name, nType, nrows, ncols, ndeep)) {
		addError("undefined symbol :", name); return true;
	}

	if (nType != VAR_TYPE_NUMERIC) {
		addError("illegal indexing");
		return true;
	}

	cMatrix m;
	m.SetToRange(0,nrows);

	switch (arg2.GetType()) {
	case VAR_TYPE_NUMERIC:
		c.SetType(VAR_TYPE_NUMERIC);
		return matAccess(c.m, name, m, arg2.m);

	default:
		addError("illegal indexing");
		return true;
	}

	return false;
}

bool cHash::containerAccessColon(cContainer& c, string& name, bool b1, string& name2)
{
	int nType, nrows, ncols, ndeep;

	if (lookupInfo(name, nType, nrows, ncols, ndeep)) {
		addError("undefined symbol :", name); return true;
	}

	if (nType != VAR_TYPE_NUMERIC) {
		addError("illegal indexing");
		return true;
	}

	cMatrix m;
	m.SetToRange(0,nrows);

	map<string, cMatrix>::iterator it = m_table_matrix.find(name2);
	if (it==m_table_matrix.end()) {
		addError("database error!");
		return true;
	}

	c.SetType(VAR_TYPE_NUMERIC);
	return matAccess(c.m, name, m, it->second);
}

bool cHash::containerAccessColon(cContainer& c, string& name, double darg1, bool b2)
{
	int nType, nrows, ncols, ndeep;

	if (lookupInfo(name, nType, nrows, ncols, ndeep)) {
		addError("undefined symbol :", name); return true;
	}

	cMatrix m;
	m.SetToRange(0,ncols);

	switch (nType) {
	case VAR_TYPE_NUMERIC:
		c.SetType(VAR_TYPE_NUMERIC);
		return matAccess(c.m, name, darg1, m);

	default:
		addError("illegal indexing");
		return true;
	}

	return false;
}

bool cHash::containerAccessColon(cContainer& c, string& name, cContainer& arg1, bool b2)
{
	int nType, nrows, ncols, ndeep;

	if (lookupInfo(name, nType, nrows, ncols, ndeep)) {
		addError("undefined symbol :", name); return true;
	}

	cMatrix m;
	m.SetToRange(0,ncols);

	if (nType != VAR_TYPE_NUMERIC) {
		addError("illegal indexing");
		return true;
	}

	switch (arg1.GetType()) {
	case VAR_TYPE_NUMERIC:
		c.SetType(VAR_TYPE_NUMERIC);
		return matAccess(c.m, name, arg1.m, m);

	default:
		addError("illegal indexing");
		return true;
	}

	return false;
}

bool cHash::containerAccessColon(cContainer& c, string& name, string& name1, bool b2)
{
	int nType, nrows, ncols, ndeep;

	if (lookupInfo(name, nType, nrows, ncols, ndeep)) {
		addError("undefined symbol :", name); return true;
	}

	if (nType != VAR_TYPE_NUMERIC) {
		addError("illegal indexing");
		return true;
	}

	cMatrix m;
	m.SetToRange(0,ncols);

	map<string, cMatrix>::iterator it = m_table_matrix.find(name1);
	if (it==m_table_matrix.end()) {
		addError("database error!");
		return true;
	}

	c.SetType(VAR_TYPE_NUMERIC);
	return matAccess(c.m, name, it->second, m);
}

bool cHash::containerAccessColon(cContainer& c, string& name, bool b1, bool b2)
{
	int nType, nrows, ncols, ndeep;

	if (lookupInfo(name, nType, nrows, ncols, ndeep)) {
		addError("undefined symbol :", name); return true;
	}

	if (nType != VAR_TYPE_NUMERIC) {
		addError("illegal indexing");
		return true;
	}

	// just return a copy of matrix with name "name"
	if (lookup(c, name)) {
		addError("undefined symbol: ", name);
		return true;
	}

	return false;
}

//
// wild access - three arg
//

// single colon
bool cHash::containerAccessColon(cContainer& c, string& name, bool b1, double darg2, double darg3)
{
	int nType, nrows, ncols, ndeep;

	if (lookupInfo(name, nType, nrows, ncols, ndeep)) {
		addError("undefined symbol :", name); return true;
	}

	cMatrix m;
	m.SetToRange(0,nrows);

	if (nType != VAR_TYPE_CUBE) {
		addError("illegal indexing");
		return true;
	}

	c.SetType(VAR_TYPE_NUMERIC);
	return cubeAccess(c.m, name, m, darg2, darg3);
}

bool cHash::containerAccessColon(cContainer& c, string& name, bool b1, double darg2, cContainer& arg3)
{
	int nType, nrows, ncols, ndeep;

	if (lookupInfo(name, nType, nrows, ncols, ndeep)) {
		addError("undefined symbol :", name); return true;
	}

	cMatrix m;
	m.SetToRange(0,nrows);

	if (nType != VAR_TYPE_CUBE && arg3.GetType() != VAR_TYPE_NUMERIC) {
		addError("illegal indexing");
		return true;
	}

	c.SetType(VAR_TYPE_NUMERIC);
	return cubeAccess(c.m, name, m, darg2, arg3.m);
}

bool cHash::containerAccessColon(cContainer& c, string& name, bool b1, cContainer& arg2, double darg3)
{
	int nType, nrows, ncols, ndeep;

	if (lookupInfo(name, nType, nrows, ncols, ndeep)) {
		addError("undefined symbol :", name); return true;
	}

	cMatrix m;
	m.SetToRange(0,nrows);

	if (nType != VAR_TYPE_CUBE && arg2.GetType() != VAR_TYPE_NUMERIC) {
		addError("illegal indexing");
		return true;
	}

	c.SetType(VAR_TYPE_NUMERIC);
	return cubeAccess(c.m, name, m, arg2.m, darg3);
}

bool cHash::containerAccessColon(cContainer& c, string& name, bool b1, cContainer& arg2, cContainer& arg3)
{
	int nType, nrows, ncols, ndeep;

	if (lookupInfo(name, nType, nrows, ncols, ndeep)) {
		addError("undefined symbol :", name); return true;
	}

	cMatrix m;
	m.SetToRange(0,nrows);

	if (nType != VAR_TYPE_CUBE && arg2.GetType() != VAR_TYPE_NUMERIC && arg3.GetType() != VAR_TYPE_NUMERIC) {
		addError("illegal indexing");
		return true;
	}

	c.SetType(VAR_TYPE_CUBE);
	return cubeAccess(c.c, name, m, arg2.m, arg3.m);
}

bool cHash::containerAccessColon(cContainer& c, string& name, bool b1, double darg2, string& argName)
{
	cContainer t;

	if (lookup(t, argName)) {
		addError("undefined symbol :", argName); return true;
	}

	return containerAccessColon(c, name, b1, darg2, t);
}

bool cHash::containerAccessColon(cContainer& c, string& name, bool b1, cContainer arg2, string& argName)
{
	cContainer t;

	if (lookup(t, argName)) {
		addError("undefined symbol :", argName); return true;
	}

	return containerAccessColon(c, name, b1, arg2, t);
}

bool cHash::containerAccessColon(cContainer& c, string& name, bool b1, string& argName, double darg3)
{
	cContainer t;

	if (lookup(t, argName)) {
		addError("undefined symbol :", argName); return true;
	}

	return containerAccessColon(c, name, b1, t, darg3);
}

bool cHash::containerAccessColon(cContainer& c, string& name, bool b1, string& argName2, string& argName3)
{
	cContainer t2, t3;

	if (lookup(t2, argName2)) {
		addError("undefined symbol :", argName2); return true;
	}
	if (lookup(t3, argName3)) {
		addError("undefined symbol :", argName2); return true;
	}

	return containerAccessColon(c, name, b1, t2, t3);
}

bool cHash::containerAccessColon(cContainer& c, string& name, double darg1, bool b2, double darg3)
{
	int nType, nrows, ncols, ndeep;

	if (lookupInfo(name, nType, nrows, ncols, ndeep)) {
		addError("undefined symbol :", name); return true;
	}

	cMatrix m;
	m.SetToRange(0,ncols);

	if (nType != VAR_TYPE_CUBE) {
		addError("illegal indexing");
		return true;
	}

	c.SetType(VAR_TYPE_NUMERIC);
	return cubeAccess(c.m, name, darg1, m, darg3);
}

bool cHash::containerAccessColon(cContainer& c, string& name, double darg1, bool b2, cContainer& arg3)
{
	int nType, nrows, ncols, ndeep;

	if (lookupInfo(name, nType, nrows, ncols, ndeep)) {
		addError("undefined symbol :", name); return true;
	}

	cMatrix m;
	m.SetToRange(0,ncols);

	if (nType != VAR_TYPE_CUBE && arg3.GetType() != VAR_TYPE_NUMERIC) {
		addError("illegal indexing");
		return true;
	}

	c.SetType(VAR_TYPE_NUMERIC);
	return cubeAccess(c.m, name, darg1, m, arg3.m);
}

bool cHash::containerAccessColon(cContainer& c, string& name, cContainer& arg1, bool b2, double darg3)
{
	int nType, nrows, ncols, ndeep;

	if (lookupInfo(name, nType, nrows, ncols, ndeep)) {
		addError("undefined symbol :", name); return true;
	}

	cMatrix m;
	m.SetToRange(0,ncols);

	if (nType != VAR_TYPE_CUBE && arg1.GetType() != VAR_TYPE_NUMERIC) {
		addError("illegal indexing");
		return true;
	}

	c.SetType(VAR_TYPE_NUMERIC);
	return cubeAccess(c.m, name, arg1.m, m, darg3);
}

bool cHash::containerAccessColon(cContainer& c, string& name, cContainer& arg1, bool b2, cContainer& arg3)
{
	int nType, nrows, ncols, ndeep;

	if (lookupInfo(name, nType, nrows, ncols, ndeep)) {
		addError("undefined symbol :", name); return true;
	}

	cMatrix m;
	m.SetToRange(0,ncols);

	if (nType != VAR_TYPE_CUBE && arg1.GetType() != VAR_TYPE_NUMERIC && arg3.GetType() != VAR_TYPE_NUMERIC) {
		addError("illegal indexing");
		return true;
	}

	c.SetType(VAR_TYPE_CUBE);
	return cubeAccess(c.c, name, arg1.m, m, arg3.m);
}

bool cHash::containerAccessColon(cContainer& c, string& name, double darg1, bool b2, string& argName)
{
	cContainer t;

	if (lookup(t, argName)) {
		addError("undefined symbol :", argName); return true;
	}

	return containerAccessColon(c, name, darg1, b2, t);
}

bool cHash::containerAccessColon(cContainer& c, string& name, string& argName, bool b2, double darg3)
{
	cContainer t;

	if (lookup(t, argName)) {
		addError("undefined symbol :", argName); return true;
	}

	return containerAccessColon(c, name, t, b2, darg3);
}

bool cHash::containerAccessColon(cContainer& c, string& name, string& argName1, bool b2, string& argName3)
{
	cContainer t1, t3;

	if (lookup(t1, argName1)) {
		addError("undefined symbol :", argName1); return true;
	}
	if (lookup(t3, argName3)) {
		addError("undefined symbol :", argName3); return true;
	}

	return containerAccessColon(c, name, t1, b2, t3);
}

bool cHash::containerAccessColon(cContainer& c, string& name, double darg1, double darg2, bool b3)
{
	int nType, nrows, ncols, ndeep;

	if (lookupInfo(name, nType, nrows, ncols, ndeep)) {
		addError("undefined symbol :", name); return true;
	}

	cMatrix m;
	m.SetToRange(0,ndeep);

	if (nType != VAR_TYPE_CUBE) {
		addError("illegal indexing");
		return true;
	}

	c.SetType(VAR_TYPE_NUMERIC);
	return cubeAccess(c.m, name, darg1, darg2, m);
}

bool cHash::containerAccessColon(cContainer& c, string& name, double darg1, cContainer& arg2, bool b3)
{
	int nType, nrows, ncols, ndeep;

	if (lookupInfo(name, nType, nrows, ncols, ndeep)) {
		addError("undefined symbol :", name); return true;
	}

	cMatrix m;
	m.SetToRange(0,ndeep);

	if (nType != VAR_TYPE_CUBE && arg2.GetType() != VAR_TYPE_NUMERIC) {
		addError("illegal indexing");
		return true;
	}

	c.SetType(VAR_TYPE_NUMERIC);
	return cubeAccess(c.m, name, darg1, arg2.m, m);
}

bool cHash::containerAccessColon(cContainer& c, string& name, cContainer& arg1, double darg2, bool b3)
{
	int nType, nrows, ncols, ndeep;

	if (lookupInfo(name, nType, nrows, ncols, ndeep)) {
		addError("undefined symbol :", name); return true;
	}

	cMatrix m;
	m.SetToRange(0,ndeep);

	if (nType != VAR_TYPE_CUBE && arg1.GetType() != VAR_TYPE_NUMERIC) {
		addError("illegal indexing");
		return true;
	}

	c.SetType(VAR_TYPE_NUMERIC);
	return cubeAccess(c.m, name, arg1.m, darg2, m);
}

bool cHash::containerAccessColon(cContainer& c, string& name, cContainer& arg1, cContainer& arg2, bool b3)
{
	int nType, nrows, ncols, ndeep;

	if (lookupInfo(name, nType, nrows, ncols, ndeep)) {
		addError("undefined symbol :", name); return true;
	}

	cMatrix m;
	m.SetToRange(0,ndeep);

	if (nType != VAR_TYPE_CUBE && arg1.GetType() != VAR_TYPE_NUMERIC && arg2.GetType() != VAR_TYPE_NUMERIC) {
		addError("illegal indexing");
		return true;
	}

	c.SetType(VAR_TYPE_CUBE);
	return cubeAccess(c.c, name, arg1.m, arg2.m, m);
}

bool cHash::containerAccessColon(cContainer& c, string& name, double darg1, string& argName, bool b3)
{
	cContainer t;

	if (lookup(t, argName)) {
		addError("undefined symbol :", argName); return true;
	}

	return containerAccessColon(c, name, darg1, t, b3);
}

bool cHash::containerAccessColon(cContainer& c, string& name, string& argName, double darg2, bool b3)
{
	cContainer t;

	if (lookup(t, argName)) {
		addError("undefined symbol :", argName); return true;
	}

	return containerAccessColon(c, name, t, darg2, b3);
}

bool cHash::containerAccessColon(cContainer& c, string& name, string& argName1, string& argName2, bool b3)
{
	cContainer t1, t2;

	if (lookup(t1, argName1)) {
		addError("undefined symbol :", argName1); return true;
	}
	if (lookup(t2, argName2)) {
		addError("undefined symbol :", argName2); return true;
	}

	return containerAccessColon(c, name, t1, t2, b3);
}


// two colons
bool cHash::containerAccessColon(cContainer& c, string& name, bool b1, bool b2, double darg3)
{
	int nType, nrows, ncols, ndeep;

	if (lookupInfo(name, nType, nrows, ncols, ndeep)) {
		addError("undefined symbol :", name); return true;
	}

	cMatrix m1, m2;
	m1.SetToRange(0,nrows);
	m2.SetToRange(0,ncols);

	if (nType != VAR_TYPE_CUBE) {
		addError("illegal indexing");
		return true;
	}

	c.SetType(VAR_TYPE_NUMERIC);
	return cubeAccess(c.m, name, m1, m2, darg3);
}

bool cHash::containerAccessColon(cContainer& c, string& name, bool b1, bool b2, cContainer& arg3)
{
	int nType, nrows, ncols, ndeep;

	if (lookupInfo(name, nType, nrows, ncols, ndeep)) {
		addError("undefined symbol :", name); return true;
	}

	cMatrix m1, m2;
	m1.SetToRange(0,nrows);
	m2.SetToRange(0,ncols);

	if (nType != VAR_TYPE_CUBE || arg3.GetType() != VAR_TYPE_NUMERIC) {
		addError("illegal indexing");
		return true;
	}

	c.SetType(VAR_TYPE_CUBE);
	return cubeAccess(c.c, name, m1, m2, arg3.m);
}

bool cHash::containerAccessColon(cContainer& c, string& name, bool b1, bool b2, string& name3)
{
	cContainer t;
	if (lookup(t,name3)) {
		addError("undefined symbol :", name3); return true;
	}

	return containerAccessColon(c, name, b1, b2, t);
}

bool cHash::containerAccessColon(cContainer& c, string& name, bool b1, double darg2, bool b3)
{
	int nType, nrows, ncols, ndeep;

	if (lookupInfo(name, nType, nrows, ncols, ndeep)) {
		addError("undefined symbol :", name); return true;
	}

	cMatrix m1, m2;
	m1.SetToRange(0,nrows);
	m2.SetToRange(0,ndeep);

	if (nType != VAR_TYPE_CUBE) {
		addError("illegal indexing");
		return true;
	}

	c.SetType(VAR_TYPE_NUMERIC);
	return cubeAccess(c.m, name, m1, darg2, m2);
}

bool cHash::containerAccessColon(cContainer& c, string& name, bool b1, cContainer& arg2, bool b3)
{
	int nType, nrows, ncols, ndeep;

	if (lookupInfo(name, nType, nrows, ncols, ndeep)) {
		addError("undefined symbol :", name); return true;
	}

	cMatrix m1, m2;
	m1.SetToRange(0,nrows);
	m2.SetToRange(0,ndeep);

	if (nType != VAR_TYPE_CUBE || arg2.GetType() != VAR_TYPE_NUMERIC) {
		addError("illegal indexing");
		return true;
	}

	c.SetType(VAR_TYPE_CUBE);
	return cubeAccess(c.c, name, m1, arg2.m, m2);
}
	
bool cHash::containerAccessColon(cContainer& c, string& name, bool b1, string& name2, bool b3)
{
	cContainer t;
	if (lookup(t,name2)) {
		addError("undefined symbol :", name2); return true;
	}

	return containerAccessColon(c, name, b1, t, b3);
}

bool cHash::containerAccessColon(cContainer& c, string& name, double darg1, bool b2, bool b3)
{
	int nType, nrows, ncols, ndeep;

	if (lookupInfo(name, nType, nrows, ncols, ndeep)) {
		addError("undefined symbol :", name); return true;
	}

	cMatrix m1, m2;
	m1.SetToRange(0,ncols);
	m2.SetToRange(0,ndeep);

	if (nType != VAR_TYPE_CUBE) {
		addError("illegal indexing");
		return true;
	}

	c.SetType(VAR_TYPE_CUBE);
	return cubeAccess(c.c, name, darg1, m1, m2);
}

bool cHash::containerAccessColon(cContainer& c, string& name, cContainer& arg1, bool b2, bool b3)
{
	int nType, nrows, ncols, ndeep;

	if (lookupInfo(name, nType, nrows, ncols, ndeep)) {
		addError("undefined symbol :", name); return true;
	}

	cMatrix m1, m2;
	m1.SetToRange(0,ncols);
	m2.SetToRange(0,ndeep);

	if (nType != VAR_TYPE_CUBE || arg1.GetType() != VAR_TYPE_NUMERIC) {
		addError("illegal indexing");
		return true;
	}

	c.SetType(VAR_TYPE_CUBE);
	return cubeAccess(c.c, name, arg1.m, m1, m2);
}

bool cHash::containerAccessColon(cContainer& c, string& name, string& name1, bool b2, bool b3)
{
	cContainer t;
	if (lookup(t,name1)) {
		addError("undefined symbol :", name1); return true;
	}

	return containerAccessColon(c, name, t, b2, b3);
}

// three colons
bool cHash::containerAccessColon(cContainer& c, string& name, bool b1, bool b2, bool b3)
{
	if (lookup(c, name)) {
		addError("undefined symbol :", name); return true;
	}
	return false;
}


