
#include "storage.h"
#include "state.h"
#include "c_string.h"
#include <fstream>
#include <iostream>

extern cState prog_state;
extern cStorage T;

bool cVariableSpace::IsEmpty()	{ return m_space.count()==0; }
void cVariableSpace::Clear()	{ m_space.clear(); setDefaults(); }

cHash * cVariableSpace::getHashPtr()
{
	return &m_space;
}

void cVariableSpace::setDefaults()
{
	// Set pi, i, j
	double val = PI;
	string spi("pi"), si("i"), sj("j"), strue("true"), sfalse("false");

	Set(spi, val);
	complex<double> i(0.,1.);
	Set(si,i);
	Set(sj,i);
	val = 1.;
	Set(strue, val);
	val = 0.;
	Set(sfalse, val);

	m_hideList.clear();
	m_hideList.push_back("true");
	m_hideList.push_back("false");
}


bool cVariableSpace::lookupEmpty(cContainer& t) const
{
	if (m_space.lookupEmpty(t))	return true;	// no entry found
	return false;
}

bool cVariableSpace::Lookup(cContainer& v, string& Symbol)
{
	if (m_space.lookup(v, Symbol))	return true;	// no entry found
	return false;
}

bool cVariableSpace::LookupInfo(int& Type, int& rows, int& columns, string Symbol)
{
	int depth;
	if (m_space.lookupInfo(Symbol, Type, rows, columns, depth)) return true;
	return false;
}

bool cVariableSpace::LookupInfo(int& Type, int& rows, int& columns, int& depth, string Symbol)
{
	if (m_space.lookupInfo(Symbol, Type, rows, columns, depth)) return true;
	return false;
}


//bool cVariableSpace::LookupInfo(string& Symbol)
//{
////	return m_space.lookupInfo(Symbol);	// do we need this fcn??
//	return false;
//}


void cVariableSpace::Set(string& Symbol, double& A)				{ m_space.set( Symbol, A ); }
void cVariableSpace::Set(string& Symbol, complex<double>& A)	{ m_space.set( Symbol, &A ); }
void cVariableSpace::Set(string& Symbol, cMatrix& A)			{ m_space.set( Symbol, A ); }
void cVariableSpace::Set(string& Symbol, char *pch)				{ string s(pch); m_space.set( Symbol, s ); }
void cVariableSpace::Set(string& Symbol, cString& str)			{ m_space.set( Symbol, str.s ); }
void cVariableSpace::Set(string& Symbol, cContainer& A)			{ m_space.set( Symbol, A ); }

bool cVariableSpace::Set(string& Dest, string& Src)		// set one symbol to another
{
	cContainer v;
	if (m_space.lookup(v, Src)) return true;		// symbol Src not found

	v.SetName( Dest );
	return m_space.insert(v);
}

bool cVariableSpace::Remove(string& Symbol)				{ return m_space.remove(Symbol); }
bool cVariableSpace::IsDefinedSymbol(string& Symbol)	{ return m_space.lookup(Symbol); }
int cVariableSpace::GetNumSymbols()						{ return m_space.count(); }

vector<string>* cVariableSpace::GetSymbols(int nType)
{
	m_symbolList.clear();

	string firstName, name;
	vector<string> symbols;
	int type;
	int nr, nc, nd;

	m_space.getSymbolList( symbols );

	for (int i=0; i<(int)symbols.size(); ++i) {
		m_space.lookupInfo( name, type, nr, nc, nd );
		if (type==nType && !isHidden(name))
			m_symbolList.push_back(name);
	}

	return &m_symbolList;
}

bool cVariableSpace::isHidden(string& name) const
{
	for (int i=0; i<(int)m_hideList.size(); ++i) {
		if (m_hideList[i].compare(name)==0) return true;
	}
	return false;
}

vector<string>* cVariableSpace::GetNumericSymbols()		{ return GetSymbols( VAR_TYPE_NUMERIC ); }
vector<string>* cVariableSpace::GetStringSymbols()		{ return GetSymbols( VAR_TYPE_STRING ); }
//vector<string>* cVariableSpace::GetEmptySymbols()		{ return GetSymbols( VAR_TYPE_EMPTY ); }


vector<string>* cVariableSpace::GetSymbolList()
{
	GetSymbolList(m_symbolList);
	return &m_symbolList;
}

