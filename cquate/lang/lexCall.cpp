
#include "container.h"
#include "interpret.h"

bool cInterpret::addCallIx(int& ix, cContainer& c)
{
	switch (c.GetType()) {
	case VAR_TYPE_NUMERIC:
		break;

	case VAR_TYPE_CUBE:
		ix += 1;
		break;

	case VAR_TYPE_STRING:
		ix += 2;
		break;

	case VAR_TYPE_HASH:
		ix += 3;
		break;

	};
	return false;
}

int cInterpret::getCallIx(cContainer& c)
{
	int ix(0);
	addCallIx(ix, c);
	return ix;
}

int cInterpret::getCallIx(cContainer& c1, cContainer& c2)
{
	int ix(0);
	addCallIx(ix, c1);

	ix = ix << 2;

	addCallIx(ix, c2);
	return ix;
}

int cInterpret::getCallIx(cContainer& c1, cContainer& c2, cContainer& c3)
{
	int ix(0);
	addCallIx(ix, c1);

	ix = ix << 2;

	addCallIx(ix, c2);

	ix = ix << 2;

	addCallIx(ix, c3);
	return ix;
}

int cInterpret::getCallIx(cContainer& c1, cContainer& c2, cContainer& c3, cContainer& c4)
{
	int ix(0);
	addCallIx(ix, c1);

	ix = ix << 2;

	addCallIx(ix, c2);

	ix = ix << 2;

	addCallIx(ix, c3);

	ix = ix << 2;

	addCallIx(ix, c3);
	return ix;
}

int cInterpret::getCallIx(vector<cContainer>& vc, int narg)
{	// for fcns with variable number of inputs (e.g., printf)
	int ix(0);
	int n = (int) vc.size();

	for (int i=n-narg; i<n; ++i)
	{
		addCallIx(ix, vc[i]);
	}
	return ix;
}

int cInterpret::getCallIxRev(vector<cContainer>& vc, int narg)
{	// for fcns with variable number of inputs (e.g., fprintf)
	int ix(0);

	for (int i=0; i<narg; ++i)
	{
		addCallIx(ix, vc[i]);
	}
	return ix;
}

int cInterpret::getCallIxRev(vector<cContainer>& vc, int nargin, int nargout)
{	// for fcns with variable number of ouinputs (e.g., fprintf)
	int n = nargin + nargout;
	int ix(0);

	for (int i=nargout; i<n; ++i)
	{
		addCallIx(ix, vc[i]);
	}
	return ix;
}

bool cInterpret::addArgIx(int& ix, int nargin, int nargout)
{
	// WARNING: this fcn only handles up to (nargin+nargout) < 64
	//
	for (int i=0; i<nargout; ++i) {
		++ix;
		if (i < (nargout-1))
			ix = ix << 1;
	}

	for (int i=0; i<(nargin-1); ++i) {
		ix  = ix << 1;
	}

	return false;
}

bool cInterpret::call(vector<valueBlob>& lblobs, keyData& key, tokData& tok)
{
	int nargout = (int) lblobs.size();
	cContainer c_out_1, c_out_2, c_out_3, c_out_4;

	//int ix = getCallIx(key,nargin);

	switch (nargout) {
	case 0:
		return call_0_arg(key, tok, tok.nargin, nargout);

	case 1:
		if (call_1_arg(key, tok, tok.nargin, nargout, c_out_1)) return true;
		lblobs[0].vResult.SetTo( c_out_1 );
		break;

	case 2:
		if (call_2_arg(key, tok, tok.nargin, nargout, c_out_1, c_out_2)) return true;
		lblobs[0].vResult.SetTo( c_out_1 );
		lblobs[1].vResult.SetTo( c_out_2 );
		break;

	case 3:
		if (call_3_arg(key, tok, tok.nargin, nargout, c_out_1, c_out_2, c_out_3)) return true;
		lblobs[0].vResult.SetTo( c_out_1 );
		lblobs[1].vResult.SetTo( c_out_2 );
		lblobs[2].vResult.SetTo( c_out_3 );
		break;

	case 4:
		if (call_4_arg(key, tok, tok.nargin, nargout, c_out_1, c_out_2, c_out_3, c_out_4)) return true;
		lblobs[0].vResult.SetTo( c_out_1 );
		lblobs[1].vResult.SetTo( c_out_2 );
		lblobs[2].vResult.SetTo( c_out_3 );
		lblobs[3].vResult.SetTo( c_out_4 );
		break;

	default:
		reportInterp("unsupported out args"); return true;
	}

	return false;
}

bool cInterpret::call(vector<valueBlob>& lblobs, keyData& key, tokData& tok, int nargin, cContainer& c)
{
	int nargout = (int) lblobs.size();
	cContainer c_out_1, c_out_2, c_out_3;

	//int ix = getCallIx(key,nargin);

	switch (nargout) {
	case 0:
		return call_1_arg(key, tok, tok.nargin, nargout, c);

	case 1:
		if (call_2_arg(key, tok, tok.nargin, nargout, c_out_1, c)) return true;
		lblobs[0].vResult.SetTo( c_out_1 );
		break;

	case 2:
		if (call_3_arg(key, tok, tok.nargin, nargout, c_out_1, c_out_2, c)) return true;
		lblobs[0].vResult.SetTo( c_out_1 );
		lblobs[1].vResult.SetTo( c_out_2 );
		break;

	case 3:
		if (call_4_arg(key, tok, tok.nargin, nargout, c_out_1, c_out_2, c_out_3, c)) return true;
		lblobs[0].vResult.SetTo( c_out_1 );
		lblobs[1].vResult.SetTo( c_out_2 );
		lblobs[2].vResult.SetTo( c_out_3 );
		break;

	default:
		reportInterp("unsupported out args"); return true;
	}

	return false;
}

