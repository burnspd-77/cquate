// cirrusPlotDoc.cpp : implementation of the CcirrusPlotDoc class
//


#include "state.h"
#include "matrix.h"
#include "storage.h"
#include "container.h"
#include "float.h"
#include <stdlib.h>
#include <limits>
#include <stdio.h>
#include "lex.h"
#include <iostream>

using namespace std;

extern cState prog_state;

// variable data db
extern cStorage T;


bool compareHistByAlpha(sHistInfo h1, sHistInfo h2)
{
	return h1.cmd.compare( h1.cmd ) < 0;
}

bool compareHistByTime(sHistInfo h1, sHistInfo h2)
{
	return h1.totalSec < h2.totalSec;
}

cState::cState()
{
	m_bDebug = false;
	m_bSafeMode = false;

	bQuitFlag = false;
	m_bAbortProgram = false;
	m_bWakeIdle = false;

	m_bIhaveWarnedAlready = false;

	m_bBreakFlag = false;
	m_bPaused = false;
	m_bAbort = false;
	m_bUpdateScripts = false;
	m_bPreventReset = false;

	m_nHistIdx = -1;

	char dir[MAX_PATH_BUFFER];
	__GETCWD( dir, MAX_PATH_BUFFER-1 );
	m_RunDir = dir;

	T.SetRunDir( m_RunDir );
	m_lang.SetRunDir( m_RunDir );

	m_nCurThread = -1;
	m_bDbBreak = false;
	m_bDbContinue = false;
	m_nDebugLevel = -1;

	// default global preferences
	iFigureCaptureFileType = m_PNG;
	m_bPaused = FALSE;

	// program state sub-component versioning
	m_nPrefVersion = 1;
	m_nHistoryVersion = 1;
	m_nWorkingDirVersion = 1;

	//loadProgramState();

	if (m_WorkingDir.empty()) {
		setWorkingDir( m_RunDir );
	}


#ifdef _DEVELOP_VERSION_
	bLog = TRUE;
#else
	bLog = FALSE;
#endif

	// initialize the default path, and search for all script files within
	//m_lang.setPath();

	// initialize hash tables (symbols, key words, scripts)
	m_lang.setSymbolTable( T.getHashPtr() );
	m_lang.setKeywordTable( T.getKeywordPtr() );
	m_lang.setCommandTable( T.getCommandPtr() );
	m_lang.setScriptTable( T.getScriptPtr() );
	m_lang.setFcnTable( T.getFcnPtr() );

}



cState::~cState()
{

}

bool cState::helpQuery(string& help_page, string& query)
{
	return T.helpQuery(help_page, query);
}

void cState::saveProgramState()
{
	savePreferences();
	saveHistory();
	saveRecentWorkingDirs();
	saveLastWorkingDir();
}

void cState::loadProgramState()
{
	loadPreferences();
	loadHistory();
	loadRecentWorkingDirs();
	loadLastWorkingDir();
}

bool cState::getStateFolder(string& csDir)
{
 //   TCHAR my_documents[MAX_PATH];
 //   if ( SHGetFolderPath(NULL, CSIDL_PERSONAL, NULL, SHGFP_TYPE_CURRENT, my_documents) != S_OK ) {
	//	// report error
	//	return true;
	//}
	//
	//csDir = my_documents;
	//csDir.Append(_T("\\cquate_files"));

	return true;
}

void cState::savePreferences()
{
	string fname;
	if (getPreferencesFile(fname)) {
		addError("internal error: cannot find a location to save preferences file");
		return;
	}

	FILE *pf = fopen(fname.c_str(),"wb");
	if (pf == NULL)
		return;

	fwrite(&m_nPrefVersion, sizeof(int), 1, pf);

	// save display options
	cTerminal::safePreferences(pf);

	fclose( pf );
}

