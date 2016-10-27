
#include "lex.h"
#include <string.h>

void cLex::setBuffer(char *begin, int len)
{
	m_bufBegin = pchCurrent = begin;
	m_bufEnd = m_bufBegin + len;
}

void cLex::setBuffer(string& s)
{
	m_bufBegin = pchCurrent = s.c_str();
	m_bufEnd = m_bufBegin + s.length();
}

void cLex::pushBuffer(sBlock& b)
{
	sBuffer buf;
	buf.pBegin = m_bufBegin;
	buf.pEnd = m_bufEnd;

	m_curFile = b.fileName;

	m_bufStack.push_back( buf );	// buffer stack
	
	m_block = b;
}

void cLex::popBuffer()
{
	sBuffer buf = m_bufStack[m_bufStack.size()-1];
	m_bufBegin = pchCurrent = buf.pBegin;
	m_bufEnd = buf.pEnd;

	m_bufStack.pop_back();

	if (m_bufStack.empty())
		m_curFile.clear();

	// clear block
	m_block.stmts.clear();
	m_block.nType = LEX_BLOCK_NULL;
	m_block.nLineOffset = 0;
	m_block.fileName = "";
	m_block.pathName = "";
}


bool cLex::checkDone()
{
	return pchCurrent>=m_bufEnd;
}

int cLex::nextchar()	// advance pchCurrent, get next char
{
	if ( pchCurrent < m_bufEnd )
	{
		pchLast = pchCurrent;
		return *(pchCurrent++);
	}

	return EOF;
}

int cLex::curchar()
{	// get a copy of current char
	if (pchCurrent>m_bufBegin)
		return *(pchCurrent-1);
	return 0;
}

int cLex::peekchar()	// get next char without moving pchCurrent
{
	if ( pchCurrent < m_bufEnd )
	{
		return *pchCurrent;
	}

	return EOF;
}

void cLex::undoLast()
{
	pchCurrent = pchLast;
}

int cLex::goback(int n)
{
	pchCurrent -= n;
	return *pchCurrent;
}

void cLex::addOperator( int nType )
{
	m_tok.nSymbolType = LEX_TYPE_OPERATOR;
	m_tok.nOperatorType = nType;
	m_tok.nLineNo = m_lineno;
	m_tokBuf.push_back(m_tok);
}

bool cLex::fileParts(string& path, string& file, string& ext, string& fullFile)
{
	size_t ixDot = fullFile.find_last_of('.', string::npos);
	if (ixDot==string::npos)
		ext = "";
	else
		ext = fullFile.substr(ixDot,string::npos);

	size_t ixSlash = fullFile.find_last_of('\\', string::npos);
	if (ixSlash==string::npos)
		path = "";							// no path?
	else
		path = fullFile.substr(0,ixSlash);	// path does not include trailing slash

	if (ixSlash != string::npos) {			// slash found in fullFile?
		if (ixDot != string::npos) {		// dot found in fullFile?
			if ( ixSlash>=(ixDot-1) )		// slash comes after dot?
				return true;					// illegal filename given
			file = fullFile.substr(ixSlash+1, ixDot-ixSlash-1);
		}
		else {								// no dot found?
			file = fullFile.substr(ixSlash+1);
		}
	}
	else {							// no slash found
		if (ixDot != string::npos) {		// dot but no slash
			file = fullFile.substr(0, ixDot);
		}
		else {								// no dot or slash
			file = fullFile.substr(ixSlash+1);
		}
	}

	return false;
}

// lexical analyzer
//

int cLex::peekTok()
{
	pushState();
	int iret = nextTok(true);
	m_peekbuf = lexbuf;
	popState();
	return iret;
}

void cLex::pushState()
{
	lexState s;
	s.lineno = m_lineno;
	s.pchCurrent = pchCurrent;
	s.pchLast = pchLast;
	s.lexbuf = lexbuf;
	s.outMsg = m_outMsg;
	s.bForStmt = m_bBranchStmt;

	m_lexState.push_back(s);
}

void cLex::popState()
{
	m_lineno = m_lexState.back().lineno;
	pchCurrent = m_lexState.back().pchCurrent;
	pchLast = m_lexState.back().pchLast;
	strcpy( lexbuf, m_lexState.back().lexbuf.c_str() );
	m_outMsg = m_lexState.back().outMsg;
	m_bBranchStmt = m_lexState.back().bForStmt;

	m_lexState.pop_back();
}

void cLex::resetStmt()
{
	m_bEquals = false;
	m_bAssignment = false;
	m_nArgsOut = 1;
	m_tokBuf.clear();
	m_tokPre.clear();
	m_tokPost.clear();
}

void cLex::reset()
{
	m_bEquals = false;
	resetStmt();
	m_bBranchStmt = false;
	m_tokBuf.clear();
	m_tokPre.clear();
	m_tokPost.clear();
	m_lvalues.clear();

	m_argStack.clear();

	clearReport();

	m_block.stmts.clear();
	m_block.nType = LEX_BLOCK_NULL;
	m_block.nLineOffset = 0;
	m_block.fileName = "";
	m_block.pathName = "";

	m_bufStack.clear();
	m_lexState.clear();

	m_bInteractive = true;
	m_bPreParsing = false;
}

bool cLex::jumpToNextDeclare()
{
	int pk;
	while (lookAhead != DONE)
	{
		pk = peekTok();
		if (pk==LANGUAGE) {
			if (m_peekbuf.compare("function")==0) { return false; }	// next tok is declare
		}
		lookAhead = nextTok();
	}

	return true;	// declare not found!
}

bool cLex::parseBlock(sBlock *pBlock)	{ return parseBlock(pBlock->stmts,false); }

bool cLex::parseBlock(vector<sStatement>& stmts, bool bForceBrace)
{	// get block of stmts proceeding from a branch or loop stmt (i.e., may be enclosed in braces { })

	// clear all stmts
	m_stmt.tokens.clear();
	m_stmt.bStop = false;

	if (lookAhead=='{')
	{
		lookAhead = nextTok();

		while (lookAhead != '}' && lookAhead != DONE)
		{
			switch (lookAhead) {
			case LANGUAGE:
				if (parseNativeStmt(stmts))
					return true;
				break;

			default:
				if (stmt())
					return true;
				m_stmt.nLineNumer = m_lineno;
				stmts.push_back( m_stmt );
				resetStmt(); m_stmt.tokens.clear();

				lookAhead = nextTok();
			};

		};
		if (lookAhead != '}') { report("syntax error: expected '}'"); return true; }
		else {
			match('}');
		}
	}
	else if (bForceBrace) { report("syntax error: expected '{'"); return true; }
	else {
		// just get next stmt
		switch (lookAhead) {
		case LANGUAGE:
			if (parseNativeStmt(stmts))
				return true;
			break;

		default:
			if (stmt())
				return true;
			stmts.push_back( m_stmt );
			resetStmt(); m_stmt.tokens.clear();

			lookAhead = nextTok();
		};

	}

	return false;
}

bool cLex::parseCaseBlock(vector<sStatement>& stmts)
{
	bool bDone = false;

	// get the block of stmts
	while (!bDone && lookAhead != '}' && lookAhead != DONE)
	{
		switch (lookAhead) {
		case LANGUAGE:
			if (strcmp(lexbuf,"case")==0) { bDone = true; break; }
			if (strcmp(lexbuf,"default")==0) { bDone = true; break; }
			if (parseNativeStmt(stmts)) return true;
			break;

		default:
			if (stmt()) return true;
			m_stmt.nLineNumer = m_lineno;
			stmts.push_back( m_stmt );
			resetStmt(); m_stmt.tokens.clear();

			lookAhead = nextTok();
		};

		if (bDone) break;
	};

	return false;
}

bool cLex::parseDefaultBlock(vector<sStatement>& stmts)
{	// for parsing 'default' case inside switch stmt

	// get the block of stmts
	while (lookAhead != '}' && lookAhead != DONE)
	{
		switch (lookAhead) {
		case LANGUAGE:
			if (parseNativeStmt(stmts)) return true;
			break;

		default:
			if (stmt()) return true;
			stmts.push_back( m_stmt );
			resetStmt(); m_stmt.tokens.clear();
		};

		lookAhead = nextTok();
	};

	return false;
}

bool cLex::appendBlock( sBlock& blockMain, sBlock& more )
{
	blockMain.stmts.insert( blockMain.stmts.end(), more.stmts.begin(), more.stmts.end() );
	return false;
}

bool cLex::parseScript( vector<sStatement>& stmts )
{
	m_stmt.tokens.clear();
	m_tokBuf.clear();

	int lineNo;

	lookAhead = nextTok();
	lineNo = m_lineno;

	while (lookAhead != DONE)
	{
		switch (lookAhead) {
		case LANGUAGE:
			if (parseNativeStmt(stmts)) return true;
			break;

		default:
			if (stmt()) return true;
			m_stmt.nLineNumer = lineNo;
			stmts.push_back( m_stmt );
			resetStmt(); m_stmt.tokens.clear();

			lookAhead = nextTok();
		};

		lineNo = m_lineno;
	};

	return false;
}