bool cInterpret::call(vector<valueBlob>& lblobs, keyData& key, tokData& tok, int nargin, cContainer& c1, cContainer& c2)
{
	int nargout = (int) lblobs.size();
	cContainer c_out_1, c_out_2;

	//int ix = getCallIx(c1, c2);

	switch (nargout) {
	case 0:
		return call_2_arg(key, tok, tok.nargin, nargout, c1, c2);

	case 1:
		if (call_3_arg(key, tok, tok.nargin, nargout, c_out_1, c1, c2)) return true;
		lblobs[0].vResult.SetTo( c_out_1 );
		break;

	case 2:
		if (call_4_arg(key, tok, tok.nargin, nargout, c_out_1, c_out_2, c1, c2)) return true;
		lblobs[0].vResult.SetTo( c_out_1 );
		lblobs[1].vResult.SetTo( c_out_2 );
		break;

	default:
		reportInterp("unsupported out args"); return true;
	}

	return false;
}

bool cInterpret::call(vector<valueBlob>& lblobs, keyData& key, tokData& tok, int nargin, cContainer& c1, cContainer& c2, cContainer& c3)
{
	int nargout = (int) lblobs.size();
	cContainer c_out_1;

	//int ix = getCallIx(c1, c2, c3);

	switch (nargout) {
	case 0:
		return call_3_arg(key, tok, tok.nargin, nargout, c1, c2, c3);

	case 1:
		if (call_4_arg(key, tok, tok.nargin, nargout, c_out_1, c1, c2, c3)) return true;
		lblobs[0].vResult.SetTo( c_out_1 );
		break;

	default:
		reportInterp("unsupported out args"); return true;
	}

	return false;
}

bool cInterpret::call(vector<valueBlob>& lblobs, keyData& key, tokData& tok, int nargin, cContainer& c1, cContainer& c2, cContainer& c3, cContainer& c4)
{
	int nargout = (int) lblobs.size();
	cContainer c_out_1, c_out_2;

	int ix = getCallIx(c1, c2, c3, c4);

	switch (nargout) {
	case 0:
		return call_4_arg(key, tok, tok.nargin, nargout, c1, c2, c3, c4);

	default:
		reportInterp("unsupported out args"); return true;
	}

	return false;
}

void cInterpret::vc2vm(vector<cMatrix>& vm, vector<cContainer>& vc)
{
	vm.clear();
	for (int i=0; i<(int)vc.size(); ++i) {
		vm.push_back( vc[i].m );
	}
}

void cInterpret::vc2vs(vector<cString>& vs, vector<cContainer>& vc)
{
	vs.clear();
	for (int i=0; i<(int)vc.size(); ++i) {
		vs.push_back( vc[i].s );
	}
}

bool cInterpret::call(vector<valueBlob>& lblobs, keyData& key, tokData& tok, int nargin, vector<cContainer>& vc)
{
	int nargout = (int) lblobs.size();
	vector<cMatrix> vm;
	vector<cString> vs;
	cContainer c_out;

	if (vc.empty()) {
		reportInterp("missing argument", tok); return true;
	}
	
	if (vc.back().GetType()==VAR_TYPE_NUMERIC) {
		vc2vm(vm, vc);
		c_out.SetType(VAR_TYPE_NUMERIC);
		if (nargout == 0) {
			if (key.mat_ni_0o != NULL) {
				return key.mat_ni_0o( vm );
			}
		}
		else if (nargout == 1) {
			if (key.mat_ni_1o != NULL) {
				if (key.mat_ni_1o( c_out.m, vm )) return true;
				lblobs[0].vResult.SetTo( c_out );
			}
		}
		else { reportInterp("no function compatible with argument list provided", tok); return true; }
	}
	else if (vc.back().GetType()==VAR_TYPE_STRING) {
		vc2vs(vs, vc);
		c_out.SetType(VAR_TYPE_STRING);
		if (nargout == 0) {
			if (key.mat_ni_0o != NULL) {
				return key.str_ni_0o( vs );
			}
			else { reportInterp("no function compatible with argument list provided", tok); return true; }
		}
		else if (nargout == 1) {
			if (key.str_ni_1o != NULL) {
				if (key.str_ni_1o( c_out.s, vs )) return true;
				lblobs[0].vResult.SetTo( c_out );
			}
			else { reportInterp("no function compatible with argument list provided", tok); return true; }
		}
		else { reportInterp("no function compatible with argument list provided", tok); return true; }
	}
	return false;
}


bool cInterpret::call_vin_arg(keyData& key, tokData& tok, int nargin, int nargout, vector<cContainer>& vc)
{
	cMyPair pairIn, pairOut;
	int n;

	pairIn = key.i2o.rbegin()->first;		// just considering the first entry in the map. if more than one valid call to fcn, each must have .nUnknown set properly
	pairOut = key.i2o[pairIn];

	n = nargin - pairIn.narg - pairOut.narg;		// n = number of variable unknown args
	if (nargin + nargout != (int)vc.size()) {
		reportInterp("no function compatible with input arg(s) provided", tok);
		return true;
	}

	if (pairIn.narg + pairOut.narg > (int)vc.size()) {
		reportInterp("missing (or too many) arguments", tok);
		return true;
	}

	switch (pairIn.narg + pairOut.narg) {		// MINIMUM number of args (incl. known output)
	case 0:
		return call_vin_0_arg(key, tok, pairIn.narg, pairOut.narg, vc);		// e.g., show
	case 1:
		return call_vin_1_arg(key, tok, pairIn.narg, pairOut.narg, vc);		// e.g., show
	case 3:
		return call_vin_3_arg(key, tok, pairIn.narg, pairOut.narg, vc);		// e.g., fprintf

	default:
		reportInterp("no function compatible with input arg(s) provided", tok);
		return true;
	}

	return false;
}

