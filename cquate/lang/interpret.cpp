
#include "interpret.h"
#include "container.h"
#include "state.h"

extern cState prog_state;
extern cStorage T;


extern void setErrorState(string& file, int iline);
extern void setErrorState(const char *pchFile, int iline);
extern void setErrorState(errorReport& er);
extern void resetErrorState();
extern errorReport getErrorState();



// report interpreter errors
void cInterpret::reportInterp( const char *pchMsg )
{
	string s = pchMsg;
	m_outMsg.push_back( s );
}

void cInterpret::reportInterp( string msg, string& name, int iLine )
{
	char pchLine[64];
	string prefix;

	if (!m_curFile.empty()) {
		prefix = m_curPath + '\\';
		prefix.append( m_curFile );
		if (!m_curFcn.empty()) {
			prefix.append(" ["); prefix.append(m_curFcn); prefix.append("] ");
		}
		sprintf(pchLine,"(line %d) : ", iLine);
		prefix.append( pchLine );
	}

	m_outMsg.push_back( prefix + msg );
}

void cInterpret::reportInterp( const char *pchMsg, tokData& tok )
{
	int line = tok.nLineNo;
	reportInterp( pchMsg, line );
}

void cInterpret::reportInterp( const char *pchMsg, int iLine )
{
	reportInterp( string(pchMsg), iLine );
}

void cInterpret::reportInterp( string msg, tokData& tok )
{
	reportInterp( msg, tok.nLineNo );
}

void cInterpret::reportInterp( string msg, int iLine )
{
	char pchLine[64];
	string prefix;

	if (!m_curFile.empty()) {
		prefix = m_curPath + '\\';
		prefix.append( m_curFile );
		if (!m_curFcn.empty()) {
			prefix.append(" ["); prefix.append(m_curFcn); prefix.append("] ");
		}
		sprintf(pchLine,"(line %d) : ", iLine);
		prefix.append( pchLine );
	}

	m_outMsg.push_back( prefix + msg );
}

void cInterpret::reportInterp( const char *pchMsg, tokData& tok, string symbol )
{
	int line = tok.nLineNo;
	reportInterp( pchMsg, line, symbol );
}

void cInterpret::reportInterp( const char *pchMsg, int iLine, string symbol )
{
	reportInterp( string(pchMsg), iLine, symbol );
}

void cInterpret::reportInterp( string msg, tokData& tok, string symbol )
{
	reportInterp( msg, tok.nLineNo, symbol );
}

void cInterpret::reportInterp( string msg, int iLine, string symbol )
{
	char pchLine[64];
	string prefix;

	if (!m_curFile.empty()) {
		prefix = m_curPath + '\\';
		prefix.append( m_curFile );
		if (!m_curFcn.empty()) {
			prefix.append(" ["); prefix.append(m_curFcn); prefix.append("] ");
		}
		sprintf(pchLine,"(line %d) : ", iLine);
		prefix.append( pchLine );
	}

	msg.append(" : ");
	msg.append( symbol );

	m_outMsg.push_back( prefix + msg );
}

void cInterpret::clearReport()			{ m_outMsg.clear(); }

//
// push, pop state
//
void cInterpret::pushInterpState()
{
	interpState is;

	is.kd = m_kd;
	is.stmt = m_stmt;
	is.lvalueName = m_lvalueName;
	is.block = m_blockStack.back();

	m_interpState.push_back( is );

	m_stmt.tokens.clear();
	m_stmt.preTokens.clear();
	m_stmt.postTokens.clear();
	m_blockStack.back().stmts.clear();
	m_blockStack.back().bError = false;
}

void cInterpret::popInterpState()
{
	m_kd =			m_interpState.back().kd;
	m_stmt =		m_interpState.back().stmt;
	m_lvalueName =	m_interpState.back().lvalueName;
	m_blockStack.back() =		m_interpState.back().block;

	m_interpState.pop_back();
}


bool cInterpret::checkForAbort()
{
	//DWORD qsResult;
	//MSG msg;
	//bool bAborted = false;

	//qsResult = GetQueueStatus(QS_KEY);
	//while (qsResult & QS_KEY) {

	//	//PeekMessage( &msg, AfxGetMainWnd()->m_hWnd, WM_KEYDOWN, WM_KEYDOWN, PM_REMOVE );
	//	PeekMessage( &msg, NULL, WM_KEYDOWN, WM_KEYDOWN, PM_REMOVE );
	//	if (msg.message==WM_KEYDOWN) {
	//		if (msg.wParam==27) {		// ESC key
	//			bAborted = true;
	//			reportInterp("Abort (ESC).");
	//			break;
	//		}
	//		else {
	//			AfxPumpMessage();
	//		}
	//	}
	//	else {
	//		//TranslateMessage(&msg);
	//		//DispatchMessage(&msg);
	//		qsResult = GetQueueStatus(QS_KEY);
	//	}

	//}

	return prog_state.checkForAbort();
}

//// token types
//enum { LEX_TYPE_CONSTANT=1000, LEX_TYPE_VARIABLE, LEX_TYPE_LVALUE, LEX_TYPE_RVALUE, LEX_TYPE_EQUALS,
//		LEX_TYPE_KEYWORD_FUNCTION, LEX_TYPE_KEYWORD_COMMAND, LEX_TYPE_KEYWORD_LANGUAGE, LEX_TYPE_OPERATOR };	// and so on...

// interpret m_block
//

void cInterpret::initialize(sBlock& block)
{
	m_blockStack.push_back( block );
}

sBlock cInterpret::uninitialize()
{
	sBlock block = m_blockStack.back();
	m_blockStack.pop_back();
	return block;
}


bool cInterpret::interpret()
{
	bool bJumpOut(false), bFalse(false);
	sStatement stmt;
	vector<sStatement> updatedStmts;
	int iJumpState = 0;;

	for (int i=0; i<(int)m_blockStack.back().stmts.size(); ++i)
	{
		stmt = m_blockStack.back().stmts[i];
		if ( interpret( stmt, iJumpState, bJumpOut, bFalse ) ) return true;
		if (iJumpState) {
			return false;
		}
		updatedStmts.push_back( stmt );
	}

	if (!updatedStmts.empty())
		m_stmt.bReport = updatedStmts.back().bReport;

	//if (!m_blockStack.empty()) {
	//	if (!m_blockStack.back().stmts.empty())
	//		m_stmt.bReport = m_blockStack.back().stmts.back().bReport;
	//}

	return false;
}

bool cInterpret::interpret(int i)
{
	int iJumpState;

	bool bJumpOut(false), bFalse(false);
	if ( interpret( m_blockStack.back().stmts[i], iJumpState, bJumpOut, bFalse ) ) return true;

	m_bTriggerStop = false;

	return false;
}

bool cInterpret::interpretStmt()
{
	bool bJumpOut(false), bFalse(false);
	int iJumpState;

	if ( interpret( m_stmt, iJumpState, bJumpOut, bFalse ) ) return true;

	m_bTriggerStop = false;

	return false;
}


void cInterpret::debugMode(sStatement& stmt)
{
	string fullFile;

	// send current file name and IP to editor
	if (!m_curFile.empty())
	{
		fullFile = m_curPath + "\\";
		fullFile.append(m_curFile);
		//prog_state.SendFileNameToEditor( fullFile );
		//prog_state.SetDebugIp( stmt.nLineNumer );

		//prog_state.SetEditState(fullFile, stmt.nLineNumer);
		m_stack.back().nLine = stmt.nLineNumer;
	}

}


void cInterpret::setReloadStack()
{	// indicate all scripts/fcns in stack require reloading, due to possible insertions or deletions of break points
	for (int i=0; i<(int)m_stack.size(); ++i)
		m_stack[i].bReload = true;
}