bool cLex::parseStmts( vector<sStatement>& stmts )
{
	m_block.bError = false;
	m_block.fileName.clear();
	m_block.pathName.clear();
	m_block.inArgs.clear();
	m_block.outArgs.clear();
	m_block.name.clear();
	m_block.outMsg.clear();
	m_block.nLineOffset = 0;
	m_block.stmts.clear();

	// this fcn is custom designed to parse a single command line input
	resetStmt();
	checkAssignment();

	m_stmt.tokens.clear();

	int lineNo;

	lookAhead = nextTok();
	lineNo = m_lineno;

	while (lookAhead != DONE)
	{
		switch (lookAhead) {
		case LANGUAGE:
			if (parseNativeStmt(stmts)) return true;
			break;

		default:
			if (stmtNoCheck()) return true;
			m_stmt.nLineNumer = lineNo;
			stmts.push_back( m_stmt );
			resetStmt(); m_stmt.tokens.clear();

			checkAssignment();
			lookAhead = nextTok();
		};

		lineNo = m_lineno;
	};

	return false;
}

bool cLex::parseStmts()
{
	// this fcn is custom designed to parse a single command line input
	resetStmt();
	checkAssignment();		// set m_bAssignment

	lookAhead = nextTok();
	if (lookAhead == DONE && !m_outMsg.empty()) {	// assume an error occured (e.g. undefined var)
		return true;
	}

	while (lookAhead != DONE)
	{
		if (stmt()) { return true; }
	}

#ifdef _LEX_DEBUG
	//emit( m_block );
#endif

	return false;
}

void cLex::checkAssignment()
{	// for single-statement buffers only! (cmd line)
	m_bAssignment = false;
	int ioff(0);
	if (pchCurrent==m_bufBegin) ioff = 1;
	for (const char *ch=pchCurrent+ioff; ch<m_bufEnd-1; ++ch) {
		if (*(ch-1)!='=' && *ch=='=' && *(ch+1)!='=') { m_bAssignment = true; return; }
		if (*ch==';' || *ch=='{' || *ch=='}') return;
	}
}

int cLex::nextFunction()
{
	int b(0);
	char t('0');

	while (t != DONE && (pchCurrent < m_bufEnd) )
	{
		t = nextTok(true);

		if ( strcmp(lexbuf,"function")==0 ) {
			return LANGUAGE;
		}
	}
	return DONE;
}

int cLex::nextTok()	{ return nextTok(false); }

int cLex::nextTok(bool bSilent)
{
	int b;
	char t, tn, q;
	bool bDot = false;
	bool bExp = false;
	const char * pch;
	string sTmp;

	int flag;

	keyData kd = m_kd;

	// save last lookAhead symbol
	if (!bSilent) lastLook = lookAhead;

	while (true)
	{
		t = nextchar();
		tn = peekchar();

		if (t==' ' || t=='\t' || t=='\r')		// white space
			;	// strip white space

		// end of line
		else if (t=='\n')
			++m_lineno;

		else if (t=='/')
		{
			t = peekchar();
			if (t=='/')				// double-slash comment
			{
				t = nextchar();
				while (t != '\n' && t != EOF)
					t = nextchar();
				if (t==EOF) { return DONE; }
				else ++m_lineno;	// newline
			}
			else if (t=='*')					// /*  comment  */
			{
				t = nextchar();
EndComment:		while (t != '*' && t != EOF) { if (t=='\n') ++m_lineno;  t = nextchar(); }
				if (t=='*') {
					t = nextchar();
					if (t != '/') goto EndComment;
					else if (t==EOF) { m_tokenval = NONE; return t; }
					// else, continue looking for next token...
				}
				else { return DONE; }	// EOF reached
			}
			else {  m_tokenval = NONE; return curchar(); }
		}

		else if (t=='"')			// string token
		{
			pch = pchCurrent;
			t = nextchar();
			m_tok.name.clear();
			while (t != '"' && t!=EOF) {
				if (t=='\\') {
					q = nextchar();
					switch (q) {
					case '"':		// double-quote
						m_tok.name.append("\"");
						break;
					case 't':		// tab
						m_tok.name.append("\t");
						break;
					case 'r':		// Windows return char
						m_tok.name.append("\r");
						break;
					case 'n':		// newline
						m_tok.name.append("\n");
						break;
					case '\\':		// backslash
						m_tok.name.append("\\");
						break;
					default:
						m_tok.name.append(1,t);
						m_tok.name.append(1,q);
					}

				}	// if escape character
				else {
					m_tok.name.append(1,t);
				}

				t = nextchar();
				if (t==EOF) { report("imbalanced quotation"); return true; }
			}
			m_tok.nLineNo = m_lineno;
			return STRING;
		}

		else if (t=='*')
		{
			// element-wise mult?
			if (peekchar()=='*') {
				nextchar();
				return LEX_OPERATOR_ELEMENT_MULT;
			}
			return t;
		}

		else if (t=='+' && peekchar()=='+')
		{
			nextchar();
			return PLUS_PLUS;
		}

		else if (t=='-' && peekchar()=='-')
		{
			nextchar();
			return MINUS_MINUS;
		}

		else if (t=='.' && peekchar()=='.')			// range operator
		{
			nextchar();
			lexbuf[0] = '.'; lexbuf[1] = '.'; lexbuf[2] = EOS;
			return LEX_OPERATOR_RANGE;
		}

		else if (t=='<' && peekchar()=='=')
		{
			nextchar();
			return LEX_OPERATOR_RELATIONAL_LE;
		}

		else if (t=='>' && peekchar()=='=')
		{
			nextchar();
			return LEX_OPERATOR_RELATIONAL_GE;
		}

		else if (t=='=' && peekchar()=='=')
		{
			nextchar();
			return LEX_OPERATOR_RELATIONAL_EQ;
		}

		else if (t=='!')
		{
			if (peekchar()=='=') {
				nextchar();
				return LEX_OPERATOR_RELATIONAL_NEQ;
			}
			else
				return t;
		}

		// beginning of a scalar
		else if ( (isdigit(t) || t=='.') && !isalpha(tn) )			// number
		{
			// scalar parsing
			b = 0;
			if (t=='.') {
				bDot = true; lexbuf[b] = t; ++b;
			}

			if (bDot)	// token begins with '.'
			{
				t = nextchar();
				while (isdigit(t) || ((t=='e'||t=='E') && !bExp))
				{
					lexbuf[b] = t;
					++b;
					if (t=='e'||t=='E') bExp = true;
					t = nextchar();
					if (t==EOF)
						break;
				}
			}
			else		// token begins with digit
			{
				while (isdigit(t) || ((t=='e'||t=='E') && !bExp))
				{
					lexbuf[b] = t;
					++b;
					if (t=='e'||t=='E') bExp = true;
					t = nextchar();
					if (t==EOF)
						break;
				}

				if (t=='.' && peekchar()!='.')
				{
					lexbuf[b] = t;
					++b;
					t = nextchar();
					while (isdigit(t))
					{
						lexbuf[b] = t;
						++b;
						t = nextchar();
					}
				}
			}
			lexbuf[b] = EOS;
			if (t!=EOF)
				goback(1);

			if (!bSilent)
			{
				m_tokenval = atof( lexbuf );

				m_tok.fSymbolVal = m_tokenval;
			}

			return NUM;
		}

		// beginning of a scalar (e.g., 1e3)
		else if ( (isdigit(t) ) && (tn=='e' || tn=='E') )			// 1e3 number
		{
			b = 0;
			while (isdigit(t) || ((t=='e'||t=='E') && !bExp))
			{
				lexbuf[b] = t;
				++b;
				if (t=='e'||t=='E') bExp = true;
				t = nextchar();
				if (t==EOF)
					break;
			}

			if (t=='.' && peekchar()!='.')
			{
				lexbuf[b] = t;
				++b;
				t = nextchar();
				while (isdigit(t))
				{
					lexbuf[b] = t;
					++b;
					t = nextchar();
				}
			}

			lexbuf[b] = EOS;
			if (t!=EOF)
				goback(1);

			if (!bSilent)
			{
				m_tokenval = atof( lexbuf );

				m_tok.fSymbolVal = m_tokenval;
			}

			return NUM;
		}

		// struct field dereferencing
		else if (t=='.' && (isalpha(peekchar()) || peekchar()=='_'))
		{
			b = 0;
			t = nextchar();		// go to first char of field name
			while (isalnum(t) || t=='_')
			{
				lexbuf[b] = t;
				t = nextchar();
				++b;
				if (b>=BSIZE)	{ report("lex error"); return DONE; }
			}
			lexbuf[b] = EOS;
			if (t!=EOF)
				goback(1);		//ungetc(t, stdin);

			return STRUCT_FIELD;
		}

		// beginning of a symbol
		else if (isalpha(t) || t=='_')				// letter
		{
			b = 0;
			while (isalnum(t) || t=='_')
			{
				lexbuf[b] = t;
				t = nextchar();
				++b;
				if (b>=BSIZE)	{ report("lex error"); return DONE; }
			}
			lexbuf[b] = EOS;
			if (t!=EOF)
				goback(1);		//ungetc(t, stdin);

			if (strcmp("return",lexbuf)==0)
				flag = 1;

			return addSymbolToken(bSilent);

		}

		else if (t=='?')
		{
			// is it a command?
			sTmp = "?";
			if (m_pCommandTable->find( sTmp ) != m_pCommandTable->end())
			{
				// parse command
				m_tok.nSymbolType = LEX_TYPE_KEYWORD_COMMAND;		// m_kd contains keyword type (FUNCTION, LANGUAGE, etc)
				m_tok.name = "?";
				return CMD;
			}
		}

		// end of input
		else if (t==EOF)
			return DONE;

		// other (e.g., '=', '+', '*', '(', '{', ';', etc)
		else
		{
			m_tokenval = NONE;
			return t;
		}	// if t==<>
	}	// while
}