void cState::loadPreferences()
{
	string fname;
	if (getPreferencesFile(fname)) {
		addError("warning: cannot locate preferences file, using default settings");
		return;
	}

	FILE *pf = fopen(fname.c_str(),"rb");
	if (pf == NULL)
		return;

	fread(&m_nPrefVersion, sizeof(int), 1, pf);

	// number display options
	cTerminal::loadPreferences(pf);

	fclose( pf );
}

bool cState::getPreferencesFile(string& fname)
{
	string csDir;

	if (getStateFolder(csDir)) {
		addError("cannot find program state folder");
		return true;
	}

	return false;
}

bool cState::getHistoryFile(string& fname)
{
	string csDir;

	if (getStateFolder(csDir)) {
		addError("cannot find program state folder");
		return true;
	}

	return false;
}

void cState::saveHistory()
{
	string histFile;

	if (getHistoryFile(histFile)) {
		addError("internal error: cannot locate history file for saving");
		return;
	}

	FILE *pf = fopen(histFile.c_str(),"wb");
	if (pf == NULL)
		return;

	fwrite(&m_nHistoryVersion, sizeof(int), 1, pf);

	int n, len;

	// number of entries
	n = (int) m_History.size();
	fwrite(&n, sizeof(int), 1, pf);

	for (int i=0; i<(int)m_History.size(); ++i)
	{
		sHistInfo hi = m_History[i];

		// date string
		len = hi.date.length();
		fwrite( &len, sizeof(int), 1, pf );						// length
		fwrite( hi.date.c_str(), sizeof(char), len, pf );		// string
		// time string
		len = hi.time.length();
		fwrite( &len, sizeof(int), 1, pf );						// length
		fwrite( hi.time.c_str(), sizeof(char), len, pf );		// string
		// cmd string
		len = hi.cmd.length();
		fwrite( &len, sizeof(int), 1, pf );						// length
		fwrite( hi.cmd.c_str(), sizeof(char), len, pf );		// string
		// data
		fwrite( &hi.year, sizeof(int), 1, pf );
		fwrite( &hi.month, sizeof(int), 1, pf );
		fwrite( &hi.day, sizeof(int), 1, pf );
		fwrite( &hi.hour, sizeof(int), 1, pf );
		fwrite( &hi.min, sizeof(int), 1, pf );
		fwrite( &hi.sec, sizeof(int), 1, pf );
		fwrite( &hi.totalSec, sizeof(int), 1, pf );
	}
}

void cState::loadHistory()
{
	string histFile;

	if (getHistoryFile(histFile)) {
		addError("internal error: cannot locate history file for loading");
		return;
	}

	FILE *pf = fopen(histFile.c_str(),"rb");

	//FILE *pf = fopen("history","rb");
	if (pf == NULL)
		return;

	size_t read_res;
	read_res = fread(&m_nHistoryVersion, sizeof(int), 1, pf);
	if (read_res != 1) {
		fclose(pf);
		return;
	}

	int n, len;
	char pch[1028];

	// number of entries
	read_res = fread(&n, sizeof(int), 1, pf);
	if (read_res != 1) {
		fclose(pf);
		return;
	}

	for (int i=0; i<n; ++i)
	{
		sHistInfo hi;

		// date string
		if (fread( &len, sizeof(int), 1, pf ) != 1) { break; }			// length
		if (fread( pch, sizeof(char), len, pf ) != 1) { break; }		// string
		pch[len] = '\0';
		hi.date = pch;
		// time string
		if (fread( &len, sizeof(int), 1, pf ) != 1) { break; }			// length
		if (fread( pch, sizeof(char), len, pf ) != 1) { break; }		// string
		pch[len] = '\0';
		hi.time = pch;
		// cmd string
		if (fread( &len, sizeof(int), 1, pf ) != 1) { break; }			// length
		if (fread( pch, sizeof(char), len, pf ) != 1) { break; }		// string
		pch[len] = '\0';
		hi.cmd = pch;
		// data
		if (fread( &hi.year, sizeof(int), 1, pf ) != 1) { break; }
		if (fread( &hi.month, sizeof(int), 1, pf ) != 1) { break; }
		if (fread( &hi.day, sizeof(int), 1, pf ) != 1) { break; }
		if (fread( &hi.hour, sizeof(int), 1, pf ) != 1) { break; }
		if (fread( &hi.min, sizeof(int), 1, pf ) != 1) { break; }
		if (fread( &hi.sec, sizeof(int), 1, pf ) != 1) { break; }
		if (fread( &hi.totalSec, sizeof(int), 1, pf ) != 1) { break; }

		m_History.push_back( hi );
	}

	fclose(pf);
}

