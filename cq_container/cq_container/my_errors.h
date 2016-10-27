
#pragma once

#include <string>
#include <vector>

using std::string;
using std::vector;

typedef struct tagErrorReport {
	string error;
	string fname;
	int line_no;
} errorReport;

// globals
extern vector<errorReport> globalError;
extern string globalFile;
extern int globalLineNo;

// prototypes
void setErrorFile(string& file);
void setErrorFile(const char *pchFile);
void setErrorLine(int iline);
void setErrorState(string& file, int iline);
void setErrorState(const char *pchFile, int iline);
void setErrorState(errorReport& er);
void resetErrorState();
errorReport getErrorState();

void addError(const char *pch);
void addError(string& s);
void addError(const char *pch, int n);
void addError(string& s, int n);
void addError(const char *pch, string& suf);
void addError(string& s, string& suf);
vector<string> flushError(void);