bool cInterpret::reloadCurrent( vector<sStatement>& stmts )
{
	sStackEntry se = m_stack.back();
	int ix, ixh;
	sBlock b;
	sBlock fb;

	switch (se.itype) {
	case LEX_TYPE_SCRIPT_CALL:
		if (m_pScriptTable->lookup(b, se.name)) { reportInterp("Bad stack element"); return true; }
		if (replaceStmts(stmts, b.stmts)) return true;
		break;

	case LEX_TYPE_SCRIPT_FCN_CALL:
		if (m_pFcnTable->lookup(ix, ixh, fb, se.name, se.nargin, se.nargout)) { reportInterp("Bad stack element"); return true; }
		if (replaceStmts(stmts, fb.stmts)) return true;
		break;

	default:
		reportInterp("Bad stack element type!");
		return true;
	};

	return false;
}

bool cInterpret::replaceStmts(vector<sStatement>& stmts, vector<sStatement>& stmtsBlock)
{
	if (stmts.empty())
		return false;

	int lineStart = stmts[0].nLineNumer;
	int iBlockStart(-1);
	int i;

	// find first stmt in block
	for (i=0; i<(int)stmtsBlock.size(); ++i) {
		if (stmtsBlock[i].nLineNumer==lineStart) {
			iBlockStart = i;
			break;
		}
	}

	// check
	if (iBlockStart == -1 || iBlockStart+stmts.size() > stmtsBlock.size()) {
		reportInterp("internal error: cannot match executing block to a known function/script!");
		return true;
	}

	// replace the statements (NOTE: this is really just changing the bStop status.. there should be a faster way to do this)
	//
	for (i=0; i<(int)stmts.size(); ++i) {
		// we loop through explicitly so we can check the line number for each one

		if (stmts[i].nLineNumer != stmtsBlock[iBlockStart+i].nLineNumer) {
			reportInterp("internal error: cannot match executing block to a known function/script!");
			return true;
		}

		stmts[i] = stmtsBlock[iBlockStart+i];
	}

	return false;
}

bool cInterpret::interpretStmts( vector<sStatement>& stmts, int& iJumpState, bool& bJumpOut, bool bJumpIn )
{
	// bJumpOut is an output, indicating if "jump out" command was issued from debug mode
	//bJumpOut = false;

	int iEndIf = 0;

	int iAction(DBG_ACTION_GO);
	int iStop = -1;
	bool bJumpOutPrior = bJumpOut;
	bool bStep = false;
	string fullFile;

	sBlock blk;
	blk.stmts = stmts;
	blk.bError = false;
	blk.fileName = m_curFile;
	blk.nLineOffset = 0;
	blk.t_lastParsed = 0;

	if (bJumpIn || ((iJumpState & _JUMP_STEP_0) != 0)) {
		iJumpState &= _JUMP_STEP_0_RESET;		// reset step 0 bit
		iStop = 0;
		bJumpIn = false;
	}

	for (int i=0; i<(int)stmts.size(); ++i)
	{
		if (checkForAbort()) {		// halt/interrupt requested from user?
			reportInterp("Abort (ESC).");
			return true;
		}

		bStep = false;

		// error state mgmt
		setErrorLine( stmts[i].nLineNumer );

		//bJumpOutPrior = bJumpOut;

		switch (stmts[i].nType) {

		case LEX_STATEMENT_EXPRESSION: case LEX_STATEMENT_ASSIGNMENT:
			if (interpret(stmts[i], iJumpState, bJumpOut, bJumpIn))
				return true;
			break;

		case LEX_STATEMENT_IF:
			if (bStep)
				bJumpIn = true;
			if (interpretIf(stmts, i, iJumpState, bJumpOut, bJumpIn)) return true;
			break;

		case LEX_STATEMENT_WHILE:
			if (bStep)
				bJumpIn = true;
			if (interpretWhile(stmts, i, iJumpState, bJumpOut, bJumpIn)) return true;
			break;

		case LEX_STATEMENT_DO:
			if (bStep)
				bJumpIn = true;
			if (interpretDo(stmts, i, iJumpState, bJumpOut, bJumpIn)) return true;
			break;

		case LEX_STATEMENT_FOR:
			++i;
			if (bStep)
				bJumpIn = true;
			if (interpretFor(stmts, i, iJumpState, bJumpOut, bJumpIn)) return true;
			break;

		case LEX_STATEMENT_SWITCH:
			if (bStep)
				bJumpIn = true;
			if (interpretSwitch(stmts, i, iJumpState, bJumpOut, bJumpIn)) return true;
			break;

		case LEX_STATEMENT_BREAK:
			if (interpretBreak(stmts, iJumpState, i)) return true;
			break;

		case LEX_STATEMENT_CONTINUE:
			if (interpretContinue(stmts, iJumpState, i)) return true;
			break;

		case LEX_STATEMENT_RETURN:
			if (interpretReturn(stmts, iJumpState, i)) return true;
			break;

		default:
			reportInterp("illegal statement"); return true;
		};

		if (iJumpState & _JUMP_BREAK || iJumpState & _JUMP_CONTINUE || iJumpState & _JUMP_RETURN) {
			break;	// break out of this loop through stmt block
		}

		// if we just jumped out of something, stop on next instruction
		if (m_bTriggerStop) {
			if (i+1 < (int)stmts.size()) {
				iStop = i+1;
				m_bTriggerStop = false;
			}
		}

		// reset bJumpIn
		bJumpIn = false;

		if (m_bReturn) return false;
	}

	return false;
}

bool cInterpret::evalScalarCondition(vector<tokData>& tokens, bool& bResult, bool& bDebugJumpOut, bool bDebugJumpIn)
{
	int dummyJumpState(0);

	if (interpretRvalue(tokens, dummyJumpState, bDebugJumpOut, bDebugJumpIn)) return true;
	if (!m_rvalue.IsScalar()) { reportInterp("condition must evaluate to a scalar"); return true; }
	if (m_rvalue.m.Get0R()==0.) {
		bResult = false;
	}
	else {
		bResult = true;
	}

	return false;
}

bool cInterpret::evalScalar(vector<tokData>& tokens, cContainer& Result, bool& bDebugJumpOut, bool bDebugJumpIn)
{
	int dummyJumpState(0);

	if (interpretRvalue(tokens, dummyJumpState, bDebugJumpOut, bDebugJumpIn)) return true;
	if (!m_rvalue.IsScalar()) { reportInterp("condition must evaluate to a scalar"); return true; }

	Result = m_rvalue;
	return false;
}

bool cInterpret::doPreActions( vector<tokData>& tokens )
{
	cContainer v, t;
	string name;

	for (int i=0; i<(int)tokens.size(); ++i)
	{
		name = tokens[i].name;
		if (tokens[i].nOperatorType == LEX_OPERATOR_PRE_INCR) {
			m_pSymbolTable->lookup(t,name);
			if (Add(v,t,1.)) return true;
			m_pSymbolTable->set(name, v);
		}
		else if (tokens[i].nOperatorType == LEX_OPERATOR_PRE_DECR) {
			m_pSymbolTable->lookup(t,name);
			if (Add(v,t,-1.)) return true;
			m_pSymbolTable->set(name, v);
		}
		else { reportInterp("unexpected token: ", tokens[i]); return true; }
	}
	return false;
}

