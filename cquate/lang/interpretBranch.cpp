
#include "interpret.h"
#include "state.h"

// program state
extern cState prog_state;
extern cStorage T;

extern void setErrorState(string& file, int iline);
extern void setErrorState(const char *pchFile, int iline);
extern void setErrorState(errorReport& er);
extern void resetErrorState();
extern errorReport getErrorState();


bool cInterpret::interpretBlock( sBlock& block, bool& bDebugJumpOut, bool bDebugJumpIn )
{
	if (block.bError) { m_outMsg = block.outMsg; return true; }
	int iJumpState = 0;

	bool bErr = interpretStmts( block.stmts, iJumpState, bDebugJumpOut, bDebugJumpIn );

	return bErr;
}

bool cInterpret::interpretFcnBlock( cContainer& v_ret, sBlock& block, vector<tokData>& inToks, int nLineNo, bool& bDebugJumpOut, bool bDebugJumpIn )
{
	vector<valueBlob> emptyBlobs;
	return interpretFcnBlock(v_ret, block, inToks, emptyBlobs, nLineNo, bDebugJumpOut, bDebugJumpIn );
}

bool cInterpret::interpretFcnBlock( cContainer& v_ret, sBlock& block, vector<tokData>& inToks, vector<valueBlob>& Lblobs, int nLineNo, bool& bDebugJumpOut, bool bDebugJumpIn )
{
	bool bErr = false;
	int ix(0);

	// first, check for parse error
	if (block.bError) {
		m_outMsg = block.outMsg;
		return true;
	}

	cContainer v;
	vector<cContainer> vv;
	string str;

	int iJumpState = 0;

	int i;
	// populate new var space with argument symbols
	//
	// 1. get in args from home var space
	for (i=0; i<(int)block.inArgs.size(); ++i) {
		if (tok2container(v, inToks[i])) { reportInterp("illegal or undefined function argument", inToks[i]); return true; }
		v.SetName( block.inArgs[i].name );
		vv.push_back( v );
	}

	// 2. instantiate new empty variable space
	cVariableSpace vs;
	if (T.Push(vs)) {
		reportInterp("unable to push a new variable space on stack - out of memory");
		return true;
	}
	m_pSymbolTable = T.getHashPtr();
	prog_state.m_lang.setSymbolTable( m_pSymbolTable );

	// 3. transfer in args to new space
	for (i=0; i<(int)block.inArgs.size(); ++i) {
		if (m_pSymbolTable!=NULL) m_pSymbolTable->insert( vv[i] );
	}

	// save state
	string saveFcn = m_curFcn;
	string saveFile = m_curFile;
	string savePath = m_curPath;

	// change state
	m_curFcn = block.name;
	m_curFile = block.fileName;
	m_curPath = block.pathName;
	setErrorFile( m_curFile );			// error msgs
	
	// file stack
	string fullFile = block.pathName + "\\";
	fullFile.append( block.fileName );
	sStackEntry se;
	se.name = block.name;
	se.bReload = false;
	se.fullFile = fullFile;
	se.nLine = m_stack.empty() ? -1 : nLineNo;
	se.itype = LEX_TYPE_SCRIPT_FCN_CALL;
	se.ipos = T.GetSize() - 1;
	se.nargin = block.inArgs.size();
	se.nargout = block.outArgs.size();
	m_stack.push_back( se );

	bool bJumptOutPrior = bDebugJumpOut;

	// execute the fcn
	if (interpretStmts( block.stmts, iJumpState, bDebugJumpOut, bDebugJumpIn )) {
		// error! pop the var space stack
		T.Pop();
		m_pSymbolTable = T.getHashPtr();
		// propagate error
		block.outMsg = m_outMsg;
		// restore and return
		bErr = true;
		goto Restore;
	}

	if (iJumpState & _JUMP_STEP_0) {
		bDebugJumpOut = true;
	}

	// check and set jump out flag
	if (!bJumptOutPrior && bDebugJumpOut) {
		m_bTriggerStop = true;
		bDebugJumpOut = false;
	}

	// out of the fcn, so reset m_bReturn
	m_bReturn = false;

	// store results in out args
	//
	// 1. get the output vars
	vv.clear();
	int n;
	if ( Lblobs.size()==0 ) {	// no out args specified, store result in "res" (may be empty)
		if (!block.outArgs.empty()) {
			if (m_pSymbolTable!=NULL)
				if (m_pSymbolTable->lookup(v, block.outArgs[0].name)) {
					reportInterp("return symbol not defined: ", block.outArgs[0].name, block.nLineOffset);
					bErr = true;
					goto Restore;
				}
		}
		else { v.IsEmpty(); }
		v.SetName("res");
		vv.push_back( v );
	}
	else {	// put all out args onto vv, change name to name in corresponding Lblobs
		n = Lblobs.size();
		if (n > (int)block.outArgs.size()) {
			for (i=(int)block.outArgs.size(); i<n; ++i) { Lblobs[i].vResult.Delete(); }
		}
		for (i=0; i<(int)block.outArgs.size(); ++i) {
			if (i < (int)Lblobs.size()) {
				if (m_pSymbolTable->lookup(v, block.outArgs[i].name)) {
					reportInterp("symbol not defined: ", block.outArgs[i].name, block.nLineOffset);
					bErr = true;
					goto Restore;
				}
				v.SetName( Lblobs[i].token.name );
				vv.push_back( v );
			}
			else
				break;
		}
	}

	// 2. pop the var space stack
	T.Pop();
	m_pSymbolTable = T.getHashPtr();
	prog_state.m_lang.setSymbolTable( T.getHashPtr() );

	// 3. transfer out vars to original space
	for (i=0; i<(int)Lblobs.size(); ++i, ++ix) {
		if ( Lblobs[i].token.name.empty() ) {
			--ix;
			continue;
		}

		vv[ix].SetName( Lblobs[i].token.name );		// set all lvalue names according to token name
		if (!Lblobs[i].hashArgs.empty()) {
			m_pSymbolTable->hashSet( Lblobs[i].token.name, Lblobs[i].hashArgs, Lblobs[i].vResult );
		}
		else if (Lblobs[i].vArg.IsEmpty())
			m_pSymbolTable->insert( vv[ix] );
		else {	// indexed assignment
			if ( m_pSymbolTable->indexSet(vv[ix], Lblobs[i].vArg) ) { reportInterp("illegal assignment (size mismatch?) ", Lblobs[i].token); return true; }
		}
	}

	// return value;
	if (vv.size() > 0) v_ret = vv[0];
	else v_ret.IsEmpty();

	// restore settings
Restore:
	m_curFcn = saveFcn;
	m_curFile = saveFile;
	m_curPath = savePath;
	setErrorFile( m_curFile );

	m_stack.pop_back();

	return bErr;
}