void cVariableSpace::GetSymbolList(vector<string>& list) const
{
	list.clear();

	string firstName, name;
	vector<string> names;

	if ( m_space.getSymbolList(names) )	return;

	for (int i=0; i<(int)names.size(); ++i)
	{
		if (isHidden(names[i])) continue;

		list.push_back( names[i] );
	}

}

// Is symbol Str a valid symbol name?
bool cVariableSpace::IsValidSymbolName(string& Str) const
{
	char ch;
	char UNDERSCORE = 95;

	int L = Str.length();
	if (L==0) return FALSE;

	ch = Str.at(0);
	if (!isalpha(ch)) return FALSE;

	for (int i=1; i<L; i++) {

		ch = char( Str.at(i) );

		if (!isalnum(ch)) {
			if (ch!=UNDERSCORE) return FALSE;
		}

	}

	return true;
}

bool cVariableSpace::Load(string& fname)
{
	vector<cContainer> vars;
	if ( ::Load(fname, vars) ) return true;

	return (T.Insert( vars ));

	return false;
}

bool cVariableSpace::getFormatColumns(string& delim, int& ncols, const char *pchLine)
{
	// initialize size
	ncols = 0;

	if (strstr(pchLine,",")!=NULL) {
		delim = ",";	// comma delimited
	}
	else {
		delim = " \t";	// whitespace delimited
	}

	vector<string> cols;
	string line = pchLine;
	str2cols( cols, delim, line );
	ncols = (int) cols.size();

	return false;
}

bool cVariableSpace::str2cols( vector<string>& cols, string& delim, string& line )
{
	// initialize output
	cols.clear();

	std::basic_string <char>::size_type L, R;

	L = line.find_first_not_of( delim );
	
	// skip white space and comments
	if (( L == string::npos ) /*||( line[L] == '#' )*/ ) {
		return false;
	}

	R = line.find_first_of( delim, L );

	while ( L != string::npos )
	{
		cols.push_back( line.substr(L, R-L) );

		L = line.find_first_not_of( delim, R );
		R = line.find_first_of( delim, L );
	}
	return false;
}

bool cVariableSpace::LoadAscii(cMatrix& R, string& fname)
{
	R.Empty();		// clear R

	cFileBuffer f;
	bool bSingleLine(false);
	string s_line;

	if (f.Load(fname.c_str()))
	{
		addError( f.strError );
		return true;
	}

	char *pchEndline = strstr( f.begin, "\n" );
	char *pch;
	if (pchEndline == NULL) {	// no newline found in file
		pch = new char[f.end - f.begin + 1];
		strncpy(pch, f.begin, f.end - f.begin);
		pch[pchEndline - f.begin] = '\0';	// null terminate
		s_line = pch;
		delete []pch;
		bSingleLine = true;
	}
	else {
		pch = new char[pchEndline - f.begin+1];
		strncpy(pch, f.begin, pchEndline - f.begin);
		pch[pchEndline - f.begin] = '\0';	// null terminate
		s_line = pch;
		delete []pch;
	}

	char *pchBegin = pchEndline + 1;
	while (!bSingleLine && !s_line.empty() && s_line[0]=='#') {		// go past all lines starting with '#'
		pchEndline = strstr(pchBegin, "\n");
		pch = new char[pchEndline - pchBegin+1];
		strncpy(pch, pchBegin, pchEndline - pchBegin);
		pch[pchEndline - pchBegin] = '\0';	// null terminate
		s_line = pch;
		delete []pch;
	}

	int ncols = 0;
	string delim;
	f.Delete();

	if (getFormatColumns(delim, ncols, s_line.c_str())) {
		delete []pch;
		return true;
	}

	std::ifstream data_file( fname.c_str() );

	if ( !data_file.is_open() )
	{
		addError("error opening file ", fname); 
		return true;
	}

	string line;
	char pchWarn[256];
	int iline(0), i;
	vector<string> cols;
	cMatrix Tmp;
	double dv;
	
	if (Tmp.Create(ncols,1)) return true;	// start with N_columns X 1 temp vector

	while( !data_file.eof() )
	{
		++iline;
		getline( data_file, line );
		if (!line.empty() && line[0]=='#')		// ignore lines with leading '#' character
			continue;

		if ( str2cols( cols, delim, line ) || ncols != (int)cols.size() ) {
			sprintf(pchWarn,"warning, line %d ignored, ", iline);
			addError(pchWarn, fname);
			continue;
		}
		for (i=0; i<(int)cols.size(); ++i) {
			dv = atof( cols[i].c_str() );
			Tmp.SetR(i, 0, dv);
		}
		R.AugmentColumns( &Tmp );
	}

	data_file.close();


	R.Trans();					// correct the orientation, transpose is only a bookkeeping operation :)

	char SLASH = '\\';			// retrieve the matrix name out of the full path
	size_t stn;
	stn = fname.find_last_of( SLASH, string::npos );
	if (stn!=string::npos) fname = fname.substr(stn+1, string::npos);
	stn = fname.find_last_of( '.', string::npos );
	if (stn!=string::npos) fname = fname.substr(0, stn);

	return false;
}

