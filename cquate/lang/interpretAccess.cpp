
#include "interpret.h"


//
// lvalue access
//
bool cInterpret::doLvalueAccessOperation(valueBlob& blob /* variable */, tokData& arg /* arg */)
{
	if ( arg.nOperatorType == LEX_TYPE_COLON_ACCESSOR ) { blob.token = arg; }
	else if (arg.nOperatorType == LEX_OPERATOR_ACCESS_END) { blob.vArg.SetTo( double(blob.nRows*blob.nColumns*blob.nDepth - 1) ); }
	else {
		if ( getVar( blob.vArg, arg ) ) return true;
		if ( blob.vArg.GetType() != VAR_TYPE_NUMERIC && blob.vArg.GetType() != VAR_TYPE_CUBE ) {
			reportInterp("cannot index with non-numeric value", blob.token); return true;
		}
	}
	return false;
}

bool cInterpret::doLvalueAccessOperation(valueBlob& blob /* variable */, tokData& arg1 /* arg 1 */, tokData& arg2 /* arg 2 */)
{
	cContainer vRows, vCols;

	if ( arg1.nSymbolType == LEX_TYPE_COLON_ACCESSOR ) { vRows.SetToRange(0.,blob.nRows-1,1.); vRows.m.Trans(); }
	else if (arg1.nOperatorType == LEX_OPERATOR_ACCESS_END) { vRows.SetTo( double(blob.nRows - 1) ); }
	else { if ( getVar( vRows, arg1 ) ) { reportInterp("unknown identifier", arg1); return true; } }

	if ( arg2.nSymbolType == LEX_TYPE_COLON_ACCESSOR ) { vCols.SetToRange(0.,blob.nColumns-1,1.);  vCols.m.Trans(); }
	else if (arg2.nOperatorType == LEX_OPERATOR_ACCESS_END) { vCols.SetTo( double(blob.nColumns - 1) ); }
	else { if ( getVar( vCols, arg2 ) ) { reportInterp("unknown identifier", arg2); return true; } }

	if ( arg2ix( blob.vArg, vRows, vCols, blob.nRows, blob.nColumns ) ) { reportInterp("illegal arguments", blob.token); return true; }

	return false;
}

bool cInterpret::doLvalueAccessOperation(valueBlob& blob /* variable */, tokData& arg1 /* arg 1 */, tokData& arg2 /* arg 2 */, tokData& arg3 /* arg 3 */)
{
	cContainer vRows, vCols, vDepth;

	if ( arg1.nSymbolType == LEX_TYPE_COLON_ACCESSOR ) { vRows.SetToRange(0.,blob.nRows-1,1.); vRows.m.Trans(); }
	else if (arg1.nOperatorType == LEX_OPERATOR_ACCESS_END) { vRows.SetTo( double(blob.nRows - 1) ); }
	else { if ( getVar( vRows, arg1 ) ) { reportInterp("unknown identifier", arg1); return true; } }

	if ( arg2.nSymbolType == LEX_TYPE_COLON_ACCESSOR ) { vCols.SetToRange(0.,blob.nColumns-1,1.); vCols.m.Trans(); }
	else if (arg2.nOperatorType == LEX_OPERATOR_ACCESS_END) { vCols.SetTo( double(blob.nColumns - 1) ); }
	else { if ( getVar( vCols, arg2 ) ) { reportInterp("unknown identifier", arg2); return true; } }

	if ( arg3.nSymbolType == LEX_TYPE_COLON_ACCESSOR ) { vDepth.SetToRange(0.,blob.nDepth-1,1.);  vDepth.m.Trans(); }
	else if (arg3.nOperatorType == LEX_OPERATOR_ACCESS_END) { vDepth.SetTo( double(blob.nDepth - 1) ); }
	else { if ( getVar( vDepth, arg3 ) ) { reportInterp("unknown identifier", arg3); return true; } }

	if ( arg2ix( blob.vArg, vRows, vCols, vDepth, blob.nRows, blob.nColumns, blob.nDepth ) ) { reportInterp("illegal arguments", blob.token); return true; }

	return false;
}


//
// hash access
//
bool cInterpret::doRvalueAccessOperationHash(cContainer& v, tokData& o /* hash */, vector<tokData>& args /* args */)
{
	cContainer t;
	vector<string> strArgs;
	
	for (int i=0; i<(int)args.size(); ++i)
	{

		if (tok2container(t, args[i])) return true;
		if (t.GetType() != VAR_TYPE_STRING) { addError("non-string argument for hash"); return true; }
		strArgs.push_back( t.s.GetString() );
	}

	return m_pSymbolTable->hashGet(v, o.name, strArgs);
}

bool cInterpret::doRvalueAccessOperationHashOneArg(cContainer& v, tokData& o /* hash */, tokData& arg /* arg */)
{
	cContainer t;

	switch (o.nSymbolType)
	{
	case LEX_TYPE_CONSTANT:
		reportInterp("illegal operation: hash access");
		return true;

	case LEX_TYPE_RVALUE:
		return rvalueAccessHash(v, m_rvalue, arg);

	case LEX_TYPE_UNKNOWN:
		//if (m_pSymbolTable->lookup(o.name)) { reportInterp("undefined symbol", o); return true; }
		// fall through...
		
	case LEX_TYPE_VARIABLE:
		if (m_pSymbolTable->lookup(t, o.name)) { reportInterp("undefined symbol", o); return true; }
		return rvalueAccessHash(v, t, arg);

	case LEX_TYPE_TEMP_VARIABLE:
		return rvalueAccessHash(v, m_tempVars[o.ix], arg);

	default:
		reportInterp("illegal hash reference", o);
		return true;
	};

	return false;
}

bool cInterpret::doRvalueAccessOperationHash(cContainer& v, tokData& o /* hash */, tokData& arg /* key */)
{
	cContainer t;

	switch (o.nSymbolType)
	{
	case LEX_TYPE_CONSTANT:
		reportInterp("illegal operation: hash access");
		return true;

	case LEX_TYPE_RVALUE:
		return rvalueAccess(v, m_rvalue, arg);

	case LEX_TYPE_UNKNOWN:
		//if (m_pSymbolTable->lookup(o.name)) { reportInterp("undefined symbol", o); return true; }
		// fall through...
		
	case LEX_TYPE_VARIABLE:
		if (m_pSymbolTable->lookup(t, o.name)) { reportInterp("undefined symbol", o); return true; }
		return rvalueAccessHash(v, t, arg);

	case LEX_TYPE_TEMP_VARIABLE:
		return rvalueAccessHash(v, m_tempVars[o.ix], arg);

	default:
		reportInterp("illegal hash reference", o);
		return true;
	};

	return false;
}

bool cInterpret::rvalueAccessHash(cContainer& v, cContainer& var, tokData& arg)
{
	cContainer t;

	switch (arg.nSymbolType)
	{
	case LEX_TYPE_RVALUE:
		return Retrieve(v, var, m_rvalue);

	case LEX_TYPE_UNKNOWN:
		// fall through...
		
	case LEX_TYPE_VARIABLE:
		if (m_pSymbolTable->lookup(t, arg.name)) { reportInterp("undefined symbol", arg); return true; }
		return Retrieve(v, var, t);

	case LEX_TYPE_TEMP_VARIABLE:
		return Retrieve(v, var, m_tempVars[arg.ix]);

	case LEX_TYPE_STRING:
		t.SetTo(arg.name);
		return Retrieve(v, var, t);

	default:
		reportInterp("illegal hash argument", arg);
		return true;
	};

	return false;
}

bool cInterpret::GetSize(double& lfsz, tokData& o)
{
	cContainer t;

	switch (o.nSymbolType)
	{
	case LEX_TYPE_CONSTANT:
		lfsz = o.fSymbolVal;
		return false;

	case LEX_TYPE_RVALUE:
		lfsz = (double) m_rvalue.GetSize();
		return false;

	case LEX_TYPE_UNKNOWN:
		//if (m_pSymbolTable->lookup(o.name)) { reportInterp("undefined symbol", o); return true; }
		// fall through...
		
	case LEX_TYPE_VARIABLE:
		if (m_pSymbolTable->lookup(t, o.name)) { reportInterp("undefined symbol", o); return true; }
		lfsz = (double) t.GetSize();
		return false;

	case LEX_TYPE_TEMP_VARIABLE:
		lfsz = (double) m_tempVars[o.ix].GetSize();
		return false;

	default:
		reportInterp("illegal array argument", o);
		return true;
	};

	return false;
}

