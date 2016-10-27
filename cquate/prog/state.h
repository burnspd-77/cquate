// cirrusPlotDoc.h : interface of the CcirrusPlotDoc class
//

#pragma once

#include "matrix.h"
#include "c_string.h"
#include "defines.h"
#include "storage.h"
#include "lex.h"
#include "interpret.h"
#include "structs.h"
#include "terminal.h"
#include <time.h>
#include <vector>
#include <string>
#include <map>
#include <iostream>

using std::vector;
using std::string;
using std::map;

enum { CMD_TYPE_CONSOLE = 0, CMD_TYPE_DIRECT, CMD_TYPE_DEBUGGER, CMD_TYPE_CONSOLE_TEXT, CMD_TYPE_QUIT,
	CMD_TYPE_NOTIFY_EDIT_BUSY, CMD_TYPE_NOTIFY_EDIT_READY };
enum { RPT_NULL = 0, RPT_ERROR, RPT_MATRIX, RPT_LIST, RPT_BRACKET_LIST };

enum { DBG_ACTION_GO = 0, DBG_ACTION_STOP, DBG_ACTION_STEP, DBG_ACTION_STEP_OUT, DBG_ACTION_STEP_INTO,
		DBG_ACTION_QUIT };

class cState : public cTerminal
{

// Attributes
public:
	bool m_bSafeMode;

	// debug state
	bool m_bDebug;

	string m_WorkingDir;

	FILE *err_file;
	bool DataExist, bAuxHide;

	// db of current open files
	map<int, FILE*> m_OpenFilesTable;
	vector<int> m_iFileIDs;

	// language parser and interpreter
	cLex m_lang;
	cInterpret m_interp;

	// multi-line edit paste cache
	//
	string m_pasteCache;

	vector<string> CmdResponse;

	int				m_nCurThread;
	bool			m_bDbBreak;
	bool			m_bDbContinue;
	int				m_nDebugLevel;		// how many debug levels deep we are (-1 indicates normal mode)

	cContainer		m_rvalue;

	bool m_bInitError;

	// program & system support
	vector<sCommand> m_cmdQueue;
	//cThreadState	m_thread;

	bool			m_bPaused;
	bool			m_bPreventReset;
	bool			m_bAbort;			// worker thread abort indication
	bool			m_bWakeIdle;		// a command came through the pipeline which requires waking an idle thread

	int				m_nPrefVersion;		// versioning for save & loading

	int				m_nHistoryVersion;		// versioning for save & loading
	vector<sHistInfo>	m_History;			// history of typed commands
	int				m_nHistIdx;

	vector<string>	m_WorkingDirHistory;			// history of previous m_nDirHistory working paths
	int				m_nWorkingDirVersion;

	vector<sEditEvent> m_editEvents;

	bool			m_bIhaveWarnedAlready;

	vector<string> m_Path, m_ScriptFileList;		// holds all active directories for lem files
	//int m_nDirHistory;					// number of paths to save in CWD_History array
	string m_RunDir;						// run directory (where program binary is located)
	string m_HelpDir;
	string m_HelpMain;
	string m_WorkingFileName;			// filename of working document
	bool bQuitFlag;						// quit command found, prepare to exit gracefully
	bool m_bUpdateScripts;				// true indicates script hash must be refreshed (working dir may have changed, e.g.) 
	bool m_bAbortProgram;
	bool m_bBreakFlag;

	string m_editorProgram;			// full path & name of the default editing program
	string m_editorProgramDir;

	// global preferences
	int iFigureCaptureFileType;
	enum { m_JPEG, m_BITMAP, m_PNG };

	string m_plotterProgram;
	string m_plotterProgramDir;

	// error logging
	BOOL bLog;
	FILE *pExpErrLog;
	FILE *pCodeErrLog;
	FILE *pGraphLog;

	vector<sReportEvent> m_textReports;
	vector<sLocalEvent> m_localEvents;

// Operations
public:
	cState();
	~cState();

	void log(const char *pch) {
		if (m_bDebug)
			std::cout << pch << std::endl;
	}

	bool helpQuery(string& help_page, string& query);

	void saveProgramState();
	void loadProgramState();

	bool getStateFolder(string& csDir);

	// Program state management
	void savePreferences();
	void loadPreferences();