bool cInterpret::call_vin_0_arg(keyData& key, tokData& tok, int nargin, int nargout, vector<cContainer>& vc)
{
	int narg = (int) vc.size();
	vector<cContainer> tvc;

	switch (nargin) {
	case 1:
		if (narg - 1 < 0) {
			reportInterp("no function compatible with argument list provided", tok);
			return true;
		}
		switch (narg - 1) {
		case 0:
			if (key.c != NULL) return key.c(vc[0]);
		default:
			if (key.vc != NULL) return key.vc( vc );
		}
	}

	reportInterp("no function compatible with argument list provided", tok);
	return true;
}

bool cInterpret::call_vin_1_arg(keyData& key, tokData& tok, int nargin, int nargout, vector<cContainer>& vc)
{
	// printf

	int narg = (int) vc.size();

	//if (nargin+nargout != narg) {
	//	reportInterp("missing (or too many) arguments", tok);
	//	return true;
	//}
	int ix = getCallIxRev( vc, nargin, nargout );

	switch (ix) {
	case 2:
		if (narg - 1 < 0) {
			reportInterp("no function compatible with argument list provided", tok);
			return true;
		}
		switch (narg - 1) {
		case 0:
			if (key.str_1 != NULL) return key.str_1(vc[0].s);
		case 1:
			if (key.sc != NULL) return key.sc(vc[0].s, vc[1]);
		case 2:
			if (key.scc != NULL) return key.scc(vc[0].s, vc[1], vc[2]);
		default:
			if (key.vc != NULL) return key.vc( vc );
		}
	}

	reportInterp("no function compatible with argument list provided", tok);
	return true;
}

bool cInterpret::call_vin_3_arg(keyData& key, tokData& tok, int nargin, int nargout, vector<cContainer>& vc)
{
	vc[0].SetType(VAR_TYPE_NUMERIC);

	int narg = (int) vc.size();
	if (nargin+nargout > narg) {
		reportInterp("missing arguments", tok);
		return true;
	}

	int ix = getCallIxRev( vc, nargin, nargout );
	vector<cContainer> tvc;


	switch (ix) {		// mat, mat, str (fprintf)
	case 2:
		if (narg - 3 < 0) {
			reportInterp("no function compatible with argument list provided", tok);
			return true;
		}
		switch (narg - 3) {
		case 0:
			if (key.mat2_str != NULL) return key.mat2_str(vc[0].m, vc[1].m, vc[2].s);
		default:
			tvc.insert( tvc.begin(), vc.begin() + 3, vc.end() );
			if (key.mmsvc != NULL) return key.mmsvc(vc[0].m, vc[1].m, vc[2].s, tvc);
		}
	}

	reportInterp("no function compatible with argument list provided", tok);
	return true;
}

bool cInterpret::call_vout_arg(keyData& key, tokData& tok, int nargin, int nargout, vector<cContainer>& vc)
{
	cMyPair pairIn, pairOut;
	int ix;

	pairIn = key.i2o.rbegin()->first;		// just considering the first entry in the map. if more than one valid call to fcn, each must have .nUnknown set properly
	pairOut = key.i2o[pairIn];

	if (nargin + nargout != (int)vc.size()) {
		reportInterp("illegal arg(s) provided", tok);
		return true;
	}

	ix = (int) vc.size() - 1;

	switch (pairIn.narg) {		// MINIMUM number of args (incl. known output)
	case 2:
		return call_vout_2_arg(key, tok, nargin, nargout, vc);

	case 3:
		return call_vout_3_arg(key, tok, nargin, nargout, vc);

	default:
		reportInterp("no function compatible with input arg(s) provided", tok);
		return true;
	}

	return false;
}

bool cInterpret::call_vout_2_arg(keyData& key, tokData& tok, int nargin, int nargout, vector<cContainer>& vc)
{
	int narg = (int) vc.size();
	int ix = getCallIx( vc, 2 );
	vector<cContainer> tvc;

	if (narg - 2 < 0) {
		reportInterp("no function compatible with argument list provided", tok);
		return true;
	}

	switch (ix) {		// (c1,...,cn) = fscanf(mat, str, c1, .. cn)
	case 0:
		switch (nargout) {
		case 0:
			if (key.mat_2 != NULL) return key.mat_2(vc[0].m, vc[1].m);
		//case 1:
		//	if (key.cmm != NULL) return key.cmm(vc[0], vc[1].m, vc[2].m);
		//case 2:
		//	if (key.ccmm != NULL) return key.ccmm(vc[0], vc[1], vc[2].m, vc[3].m);
		}
	case 2:
		switch (nargout) {
		case 0:
			if (key.mat_str != NULL) return key.mat_str(vc[0].m, vc[1].s);
		case 1:
			if (key.cms != NULL) return key.cms(vc[0], vc[1].m, vc[2].s);
		case 2:
			if (key.ccms != NULL) return key.ccms(vc[0], vc[1], vc[2].m, vc[3].s);
		case 3:
			if (key.cccms != NULL) return key.cccms(vc[0], vc[1], vc[2], vc[3].m, vc[4].s);
		default:
			tvc.insert( tvc.begin(), vc.begin() + 2, vc.end() );
			if (key.vcms != NULL) return key.vcms(tvc, vc[2].m, vc[3].s);
		}
	}

	reportInterp("no function compatible with argument list provided", tok);
	return true;
}

