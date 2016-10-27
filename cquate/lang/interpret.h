#pragma once

#include "structs.h"
#include "lexTypes.h"
#include "fcnHash.h"
#include "scriptHash.h"
#include "map_hash.h"

typedef struct tagInterpState {
	keyData		kd;
	sStatement	stmt;
	sBlock		block;
	string		lvalueName;
} interpState;

typedef struct tagStackEntry {
	int ipos;				// statement index (instruction pointer)
	int nLine;				// line number of fcn/script call
	int itype;				// LEX_TYPE_SCRIPT_CALL or LEX_TYPE_SCRIPT_FCN_CALL
	string name;			// fcn/script name
	string fullFile;		// full file name containing fcn/script
	int nargin;				// number of args in (fcns only)
	int nargout;			// number of args out (fcns only)
	bool bReload;			// requires reloading from script or fcn table? (due to changes in debug break points)
} sStackEntry;


class cInterpret
{
public:
	cInterpret() {
		m_bNoRvalue = false;

		m_bTriggerStop = false;			// used for debugger stepping out of fcns

		// debug support
		m_ixStop = -1;			// -1 indicates no stopping!
		m_bJumpOut = true;		// indicates to keep going until out of fcn

		// Get current working directory
		char dir[MAX_PATH_BUFFER];
		__GETCWD( dir, MAX_PATH_BUFFER-1 );
		m_RunDir = dir;

		m_bLvalueInterp = false;

		m_bReturn = false;

		m_curPath = "";
		m_curFile = "";
		m_curFcn = "";

		m_resultName = "result";
	}
	~cInterpret() { }

	void reportInterp( const char *pchMsg );
	void reportInterp( string msg, string& name, int iLine );
	void reportInterp( const char *pchMsg, tokData& tok );
	void reportInterp( const char *pchMsg, int iLine );
	void reportInterp( string msg, tokData& tok );
	void reportInterp( string msg, int iLine );
	void reportInterp( const char *pchMsg, tokData& tok, string symbol );
	void reportInterp( const char *pchMsg, int iLine, string symbol );
	void reportInterp( string msg, tokData& tok, string symbol );
	void reportInterp( string msg, int iLine, string symbol );

	void initialize(sBlock& block);
	sBlock uninitialize();

	void pushInterpState(void);
	void popInterpState(void);

	bool checkForAbort(void);

	bool interpretStmt(void);
	//bool interpretStmts( vector<sStatement>& stmts, bool bDebugJumpIn );
	bool interpretStmts( vector<sStatement>& stmts, int& iJumpState, bool& bJumpOut, bool bJumpIn );
	bool interpretBlock( sBlock& block, bool& bDebugJumpOut, bool bDebugJumpIn );
	bool interpretFcnBlock( cContainer& v_ret, sBlock& block, vector<tokData>& inToks, int nLineNo, bool& bDebugJumpOut, bool bDebugJumpIn );	// no lvalue, just return v_ret
	bool interpretFcnBlock( cContainer& v_ret, sBlock& block, vector<tokData>& inToks, vector<valueBlob>& Lblobs, int nLineNo, bool& bDebugJumpOut, bool bDebugJumpIn );
	bool interpretFcnBlock( cContainer& v_ret, sBlock& block, cContainer& varg1, cContainer& varg2 );	// for ODE support

	bool interpretIf(vector<sStatement>& stmts, int& ix, int& iJumpState, bool& bJumpOut, bool bDebugJumpIn);
	bool interpretWhile(vector<sStatement>& stmts, int& ix, int& iJumpState, bool& bJumpOut, bool bDebugJumpIn);
	bool interpretDo(vector<sStatement>& stmts, int& ix, int& iJumpState, bool& bJumpOut, bool bDebugJumpIn);
	bool interpretFor(vector<sStatement>& stmts, int& ix, int& iJumpState, bool& bJumpOut, bool bDebugJumpIn);
	bool interpretSwitch(vector<sStatement>& stmts, int& ix, int& iJumpState, bool& bJumpOut, bool bDebugJumpIn);
	bool interpretBreak( vector<sStatement>& stmts, int& iJumpState, int& ix );
	bool interpretContinue( vector<sStatement>& stmts, int& iJumpState, int& ix );
	bool interpretReturn( vector<sStatement>& stmts, int& iJumpState, int& ix );