void cState::saveRecentWorkingDirs()
{
	string csDir;

	if (getStateFolder(csDir)) {
		//AfxMessageBox(_T("cannot find program state folder"));
		addError("cannot find program state folder");
		return;
	}

	//string dir;
	//CS2SS(dir,csDir);
	//string histFile = dir + "\\wd_history";

	//FILE *pf = fopen(histFile.c_str(),"wb");

	//if (pf == NULL) {
	//	//AfxMessageBox(_T("cannot open working dir file"));
	//	return;
	//}

	//fwrite(&m_nWorkingDirVersion, sizeof(int), 1, pf);

	//int n, sz, len;

	//// number of entries
	//sz = n = (int) m_WorkingDirHistory.size();
	//if (sz > RECENT_WORKINGDIR_LIMIT)
	//	n = RECENT_WORKINGDIR_LIMIT;

	//fwrite(&n, sizeof(int), 1, pf);

	//for (int i=sz-n; i<sz; ++i)
	//{
	//	string si = m_WorkingDirHistory[i];

	//	// string
	//	len = si.length();
	//	fwrite( &len, sizeof(int), 1, pf );					// length
	//	fwrite( si.c_str(), sizeof(char), len, pf );		// string
	//}

	//fclose(pf);
}

void cState::saveLastWorkingDir()
{
	string csDir;

	if (getStateFolder(csDir)) {
		//AfxMessageBox(_T("cannot find program state folder"));
		addError("cannot find program state folder");
		return;
	}

	//string dir;
	//CS2SS(dir,csDir);
	//string histFile = dir + "\\wd";

	////string histFile = m_RunDir + "\\wd";
	//FILE *pf = fopen(histFile.c_str(),"wb");

	//if (pf == NULL) {
	//	//AfxMessageBox(_T("cannot open working dir file"));
	//	return;
	//}

	//fwrite(&m_nWorkingDirVersion, sizeof(int), 1, pf);

	//int len;

	//string si = getWorkingDir();

	//// string
	//len = si.length();
	//fwrite( &len, sizeof(int), 1, pf );					// length
	//fwrite( si.c_str(), sizeof(char), len, pf );		// string

	//fclose(pf);
}

void cState::loadRecentWorkingDirs()
{
	string csDir;

	if (getStateFolder(csDir)) {
		//AfxMessageBox(_T("cannot find program state folder"));
		addError("cannot find program state folder");
		return;
	}

	//string dir;
	//CS2SS(dir,csDir);
	//string histFile = dir + "\\wd_history";

	////string histFile = m_RunDir + "\\wd_history";
	//FILE *pf = fopen(histFile.c_str(),"rb");

	//if (pf == NULL) {
	//	//AfxMessageBox(_T("cannot open working dir file"));
	//	return;
	//}

	//fread(&m_nWorkingDirVersion, sizeof(int), 1, pf);

	//m_WorkingDirHistory.clear();

	//int n, len;
	//char pch[1028];
	//string si;

	//fread(&n, sizeof(int), 1, pf);

	//for (int i=0; i<n; ++i)
	//{
	//	// string
	//	fread( &len, sizeof(int), 1, pf );			// length
	//	fread( pch, sizeof(char), len, pf );		// string
	//	pch[len] = '\0';
	//	si = pch;

	//	m_WorkingDirHistory.push_back( si );
	//}

	//fclose(pf);
}