bool cInterpret::doPostActions( vector<tokData>& tokens )
{
	cContainer v, t;
	string name;

	for (int i=0; i<(int)tokens.size(); ++i)
	{
		name = tokens[i].name;
		if (tokens[i].nOperatorType == LEX_OPERATOR_POST_INCR) {
			m_pSymbolTable->lookup(t,name);
			if (Add(v,t,1.)) return true;
			m_pSymbolTable->set(name, v);
		}
		else if (tokens[i].nOperatorType == LEX_OPERATOR_POST_DECR) {
			m_pSymbolTable->lookup(t,name);
			if (Add(v,t,-1.)) return true;
			m_pSymbolTable->set(name, v);
		}
		else { reportInterp("unexpected token: ", tokens[i]); return true; }
	}
	return false;
}

bool cInterpret::interpret( sStatement& stmt, int& iJumpState, bool& bDebugJumpOut, bool& bDebugJumpIn )
{
	// local copy of stmt tokens
	vector<tokData> tokens = stmt.tokens;
	vector<tokData> tokLeft;
	vector<valueBlob> Lvals, Rvals;
	tokData tok;
	bool bAssignedLvalues(false);
	int ix;
	string resultName;

	if (doPreActions(stmt.preTokens)) return true;

	switch (stmt.nType)
	{
	case LEX_STATEMENT_BREAK:
		iJumpState |= _JUMP_BREAK;
		return false;

	case LEX_STATEMENT_CONTINUE:
		iJumpState |= _JUMP_CONTINUE;
		return false;

	case LEX_STATEMENT_EXPRESSION:
		if (interpretRvalue( tokens, iJumpState, bDebugJumpOut, bDebugJumpIn )) { m_bNoRvalue = false; return true; }
		if (!m_bNoRvalue) {
			// report mgmt
			resultName = string("1");
			setResultName( resultName );
		}
		break;

	case LEX_STATEMENT_ASSIGNMENT:
		ix = findEquals( tokens );
		if (ix==-1 || ix==0) { reportInterp("syntax error in stmt"); return true; }
		tokLeft.clear();
		tokLeft.insert( tokLeft.begin(), tokens.begin(), tokens.begin()+ix );
		if (tokLeft.empty())
			resultName = "";
		else
			resultName = tokLeft[0].name;

		// reduce the lvalue tokens
		if (interpretLvalue( Lvals, tokLeft, bDebugJumpOut, bDebugJumpIn )) return true;
		// remove lvalue and equal tokens
		tokens.erase( tokens.begin(), tokens.begin()+ix+1 );
		// evaluate expression on RHS
		if (interpretRvalue( Lvals, bAssignedLvalues, tokens, iJumpState, bDebugJumpOut, bDebugJumpIn )) { m_bNoRvalue = false; return true; }
		if (!m_bNoRvalue) {
			// assign rvalues to lvalues (for now just consider one lvalue)
			if ( assignRvalue( Lvals, bAssignedLvalues ) ) return true;
			// report mgmt
			if (stmt.bReport) setResultName( resultName );
		}
		break;

	default:
		reportInterp("undefined statement type"); return true;
	};

	if (doPostActions(stmt.postTokens)) return true;

	stmt.bReport = stmt.bReport && !m_bNoRvalue;
	m_bNoRvalue = false;
	return false;
}

int cInterpret::nextFunction(vector<tokData>& tokens, int ix)
{
	// return index of m_tokBuf corresponding to the first fcn or binary operator
	int n;
	for (int i=ix; i<(int)tokens.size(); ++i)
	{
		n = tokens[i].nSymbolType;
		if (n==LEX_TYPE_OPERATOR ||
			n==LEX_TYPE_OPERATOR_UNITARY ||
			n==LEX_TYPE_OPERATOR_TERTIARY ||
			n==LEX_OPERATOR_POST_INCR ||
			n==LEX_OPERATOR_POST_DECR ||
			n==LEX_OPERATOR_PRE_INCR ||
			n==LEX_OPERATOR_PRE_DECR ||
			n==LEX_TYPE_OPERATOR_ACCESS ||
			n==LEX_TYPE_OPERATOR_HASH_ACCESS ||
			n==LEX_OPERATOR_DEREFERENCE ||
			n==LEX_TYPE_LVALUE_SEPARATOR ||
			n==LEX_TYPE_SCRIPT_CALL ||
			n==LEX_TYPE_SCRIPT_FCN_CALL ||
			n==LEX_TYPE_KEYWORD_FUNCTION ||
			n==LEX_TYPE_KEYWORD_COMMAND)
			return i;
	}

	return -1;
}

int cInterpret::findEquals( vector<tokData>& tokens )
{
	for (int i=0; i<(int)tokens.size(); ++i)
	{
		if (tokens[i].nSymbolType==LEX_TYPE_EQUALS) { return i; }
	}
	return -1;	// '=' not found
}

//bool cInterpret::assignRvalue( vector<valueBlob>& lvalueBlobs, vector<valueBlob>& rvalueBlobs )
//{
//	int ix;
//	cVariable vix;	// matrix of indices
//
//	cVariable rvalueTemp = m_rvalue;
//
//	// finish this...
//
//
//	cVariable v;
//	if (m_pSymbolTable->lookup(v, tokLeft[0].name))
//	{
//		// no current variable by that name, create new one
//		v = m_rvalue;
//		v.m_name = tokLeft[0].name;
//		if (m_pSymbolTable->insert( v )) { report("out of memory"); return true; }
//	}
//	else
//	{
//		// overwrite the existing variable
//		if (m_pSymbolTable->set( tokLeft[0].name, m_rvalue )) { report("out of memory"); return true; }
//	}
//
//	return false;
//}