	void debugMode(sStatement& stmt);
	void setReloadStack(void);
	bool reloadCurrent(vector<sStatement>& stmts);
	bool replaceStmts(vector<sStatement>& stmts, vector<sStatement>& stmtBlock);

	bool doPreActions( vector<tokData>& tokens );
	bool doPostActions( vector<tokData>& tokens );

	bool interpret(void);
	bool interpret(int i);
	bool interpret( sStatement& stmt, int& iJumpState, bool& bDebugJumpOut, bool& bDebugJumpIn );

	void setSymbolTable(cHash* pT)		{ m_pSymbolTable = pT; }
	void setKeywordTable(std::map<string, keyData>* pT)	{ m_pKeywordTable = pT; }
	void setCommandTable(std::map<string, keyData>* pT)	{ m_pCommandTable = pT; }
	void setScriptTable(cScriptHash* pT) { m_pScriptTable = pT; }
	void setFcnTable(cFcnHash* pT) { m_pFcnTable = pT; }

	void clearReport(void);

	bool fileParts(string& path, string& file, string& ext, string& fullFile);

	int stmtCount(void) { return m_blockStack.back().stmts.size(); }
	string getResultName(void) { if (m_resultName.empty()) {return "1";} else {return m_resultName;} }
	bool getReport() {
		return m_stmt.bReport;
	}
	bool getReport(int i) {
		if (i>stmtCount()) return false;
		return m_blockStack.back().stmts[i].bReport;
	}
	void getRvalue(cContainer& c) { c = m_rvalue; }
	//void resetBlock(void);

protected:
	// interpreting (translation) support
	int findEquals( vector<tokData>& tokens );
	bool assignRvalue( vector<valueBlob>& lblobs, bool bAssignedLvalues );
	//bool findLvalueAccess( int ix, vector<tokData>& tokLeft );

	bool evalScalarCondition(vector<tokData>& tokens, bool& bResult, bool& bDebugJumpOut, bool bDebugJumpIn);
	bool evalScalar(vector<tokData>& tokens, cContainer& Result, bool& bDebugJumpOut, bool bDebugJumpIn);

	bool doLvalueAccess( cContainer& vix, tokData& tokarg );
	bool doLvalueAccess( cContainer& vix, tokData& tokarg1, tokData& tokarg2 );

	bool interpretLvalue( vector<valueBlob>& lblobs, TOKS& tokLeft, bool& bDebugJumpOut, bool& bDebugJumpIn );
	bool interpretRvalue( vector<tokData>& tokens, int& iJumpState, bool& bDebugJumpOut, bool& bDebugJumpIn );
	bool interpretRvalue( vector<valueBlob>& lblobs, bool& bAssigned, vector<tokData>& tokens, int& iJumpState, bool& bDebugJumpOut, bool& bDebugJumpIn );

	bool makeScriptCall(sBlock& block, string& name, int nLineNo, bool& bDebugJumpOut, bool bDebugJumpIn);
	bool makeFcnCall(vector<valueBlob>& lblobs, cContainer& vLoc, bool& bAssigned, sBlock& fcnBlock, vector<tokData>& tokens, int i, bool& bDebugJumpOut, bool bDebugJumpIn);
	bool makeFcnCall(cContainer& vLoc, sBlock& fcnBlock, vector<tokData>& tokens, int i, bool& bDebugJumpOut, bool bDebugJumpIn);

	bool getRvalue(cContainer& Result, vector<valueBlob>& lblobs);
	int nextFunction(vector<tokData>& stmt, int ix);
	bool getVar( cContainer& v, tokData& tok );		// token --> variable conversion
	bool getVarDimensions( int& nRows, int& nCols, int& nDepth, tokData& tok );
	//bool args2idx( cVariable& arg, cVariable& row, cVariable& col, tokData& tok );
	bool getLvalueBlobHash( valueBlob& blob, vector<tokData>& tokens, int i );
	bool getLvalueBlob( valueBlob& blob, vector<tokData>& tokens, int i );

	bool doFunctionCall(cContainer& Result, vector<tokData>& tokens, int ix);
	bool doFunctionCall(vector<tokData>& tokens, int ix, vector<valueBlob>& lblobs);

