
#include "terminal.h"
#include "state.h"
#include "variable.h"
#include "varUtil.h"

#ifndef _WINDOWS_
#include <sys/ioctl.h>
#include <unistd.h>
#endif
#include <stdio.h>

cTerminal::cTerminal()
{
	m_nColumns = GetTerminalCols();
	m_nMaxLines = 10000;	// Maximum of lines ( default )

	// default number display options
	m_strDefNumFormatType = "Eng";
	m_nDefExpGranular = 3;
	m_nDefDecimals = 3;
	UpdateDefaultFormatStr();	// sets m_DefFormatStr to "%5.3f" using current settings

	SetPrefsToDefault();

	NML_PROMPT = "> ";
	DBG_PROMPT = "d> ";
	PROMPT  = NML_PROMPT;
}


int cTerminal::GetTerminalCols()
{
#if defined(_WIN32)
	return 80;
#elif defined(__linux__)
    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);

    //printf ("lines %d\n", w.ws_row);
    //printf ("columns %d\n", w.ws_col);

	int nCol = (int)w.ws_col;
    return nCol;
#endif
}


void cTerminal::UpdateDefaultFormatStr()
{
	if (m_strDefNumFormatType.compare("Eng")==0 || m_strDefNumFormatType.compare("Sci")==0) {
		m_DefNumFormatStr = "%5.";
	}
	else	// "Fix"
		m_DefNumFormatStr = "%10.";

	char pch[16];
	int nDec = GetFormatDecimals();
	sprintf( pch, "%df", nDec );
	m_DefNumFormatStr.append( pch );
}

void cTerminal::UpdateFormatStr()
{
	if (m_strNumFormatType.compare("Eng")==0 || m_strNumFormatType.compare("Sci")==0) {
		m_NumFormatStr = "%5.";
	}
	else	// "Fix"
		m_NumFormatStr = "%10.";

	char pch[16];
	int nDec = GetFormatDecimals();
	sprintf( pch, "%df", nDec );
	m_NumFormatStr.append( pch );
}

void cTerminal::UpdateFormatType()
{
	if (m_strNumFormatType.compare("Eng")==0) {
		m_nExpGranular = 3;
	}
	else if (m_strNumFormatType.compare("Sci")==0) {
		m_nExpGranular = 1;
	}
	else if (m_strNumFormatType.compare("Fix")==0) {
		// NOP
	}
}

void cTerminal::SetPrefsToDefault()
{
	// numeric display options (command line window)
	m_nExpGranular = m_nDefExpGranular;
	m_strNumFormatType = m_strDefNumFormatType;		// "Eng", "Sci", or "Fix"
	m_nDecimals = m_nDefDecimals;
	UpdateFormatStr();			// sets m_NumFormatStr = "%4.5f" e.g.
}

void cTerminal::PrintLine(string strLine)
{
	printf("%s\n", strLine.c_str());
}

// code below pulled from Erik Aronesty's answer to Stackoverflow question
//   http://stackoverflow.com/questions/2342162/stdstring-formatting-like-sprintf
//
std::string cTerminal::string_format(const std::string fmt, ...)
{
    int size = ((int)fmt.size()) * 2 + 50;   // Use a rubric appropriate for your code
    std::string str;
    va_list ap;
    while (1) {     // Maximum two passes on a POSIX system...
        str.resize(size);
        va_start(ap, fmt);
        int n = vsnprintf((char *)str.data(), size, fmt.c_str(), ap);
        va_end(ap);
        if (n > -1 && n < size) {  // Everything worked
            str.resize(n);
            return str;
        }
        if (n > -1)  // Needed size returned
            size = n + 1;   // For null char
        else
            size *= 2;      // Guess at a larger size (OS specific)
    }
    return str;
}