bool cVariableSpace::Save(string& fname)
{
	cString name;

	name.SetTo( fname );

	return SaveAll(name);
}

bool cVariableSpace::SaveVariable( FILE *pF, string& strVar )
{
	cContainer var;
	cMatrix R;
	std::string str;

	int L;

	if ( !m_space.lookup( var, strVar ) ) {
		return true;
	}

	//if ( var.Type==VAR_TYPE_NUMERIC ) {
	//	L = strVar.length();
	//	fwrite( &L, sizeof(int), 1, pF );				// # of chars in symbol name
	//	fwrite( strVar.c_str(), sizeof(char), L, pF );	// symbol name (char array)
	//	WriteMatrix( pF, &var.m_M );			// double value associated with symb. name
	//}

	else if ( var.GetType()==VAR_TYPE_STRING ) {
		L = strVar.length();
		fwrite( &L, sizeof(int), 1, pF );				// # of chars in symbol name
		fwrite( strVar.c_str(), sizeof(char), L, pF );	// symbol name (char array)
		// now write the string data itself
		L = var.GetSize();
		fwrite( &L, sizeof(int), 1, pF );
		str = var.s.s;
		fwrite( str.c_str(), sizeof(char), L, pF );
	}

	return false;
}

bool cVariableSpace::SaveVariableAscii( FILE *pF, string& strVar )
{
	cContainer var;
	cMatrix R;

	if ( Lookup( var, strVar ) ) {
		addError("variable not found: ", strVar);
		return true;
	}

	// print header
	string prefix = "## Cquate variable ";
	int len = strVar.length() + prefix.length();
	char *pchHead = new char[len+1];
	sprintf( pchHead, "%s%s", prefix.c_str(), strVar.c_str() );
	fprintf( pF, "%s\n", pchHead );
	delete []pchHead;

	string parentName;

	//if ( var.Type==VAR_TYPE_NUMERIC ) {
	//	PrintMatrix( pF, &var.RMv );			// double value associated with symb. name
	//}

	switch (var.GetType())
	{
	case VAR_TYPE_NUMERIC:	// matrix
		if (var.m.SaveAscii(pF)) return true;
		break;

	case VAR_TYPE_CUBE:
		if (var.c.SaveAscii(pF)) return true;
		break;

	case VAR_TYPE_STRING:
		fprintf( pF, "%s\n", var.s.GetString().c_str() );
		break;

	case VAR_TYPE_HASH:
		parentName = var.GetName();
		if (SaveHashAscii(pF, parentName, var.h)) return true;
		break;

	default:
		addError("unknown variable type encountered!");
		return true;
	}

	return false;
}

bool cVariableSpace::SaveHashAscii( FILE *pF, string parentName, cContainer::cUserHash& h )
{
	std::map<string,cContainer>::iterator it = h.h.begin();

	//// print header
	//string prefix = "### hash ";
	//int len =  prefix.length() + parentName.length();
	//char *pchHead = new char[len+1];
	//sprintf( pchHead, "%s%s", prefix.c_str(), parentName.c_str() );
	//fprintf( pF, "%s\n", pchHead );
	//delete []pchHead;

	string child;

	for (it = h.h.begin(); it != h.h.end(); ++it)
	{
		child = parentName + ".";
		child.append( it->first );

		fprintf(pF, "### %s\n", child.c_str());

		switch (it->second.GetType())
		{
		case VAR_TYPE_NUMERIC:	// matrix
			if (it->second.m.SaveAscii(pF)) return true;
			break;

		case VAR_TYPE_CUBE:
			if (it->second.c.SaveAscii(pF)) return true;
			break;

		case VAR_TYPE_STRING:
			fprintf( pF, "%s\n", it->second.s.GetString().c_str() );
			break;

		case VAR_TYPE_HASH:
			if (SaveHashAscii(pF, child, it->second.h)) return true;
			break;

		default:
			addError("unknown variable type encountered!");
			return true;
		}
	}
	return false;
}