// 3 mandatory inputs, variable # outputs (e.g., knn_query)
bool cInterpret::call_vout_3_arg(keyData& key, tokData& tok, int nargin, int nargout, vector<cContainer>& vc)
{
	int narg = (int)vc.size();
	int ix = getCallIx(vc, 2);
	vector<cContainer> tvc;

	if (narg - 3 < 0) {
		reportInterp("no function compatible with argument list provided", tok);
		return true;
	}

	switch (ix) {		// (nn, dist) = knn_query(table, query, k)
	case 0:
		switch (nargout) {
		case 0:
			if (key.mat_3 != NULL) return key.mat_3(vc[0].m, vc[1].m, vc[2].m);
		//case 1:
		//	if (key.cmmm != NULL) return key.cmmm(vc[0], vc[1].m, vc[2].m, vc[3].m);
		//case 2:
		//	if (key.ccmmm != NULL) return key.ccmmm(vc[0], vc[1], vc[2].m, vc[3].m, vc[4].m);
		}
	}

	reportInterp("no function compatible with argument list provided", tok);
	return true;
}

bool cInterpret::call_0_arg(keyData& key, tokData& tok, int nargin, int nargout)
{
	// NOTE: all zero-argument fcns will be pointed to by mat_0
	if (key.mat_0 != NULL) return key.mat_0();
	else { reportInterp("no function compatible with argument list provided", tok); return true; }
}

bool cInterpret::call_1_arg(keyData& key, tokData& tok, int nargin, int nargout, cContainer& c)
{
	int n = nargin + nargout;
	if (n != 1) { reportInterp("system error: unexpected nargs != 1", tok); return true; }

	// clear output args first
	if (nargout > 0) {
		c.Delete();
	}

	cMyPair pairIn, pairOut;

	if (nargin==1) {
		pairIn.ix = getCallIx(c);
	}
	else {
		pairIn.ix = 0;
	}
	pairIn.narg = nargin;

	if (key.i2o.find(pairIn)==key.i2o.end()) {
		reportInterp("no function compatible with input arg(s) provided", tok);
		return true;
	}

	pairOut = key.i2o[pairIn];

	if (nargin == 1)
	{
		switch (pairIn.ix) {
		case 0:
			if (c.GetType() != VAR_TYPE_NUMERIC) { reportInterp("argument type not compatible with function", tok); return true; }
			if (key.mat_1 != NULL) {
				return key.mat_1( c.m );
			}
			else { reportInterp("no function compatible with argument list provided", tok); return true; }
			break;

		case 1:
			if (c.GetType() != VAR_TYPE_CUBE) { reportInterp("argument type not compatible with function", tok); return true; }
			if (key.cbe_1 != NULL) return key.cbe_1( c.c );
			else { reportInterp("no function compatible with argument list provided", tok); return true; }
			break;

		case 2:
			if (c.GetType() != VAR_TYPE_STRING) { reportInterp("argument type not compatible with function", tok); return true; }
			if (key.str_1 != NULL) return key.str_1( c.s );
			else { reportInterp("no function compatible with argument list provided", tok); return true; }
			break;

		case 3:
			if (c.GetType() != VAR_TYPE_HASH) { reportInterp("argument type not compatible with function", tok); return true; }
			if (key.hsh_1 != NULL) return key.hsh_1( c.h );
			else { reportInterp("no function compatible with argument list provided", tok); return true; }
			break;

		default:
			reportInterp("invalid argument type", tok); return true;
		};
	}
	else	// nargout = 1
	{
		switch (pairOut.ix) {
		case 0:
			c.SetType(VAR_TYPE_NUMERIC);
			if (key.mat_1 != NULL) return key.mat_1( c.m );
			else { reportInterp("no function compatible with argument list provided", tok); return true; }
			break;

		case 1:
			c.SetType(VAR_TYPE_CUBE);
			if (key.cbe_1 != NULL) return key.cbe_1( c.c );
			else { reportInterp("no function compatible with argument list provided", tok); return true; }
			break;

		case 2:
			c.SetType(VAR_TYPE_STRING);
			if (key.str_1 != NULL) return key.str_1( c.s );
			else { reportInterp("no function compatible with argument list provided", tok); return true; }
			break;

		case 3:
			c.SetType(VAR_TYPE_HASH);
			if (key.hsh_1 != NULL) return key.hsh_1( c.h );
			else { reportInterp("no function compatible with argument list provided", tok); return true; }
			break;

		default:
			reportInterp("invalid argument type", tok); return true;
		};
	}

	return false;
}

