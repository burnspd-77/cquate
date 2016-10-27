
#pragma once

#include <string>
#include <vector>
#include "container.h"

using std::vector;
using std::string;

#define _JUMP_BREAK		0x01
#define _JUMP_CONTINUE	0x02
#define _JUMP_RETURN	0x04
#define _JUMP_STEP_0	0x08		// step requested on last line of a code block

#define _JUMP_STEP_0_RESET	0xf7

#define _JUMP_BREAK_RESET		0xfe
#define _JUMP_BREAK_CONTINUE	0xfd
#define _JUMP_BREAK_RETURN		0xfc

//// token struct
//typedef struct tagTokData
//{
//	int nSymbolType;		// what kind of token?
//	int ix, ixh;			// symbol address (if a variable)
//	string name;					// this is needed for (as yet) undefined lvalue's
//	double fSymbolVal;				// if a numerical constant
//	int nOperatorType;		// what kind of operator (if an operator)?
//	int nargin;			// no. of input arguments given to this token (if a fcn token)
//	int nargout;			// no. out arguments (for fcn tokens)
//	int nLineNo;			// line number in which this token appears
//	int nColumn;			// text column in which this token appears (first char of token)
//} tokData;


// token struct
typedef struct tagTokData
{
	int nSymbolType;		// what kind of token?
	int ix, ixh;			// symbol address (if a variable)
	string name;					// this is needed for (as yet) undefined lvalue's
	double fSymbolVal;				// if a numerical constant
	int nOperatorType;		// what kind of operator (if an operator)?
	int nargin;			// no. of input arguments given to this token (if a fcn token)
	int nargout;			// no. out arguments (for fcn tokens)
	int nLineNo;			// line number in which this token appears
	int nColumn;			// text column in which this token appears (first char of token)
} tokData;


// statement struct
typedef struct tagStatement
{
	vector<tokData> tokens;		// stmt is a vector of tokens
	vector<tokData> preTokens;		// actions to take before interpreting 'tokens'
	vector<tokData> postTokens;		// actions to take after interpreting 'tokens'
	int nType;			// what kind of stmt?
	int nLineNumer;	// what line number does this occur (if inside a script)
	bool bReport;				// report result to cmd window?
	int nSkipTrue;			// how many stmts ahead to jump (if branch condition is true)
	int nSkipFalse;		// how many stmts ahead to jump (if branch condition is false)
	bool bStop;					// true if debugger set a stop here
} sStatement;

// script block struct, from scriptHash.h
typedef struct tagBlock
{
	vector<sStatement> stmts;	// a block is a vector of stmts
	int nType;			// what kind of block (null (main), branch (if), loop (for, while), script, etc)?
	int nLineOffset;	// line number corresponding to first line of block (=first line of block, if fcn or script)
	time_t t_lastParsed;		// time of last file edit
	string fileName;			// file name (if a fcn or script)
	string pathName;			// path containing this file
	string name;				//
	vector<tokData> inArgs;		// input arguments
	vector<tokData> outArgs;	// function outputs
	vector<string> outMsg;		// parse error/warning messages associated with script
	bool bError;				// is there a parse error in the script?
} sBlock;


typedef struct tagValueBlob
{
	tokData		token;			// token describing lvalue variable (or unknown)
	int			nRows, nColumns, nDepth;	// size of lvalue variable being set
	vector<string> hashArgs;	// forced to do this to support hashes
	cContainer	vArg;			// variables containing indices for selective assignment
	cContainer	vResult;		// result of operation
} valueBlob;


// file-function association support
typedef struct tagFcnInfo
{
	string name;
	int nargin, nargout;
} sFcnInfo;

typedef struct tagFileFcn
{
	string fileName;
	string pathName;
	vector<sFcnInfo> fcns;		// functions in file
} sFileFcn;

typedef struct tagBuffer
{
	const char *pBegin;
	const char *pEnd;
} sBuffer;

typedef vector<tokData> TOKS;