void cTerminal::AddLine( string& strLine )
{
	size_t st = strLine.find_first_of( NEWLINE_BYTE );
	string strColCode, strOut;
	
	if (st != string::npos) {
		strColCode = strLine.substr(st+1, string::npos);
		strOut = strLine.substr(0, st);
		//GetColorCodes( strColCode, &Colors );
		AddLine( strOut, COLOR_NORMAL );
	}
	else {
		AddLine( strLine, COLOR_NORMAL );
		return;
	}

}

void cTerminal::AddLine( string& strLine, int iCol )
{
	PrintLine( strLine );
}



void cTerminal::AddString( string& str, string& strCol )
{
	cleanTabs(str);

	size_t ix = str.find("\n");
	size_t ixLast = 0;
	string s, sline;
	
	while (ix != string::npos) {

		s = str.substr(ixLast, ix-ixLast);

		AddLine(s);

		ixLast = ix + 1;
		ix = str.find("\n",ix + 1);
	}

	// add last segment
	s = str.substr(ixLast);

	AddLine(s);
}


void cTerminal::cleanTabs(string& str)
{
	size_t ix = str.find("\t");
	
	while (ix != string::npos) {
		str.erase( str.begin()+ix );
		str.insert( ix, _CQ_TAB );

		ix = str.find("\t",ix + 1);
	}
}

void cTerminal::WindowPrintString(cString& cs)
{
	char ColByte[2];
	ColByte[0] = NEWLINE_BYTE; ColByte[1] = '\0';
	string Cntrl = ColByte;
	string ColStr = "4,0";

	AddString(cs.s, ColStr);
}

int cTerminal::GetKeyWidth(cContainer::cUserHash& h)
{
	std::map<string,cContainer>::iterator it;
	string key;
	int maxLen(0);

	for (it = h.h.begin(); it != h.h.end(); ++it) {
		key = it->first;
		if (int(key.length()) > maxLen) maxLen = key.length();
	}
	return maxLen;
}

void cTerminal::WindowPrintHash(cContainer::cUserHash& h)
{
	std::map<string,cContainer>::iterator it;
	string key, strType, strSz;
	int type, r, c, d;
	char pch[64], pchC[64];
	string out;
	int nPad = 6;		// no. whitespace chars between longest key and type column

	int width = GetKeyWidth(h);

	// print keys and data types
	for (it = h.h.begin(); it != h.h.end(); ++it) {
		key = it->first;
		type = it->second.GetType();
		switch (type) {
		case VAR_TYPE_NUMERIC: case VAR_TYPE_CUBE:
			strType = "numeric";
			r = it->second.GetRows();
			c = it->second.GetColumns();
			sprintf(pch, "%d x %d", r, c);
			strSz = pch;
			if (type==VAR_TYPE_CUBE) {
				d = it->second.GetDepth();
				sprintf(pchC, " x %d", d);
				strSz.append( pchC );
			}
			break;

		case VAR_TYPE_STRING:
			strType = "string ";
			sprintf(pch,"%d", it->second.s.GetSize());
			strSz = pch;
			break;

		case VAR_TYPE_HASH:
			strType = "hash   ";
			sprintf(pch,"%d", it->second.h.GetSize());
			strSz = pch;
			break;

		default:
			strType = "empty  ";
			strSz = "0";
		}

		// print row in table
		out = "  ";
		out.append(key);
		int keyLen = key.length();
		out.append(width - keyLen, ' ');
		out.append("   ");
		out.append(strType);
		out.append("   ");
		out.append(strSz);

		AddLine(out);
	}
}