bool cInterpret::interpretLvalue( vector<valueBlob>& lvalues, TOKS& tokens, bool& bDebugJumpOut, bool& bDebugJumpIn )
{	// this fcn takes all the lvalue tokens and produces a vector of lvalueBlob's (one for each argument)
	// the valueBlob's contain the variable token (the lvalue), along with a cVariable describing the
	// indices into the variable relevant for the assignment (lvalueBlob.vArgs)

	lvalues.clear();

	valueBlob blob;
	vector<valueBlob> oneBlob;

	oneBlob.push_back(blob);

	// chews up tokens as it goes...
	if (tokens.empty()) { reportInterp("missing lvalue"); return true; }

	int i = nextFunction(tokens,0);
	int ndel = 0;

	int nargs;
	vector<tokData> tokArgs;

	int ixArgSep = 0;
	int nargin(0);

	tokData tok = tokens[0];
	cContainer vLoc;
	vector<TOKS> tokArg;
	TOKS Ltokens, fcnOutputs;

	sBlock block;
	sBlock fcnBlock;
	vector<tokData> inToks;
	vector<tokData>::iterator it_cur;

	if (i==-1) {	// no operators or fcns, simple assignment?
		if (tokens.size() > 1) { reportInterp("illegal assignment"); return true; }
		// just one token
		blob.token = tokens[0];
		// verify
		if ( !(blob.token.nSymbolType == LEX_TYPE_LVALUE || blob.token.nSymbolType == LEX_TYPE_VARIABLE || blob.token.nSymbolType == LEX_TYPE_UNKNOWN) ) {
			reportInterp("illegal assignment"); return true;
		}
		// no arguments
		blob.vArg.Delete();
		blob.vArg.SetName( blob.token.name );
		// shove onto lvalues
		lvalues.push_back( blob );
		return false;
	}

	// proceed through all fcns from L to R
	while (i != -1)
	{
		switch (tokens[i].nSymbolType)
		{
		case LEX_TYPE_LVALUE_SEPARATOR:
			blob.token = tokens[i-1];
			lvalues.push_back( blob );
			ndel = 1;
			break;

		case LEX_TYPE_VARIABLE: case LEX_TYPE_UNKNOWN:
			blob.token = tokens[i];				// plain vanilla lvalue
			blob.vArg.IsEmpty();
			blob.vArg.SetName( tokens[i].name );
			lvalues.push_back( blob );
			break;

		case LEX_TYPE_OPERATOR_TERTIARY:		// tertiary
			if (i<3) { reportInterp("interpreter error, insufficient operands for tertiary operator", tokens[i]); return true; }
			if (doTertiaryOperation(vLoc, tokens[i-3], tokens[i-2], tokens[i-1], tokens[i].nOperatorType))
				{ m_rvalue = vLoc; return true; }
			ndel = 3;		// delete two entries in tokens array (binary operator)
			break;

		case LEX_TYPE_OPERATOR:					// binary
			if (i<2) { reportInterp("interpreter error, insufficient operands for binary operator", tokens[i]); return true; }
			if (doBinaryOperation(vLoc, tokens[i-2], tokens[i-1], tokens[i].nOperatorType))
				{ m_rvalue = vLoc; return true; }
			ndel = 2;		// delete two entries in tokens array (binary operator)
			break;

		case LEX_TYPE_OPERATOR_UNITARY:
			if (i<1) { reportInterp("interpreter error, no operands for unitary operator", tokens[i]); return true; }
			if (doUnitaryOperation(vLoc, tokens[i-1], tokens[i].nOperatorType))
				{ m_rvalue = vLoc; return true; }
			ndel = 1;		// delete one entries in tokens array (binary operator)
			break;

		case LEX_OPERATOR_DEREFERENCE:
			// just another type of hash access
		case LEX_TYPE_OPERATOR_HASH_ACCESS:

			if (i<1) { reportInterp("missing element access value", tokens[i]); return true; }

			if ( i==(tokens.size()-1) ) {	// last token, so shove rest onto lvalues
				if ( getLvalueBlobHash(blob, tokens, i) ) return true;
				lvalues.push_back( blob );
				blob.vArg.Delete();
				ndel = tokens[i].nargin + 1;
			}
			else			// this is not an assignment, but an access operator buried in an lvalue expression (e.g., a( b[2] ) = 3; )
			{
				nargs = tokens[i].nargin;
				tokArgs.clear();
				tokArgs.insert( tokArgs.begin(), tokens.begin()+i-nargs, tokens.begin()+i );
				if (doRvalueAccessOperationHash(vLoc, tokens[i-nargs-1] /* hash */, tokArgs /* args */))
					{ m_rvalue = vLoc; return true; }
				ndel = nargs+1;		// delete two entries in tokens array (binary operator)

				//// handling matrix/array access for rvalue's
				//switch (nargin) {
				//case 1:
				//	// single argument
				//	if (doRvalueAccessOperationHash(vLoc, tokens[i-2] /* hash */, tokens[i-1] /* key */))
				//		{ m_rvalue = vLoc; return true; }
				//	ndel = 2;		// delete two entries in tokens array (binary operator)
				//	break;

				//default:
				//	reportInterp("error: more than one hash access arguments not allowed", tokens[i]);
				//	return true;
				//};
			}
			break;

		case LEX_TYPE_OPERATOR_ACCESS:
			if (i<1) { reportInterp("missing element access value", tokens[i]); return true; }

			if ( i==(tokens.size()-1) ) {	// last token, so shove rest onto lvalues
				if ( getLvalueBlob(blob, tokens, i) ) return true;
				lvalues.push_back( blob );
				blob.vArg.Delete();
				ndel = tokens[i].nargin + 1;
			}
			else if (tokens[i+1].nSymbolType==LEX_TYPE_LVALUE_SEPARATOR) {

				if ( getLvalueBlob(blob, tokens, i) ) return true;
				lvalues.push_back( blob );
				blob.vArg.Delete();
				ndel = tokens[i].nargin + 1;
			}
			else			// this is not an assignment, but an access operator buried in an lvalue expression (e.g., a( b(2) ) = 3; )
			{
				// handling matrix/array access for rvalue's
				nargs = tokens[i].nargin;

				switch (nargs) {
				case 1:
					// single argument
					if (doRvalueAccessOperation(vLoc, tokens[i-2] /* matrix or vect */, tokens[i-1] /* arg */))
						{ m_rvalue = vLoc; return true; }
					ndel = 2;		// delete two entries in tokens array
					break;
				case 2:
					// two arguments
					if (doRvalueAccessOperation(vLoc, tokens[i-3] /* matrix or vect */, tokens[i-2] /* arg 1 */, tokens[i-1] /* arg 2 */))
						{ m_rvalue = vLoc; return true; }
					ndel = 3;		// delete three entries in tokens array
					break;
				case 3:
					// three arguments
					if (doRvalueAccessOperation(vLoc, tokens[i-4] /* matrix or vect */, tokens[i-3] /* arg 1 */, tokens[i-2] /* arg 2 */, tokens[i-1] /* arg 3 */))
						{ m_rvalue = vLoc; return true; }
					ndel = 4;		// delete four entries in tokens array
					break;
				default:
					reportInterp("error: more than 3 access arguments not allowed", tokens[i]);
					return true;
				};
			}

			break;

		case LEX_TYPE_SCRIPT_FCN_CALL:
			// Something like this:
			if (m_pFcnTable->lookup(fcnBlock, tokens[i].name, tokens[i].nargin, tokens[i].nargout)) { reportInterp("function not found", tokens[i], tokens[i].name); return true; }

			// get input and ouput args
			it_cur = tokens.begin() + i;
			// more than 1 lvalue is not allowed here
			oneBlob.back().vArg.IsEmpty();
			inToks.insert( inToks.begin(), it_cur - tokens[i].nargin, it_cur );
			// call the fcn
			if (interpretFcnBlock(vLoc, fcnBlock, inToks, oneBlob, tokens[i].nLineNo, bDebugJumpOut, bDebugJumpIn)) return true;
			bDebugJumpIn = false;
			ndel = tokens[i].nargin;
			break;

		case LEX_TYPE_KEYWORD_FUNCTION:
			if (i<(int)tokens[i].nargin) { reportInterp("interpreter error, insufficient operands for binary operator", tokens[i]); return true; }
			if (doFunctionCall(vLoc, tokens, i)) { return true; }
			if (m_bNoRvalue) {
				reportInterp("interpreter error, function with no return value found on left-hand side", tokens[i]); return true;
			}
			ndel = tokens[i].nargin;
			break;

		default:
			reportInterp("unsupported function or operator", tokens[i]);
			return true;
		};

		// change operator token to rvalue type
		tokens[i].nSymbolType = LEX_TYPE_TEMP_VARIABLE;
		tokens[i].ix = m_tempVars.size();

		m_tempVars.push_back( vLoc );

		// erase ndel tokens preceding operator
		tokens.erase( tokens.begin()+i-ndel, tokens.begin()+i );

		i = nextFunction(tokens, i-ndel);
	}

	// clear temps
	m_tempVars.clear();
	// set rvalue
	m_rvalue = vLoc;
	return false;
}

bool cInterpret::getLvalueBlobHash( valueBlob& blob, vector<tokData>& tokens, int i )
{
	tokData tok = tokens[i];
	int nargin = tok.nargin;

	string arg_name;
	blob.hashArgs.clear();

	blob.token = tokens[i-nargin-1];
	blob.vArg.Delete();

	cContainer t;

	for (int j=i-nargin; j<i; ++j)
	{
		if ( tok2container( t, tokens[j] ) ) return true;		// used to be call to getVar(..)
		if (t.GetType() != VAR_TYPE_STRING) { addError("non-string hash argument"); return true; }
		arg_name = t.s.GetString();
		blob.hashArgs.push_back( arg_name );
	}

	return false;
}

