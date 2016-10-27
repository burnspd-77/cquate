
#pragma once

#include <stdio.h>
#include <ctype.h>		// char test fcns
#include <string.h>
#include <stdlib.h>
#include <string>
#include "lexTypes.h"
#include "container.h"
#include "map_hash.h"
#include "scriptHash.h"
#include "fcnHash.h"
#include "fb.h"
#include "defines.h"
#include "structs.h"
#ifdef _WINDOWS_
#include <direct.h>
#else
#include <unistd.h>
#endif

using std::string;

#define BSIZE		1024
#define NONE		-1
#define EOS			'\0'

enum { NUM = 256, STRING, ID, FCN, CMD, LANGUAGE, SCRIPT_CALL, SCRIPT_FCN_CALL, U_OP, L_VALUE, PLUS_PLUS, MINUS_MINUS, STRUCT_FIELD, DONE };

//#define NUM		256
//#define ID		257
//#define LVALUE  258
//#define DONE	259

#define LEX_ERROR	300

#define _LEX_DEBUG		// uncomment to debug parser

typedef struct tagLexState {
	int lineno;
	string lexbuf;
	const char *pchCurrent, *pchLast;
	bool bForStmt;
	vector<string> outMsg;
} lexState;


typedef struct tagArgSet {
	string name;
	vector<int> ix_set;
} sArgSet;


class cLex
{
public:
	cLex()
	{
		m_tokenval = 1;
		m_lineno = 0;
		pchCurrent = pchLast = m_bufBegin = m_bufEnd = NULL;

		m_bVerbose = false;

		m_bEquals = false;
		m_nArgsOut = 1;

		m_bPreParsing = false;			// parsing a script/fcn for a table entry
		m_bParsingSwitch = false;

		m_bBranchStmt = false;

		m_bInteractive = false;

		// initialize path
		char *pPath = getenv("CQPATH");
		char *pch;
		string str;
		if (pPath != NULL) {
			printf("CQPATH=%s\n", pPath);
			pch = strtok(pPath, ";");
			while (pch != NULL) {
				str = pch;
				m_DefaultAutoPaths.push_back(str);
				pch = strtok(NULL, ";");
			}
		}
	}
	~cLex() { };

public:
	// path and file mgmt
	void SetRunDir(string& dir) {
		m_RunDir = dir;
		setPath();
	}

	//
	bool setPath(vector<string>& userPath);
	bool setPath(void);
	vector<string> getPath(void) { return m_path; }
	bool addPath(string& dir);
	bool delPath(string& dir);
	bool getFileList(void);
	bool getScriptFilesInDir(vector<string>& files, string& dir);
	bool insertFiles(vector<string>& newFiles, string& dir);
	bool UpdatePath(void);
	string ConvertToLower(string& strInput);
	bool IsValidPath(string& line);
	bool getMissingFiles(vector<string>& missingFiles, vector<string>& oldList, vector<string>& curList);
	bool fileParts(string& path, string& file, string& ext, string& fullFile);

	void setBuffer(char *begin, int len);
	void setBuffer(string& s);

	void setLineNo(int nline) { m_lineno = nline; }

	void setInteractive(bool bInt) { m_bInteractive = bInt; }

	void setSymbolTable(cHash* pT)		{ m_pSymbolTable = pT; }
	void setKeywordTable(std::map<string, keyData>* pT)	{ m_pKeywordTable = pT; }
	void setCommandTable(std::map<string, keyData>* pT)	{ m_pCommandTable = pT; }
	void setScriptTable(cScriptHash* pT) { m_pScriptTable = pT; }
	void setFcnTable(cFcnHash* pT) { m_pFcnTable = pT; }

	// file mgmt support
	bool fullScript2block(sBlock& b, string& fullFile);
	bool updateScript(string& fullFile);
	bool removeScripts( vector<string>& files );

	bool initializeFiles(void);
	bool updateAllScripts(void);
	bool updateAllScripts(vector<string>& newFiles);
	bool checkTime(bool& bNewer, string& fullFile);
	bool addToScriptTable(vector<string> *pScriptFiles);
	bool addFile(string& fullFile);			// add file to script hash, and invoke parsing of file's code
	bool isInPath(string& dir);				// is this dir already in the path?
	bool updateFunctions(string& fname, bool bParse);

	bool parseFileCode();		// parse all file code, and store in pBlock
	bool parseScript( vector<sStatement>& stmts );
	bool parseBlock(vector<sStatement>& s, bool bForceBrace);
	bool parseBlock(sBlock *pBlock);		// parse a standard block of code, usually delimited by braces { }
	bool parseCaseBlock(vector<sStatement>& stmts);
	bool parseDefaultBlock(vector<sStatement>& stmts);
	bool appendBlock( sBlock& blockMain, sBlock& more );

	bool jumpToNextDeclare(void);