void cTerminal::WindowPrintMatrix(cMatrix& M, int iColumnsPerLine, int nWordSize, int nExp)
{
	// disallow printing more than m_nMaxLines
	int nNewLines = (int) ( double(M.GetRows()) * ceil( double(M.GetColumns()) / double(iColumnsPerLine) ) );
	int ctLines(0);

	int i, j;

	string NumStr1, NumStr2, TmpStr;
	string hdr, spc = string(" "), clip = string("<output clipped>");
	string strExp;
	double a, b;
	complex<double> cv;

	bool bPrintColumnSet = false;
	bool bDone = false;
	int beginColumn = 0, endColumn = 0;

	int nFill;

	string LineStr, Type = "Real";

	char ColByte[2];
	ColByte[0] = NEWLINE_BYTE; ColByte[1] = '\0';
	string Cntrl = ColByte;
	string ColStr = "4,0";
	ColStr = Cntrl + ColStr;

	double lfDiv = pow(10.,nExp);

	// exponent string
	strExp = GetPrintExponent( nExp );

	if (!M.IsReal()) {		// complex matrix

		if (iColumnsPerLine!=0 && iColumnsPerLine>M.GetColumns()) {
			endColumn = M.GetColumns() - 1;
		}
		else
		{
			if (iColumnsPerLine!=0) {
				bPrintColumnSet = true;
				endColumn = iColumnsPerLine - 1;
			}
			else {
				endColumn = beginColumn;
			}
		}

		if (nExp!=0) { LineStr = strExp; AddLine(LineStr); }
		LineStr = " ";
		AddLine( LineStr );

		while (!bDone) {

			if (bPrintColumnSet) {
				hdr = string(" Columns %d - %d");
				TmpStr = GetStr(hdr, beginColumn, endColumn);
				AddLine(TmpStr);
				AddLine(spc);
			}

			for (i=0; i<M.GetRows(); i++) {

				LineStr = " ";

				for (j=beginColumn; j<=endColumn; ++j)
				{
					M.Get(cv,i,j);
					a = cv.real();
					b = cv.imag();
					NumStr1 = GetStr(a / lfDiv);
					if (b < 0) { NumStr2 = GetAbsStr(-b / lfDiv); TmpStr = NumStr1 + " - i" + NumStr2; }
					else {  NumStr2 = GetAbsStr(b / lfDiv); TmpStr = NumStr1 + " + i" + NumStr2; }
					// fill out to nominal column width
					nFill = nWordSize-3 > TmpStr.length() ? nWordSize - 3 - TmpStr.length() : 0;
					if (nFill>0) { LineStr.append(nFill,' '); }
					LineStr.append( TmpStr );
					// add column separator
					if (j!=endColumn) LineStr += " , ";
				}

				// add color code
				LineStr += ColStr;

				AddLine(LineStr);

				++ctLines;
				if (ctLines > m_nMaxLines) {
					bDone = true;
					AddLine(spc);
					AddLine(clip);
					break;
				}

			}

			if (endColumn == M.GetColumns()-1)
				endColumn++;			// exit out of while loop

			else {
				beginColumn += iColumnsPerLine;
				endColumn += iColumnsPerLine;
				if (endColumn >= M.GetColumns())	// clamp the endColumn to the last column
					endColumn = M.GetColumns() - 1;
			}

			bDone = endColumn >= M.GetColumns() || ctLines > m_nMaxLines;

			if (!bDone) {
				AddLine(spc);
			}

		}

	}

	else {		// Real matrix

		if (iColumnsPerLine!=0 && iColumnsPerLine>M.GetColumns()) {
			endColumn = M.GetColumns() - 1;
		}
		else
		{
			if (iColumnsPerLine!=0) {
				bPrintColumnSet = true;
				endColumn = iColumnsPerLine - 1;
			}
			else {
				endColumn = beginColumn;
			}
		}

		if (nExp!=0) { LineStr = strExp; AddLine(LineStr); }
		LineStr = " ";
		AddLine( LineStr );

		while (!bDone) {

			if (bPrintColumnSet) {
				hdr = string(" Columns %d - %d");
				TmpStr = GetStr(hdr, beginColumn, endColumn);
				AddLine(TmpStr);
				AddLine(spc);
			}

			for (i=0; i<M.GetRows(); i++) {

				LineStr = " ";

				for (j=beginColumn; j<=endColumn; ++j)
				{
					M.GetR(a,i,j);
					NumStr1 = GetStr(a / lfDiv);
					// fill out to nominal column width
					nFill = nWordSize-3 > TmpStr.length() ? nWordSize - 3 - TmpStr.length() : 0;
					if (nFill>0) { LineStr.append(nFill,' '); }
					// add number
					LineStr.append( NumStr1 );
					// add column separator
					if (j!=endColumn) LineStr += " , ";
				}

				// add color code
				LineStr += ColStr;

				AddLine(LineStr);

				++ctLines;
				if (ctLines > m_nMaxLines) {
					bDone = true;
					AddLine(spc);
					AddLine(clip);
					break;
				}
			}

			if (endColumn == M.GetColumns()-1)
				endColumn++;			// exit out of while loop
			else {
				beginColumn += iColumnsPerLine;
				endColumn += iColumnsPerLine;
				if (endColumn >= M.GetColumns())	// clamp the endColumn to the last column
					endColumn = M.GetColumns() - 1;
			}

			bDone = endColumn >= M.GetColumns() || ctLines > m_nMaxLines;

			if (!bDone) {
				AddLine(spc);
			}

		}

	}
}