// future hack to support function eval - TBD
bool cInterpret::interpretFcnBlock( cContainer& v_ret, sBlock& block, cContainer& varg1, cContainer& varg2 )
{
	return false;
}

bool cInterpret::interpretIf(vector<sStatement>& stmts, int& ix, int& iJumpState, bool& bJumpOut, bool bDebugJumpIn)
{
	bool bCond = false;
	vector<tokData> tokens;

	while (!bCond) {

		if (stmts[ix].nType==LEX_STATEMENT_ELSE_IF) { ++ix; }					// skip past dummy ELSE_IF stmt
		else if (stmts[ix].nType==LEX_STATEMENT_ELSE) { bCond = true; break; }	// always do else if bCond is false

		// eval condition
		tokens = stmts[ix].tokens;
		if ( evalScalarCondition(tokens, bCond, bJumpOut, bDebugJumpIn) ) return true;
		if (!bCond) {
			ix += (stmts[ix].nSkipFalse + 1);
			if (ix >= (int)stmts.size()) return false;								// nothing past the if block(s)
			if (stmts[ix].nType != LEX_STATEMENT_ELSE_IF && stmts[ix].nType != LEX_STATEMENT_ELSE) { --ix; break; }	// no if branches will be taken!
		}
	}

	vector<sStatement> ifStmts;

	if (bCond) {

		// ix points to if or else statment, so (ix+1) is first stmt in block

		// get stmt block
		ifStmts.insert( ifStmts.begin(), stmts.begin()+ix+1, stmts.begin()+ix+1+stmts[ix].nSkipFalse );

		// interpret stmts
		if (interpretStmts( ifStmts, iJumpState, bJumpOut, bDebugJumpIn )) return true;

		// reload if we hit a debug stop
		if (!m_stack.empty()) {
			if (m_stack.back().bReload)
				reloadCurrent( ifStmts );
		}

		if (iJumpState & _JUMP_BREAK || iJumpState & _JUMP_CONTINUE || iJumpState & _JUMP_RETURN) {	// jump out (break, continue, return)
			return false;
		}

		if (m_bReturn) return false;

		// skip past all remaining branches
		ix += (stmts[ix].nSkipFalse + 1);
		while (true) {
			if (ix >= (int)stmts.size())
				break;

			switch (stmts[ix].nType) {
			case LEX_STATEMENT_ELSE_IF:
				++ix;
				// fall through...
			case LEX_STATEMENT_ELSE:
				ix += (stmts[ix].nSkipFalse + 1);
				break;

			default:
				bCond = false;
				break;
			};
			if (!bCond)
				break;
		}

		--ix;	// now go back one step, b/c parent fcn will advance to here
	}

	return false;
}