bool cInterpret::call_2_arg(keyData& key, tokData& tok, int nargin, int nargout, cContainer& c1, cContainer& c2)
{
	cMatrix m1, m2;
	cString s1, s2;
	cCube cb1, cb2;

	int n = nargin + nargout;
	if (n != 2) { reportInterp("system error: unexpected nargs != 2", tok); return true; }

	cMyPair pairIn, pairOut;
	pairIn.narg = nargin;

	switch (nargin) {
	case 0:
		pairIn.ix = 0; break;

	case 1:
		pairIn.ix = getCallIx(c2); break;
	
	case 2:
		pairIn.ix = getCallIx(c1, c2); break;
	}

	if (key.i2o.find(pairIn)==key.i2o.end()) {
		reportInterp("no function compatible with input arg(s) provided", tok);
		return true;
	}

	pairOut = key.i2o[pairIn];

	// clear output args first
	switch (nargout) {
	case 2:
		c2.Delete();

	case 1:
		c1.Delete();
	}

	// make the fcn call

	switch (nargin) {
	case 0:
		// two outputs
		return call_f2(key, tok, nargin, pairOut.ix, c1, c2);

	case 1:
		return call_f2(key, tok, nargin, pairOut.ix*4 + pairIn.ix, c1, c2);
	
	case 2:
		// two inputs
		return call_f2(key, tok, nargin, pairIn.ix, c1, c2);
	}

	return false;
}


bool cInterpret::call_3_arg(keyData& key, tokData& tok, int nargin, int nargout, cContainer& c1, cContainer& c2, cContainer& c3)
{
	// clear output args first
	switch (nargout) {
	case 3:
		c3.Delete();

	case 2:
		c2.Delete();

	case 1:
		c1.Delete();
	}

	cMyPair pairIn, pairOut;
	pairIn.narg = nargin;

	switch (nargin) {
	case 0:
		pairIn.ix = 0; break;

	case 1:
		pairIn.ix = getCallIx(c3); break;
	
	case 2:
		pairIn.ix = getCallIx(c2, c3); break;

	case 3:
		pairIn.ix = getCallIx(c1, c2, c3); break;
	}

	if (key.i2o.find(pairIn)==key.i2o.end()) {
		reportInterp("no function compatible with input arg(s) provided", tok);
		return true;
	}

	pairOut = key.i2o[pairIn];

	switch (nargin) {
	case 0:
		// 3 outputs
		return call_f3(key, tok, nargin, pairOut.ix, c1, c2, c3);

	case 1:
		// 2 outputs, 1 input
		return call_f3(key, tok, nargin, pairOut.ix*4 + pairIn.ix, c1, c2, c3);
	
	case 2:
		// 1 output, 2 inputs
		return call_f3(key, tok, nargin, pairOut.ix*16 + pairIn.ix, c1, c2, c3);

	case 3:
		// 3 inputs
		return call_f3(key, tok, nargin, pairIn.ix, c1, c2, c3);
	};

	return false;
}

bool cInterpret::call_4_arg(keyData& key, tokData& tok, int nargin, int nargout, cContainer& c1, cContainer& c2, cContainer& c3, cContainer& c4)
{
	// clear output args first
	switch (nargout) {
	case 4:
		c4.Delete();

	case 3:
		c3.Delete();

	case 2:
		c2.Delete();

	case 1:
		c1.Delete();
	}

	cMyPair pairIn, pairOut;
	pairIn.narg = nargin;

	switch (nargin) {
	case 0:
		pairIn.ix = 0; break;

	case 1:
		pairIn.ix = getCallIx(c4); break;
	
	case 2:
		pairIn.ix = getCallIx(c3, c4); break;

	case 3:
		pairIn.ix = getCallIx(c2, c3, c4); break;

	case 4:
		pairIn.ix = getCallIx(c1, c2, c3, c4); break;
	}

	if (key.i2o.find(pairIn)==key.i2o.end()) {
		reportInterp("no function compatible with input arg(s) provided", tok);
		return true;
	}

	pairOut = key.i2o[pairIn];

	switch (nargin) {
	case 0:
		// 4 outputs
		return call_f4(key, tok, nargin, pairOut.ix, c1, c2, c3, c4);

	case 1:
		// 3 outputs, 1 inputs
		return call_f4(key, tok, nargin, pairOut.ix*4 + pairIn.ix, c1, c2, c3, c4);
	
	case 2:
		// 2 output, 2 inputs
		return call_f4(key, tok, nargin, pairOut.ix*16 + pairIn.ix, c1, c2, c3, c4);

	case 3:
		// 1 output, 3 inputs
		return call_f4(key, tok, nargin, pairOut.ix*64 + pairIn.ix, c1, c2, c3, c4);

	case 4:
		// 4 inputs
		return call_f4(key, tok, nargin, pairIn.ix, c1, c2, c3, c4);
	};

	return false;
}