void cState::loadLastWorkingDir()
{
	string csDir;

	if (getStateFolder(csDir)) {
		//AfxMessageBox(_T("cannot find program state folder"));
		addError("cannot find program state folder");
		return;
	}

	//string dir;
	//CS2SS(dir,csDir);
	//string histFile = dir + "\\wd";

	////string histFile = m_RunDir + "\\wd";
	//FILE *pf = fopen(histFile.c_str(),"rb");

	//if (pf == NULL) {
	//	//AfxMessageBox(_T("cannot open working dir file"));
	//	return;
	//}

	//fread(&m_nWorkingDirVersion, sizeof(int), 1, pf);

	//int len;
	//char pch[1028];
	//string si;

	//// string
	//fread( &len, sizeof(int), 1, pf );			// length
	//fread( pch, sizeof(char), len, pf );		// string
	//pch[len] = '\0';
	//si = pch;

	//sReportEvent re;
	//string str;

	//setWorkingDir( si );

	//fclose(pf);
}

void cState::addWorkingHistory(string& dir)
{
	int i;
	bool bAlreadyThere(false);
	cString qdirLower, cdir, cwdi;

	cdir.s = dir;
	ToLower(qdirLower, cdir);

	for (i=0; i<(int)m_WorkingDirHistory.size(); ++i)
	{
		cwdi.s = m_WorkingDirHistory[i];
		ToLower(cdir, cwdi);

		if (cdir.s == qdirLower.s) {
			bAlreadyThere = true;
			break;
		}
	}

	if (!bAlreadyThere) {
		m_WorkingDirHistory.push_back( dir );
	}
}

sHistInfo cState::addHistory(string cmd)
{
	char date_buf[128], time_buf[128];
	char yr[32], mo[32], dy[32], hr[32], min[32], sec[32];
	time_t t_now;
	tm * timeinfo;
	time(&t_now);
	timeinfo = localtime(&t_now);
	strftime(date_buf, 128, "%Y-%m-%d", timeinfo);
	strftime(time_buf, 128, "%I:%M:%S %p", timeinfo);
	// set all histInfo fields
	strftime(yr, 32, "%Y", timeinfo);
	strftime(mo, 32, "%m", timeinfo);
	strftime(dy, 32, "%d", timeinfo);
	strftime(hr, 32, "%H", timeinfo);
	strftime(min, 32, "%M", timeinfo);
	strftime(sec, 32, "%S", timeinfo);

	sHistInfo hist;
	hist.date = date_buf;
	hist.time = time_buf;
	//
	hist.year = atoi(yr);
	hist.month = atoi(mo);
	hist.day = atoi(dy);
	hist.hour = atoi(hr);
	hist.min = atoi(min);
	hist.sec = atoi(sec);
	hist.totalSec = (int) t_now;
	//
	hist.cmd = cmd;

	m_History.push_back( hist );
	m_nHistIdx = (int)m_History.size();		// point m_nHistIdx just past top of queue

	return hist;
}

string cState::getHistoryCmd(int ix)
{
	string s;
	if (ix>=0 && ix<(int)m_History.size())
		return m_History[ix].cmd;
	else
		return s;	// empty
}

void cState::incHistoryIdx() { if (m_nHistIdx<getHistSize()) m_nHistIdx++; }
void cState::decHistoryIdx() { if (m_nHistIdx>0) m_nHistIdx--; }

int cState::getHistIdx()
{
	return m_nHistIdx;
}

int cState::getHistSize()
{
	return (int) m_History.size();
}

vector<string> cState::getAllCmdHistory()
{
	vector<string> ret;
	for (int i=0; i<(int)m_History.size(); ++i) { ret.push_back( m_History[i].cmd ); }
	return ret;
}

vector<string> cState::getCmdHistory(int nMax)
{
	vector<string> ret;
	int nHist = (int)m_History.size();
	int n, iBeg;
	
	if (nMax < nHist) {
		n = nMax;
		iBeg = nHist - n - 1;
	}
	else {
		n = nHist;
		iBeg = 0;
	}

	for (int i=0; i<n; ++i) { ret.push_back( m_History[iBeg+i].cmd ); }
	return ret;
}