	bool doCommandCall(vector<tokData>& tokens, int ix);

	bool checkCompatability(cContainer& v1, cContainer& v2, char op);
	bool checkCompatability(cContainer& v1, cContainer& v2, cContainer& v3, char op);

	bool checkValidLvalues( vector<tokData>& leftTokens );
	bool checkValidLvalue( tokData& tok );

	//bool tok2variable( cVariable& v, tokData& tok );
	//bool tok2variableVector( vector<cVariable>& vv, vector<tokData>& tokens, int ix );

	bool tok2container( cContainer& v, tokData& tok );
	bool tok2containerVector( vector<cContainer>& vv, vector<tokData>& tokens, int ix );
	bool tok2stringVector( vector<cString>& vs, vector<tokData>& tokens, int ix );

	// fcn call helpers
	void vc2vm(vector<cMatrix>& vm, vector<cContainer>& vc);
	void vc2vs(vector<cString>& vs, vector<cContainer>& vc);
	bool call_f2(keyData& key, tokData& tok, int nargin, int ix, cContainer& c1, cContainer& c2);
	bool call_f3(keyData& key, tokData& tok, int nargin, int ix, cContainer& c1, cContainer& c2, cContainer& c3);
	bool call_f4(keyData& key, tokData& tok, int nargin, int ix, cContainer& c1, cContainer& c2, cContainer& c3, cContainer& c4);
	//
	bool call(vector<valueBlob>& lblobs, keyData& key, tokData& tok);		// nargin==0
	bool call(vector<valueBlob>& lblobs, keyData& key, tokData& tok, int nargin, cContainer& c);
	bool call(vector<valueBlob>& lblobs, keyData& key, tokData& tok, int nargin, cContainer& c1, cContainer& c2);
	bool call(vector<valueBlob>& lblobs, keyData& key, tokData& tok, int nargin, cContainer& c1, cContainer& c2, cContainer& c3);
	bool call(vector<valueBlob>& lblobs, keyData& key, tokData& tok, int nargin, cContainer& c1, cContainer& c2, cContainer& c3, cContainer& c4);
	bool call(vector<valueBlob>& lblobs, keyData& key, tokData& tok, int nargin, vector<cContainer>& vc);
	bool call_0_arg(keyData& key, tokData& tok, int nargin, int nargout);
	bool call_1_arg(keyData& key, tokData& tok, int nargin, int nargout, cContainer& c);
	bool call_2_arg(keyData& key, tokData& tok, int nargin, int nargout, cContainer& c1, cContainer& c2);
	bool call_3_arg(keyData& key, tokData& tok, int nargin, int nargout, cContainer& c1, cContainer& c2, cContainer& c3);
	bool call_4_arg(keyData& key, tokData& tok, int nargin, int nargout, cContainer& c1, cContainer& c2, cContainer& c3, cContainer& c4);
	bool call_vin_arg(keyData& key, tokData& tok, int nargin, int nargout, vector<cContainer>& vc);
	bool call_vin_0_arg(keyData& key, tokData& tok, int nargin, int nargout, vector<cContainer>& vc);
	bool call_vin_1_arg(keyData& key, tokData& tok, int nargin, int nargout, vector<cContainer>& vc);
	bool call_vin_3_arg(keyData& key, tokData& tok, int nargin, int nargout, vector<cContainer>& vc);	// 2 mandatory inputs + variable # inputs, 1 output (e.g., fprintf)
	bool call_vout_arg(keyData& key, tokData& tok, int nargin, int nargout, vector<cContainer>& vc);
	bool call_vout_2_arg(keyData& key, tokData& tok, int nargin, int nargout, vector<cContainer>& vc);	// 2 mandatory inputs, variable # outputs (e.g., fscanf)
	bool call_vout_3_arg(keyData& key, tokData& tok, int nargin, int nargout, vector<cContainer>& vc);	// 3 mandatory inputs, variable # outputs (e.g., knn_query)
	//
	bool callCmd(keyData& key, tokData& tok, int nargin, int nargout, cContainer& c);
	bool callCmd(keyData& key, tokData& tok, int nargin, int nargout, cContainer& c1, cContainer& c2);
	bool callCmd(keyData& key, tokData& tok, int nargin, int nargout, cContainer& c1, cContainer& c2, cContainer& c3);
	bool callCmd(keyData& key, tokData& tok, int nargin, int nargout, cContainer& c1, cContainer& c2, cContainer& c3, cContainer& c4);
	//bool call(keyData& key, tokData& tok, int nargin, int nargout, vector<cContainer>& vc);
	// call helpers
	int getCallIx(cContainer& c);
	int getCallIx(cContainer& c1, cContainer& c2);
	int getCallIx(cContainer& c1, cContainer& c2, cContainer& c3);
	int getCallIx(cContainer& c1, cContainer& c2, cContainer& c3, cContainer& c4);
	int getCallIx(vector<cContainer>& vc, int narg);
	int getCallIxRev(vector<cContainer>& vc, int narg);
	int getCallIxRev(vector<cContainer>& vc, int nargin, int nargout);
	bool addCallIx(int& ix, cContainer& c);
	bool addArgIx(int& ix, int nargin, int nargout);