void cTerminal::WindowPrintMatrix(cContainer& V)
{
	string LineStr, Type = "Real";

	char ColByte[2];
	ColByte[0] = NEWLINE_BYTE; ColByte[1] = '\0';
	string Cntrl = ColByte;
	string ColStr = "4,0";
	ColStr = Cntrl + ColStr;

	// columns per line
	int nWordSize;
	int iColumnsPerLine = GetColumnsPerLine(nWordSize, V);

	string msg;

	if (V.GetSize()==0) {
		LineStr = string("[ ]") + ColStr;
		AddLine(LineStr);
		return;
	}

	int nExp(0);

	if (V.m_nType == VAR_TYPE_NUMERIC) {		// matrix
		if (GetDispExponent( nExp, V.m )) {
			msg = string("error determining display properties");
			AddLine(msg, COLOR_ERROR); return;
		}
		return WindowPrintMatrix(V.m, iColumnsPerLine, nWordSize, nExp);
	}

	// cube
	cMatrix m;
	if (GetDispExponent( nExp, V.c )) {
		msg = string("error determining display properties");
		AddLine(msg, COLOR_ERROR); return;
	}

	string str = string("depth %d:");
	for (int k=0; k<V.c.GetDepth(); ++k)
	{
		V.c.GetMatrix(m,k);
		LineStr = string("") + ColStr;
		AddLine(LineStr);
		LineStr = GetStr(str, k) + ColStr;
		AddLine(LineStr);

		WindowPrintMatrix(m, iColumnsPerLine, nWordSize, nExp);
	}

}

void cTerminal::WindowPrintMatrix(cMatrix& m)
{
	string LineStr, Type = "Real";

	char ColByte[2];
	ColByte[0] = NEWLINE_BYTE; ColByte[1] = '\0';
	string Cntrl = ColByte;
	string ColStr = "4,0";
	ColStr = Cntrl + ColStr;

	// columns per line
	int nWordSize;
	int iColumnsPerLine = GetColumnsPerLine(nWordSize, m);

	string msg;

	if (m.GetSize()==0) {
		LineStr = string("[ ]") + ColStr;
		AddLine(LineStr);
		return;
	}

	int nExp;
	if (GetDispExponent( nExp, m )) {
		msg = string("error determining display properties");
		AddLine(msg, COLOR_ERROR); return;
	}

	return WindowPrintMatrix(m, iColumnsPerLine, nWordSize, nExp);
}

// Add to the buffer, cutting off the first ( top ) line if
// we reach buffer size limit. This will be very inefficient
// using std::vector, but what the hell
void cTerminal::UpdateLineBuffer( string& strLine )
{
	string str = strLine;
	m_Lines.push_back( str );
	int nOver = int(m_Lines.size()) - m_nMaxLines;
	if (nOver > 0) {
		m_Lines.erase( m_Lines.begin(), m_Lines.begin() + nOver );
	}
}