bool cLex::nextCmdArg(string& arg)
{
	arg.clear();
	bool bTok(false);
	int t, b;

	// save last lookAhead symbol
	lastLook = lookAhead;

	while (true)
	{
		t = nextchar();

		if (t==' ' || t=='\t' || t=='\r')		// white space
			;	// strip white space

		else if (t=='\n')
			++m_lineno;

		// end of input
		else if (t==EOF) {
			lookAhead = DONE;
			return true;
		}

		else {		// first char of string
			b = 0;
			while (t!=' ' && t!='\t' && t!='\r' && t!='\n' && t!=';' && t!=EOF) {
				bTok = true;
				lexbuf[b] = t;
				++b;
				t = nextchar();
			}
			lexbuf[b] = '\0';
		}

		if (bTok) {
			arg = lexbuf;
			break;
		}
	}

	if (t==';') {
		lookAhead = DONE;
		return true;		// this is last arg
	}

	if (t==EOF) lookAhead = DONE;
	return t==EOF;
}


int cLex::checkKeySymbol(bool bSilent)
{
	keyData kd = m_kd;
	bool bKeyword(false);
	map<string,keyData>::iterator it = m_pKeywordTable->find( lexbuf );

	if (m_pKeywordTable!=NULL && it != m_pKeywordTable->end() )
	{
		// we have a keyword
		if (it->second.nType == LEX_TYPE_KEYWORD_LANGUAGE) {
			m_tok.nSymbolType = it->second.nType;		// m_kd contains keyword type (LANGUAGE)
			m_tok.name = lexbuf;
			return LANGUAGE;
		}
		else if (it->second.nType == LEX_TYPE_KEYWORD_FUNCTION) {
			// parse fcn call
			m_tok.nSymbolType = it->second.nType;		// m_kd contains keyword type (FUNCTION, LANGUAGE, etc)
			m_tok.name = lexbuf;
			return FCN;
		}
		else if (kd.nType == LEX_TYPE_KEYWORD_FUNCTION) {
			bKeyword = true;
		}
	}

	if (m_pSymbolTable!=NULL && !m_pSymbolTable->lookup(lexbuf))
	{
		// we have a variable
		m_tok.nSymbolType = LEX_TYPE_VARIABLE;
		m_tok.name = lexbuf;

		return ID;
	}
	else if (m_pFcnTable!=NULL && !m_pFcnTable->find( lexbuf )) {
		if (kd.nType == LEX_TYPE_KEYWORD_FUNCTION) {
			// parse fcn call
			m_tok.nSymbolType = kd.nType;		// m_kd contains keyword type (FUNCTION, LANGUAGE, etc)
			m_tok.name = lexbuf;
			return FCN;
		}
	}

	if (bKeyword) {
		m_tok.nSymbolType = kd.nType;		// m_kd contains keyword type (FUNCTION)
		m_tok.name = lexbuf;
		return FCN;
	}

	return NONE;
}

int cLex::checkFunction(bool bSilent)
{
	// now check user fcn/script tables
	if (!m_pFcnTable->getAddr( m_tok.ix, lexbuf ))		// we can't resolve ixh until nargin & nargout are both known (interpret stage)
	{
		// we have a possible valid script function call
		m_tok.ixh = 0;	// dummy value
		m_tok.nSymbolType = LEX_TYPE_SCRIPT_FCN_CALL;
		m_tok.name = lexbuf;
		m_tok.nLineNo = m_lineno;
		return SCRIPT_FCN_CALL;
	}
	else if (!m_pScriptTable->lookup(lexbuf))
	{
		// we have a script call
		// first, check for update
		m_tok.nSymbolType = LEX_TYPE_SCRIPT_CALL;
		m_tok.name = lexbuf;
		m_tok.nLineNo = m_lineno;
		if (!bSilent)
		{
			m_tokBuf.push_back(m_tok);
			m_tokenval = m_tokBuf.size()-1;
		}
		return SCRIPT_CALL;
	}

	return NONE;
}

int cLex::addFcnPointerToken(bool bSilent)
{
	keyData kd = m_kd;
	map<string,keyData>::iterator it = m_pKeywordTable->find( lexbuf );

	// shouldn't need to refresh function list here?
	// now check user fcn/script tables
	if (!m_pFcnTable->getAddr( m_tok.ix, lexbuf ))		// we can't resolve ixh until nargin & nargout are both known (interpret stage)
	{
		// we have a possible valid script function call
		m_tok.ixh = 0;	// dummy value
		m_tok.nSymbolType = LEX_TYPE_SCRIPT_FCN_CALL;
		m_tok.nOperatorType = SCRIPT_FCN_CALL;
		m_tok.name = lexbuf;
		m_tok.nLineNo = m_lineno;
		return LEX_TYPE_FCN_POINTER;
	}
	else if (it != m_pKeywordTable->end())
	{
		// we have a keyword
		if (kd.nType==LEX_TYPE_KEYWORD_FUNCTION) {
			m_tok.nSymbolType = kd.nType;		// m_kd contains keyword type (FUNCTION, LANGUAGE, etc)
			m_tok.name = lexbuf;
			m_tok.nOperatorType = FCN;
			return FCN;
		}
		else {		// error
			report("invalid function pointer - no function found", lexbuf);
		}
	}
	else {		// error
		report("invalid function pointer - no function found", lexbuf);
	}

	return DONE;
}

int cLex::addSymbolToken(bool bSilent)
{
	// is it a keyword or variable?
	int t = checkKeySymbol(bSilent);
	if (t != NONE) return t;

	keyData kd = m_kd;
	m_tok.nLineNo = m_lineno;
	cContainer vnew;

	map<string,keyData>::iterator it = m_pCommandTable->find( lexbuf );

	// is it a command?
	if (it != m_pCommandTable->end())
	{
		// parse command
		m_tok.nSymbolType = LEX_TYPE_KEYWORD_COMMAND;		// m_kd contains keyword type (FUNCTION, LANGUAGE, etc)
		m_tok.name = lexbuf;
		return CMD;
	}

	// not a keyword, built-in fcn, or variable, so refresh user fcn/script tables (if cmd line)
	if (!m_bPreParsing) {
		if (updateAllScripts()) { report("error encountered updating script table"); return true; }
	}

	// is it a function (either native or user-defined)?
	t = checkFunction(bSilent);
	if (t != NONE) return t;

	if (m_bPreParsing)
	{
		// allow an unknown token (for script pre-parsing)
		m_tok.nSymbolType = LEX_TYPE_UNKNOWN;
		m_tok.name = lexbuf;
		m_tok.nLineNo = m_lineno;

		return ID;
	}
	
	int peek = peekTok();

	if (peek=='[' || peek==STRUCT_FIELD)	// hash access, this is ok
	{
		if (m_bAssignment) m_tok.nSymbolType = m_bEquals ? LEX_TYPE_VARIABLE : LEX_TYPE_LVALUE;
		else m_tok.nSymbolType = LEX_TYPE_VARIABLE;
		m_tok.name = lexbuf;
		m_tok.nLineNo = m_lineno;
		m_tokBuf.push_back( m_tok );

		if (m_tok.nSymbolType==LEX_TYPE_LVALUE) {
			vnew.SetName( lexbuf );
			vnew.SetType(VAR_TYPE_HASH);
			if (m_pSymbolTable != NULL)
				m_pSymbolTable->insert( vnew );
		}

		return ID;
	}
	else if (!m_bAssignment || (m_bAssignment && m_bEquals))	// oops, we are already working on r_value !
	{	// nothing we can properly identify (yet)
		report("undefined symbol", lexbuf);
		return DONE;
	}
	else
	{
		// add lvalue token
		if (!bSilent)
		{
			m_tok.nSymbolType = LEX_TYPE_LVALUE;
			m_tok.name = lexbuf;
			m_tok.nLineNo = m_lineno;
			m_tokBuf.push_back( m_tok );

			vnew.SetName( lexbuf );
			if (m_pSymbolTable != NULL)
				m_pSymbolTable->insert( vnew );
		}
		return L_VALUE;
	}
}

bool cLex::stmtNoCheck(void)
{
	bool bSemicolon;
	if (expr()) return true;

	switch (lookAhead)
	{
	case '=':
		m_bEquals = true;
		if (true)
		{
			// push equals sign token
			m_tok.nSymbolType =	LEX_TYPE_EQUALS;
			m_tok.name = "";
			m_tok.nLineNo = m_lineno;
			m_tokBuf.push_back( m_tok );

			lookAhead = nextTok();
			if (expr()) return true;		// rvalue

			bSemicolon = lookAhead == ';';
			if (!bSemicolon && !m_bInteractive)
			{
				report("syntax error: missing ';'"); return true;
			}

			// push statement
			m_stmt.nType = LEX_STATEMENT_ASSIGNMENT;
			m_stmt.tokens = m_tokBuf;
			m_stmt.preTokens = m_tokPre;
			m_stmt.postTokens = m_tokPost;
			m_stmt.bReport = !bSemicolon && m_bInteractive;

			if (!m_bBranchStmt && !m_bPreParsing)		// do not touch m_block if we are inside a branch statment complex (e.g. for(;;)
				m_block.stmts.push_back( m_stmt );

			resetStmt();		// reset state machine

			// next tok
			m_bEquals = false;
		}
		else
		{
			report("syntax error","=");
			return true;
		}
		break;

	case ':':
		if (m_bParsingSwitch) break;
		report("syntax error",":");
		return true;
	
	case ';':
		// push statement
		m_stmt.nType = LEX_STATEMENT_EXPRESSION;
		m_stmt.tokens = m_tokBuf;
		m_stmt.preTokens = m_tokPre;
		m_stmt.postTokens = m_tokPost;
		m_stmt.bReport = false;
		
		if (!m_bBranchStmt && m_bInteractive)
			m_block.stmts.push_back( m_stmt );

		if (!m_bPreParsing) lookAhead = nextTok();

		break;

	case ')':
		if (m_bBranchStmt) {
			// push statement
			m_stmt.nType = LEX_STATEMENT_EXPRESSION;
			m_stmt.tokens = m_tokBuf;
			m_stmt.preTokens = m_tokPre;
			m_stmt.postTokens = m_tokPost;

			m_block.stmts.push_back( m_stmt );
			resetStmt();		// reset state machine

			break;
		}
		else { report("syntax error","unbalanced parentheses"); return true; }

	case DONE:
		m_stmt.nType = LEX_STATEMENT_EXPRESSION;
		m_stmt.tokens = m_tokBuf;
		m_stmt.preTokens = m_tokPre;
		m_stmt.postTokens = m_tokPost;
		m_stmt.bReport = true;

		m_block.stmts.push_back( m_stmt );
		resetStmt();		// reset state machine

		break;

	default:
		report("syntax error", "unexpected token"); return true;
	}

	return false;
}