bool cInterpret::call_f2(keyData& key, tokData& tok, int nargin, int ix, cContainer& c1, cContainer& c2)
{
	switch (ix) {
	case 0:	// mat, mat
		if (nargin<=1) c1.SetType(VAR_TYPE_NUMERIC);
		if (nargin==0) c2.SetType(VAR_TYPE_NUMERIC);
		if (key.mat_2 != NULL) {
			if (key.mat_2( c1.m, c2.m ))
				return true;
		}
		else { reportInterp("no function compatible with argument list provided", tok); return true; }
		break;

	case 1:	// mat, cube
		if (nargin<=1) c1.SetType(VAR_TYPE_NUMERIC);
		if (nargin==0) c2.SetType(VAR_TYPE_CUBE);
		if (key.mat_cbe_2 != NULL) {
			if (key.mat_cbe_2( c1.m, c2.c ))
				return true;
		}
		else { reportInterp("no function compatible with argument list provided", tok); return true; }
		break;

	case 2:	// mat, str
		if (nargin<=1) c1.SetType(VAR_TYPE_NUMERIC);
		if (nargin==0) c2.SetType(VAR_TYPE_STRING);
		if (key.mat_str != NULL) {
			if (key.mat_str( c1.m, c2.s ))
				return true;
		}
		else { reportInterp("no function compatible with argument list provided", tok); return true; }
		break;

	case 3:	// mat, hsh
		if (nargin<=1) c1.SetType(VAR_TYPE_NUMERIC);
		if (nargin==0) c2.SetType(VAR_TYPE_HASH);
		if (key.mat_hsh != NULL) {
			if (key.mat_hsh( c1.m, c2.h ))
				return true;
		}
		else { reportInterp("no function compatible with argument list provided", tok); return true; }
		break;

	case 4: // cube, mat
		if (nargin<=1) c1.SetType(VAR_TYPE_CUBE);
		if (nargin==0) c2.SetType(VAR_TYPE_NUMERIC);
		if (key.mat_2 != NULL) {
			if (key.cbe_2( c1.c, c2.m ))
				return true;
		}
		else { reportInterp("no function compatible with argument list provided", tok); return true; }
		break;

	case 6: // cube, str
		if (nargin<=1) c1.SetType(VAR_TYPE_CUBE);
		if (nargin==0) c2.SetType(VAR_TYPE_STRING);
		if (key.cbe_str != NULL) {
			if (key.cbe_str( c1.c, c2.s ))
				return true;
		}
		else { reportInterp("no function compatible with argument list provided", tok); return true; }
		break;

	case 8: // str, mat
		if (nargin<=1) c1.SetType(VAR_TYPE_STRING);
		if (nargin==0) c2.SetType(VAR_TYPE_NUMERIC);
		if (key.str_mat != NULL) {
			if (key.str_mat( c1.s, c2.m ))
				return true;
		}
		else { reportInterp("no function compatible with argument list provided", tok); return true; }
		break;

	case 10:	// str, str
		if (nargin<=1) c1.SetType(VAR_TYPE_STRING);
		if (nargin==0) c2.SetType(VAR_TYPE_STRING);
		if (key.str_2 != NULL) {
			if (key.str_2( c1.s, c2.s ))
				return true;
		}
		else { reportInterp("no function compatible with argument list provided", tok); return true; }
		break;

	case 14:	// hash, str
		if (nargin<=1) c1.SetType(VAR_TYPE_HASH);
		if (nargin==0) c2.SetType(VAR_TYPE_STRING);
		if (key.hsh_str != NULL) {
			if (key.hsh_str( c1.h, c2.s ))
				return true;
		}
		else { reportInterp("no function compatible with argument list provided", tok); return true; }
		break;

	default:
		reportInterp("no function compatible with argument list provided", tok); return true;
	};
	
	return false;
}