//
// 1 arg
//

bool cInterpret::doRvalueAccessOperation(cContainer& v, tokData& o /* matrix, vect, or ':' operator */, tokData& arg /* arg */)
{
	cContainer t;

	switch (o.nSymbolType)
	{
	case LEX_TYPE_CONSTANT:
		return rvalueAccess(v, o.fSymbolVal, arg);

	case LEX_TYPE_UNKNOWN:
		// fall through...
		
	case LEX_TYPE_VARIABLE:
		return rvalueAccess(v, o.name, arg);

	default:
		reportInterp("illegal array argument", o);
		return true;
	};

	return false;
}

bool cInterpret::rvalueAccess(cContainer& v, string& name, tokData& arg)
{
	int nType, nrows, ncols, ndeep;

	switch (arg.nSymbolType)
	{
	case LEX_OPERATOR_ACCESS_END:
		if (m_pSymbolTable->lookupInfo(name, nType, nrows, ncols, ndeep)) {
			reportInterp("undefined symbol", arg.nLineNo, name);
			return true;
		}
		arg.fSymbolVal = nrows*ncols*ndeep - 1;

	case LEX_TYPE_CONSTANT:
		if (m_pSymbolTable->containerAccess(v, name, arg.fSymbolVal)) {
			reportInterp("illegal array argument", arg);
			return true;
		}
		break;

	case LEX_TYPE_RVALUE:
		if (m_pSymbolTable->containerAccess(v, name, m_rvalue)) { reportInterp("illegal array argument", arg); return true; }
		break;

	case LEX_TYPE_UNKNOWN:
		// fall through...

	case LEX_TYPE_VARIABLE:
		if (m_pSymbolTable->containerAccess(v, name, arg.name)) { reportInterp("illegal array argument", arg); return true; }
		break;

	case LEX_TYPE_TEMP_VARIABLE:
		if (m_pSymbolTable->containerAccess(v, name, m_tempVars[arg.ix])) { return true; }
		break;

	case LEX_TYPE_COLON_ACCESSOR:
		return m_pSymbolTable->containerAccessColon(v, name);
		break;

	default:
		reportInterp("illegal array argument", arg);
		return true;
	};

	return false;
}

bool cInterpret::rvalueAccess(cContainer& v, double d, tokData& arg)
{
	cContainer t;

	switch (arg.nSymbolType)
	{
	case LEX_TYPE_CONSTANT:
		if (containerAccess(v, d, arg.fSymbolVal)) { reportInterp("illegal array argument", arg); return true; }
		break;

	case LEX_OPERATOR_ACCESS_END:
		if (containerAccess(v, d, 0.)) { reportInterp("illegal array argument", arg); return true; }
		break;

	case LEX_TYPE_RVALUE:
		if (containerAccess(v, d, m_rvalue)) { reportInterp("illegal array argument", arg); return true; }
		break;

	case LEX_TYPE_UNKNOWN:
		// fall through...

	case LEX_TYPE_VARIABLE:
		if (m_pSymbolTable->lookup(t, arg.name)) { reportInterp("undefined symbol", arg); return true; }
		if (containerAccess(v, d, t)) { reportInterp("illegal array argument", arg); return true; }
		break;

	case LEX_TYPE_TEMP_VARIABLE:
		if (containerAccess(v, d, m_tempVars[arg.ix])) { reportInterp("illegal array argument", arg); return true; }
		break;

	case LEX_TYPE_COLON_ACCESSOR:
		if (containerAccessColon(v, d)) { reportInterp("illegal array argument", arg); return true; }
		break;

	default:
		reportInterp("illegal array argument", arg);
		return true;
	};

	return false;
}

bool cInterpret::rvalueAccess(cContainer& v, cContainer& var, tokData& arg)
{
	cContainer t;
	string name = var.GetName();

	switch (arg.nSymbolType)
	{
	case LEX_OPERATOR_ACCESS_END:
		arg.fSymbolVal = var.GetSize() - 1;
		// fall through..

	case LEX_TYPE_CONSTANT:
		if (m_pSymbolTable->containerAccess(v, name, arg.fSymbolVal)) { reportInterp("illegal array argument", arg); return true; }
		break;

	case LEX_TYPE_RVALUE:
		if (m_pSymbolTable->containerAccess(v, name, m_rvalue)) { reportInterp("illegal array argument", arg); return true; }
		break;

	case LEX_TYPE_UNKNOWN:
		if (m_pSymbolTable->lookup(arg.name)) //m_pSymbolTable->getAddr(arg.ix, arg.ixh, arg.name))
			{ reportInterp("undefined symbol", arg); return true; }
		// fall through...

	case LEX_TYPE_VARIABLE:
		if (m_pSymbolTable->containerAccess(v, name, arg.name)) { reportInterp("illegal array argument", arg); return true; }
		break;

	case LEX_TYPE_TEMP_VARIABLE:
		if (m_pSymbolTable->containerAccess(v, name, m_tempVars[arg.ix])) { reportInterp("illegal array argument", arg); return true; }
		break;

	case LEX_TYPE_COLON_ACCESSOR:
		if (m_pSymbolTable->containerAccessColon(v, name)) { reportInterp("illegal array argument", arg); return true; }
		break;

	default:
		reportInterp("illegal array argument", arg);
		return true;
	};

	return false;
}


//
// 2 args
//

bool cInterpret::doRvalueAccessOperation(cContainer& v, tokData& o /* matrix or vect */, tokData& arg1 /* arg 1 */, tokData& arg2 /* arg 2 */)
{
	cContainer t;

	switch (o.nSymbolType)
	{
	case LEX_TYPE_CONSTANT:
		return rvalueAccess(v, o.fSymbolVal, arg1, arg2);

	case LEX_TYPE_UNKNOWN:
		// fall through...

	case LEX_TYPE_VARIABLE:
		return rvalueAccess(v, o.name, arg1, arg2);

	default:
		reportInterp("illegal array argument", o);
		return true;
	};

	return false;
}

bool cInterpret::rvalueAccess(cContainer& v, double dvar, tokData& arg1, tokData& arg2)
{
	cContainer t;

	switch (arg1.nSymbolType)
	{
	case LEX_OPERATOR_ACCESS_END:
		arg1.fSymbolVal = 0.;
		// fall through..

	case LEX_TYPE_CONSTANT:
		return rvalueAccess(v, dvar, arg1.fSymbolVal, arg2);

	case LEX_TYPE_RVALUE:
		return rvalueAccess(v, dvar, m_rvalue, arg2);

	case LEX_TYPE_UNKNOWN:
		// fall through...

	case LEX_TYPE_VARIABLE:
		if (m_pSymbolTable->lookup(t, arg1.name)) { reportInterp("undefined symbol", arg1); return true; }
		return rvalueAccess(v, dvar, t, arg2);

	case LEX_TYPE_TEMP_VARIABLE:
		return rvalueAccess(v, dvar, m_tempVars[arg1.ix], arg2);

	case LEX_TYPE_COLON_ACCESSOR:
		return rvalueAccessColon(v, dvar, true, arg2);

	default:
		reportInterp("illegal array argument", arg1);
		return true;
	};

	return false;
}

bool cInterpret::rvalueAccess(cContainer& v, double dvar, cContainer& arg1, tokData& arg2)
{
	cContainer t;

	switch (arg2.nSymbolType)
	{
	case LEX_OPERATOR_ACCESS_END:
		arg2.fSymbolVal = 0.;
		// fall through..

	case LEX_TYPE_CONSTANT:
		if (containerAccess(v, dvar, arg1, arg2.fSymbolVal)) { reportInterp("illegal array argument", arg2); return true; }
		break;

	case LEX_TYPE_RVALUE:
		if (containerAccess(v, dvar, arg1, m_rvalue)) { reportInterp("illegal array argument", arg2); return true; }
		break;

	case LEX_TYPE_UNKNOWN:
		// fall through...

	case LEX_TYPE_VARIABLE:
		if (m_pSymbolTable->lookup(t, arg2.name)) { reportInterp("undefined symbol", arg2); return true; }
		if (containerAccess(v, dvar, arg1, t)) { reportInterp("illegal array argument", arg2); return true; }
		break;

	case LEX_TYPE_TEMP_VARIABLE:
		if (containerAccess(v, dvar, arg1, m_tempVars[arg2.ix])) { reportInterp("illegal array argument", arg2); return true; }
		break;

	case LEX_TYPE_COLON_ACCESSOR:
		return rvalueAccessColon(v, dvar, true, arg2);

	default:
		reportInterp("illegal array argument", arg2);
		return true;
	};

	return false;
}