bool cInterpret::getLvalueBlob( valueBlob& blob, vector<tokData>& tokens, int i )
{
	tokData tok = tokens[i];
	int nargin = tok.nargin;

	if (nargin > 3) { reportInterp("cannot index more than 3 dimensions", tok); return true; }

	cContainer vRows, vCols, vDepth;

	blob.vArg.Delete();
	blob.token = tokens[i-nargin-1];

	if ( getVarDimensions( blob.nRows, blob.nColumns, blob.nDepth, blob.token ) ) { reportInterp("illegal argument", blob.token); return true; }

	// handling matrix/array access for rvalue's

	switch (nargin) {
	case 1:
		// single argument
		if (doLvalueAccessOperation(blob, tokens[i-1] /* arg */))
			{ return true; }
		break;

	case 2:
		// two arguments
		if (doLvalueAccessOperation(blob, tokens[i-2] /* arg 1 */, tokens[i-1] /* arg 2 */))
			{ return true; }
		break;
	case 3:
		// three arguments
		if (doLvalueAccessOperation(blob, tokens[i-3] /* arg 1 */, tokens[i-2] /* arg 2 */, tokens[i-1] /* arg 3 */))
			{ return true; }
		break;
	default:
		reportInterp("cannot index more than 3 dimensions", tokens[i]); return true;
	};

	return false;
}

bool cInterpret::getVarDimensions( int& nRows, int& nCols, int& nDepth, tokData& tok )
{
	switch (tok.nSymbolType)
	{
	case LEX_TYPE_UNKNOWN:
		if (m_pSymbolTable->lookup(tok.name)) { reportInterp("undefined symbol", tok); return true; }
		// fall through...

	case LEX_TYPE_VARIABLE:
		if (m_pSymbolTable->getSize(nRows, nCols, nDepth, tok.name)) { reportInterp("undefined symbol", tok); return true; }
		break;

	default:
		reportInterp("illegal lvalue", tok);
		return true;
	}

	return false;
}

bool cInterpret::getVar( cContainer& v, tokData& tok )
{
	int nType, nrows, ncols, ndeep;
	cVariable t;
	v.Delete();

	switch (tok.nSymbolType)
	{
	case LEX_OPERATOR_ACCESS_END:
		if (m_pSymbolTable->lookupInfo(tok.name, nType, nrows, ncols, ndeep)) {
			reportInterp("undefined symbol", tok.nLineNo, tok.name);
			return true;
		}
		tok.fSymbolVal = nrows*ncols*ndeep - 1;

	case LEX_TYPE_CONSTANT:
		v.SetTo( tok.fSymbolVal );
		break;

	case LEX_TYPE_STRING: case LEX_TYPE_STRUCT_ACCESSOR:
		v.SetTo( tok.name );
		break;

	case LEX_TYPE_EMPTY:
		v.SetType(VAR_TYPE_NUMERIC);	// really, this is arbitrary
		return false;

	case LEX_TYPE_RVALUE:
		v = m_rvalue;
		break;

	case LEX_TYPE_UNKNOWN:
		// fall through...

	case LEX_TYPE_VARIABLE:
		if (m_pSymbolTable->lookup(v, tok.name)) { reportInterp("undefined symbol", tok); return true; }
		break;

	case LEX_TYPE_TEMP_VARIABLE:
		v = m_tempVars[tok.ix];
		break;

	default:
		reportInterp("illegal lvalue", tok);
		return true;
	};

	return false;
}


bool cInterpret::interpretRvalue( vector<tokData>& tokens, int& iJumpState, bool& bDebugJumpOut, bool& bDebugJumpIn )
{
	// just one lvalue
	vector<valueBlob> blobs;
	valueBlob b;
	blobs.push_back(b);
	bool bAssignLvalue(false);

	if ( interpretRvalue( blobs, bAssignLvalue, tokens, iJumpState, bDebugJumpOut, bDebugJumpIn ) ) return true;

	return false;
}