	bool tokEnd2Ix(tokData& arg);

	bool doUnitaryOperation(cContainer& v, tokData& o1, int nOpType);
	bool tokUnitaryOp(cContainer& v, tokData& o1, const char op);
	bool tokUnitaryOp(cContainer& v, double lf, char op);
	bool tokUnitaryOp(cContainer& v, cContainer& t, char op);

	bool doBinaryOperation(cContainer& v, tokData& o1, tokData& o2, int nOpType);
	bool tokBinaryOp(cContainer& v, tokData& o1, tokData& o2, const char op);
	bool tokBinaryOp(cContainer& v, double lf, tokData& o2, char op);
	bool tokBinaryOp(cContainer& v, cContainer& t, tokData& o2, char op);

	bool tokBinaryOp(cContainer& v, string& str1, tokData& o2, char op);
	bool tokBinaryOp(cContainer& v, string& str1, cContainer& v2, char op);
	bool tokBinaryOp(cContainer& v, string& str1, string& str2, char op);


	bool doTertiaryOperation(cContainer& v, tokData& o1, tokData& o2, tokData& o3, int nOpType);
	bool tokTertiaryOp(cContainer& v, tokData& o1, tokData& o2, tokData& o3, const char op);
	bool tokTertiaryOp(cContainer& v, double lf, tokData& o2, tokData& o3, char op);
	bool tokTertiaryOp(cContainer& v, tokData& o1, double lf, tokData& o3, char op);
	bool tokTertiaryOp(cContainer& v, cContainer& t, tokData& o2, tokData& o3, char op);
	bool tokTertiaryOp(cContainer& v, tokData& o1, cContainer& t, tokData& o3, char op);
	bool tokTertiaryOp(cContainer& v, cContainer& t1, cContainer& t2, tokData& o3, char op);
	bool tokTertiaryOp(cContainer& v, cContainer& t, double lf, tokData& o3, char op);
	bool tokTertiaryOp(cContainer& v, double lf, cContainer& t, tokData& o3, char op);
	bool tokTertiaryOp(cContainer& v, double lf1, double lf2, tokData& o3, char op);

	// hash access
	bool doRvalueAccessOperationHash(cContainer& v, tokData& o /* hash */, vector<tokData>& args /* args */);
	bool doRvalueAccessOperationHashOneArg(cContainer& v, tokData& o /* hash */, tokData& arg /* arg */);
	bool doRvalueAccessOperationHash(cContainer& v, tokData& o /* hash */, tokData& arg /* key */);
	//
	bool rvalueAccessHash(cContainer& v, cContainer& var, tokData& arg);

	bool GetSize(double& lfsz, tokData& o);

	// lvalue access
	bool doLvalueAccessOperation(valueBlob& blob /* variable */, tokData& arg /* arg */);
	bool doLvalueAccessOperation(valueBlob& blob /* variable */, tokData& arg1 /* arg 1 */, tokData& arg2 /* arg 2 */);
	bool doLvalueAccessOperation(valueBlob& blob /* variable */, tokData& arg1 /* arg 1 */, tokData& arg2 /* arg 2 */, tokData& arg3 /* arg 3 */);
	