bool cLex::stmt()
{
	checkAssignment();		// set m_bAssignment

	return stmtNoCheck();
}

bool cLex::expr()
{
	int t;
	if (post_incr()) return true;
	while (true)
	{
		switch (lookAhead)
		{
		case LEX_OPERATOR_RANGE:
			t = lookAhead;
			match(lookAhead);
			if (post_incr()) return true;		// sum ?

			if (lookAhead==LEX_OPERATOR_RANGE)
			{	// get increment argument
				match( LEX_OPERATOR_RANGE );
				if (post_incr()) return true;

				// adjust nargin on range operator

				// add operator token
				m_tok.nSymbolType = LEX_TYPE_OPERATOR_TERTIARY;
				m_tok.nOperatorType = LEX_OPERATOR_RANGE;
				m_tok.nargin = 3;
				m_tok.nLineNo = m_lineno;
				m_tokBuf.push_back(m_tok);
			}
			else
			{
				// add operator token
				m_tok.nSymbolType = LEX_TYPE_OPERATOR;
				m_tok.nOperatorType = LEX_OPERATOR_RANGE;
				m_tok.nargin = 2;
				m_tok.nLineNo = m_lineno;
				m_tokBuf.push_back(m_tok);
			}

			continue;
		default:
			return false;
		}
	}	// while

	return false;
}

bool cLex::post_incr(void)	// currently unused
{
	if (logical()) return true;

	if ( lastLook==ID )
	{
		switch (lookAhead)
		{
		case PLUS_PLUS:
			m_tok.nSymbolType = LEX_OPERATOR_POST_INCR;
			m_tok.nOperatorType = LEX_OPERATOR_POST_INCR;
			m_tok.nLineNo = m_lineno;
			m_tokPost.push_back(m_tok);
			match(lookAhead);
			break;

		case MINUS_MINUS:
			m_tok.nSymbolType = LEX_OPERATOR_POST_DECR;
			m_tok.nOperatorType = LEX_OPERATOR_POST_DECR;
			m_tok.nLineNo = m_lineno;
			m_tokPost.push_back(m_tok);
			match(lookAhead);
			break;

		default:
			return false;
		}

	}

	return false;
}

bool cLex::logical()
{
	int t;
	if (relational()) return true;
	while (true)
	{
		switch (lookAhead)
		{
		case LEX_OPERATOR_RELATIONAL_NEQ:
			t = lookAhead;
			match(lookAhead);
			if (relational()) return true;

			addOperator( LEX_OPERATOR_RELATIONAL_NEQ );
			continue;

		case '&':
			t = lookAhead;
			match(lookAhead);
			if (relational()) return true;

			addOperator( LEX_OPERATOR_LOGICAL_AND );
			continue;

		case '|':
			t = lookAhead;
			match(lookAhead);
			if (relational()) return true;

			addOperator( LEX_OPERATOR_LOGICAL_OR );
			continue;

		default:
			return false;
		}
	}

	return false;
}

bool cLex::relational()
{
	int t;
	if (sum()) return true;
	while (true)
	{
		switch (lookAhead)
		{
		case LEX_OPERATOR_RELATIONAL_EQ:
			t = lookAhead;
			match(lookAhead);
			if (sum()) return true;

			addOperator( LEX_OPERATOR_RELATIONAL_EQ );
			continue;

		case '<':
			t = lookAhead;
			match(lookAhead);
			if (sum()) return true;

			addOperator( LEX_OPERATOR_RELATIONAL_LT );
			continue;

		case LEX_OPERATOR_RELATIONAL_LE:
			t = lookAhead;
			match(lookAhead);
			if (sum()) return true;

			addOperator( LEX_OPERATOR_RELATIONAL_LE );
			continue;

		case '>':
			t = lookAhead;
			match(lookAhead);
			if (sum()) return true;

			addOperator( LEX_OPERATOR_RELATIONAL_GT );
			continue;

		case LEX_OPERATOR_RELATIONAL_GE:
			t = lookAhead;
			match(lookAhead);
			if (sum()) return true;

			addOperator( LEX_OPERATOR_RELATIONAL_GE );
			continue;

		default:
			return false;
		}
	}

	return false;
}

bool cLex::sum()
{
	int t;
	if (term()) return true;
	while (true)
	{
		switch (lookAhead)
		{
		case '+': case LEX_OPERATOR_ELEMENT_PLUS:
			t = lookAhead;
			match(lookAhead);
			if (term()) return true;

			// add operator token
			m_tok.nSymbolType = LEX_TYPE_OPERATOR;
			m_tok.nOperatorType = LEX_OPERATOR_PLUS;
			m_tok.nLineNo = m_lineno;
			m_tokBuf.push_back(m_tok);
			continue;

		case '-': case LEX_OPERATOR_ELEMENT_MINUS:
			t = lookAhead;
			match(lookAhead);
			if (term()) return true;

			// add operator token
			m_tok.nSymbolType = LEX_TYPE_OPERATOR;
			m_tok.nOperatorType = LEX_OPERATOR_MINUS;
			m_tok.nLineNo = m_lineno;
			m_tokBuf.push_back(m_tok);
			continue;

		default:
			return false;
		}
	}	// while

	return false;
}

bool cLex::term()
{
	int t;
	if (pow()) return true;
	while (true)
	{
		switch (lookAhead)
		{
		case '*': case LEX_OPERATOR_ELEMENT_MULT:
			t = lookAhead;
			match(lookAhead);
			if (pow()) return true;

			// add operator token
			m_tok.nSymbolType = LEX_TYPE_OPERATOR;
			switch (t)
			{
			case '*':						m_tok.nOperatorType = LEX_OPERATOR_MULT; break;
			case LEX_OPERATOR_ELEMENT_MULT:	m_tok.nOperatorType = LEX_OPERATOR_ELEMENT_MULT; break;
			};
			m_tok.nLineNo = m_lineno;
			m_tokBuf.push_back(m_tok);
			continue;

		case '/': case LEX_OPERATOR_ELEMENT_DIV:
			t = lookAhead;
			match(lookAhead);
			if (pow()) return true;

			// add operator token
			m_tok.nSymbolType = LEX_TYPE_OPERATOR;
			switch (t)
			{
			case '/':						m_tok.nOperatorType = LEX_OPERATOR_DIV; break;
			case LEX_OPERATOR_ELEMENT_DIV:	m_tok.nOperatorType = LEX_OPERATOR_ELEMENT_DIV; break;
			};
			m_tok.nLineNo = m_lineno;
			m_tokBuf.push_back(m_tok);
			continue;

		case '%': case LEX_OPERATOR_ELEMENT_MOD:
			t = lookAhead;
			match(lookAhead);
			if (pow()) return true;

			// add operator token
			m_tok.nSymbolType = LEX_TYPE_OPERATOR;
			m_tok.nOperatorType = LEX_OPERATOR_MOD;		// '%' == '#%'
			m_tok.nLineNo = m_lineno;
			m_tokBuf.push_back(m_tok);
			continue;

		default:
			return false;
		}
	}

	return false;
}

bool cLex::pow()
{
	int t;
	if (factor()) return true;
	while (true)
	{
		switch (lookAhead)
		{
		case '^': case LEX_OPERATOR_ELEMENT_POW:
			t = lookAhead;
			match(lookAhead);
			if (factor()) return true;

			// add operator token
			m_tok.nSymbolType = LEX_TYPE_OPERATOR;
			switch (t) {
			case '^':						m_tok.nOperatorType = LEX_OPERATOR_POW; break;
			case LEX_OPERATOR_ELEMENT_POW:	m_tok.nOperatorType = LEX_OPERATOR_ELEMENT_POW; break;
			};
			m_tok.nLineNo = m_lineno;
			m_tokBuf.push_back(m_tok);

			break;

		case '\'':
			t = lookAhead;
			match(lookAhead);

			// add operator token
			m_tok.nSymbolType = LEX_TYPE_OPERATOR_UNITARY;
			m_tok.nOperatorType = LEX_OPERATOR_UNITARY_HERMITIAN;
			m_tok.nLineNo = m_lineno;
			m_tokBuf.push_back(m_tok);

			break;

		default:
			return false;
		}
	}
	return false;
}


bool cLex::dereference()
{
	int nargs;
	if (factor()) return true;

	while (true)
	{
		switch (lookAhead) {
		case STRUCT_FIELD:
			match(STRUCT_FIELD);
			if (derefs(nargs)) return true;
			m_tok.nOperatorType = LEX_OPERATOR_DEREFERENCE;
			m_tok.nargin = nargs;
			m_tokBuf.push_back( m_tok );
			break;

		default:
			return false;
		}
	}

	return false;
}