void cState::sortHistoryByAlpha()
{
	std::sort( m_History.begin(), m_History.end(), compareHistByAlpha );
}

void cState::sortHistoryByTime()
{
	std::sort( m_History.begin(), m_History.end(), compareHistByTime );
}

bool cState::whichScript(string& ret, string& name)
{
	ret.clear();
	return T.getScriptPtr()->which(ret,name);
}

bool cState::whichFcn(string& ret, string& name)
{
	ret.clear();
	return T.getFcnPtr()->which(ret,name);
}

vector<string> cState::getAllScripts()
{
	return T.getScriptPtr()->getAll();
}

vector<string> cState::getAllFcns()
{
	return T.getFcnPtr()->getAll();
}


vector<string> cState::getOpenFileIDs()
{
	vector<string> out;
	int id;
	char pch[64];

	GetFileIDList();

	for (int iFile=0; iFile<(int)m_iFileIDs.size(); iFile++) {
		id = m_iFileIDs[iFile];
		sprintf(pch,"%d",id);
		out.push_back( pch );
	}

	return out;
}

//---------------------------------------------------------------------------
// File I/O
//

bool cState::fileLookup(FILE *pf, int fid)
{
	pf = NULL;
	std::map<int,FILE*>::iterator it = m_OpenFilesTable.find(fid);
	if (it != m_OpenFilesTable.end())
		pf = it->second;
	else {
		addError("invalid file ID: ", fid); return true;
	}
	return false;
}

int cState::GetNextFID()
{
	GlobalFileIDCount++;
	return GlobalFileIDCount;
}

void cState::RmFID( int fid )
{
	std::map<int,FILE*>::iterator it = m_OpenFilesTable.find(fid);
	if (it != m_OpenFilesTable.end())
		m_OpenFilesTable.erase(it);
}

void cState::AddFID( int fid, FILE *pFile )
{
	m_OpenFilesTable[fid] = pFile;
}

void cState::CloseAllFiles(void)
{
	int id;
	GetFileIDList();		// set m_iFileIDs

	for (int iFile=0; iFile<(int)m_iFileIDs.size(); iFile++) {
		id = m_iFileIDs[iFile];
		FClose( id ); 
	}

	m_iFileIDs.clear();
}

void cState::GetFileIds(vector<int>& vids)
{
	vids.clear();
	for (std::map<int,FILE*>::iterator it = m_OpenFilesTable.begin(); it!=m_OpenFilesTable.end(); ++it)
	{
		vids.push_back(it->first);
	}
	
}

//---------------------------------------------------------------------------
// Code interpreting functions
//

void CheckQueue()
{
	prog_state.RunNextCommand();
}

void cState::AddPasteCache( string& text )
{
	m_pasteCache.append( text );
}

void cState::RunNextCommand()
{
	if (m_cmdQueue.empty()) {
		return;
	}

	sCommand s = m_cmdQueue[0];
	m_cmdQueue.erase( m_cmdQueue.begin() );

//	SendRunningNotify();
	ParseManager( s.strCmd, false );
//	SendFinishedNotify();
}

bool cState::MultiLineManager(string& TypedString)
{
	bool bErr(false);

	if (!m_pasteCache.empty()) {
		m_pasteCache.append(TypedString);
		bErr = DoTheWork(m_pasteCache);
		m_pasteCache.clear();
		return bErr;
	}

	return DoTheWork(TypedString);
}

bool cState::DoTheWork(string& TypedString)
{
	string Fname;
	bool bResult = false;
	m_bPreventReset = false;
	m_bWakeIdle = false;

	sHistInfo elem = addHistory(TypedString);

	bResult = ParseManager(TypedString, false);

	m_bAbort = false;

	m_bPreventReset = false;

	return bResult;
}