	// matrix, cube, and string access
	bool doRvalueAccessOperation(cContainer& v, tokData& o /* matrix or vect */, tokData& arg /* arg */);
	bool doRvalueAccessOperation(cContainer& v, tokData& o /* matrix or vect */, tokData& arg1 /* arg 1 */, tokData& arg2 /* arg 2 */);
	bool doRvalueAccessOperation(cContainer& v, tokData& o /* matrix or vect */, tokData& arg1 /* arg 1 */, tokData& arg2 /* arg 2 */, tokData& arg3 /* arg 3 */);
	// 1-arg
	bool rvalueAccess(cContainer& v, double dvar, tokData& arg);
	bool rvalueAccess(cContainer& v, cContainer& var, tokData& arg);
	bool rvalueAccess(cContainer& v, string& name, tokData& arg);
	// 2-arg
	bool rvalueAccess(cContainer& v, double dvar, tokData& arg1, tokData& arg2);
	bool rvalueAccess(cContainer& v, double dvar, cContainer& arg1, tokData& arg2);
	bool rvalueAccess(cContainer& v, double dvar, double darg1, tokData& arg2);
	bool rvalueAccess(cContainer& v, double dvar, tokData& arg1, double darg2);
	bool rvalueAccess(cContainer& v, double dvar, double darg1, double darg2);
	//bool rvalueAccess(cContainer& v, cContainer& var, tokData& arg1, tokData& arg2);
	//bool rvalueAccess(cContainer& v, cContainer& var, cContainer& arg1, tokData& arg2);
	//bool rvalueAccess(cContainer& v, cContainer& var, double darg1, tokData& arg2);
	//bool rvalueAccess(cContainer& v, cContainer& var, tokData& arg1, double darg2);
	//bool rvalueAccess(cContainer& v, cContainer& var, double darg1, double darg2);
	bool rvalueAccess(cContainer& v, string& name, tokData& arg1, tokData& arg2);
	bool rvalueAccess(cContainer& v, string& name, cContainer& arg1, tokData& arg2);
	bool rvalueAccess(cContainer& v, string& name, double darg1, tokData& arg2);
	bool rvalueAccess(cContainer& v, string& name, string& name1, tokData& arg2);

	//
	bool rvalueAccessColon(cContainer& v, double dvar, bool b1, tokData& arg2);
	bool rvalueAccessColon(cContainer& v, cContainer& var, bool b1, tokData& arg2);
	bool rvalueAccessColon(cContainer& v, string& name, bool b1, tokData& arg2);
	// 3-arg
	bool rvalueAccess(cContainer& v, double dvar, tokData& arg1, tokData& arg2, tokData& arg3);
	bool rvalueAccess(cContainer& v, string& name, tokData& arg1, tokData& arg2, tokData& arg3);
	bool rvalueAccess(cContainer& v, string& name, string& name1, tokData& arg2, tokData& arg3);
	bool rvalueAccess(cContainer& v, string& name, string& name1, double darg2, tokData& arg3);
	bool rvalueAccess(cContainer& v, string& name, string& name1, string& name2, tokData& arg3);
	//bool rvalueAccess(cContainer& v, cContainer& var, tokData& arg1, tokData& arg2, tokData& arg3);
	//
	bool rvalueAccess(cContainer& v, double dvar, double darg1, tokData& arg2, tokData& arg3);
	bool rvalueAccess(cContainer& v, double dvar, cContainer& arg1, tokData& arg2, tokData& arg3);
	bool rvalueAccess(cContainer& v, string& name, double darg1, tokData& arg2, tokData& arg3);
	bool rvalueAccess(cContainer& v, string& name, cContainer& arg1, tokData& arg2, tokData& arg3);
	//bool rvalueAccess(cContainer& v, cContainer& var, double darg1, tokData& arg2, tokData& arg3);
	//bool rvalueAccess(cContainer& v, cContainer& var, cContainer& arg1, tokData& arg2, tokData& arg3);
	//
	bool rvalueAccess(cContainer& v, double dvar, double darg1, double darg2, tokData& arg3);
	bool rvalueAccess(cContainer& v, double dvar, double darg1, cContainer& arg2, tokData& arg3);
	bool rvalueAccess(cContainer& v, double dvar, cContainer& arg1, double darg2, tokData& arg3);
	bool rvalueAccess(cContainer& v, double dvar, cContainer& arg1, cContainer& arg2, tokData& arg3);
	bool rvalueAccess(cContainer& v, string& name, double darg1, double darg2, tokData& arg3);
	bool rvalueAccess(cContainer& v, string& name, double darg1, cContainer& arg2, tokData& arg3);
	bool rvalueAccess(cContainer& v, string& name, cContainer& arg1, double darg2, tokData& arg3);
	bool rvalueAccess(cContainer& v, string& name, cContainer& arg1, cContainer& arg2, tokData& arg3);
	//bool rvalueAccess(cContainer& v, cContainer& var, double darg1, double darg2, tokData& arg3);
	//bool rvalueAccess(cContainer& v, cContainer& var, double darg1, cContainer& arg2, tokData& arg3);
	//bool rvalueAccess(cContainer& v, cContainer& var, cContainer& arg1, double darg2, tokData& arg3);
	//bool rvalueAccess(cContainer& v, cContainer& var, cContainer& arg1, cContainer& arg2, tokData& arg3);
	//
	bool rvalueAccessColon(cContainer& v, double dvar, bool b1, tokData& arg2, tokData& arg3);
	bool rvalueAccessColon(cContainer& v, cContainer& var, bool b1, tokData& arg2, tokData& arg3);