bool cLex::factor()
{
	int n;
	int np, t;
	tokData tokFcn, tokCmd;

	switch (lookAhead)
	{
	case '(':
		if (m_bAssignment) {
			if (m_bEquals) { // RHS
				match('(');
				if (expr()) return true;
				if (match(')')) return true;
			}
			else {			// LHS
				if ( lvalue_clist(m_nArgsOut) ) { m_nArgsOut = 1; report("illegal output argument list"); return true; }
			}
		}
		else {			// expression
			match('(');
			if (expr()) return true;
			if (match(')')) return true;
		}
		break;

	case '[':
		return parseMatrix();

	case ':':
		m_tok.nSymbolType = LEX_TYPE_COLON_ACCESSOR;
		m_tok.nLineNo = m_lineno;
		m_tokBuf.push_back( m_tok );
		if (match(':')) return true;
		break;

	case NUM:
		m_tok.nSymbolType = LEX_TYPE_CONSTANT;
		m_tok.nLineNo = m_lineno;
		m_tokBuf.push_back( m_tok );
		match(NUM); break;

	case STRING:
		m_tok.nSymbolType = LEX_TYPE_STRING;
		m_tok.nLineNo = m_lineno;
		m_tokBuf.push_back( m_tok );
		lookAhead = nextTok();
		break;

	case ID:
		m_tokBuf.push_back( m_tok );
		match(ID);
		if (lookAhead=='(') {
			return parseAccessor();
		}
		else if (lookAhead=='[') {	// hash
			return parseHash();
		}
		else if (lookAhead==STRUCT_FIELD) {	// structure
			derefs(n);
			m_tok.nSymbolType = LEX_OPERATOR_DEREFERENCE;
			m_tok.nOperatorType = LEX_OPERATOR_DEREFERENCE;
			m_tok.nargin = n;
			m_tokBuf.push_back( m_tok );
		}

		break;

	case FCN:
		tokFcn = m_tok;					// push fcn token on stack
		if (args(n)) return true;		// add all fcn arguments to m_tokBuf
		tokFcn.nargin = n;
		tokFcn.nargout = m_nArgsOut;
		// reset argsout
		m_nArgsOut = 1;
		// add fcn token to buffer
		m_tokBuf.push_back(tokFcn);
		break;

	case CMD:
		tokCmd = m_tok;					// push fcn token on stack
		if (cmdArgs(n)) return true;		// add all fcn arguments to m_tokBuf
		tokCmd.nargin = n;
		// add fcn token to buffer
		m_tokBuf.push_back(tokCmd);
		break;

	case SCRIPT_CALL:
		match(SCRIPT_CALL);
		break;

	case SCRIPT_FCN_CALL:
		tokFcn = m_tok;					// push fcn token on stack
		if (args(n)) return true;		// add all fcn arguments to m_tokBuf
		tokFcn.nargin = n;
		tokFcn.nargout = m_nArgsOut;
		// reset argsout
		m_nArgsOut = 1;
		// add fcn token to buffer
		m_tokBuf.push_back(tokFcn);
		break;

	case L_VALUE:
		match(L_VALUE); break;
		// reset argsout
		m_nArgsOut = 1;

	case PLUS_PLUS:
		np = peekTok();
		if ( np==ID ) {
			match(lookAhead);
			if (factor()) return true;
			m_tok.nSymbolType = LEX_OPERATOR_PRE_INCR;
			m_tok.nOperatorType = LEX_OPERATOR_PRE_INCR;
			m_tok.nLineNo = m_lineno;
			m_tokPre.push_back(m_tok);
		}
		else { report("syntax error", "illegal token after ++"); return true; }
		break;

	case MINUS_MINUS:
		np = peekTok();
		if ( np==ID ) {
			match(lookAhead);
			if (factor()) return true;
			m_tok.nSymbolType = LEX_OPERATOR_PRE_DECR;
			m_tok.nOperatorType = LEX_OPERATOR_PRE_DECR;
			m_tok.nLineNo = m_lineno;
			m_tokPre.push_back(m_tok);
		}
		else { report("syntax error", "illegal token after --"); return true; }
		break;

	case STRUCT_FIELD:
		if (derefs(n)) return true;
		m_tok.nSymbolType = LEX_OPERATOR_DEREFERENCE;
		m_tok.nOperatorType = LEX_OPERATOR_DEREFERENCE;
		m_tok.nargin = n;
		m_tokBuf.push_back( m_tok );
		break;

	case '#':		// last element of vector--evaluates to a scalar
		match('#');
		m_tok.nOperatorType = LEX_OPERATOR_ACCESS_END;
		m_tok.nSymbolType = LEX_OPERATOR_ACCESS_END;
		m_tok.nLineNo = m_lineno;
		if (m_argStack.empty()) {
			report("unexpected '#' symbol"); return true;
		}
		m_tok.name = m_argStack.back().name;
		m_tok.ix = m_iarg;
		m_tok.nargin = 0;		// we don't know how many args yet
		m_argStack.back().ix_set.push_back( (int)m_tokBuf.size() );		// index into m_tokBuf pointing to this token
		m_tokBuf.push_back( m_tok );
		break;

	default:
		if (lookAhead=='-' || lookAhead=='+') {		// unitary operator
			t = lookAhead;
			np = peekTok();
			if (np==NUM || np==ID || np==FCN || np==SCRIPT_FCN_CALL || np=='(' || np=='[')
			{
				// insert unitary operator
				m_tok.nSymbolType = LEX_TYPE_OPERATOR_UNITARY;
				if (lookAhead=='-') m_tok.nOperatorType = LEX_OPERATOR_UNITARY_MINUS;
				else				m_tok.nOperatorType = LEX_OPERATOR_UNITARY_PLUS;
			}
			else { report("syntax error", "unexpected token after unitary operator"); return true; }
			m_tokenval = NONE;
			m_tok.nLineNo = m_lineno;
			tokFcn = m_tok;
			lookAhead = nextTok();
			if (term()) return true;
			m_tokBuf.push_back(tokFcn);
		}
		else if (lookAhead=='!') {		// unitary NOT
			t = lookAhead;
			np = peekTok();
			if (np==NUM || np==ID || np==FCN || np==SCRIPT_FCN_CALL || np=='(' || np=='[')
			{
				// insert unitary operator
				m_tok.nSymbolType = LEX_TYPE_OPERATOR_UNITARY;
				m_tok.nOperatorType = LEX_OPERATOR_RELATIONAL_NOT;
			}
			else { report("syntax error", "unexpected token after !"); return true; }
			m_tokenval = NONE;
			m_tok.nLineNo = m_lineno;
			tokFcn = m_tok;
			lookAhead = nextTok();
			if (term()) return true;
			m_tokBuf.push_back(tokFcn);
		}
		else {
			report("syntax error", "unexpected factor token");
			return true;
		}

		return false;

	}	// switch

	return false;
}


bool cLex::parseNativeStmt(vector<sStatement>& stmts)
{
	bool bFoundElse = true;
	int ixif;
	if (m_tok.name.compare("if")==0) {
		ixif = stmts.size() - 1;
		if (parseIf(stmts, bFoundElse)) return true;
		while (bFoundElse) {
			if (parseElse(stmts,ixif,bFoundElse)) return true;
		}
	}
	else if (m_tok.name.compare("for")==0) {
		if (parseFor(stmts)) return true;
	}
	else if (m_tok.name.compare("while")==0) {
		if (parseWhile(stmts)) return true;
	}
	else if (m_tok.name.compare("do")==0) {
		if (parseDo(stmts)) return true;
	}
	else if (m_tok.name.compare("switch")==0) {
		if (parseSwitch(stmts)) return true;
	}
	else if  (m_tok.name.compare("case")==0) {
		if (parseCase(stmts)) return true;
	}
	else if  (m_tok.name.compare("default")==0) {
		if (parseDefault(stmts)) return true;
	}
	else if  (m_tok.name.compare("break")==0) {
		if (parseBreak(stmts)) return true;
	}
	else if  (m_tok.name.compare("continue")==0) {
		if (parseContinue(stmts)) return true;
	}
	else if (m_tok.name.compare("return")==0) {
		if (parseReturn(stmts)) return true;
	}
	else if (m_tok.name.compare("function")==0) {
		sBlock fcnBlock;
		if (parseFunction(fcnBlock, true)) {
			m_pFcnTable->insert(fcnBlock);
			jumpToNextDeclare();
		}
		else {
			m_pFcnTable->insert(fcnBlock);
		}
	}
	else {
		report("illegal statement"); return true;
	}

	return false;
}

bool cLex::parseIf(vector<sStatement>& stmts, bool& bFoundElse)
{
	match(LANGUAGE);

	if (match('(')) { report("syntax error, expecting '('"); return true; }
	if (expr()) return true;
	if (lookAhead != ')') { report("syntax error, expecting ')'"); return true; }

	// fill out sStatement struct for if stmt
	sStatement s;
	s.bStop = false;
	s.tokens = m_tokBuf;
	s.nType = LEX_STATEMENT_IF;
	s.nLineNumer = m_lineno;
	
	m_tokBuf.clear();
	if (match(')')) { report("syntax error"); return true; }

	// get the block of stmts
	vector<sStatement> blockStmts;
	if (parseBlock(blockStmts, false)) return true;		// error inside the block, abort

	// check for else branch
	bFoundElse = false;
	if (strcmp(lexbuf,"else")==0)
		bFoundElse = true;

	// finish if stmt
	s.nSkipTrue = stmts.size() + 1;			// steps back to parent
	s.nSkipFalse = blockStmts.size();

	// insert the if statement itself
	stmts.push_back( s );
	// insert the block of statments following the if
	stmts.insert( stmts.end(), blockStmts.begin(), blockStmts.end() );

	return false;
}

