
#pragma once

#include <stdio.h>
#include <ctype.h>		// char test fcns
#include <string.h>

#define BSIZE		128
#define NONE		-1
#define EOS			'\0'

#define TOK_KEY_CONSTANT	0		// two different kinds of keywords
#define TOK_KEY_FUNCTION	1

//#define TOK_OPERATOR_

#define NUM		256
#define DIV		257
#define MOD		258
#define ID		259
#define DONE	260

int tokenval;
int lineno;

typedef struct tagLexEntry
{
	char *pchLexPtr;
	int nToken;
} lexEntry;