string cTerminal::GetFpString(double val)
{
	string Result;

	switch(std::fpclassify(val))
	{
        case FP_INFINITE:
			if (std::signbit(val)) Result = "-Inf";
			else Result = "Inf";
			break;
        case FP_NAN:     Result = "NaN"; break;
		case FP_ZERO:    Result = "0";
        default:
			Result = "0";
    }
	return Result;
}

string cTerminal::GetStr(double val)
{
	string Result;
	char pch[1024];
	if (std::isfinite(val)) {
		if (sprintf( pch, m_NumFormatStr.c_str(), val )<0) return Result;
		Result = pch;
		return Result;
	}

	return GetFpString(val);
}

string cTerminal::GetAbsStr(double val)
{
	return GetStr(fabs(val));
}

string cTerminal::GetStr(string& format, int i1)
{
	string Result;
	char pch[1024];
	if (sprintf( pch, format.c_str(), i1 )<0) return Result;
	Result = pch;
	return Result;
}

string cTerminal::GetStr(string& format, int i1, int i2)
{
	string Result;
	char pch[1024];
	if (sprintf( pch, format.c_str(), i1, i2 )<0) return Result;
	Result = pch;
	return Result;
}


bool cTerminal::GetDispExponent( int& nExp, cMatrix& v )
{
	// fixed format check
	if (m_strNumFormatType.compare("Fix")==0) { nExp = 0; return false; }

	cMatrix vabs;
	double dmax;

	if (Abs(vabs,v)) return true;
	if (vabs.MaxNumberR(dmax)) dmax = 0;

	if (dmax==0) { nExp = 0; return false; }

	double dlog = log10(dmax);		// 10^dlog = dmax
	int imult;

	if (dlog > 0.) {
		imult = (int) floor(dlog/double(m_nExpGranular));
	}
	else {
		imult = (int) floor((dlog+1.)/double(m_nExpGranular));
	}

	nExp = m_nExpGranular * imult;		// largest log not greater than
	
	return false;
}

string cTerminal::GetPrintExponent(int nExp)
{
	string s;
	if (nExp==0) return s;
	if (nExp>400) return s;

	s = "(10^";
	char pch[64];
	sprintf( pch, "%d) * ", nExp );
	s.append( pch );

	return s;
}

int cTerminal::GetColumnsPerLine(int& nWordChars, cContainer& v)
/*
	Returns the number of columns to print to the console per line for
	matrices. If v.Type is not a matrix, indicate that it will fit on
	one line by returning 0. Otherwise, calculate the width of a sample
	number (using the current output string format) and divide the console
	view width by that number. Round toward zero to get the final result.
*/
{
	string ColStr, CellStr, NumStr;
	NumStr = GetStr(100.);

	if (v.GetType()==VAR_TYPE_NUMERIC || v.GetType()==VAR_TYPE_CUBE) {
		//pm = v.GetMatPtr();
		if (v.IsReal()) {
			if (v.GetSize()==0) return 0;
			NumStr += "+";
			CellStr = NumStr;
			ColStr = CellStr + " , ";
		}
		else {
			if (v.GetSize()==0) return 0;
			CellStr = NumStr + " + i";
			CellStr += NumStr;	//Format(string("%s + i%s"), NumStr, NumStr);
			CellStr += "+";
			ColStr = CellStr + " , ";
		}
	}
	else {
		return 0;
	}

	nWordChars = ColStr.length();
	int iColumnsPerLine = int( double(m_nColumns) / double(nWordChars) );

	return iColumnsPerLine;
}

int cTerminal::GetColumnsPerLine(int& nWordChars, cMatrix& v)
{
	string ColStr, CellStr, NumStr;
	NumStr = GetStr(100.);

	if (v.IsReal()) {
		if (v.GetSize()==0) return 0;
		NumStr += "+";
		CellStr = NumStr;
		ColStr = CellStr + " , ";
	}
	else {
		if (v.GetSize()==0) return 0;
		CellStr = NumStr + " + i";
		CellStr += NumStr;	//Format(string("%s + i%s"), NumStr, NumStr);
		CellStr += "+";
		ColStr = CellStr + " , ";
	}

	nWordChars = CellStr.length();
	int iColumnsPerLine = int( double(m_nColumns) / double(nWordChars) );

	return iColumnsPerLine;
}