	bool getHistoryFile(string& fname);
	bool getPreferencesFile(string& fname);

	void saveHistory();
	void loadHistory();
	void saveRecentWorkingDirs();
	void loadRecentWorkingDirs();
	void saveLastWorkingDir();
	void loadLastWorkingDir();

	void SetFormatDecimals(int n) {
		cTerminal::SetFormatDecimals(n);
	}

	// cmd history-related fcns
	//
	sHistInfo addHistory(string);
	string getHistoryCmd(int ix);
	void decHistoryIdx();		// decrement history idx
	void incHistoryIdx();		// increment history idx
	int getHistIdx();
	int getHistSize();
	vector<string> getAllCmdHistory();
	vector<string> getCmdHistory(int nMax);
	void sortHistoryByAlpha();
	void sortHistoryByTime();

	bool checkForLocalEvents() {
		return !m_localEvents.empty();
	}

	// File I/O
	int GetNextFID();
	void RmFID( int fid );
	void AddFID( int fid, FILE *pFile );
	bool fileLookup(FILE *pf, int fid);
	void CloseAllFiles(void);
	void GetFileIds(vector<int>& vids);

	// awareness debugging
	//
	bool whichScript(string& ret, string& name);
	bool whichFcn(string& ret, string& name);
	vector<string> getAllScripts();
	vector<string> getAllFcns();
	//vector<string> getOpenFiles();
	vector<string> getStops();
	vector<string> getOpenFileIDs();		// files opened using fopen

	cInterpret *getInterpret() {
		return &m_interp;
	}


	// working dir mgmt
	//
	string getWorkingDir() { return m_WorkingDir; }
	bool setWorkingDir(string& dir) {
		sReportEvent re;
		string str, nowdir;
		//
		if (__CHDIR( dir.c_str() ) != 0) {
			re.bReport = true;
			re.iType = RPT_LIST;
			str = "destination directory does not exist";
			re.list.push_back( str );
			pushReport( re );
			return true;
		}
		// update current working dir
		char *pch = __GETCWD(NULL,MAX_PATH_BUFFER);
		m_WorkingDir = pch;
		addWorkingHistory( m_WorkingDir );
		return false;
	}
	void addWorkingHistory(string& dir);

	void pushReport(sReportEvent& re) { m_textReports.push_back(re); }
	void resetReportState() { m_textReports.clear(); }

	void flushReports();
	void printError();

	void Pause(void);
	bool checkForAbort() {
		return m_bAbort;
	}


	// Line parsing-related functions
	//
	void AddPasteCache( string& text );
	bool MultiLineManager(string& TypedString);
	bool DoTheWork(string& TypedString);
	bool ParseManager(string& TypedLine, bool bNotifyEditor);
	void RunCurrentFile(void);			// run file that is currently open in editor
	void RunSelection(void);			// run selected text in editor (in filename)
	void RunNextCommand(void);
	// string helpers
	void trimLeft(string& TypedLine);
	void trimRight(string& TypedLine);
	vector<string> splitLine(string& cmd);	// split a typed line into multiple statments, if necessary

	// output control wrapper
	void WindowPrintMatrix(string VarName, bool bShowName);
	void WindowPrintHash(cContainer::cUserHash& h) { return cTerminal::WindowPrintHash(h); }
	void WindowPrintMatrix(cMatrix& M, int iColumnsPerLine, int nWordSize, int nExp) { return cTerminal::WindowPrintMatrix(M, iColumnsPerLine, nWordSize, nExp); }
	void WindowPrintMatrix(cContainer& V) { return cTerminal::WindowPrintMatrix(V); }
	void WindowPrintMatrix(cMatrix& m) { return cTerminal::WindowPrintMatrix(m); }

	//void feedReports(sReportEvent& re, bool bShowName) { return cTerminal::feedReports(re, bShowName); }
	//void feedReports(vector<sReportEvent>& vre, bool bShowName) { return cTerminal::feedReports(vre, bShowName); }

	void feedReports(sReportEvent& re, bool bShowName);
	void feedReports(vector<sReportEvent>& vre, bool bShowName);

	// file mgmt related functions
	//
	bool IsValidSymbolName(string& Str);

	void printBlockError(sBlock& b);
};