bool cVariableSpace::Save(vector<string> *pVarArray, string& fname)
{
	int N_vars = pVarArray->size();
	string msg;
	bool bErr = false;

	FILE *pF;
	pF = fopen( fname.c_str(), "wb" );
	if (pF==NULL) {
		addError("unable to open file: ", fname); return true;
		//msg = "Unable to open file: ";
		//msg += fname;
		//pDoc->Error.Report(msg);
		//return true;
	}

	int N[2];
	int iError;
	if (GetVarCounts( N, &iError, pVarArray )) {
		msg = "unknown variable: ";
		msg += (*pVarArray)[iError];
		addError(msg);
		//pDoc->Error.Report(msg);
		return true;
	}

	// write the header
	WriteBinaryHeader(pF);

	// write the variable type counts
	fwrite( N, sizeof(int), 2, pF );			// # of each type of symbol

	string name;
	for (int i=0; i<N_vars; i++) {
		name = (*pVarArray)[i];
		bErr = SaveVariable( pF, name );
		if (bErr) break;
	}

	fclose( pF );

	return bErr;
}

bool cVariableSpace::SaveAscii(vector<string>& VarArray, string& fname)
{
	int N_vars = VarArray.size();
	string msg;
	bool bErr = false;

	FILE *pF;
	pF = fopen( fname.c_str(), "w" );
	if (pF==NULL) {
		msg = "Unable to open file: ";
		msg += fname;
		addError(msg);
		return true;
	}

	// write the data
	string name;
	for (int i=0; i<N_vars; i++) {
		name = VarArray[i];
		bErr = SaveVariableAscii( pF, name );
		if (bErr) break;
	}

	fclose( pF );

	return bErr;
}

bool cVariableSpace::GetVarCounts( int *pN, int *iError, vector<string> *pVarArray )
{
	cContainer var;

	*iError = -1;	// no error (yet)

	// initialize counts to zero
	for (int i=0; i<2; i++) pN[i] = 0;

	string name;
	for (int i=0; i<(int)pVarArray->size(); i++) {
		name = (*pVarArray)[i];
		if (!m_space.lookup(var, name)) {
			*iError = i;
			return true;
		}

		if ( var.GetType()==ENUM_NUMERIC )		pN[ENUM_NUMERIC]++;
		if ( var.GetType()==VAR_TYPE_STRING )	pN[ENUM_STRING]++;
	}

	return false;
}

bool cVariableSpace::SavePreferences(string& fname)	{ return false; }
bool cVariableSpace::LoadPreferences(string& fname)	{ return false; }

void cVariableSpace::WriteBinaryHeader(FILE *pF)
{
	char VersionMessage[ BINARY_HEADER_VERSION_CHARS+1 ];
	string strMsg = CQ_BINARY_FILE_HEADER;

	strcpy( VersionMessage, strMsg.c_str() );
	VersionMessage[ strMsg.length() ] = '\0';

	fwrite( VersionMessage, sizeof(char), BINARY_HEADER_VERSION_CHARS, pF );
}