	bool parseFunction(sBlock& fcnBlock, bool bGetCode);
	bool parseIf(vector<sStatement>& ifStmts, bool& bFoundElse);
	bool parseElse(vector<sStatement>& elseStmts, int ixif, bool& bIsElse);
	bool parseWhile(vector<sStatement>& stmts);
	bool parseDo(vector<sStatement>& stmts);
	bool parseFor(vector<sStatement>& stmts);
	bool parseSwitch(vector<sStatement>& stmts);
	bool parseCase(vector<sStatement>& stmts);
	bool parseDefault(vector<sStatement>& stmts);
	bool parseBreak(vector<sStatement>& stmts);
	bool parseReturn(vector<sStatement>& stmts);
	bool parseContinue(vector<sStatement>& stmts);

	bool parseAccessor(void);
	bool parseHash(void);
	bool parseMatrix(void);
	bool matrow(void);
	bool parseStmts(void);
	bool parseStmts( vector<sStatement>& stmts );


	// parsing support
	void checkAssignment(void);
	//void checkNextAssignment(void);
	int nextTok(void);
	int nextTok(bool bSilent);
	int nextFunction(void);
	bool nextCmdArg(string& arg);
	int checkKeySymbol(bool bSilent);
	int checkFunction(bool bSilent);
	int addFcnPointerToken(bool bSilent);
	int addSymbolToken(bool bSilent);
	void addOperator( int nType );

	bool stmtNoCheck(void);
	bool stmt(void);
	bool expr(void);
	bool post_incr(void);
	bool logical(void);
	bool relational(void);
	bool sum(void);
	bool unitarySign(void);
	bool term(void);
	bool factor(void);
	bool dereference(void);
	bool pre_incr(void);
	bool pow(void);
	bool lvalue_clist(int& nCount);
	bool decl_args(int& nargs);
	bool args(int&);		// arg list support
	bool mat_args(int&);
	bool hash_args(int&);		// should only be 1 arg!
	bool cmdArgs(int& nargs);
	bool decl_clist(int& nCount);
	bool derefs(int& nCount);
	bool clist(int&);
	bool alist(int&);
	bool symbol(void);
	bool match(int itok);
	bool quiet_match(int itok);

	bool parseNativeStmt(vector<sStatement>& stmts);

	int peekTok(void);
	void pushBuffer(sBlock& b);
	void popBuffer();

	bool checkDone(void);
	int nextchar(void);
	int curchar(void);
	int peekchar(void);
	int goback(int n);
	void undoLast(void);

	// state mgmt
	void resetStmt(void);
	void reset(void);

public:
	sBlock getOutput() {
		return m_block;
	}
	void pushState(void);
	void popState(void);

	// debug and error reporting
	void emit(int, int);
	void emit(int t, double tval);
	void emit( sBlock& block );
	void emit( sStatement& stmt );
	void emit( tokData& tok );
	void report(const char *pch);
	void report(string& msg);
	void report(const char *pch, const char *pchSymbol);
	void report(const char *pch, string& symbol);
	void report(string msg, string& symbol);
	void clearReport();

	// get internals
	std::map<string, keyData> * getKeywordTable(void) { return m_pKeywordTable; }
	cFcnHash * getFcnTable(void) { return m_pFcnTable; }

public:
	vector<string> m_outMsg;		// error state
	vector<tokData> m_tokBuf, m_tokPre, m_tokPost;
	vector<TOKS> m_lvalues;

protected:
	cHash * m_pSymbolTable;			// symbol table hash (variable space)
	std::map<string, keyData> * m_pKeywordTable;		// hash of all keywords (built-in native functions)
	std::map<string, keyData> * m_pCommandTable;		// hash of all commands
	cScriptHash * m_pScriptTable;	// hash of all processed scripts
	cFcnHash *m_pFcnTable;			// hash of all user-written functions
	//vector<string> *m_pPath;		// set of directories to search for scripts, in ascending order of precedence
	//								//  (first entry has lowest precedence, last entry highest)

	string m_curFile;

	string m_RunDir;

	bool m_bVerbose;

	tokData m_tok;				// temp storage
	keyData m_kd;
	sStatement m_stmt;
	sBlock m_block;

	vector<string> m_fileList;

	// default paths
	vector<string> m_path;
	vector<string> m_DefaultAutoPaths;

	char lexbuf[BSIZE];				// token name buffer
	string m_peekbuf;

	char m_pchOut[100];				// sprintf output buffer

	// parse support
	double m_tokenval;
	int m_lineno;
	int lookAhead;
	int lastLook;
	bool m_bEquals;				// have we hit the (any?) equals sign in this stmt yet?
	bool m_bAssignment;			// is this stmt an assignment?
	bool m_bBranchStmt;			// are we parsing a branch statement (e.g. for (;;) )?
	bool m_bParsingSwitch;
	int m_nArgsOut;
	const char *m_bufBegin, *m_bufEnd, *pchCurrent, *pchLast;
	vector<sBuffer> m_bufStack;
	vector<lexState> m_lexState;

	vector<sArgSet> m_argStack;
	int m_iarg;

	bool m_bInteractive;		// interactive mode? - that is, code from cmd window?

public:
	bool m_bPreParsing;			// pre-parsing mode? (for scripts)

};