bool cInterpret::rvalueAccess(cContainer& v, double dvar, double darg1, tokData& arg2)
{
	cContainer t;

	switch (arg2.nSymbolType)
	{
	case LEX_OPERATOR_ACCESS_END:
		arg2.fSymbolVal = 0.;
		// fall through..

	case LEX_TYPE_CONSTANT:
		if (containerAccess(v, dvar, darg1, arg2.fSymbolVal)) { reportInterp("illegal array argument", arg2); return true; }
		break;

	case LEX_TYPE_RVALUE:
		if (containerAccess(v, dvar, darg1, m_rvalue)) { reportInterp("illegal array argument", arg2); return true; }
		break;

	case LEX_TYPE_UNKNOWN:
		// fall through...

	case LEX_TYPE_VARIABLE:
		if (m_pSymbolTable->lookup(t, arg2.name)) { reportInterp("undefined symbol", arg2); return true; }
		if (containerAccess(v, dvar, darg1, t)) { reportInterp("illegal array argument", arg2); return true; }
		break;

	case LEX_TYPE_TEMP_VARIABLE:
		if (containerAccess(v, dvar, darg1, m_tempVars[arg2.ix])) { reportInterp("illegal array argument", arg2); return true; }
		break;

	case LEX_TYPE_COLON_ACCESSOR:
		return containerAccessColon(v, dvar, darg1, true);

	default:
		reportInterp("illegal array argument", arg2);
		return true;
	};

	return false;
}

bool cInterpret::rvalueAccess(cContainer& v, double dvar, tokData& arg1, double darg2)
{
	cContainer t;

	switch (arg1.nSymbolType)
	{
	case LEX_OPERATOR_ACCESS_END:
		arg1.fSymbolVal = 0.;
		// fall through..

	case LEX_TYPE_CONSTANT:
		if (containerAccess(v, dvar, arg1.fSymbolVal, darg2)) { reportInterp("illegal array argument", arg1); return true; }
		break;

	case LEX_TYPE_RVALUE:
		if (containerAccess(v, dvar, m_rvalue, darg2)) { reportInterp("illegal array argument", arg1); return true; }
		break;

	case LEX_TYPE_UNKNOWN:
		// fall through...

	case LEX_TYPE_VARIABLE:
		if (m_pSymbolTable->lookup(t, arg1.name)) { reportInterp("undefined symbol", arg1); return true; }
		if (containerAccess(v, dvar, t, darg2)) { reportInterp("illegal array argument", arg1); return true; }
		break;

	case LEX_TYPE_TEMP_VARIABLE:
		if (containerAccess(v, dvar, m_tempVars[arg1.ix], darg2)) { reportInterp("illegal array argument", arg1); return true; }
		break;

	case LEX_TYPE_COLON_ACCESSOR:
		return containerAccessColon(v, dvar, true, darg2);

	default:
		reportInterp("illegal array argument", arg1);
		return true;
	};

	return false;
}

bool cInterpret::rvalueAccess(cContainer& v, double dvar, double darg1, double darg2)
{
	if (containerAccess(v, dvar, darg1, darg2)) { reportInterp("illegal array argument"); return true; }
	return false;
}


bool cInterpret::rvalueAccessColon(cContainer& v, double dvar, bool b1, tokData& arg2)
{
	cContainer t;

	switch (arg2.nSymbolType)
	{
	case LEX_OPERATOR_ACCESS_END:
		arg2.fSymbolVal = 0.;
		// fall through..

	case LEX_TYPE_CONSTANT:
		if (containerAccessColon(v, dvar, b1, arg2.fSymbolVal)) { reportInterp("illegal array argument", arg2); return true; }
		break;

	case LEX_TYPE_RVALUE:
		if (containerAccessColon(v, dvar, b1, m_rvalue)) { reportInterp("illegal array argument", arg2); return true; }
		break;

	case LEX_TYPE_UNKNOWN:
		// fall through...

	case LEX_TYPE_VARIABLE:
		if (m_pSymbolTable->lookup(t, arg2.name)) { reportInterp("undefined symbol", arg2); return true; }
		if (containerAccessColon(v, dvar, b1, t)) { reportInterp("illegal array argument", arg2); return true; }
		break;

	case LEX_TYPE_TEMP_VARIABLE:
		if (containerAccessColon(v, dvar, b1, m_tempVars[arg2.ix])) { reportInterp("illegal array argument", arg2); return true; }
		break;

	case LEX_TYPE_COLON_ACCESSOR:
		return containerAccessColon(v, dvar, true, true);

	default:
		reportInterp("illegal array argument", arg2);
		return true;
	};

	return false;
}


bool cInterpret::rvalueAccessColon(cContainer& v, string& name, bool b1, tokData& arg2)
{
	int nType, nrows, ncols, ndeep;

	if (m_pSymbolTable->lookupInfo(name, nType, nrows, ncols, ndeep)) {
		reportInterp("undefined symbol", arg2.nLineNo, name);
		return true;
	}

	switch (arg2.nSymbolType)
	{
	case LEX_OPERATOR_ACCESS_END:
		arg2.fSymbolVal = ncols - 1;
		// fall through..

	case LEX_TYPE_CONSTANT:
		if (m_pSymbolTable->containerAccessColon(v, name, b1, arg2.fSymbolVal)) { reportInterp("illegal array argument", arg2); return true; }
		break;

	case LEX_TYPE_RVALUE:
		if (m_pSymbolTable->containerAccessColon(v, name, b1, m_rvalue)) { reportInterp("illegal array argument", arg2); return true; }
		break;

	case LEX_TYPE_UNKNOWN:
		// fall through...

	case LEX_TYPE_VARIABLE:
		if (m_pSymbolTable->containerAccessColon(v, name, b1, arg2.name)) { reportInterp("illegal array argument", arg2); return true; }
		break;

	case LEX_TYPE_TEMP_VARIABLE:
		if (m_pSymbolTable->containerAccessColon(v, name, b1, m_tempVars[arg2.ix])) { reportInterp("illegal array argument", arg2); return true; }
		break;

	case LEX_TYPE_COLON_ACCESSOR:
		return m_pSymbolTable->containerAccessColon(v, name, true, true);

	default:
		reportInterp("illegal array argument", arg2);
		return true;
	};

	return false;
}

bool cInterpret::rvalueAccess(cContainer& v, string& name, tokData& arg1, tokData& arg2)
{
	cContainer t;
	double lfsz;
	int nType, nrows, ncols, ndeep;

	switch (arg1.nSymbolType)
	{
	case LEX_OPERATOR_ACCESS_END:
		if (m_pSymbolTable->lookupInfo(name, nType, nrows, ncols, ndeep)) {
			reportInterp("undefined symbol", arg1.nLineNo, name);
			return true;
		}
		lfsz = nrows - 1;
		arg1.fSymbolVal = lfsz;
		// fall through..

	case LEX_TYPE_CONSTANT:
		return rvalueAccess(v, name, arg1.fSymbolVal, arg2);

	case LEX_TYPE_RVALUE:
		return rvalueAccess(v, name, m_rvalue, arg2);

	case LEX_TYPE_UNKNOWN:
		// fall through...

	case LEX_TYPE_VARIABLE:
		return rvalueAccess(v, name, arg1.name, arg2);

	case LEX_TYPE_TEMP_VARIABLE:
		return rvalueAccess(v, name, m_tempVars[arg1.ix], arg2);

	case LEX_TYPE_COLON_ACCESSOR:
		return rvalueAccessColon(v, name, true, arg2);

	default:
		reportInterp("illegal array argument", arg1);
		return true;
	};

	return false;
}