bool cLex::parseElse(vector<sStatement>& stmts, int ixif, bool& bIsElse)
{
	bIsElse = false;
	//int pk = peekTok();

	// parse the else block
	match(LANGUAGE);

	// fill out sStatement struct for if stmt
	sStatement s;
	s.bStop = false;
	s.tokens = m_tokBuf;
	s.nSkipTrue = stmts.size() + 1;
	s.nType = LEX_STATEMENT_ELSE;
	s.nLineNumer = m_lineno;

	// is it an "else if"?
	if (lookAhead==LANGUAGE && strcmp(lexbuf,"if")==0) {
		s.nType = LEX_STATEMENT_ELSE_IF;
		// insert the else if statement itself
		stmts.push_back( s );
		if (parseIf(stmts, bIsElse)) return true;
		// set skipfalse
		stmts[ s.nSkipTrue-1 ].nSkipFalse = stmts.size() - s.nSkipTrue;
	}
	else {		// a simple else block
		// get the block of stmts
		vector<sStatement> blockStmts;
		m_tokBuf.clear();
		if (parseBlock(blockStmts, false)) return true;		// error inside the block, abort

		// finish if stmt
		s.nSkipTrue = stmts.size() + 1;			// steps back to parent
		s.nSkipFalse = blockStmts.size();

		stmts.push_back( s );
		// insert the block of statments following the if
		stmts.insert( stmts.end(), blockStmts.begin(), blockStmts.end() );
	}

	return false;
}

bool cLex::parseSwitch(vector<sStatement>& stmts)
{
	match(LANGUAGE);

	if (match('(')) { report("syntax error, expecting '('"); return true; }
	if (expr()) return true;
	if (lookAhead != ')') { report("syntax error, expecting ')'"); return true; }

	// fill out sStatement struct for if stmt
	sStatement s;
	s.bStop = false;
	s.tokens = m_tokBuf;
	s.nSkipTrue = 0;
	s.nType = LEX_STATEMENT_SWITCH;
	s.nLineNumer = m_lineno;
	
	m_tokBuf.clear();
	if (match(')')) { report("syntax error, expecting ')'"); return true; }

	m_bParsingSwitch = true;

	vector<sStatement> blockStmts;

	// get the block of stmts
	if ( parseBlock(blockStmts, true) ) { m_bParsingSwitch = false; return true; }

	m_bParsingSwitch = false;

	// finish stmt
	s.nSkipFalse = blockStmts.size();

	// insert the switch statement itself
	stmts.push_back( s );
	// insert the block of statments following the if
	stmts.insert( stmts.end(), blockStmts.begin(), blockStmts.end() );

	return false;
}

bool cLex::parseCase(vector<sStatement>& stmts)
{
	match(LANGUAGE);

	if (expr()) return true;
	if (lookAhead != ':') { report("syntax error, expecting ':'"); return true; }

	// fill out sStatement struct for if stmt
	sStatement s;
	s.bStop = false;
	s.tokens = m_tokBuf;
	s.nSkipTrue = 0;
	s.nType = LEX_STATEMENT_CASE;
	s.nLineNumer = m_lineno;
	
	m_tokBuf.clear();
	if (match(':')) { report("syntax error, expecting ':'"); return true; }

	vector<sStatement> blockStmts;
	bool bDone = false;

	// get the block of stmts
	if (parseCaseBlock(blockStmts)) return true;

	// finish stmt
	s.nSkipFalse = blockStmts.size();

	// insert the switch statement itself
	stmts.push_back( s );
	// insert the block of statments following the if
	stmts.insert( stmts.end(), blockStmts.begin(), blockStmts.end() );

	return false;
}

bool cLex::parseDefault(vector<sStatement>& stmts)
{
	match(LANGUAGE);

	if (lookAhead != ':') { report("syntax error, expecting ':'"); return true; }

	// fill out sStatement struct for if stmt
	sStatement s;
	s.bStop = false;
	s.nSkipTrue = 0;
	s.nType = LEX_STATEMENT_DEFAULT;
	s.nLineNumer = m_lineno;
	
	m_tokBuf.clear();
	if (match(':')) { report("syntax error, expecting ':'"); return true; }

	vector<sStatement> blockStmts;
	bool bDone = false;

	// get the block of stmts
	if (parseDefaultBlock(blockStmts)) return true;

	// finish stmt
	s.nSkipFalse = blockStmts.size();

	// insert the switch statement itself
	stmts.push_back( s );
	// insert the block of statments following the if
	stmts.insert( stmts.end(), blockStmts.begin(), blockStmts.end() );

	return false;
}

bool cLex::parseWhile(vector<sStatement>& stmts)
{
	match(LANGUAGE);

	if (match('(')) { report("syntax error"); return true; }
	if (expr()) return true;
	if (lookAhead != ')') { report("syntax error"); return true; }

	// fill out sStatement struct for if stmt
	sStatement s;
	s.bStop = false;
	s.tokens = m_tokBuf;
	s.nType = LEX_STATEMENT_WHILE;
	s.nLineNumer = m_lineno;
	
	m_tokBuf.clear();
	if (match(')')) { report("syntax error"); return true; }

	// get the block of stmts
	vector<sStatement> blockStmts;
	if (parseBlock(blockStmts, false)) return true;		// error inside the block, abort

	// finish if stmt
	s.nSkipTrue = 0;
	s.nSkipFalse = blockStmts.size();

	// insert the if statement itself
	stmts.push_back( s );
	// insert the block of statments following the if
	stmts.insert( stmts.end(), blockStmts.begin(), blockStmts.end() );

	return false;
}

bool cLex::parseDo(vector<sStatement>& stmts)
{
	match(LANGUAGE);

	// get the block of stmts
	vector<sStatement> blockStmts;
	m_tokBuf.clear();
	if (parseBlock(blockStmts, false)) return true;		// error inside the block, abort

	if (lookAhead != LANGUAGE) { report("syntax error: expecting 'while'"); return true; }
	match(LANGUAGE);

	// add the do stmt
	sStatement s;
	s.bStop = false;
	s.nType = LEX_STATEMENT_DO;
	s.nSkipTrue = 0;
	s.nSkipFalse = blockStmts.size();
	s.nLineNumer = m_lineno;
	stmts.push_back( s );

	// parse : while (cond);
	if (match('(')) { report("syntax error"); return true; }
	if (expr()) return true;
	if (lookAhead != ')') { report("syntax error"); return true; }

	// insert the block of statments
	stmts.insert( stmts.end(), blockStmts.begin(), blockStmts.end() );

	// add while cond stmt at the end
	s.tokens = m_tokBuf;
	s.nSkipFalse = 0;
	stmts.push_back( s );

	m_tokBuf.clear();
	if (match(')')) { report("syntax error"); return true; }

	return false;
}

bool cLex::parseFor(vector<sStatement>& stmts)
{
	match(LANGUAGE);

	// fill out sStatement struct for if stmt
	sStatement s_for, s_init, s_stop, s_incr;
	s_init.bStop = false;
	s_stop.bStop = false;
	s_incr.bStop = false;
	m_tokBuf.clear();

	s_for.nType = LEX_STATEMENT_FOR;
	s_for.nLineNumer = m_lineno;
	s_for.nSkipTrue = s_for.nSkipFalse = 0;
	s_for.bStop = false;

	if (match('(')) { report("syntax error"); return true; }

	// loop initialization:
	//
	m_bBranchStmt = true;
	if (stmt()) { m_bBranchStmt = false; return true; }
	m_bBranchStmt = false;

	s_init.nType = m_stmt.nType;
	s_init.preTokens = m_stmt.preTokens;
	s_init.tokens = m_stmt.tokens;
	s_init.preTokens = m_stmt.preTokens;
	s_init.nLineNumer = m_lineno;
	m_tokBuf.clear();

	if (match(';')) { report("syntax error: expecting ';'"); return true; }

	// loop termination:
	//
	m_bBranchStmt = true;
	if (expr()) { m_bBranchStmt = false; return true; }
	m_bBranchStmt = false;
	if (lookAhead == '=') {
		// push equals sign token
		m_tok.nSymbolType =	LEX_TYPE_EQUALS;
		m_tok.name = "";
		m_tok.nLineNo = m_lineno;
		m_tokBuf.push_back( m_tok );

		lookAhead = nextTok();
		if (expr()) return true;		// rvalue
		s_stop.nType = LEX_STATEMENT_ASSIGNMENT;
	}
	else {
		s_stop.nType = LEX_STATEMENT_EXPRESSION;
	}
	s_stop.preTokens = m_tokPre;
	s_stop.tokens = m_tokBuf;
	s_stop.postTokens = m_tokPost;
	s_stop.nLineNumer = m_lineno;

	m_tokPre.clear(); m_tokBuf.clear(); m_tokPost.clear();

	if (match(';')) { report("syntax error: expecting ';'"); return true; }

	// loop increment:
	//
	m_bBranchStmt = true;
	if (stmt()) { m_bBranchStmt = false; return true; }
	m_bBranchStmt = false;

	s_incr.nType = m_stmt.nType;
	s_incr.preTokens = m_stmt.preTokens;
	s_incr.tokens = m_stmt.tokens;
	s_incr.postTokens = m_stmt.postTokens;
	s_incr.nLineNumer = m_lineno;
	m_tokBuf.clear();

	if (match(')')) { report("syntax error"); return true; }

	// get the block of stmts
	vector<sStatement> blockStmts;
	if (parseBlock(blockStmts, false)) return true;		// error inside the block, abort

	// finish if stmt
	s_init.nSkipTrue = 0;
	s_init.nSkipFalse = blockStmts.size() + 2;

	// insert the for statements
	stmts.push_back( s_for );
	stmts.push_back( s_init );
	stmts.push_back( s_stop );
	stmts.push_back( s_incr );
	// insert the block of statments following the if
	stmts.insert( stmts.end(), blockStmts.begin(), blockStmts.end() );

	return false;
}