bool cInterpret::interpretWhile(vector<sStatement>& stmts, int& ix, int& iJumpState, bool& bJumpOut, bool bDebugJumpIn)
{
	// dummy lvalue
	bool bCond = false;

	// eval condition
	vector<tokData> condTokens = stmts[ix].tokens;
	if ( evalScalarCondition(condTokens, bCond, bJumpOut, bDebugJumpIn) ) return true;
	if (!bCond) return false;

	// create while block of stmts
	vector<sStatement> whileBlock;
	whileBlock.insert( whileBlock.begin(), stmts.begin()+ix+1, stmts.begin()+ix+stmts[ix].nSkipFalse+1 );

	while (bCond) {

		// eval block
		if (interpretStmts(whileBlock, iJumpState, bJumpOut, bDebugJumpIn)) return true;

		if (iJumpState & _JUMP_BREAK | iJumpState & _JUMP_RETURN) {
			// ignore _JUMP_CONTINUE, as it will simply continue on to eval condition...
			return false;
		}

		// reload if we hit a debug stop
		if (!m_stack.empty()) {
			if (m_stack.back().bReload)
				reloadCurrent( whileBlock );
		}

		if (m_bReturn) return false;

		// eval condition
		condTokens = stmts[ix].tokens;
		if ( evalScalarCondition(condTokens, bCond, bJumpOut, bDebugJumpIn) ) return true;
	}

	ix += stmts[ix].nSkipFalse;

	return false;
}

bool cInterpret::interpretDo(vector<sStatement>& stmts, int& ix, int& iJumpState, bool& bJumpOut, bool bDebugJumpIn)
{
	// dummy lvalue
	bool bCond = true;
	vector<tokData> tokens;

	// create while block of stmts
	vector<sStatement> doBlock;
	doBlock.insert( doBlock.begin(), stmts.begin()+ix+1, stmts.begin()+ix+stmts[ix].nSkipFalse+1 );
	int ixWhile = ix + stmts[ix].nSkipFalse + 1;

	while (bCond) {

		// eval block
		if (interpretStmts(doBlock, iJumpState, bJumpOut, bDebugJumpIn)) return true;

		if (iJumpState & _JUMP_BREAK | iJumpState & _JUMP_RETURN) {
			// ignore _JUMP_CONTINUE, as it will simply continue on to eval condition...
			return false;
		}

		// reload if we hit a debug stop
		if (!m_stack.empty()) {
			if (m_stack.back().bReload)
				reloadCurrent( doBlock );
		}

		if (m_bReturn) return false;

		// eval condition
		tokens = stmts[ixWhile].tokens;
		if ( evalScalarCondition(tokens, bCond, bJumpOut, bDebugJumpIn) ) return true;
	}

	ix += stmts[ix].nSkipFalse + 1;

	return false;
}

bool cInterpret::interpretFor(vector<sStatement>& stmts, int& ix, int& iJumpState, bool& bJumpOut, bool bDebugJumpIn)
{
	// eval initial condition
	if (interpret(stmts[ix], iJumpState, bJumpOut, bDebugJumpIn)) return true;

	if (iJumpState) {
		return false;
	}

	// create a for-block of stmts
	vector<sStatement> forBlock;
	forBlock.insert( forBlock.begin(), stmts.begin()+ix+3, stmts.begin()+ix+stmts[ix].nSkipFalse + 1 );

	bool bCond = true;
	vector<tokData> tokens = stmts[ix+1].tokens;
	if (evalScalarCondition(tokens, bCond, bJumpOut, bDebugJumpIn)) return true;

	while ( bCond ) {

		// eval block
		if (interpretStmts(forBlock, iJumpState, bJumpOut, bDebugJumpIn)) return true;

		if (iJumpState & _JUMP_BREAK | iJumpState & _JUMP_RETURN) {
			// ignore _JUMP_CONTINUE, as it will simply continue on to eval condition...
			iJumpState &= _JUMP_BREAK_RESET;
			return false;
		}

		iJumpState &= _JUMP_BREAK_CONTINUE;

		// reload if we hit a debug stop
		if (!m_stack.empty()) {
			if (m_stack.back().bReload)
				reloadCurrent( forBlock );
		}

		if (m_bReturn) return false;

		// eval increment
		if (interpret(stmts[ix+2], iJumpState, bJumpOut, bDebugJumpIn)) return true;

		if (iJumpState & _JUMP_BREAK | iJumpState & _JUMP_RETURN) {
			// ignore _JUMP_CONTINUE, as it will simply continue on to eval condition...
			iJumpState &= _JUMP_BREAK_RESET;
			return false;
		}

		// eval stop condition
		tokens = stmts[ix+1].tokens;
		if (evalScalarCondition(tokens, bCond, bJumpOut, bDebugJumpIn)) return true;
	}

	ix += stmts[ix].nSkipFalse;

	return false;
}