bool cInterpret::rvalueAccess(cContainer& v, string& name, cContainer& arg1, tokData& arg2)
{
	int nType, nrows, ncols, ndeep;
	double lfsz;

	if (arg1.GetType() != VAR_TYPE_NUMERIC) {
		reportInterp("illegal argument");
		return true;
	}

	switch (arg2.nSymbolType)
	{
	case LEX_OPERATOR_ACCESS_END:
		if (m_pSymbolTable->lookupInfo(name, nType, nrows, ncols, ndeep)) {
			reportInterp("undefined symbol: ", arg2.nLineNo, name);
			return true;
		}
		lfsz = nrows - 1;
		arg2.fSymbolVal = lfsz;
		// fall through..

	case LEX_TYPE_CONSTANT:
		v.SetType(VAR_TYPE_NUMERIC);
		return m_pSymbolTable->matAccess(v.m, name, arg1.m, arg2.fSymbolVal);

	case LEX_TYPE_RVALUE:
		return m_pSymbolTable->containerAccess(v, name, arg1, m_rvalue);

	case LEX_TYPE_UNKNOWN:
		// fall through...

	case LEX_TYPE_VARIABLE:
		return m_pSymbolTable->containerAccess(v, name, arg1, arg2.name);

	case LEX_TYPE_TEMP_VARIABLE:
		return m_pSymbolTable->containerAccess(v, name, arg1, m_tempVars[arg2.ix]);

	case LEX_TYPE_COLON_ACCESSOR:
		return m_pSymbolTable->containerAccessColon(v, name, arg1, true);

	default:
		reportInterp("illegal array argument", arg2);
		return true;
	};

	return false;
}

bool cInterpret::rvalueAccess(cContainer& v, string& name, double darg1, tokData& arg2)
{
	int nType, nrows, ncols, ndeep;
	double lfsz;

	switch (arg2.nSymbolType)
	{
	case LEX_OPERATOR_ACCESS_END:
		if (m_pSymbolTable->lookupInfo(name, nType, nrows, ncols, ndeep)) {
			reportInterp("undefined symbol: ", arg2.nLineNo, name);
			return true;
		}
		lfsz = nrows - 1;
		arg2.fSymbolVal = lfsz;
		// fall through..

	case LEX_TYPE_CONSTANT:
		v.SetType(VAR_TYPE_NUMERIC);
		return m_pSymbolTable->matAccess(v.m, name, darg1, arg2.fSymbolVal);

	case LEX_TYPE_RVALUE:
		return m_pSymbolTable->containerAccess(v, name, darg1, m_rvalue);

	case LEX_TYPE_UNKNOWN:
		// fall through...

	case LEX_TYPE_VARIABLE:
		if (m_pSymbolTable->lookupInfo(nType, arg2.name)) { reportInterp("undefined symbol", arg2); return true; }
		if (nType!=VAR_TYPE_NUMERIC) { reportInterp("illegal indexing", arg2); return true; }
		v.SetType(VAR_TYPE_NUMERIC);
		return m_pSymbolTable->matAccess(v.m, name, darg1, arg2.name);

	case LEX_TYPE_TEMP_VARIABLE:
		return m_pSymbolTable->containerAccess(v, name, darg1, m_tempVars[arg2.ix]);

	case LEX_TYPE_COLON_ACCESSOR:
		return m_pSymbolTable->containerAccessColon(v, name, darg1, true);

	default:
		reportInterp("illegal array argument", arg2);
		return true;
	};

	return false;
}

bool cInterpret::rvalueAccess(cContainer& v, string& name, string& name1, tokData& arg2)
{
	int nType, nrows, ncols, ndeep;
	double lfsz;

	switch (arg2.nSymbolType)
	{
	case LEX_OPERATOR_ACCESS_END:
		if (m_pSymbolTable->lookupInfo(name, nType, nrows, ncols, ndeep)) {
			reportInterp("undefined symbol: ", arg2.nLineNo, name);
			return true;
		}
		lfsz = nrows - 1;
		arg2.fSymbolVal = lfsz;
		// fall through..

	case LEX_TYPE_CONSTANT:
		v.SetType(VAR_TYPE_NUMERIC);
		return m_pSymbolTable->matAccess(v.m, name, name1, arg2.fSymbolVal);

	case LEX_TYPE_RVALUE:
		return m_pSymbolTable->containerAccess(v, name, name1, m_rvalue);

	case LEX_TYPE_UNKNOWN:
		// fall through...

	case LEX_TYPE_VARIABLE:
		if (m_pSymbolTable->lookupInfo(nType, arg2.name)) { reportInterp("undefined symbol", arg2); return true; }
		if (nType!=VAR_TYPE_NUMERIC) { reportInterp("illegal indexing", arg2); return true; }
		v.SetType(VAR_TYPE_NUMERIC);
		return m_pSymbolTable->matAccess(v.m, name, name1, arg2.name);

	case LEX_TYPE_TEMP_VARIABLE:
		return m_pSymbolTable->containerAccess(v, name, name1, m_tempVars[arg2.ix]);

	case LEX_TYPE_COLON_ACCESSOR:
		return m_pSymbolTable->containerAccessColon(v, name, name1, true);

	default:
		reportInterp("illegal array argument", arg2);
		return true;
	};

	return false;
}


//
// 3 args
//

bool cInterpret::doRvalueAccessOperation(cContainer& v, tokData& o /* matrix or vect */, tokData& arg1 /* arg 1 */, tokData& arg2 /* arg 2 */, tokData& arg3 /* arg 3 */)
{
	cContainer t;

	switch (o.nSymbolType)
	{
	case LEX_TYPE_CONSTANT:
		return rvalueAccess(v, o.fSymbolVal, arg1, arg2, arg3);

	case LEX_TYPE_UNKNOWN:
		// fall through...

	case LEX_TYPE_VARIABLE:
		return rvalueAccess(v, o.name, arg1, arg2, arg3);

	default:
		reportInterp("illegal array argument", o);
		return true;
	};

	return false;
}

bool cInterpret::rvalueAccess(cContainer& v, double dvar, tokData& arg1, tokData& arg2, tokData& arg3)
{
	cContainer t;

	switch (arg1.nSymbolType)
	{
	case LEX_OPERATOR_ACCESS_END:
		arg1.fSymbolVal = 0.;
		// fall through..

	case LEX_TYPE_CONSTANT:
		return rvalueAccess(v, dvar, arg1.fSymbolVal, arg2, arg3);

	case LEX_TYPE_RVALUE:
		return rvalueAccess(v, dvar, m_rvalue, arg2, arg3);

	case LEX_TYPE_UNKNOWN:
		// fall through...

	case LEX_TYPE_VARIABLE:
		if (m_pSymbolTable->lookup(t, arg1.name)) { reportInterp("undefined symbol", arg1); return true; }
		return rvalueAccess(v, dvar, t, arg2, arg3);

	case LEX_TYPE_TEMP_VARIABLE:
		return rvalueAccess(v, dvar, m_tempVars[arg1.ix], arg2, arg3);

	case LEX_TYPE_COLON_ACCESSOR:
		return rvalueAccessColon(v, dvar, true, arg2, arg3);

	default:
		reportInterp("illegal array argument", arg1);
		return true;
	};

	return false;
}

bool cInterpret::rvalueAccess(cContainer& v, string& name, tokData& arg1, tokData& arg2, tokData& arg3)
{
	cContainer t;
	int nType, nrows, ncols, ndeep;

	switch (arg1.nSymbolType)
	{
	case LEX_OPERATOR_ACCESS_END:
		if (m_pSymbolTable->lookupInfo(name, nType, nrows, ncols, ndeep)) {
			addError("undefined symbol: ", name);
			return true;
		}
		arg1.fSymbolVal = nrows - 1;
		// fall through..

	case LEX_TYPE_CONSTANT:
		return rvalueAccess(v, name, arg1.fSymbolVal, arg2, arg3);

	case LEX_TYPE_RVALUE:
		return rvalueAccess(v, name, m_rvalue, arg2, arg3);

	case LEX_TYPE_UNKNOWN:
		// fall through...

	case LEX_TYPE_VARIABLE:
		return rvalueAccess(v, name, arg1.name, arg2, arg3);

	case LEX_TYPE_TEMP_VARIABLE:
		return rvalueAccess(v, name, m_tempVars[arg1.ix], arg2, arg3);

	case LEX_TYPE_COLON_ACCESSOR:
		if (m_pSymbolTable->lookup(t, name)) {
			addError("undefined symbol: ", name);
			return true;
		}
		return rvalueAccessColon(v, t, true, arg2, arg3);

	default:
		reportInterp("illegal array argument", arg1);
		return true;
	};

	return false;
}