int cTerminal::GetColorCodes( string& strData, COLORCODE* pColourCode )
{
	assert( pColourCode != NULL );
	//assert( strData.substr(0, 1) == NEWLINE_BYTE );
	strData = strData.substr(1,string::npos);	// Truncate control byte
	int nBytesToSkip = 1;

	// Jump out of strData is now empty...

	if( strData == "" )
	{
		pColourCode->nFore = -1;	// No fore col
		pColourCode->nBack = -1;	// No back col
	}
	else
	{

		// Now handle foreground and background colours:

		size_t stComma = strData.find_first_of( "," );		// Position of comma.  Only present if background colour present
		if( ( stComma != string::npos ) &&
			( ( stComma == 1 ) ||
			  ( stComma == 2 && isdigit( strData[ 1 ] ) ) 
			)
		  )
		{
			string strFore = strData.substr(stComma,string::npos);
			nBytesToSkip += int(strFore.length()) + 1;	// Includes code and comma
			pColourCode->nFore = atoi( strFore.c_str() );		// Get Code
			strData = strData.substr(0, stComma + 1 );		// Truncate string

			// Now look for backcolour:

			string strBack;
			int nCodeLength = 1;
			if( strData.length() >= 2 )
			{
				if( isdigit( strData[ 1 ] ) )
					nCodeLength++;
			};

			nBytesToSkip+= nCodeLength;
			strBack = strData.substr(0, nCodeLength );
			strData = strData.substr( nCodeLength, string::npos );
			pColourCode->nBack = atoi( strBack.c_str() );
		}
		else
		{
			// No background colour present, the delimiter for this code
			// is the next non-numeric character...

			string strFore;
			int nCodeLength = 1;
			if( strData.length() >= 2 )
			{
				if( isdigit( strData[ 1 ] ) )
					nCodeLength++;
			};

			nBytesToSkip += nCodeLength;
			strFore = strData.substr(0, nCodeLength );
			strData = strData.substr( nCodeLength, string::npos );
			pColourCode->nFore = atoi( strFore.c_str() );
			pColourCode->nBack = -1;
		};
	};


	// Return number of bytes processed:
	return( nBytesToSkip );
}


void cTerminal::WindowPrintList(vector<string>& list)
{
	for (int i=0; i<(int)list.size(); ++i) {
		AddLine( list[i] );
	}
}

void cTerminal::WindowPrintBracketList(vector<string>& list)
{
	string line;
	for (int i=0; i<(int)list.size(); ++i) {
		line = "[";
		line.append( list[i] );
		line.append( "]" );

		AddLine( line );
	}
}


// cTerminal state
bool cTerminal::safePreferences(FILE *pf)
{
	int n = (int) m_strNumFormatType.length();
	if (fwrite(&n, sizeof(int), 1, pf) != 1)
		return true;

	if (fwrite(m_strNumFormatType.c_str(), sizeof(char), n, pf) != n)
		return true;

	if (fwrite(&m_nExpGranular, sizeof(int), 1, pf) != 1)
		return true;

	if (fwrite(&m_nDecimals, sizeof(int), 1, pf) != 1)
		return true;

	return false;
}

bool cTerminal::loadPreferences(FILE *pf)
{
	int n;
	char pch[64];

	if (fread(&n, sizeof(int), 1, pf) != 1)
		return true;

	if (n>63) {
		n = 63;
	}

	if (fread(pch, sizeof(char), n, pf) != n)
		return true;

	pch[n] = '\0';
	m_strNumFormatType = pch;

	if (fread(&m_nExpGranular, sizeof(int), 1, pf) != 1)
		return true;

	if (fread(&m_nDecimals, sizeof(int), 1, pf) != 1)
		return true;

	UpdateFormatStr();

	return false;
}