bool cInterpret::interpretRvalue( vector<valueBlob>& lblobs, bool& bAssigned, vector<tokData>& tokens, int& iJumpState, bool& bDebugJumpOut, bool& bDebugJumpIn )
{
	// chews up tokens as it goes...
	if (tokens.empty()) { reportInterp("missing rvalue"); return true; }

	// lblobs assigned? (.vResult)
	bAssigned = false;

	string strMsg;

	int i = nextFunction(tokens,0);
	int tokPos(0);
	int ndel = 0;
	int nargs;

	tokData tok = tokens[0];
	cContainer vLoc;

	sBlock block, blockCopy;
	sBlock fcnBlock;

	vector<tokData> tokArgs;

	// clear temps
	m_tempVars.clear();

	if (i==-1) {	// no operators or fcns, simple assignment?
		// just eval token
		if ( tok2container(m_rvalue, tok) ) {
			reportInterp("undefined symbol", tok);
			return true;
		}
		if ( !lblobs.empty() ) {
			lblobs[0].vResult = m_rvalue;
		}

		return false;
	}

	// proceed through all fcns from L to R
	while (i != -1)
	{
		switch (tokens[i].nSymbolType)
		{
		case LEX_OPERATOR_POST_INCR: case LEX_OPERATOR_POST_DECR: case LEX_OPERATOR_PRE_INCR: case LEX_OPERATOR_PRE_DECR:
			if (i<1) { reportInterp("interpreter error, operand not found for post-increment", tokens[i]); return true; }
			if (doUnitaryOperation(vLoc, tokens[i-1], tokens[i].nOperatorType))
				{ m_rvalue = vLoc; return true; }
			ndel = 1;
			break;

		case LEX_TYPE_OPERATOR_TERTIARY:		// tertiary
			if (i<3) { reportInterp("interpreter error, insufficient operands for tertiary operator", tokens[i]); return true; }
			if (doTertiaryOperation(vLoc, tokens[i-3], tokens[i-2], tokens[i-1], tokens[i].nOperatorType))
				{ m_rvalue = vLoc; return true; }
			ndel = 3;		// delete two entries in tokens array (binary operator)
			break;

		case LEX_TYPE_OPERATOR:		// binary
			if (i<2) { reportInterp("interpreter error, insufficient operands for binary operator", tokens[i]); return true; }
			if (doBinaryOperation(vLoc, tokens[i-2], tokens[i-1], tokens[i].nOperatorType))
				{ m_rvalue = vLoc; return true; }
			ndel = 2;		// delete two entries in tokens array (binary operator)
			break;

		case LEX_TYPE_OPERATOR_UNITARY:
			if (i<1) { reportInterp("interpreter error, no operands for unitary operator", tokens[i]); return true; }
			if (doUnitaryOperation(vLoc, tokens[i-1], tokens[i].nOperatorType))
				{ m_rvalue = vLoc; return true; }
			ndel = 1;		// delete one entries in tokens array (binary operator)
			break;

		case LEX_TYPE_OPERATOR_ACCESS:
			if (i<1) { reportInterp("missing element access value", tokens[i]); return true; }

			// handling matrix/array access for rvalue's

			switch (tokens[i].nargin) {
			case 1:
				// single argument
				if (doRvalueAccessOperation(vLoc, tokens[i-2] /* cube, matrix, or vect */, tokens[i-1] /* arg */))
					{ m_rvalue = vLoc; return true; }
				ndel = 2;		// delete two entries in tokens array (binary operator)
				break;

			case 2:
				// two arguments
				if (doRvalueAccessOperation(vLoc, tokens[i-3] /* cube, matrix, or vect */, tokens[i-2] /* arg 1 */, tokens[i-1] /* arg 2 */))
					{ m_rvalue = vLoc; return true; }
				ndel = 3;		// delete two entries in tokens array (binary operator)
				break;
			case 3:
				// three arguments
				if (doRvalueAccessOperation(vLoc, tokens[i-4] /* cube only */, tokens[i-3] /* arg 1 */, tokens[i-2] /* arg 2 */, tokens[i-1] /* arg 3 */))
					{ m_rvalue = vLoc; return true; }
				ndel = 4;		// delete two entries in tokens array (binary operator)
				break;
			default:
				reportInterp("error: more than two matrix access arguments not allowed", tokens[i]); return true;
			};
			break;

		case LEX_OPERATOR_DEREFERENCE:
			// just another type of hash access
		case LEX_TYPE_OPERATOR_HASH_ACCESS:
			if (i<1) { reportInterp("missing element access value", tokens[i]); return true; }

			nargs = tokens[i].nargin;
			tokArgs.clear();
			tokArgs.insert( tokArgs.begin(), tokens.begin()+i-nargs, tokens.begin()+i );
			if (doRvalueAccessOperationHash(vLoc, tokens[i-nargs-1] /* hash */, tokArgs /* args */))
				{ m_rvalue = vLoc; return true; }
			ndel = nargs+1;		// delete two entries in tokens array (binary operator)
			break;

		case LEX_TYPE_SCRIPT_CALL:
			if (m_pScriptTable->lookup(block, tok.name)) { reportInterp("undefined symbol", tok, tok.name); return true; }
			blockCopy = block;
			if (makeScriptCall(blockCopy, tok.name, tok.nLineNo, bDebugJumpOut, bDebugJumpIn)) 
				return true;
			bAssigned = true;
			m_bNoRvalue = true;
			bDebugJumpIn = false;
			break;

		case LEX_TYPE_SCRIPT_FCN_CALL:
			// Something like this:
			if (m_pFcnTable->lookup(fcnBlock, tokens[i].name, tokens[i].nargin, tokens[i].nargout)) { reportInterp("function not found", tokens[i], tokens[i].name); return true; }
			if (makeFcnCall(lblobs, vLoc, bAssigned, fcnBlock, tokens, i, bDebugJumpOut, bDebugJumpIn))
				return true;

			bDebugJumpIn = false;
			ndel = tokens[i].nargin;
			break;

		case LEX_TYPE_KEYWORD_FUNCTION:
			if (i<tokens[i].nargin) { reportInterp("interpreter error, too few arguments given to function", tokens[i]); return true; }

			// call the fcn
			if (i!=(tokens.size()-1)) {
				if (doFunctionCall(vLoc, tokens, i)) return true;
				if (m_bNoRvalue) {
					reportInterp("interpreter error, invalid expression using function with no return value", tokens[i]); return true;
				}
			}
			else {
				if (doFunctionCall(tokens, i, lblobs)) return true;
				if (lblobs.empty())
					m_bNoRvalue = true;

				if (!m_bNoRvalue) getRvalue(vLoc, lblobs);
			}
			ndel = tokens[i].nargin;
			break;

		case LEX_TYPE_KEYWORD_COMMAND:
			if (i<tokens[i].nargin) { reportInterp("interpreter error, too few arguments given to command", tokens[i]); return true; }

			// call the fcn
			if (i!=(tokens.size()-1)) {
				reportInterp("illegal use of command", tokens[i]); return true;
			}
			else {
				bAssigned = true;
				m_bNoRvalue = true;
				if (doCommandCall(tokens, i)) { return true; }
			}
			ndel = tokens[i].nargin;
			break;

		default:
			reportInterp("unsupported function or operator", tokens[i]);
			return true;
		};

		// change operator token to rvalue type
		tokens[i].nSymbolType = LEX_TYPE_TEMP_VARIABLE;
		tokens[i].ix = m_tempVars.size();

		m_tempVars.push_back( vLoc );

		// erase ndel tokens preceding operator
		tokens.erase( tokens.begin()+i-ndel, tokens.begin()+i );
		//tokPos = i;

		i = nextFunction(tokens, i-ndel);
		//i = nextFunction(tokens, i+1);

		if (i != -1 && m_bNoRvalue) {
			reportInterp("function returns no value", tokens[i]);
			return true;
		}
	}

	// clear temps
	m_tempVars.clear();
	// set rvalue
	m_rvalue = vLoc;

	if ( !bAssigned && !lblobs.empty() ) {
		lblobs[0].vResult = m_rvalue;
	}

	return false;
}

bool cInterpret::makeScriptCall(sBlock& block, string& name, int nLineNo, bool& bDebugJumpOut, bool bDebugJumpIn)
{
	bool bErr(false);
	errorReport error_state;
	string saveFname, savePath;

	// error state
	error_state = getErrorState();
	setErrorState( block.fileName, nLineNo );

	// file state
	saveFname = m_curFile;
	savePath = m_curPath;
	m_curFile = block.fileName;
	m_curPath = block.pathName;

	string fname = block.pathName + "\\";
	fname.append( block.fileName );
	sStackEntry se;
	se.bReload = false;
	se.fullFile = fname;
	se.nLine = m_stack.empty() ? -1 : nLineNo;
	se.ipos = T.GetSize()-1;		// pts to top of vs stack
	se.itype = LEX_TYPE_SCRIPT_CALL;
	se.name = name;
	m_stack.push_back( se );

	bool bJumptOutPrior = bDebugJumpOut;

	// interpret script
	if ( interpretBlock(block, bDebugJumpOut, bDebugJumpIn) )	{ bErr = true; }

	// check and set jump out flag
	if (!bJumptOutPrior && bDebugJumpOut) {
		m_bTriggerStop = true;
		bDebugJumpOut = false;
	}

	// restore file state
	m_stack.pop_back();
	m_curFile = saveFname;
	m_curPath = savePath;

	// restore error msg prefix
	setErrorState( error_state );
			
	return bErr;
}

bool cInterpret::makeFcnCall(vector<valueBlob>& lblobs, cContainer& vLoc, bool& bAssigned, sBlock& fcnBlock, vector<tokData>& tokens, int i, bool& bDebugJumpOut, bool bDebugJumpIn)
{
	vector<tokData> inToks;
	vector<tokData>::iterator it_cur;

	int nLineNo = tokens[i].nLineNo;
	// get input and ouput args
	it_cur = tokens.begin() + i;
	inToks.clear();
	inToks.insert( inToks.begin(), it_cur - tokens[i].nargin, it_cur );
	// call the fcn
	if (i!=(tokens.size()-1)) {
		if (interpretFcnBlock(vLoc, fcnBlock, inToks, nLineNo, bDebugJumpOut, bDebugJumpIn)) { return true; }
	}
	else {
		bAssigned = true;
		if (interpretFcnBlock(vLoc, fcnBlock, inToks, lblobs, nLineNo, bDebugJumpOut, bDebugJumpIn)) { return true; }
	}

	return false;
}

bool cInterpret::makeFcnCall(cContainer& vLoc, sBlock& fcnBlock, vector<tokData>& tokens, int i, bool& bDebugJumpOut, bool bDebugJumpIn)
{
	// This fcn call assumes only 1 return value (stored in vLoc)
	//
	vector<tokData> inToks;
	vector<tokData>::iterator it_cur;

	// get input and ouput args
	it_cur = tokens.begin() + i;
	inToks.clear();
	inToks.insert( inToks.begin(), it_cur - tokens[i].nargin, it_cur );
	// call the fcn
	if (interpretFcnBlock(vLoc, fcnBlock, inToks, tokens[i].nLineNo, bDebugJumpOut, bDebugJumpIn)) { return true; }

	return false;
}