bool cInterpret::rvalueAccess(cContainer& v, string& name, string& name1, tokData& arg2, tokData& arg3)
{
	cContainer t, t1;
	int nType, nrows, ncols, ndeep;

	switch (arg2.nSymbolType)
	{
	case LEX_OPERATOR_ACCESS_END:
		if (m_pSymbolTable->lookupInfo(name, nType, nrows, ncols, ndeep)) {
			addError("undefined symbol: ", name);
			return true;
		}
		arg2.fSymbolVal = ncols - 1;
		// fall through..

	case LEX_TYPE_CONSTANT:
		return rvalueAccess(v, name, name1, arg2.fSymbolVal, arg3);

	case LEX_TYPE_RVALUE:
		return rvalueAccess(v, name, m_rvalue, arg2, arg3);

	case LEX_TYPE_UNKNOWN:
		// fall through...

	case LEX_TYPE_VARIABLE:
		return rvalueAccess(v, name, name1, arg2.name, arg3);

	case LEX_TYPE_TEMP_VARIABLE:
		if (m_pSymbolTable->lookup(t1, name1)) {
			addError("undefined symbol: ", name1);
			return true;
		}
		return rvalueAccess(v, name, t1, m_tempVars[arg2.ix], arg3);

	case LEX_TYPE_COLON_ACCESSOR:
		if (m_pSymbolTable->lookup(t, name)) {
			addError("undefined symbol: ", name);
			return true;
		}
		if (m_pSymbolTable->lookup(t1, name1)) {
			addError("undefined symbol: ", name1);
			return true;
		}
		return rvalueAccessColon(v, t, t1, true, arg3);

	default:
		reportInterp("illegal array argument", arg2);
		return true;
	};

	return false;
}

bool cInterpret::rvalueAccess(cContainer& v, string& name, string& name1, double darg2, tokData& arg3)
{
	int nType, nrows, ncols, ndeep;

	switch (arg3.nSymbolType)
	{
	case LEX_OPERATOR_ACCESS_END:
		if (m_pSymbolTable->lookupInfo(name, nType, nrows, ncols, ndeep)) {
			addError("undefined symbol: ", name);
			return true;
		}
		arg3.fSymbolVal = ndeep - 1;
		// fall through..

	case LEX_TYPE_CONSTANT:
		return m_pSymbolTable->containerAccess(v, name, name1, darg2, arg3.fSymbolVal);

	case LEX_TYPE_RVALUE:
		return m_pSymbolTable->containerAccess(v, name, name1, darg2, m_rvalue);

	case LEX_TYPE_UNKNOWN:
		// fall through...

	case LEX_TYPE_VARIABLE:
		return m_pSymbolTable->containerAccess(v, name, name1, darg2, arg3.name);

	case LEX_TYPE_TEMP_VARIABLE:
		return m_pSymbolTable->containerAccess(v, name, name1, darg2, m_tempVars[arg3.ix]);

	case LEX_TYPE_COLON_ACCESSOR:
		return m_pSymbolTable->containerAccessColon(v, name, name1, darg2, true);

	default:
		reportInterp("illegal array argument", arg3);
		return true;
	};

	return false;
}

bool cInterpret::rvalueAccess(cContainer& v, string& name, string& name1, string& name2, tokData& arg3)
{
	int nType, nrows, ncols, ndeep;

	switch (arg3.nSymbolType)
	{
	case LEX_OPERATOR_ACCESS_END:
		if (m_pSymbolTable->lookupInfo(name, nType, nrows, ncols, ndeep)) {
			addError("undefined symbol: ", name);
			return true;
		}
		arg3.fSymbolVal = ndeep - 1;
		// fall through..

	case LEX_TYPE_CONSTANT:
		return m_pSymbolTable->containerAccess(v, name, name1, name2, arg3.fSymbolVal);

	case LEX_TYPE_RVALUE:
		return m_pSymbolTable->containerAccess(v, name, name1, name2, m_rvalue);

	case LEX_TYPE_UNKNOWN:
		// fall through...

	case LEX_TYPE_VARIABLE:
		return m_pSymbolTable->containerAccess(v, name, name1, name2, arg3.name);

	case LEX_TYPE_TEMP_VARIABLE:
		return m_pSymbolTable->containerAccess(v, name, name1, name2, m_tempVars[arg3.ix]);

	case LEX_TYPE_COLON_ACCESSOR:
		return m_pSymbolTable->containerAccessColon(v, name, name1, name2, true);

	default:
		reportInterp("illegal array argument", arg3);
		return true;
	};

	return false;
}

bool cInterpret::rvalueAccess(cContainer& v, double dvar, double darg1, tokData& arg2, tokData& arg3)
{
	cContainer t;

	switch (arg2.nSymbolType)
	{
	case LEX_OPERATOR_ACCESS_END:
		arg2.fSymbolVal = 0.;
		// fall through..

	case LEX_TYPE_CONSTANT:
		return rvalueAccess(v, dvar, darg1, arg2.fSymbolVal, arg3);

	case LEX_TYPE_RVALUE:
		return rvalueAccess(v, dvar, darg1, m_rvalue, arg3);

	case LEX_TYPE_UNKNOWN:
		// fall through...

	case LEX_TYPE_VARIABLE:
		if (m_pSymbolTable->lookup(t, arg2.name)) { reportInterp("undefined symbol", arg2); return true; }
		return rvalueAccess(v, dvar, darg1, t, arg3);

	case LEX_TYPE_TEMP_VARIABLE:
		return rvalueAccess(v, dvar, darg1, m_tempVars[arg2.ix], arg3);

	case LEX_TYPE_COLON_ACCESSOR:
		return rvalueAccessColon(v, dvar, darg1, true, arg3);

	default:
		reportInterp("illegal array argument", arg2);
		return true;
	};

	return false;
}

bool cInterpret::rvalueAccess(cContainer& v, double dvar, cContainer& arg1, tokData& arg2, tokData& arg3)
{
	cContainer t;

	switch (arg2.nSymbolType)
	{
	case LEX_OPERATOR_ACCESS_END:
		arg2.fSymbolVal = 0.;
		// fall through..

	case LEX_TYPE_CONSTANT:
		return rvalueAccess(v, dvar, arg1, arg2.fSymbolVal, arg3);

	case LEX_TYPE_RVALUE:
		return rvalueAccess(v, dvar, arg1, m_rvalue, arg3);

	case LEX_TYPE_UNKNOWN:
		// fall through...

	case LEX_TYPE_VARIABLE:
		if (m_pSymbolTable->lookup(t, arg2.name)) { reportInterp("undefined symbol", arg2); return true; }
		return rvalueAccess(v, dvar, arg1, t, arg3);

	case LEX_TYPE_TEMP_VARIABLE:
		return rvalueAccess(v, dvar, arg1, m_tempVars[arg2.ix], arg3);

	case LEX_TYPE_COLON_ACCESSOR:
		return rvalueAccessColon(v, dvar, arg1, true, arg3);

	default:
		reportInterp("illegal array argument", arg2);
		return true;
	};

	return false;
}

bool cInterpret::rvalueAccess(cContainer& v, string& name, double darg1, tokData& arg2, tokData& arg3)
{
	cContainer t;

	switch (arg2.nSymbolType)
	{
	case LEX_OPERATOR_ACCESS_END:
		arg2.fSymbolVal = 0.;
		// fall through..

	case LEX_TYPE_CONSTANT:
		return rvalueAccess(v, name, darg1, arg2.fSymbolVal, arg3);

	case LEX_TYPE_RVALUE:
		return rvalueAccess(v, name, darg1, m_rvalue, arg3);

	case LEX_TYPE_UNKNOWN:
		// fall through...

	case LEX_TYPE_VARIABLE:
		if (m_pSymbolTable->lookup(t, arg2.name)) { reportInterp("undefined symbol", arg2); return true; }
		return rvalueAccess(v, name, darg1, t, arg3);

	case LEX_TYPE_TEMP_VARIABLE:
		return rvalueAccess(v, name, darg1, m_tempVars[arg2.ix], arg3);

	case LEX_TYPE_COLON_ACCESSOR:
		if (m_pSymbolTable->lookup(t, name)) { reportInterp("undefined symbol: ", arg2.nLineNo, name); return true; }
		return rvalueAccessColon(v, t, darg1, true, arg3);

	default:
		reportInterp("illegal array argument", arg2);
		return true;
	};

	return false;
}