void cState::trimLeft(string& TypedLine)
{
	string white(" \t");
	size_t pos = TypedLine.find_first_not_of(white);
	if (pos == string::npos) TypedLine.clear();
	else {
		TypedLine = TypedLine.substr(pos);
	}
}

void cState::trimRight(string& TypedLine)
{
	string white(" \t");
	size_t pos = TypedLine.find_last_not_of(white);
	if (pos == string::npos) TypedLine.clear();
	else {
		TypedLine = TypedLine.substr(0,pos+1);
	}
}


vector<string> cState::splitLine(string& cmd)	// split a typed line into multiple statments, if necessary
{
	vector<string> stmts;
	string tmp;
	string sep(";");
	size_t pos_L = 0;
	size_t pos_R = cmd.find_first_of( sep, 0 );
	if (pos_R == string::npos) {
		stmts.push_back( cmd );
		return stmts;
	}

	while (pos_R != string::npos) {
		tmp = cmd.substr( pos_L, pos_R-pos_L+1 );
		stmts.push_back( tmp );
		pos_L = pos_R + 1;
		pos_R = cmd.find_first_of( sep, pos_L );
	}

	tmp = cmd.substr( pos_L );
	if (!tmp.empty())
		stmts.push_back( tmp );

	return stmts;
}

bool cState::ParseManager(string& TypedLine, bool bNotifyEditor)
{
	bool bErr = false;
	sReportEvent re;

	int jmpState(0);
	bool bJmpOut(false), bJmpIn(false);

	string varName;

	trimLeft(TypedLine);
	trimRight(TypedLine);

	if (TypedLine.empty())
		return false;

	log("trimmed.");

	// create an interpreter object
	cInterpret *pIntr = getInterpret();
	if (pIntr == NULL) {
		addError("NULL pointer, where interpreter was expected!");
		return true;
	}

	vector<string> strStmts = splitLine(TypedLine);
	sBlock parse_output;

	log("pIntr.");

	m_lang.pushState();
	m_lang.setLineNo( 1 );
	m_lang.setInteractive(true);
	m_lang.setBuffer( TypedLine );
	m_lang.m_bPreParsing = strStmts.size() > 1;

	log("lang ready to parse.");

	if ( m_lang.parseStmts( parse_output.stmts ) )
	{
		printError();
		m_lang.clearReport();
		m_lang.popState();
		bErr = true;
		goto ParseEnd;
	}

	log("parsed.");

	// capture errors in cases when they occur inside cLex::nextTok (no error capture there!)
	if (!m_lang.m_outMsg.empty()) {
		printError();	m_lang.clearReport();
		m_lang.popState();
		bErr = true;
		goto ParseEnd;
	}
	m_lang.popState();

	log("errors reported.");

	m_lang.setInteractive(false);

	re.bReport = true;

	//parse_output = m_lang.getOutput();
	m_lang.reset();

	pIntr->setSymbolTable( T.getHashPtr() );
	pIntr->setKeywordTable( T.getKeywordPtr() );
	pIntr->setCommandTable( T.getCommandPtr() );
	pIntr->setScriptTable( T.getScriptPtr() );
	pIntr->setFcnTable( T.getFcnPtr() );

	// COMMENTED FOR DEBUGGING, 19:05 3/14
	//
	if ( pIntr->interpretStmts( parse_output.stmts, jmpState, bJmpOut, bJmpIn ) ) {
		bErr = true; goto ParseEnd;
	}
	if ( !parse_output.stmts.empty() && parse_output.stmts.back().bReport ) {
		re.iType = RPT_MATRIX;
		re.varName = pIntr->getResultName();
		pushReport(re);
	}

	log("interpreted.");

	// xfer rvalue
	m_rvalue = pIntr->m_rvalue;

ParseEnd:

	m_lang.m_bPreParsing = true;

	// give parser the correct symbol table pointer
	m_lang.setSymbolTable( T.getHashPtr() );

	vector<string> newFiles;
	if (m_bUpdateScripts)
	{
		m_bUpdateScripts = false;

		if (m_lang.updateAllScripts(newFiles)) {
			m_lang.report("error updating script table");
			bErr = true;
		}
	}

	// send error and/or debug output to cmdView
	printError();
	m_lang.clearReport();
	pIntr->clearReport();

	flushReports();


	return bErr;
}