bool cInterpret::assignRvalue(vector<valueBlob>& lblobs, bool bAssignedLvalues)
{
	if (m_pSymbolTable==NULL) return true;

	// set lvalues
	if (!bAssignedLvalues)
	{
		for (int i=0; i<(int)lblobs.size(); ++i) {
			lblobs[i].vResult.SetName( lblobs[i].token.name );		// set all lvalue names according to token name
			if (!lblobs[i].hashArgs.empty()) {
				m_pSymbolTable->hashSet( lblobs[i].token.name, lblobs[i].hashArgs, lblobs[i].vResult );
			}
			else if (lblobs[i].vArg.IsEmpty())
				m_pSymbolTable->insert( lblobs[i].vResult );
			else {	// indexed assignment
				if ( m_pSymbolTable->indexSet(lblobs[i].vResult, lblobs[i].vArg) ) { reportInterp("illegal assignment (size mismatch?) ", lblobs[i].token); return true; }
			}
		}
	}

	return false;
}

bool cInterpret::getRvalue(cContainer& Result, vector<valueBlob>& lblobs)
{
	Result.Delete();

	if (lblobs.size()>0) {
		Result = lblobs[0].vResult;
	}

	return false;
}

//typedef struct tagTokInfo
//{
//	int ix, ixh;
//	int nSymbolType;
//	double fSymbolVal;				// if a numerical constant
//  int nOperatorType;		// if an operator
	//int nargin;			// no. of input arguments given to this token (if a fcn token)
//} tokInfo;

//// operator types
//enum { LEX_OPERATOR_PLUS=1500, LEX_OPERATOR_MINUS, LEX_OPERATOR_MULT, LEX_OPERATOR_DIV, LEX_OPERATOR_MOD, LEX_OPERATOR_POW };	// and so on...


// eval fcn token at ix
bool cInterpret::doFunctionCall(cContainer& Result, vector<tokData>& tokens, int ix)
{
	vector<valueBlob> blob;
	valueBlob b;
	blob.push_back(b);

	Result.Delete();

	if ( doFunctionCall( tokens, ix, blob ) ) return true;
	
	if (!m_bNoRvalue) Result = blob[0].vResult;

	return false;
}

bool cInterpret::doFunctionCall(vector<tokData>& tokens, int ix, vector<valueBlob>& lblobs)
{
	tokData tok = tokens[ix];
	keyData key;
	cContainer varg_1, varg_2, varg_3, varg_4;
	string str;
	int i;

	map<string,keyData>::iterator it = m_pKeywordTable->find(tok.name);

	if (it == m_pKeywordTable->end())
	{ reportInterp("parsing error found while interpreting", tok); return true; }	// let's hope this never happens...

	// ensure there are enough preceeding tokens to hold all the arguments (avoid indexing errors)
	if ( tok.nargin > ix ) {
		str = "too few arguments given to function: " + tok.name;
		reportInterp(str.c_str());
		return true;
	}

	vector<cContainer> vv;
	int nargout = lblobs.size();

	cMyPair pairIn, pairOut;
	key = it->second;
	pairIn = key.i2o.rbegin()->first;		// just considering the first entry in the map. if more than one valid call to fcn, each must have .nUnknown set properly
	pairOut = key.i2o[pairIn];

	if (pairOut.narg < (int)lblobs.size()) {
		lblobs.erase( lblobs.begin() + pairOut.narg, lblobs.end() );
		if (lblobs.empty())
			m_bNoRvalue = true;
	}

	if (pairIn.nUnkown > 0) {				// pairIn.nUnkown = number of MINIMUM arguments, if not equal to zero
		if (tok2containerVector( vv, tokens, ix ))
			return true;

		if (pairOut.narg > 0) {
			for (i=0; i<(int)lblobs.size(); ++i) {			// output args
				vv.insert( vv.begin() + i, lblobs[i].vArg );
			}
		}

		//if (call_vin_arg(key, tok, tok.nargin, nargout, vv))
		if (call_vin_arg(key, tok, tok.nargin, pairOut.narg, vv))
			return true;

		for (i=0; i<pairOut.narg /*(int)lblobs.size()*/; ++i) {			// set lblobs[x].vResult
			lblobs[i].vResult = vv[i];
		}
		return false;
	}

	if (pairOut.nUnkown > 0) {
		if (tok2containerVector( vv, tokens, ix ))		// input args
			return true;
		for (i=0; i<(int)lblobs.size(); ++i) {			// output args
			vv.insert( vv.begin() + i, lblobs[i].vArg );
		}
		if (call_vout_arg(key, tok, tok.nargin, nargout, vv))
			return true;
		for (i=0; i<(int)lblobs.size(); ++i) {			// set lblobs[x].vResult
			lblobs[i].vResult = vv[i];
		}
		return false;
	}

	valueBlob vbDummy;
	if (pairOut.narg < nargout) {		// trim lblobs as needed
		lblobs.erase( lblobs.begin() + pairOut.narg, lblobs.end() );
		nargout = pairOut.narg;
	}
	else if (pairOut.narg > nargout) {
		for (i=0; i<pairOut.narg - nargout; ++i)
			lblobs.push_back( vbDummy );
	}

	nargout = lblobs.size();

	if (tok.nargin==0) {
		return call(lblobs, key, tok);
	}
	else if (tok.nargin==1) {
		if (tok2container( varg_1, tokens[ix-1] )) { reportInterp("illegal argument", tokens[ix-1]); return true; }
		return call( lblobs, key, tok, tok.nargin, varg_1 );
	}
	else if (tok.nargin==2) {
		if (tok2container( varg_1, tokens[ix-2] )) { reportInterp("illegal argument", tokens[ix-2]); return true; }
		if (tok2container( varg_2, tokens[ix-1] )) { reportInterp("illegal argument", tokens[ix-1]); return true; }
		return call( lblobs, key, tok, tok.nargin, varg_1, varg_2 );
	}
	else if (tok.nargin==3) {
		if (tok2container( varg_1, tokens[ix-3] )) { reportInterp("illegal argument", tokens[ix-3]); return true; }
		if (tok2container( varg_2, tokens[ix-2] )) { reportInterp("illegal argument", tokens[ix-2]); return true; }
		if (tok2container( varg_3, tokens[ix-1] )) { reportInterp("illegal argument", tokens[ix-1]); return true; }
		return call( lblobs, key, tok, tok.nargin, varg_1, varg_2, varg_3 );
	}
	else if (tok.nargin==4) {
		if (tok2container( varg_1, tokens[ix-4] )) { reportInterp("illegal argument", tokens[ix-4]); return true; }
		if (tok2container( varg_2, tokens[ix-3] )) { reportInterp("illegal argument", tokens[ix-3]); return true; }
		if (tok2container( varg_3, tokens[ix-2] )) { reportInterp("illegal argument", tokens[ix-2]); return true; }
		if (tok2container( varg_4, tokens[ix-1] )) { reportInterp("illegal argument", tokens[ix-1]); return true; }
		return call( lblobs, key, tok, tok.nargin, varg_1, varg_2, varg_3, varg_4 );
	}
	else {
		if (tok2containerVector( vv, tokens, ix )) {
			reportInterp("illegal argument given to function", tok, tok.name);
			return true;
		}
		return call( lblobs, key, tok, tok.nargin, vv );
	}

	//m_bNoRvalue = true;

	return false;		// A-ok...
}