bool cInterpret::rvalueAccess(cContainer& v, string& name, cContainer& arg1, tokData& arg2, tokData& arg3)
{
	cContainer t;
	int nType, nrows, ncols, ndeep;

	switch (arg2.nSymbolType)
	{
	case LEX_OPERATOR_ACCESS_END:
		if (m_pSymbolTable->lookupInfo(name, nType, nrows, ncols, ndeep)) {
			reportInterp("undefined symbol: ", arg2.nLineNo, name); return true;
		}
		arg2.fSymbolVal = ncols - 1;
		// fall through..

	case LEX_TYPE_CONSTANT:
		return rvalueAccess(v, name, arg1, arg2.fSymbolVal, arg3);

	case LEX_TYPE_RVALUE:
		return rvalueAccess(v, name, arg1, m_rvalue, arg3);

	case LEX_TYPE_UNKNOWN:
		// fall through...

	case LEX_TYPE_VARIABLE:
		if (m_pSymbolTable->lookup(t, arg2.name)) { reportInterp("undefined symbol", arg2); return true; }
		return rvalueAccess(v, name, arg1, t, arg3);

	case LEX_TYPE_TEMP_VARIABLE:
		return rvalueAccess(v, name, arg1, m_tempVars[arg2.ix], arg3);

	case LEX_TYPE_COLON_ACCESSOR:
		if (m_pSymbolTable->lookup(t, name)) { reportInterp("undefined symbol: ", arg2.nLineNo, name); return true; }
		return rvalueAccessColon(v, t, arg1, true, arg3);

	default:
		reportInterp("illegal array argument", arg2);
		return true;
	};

	return false;
}

bool cInterpret::rvalueAccess(cContainer& v, double dvar, double darg1, double darg2, tokData& arg3)
{
	cContainer t;

	switch (arg3.nSymbolType)
	{
	case LEX_OPERATOR_ACCESS_END:
		arg3.fSymbolVal = 0.;
		// fall through..

	case LEX_TYPE_CONSTANT:
		return containerAccess(v, dvar, darg1, darg2, arg3.fSymbolVal);

	case LEX_TYPE_RVALUE:
		return containerAccess(v, dvar, darg1, darg2, m_rvalue);

	case LEX_TYPE_UNKNOWN:
		// fall through...

	case LEX_TYPE_VARIABLE:
		if (m_pSymbolTable->lookup(t, arg3.name)) { reportInterp("undefined symbol", arg3); return true; }
		return containerAccess(v, dvar, darg1, darg2, t);

	case LEX_TYPE_TEMP_VARIABLE:
		return containerAccess(v, dvar, darg1, darg2, m_tempVars[arg3.ix]);

	case LEX_TYPE_COLON_ACCESSOR:
		return containerAccessColon(v, dvar, darg1, darg2, true);

	default:
		reportInterp("illegal array argument", arg3);
		return true;
	};

	return false;
}

bool cInterpret::rvalueAccess(cContainer& v, double dvar, double darg1, cContainer& arg2, tokData& arg3)
{
	cContainer t;

	switch (arg3.nSymbolType)
	{
	case LEX_OPERATOR_ACCESS_END:
		arg3.fSymbolVal = 0.;
		// fall through..

	case LEX_TYPE_CONSTANT:
		return containerAccess(v, dvar, darg1, arg2, arg3.fSymbolVal);

	case LEX_TYPE_RVALUE:
		return containerAccess(v, dvar, darg1, arg2, m_rvalue);

	case LEX_TYPE_UNKNOWN:
		// fall through...

	case LEX_TYPE_VARIABLE:
		if (m_pSymbolTable->lookup(t, arg3.name)) { reportInterp("undefined symbol", arg3); return true; }
		return containerAccess(v, dvar, darg1, arg2, t);

	case LEX_TYPE_TEMP_VARIABLE:
		return containerAccess(v, dvar, darg1, arg2, m_tempVars[arg3.ix]);

	case LEX_TYPE_COLON_ACCESSOR:
		return containerAccessColon(v, dvar, darg1, arg2, true);

	default:
		reportInterp("illegal array argument", arg3);
		return true;
	};

	return false;
}

bool cInterpret::rvalueAccess(cContainer& v, double dvar, cContainer& arg1, double darg2, tokData& arg3)
{
	cContainer t;

	switch (arg3.nSymbolType)
	{
	case LEX_OPERATOR_ACCESS_END:
		arg3.fSymbolVal = 0.;
		// fall through..

	case LEX_TYPE_CONSTANT:
		return containerAccess(v, dvar, arg1, darg2, arg3.fSymbolVal);

	case LEX_TYPE_RVALUE:
		return containerAccess(v, dvar, arg1, darg2, m_rvalue);

	case LEX_TYPE_UNKNOWN:
		// fall through...

	case LEX_TYPE_VARIABLE:
		if (m_pSymbolTable->lookup(t, arg3.name)) { reportInterp("undefined symbol", arg3); return true; }
		return containerAccess(v, dvar, arg1, darg2, t);

	case LEX_TYPE_TEMP_VARIABLE:
		return containerAccess(v, dvar, arg1, darg2, m_tempVars[arg3.ix]);

	case LEX_TYPE_COLON_ACCESSOR:
		return containerAccessColon(v, dvar, arg1, darg2, true);

	default:
		reportInterp("illegal array argument", arg3);
		return true;
	};

	return false;
}

bool cInterpret::rvalueAccess(cContainer& v, double dvar, cContainer& arg1, cContainer& arg2, tokData& arg3)
{
	cContainer t;

	switch (arg3.nSymbolType)
	{
	case LEX_OPERATOR_ACCESS_END:
		arg3.fSymbolVal = 0.;
		// fall through..

	case LEX_TYPE_CONSTANT:
		return containerAccess(v, dvar, arg1, arg2, arg3.fSymbolVal);

	case LEX_TYPE_RVALUE:
		return containerAccess(v, dvar, arg1, arg2, m_rvalue);

	case LEX_TYPE_UNKNOWN:
		// fall through...

	case LEX_TYPE_VARIABLE:
		if (m_pSymbolTable->lookup(t, arg3.name)) { reportInterp("undefined symbol", arg3); return true; }
		return containerAccess(v, dvar, arg1, arg2, t);

	case LEX_TYPE_TEMP_VARIABLE:
		return containerAccess(v, dvar, arg1, arg2, m_tempVars[arg3.ix]);

	case LEX_TYPE_COLON_ACCESSOR:
		return containerAccessColon(v, dvar, arg1, arg2, true);

	default:
		reportInterp("illegal array argument", arg3);
		return true;
	};

	return false;
}

bool cInterpret::rvalueAccess(cContainer& v, string& name, double darg1, double darg2, tokData& arg3)
{
	int nType, nrows, ncols, ndeep;

	switch (arg3.nSymbolType)
	{
	case LEX_OPERATOR_ACCESS_END:
		if (m_pSymbolTable->lookupInfo(name, nType, nrows, ncols, ndeep)) {
			reportInterp("undefined symbol", arg3.nLineNo, name);
			return true;
		}
		arg3.fSymbolVal = ndeep - 1;
		// fall through..

	case LEX_TYPE_CONSTANT:
		v.SetType(VAR_TYPE_NUMERIC);
		return m_pSymbolTable->cubeAccess(v.m, name, darg1, darg2, arg3.fSymbolVal);

	case LEX_TYPE_RVALUE:
		return m_pSymbolTable->containerAccess(v, name, darg1, darg2, m_rvalue);

	case LEX_TYPE_UNKNOWN:
		// fall through...

	case LEX_TYPE_VARIABLE:
		return m_pSymbolTable->containerAccess(v, name, darg1, darg2, arg3.name);

	case LEX_TYPE_TEMP_VARIABLE:
		return m_pSymbolTable->containerAccess(v, name, darg1, darg2, m_tempVars[arg3.ix]);

	case LEX_TYPE_COLON_ACCESSOR:
		return m_pSymbolTable->containerAccessColon(v, name, darg1, darg2, true);

	default:
		reportInterp("illegal array argument", arg3);
		return true;
	};

	return false;
}

bool cInterpret::rvalueAccess(cContainer& v, string& name, double darg1, cContainer& arg2, tokData& arg3)
{
	int nType, nrows, ncols, ndeep;

	switch (arg3.nSymbolType)
	{
	case LEX_OPERATOR_ACCESS_END:
		if (m_pSymbolTable->lookupInfo(name, nType, nrows, ncols, ndeep)) {
			reportInterp("undefined symbol", arg3.nLineNo, name);
			return true;
		}
		arg3.fSymbolVal = ndeep - 1;
		// fall through..

	case LEX_TYPE_CONSTANT:
		return m_pSymbolTable->containerAccess(v, name, darg1, arg2, arg3.fSymbolVal);

	case LEX_TYPE_RVALUE:
		return m_pSymbolTable->containerAccess(v, name, darg1, arg2, m_rvalue);

	case LEX_TYPE_UNKNOWN:
		// fall through...

	case LEX_TYPE_VARIABLE:
		return m_pSymbolTable->containerAccess(v, name, darg1, arg2, arg3.name);

	case LEX_TYPE_TEMP_VARIABLE:
		return m_pSymbolTable->containerAccess(v, name, darg1, arg2, m_tempVars[arg3.ix]);

	case LEX_TYPE_COLON_ACCESSOR:
		return m_pSymbolTable->containerAccessColon(v, name, darg1, arg2, true);

	default:
		reportInterp("illegal array argument", arg3);
		return true;
	};

	return false;
}