bool cLex::parseBreak(vector<sStatement>& stmts)
{
	match(LANGUAGE);

	sStatement s;
	s.bStop = false;
	s.nType = LEX_STATEMENT_BREAK;
	s.nLineNumer = m_lineno;
	m_stmt.nSkipTrue = stmts.size() + 1;		// how many steps back to parent
	stmts.push_back( s );

	if (match(';')) return true;

	return false;
}

bool cLex::parseContinue(vector<sStatement>& stmts)
{
	match(LANGUAGE);

	sStatement s;
	s.bStop = false;
	s.nType = LEX_STATEMENT_CONTINUE;
	s.nLineNumer = m_lineno;
	m_stmt.nSkipTrue = stmts.size() + 1;		// how many steps back to parent
	stmts.push_back( s );

	if (match(';')) return true;

	return false;
}

bool cLex::parseReturn(vector<sStatement>& stmts)
{
	match(LANGUAGE);

	sStatement s;
	s.bStop = false;
	s.nType = LEX_STATEMENT_RETURN;
	s.nLineNumer = m_lineno;
	m_stmt.nSkipTrue = stmts.size() + 1;		// how many steps back to parent
	stmts.push_back( s );

	if (lookAhead==';') {
		if (match(';')) return true;
	}

	return false;
}

bool cLex::parseFunction(sBlock& fcnBlock, bool bGetCode)
{
	int flag;
	// save tok buf, clear - we don't want any of this fcn's code on m_tokBuf !!!
	vector<tokData> tokBufSave = m_tokBuf;	// save m_tokBuf for restoration at the end

	sBlock blk;
	blk.nLineOffset = m_lineno - 1;
	tokData tok;

	bool bErr = false;						// return condition
	m_tokBuf.clear();
	int nargin, nargout;
	int pk;

	vector<string> msgSave = m_outMsg;

	match(LANGUAGE);

	// parse fcn declaration
	//
	switch (lookAhead) {
	case ID: case SCRIPT_CALL: case SCRIPT_FCN_CALL:

		tok = m_tok;
		pk = peekTok();	
		if (pk=='=') {			// a single output is given: declare a = func(x,...,z)
			nargout = 1;
			tok.nSymbolType = LEX_TYPE_VARIABLE;
			//m_blockStack.back().outArgs.push_back(tok);		// output arg
			match(lookAhead);
			if (match('=')) { report("illegal function declaration"); bErr = true; goto fcnRestore; }
		}
		else if (pk=='(') {		// no outputs are given: declare func(x,...,z)
			nargout = 0;
		}

		//m_blockStack.back().name = m_tok.name;
		m_tokBuf.clear();

		if (lookAhead==SCRIPT_FCN_CALL || lookAhead==SCRIPT_CALL || lookAhead==ID)
		{	// all OK
			if (decl_args(nargin)) { report("illegal function declaration"); bErr = true; goto fcnRestore; }
		}
		else { report("illegal function declaration"); bErr = true; goto fcnRestore; }

		//m_blockStack.back().inArgs = m_tokBuf;
		assert( m_tokBuf.size()==nargin );

		break;

	case '(':					// multiple outputs given: declare (a,b,c) = func(x,..,z)
		goback(1);
		if (decl_args(nargout)) { report("illegal function declaration"); bErr = true; goto fcnRestore; }

		// fill in out args
		//m_blockStack.back().outArgs = m_tokBuf;
		assert( m_tokBuf.size()==nargout );

		blk.outArgs = m_tokBuf;
		m_tokBuf.clear();

		if (match('=')) { report("illegal function declaration"); bErr = true; goto fcnRestore; }

		tok = m_tok;

		if (lookAhead==SCRIPT_FCN_CALL || lookAhead==SCRIPT_CALL || lookAhead==ID)
		{	// all OK
			if (decl_args(nargin)) { report("illegal function declaration"); bErr = true; goto fcnRestore; }
		}

		// fill in in args
		//m_blockStack.back().inArgs = m_tokBuf;
		assert( m_tokBuf.size()==nargin );

		break;

	default:
		report("syntax error in function declaration"); bErr = true; goto fcnRestore;
	};

	// parse fcn body
	//
	//m_curFcn = m_blockStack.back().name;

	blk.bError = false;
	blk.name = tok.name;
	blk.fileName = m_block.fileName;
	blk.pathName = m_block.pathName;
	blk.inArgs = m_tokBuf;

	m_tokBuf.clear();

	if (bGetCode) {
		if (parseBlock( blk.stmts, true )) {
			blk.outMsg = m_outMsg; blk.bError = true; bErr = true; goto fcnRestore;
		}
		fcnBlock = blk;
	}
	
fcnRestore:
	//m_blockStack.back().bError = bErr;
	//m_curFcn = fcnSave;
	fcnBlock = blk;
	m_outMsg = msgSave;
	m_tokBuf = tokBufSave;
	return bErr;
}

bool cLex::parseAccessor()
{
	tokData tok;
	tok.nSymbolType = LEX_TYPE_OPERATOR_ACCESS;

	int nargs;

	sArgSet as;
	as.name = m_tok.name;

	m_argStack.push_back(as);

	if (mat_args(nargs)) return true;

	if (nargs > 3) {
		report("too many arguments (limit = 3)"); return true;
	}

	tok.nOperatorType = LEX_OPERATOR_ACCESS;
	tok.nargin = nargs;
	tok.nLineNo = m_lineno;
	
	// set .nargin of each '#' token equal to number of args
	int ix;
	for (int i=0; i<(int)m_argStack.back().ix_set.size(); ++i)
	{
		ix = m_argStack.back().ix_set[i];
		m_tokBuf[ix].nargin = nargs;
	}
	m_argStack.pop_back();

	m_tokBuf.push_back( tok );
	
	return false;
}

bool cLex::parseHash()
{
	tokData tok;
	tok.nSymbolType = LEX_TYPE_OPERATOR_HASH_ACCESS;
	tok.name = m_tok.name;

	int nargs;

	if (hash_args(nargs)) return true;

	tok.nOperatorType = LEX_OPERATOR_HASH_ACCESS;
	tok.nargin = nargs;
	tok.nLineNo = m_lineno;

	m_tokBuf.push_back( tok );

	return false;
}

bool cLex::parseMatrix()
{
	tokData tok;
	tok.nSymbolType = LEX_TYPE_OPERATOR;

	if (match('[')) return true;
	if (matrow()) return true;

	while (lookAhead=='\\')
	{
		switch (lookAhead)
		{
		//case ',':
		//	tok.nOperatorType = LEX_OPERATOR_CAT_HORZ;
		//	if (match(',')) return true;
		//	if (expr()) return true;
		//	tok.nLineNo = m_lineno;
		//	m_tokBuf.push_back( tok );
		//	break;

		case '\\':
			tok.nOperatorType = LEX_OPERATOR_CAT_VERT;
			if (match('\\')) return true;
			if (matrow()) return true;
			tok.nLineNo = m_lineno;
			m_tokBuf.push_back( tok );
			break;

		default:
			report("Syntax error");
			return true;
		};
	}
	if (match(']')) return true;

	return false;
}

bool cLex::matrow()
{
	tokData tok;
	tok.nSymbolType = LEX_TYPE_OPERATOR;

	if (lookAhead==']') {	// empty matrix
		tok.nSymbolType = LEX_TYPE_EMPTY;
		m_tokBuf.push_back( tok );
		return false;
	}

	if (expr()) return true;
	while (lookAhead==',')
	{
		tok.nOperatorType = LEX_OPERATOR_CAT_HORZ;
		if (match(',')) return true;
		if (expr()) return true;
		tok.nLineNo = m_lineno;
		m_tokBuf.push_back( tok );
	}

	return false;
}


// argument parsing
bool cLex::decl_args(int& nargs)
{
	nargs = 0;
	lookAhead = nextTok();
	switch (lookAhead)
	{
	case '(':
		match('(');
		if (decl_clist(nargs)) return true;
		if (match(')')) return true;
		break;
	default:
		report("Syntax error");
		return true;
	}

	return false;
}

bool cLex::args(int& nargs)
{
	nargs = 0;
	lookAhead = nextTok();
	switch (lookAhead)
	{
	case '(':
		if (match('(')) return true;
		if (clist(nargs)) return true;
		if (match(')')) return true;
		break;
	default:
		return false;	// no args given to function
	}

	return false;
}

bool cLex::mat_args(int& nargs)
{
	nargs = 0;
	//lookAhead = nextTok();
	switch (lookAhead)
	{
	case '(':
		if (match('(')) return true;
		if (clist(nargs)) return true;
		if (match(')')) return true;
		break;
	default:
		return false;	// no args given to vector/matrix/cube
	}

	return false;
}

bool cLex::hash_args(int& nargs)
{
	nargs = 0;
	while (lookAhead == '[')
	{
		if (match('[')) return true;
		if (expr()) return true;
		if (match(']')) return true;

		++nargs;
	}

	return false;
}