bool cInterpret::doCommandCall(vector<tokData>& tokens, int ix)
{
	tokData tok = tokens[ix];
	keyData key;
	cContainer varg_1, varg_2, varg_3;
	string str;

	int nargout = 0;
	bool bErr(false);

	map<string,keyData>::iterator it = m_pCommandTable->find(tok.name);

	if (it == m_pCommandTable->end())
	{ reportInterp("parsing error found while interpreting", tok); return true; }	// let's hope this never happens...

	// ensure there are enough preceeding tokens to hold all the arguments (avoid indexing errors)
	if ( tok.nargin > ix ) {
		str = "too few arguments given to command: " + tok.name;
		reportInterp(str.c_str(), tok);
		return true;
	}

	vector<cString> vs;

	if (tok.nargin==0) {
		return call_0_arg(key, tok, tok.nargin, nargout);
	}
	else if (tok.nargin==1) {
		if (tok2container( varg_1, tokens[ix-1] )) { reportInterp("illegal argument", tokens[ix-1]); return true; }
		return callCmd(key, tok, tok.nargin, nargout, varg_1);
	}
	else if (tok.nargin==2) {
		if (tok2container( varg_1, tokens[ix-2] )) { reportInterp("illegal argument", tokens[ix-2]); return true; }
		if (tok2container( varg_2, tokens[ix-1] )) { reportInterp("illegal argument", tokens[ix-1]); return true; }
		return callCmd(key, tok, tok.nargin, nargout, varg_1, varg_2);
	}
	else if (tok.nargin==3) {
		if (tok2container( varg_1, tokens[ix-3] )) { reportInterp("illegal argument", tokens[ix-3]); return true; }
		if (tok2container( varg_2, tokens[ix-2] )) { reportInterp("illegal argument", tokens[ix-2]); return true; }
		if (tok2container( varg_3, tokens[ix-1] )) { reportInterp("illegal argument", tokens[ix-1]); return true; }
		return callCmd(key, tok, tok.nargin, nargout, varg_1, varg_2, varg_3);
	}
	else {
		if (tok2stringVector( vs, tokens, ix )) {
			reportInterp("illegal argument given to command", tok, tok.name);
			return true;
		}
		//call_vin_0_arg(key, tok, tok.nargin, nargout, vv);
		if (key.str_ni_0o==NULL) {
			reportInterp("no function compatible with argument list provided", tok); return true;
		}
		else return key.str_ni_0o( vs );
	}

	if (nargout == 0) {
		m_bNoRvalue = true;
	}

	//call(key, tok, tok.nargin, nargout);

	return false;		// A-ok...
}


//
// check arg compatability
//
bool cInterpret::checkCompatability(cContainer& v1, cContainer& v2, char op)
{
	bool b1 = v1.GetType()==VAR_TYPE_STRING;
	bool b2 = v2.GetType()==VAR_TYPE_STRING;

	if (b1 && !b2) return false;
	if (!b1 && b2) return false;
	bool bStrings = b1 && b2;

	if ( !bStrings && (v1.IsScalar() || v2.IsScalar()) ) return true;

	if (bStrings) {

		switch (op) {
		case '+': return true;
		default: return false;
		};
	}
	else {

		switch (op)
		{
		case '+': case '-': case '%': case 'M': case 'D':	// element-by-element operations
			return (v1.GetRows()==v2.GetRows() && v1.GetColumns()==v2.GetColumns());

		case '*':
			return v1.GetColumns()==v2.GetRows();

		case '/':
			// check to see that we have b/A (inv(A)*b), where we are solving system Ax=b
			return (v1.GetColumns()==v1.GetRows() && v1.GetColumns()==v2.GetRows());

		case '^':	// element-by-element operation
			return (v1.GetRows()==v2.GetRows() && v1.GetColumns()==v2.GetColumns());

		case 'R':	// range operator '..'
			return (v1.IsScalar() && v2.IsScalar());

		case 'v':	// VertCat
			return (v1.GetColumns() == v2.GetColumns());

		case 'h':	// HorzCat
			return (v1.GetRows() == v2.GetRows());

		default:
			reportInterp("unknown operator");
		};
	}

	return false;	// incompatable operands
}

bool cInterpret::checkCompatability(cContainer& v1, cContainer& v2, cContainer& v3, char op)
{
	switch (op)
	{
	case 'R':	// range operator '..'
		return (v1.IsScalar() && v2.IsScalar() && v3.IsScalar());

	default:
		reportInterp("unknown operator");
	};

	return false;	// incompatable operands
}

bool cInterpret::checkValidLvalues( vector<tokData>& leftTokens )
{
	for (int i=0; i<(int)leftTokens.size(); ++i)
	{
		if (!checkValidLvalue( leftTokens[i] )) return false;
	}
	return true;
}

bool cInterpret::checkValidLvalue( tokData& tok )
{
	if (tok.nSymbolType == LEX_TYPE_LVALUE ||		// all lvalues and variables are OK
		tok.nSymbolType == LEX_TYPE_UNKNOWN ||
		tok.nSymbolType == LEX_TYPE_VARIABLE) return true;

	return false;	// not a valid lvalue!
}

bool cInterpret::tok2container( cContainer& c, tokData& tok )
{
	c.Delete();

	switch (tok.nSymbolType)
	{
	case LEX_TYPE_UNKNOWN:
		tok.nSymbolType = LEX_TYPE_VARIABLE;
		//if (m_pSymbolTable->getAddr( tok.ix, tok.ixh, tok.name )) { reportInterp("interpreter error", tok); return true; }
		// fall through...
	case LEX_TYPE_VARIABLE:
		c.SetName( tok.name );
		return m_pSymbolTable->lookup( c, tok.name );
	case LEX_TYPE_EMPTY:
		c.SetType(VAR_TYPE_NUMERIC);	// really, this is arbitrary
		return false;
	case LEX_TYPE_TEMP_VARIABLE:
		c = m_tempVars[tok.ix]; break;
	case LEX_TYPE_CONSTANT:
		c.SetTo( tok.fSymbolVal ); break;
	case LEX_TYPE_STRING: case LEX_TYPE_STRUCT_ACCESSOR:
		c.SetTo( tok.name ); break;
	case LEX_TYPE_RVALUE:
		c = m_rvalue; break;
	//case LEX_TYPE_FCN_POINTER:
	//	v.Set( tok.name ); v.Type = VAR_TYPE_FCN_POINTER; break;
	default:
		reportInterp("undefined token", tok);
		return true;		// bad tok...
	};

	return false;
}

bool cInterpret::tok2containerVector( vector<cContainer>& cv, vector<tokData>& tokens, int ix )
{
	cContainer c;
	cv.clear();

	int nargin = tokens[ix].nargin;
	if (ix-nargin < 0) return true;

	for (int i=0; i<tokens[ix].nargin; ++i) {
		if (tok2container(c, tokens[ix-nargin+i])) { cv.clear(); return true; }
		cv.push_back(c);
	}

	return false;
}

bool cInterpret::tok2stringVector( vector<cString>& vs, vector<tokData>& tokens, int ix )
{
	cContainer c;
	cString cs;
	vs.clear();

	int nargin = tokens[ix].nargin;
	if (ix-nargin < 0) return true;

	for (int i=0; i<tokens[ix].nargin; ++i) {
		if (tok2container(c, tokens[ix-nargin+i])) { vs.clear(); return true; }
		if (c.GetType() != VAR_TYPE_STRING) {
			reportInterp("expected string type", tokens[ix]);
			vs.clear();
			return true;
		}
		cs.SetTo( c.s );
		vs.push_back(cs);
	}

	return false;
}

//bool cInterpret::tok2variableVector( vector<cVariable>& vv, vector<tokData>& tokens, int ix )
//{
//	cVariable v;
//	vv.clear();
//
//	int nargin = tokens[ix].nargin;
//	if (ix-nargin < 0) return true;
//
//	for (int i=0; i<tokens[ix].nargin; ++i) {
//		if (tok2variable(v, tokens[ix-nargin+i])) { vv.clear(); return true; }
//		vv.push_back(v);
//	}
//
//	return false;
//}