bool cInterpret::rvalueAccess(cContainer& v, string& name, cContainer& arg1, double darg2, tokData& arg3)
{
	int nType, nrows, ncols, ndeep;

	switch (arg3.nSymbolType)
	{
	case LEX_OPERATOR_ACCESS_END:
		if (m_pSymbolTable->lookupInfo(name, nType, nrows, ncols, ndeep)) {
			reportInterp("undefined symbol", arg3.nLineNo, name);
			return true;
		}
		arg3.fSymbolVal = ndeep - 1;
		// fall through..

	case LEX_TYPE_CONSTANT:
		return m_pSymbolTable->containerAccess(v, name, arg1, darg2, arg3.fSymbolVal);

	case LEX_TYPE_RVALUE:
		return m_pSymbolTable->containerAccess(v, name, arg1, darg2, m_rvalue);

	case LEX_TYPE_UNKNOWN:
		// fall through...

	case LEX_TYPE_VARIABLE:
		return m_pSymbolTable->containerAccess(v, name, arg1, darg2, arg3.name);

	case LEX_TYPE_TEMP_VARIABLE:
		return m_pSymbolTable->containerAccess(v, name, arg1, darg2, m_tempVars[arg3.ix]);

	case LEX_TYPE_COLON_ACCESSOR:
		return m_pSymbolTable->containerAccessColon(v, name, arg1, darg2, true);

	default:
		reportInterp("illegal array argument", arg3);
		return true;
	};

	return false;
}

bool cInterpret::rvalueAccess(cContainer& v, string& name, cContainer& arg1, cContainer& arg2, tokData& arg3)
{
	int nType, nrows, ncols, ndeep;

	switch (arg3.nSymbolType)
	{
	case LEX_OPERATOR_ACCESS_END:
		if (m_pSymbolTable->lookupInfo(name, nType, nrows, ncols, ndeep)) {
			reportInterp("undefined symbol", arg3.nLineNo, name);
			return true;
		}
		arg3.fSymbolVal = ndeep - 1;
		// fall through..

	case LEX_TYPE_CONSTANT:
		return m_pSymbolTable->containerAccess(v, name, arg1, arg2, arg3.fSymbolVal);

	case LEX_TYPE_RVALUE:
		return m_pSymbolTable->containerAccess(v, name, arg1, arg2, m_rvalue);

	case LEX_TYPE_UNKNOWN:
		// fall through...

	case LEX_TYPE_VARIABLE:
		return m_pSymbolTable->containerAccess(v, name, arg1, arg2, arg3.name);

	case LEX_TYPE_TEMP_VARIABLE:
		return m_pSymbolTable->containerAccess(v, name, arg1, arg2, m_tempVars[arg3.ix]);

	case LEX_TYPE_COLON_ACCESSOR:
		return m_pSymbolTable->containerAccessColon(v, name, arg1, arg2, true);

	default:
		reportInterp("illegal array argument", arg3);
		return true;
	};

	return false;
}


// with colon operator
bool cInterpret::rvalueAccessColon(cContainer& v, double dvar, bool b1, tokData& arg2, tokData& arg3)
{
	cContainer t;

	switch (arg2.nSymbolType)
	{
	case LEX_OPERATOR_ACCESS_END:
		arg2.fSymbolVal = 0.;
		// fall through..

	case LEX_TYPE_CONSTANT:
		return rvalueAccessColon(v, dvar, b1, arg2.fSymbolVal, arg3);

	case LEX_TYPE_RVALUE:
		return rvalueAccessColon(v, dvar, b1, m_rvalue, arg3);

	case LEX_TYPE_UNKNOWN:
		// fall through...

	case LEX_TYPE_VARIABLE:
		if (m_pSymbolTable->lookup(t, arg3.name)) { reportInterp("undefined symbol", arg3); return true; }
		return rvalueAccessColon(v, dvar, b1, t, arg3);

	case LEX_TYPE_TEMP_VARIABLE:
		return rvalueAccessColon(v, dvar, b1, m_tempVars[arg2.ix], arg3);

	case LEX_TYPE_COLON_ACCESSOR:
		return rvalueAccessColon(v, dvar, b1, true, arg3);

	default:
		reportInterp("illegal array argument", arg2);
		return true;
	};

	return false;
}

bool cInterpret::rvalueAccessColon(cContainer& v, cContainer& var, bool b1, tokData& arg2, tokData& arg3)
{
	cContainer t;

	switch (arg2.nSymbolType)
	{
	case LEX_OPERATOR_ACCESS_END:
		arg2.fSymbolVal = var.GetColumns() - 1;
		// fall through..

	case LEX_TYPE_CONSTANT:
		return rvalueAccessColon(v, var, b1, arg2.fSymbolVal, arg3);

	case LEX_TYPE_RVALUE:
		return rvalueAccessColon(v, var, b1, m_rvalue, arg3);

	case LEX_TYPE_UNKNOWN:
		// fall through...

	case LEX_TYPE_VARIABLE:
		if (m_pSymbolTable->lookup(t, arg3.name)) { reportInterp("undefined symbol", arg3); return true; }
		return rvalueAccessColon(v, var, b1, t, arg3);

	case LEX_TYPE_TEMP_VARIABLE:
		return rvalueAccessColon(v, var, b1, m_tempVars[arg2.ix], arg3);

	case LEX_TYPE_COLON_ACCESSOR:
		return rvalueAccessColon(v, var, b1, true, arg3);

	default:
		reportInterp("illegal array argument", arg2);
		return true;
	};

	return false;
}

bool cInterpret::rvalueAccessColon(cContainer& v, double dvar, bool b1, double darg2, tokData& arg3)
{
	cContainer t;

	switch (arg3.nSymbolType)
	{
	case LEX_OPERATOR_ACCESS_END:
		arg3.fSymbolVal = 0.;
		// fall through..

	case LEX_TYPE_CONSTANT:
		return containerAccessColon(v, dvar, b1, darg2, arg3.fSymbolVal);

	case LEX_TYPE_RVALUE:
		return containerAccessColon(v, dvar, b1, darg2, m_rvalue);

	case LEX_TYPE_UNKNOWN:
		// fall through...

	case LEX_TYPE_VARIABLE:
		if (m_pSymbolTable->lookup(t, arg3.name)) { reportInterp("undefined symbol", arg3); return true; }
		return containerAccessColon(v, dvar, b1, darg2, t);

	case LEX_TYPE_TEMP_VARIABLE:
		return containerAccessColon(v, dvar, b1, darg2, m_tempVars[arg3.ix]);

	case LEX_TYPE_COLON_ACCESSOR:
		return containerAccessColon(v, dvar, b1, darg2, true);

	default:
		reportInterp("illegal array argument", arg3);
		return true;
	};

	return false;
}

bool cInterpret::rvalueAccessColon(cContainer& v, double dvar, bool b1, cContainer& arg2, tokData& arg3)
{
	cContainer t;

	switch (arg3.nSymbolType)
	{
	case LEX_OPERATOR_ACCESS_END:
		arg3.fSymbolVal = 0.;
		// fall through..

	case LEX_TYPE_CONSTANT:
		return containerAccessColon(v, dvar, b1, arg2, arg3.fSymbolVal);

	case LEX_TYPE_RVALUE:
		return containerAccessColon(v, dvar, b1, arg2, m_rvalue);

	case LEX_TYPE_UNKNOWN:
		// fall through...

	case LEX_TYPE_VARIABLE:
		if (m_pSymbolTable->lookup(t, arg3.name)) { reportInterp("undefined symbol", arg3); return true; }
		return containerAccessColon(v, dvar, b1, arg2, t);

	case LEX_TYPE_TEMP_VARIABLE:
		return containerAccessColon(v, dvar, b1, arg2, m_tempVars[arg3.ix]);

	case LEX_TYPE_COLON_ACCESSOR:
		return containerAccessColon(v, dvar, b1, arg2, true);

	default:
		reportInterp("illegal array argument", arg3);
		return true;
	};

	return false;
}