bool cLex::cmdArgs(int& nargs)
{
	nargs = 0;
	int nline = m_tok.nLineNo;
	string arg;

	lookAhead = peekTok();

	switch (lookAhead)
	{
	case '(':
		lookAhead = nextTok();
		if (match('(')) return true;
		if (clist(nargs)) return true;
		if (match(')')) return true;
		break;
	default:
		while (!nextCmdArg(arg)) {
			m_tok.nSymbolType = LEX_TYPE_STRING;
			m_tok.name = arg;
			m_tokBuf.push_back(m_tok);
			++nargs;
		}
		if (!arg.empty()) {
			m_tok.nSymbolType = LEX_TYPE_STRING;
			m_tok.name = arg;
			m_tokBuf.push_back(m_tok);
			++nargs;
		}
		return false;	// no args given to function
	}

	return false;
}

bool cLex::lvalue_clist(int& nCount)
{	// comma-separated lvalue list
	nCount = 0;

	tokData tokSep;
	tokSep.nSymbolType = LEX_TYPE_LVALUE_SEPARATOR;
	tokSep.nLineNo = m_lineno;

	lookAhead = nextTok();

	while (true)
	{
		if (expr()) return true;
		++nCount;

		switch (lookAhead)
		{
		case ',':
			// lvalue arg separator token
			m_tokBuf.push_back( tokSep );
			// get next token (arg or ')')
			if (match(',')) return true;
			break;	// keep going
		case ')':
			// add final (terminating) separator token
			m_tokBuf.push_back( tokSep );
			if (match(')')) return true;
			return false;	// natural end of list
		default:
			report("syntax error", "illegal lvalue list"); return true;
		}
	}

	return false;
}

bool cLex::decl_clist(int& nCount)
{	// comma-separated list
	nCount = 0;
	while (true)
	{
		m_tok.nSymbolType = LEX_TYPE_VARIABLE;
		m_tokBuf.push_back( m_tok );
		if (symbol()) return true;
		++nCount;
		switch (lookAhead)
		{
		case ',':
			if (match(',')) return true;
			break;	// keep going
		case ')':
			return false;	// natural end of list
		default:
			report("syntax error", "illegal decl list"); return true;
		}
	}

	return false;
}

bool cLex::symbol()
{
	tokData tokFcn;

	switch (lookAhead)
	{
	case '(':
		match('(');
		if (symbol()) return true;
		//if (match(')')) return true;
		break;

	case ID: case L_VALUE: case FCN: case SCRIPT_CALL: case SCRIPT_FCN_CALL:
		// add symbol token to buffer
		match(lookAhead);
		break;

	default:
		report("illegal argument list for function");
		return true;
	}

	return false;
}

bool cLex::derefs(int& nCount)
{	// '.' separated list
	nCount = 0;

	while (lookAhead==STRUCT_FIELD)
	{
		++nCount;
		m_tok.nSymbolType = LEX_TYPE_STRUCT_ACCESSOR;
		m_tok.nOperatorType = 0;
		m_tok.name = lexbuf;
		m_tokBuf.push_back( m_tok );
		if (match(STRUCT_FIELD)) return true;
	}

	return false;
}

bool cLex::clist(int& nCount)
{	// comma-separated list
	nCount = 0;
	m_iarg = 0;

	while (true)
	{
		if (expr()) return true;
		++nCount;
		switch (lookAhead)
		{
		case ',':
			if (match(',')) return true;
			++m_iarg;
			break;	// keep going
		case ')':
			return false;	// natural end of list
		default:
			report("syntax error", "illegal symbol list"); return true;
		}
	}

	return false;
}

bool cLex::alist(int& nCount)
{	// comma-separated argument list, bound by []
	nCount = 0;
	while (true)
	{
		if (expr()) return true;
		++nCount;
		switch (lookAhead)
		{
		case ',':
			if (match(',')) return true;
			break;	// keep going
		case ']':
			return false;	// natural end of list
		default:
			report("syntax error", "illegal bracketed list"); return true;
		}
	}

	return false;
}

bool cLex::match(int itok)
{
	if (lookAhead==itok) { lookAhead = nextTok(); return false; }
	//else if (lookAhead==DONE)
	//	return false;
	else
	{
		report("Syntax error");
		return true;
	}
}

bool cLex::quiet_match(int itok)
{
	if (lookAhead==itok) {
		lookAhead = nextTok();
		return false;
	}
	else if (lookAhead==DONE)
		return false;
	else
		return true;		// no error reporting
}

void cLex::emit(int t, int tval)
{
	if (!m_bVerbose) return;

	switch (t)
	{
	case '+': case '-': case '*': case '/': case '%': case '^':
		sprintf( m_pchOut, "%c", t ); break;
	case NUM:
		sprintf( m_pchOut, "%d", tval ); break;
	case ID:
		if (tval < (int)m_tokBuf.size())
		{
			sprintf( m_pchOut, "(symbol %d)", m_tokBuf[tval].nSymbolType );
		}
		break;
	default:
		sprintf( m_pchOut, "token %d, m_tokenval %d", t, tval);
	};

	report( m_pchOut );
}

void cLex::emit(int t, double tval)
{
	if (!m_bVerbose) return;

	switch (t)
	{
	case NUM:
		sprintf( m_pchOut, "%f", tval ); break;
		//printf("%d\n", tval); break;
	default:
		sprintf( m_pchOut, "token %d, m_tokenval %f", t, tval);
		//printf("token %d, m_tokenval %d\n", t, tval);
	};

	report( m_pchOut );
}

void cLex::emit( sBlock& block )
{
	if (!m_bVerbose) return;

	for (int i=0; i<(int)block.stmts.size(); ++i)
	{
		emit( block.stmts[i] );
		for (int j=0; j<(int)block.stmts[i].tokens.size(); ++j) { emit( block.stmts[i].tokens[j] ); }
		report("-------------");
	}
}

//enum { LEX_STATEMENT_ASSIGNMENT=2000, LEX_STATEMENT_EXPRESSION, LEX_STATEMENT_SCRIPT_CALL, LEX_STATEMENT_BLOCK_START, LEX_STATEMENT_IF, LEX_STATEMENT_ELSE_IF, LEX_STATEMENT_ELSE,
//		LEX_STATEMENT_FOR, LEX_STATEMENT_WHILE };
void cLex::emit( sStatement& stmt )
{
	string strType;
	if (!m_bVerbose) return;

	switch (stmt.nType)
	{
	case LEX_STATEMENT_ASSIGNMENT:
		strType = "assignment";
		break;
	case LEX_STATEMENT_EXPRESSION:
		strType = "expression";
		break;
	case LEX_STATEMENT_SCRIPT_CALL:
		strType = "script call";
		break;
	case LEX_STATEMENT_BLOCK_START:
		strType = "block start";
		break;
	case LEX_STATEMENT_IF:
		strType = "if";
		break;
	case LEX_STATEMENT_ELSE_IF:
		strType = "else if";
		break;
	case LEX_STATEMENT_ELSE:
		strType = "else";
		break;
	case LEX_STATEMENT_FOR:
		strType = "for";
		break;
	case LEX_STATEMENT_WHILE:
		strType = "while";
		break;
	default:
		strType = "unknown";
	};

	sprintf( m_pchOut, "stmt: %s", strType.c_str() );
	report(m_pchOut);
}

//enum { LEX_TYPE_CONSTANT=1000, LEX_TYPE_VARIABLE, LEX_TYPE_LVALUE, LEX_TYPE_EQUALS,
//		LEX_TYPE_KEYWORD_FUNCTION, LEX_TYPE_KEYWORD_COMMAND, LEX_TYPE_KEYWORD_LANGUAGE, LEX_TYPE_OPERATOR };	// and so on...
void cLex::emit( tokData& tok )
{
	string strType;
	if (!m_bVerbose) return;

	switch (tok.nSymbolType)
	{
	case LEX_TYPE_CONSTANT:
		strType = "constant";
		break;
	case LEX_TYPE_VARIABLE:
		strType = "variable";
		break;
	case LEX_TYPE_LVALUE:
		strType = "lvalue";
		break;
	case LEX_TYPE_EQUALS:
		strType = "=";
		break;
	case LEX_TYPE_KEYWORD_FUNCTION:
		strType = "fcn";
		break;
	case LEX_TYPE_KEYWORD_COMMAND:
		strType = "cmd";
		break;
	case LEX_TYPE_KEYWORD_LANGUAGE:
		strType = "keyword";
		break;
	case LEX_TYPE_OPERATOR:
		strType = "operator";
		break;
	default:
		strType = "unknown";
	};

	sprintf( m_pchOut, "token: %s", strType.c_str() );
	report(m_pchOut);
}

//m_curFile
//m_curPath

void cLex::report(const char *pch)	{
	string s(pch);
	report( s );
}
void cLex::report(string& msg)
{
	char pch[64];
	string postfix;

	if (m_bPreParsing) {
		//prefix = m_curPath + '\\';
		//prefix = "(";
		//prefix.append( m_curFile );
		sprintf(pch, "%d", m_lineno);
		if (!m_curFile.empty()) {
			postfix.append(" ("); postfix.append(m_curFile); postfix.append(", line "); postfix.append(pch); postfix.append(")");
		}
	}

	m_outMsg.push_back( msg + postfix );
}

void cLex::report(const char *pch, const char *pchSymbol)
{
	string s = pch;
	s.append(": ");
	s.append(pchSymbol);

	report( s );
}

void cLex::report(const char *pch, string& symbol)
{
	return report(string(pch), symbol);
}

void cLex::report(string msg, string& symbol)
{
	msg.append(": ");
	msg.append(symbol);

	report( msg );
}

void cLex::clearReport()			{ m_outMsg.clear(); }