bool cInterpret::call_f3(keyData& key, tokData& tok, int nargin, int ix, cContainer& c1, cContainer& c2, cContainer& c3)
{
	switch (ix) {
	case 0:	// mat, mat, mat
		if (nargin<=2) c1.SetType(VAR_TYPE_NUMERIC);
		if (nargin<=1) c2.SetType(VAR_TYPE_NUMERIC);
		if (nargin==0) c3.SetType(VAR_TYPE_NUMERIC);
		if (key.mat_3 != NULL) {
			if (key.mat_3( c1.m, c2.m, c3.m ))
				return true;
		}
		else { reportInterp("no function compatible with argument list provided", tok); return true; }
		break;

	case 5:	// mat, cube, cube
		if (nargin<=2) c1.SetType(VAR_TYPE_NUMERIC);
		if (nargin<=1) c2.SetType(VAR_TYPE_CUBE);
		if (nargin==0) c3.SetType(VAR_TYPE_CUBE);
		if (key.mat_cbe_3 != NULL) {
			if (key.mat_cbe_3( c1.m, c2.c, c3.c ))
				return true;
		}
		else { reportInterp("no function compatible with argument list provided", tok); return true; }
		break;

	case 10:	// mat, str, str
		if (nargin<=2) c1.SetType(VAR_TYPE_NUMERIC);
		if (nargin<=1) c2.SetType(VAR_TYPE_STRING);
		if (nargin==0) c3.SetType(VAR_TYPE_STRING);
		if (key.mat_str2 != NULL) {
			if (key.mat_str2( c1.m, c2.s, c3.s ))
				return true;
		}
		else { reportInterp("no function compatible with argument list provided", tok); return true; }
		break;

	case 14:
		if (nargin<=2) c1.SetType(VAR_TYPE_NUMERIC);
		if (nargin<=1) c2.SetType(VAR_TYPE_HASH);
		if (nargin==0) c3.SetType(VAR_TYPE_STRING);
		if (key.mat_hsh_str != NULL) {
			if (key.mat_hsh_str( c1.m, c2.h, c3.s ))
				return true;
		}
		else { reportInterp("no function compatible with argument list provided", tok); return true; }
		break;
		
	case 16: // cube, mat, mat
		if (nargin<=2) c1.SetType(VAR_TYPE_CUBE);
		if (nargin<=1) c2.SetType(VAR_TYPE_NUMERIC);
		if (nargin==0) c3.SetType(VAR_TYPE_NUMERIC);
		if (key.cbe_3 != NULL) {
			if (key.cbe_3( c1.c, c2.m, c3.m ))
				return true;
		}
		else { reportInterp("no function compatible with argument list provided", tok); return true; }
		break;

	case 20: // cube, cube, mat
		if (nargin<=2) c1.SetType(VAR_TYPE_CUBE);
		if (nargin<=1) c2.SetType(VAR_TYPE_CUBE);
		if (nargin==0) c3.SetType(VAR_TYPE_NUMERIC);
		if (key.cbe2_mat != NULL) {
			if (key.cbe2_mat( c1.c, c2.c, c3.m ))
				return true;
		}
		else { reportInterp("no function compatible with argument list provided", tok); return true; }
		break;

	case 21: // cube, cube, cube
		if (nargin<=2) c1.SetType(VAR_TYPE_CUBE);
		if (nargin<=1) c2.SetType(VAR_TYPE_CUBE);
		if (nargin==0) c3.SetType(VAR_TYPE_CUBE);
		if (key.cbe_cbe_3 != NULL) {
			if (key.cbe_cbe_3( c1.c, c2.c, c3.c ))
				return true;
		}
		else { reportInterp("no function compatible with argument list provided", tok); return true; }
		break;

	case 26: // cube, str, str
		if (nargin<=2) c1.SetType(VAR_TYPE_CUBE);
		if (nargin<=1) c2.SetType(VAR_TYPE_STRING);
		if (nargin==0) c3.SetType(VAR_TYPE_STRING);
		if (key.cbe_str2 != NULL) {
			if (key.cbe_str2( c1.c, c2.s, c3.s ))
				return true;
		}
		else { reportInterp("no function compatible with argument list provided", tok); return true; }
		break;

	case 40:	// str, str, mat
		if (nargin<=2) c1.SetType(VAR_TYPE_STRING);
		if (nargin<=1) c2.SetType(VAR_TYPE_STRING);
		if (nargin==0) c3.SetType(VAR_TYPE_NUMERIC);
		if (key.str2_mat != NULL) {
			if (key.str2_mat( c1.s, c2.s, c3.m ))
				return true;
		}
		else { reportInterp("no function compatible with argument list provided", tok); return true; }
		break;

	case 42:	// str, str, str
		if (nargin<=2) c1.SetType(VAR_TYPE_STRING);
		if (nargin<=1) c2.SetType(VAR_TYPE_STRING);
		if (nargin==0) c3.SetType(VAR_TYPE_STRING);
		if (key.str_3 != NULL) {
			if (key.str_3( c1.s, c2.s, c3.s ))
				return true;
		}
		else { reportInterp("no function compatible with argument list provided", tok); return true; }
		break;

	case 46:	// str, hsh, str
		if (nargin<=2) c1.SetType(VAR_TYPE_STRING);
		if (nargin<=1) c2.SetType(VAR_TYPE_HASH);
		if (nargin==0) c3.SetType(VAR_TYPE_STRING);
		if (key.str_hsh_str != NULL) {
			if (key.str_hsh_str( c1.s, c2.h, c3.s ))
				return true;
		}
		else { reportInterp("no function compatible with argument list provided", tok); return true; }
		break;

	case 62:	// hsh, hsh, str
		if (nargin<=2) c1.SetType(VAR_TYPE_HASH);
		if (nargin<=1) c2.SetType(VAR_TYPE_HASH);
		if (nargin==0) c3.SetType(VAR_TYPE_STRING);
		if (key.hsh2_str != NULL) {
			if (key.hsh2_str( c1.h, c2.h, c3.s ))
				return true;
		}
		else { reportInterp("no function compatible with argument list provided", tok); return true; }
		break;

	default:
		reportInterp("no function compatible with argument list provided", tok); return true;
	};
	
	return false;
}

