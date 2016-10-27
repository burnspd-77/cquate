
#include "my_errors.h"
#include<stdio.h>
#include<vector>

using std::vector;

// globals
vector<errorReport> globalError;
string globalFile;
int globalLineNo;

// error state mgmt
void setErrorFile(string& file)
{
	globalFile = file;
}
void setErrorFile(const char *pchFile)
{
	string sFile(pchFile);
	setErrorFile(sFile);
}
void setErrorLine(int iline)
{
	globalLineNo = iline;
}
void setErrorState(string& file, int iline)
{
	setErrorFile(file);
	setErrorLine(iline);
}
void setErrorState(const char *pchFile, int iline)
{
	setErrorFile(pchFile);
	setErrorLine(iline);
}
void setErrorState(errorReport& er)
{
	globalFile = er.fname;
	globalLineNo = er.line_no;
}
void resetErrorState()
{
	globalFile.clear();
	globalLineNo = -1;
}
errorReport getErrorState()
{
	errorReport er;
	er.fname = globalFile;
	er.line_no = globalLineNo;
	return er;
}

// error msg mgmt
string getPrefixString()
{
	string sout, t;
	char pch[64];

	if (globalFile.empty())
	{
		return sout;
	}
	else
	{
		t = ", line ";
		sout = globalFile + t;
		sprintf(pch,"%d",globalLineNo);
		sout.append(pch);
	}

	return sout;
}
string getErrorString(errorReport& er)
{
	string sout, t;
	char pch[64];

	if (er.fname.empty())
	{
		sout = er.error;
	}
	else
	{
		t = ", line ";
		t = er.fname + t;
		t = "[" + t;
		sprintf(pch,"%d",er.line_no);
		t.append(pch);
		t = t + "] ";
		sout = t.append( er.error );
	}

	return sout;
}

void addError(const char *pch)
{
	string s(pch);
	addError( s );
}

void addError(string& errMsg)
{
	errorReport er;
	er.error = errMsg;
	er.fname = globalFile;
	er.line_no = globalLineNo;

	globalError.push_back( er );
}

void addError(string& s, int n)
{
	char txt[64]; sprintf(txt,"%d",n);
	string sn=txt;

	errorReport er;
	er.error = s + sn;
	er.fname = globalFile;
	er.line_no = n;
	globalError.push_back( er );
}
void addError(const char *pch, int n)
{
	string msg(pch);
	addError(msg, n);
}


void addError(const char *pch, string& suf)	{ string s = pch; s.append(suf); addError( s ); }
void addError(string& s, string& suf)		{ s.append(suf); addError( s ); }

vector<string> flushError(void)
{
	vector<string> vs;
	string s;
	int i;

	for (i=0; i<globalError.size(); ++i) {
		s = getErrorString( globalError[i] );
		vs.push_back( s );
	}

	globalError.clear();
	return vs;
}