bool cInterpret::interpretSwitch(vector<sStatement>& stmts, int& ix, int& iJumpState, bool& bJumpOut, bool bDebugJumpIn)
{
	cContainer varg, vcase, v;
	bool bEqual = false;
	bool bDone = false;
	vector<sStatement> Stmts;

	// eval switch argument
	if ( evalScalar(stmts[ix].tokens, varg, bJumpOut, bDebugJumpIn) ) return true;

	int iStart = ix;
	int nEnd = stmts[ix].nSkipFalse;
	++ix;

	// eval 1st case
	if ( evalScalar(stmts[ix].tokens, vcase, bJumpOut, bDebugJumpIn) ) return true;

	bEqual = varg == vcase;

	while (!bEqual && stmts[ix].nType==LEX_STATEMENT_CASE) {

		ix += stmts[ix].nSkipFalse + 1;

		if (stmts[ix].nType==LEX_STATEMENT_DEFAULT)
			break;

		// eval next case
		if ( evalScalar(stmts[ix].tokens, vcase, bJumpOut, bDebugJumpIn) ) return true;

		bEqual = varg == vcase;
	}

	if (bEqual && stmts[ix].nType==LEX_STATEMENT_CASE)
	{
		// get this case, and all subsequent cases (and default)

		while (stmts[ix].nType==LEX_STATEMENT_CASE || stmts[ix].nType==LEX_STATEMENT_DEFAULT)
		{
			// get stmt block
			Stmts.clear();
			Stmts.insert( Stmts.begin(), stmts.begin()+ix+1, stmts.begin()+ix+stmts[ix].nSkipFalse+1 );

			if ( interpretStmts( Stmts, iJumpState, bJumpOut, bDebugJumpIn )) return true;

			if (iJumpState & _JUMP_BREAK) {
				// ix = ?
				iJumpState &= _JUMP_BREAK_RESET;
				bDone = true;		// do not jump into any more cases, or default
				break;
			}

			// jump to next switch case
			ix += stmts[ix].nSkipFalse + 1;
		}
	}

	if (!bDone && !bEqual && stmts[ix].nType==LEX_STATEMENT_DEFAULT) {

		// get stmt block
		Stmts.insert( Stmts.begin(), stmts.begin()+ix+1, stmts.begin()+ix+stmts[ix].nSkipFalse+1 );

		// interpret stmts
		if (interpretStmts( Stmts, iJumpState, bJumpOut, bDebugJumpIn )) return true;

		// update ip
		ix = iStart + nEnd + 1;

		if (iJumpState) {
			return false;
		}

		// reload if we hit a debug stop
		if (!m_stack.empty()) {
			if (m_stack.back().bReload)
				reloadCurrent( Stmts );
		}

		if (m_bReturn) return false;
	}
	else if (bDone) {
		while (stmts[ix].nType==LEX_STATEMENT_CASE || stmts[ix].nType==LEX_STATEMENT_DEFAULT)
		{
			// jump to next switch case
			ix += stmts[ix].nSkipFalse + 1;
		}
	}

	// update ip
	ix = iStart + nEnd + 1;

	return false;
}

bool cInterpret::interpretBreak( vector<sStatement>& stmts, int& iJumpState, int& ix )
{
	int ixParent = ix - stmts[ix].nSkipTrue;	// break points to this stmt
	
	if (ixParent<0) { reportInterp("interpreter error"); return true; }

	iJumpState |= _JUMP_BREAK;

	return false;
}

bool cInterpret::interpretContinue( vector<sStatement>& stmts, int& iJumpState, int& ix )
{
	iJumpState |= _JUMP_CONTINUE;
	return false;
}

bool cInterpret::interpretReturn( vector<sStatement>& stmts, int& iJumpState, int& ix )
{
	iJumpState |= _JUMP_RETURN;
	m_bReturn = true;
	return false;
}