bool cInterpret::rvalueAccessColon(cContainer& v, double dvar, bool b1, bool b2, tokData& arg3)
{
	cContainer t;

	switch (arg3.nSymbolType)
	{
	case LEX_OPERATOR_ACCESS_END:
		arg3.fSymbolVal = 0.;
		// fall through..

	case LEX_TYPE_CONSTANT:
		return containerAccessColon(v, dvar, b1, b2, arg3.fSymbolVal);

	case LEX_TYPE_RVALUE:
		return containerAccessColon(v, dvar, b1, b2, m_rvalue);

	case LEX_TYPE_UNKNOWN:
		// fall through...

	case LEX_TYPE_VARIABLE:
		if (m_pSymbolTable->lookup(t, arg3.name)) { reportInterp("undefined symbol", arg3); return true; }
		return containerAccessColon(v, dvar, b1, b2, t);

	case LEX_TYPE_TEMP_VARIABLE:
		return containerAccessColon(v, dvar, b1, b2, m_tempVars[arg3.ix]);

	case LEX_TYPE_COLON_ACCESSOR:
		return containerAccessColon(v, dvar, b1, b2, true);

	default:
		reportInterp("illegal array argument", arg3);
		return true;
	};

	return false;
}

bool cInterpret::rvalueAccessColon(cContainer& v, cContainer& var, bool b1, double darg2, tokData& arg3)
{
	cContainer t;
	string name = var.GetName();

	switch (arg3.nSymbolType)
	{
	case LEX_OPERATOR_ACCESS_END:
		arg3.fSymbolVal = var.GetDepth() - 1;
		// fall through..

	case LEX_TYPE_CONSTANT:
		return m_pSymbolTable->containerAccessColon(v, name, b1, darg2, arg3.fSymbolVal);

	case LEX_TYPE_UNKNOWN:
		// fall through...

	case LEX_TYPE_VARIABLE:
		return m_pSymbolTable->containerAccessColon(v, name, b1, darg2, arg3.name);

	case LEX_TYPE_COLON_ACCESSOR:
		return m_pSymbolTable->containerAccessColon(v, name, b1, darg2, true);

	default:
		reportInterp("illegal array argument", arg3);
		return true;
	};

	return false;
}

bool cInterpret::rvalueAccessColon(cContainer& v, cContainer& var, bool b1, cContainer& arg2, tokData& arg3)
{
	cContainer t;
	string name = var.GetName();

	switch (arg3.nSymbolType)
	{
	case LEX_OPERATOR_ACCESS_END:
		arg3.fSymbolVal = var.GetDepth() - 1;
		// fall through..

	case LEX_TYPE_CONSTANT:
		return m_pSymbolTable->containerAccessColon(v, name, b1, arg2, arg3.fSymbolVal);

	case LEX_TYPE_UNKNOWN:
		// fall through...

	case LEX_TYPE_VARIABLE:
		return m_pSymbolTable->containerAccessColon(v, name, b1, arg2, arg3.name);

	case LEX_TYPE_COLON_ACCESSOR:
		return m_pSymbolTable->containerAccessColon(v, name, b1, arg2, true);

	default:
		reportInterp("illegal array argument", arg3);
		return true;
	};

	return false;
}

bool cInterpret::rvalueAccessColon(cContainer& v, double dvar, cContainer& arg1, bool b2, tokData& arg3)
{
	cContainer t;

	switch (arg3.nSymbolType)
	{
	case LEX_OPERATOR_ACCESS_END:
		arg3.fSymbolVal = 0.;
		// fall through..

	case LEX_TYPE_CONSTANT:
		return containerAccessColon(v, dvar, arg1, b2, arg3.fSymbolVal);

	case LEX_TYPE_RVALUE:
		return containerAccessColon(v, dvar, arg1, b2, m_rvalue);

	case LEX_TYPE_UNKNOWN:
		// fall through...

	case LEX_TYPE_VARIABLE:
		if (m_pSymbolTable->lookup(t, arg3.name)) { reportInterp("undefined symbol", arg3); return true; }
		return containerAccessColon(v, dvar, arg1, b2, t);

	case LEX_TYPE_TEMP_VARIABLE:
		return containerAccessColon(v, dvar, arg1, b2, m_tempVars[arg3.ix]);

	case LEX_TYPE_COLON_ACCESSOR:
		return containerAccessColon(v, dvar, arg1, b2, true);

	default:
		reportInterp("illegal array argument", arg3);
		return true;
	};

	return false;
}

bool cInterpret::rvalueAccessColon(cContainer& v, double dvar, double darg1, bool b2, tokData& arg3)
{
	cContainer t;

	switch (arg3.nSymbolType)
	{
	case LEX_OPERATOR_ACCESS_END:
		arg3.fSymbolVal = 0.;
		// fall through..

	case LEX_TYPE_CONSTANT:
		return containerAccessColon(v, dvar, darg1, b2, arg3.fSymbolVal);

	case LEX_TYPE_RVALUE:
		return containerAccessColon(v, dvar, darg1, b2, m_rvalue);

	case LEX_TYPE_UNKNOWN:
		// fall through...

	case LEX_TYPE_VARIABLE:
		if (m_pSymbolTable->lookup(t, arg3.name)) { reportInterp("undefined symbol", arg3); return true; }
		return containerAccessColon(v, dvar, darg1, b2, t);

	case LEX_TYPE_TEMP_VARIABLE:
		return containerAccessColon(v, dvar, darg1, b2, m_tempVars[arg3.ix]);

	case LEX_TYPE_COLON_ACCESSOR:
		return containerAccessColon(v, dvar, darg1, b2, true);

	default:
		reportInterp("illegal array argument", arg3);
		return true;
	};

	return false;
}

bool cInterpret::rvalueAccessColon(cContainer& v, cContainer& var, cContainer& arg1, bool b2, tokData& arg3)
{
	cContainer t;
	string name = var.GetName();

	switch (arg3.nSymbolType)
	{
	case LEX_OPERATOR_ACCESS_END:
		arg3.fSymbolVal = var.GetDepth() - 1;
		// fall through..

	case LEX_TYPE_CONSTANT:
		return m_pSymbolTable->containerAccessColon(v, name, arg1, b2, arg3.fSymbolVal);

	case LEX_TYPE_UNKNOWN:
		// fall through...

	case LEX_TYPE_VARIABLE:
		return m_pSymbolTable->containerAccessColon(v, name, arg1, b2, t);

	case LEX_TYPE_COLON_ACCESSOR:
		return m_pSymbolTable->containerAccessColon(v, name, arg1, b2, true);

	default:
		reportInterp("illegal array argument", arg3);
		return true;
	};

	return false;
}

bool cInterpret::rvalueAccessColon(cContainer& v, cContainer& var, double darg1, bool b2, tokData& arg3)
{
	cContainer t;
	string name = var.GetName();

	switch (arg3.nSymbolType)
	{
	case LEX_OPERATOR_ACCESS_END:
		arg3.fSymbolVal = var.GetDepth() - 1;
		// fall through..

	case LEX_TYPE_CONSTANT:
		return m_pSymbolTable->containerAccessColon(v, name, darg1, b2, arg3.fSymbolVal);

	case LEX_TYPE_UNKNOWN:
		// fall through...

	case LEX_TYPE_VARIABLE:
		return m_pSymbolTable->containerAccessColon(v, name, darg1, b2, arg3.name);

	case LEX_TYPE_COLON_ACCESSOR:
		return m_pSymbolTable->containerAccessColon(v, name, darg1, b2, true);

	default:
		reportInterp("illegal array argument", arg3);
		return true;
	};

	return false;
}


bool cInterpret::rvalueAccessColon(cContainer& v, cContainer& var, bool b1, bool b2, tokData& arg3)
{
	cContainer t;
	string name = var.GetName();

	switch (arg3.nSymbolType)
	{
	case LEX_OPERATOR_ACCESS_END:
		arg3.fSymbolVal = var.GetDepth() - 1;
		// fall through..

	case LEX_TYPE_CONSTANT:
		return m_pSymbolTable->containerAccessColon(v, name, b1, b2, arg3.fSymbolVal);

	case LEX_TYPE_UNKNOWN:
		// fall through...

	case LEX_TYPE_VARIABLE:
		return m_pSymbolTable->containerAccessColon(v, name, b1, b2, arg3.name);

	case LEX_TYPE_COLON_ACCESSOR:
		return m_pSymbolTable->containerAccessColon(v, name, b1, b2, true);

	default:
		reportInterp("illegal array argument", arg3);
		return true;
	};

	return false;
}