// token types
enum { LEX_TYPE_CONSTANT=1000, LEX_TYPE_VARIABLE, LEX_TYPE_STRING, LEX_TYPE_TEMP_VARIABLE, LEX_TYPE_EMPTY, LEX_TYPE_LVALUE, LEX_TYPE_RVALUE, LEX_TYPE_EQUALS,
		LEX_TYPE_KEYWORD_LANGUAGE, LEX_TYPE_KEYWORD_FCN_ARG, LEX_TYPE_KEYWORD_FUNCTION, LEX_TYPE_KEYWORD_COMMAND,
		LEX_TYPE_SCRIPT_CALL, LEX_TYPE_SCRIPT_FCN_CALL, LEX_TYPE_FCN_POINTER,
		LEX_TYPE_UNKNOWN, LEX_TYPE_COLON_ACCESSOR, LEX_TYPE_STRUCT_ACCESSOR,
		LEX_TYPE_OPERATOR, LEX_TYPE_OPERATOR_UNITARY, LEX_TYPE_OPERATOR_TERTIARY, LEX_TYPE_OPERATOR_ACCESS, LEX_TYPE_OPERATOR_HASH_ACCESS, LEX_TYPE_LVALUE_SEPARATOR };	// and so on...
// operator types
enum { LEX_OPERATOR_PLUS=1500, LEX_OPERATOR_ELEMENT_PLUS,
		LEX_OPERATOR_MINUS, LEX_OPERATOR_ELEMENT_MINUS,
		LEX_OPERATOR_MULT, LEX_OPERATOR_ELEMENT_MULT,
		LEX_OPERATOR_DIV, LEX_OPERATOR_ELEMENT_DIV,
		LEX_OPERATOR_MOD, LEX_OPERATOR_ELEMENT_MOD,
		LEX_OPERATOR_POW, LEX_OPERATOR_ELEMENT_POW,
		LEX_OPERATOR_RANGE, LEX_OPERATOR_DEREFERENCE,
		LEX_OPERATOR_LOGICAL_AND, LEX_OPERATOR_LOGICAL_OR,
		LEX_OPERATOR_RELATIONAL_EQ, LEX_OPERATOR_RELATIONAL_NEQ,
		LEX_OPERATOR_RELATIONAL_LE, LEX_OPERATOR_RELATIONAL_GE,
		LEX_OPERATOR_RELATIONAL_LT, LEX_OPERATOR_RELATIONAL_GT,
		LEX_OPERATOR_RELATIONAL_NOT,
		LEX_OPERATOR_CAT_VERT, LEX_OPERATOR_CAT_HORZ,		// vertical or horizontal cancatenation (binary)
		LEX_OPERATOR_UNITARY_MINUS, LEX_OPERATOR_UNITARY_PLUS,
		LEX_OPERATOR_ACCESS,							// array access
		LEX_OPERATOR_ACCESS_END,						// accessing last element in array
		LEX_OPERATOR_HASH_ACCESS,						// hash access
		LEX_OPERATOR_UNITARY_HERMITIAN,
		LEX_OPERATOR_PRE_INCR, LEX_OPERATOR_PRE_DECR,
		LEX_OPERATOR_POST_INCR, LEX_OPERATOR_POST_DECR,
		LEX_OPERATOR_HELP };	// and so on...
// statement types
enum { LEX_STATEMENT_ASSIGNMENT=2000, LEX_STATEMENT_EXPRESSION,
	LEX_STATEMENT_SCRIPT_CALL, LEX_STATEMENT_FCN_CALL, LEX_STATEMENT_BLOCK_START,
	LEX_STATEMENT_IF, LEX_STATEMENT_ELSE_IF, LEX_STATEMENT_ELSE,
		LEX_STATEMENT_FOR, LEX_STATEMENT_WHILE, LEX_STATEMENT_DO,
		LEX_STATEMENT_SWITCH, LEX_STATEMENT_CASE, LEX_STATEMENT_DEFAULT,
		LEX_STATEMENT_BREAK, LEX_STATEMENT_CONTINUE, LEX_STATEMENT_RETURN };
// code block types
enum { LEX_BLOCK_NULL=2500, LEX_BLOCK_BRANCH, LEX_BLOCK_LOOP, LEX_BLOCK_SCRIPT, LEX_BLOCK_FUNCTION };
