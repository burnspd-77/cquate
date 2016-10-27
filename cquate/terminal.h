#pragma once

#include "container.h"
#include <string>
#include <stdarg.h>

using std::string;

// Color indices
//
#define COLOR_NORMAL		0
#define COLOR_TYPED			1
#define COLOR_WARNING		2
#define COLOR_ERROR			3

#define		NEWLINE_BYTE		 3			// multi-line control
#define _CQ_TAB		"     "		// 5 spaces
#define _CQ_TAB_WIDTH	5

typedef struct _tagCOLORCODE
{
	unsigned int nFore;		// Foreground color index
	unsigned int nBack;		// Background color index
} COLORCODE;


class cTerminal
{
public:
	cTerminal();
	~cTerminal() { };

	// interface
	void PrintLine(string strLine);	// direct std output

	int GetTerminalCols();

	void WindowPrintList(vector<string>& list);
	void WindowPrintBracketList(vector<string>& list);

	void AddString( string& str, string& strCol );
	void cleanTabs(string& str);
	void WindowPrintString(cString& cs);
	int GetKeyWidth(cContainer::cUserHash& h);
	void WindowPrintHash(cContainer::cUserHash& h);
	void WindowPrintMatrix(cMatrix& M, int iColumnsPerLine, int nWordSize, int nExp);
	void WindowPrintMatrix(cContainer& V);
	void WindowPrintMatrix(cMatrix& m);
	string GetStr(double val);

	void AddLine( string& strLine );	// Add a Line
	void AddLine( string& strLine, int iCol );

	// state
	bool safePreferences(FILE *pf);
	bool loadPreferences(FILE *pf);

	void SetFormatDecimals(int nDec) { m_nDecimals = nDec; UpdateFormatStr(); }

private:
	void UpdateLineBuffer( string& strLine );

	// internal helpers

	// number format
	//
	string GetFormatType(void) { return m_strNumFormatType; }
	void SetFormatType(string fmt) { m_strNumFormatType = fmt; UpdateFormatType(); }
	int GetFormatDecimals() {
		int nDec = m_nDecimals < 0 ? 0 : m_nDecimals;
		return nDec > 16 ? 16 : nDec;
	}
	//
	void UpdateDefaultFormatStr();		// set m_nExpGranular based on "Eng" or "Sci" setting
	void UpdateFormatStr();	// set m_NumFormatStr based on m_nDecimals setting
	void UpdateFormatType();
	//
	void SetPrefsToDefault();
	int GetPrefDefDigits() { return m_nDefDecimals; }

	int GetColumnsPerLine(int& nWordChars, cContainer& v);
	int GetColumnsPerLine(int& nWordChars, cMatrix& v);

	string string_format(const std::string fmt, ...);

	string GetFpString(double val);
	string GetAbsStr(double val);
	string GetStr(string& format, int i1);
	string GetStr(string& format, int i1, int i2);
	bool GetDispExponent( int& nExp, cMatrix& v );
	string GetPrintExponent(int nExp);

	int GetColorCodes( string& strData, COLORCODE* pColourCode );

private:
	vector<string>	m_Lines;			// history

	int m_nMaxLines;		// Maximum buffer size
	int m_nColumns;

	// number display options
	string			m_NumFormatStr;
	string			m_strNumFormatType;
	int				m_nExpGranular;
	int				m_nDecimals;
	// default number display options
	string			m_DefNumFormatStr;
	string			m_strDefNumFormatType;
	int				m_nDefExpGranular;
	int				m_nDefDecimals;

	string NML_PROMPT;
	string DBG_PROMPT;
	string PROMPT;
};