bool cInterpret::call_f4(keyData& key, tokData& tok, int nargin, int ix, cContainer& c1, cContainer& c2, cContainer& c3, cContainer& c4)
{
	switch (ix) {
	case 0:	// mat, mat, mat, mat
		if (nargin<=3) c1.SetType(VAR_TYPE_NUMERIC);
		if (nargin<=2) c2.SetType(VAR_TYPE_NUMERIC);
		if (nargin<=1) c3.SetType(VAR_TYPE_NUMERIC);
		if (nargin==0) c4.SetType(VAR_TYPE_NUMERIC);
		if (key.mat_4 != NULL) {
			if (key.mat_4( c1.m, c2.m, c3.m, c4.m ))
				return true;
		}
		else { reportInterp("no function compatible with argument list provided", tok); return true; }
		break;

	case 40:	// mat, str, str, mat
		if (nargin<=3) c1.SetType(VAR_TYPE_NUMERIC);
		if (nargin<=2) c2.SetType(VAR_TYPE_STRING);
		if (nargin<=1) c3.SetType(VAR_TYPE_STRING);
		if (nargin==0) c4.SetType(VAR_TYPE_NUMERIC);
		if (key.mat_str2_mat != NULL) {
			if (key.mat_str2_mat( c1.m, c2.s, c3.s, c4.m ))
				return true;
		}
		else { reportInterp("no function compatible with argument list provided", tok); return true; }
		break;

	case 64:	// cube, mat, mat, mat
		if (nargin<=3) c1.SetType(VAR_TYPE_CUBE);
		if (nargin<=2) c2.SetType(VAR_TYPE_NUMERIC);
		if (nargin<=1) c3.SetType(VAR_TYPE_NUMERIC);
		if (nargin==0) c4.SetType(VAR_TYPE_NUMERIC);
		if (key.cbe_4 != NULL) {
			if (key.cbe_4( c1.c, c2.m, c3.m, c4.m ))
				return true;
		}
		else { reportInterp("no function compatible with argument list provided", tok); return true; }
		break;

	case 85:	// cube, cube, cube, cube
		if (nargin<=3) c1.SetType(VAR_TYPE_CUBE);
		if (nargin<=2) c2.SetType(VAR_TYPE_CUBE);
		if (nargin<=1) c3.SetType(VAR_TYPE_CUBE);
		if (nargin==0) c4.SetType(VAR_TYPE_CUBE);
		if (key.cbe_cbe_4 != NULL) {
			if (key.cbe_cbe_4( c1.c, c2.c, c3.c, c4.c ))
				return true;
		}
		else { reportInterp("no function compatible with argument list provided", tok); return true; }
		break;
		
	case 160:	// str, str, mat, mat
		if (nargin<=3) c1.SetType(VAR_TYPE_STRING);
		if (nargin<=2) c2.SetType(VAR_TYPE_STRING);
		if (nargin<=1) c3.SetType(VAR_TYPE_NUMERIC);
		if (nargin==0) c4.SetType(VAR_TYPE_NUMERIC);
		if (key.str2_mat2 != NULL) {
			if (key.str2_mat2( c1.s, c2.s, c3.m, c4.m ))
				return true;
		}
		else { reportInterp("no function compatible with argument list provided", tok); return true; }
		break;

	case 168:	// str, str, str, mat
		if (nargin<=3) c1.SetType(VAR_TYPE_STRING);
		if (nargin<=2) c2.SetType(VAR_TYPE_STRING);
		if (nargin<=1) c3.SetType(VAR_TYPE_STRING);
		if (nargin==0) c4.SetType(VAR_TYPE_NUMERIC);
		if (key.str3_mat != NULL) {
			if (key.str3_mat( c1.s, c2.s, c3.s, c4.m ))
				return true;
		}
		else { reportInterp("no function compatible with argument list provided", tok); return true; }
		break;

	case 170:	// str, str, str, str
		if (nargin<=3) c1.SetType(VAR_TYPE_STRING);
		if (nargin<=2) c2.SetType(VAR_TYPE_STRING);
		if (nargin<=1) c3.SetType(VAR_TYPE_STRING);
		if (nargin==0) c4.SetType(VAR_TYPE_STRING);
		if (key.str_4 != NULL) {
			if (key.str_4( c1.s, c2.s, c3.s, c4.s ))
				return true;
		}
		else { reportInterp("no function compatible with argument list provided", tok); return true; }
		break;

	case 248:	// hsh, hsh, str, mat
		if (nargin<=3) c1.SetType(VAR_TYPE_HASH);
		if (nargin<=2) c2.SetType(VAR_TYPE_HASH);
		if (nargin<=1) c3.SetType(VAR_TYPE_STRING);
		if (nargin==0) c4.SetType(VAR_TYPE_NUMERIC);
		if (key.hsh2_str_mat != NULL) {
			if (key.hsh2_str_mat( c1.h, c2.h, c3.s, c4.m ))
				return true;
		}
		else { reportInterp("no function compatible with argument list provided", tok); return true; }
		break;

	case 249:	// hsh, hsh, str, cbe
		if (nargin<=3) c1.SetType(VAR_TYPE_HASH);
		if (nargin<=2) c2.SetType(VAR_TYPE_HASH);
		if (nargin<=1) c3.SetType(VAR_TYPE_STRING);
		if (nargin==0) c4.SetType(VAR_TYPE_CUBE);
		if (key.hsh2_str_cbe != NULL) {
			if (key.hsh2_str_cbe( c1.h, c2.h, c3.s, c4.c ))
				return true;
		}
		else { reportInterp("no function compatible with argument list provided", tok); return true; }
		break;

	case 250:	// hsh, hsh, str, str
		if (nargin<=3) c1.SetType(VAR_TYPE_HASH);
		if (nargin<=2) c2.SetType(VAR_TYPE_HASH);
		if (nargin<=1) c3.SetType(VAR_TYPE_STRING);
		if (nargin==0) c4.SetType(VAR_TYPE_STRING);
		if (key.hsh2_str_str != NULL) {
			if (key.hsh2_str_str( c1.h, c2.h, c3.s, c4.s ))
				return true;
		}
		else { reportInterp("no function compatible with argument list provided", tok); return true; }
		break;
		
	case 251:	// hsh, hsh, str, hsh
		if (nargin<=3) c1.SetType(VAR_TYPE_HASH);
		if (nargin<=2) c2.SetType(VAR_TYPE_HASH);
		if (nargin<=1) c3.SetType(VAR_TYPE_STRING);
		if (nargin==0) c4.SetType(VAR_TYPE_HASH);
		if (key.hsh2_str_hsh != NULL) {
			if (key.hsh2_str_hsh( c1.h, c2.h, c3.s, c4.h ))
				return true;
		}
		else { reportInterp("no function compatible with argument list provided", tok); return true; }
		break;
	};

	return false;
}

bool cInterpret::callCmd(keyData& key, tokData& tok, int nargin, int nargout, cContainer& c)
{
	return call_1_arg(key, tok, nargin, nargout, c);
}

bool cInterpret::callCmd(keyData& key, tokData& tok, int nargin, int nargout, cContainer& c1, cContainer& c2)
{
	return call_2_arg(key, tok, nargin, nargout, c1, c2);
}

bool cInterpret::callCmd(keyData& key, tokData& tok, int nargin, int nargout, cContainer& c1, cContainer& c2, cContainer& c3)
{
	return call_3_arg(key, tok, nargin, nargout, c1, c2, c3);
}

bool cInterpret::callCmd(keyData& key, tokData& tok, int nargin, int nargout, cContainer& c1, cContainer& c2, cContainer& c3, cContainer& c4)
{
	return call_4_arg(key, tok, nargin, nargout, c1, c2, c3, c4);
}