bool cVariableSpace::ReadBinaryHeader(FILE *pF, string& strApp, double& fVersion, int& iType)
{
	char VersionMessage[ BINARY_HEADER_VERSION_CHARS+1 ];
	string strMsg, strTemp;
	string strToken;
	string separators = " ";
	string strWhite = " \t";
	string strBinary = "binary";

	fread( VersionMessage, sizeof(char), BINARY_HEADER_VERSION_CHARS, pF );
	VersionMessage[ BINARY_HEADER_VERSION_CHARS ] = '\0';		// tack on a NULL, just in case

	strMsg = VersionMessage;

	// get the app name (e.g. 'Lemma')
	size_t stn, stn_end;
	stn = strMsg.find_first_of( separators, 0 );
	if (stn != string::npos ) {
		strToken = strMsg.substr( 0, stn );
		TrimWhite(strApp, strToken);
		strMsg = strMsg.substr( stn+1, string::npos );
		//strMsg.TrimLeft();
		stn = strMsg.find_first_not_of( strWhite, 0 );
		strMsg = strMsg.substr( stn, string::npos );
	}
	else {
		return true;
	}

	// get the version number
	//idx = strMsg.Find( separators );
	stn = strMsg.find_first_not_of( separators, 0 );
//	stn = strMsg.find_first_of( separators, stn );
	if (stn != string::npos ) {

		strToken = strMsg.substr( stn, string::npos );
		stn = strToken.find_first_of('v',0);

		strMsg = strMsg.substr( stn+1, string::npos );
		//strMsg.TrimLeft();
		stn = strMsg.find_first_not_of( strWhite, 0 );
		strMsg = strMsg.substr( stn, string::npos );
		stn_end = strMsg.find_first_of( separators, 0 );
		if (stn_end != string::npos)
			strTemp = strMsg.substr( stn, stn_end - stn );
		else
			strTemp = strMsg.substr( stn );

		if (stn_end != string::npos) strMsg = strMsg.substr( stn_end+1, string::npos );
		else return true;

		fVersion = atof( strTemp.c_str() );

	}
	else {
		return true;
	}

	// get the file type
	//idx = strMsg.Find( separators );
	stn = strMsg.find_first_not_of( separators, 0 );
	if (stn != string::npos) {
		//strToken = strMsg.Mid( idx+1 );
		strToken = strMsg.substr( stn, string::npos );
	}
	//strToken.TrimRight();
	size_t stne = strToken.find_last_not_of( strWhite, 0 );
	if (stne != 0)
		strToken = strToken.substr( 0, stne+1 );

	if (strToken.compare(strBinary)==0) {
		iType = FILETYPE_BINARY;
	}
	else {
		return true;
	}

	return false;
}

void cVariableSpace::TrimWhite( string& New, string Old )		// another rendition of CLemmaDoc's TrimWhite funct
{
	size_t ix;
	bool done = false;
	New = Old;
	string whites = " ";
	char chTab = '\t';
	whites += chTab;

	while (!done) {
		ix = New.find_last_of( whites, string::npos );
		if (ix==string::npos) done = true;
		else New.erase( ix, 1 );
	}
}

int cVariableSpace::TextLineLength(string& fname)
{
	FILE *pF = fopen(fname.c_str(), "rb");
	if (pF==NULL) return 0;

	int FUCK_THIS_SHIT = int( pow(2.,30.) );
	int N_block = 1000, N_count = 0, N_bytes, N_block_bytes;
	char *p_buffer = new char[N_block];
	bool done = false, bErr = false;

	while (!done) {
		N_block_bytes = fread( (void *)p_buffer, 1, N_block, pF );
		for (int i=0; i<N_block_bytes; i++)
		{
			if (p_buffer[i]=='\n') {
				N_bytes = (N_count+i);			// found the end of the first line
				done = true;
				break;
			}
		}
		if ( (N_block_bytes<N_block)&&(!done) ) {
			N_bytes = N_block_bytes;			// found the end of file
			done = true;
		}
		N_count += N_block;						// increment count
		if (N_count >= FUCK_THIS_SHIT) {		// fuck this shit... give up
			N_bytes = 0;
			bErr = true;
		}
	}

	fclose(pF);
	delete []p_buffer;

	return N_bytes;
}

int cVariableSpace::ScanLine( char *p_buffer )
{
	int N_values = 0;
	string whitespace = " ";
	char chTab = '\t';
	whitespace += chTab;

	string buffer = p_buffer;
	bool done = false;

	size_t stn;
	stn = buffer.find_first_not_of( whitespace, 0 );
	if (stn != string::npos) {
		buffer = buffer.substr( stn, string::npos );
		++N_values;
	}
	else done = true;

	while ( !done ) {
		stn = buffer.find_first_of( whitespace, 0 );
		if (stn==string::npos)
			done = true;
		else if (stn>0) {
			buffer = buffer.substr(stn,string::npos);

			stn = buffer.find_first_not_of( whitespace, 0 );

			if (stn == string::npos)
				done = true;
			else {
				buffer = buffer.substr( stn, string::npos );
				N_values++;
			}

		}
		else if (buffer.length() > 0) {
			N_values++;
			done = true;
		}
		else done = true;
	}
	return N_values;
}

void cVariableSpace::Empty(void)
{
	// deleta all tables
	m_space.clear();
	setDefaults();
}