void cState::flushReports()
{
	for (int i=0; i<(int)m_textReports.size(); ++i)
	{
		if (m_textReports[i].bReport) {
			switch (m_textReports[i].iType) {
			case RPT_MATRIX:
				WindowPrintMatrix( m_textReports[i].varName, true );
				break;
			case RPT_LIST:
				WindowPrintList( m_textReports[i].list );
				break;
			case RPT_BRACKET_LIST:
				WindowPrintBracketList( m_textReports[i].list );
				break;
			case RPT_ERROR:
				WindowPrintList( m_textReports[i].list );
				break;
			//default:
			//	// NOP
			}
		}
	}
	resetReportState();
}

void cState::printError()
{
	sReportEvent re;
	re.bReport = true;
	re.iType = RPT_ERROR;
	re.list = flushError();
	re.list.insert( re.list.end(), m_lang.m_outMsg.begin(), m_lang.m_outMsg.end() );
	re.list.insert( re.list.end(), getInterpret()->m_outMsg.begin(), getInterpret()->m_outMsg.end() );

	pushReport(re);

	//// send error and/or debug output to cmdView
	//for (i=0; i<s.size(); ++i)
	//	pv->AddLine( s[i] );

	//for (i=0; i<m_lang.m_outMsg.size(); ++i)
	//	pv->AddLine( m_lang.m_outMsg[i] );
}


void cState::Pause(void)
{
	m_bPaused = true;
}



void cState::feedReports(sReportEvent& re, bool bShowName)
{
	switch (re.iType) {
	case RPT_MATRIX:
		WindowPrintMatrix( re.varName, bShowName );
		break;
	case RPT_LIST:
		WindowPrintList( re.list );
		break;
	case RPT_BRACKET_LIST:
		WindowPrintBracketList( re.list );
		break;
	case RPT_ERROR:
		WindowPrintList( re.list );
		break;
	}
}

void cState::feedReports(vector<sReportEvent>& vre, bool bShowName)
{
	for (int i=0; i<(int)vre.size(); ++i)
	{
		switch (vre[i].iType) {
		case RPT_MATRIX:
			WindowPrintMatrix( vre[i].varName, bShowName );
			break;
		case RPT_LIST:
			WindowPrintList( vre[i].list );
			break;
		case RPT_BRACKET_LIST:
			WindowPrintBracketList( vre[i].list );
			break;
		case RPT_ERROR:
			WindowPrintList( vre[i].list );
			break;
		}
	}

}


void cState::WindowPrintMatrix(string VarName, bool bShowName)
{
	cContainer V;

	if (VarName=="1") {
		if (getInterpret() != NULL) {
			V = m_rvalue;
		}
		VarName.clear();
	}
	else {
		if (VarName.empty()) {
			if (T.LookupEmpty(V)) return;
		}
		else {
			if (T.Lookup(V, VarName)) return;
		}
	}

	char ColByte[2];
	ColByte[0] = NEWLINE_BYTE; ColByte[1] = '\0';
	string Cntrl = ColByte;
	string ColStr = "4,0";
	ColStr = Cntrl + ColStr;

	string sOut;

	if (bShowName) {
		sOut = VarName + string(" =");
		sOut += ColStr;
		AddLine(sOut);
	}

	switch (V.GetType()) {
	case VAR_TYPE_STRING:
		return WindowPrintString(V.s);
		break;

	case VAR_TYPE_HASH:
		return WindowPrintHash(V.h);
		break;

	default:
		// matrix or cube.. down below
		break;
	};

	cTerminal::WindowPrintMatrix(V);
}