	bool rvalueAccessColon(cContainer& v, double dvar, bool b1, double darg2, tokData& arg3);
	bool rvalueAccessColon(cContainer& v, double dvar, bool b1, cContainer& arg2, tokData& arg3);
	bool rvalueAccessColon(cContainer& v, double dvar, bool b1, bool b2, tokData& arg3);
	bool rvalueAccessColon(cContainer& v, cContainer& var, bool b1, double darg2, tokData& arg3);
	bool rvalueAccessColon(cContainer& v, cContainer& var, bool b1, cContainer& arg2, tokData& arg3);
	bool rvalueAccessColon(cContainer& v, cContainer& var, bool b1, bool b2, tokData& arg3);

	bool rvalueAccessColon(cContainer& v, double dvar, cContainer& arg1, bool b2, tokData& arg3);
	bool rvalueAccessColon(cContainer& v, double dvar, double darg1, bool b2, tokData& arg3);
	bool rvalueAccessColon(cContainer& v, cContainer& var, cContainer& arg1, bool b2, tokData& arg3);
	bool rvalueAccessColon(cContainer& v, cContainer& var, double darg1, bool b2, tokData& arg3);

	//// access functions (1 arg)
	//bool varAccess(cContainer& c, string& name, double darg);
	//bool varAccess(cContainer& c, string& name, string& argName);
	//bool varAccess(cContainer& c, string& name, cContainer& arg);

	void setResultName( string& s ) { m_resultName = s; }

public:
	vector<string> m_outMsg;		// error state

	vector<sStackEntry> m_stack;

	bool m_bLvalueInterp;
	string m_resultName;
	cContainer m_vTmp;
	cContainer m_rvalue, m_lvalue;
	vector<cContainer> m_args;	// for fcn argument handling in translation
	vector<cContainer> m_tempVars;	// for evaluating expressions

protected:
	cHash * m_pSymbolTable;			// symbol table hash (variable space)
	std::map<string, keyData> * m_pKeywordTable;		// hash of all keywords (built-in native functions)
	std::map<string, keyData> * m_pCommandTable;		// hash of all commands
	cScriptHash * m_pScriptTable;	// hash of all processed scripts
	cFcnHash *m_pFcnTable;			// hash of all user-written functions

	vector<interpState> m_interpState;

	// debug support
	int m_ixStop;
	bool m_bJumpOut;
	bool m_bTriggerStop;

	string m_RunDir;

	string m_curFile;
	string m_curPath;
	string m_curFcn;

	// default paths
	vector<string> m_path;
	vector<string> m_DefaultAutoPaths;

	string m_lvalueName;

	keyData m_kd;
	sStatement m_stmt;

	vector<sBlock> m_blockStack;

	bool m_bReturn;
	bool m_bNoRvalue;
};
