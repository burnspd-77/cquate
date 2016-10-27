
#pragma once

#include <vector>
#include <string>
#include <map>

//#define _WINDOWS_

#define MAX_PATH_BUFFER 2048

#ifdef _WINDOWS_
#include <io.h>
#include <direct.h>
#define __CHDIR _chdir
#define __GETCWD _getcwd
#else
#include <unistd.h>
#define __CHDIR chdir
#define __GETCWD getcwd
#endif

using std::vector;
using std::string;
using std::map;

#define		WM_HISTORY_CMD		(WM_USER + 1)
#define		WM_DB_GO			(WM_USER + 2)
#define		WM_DB_QUIT			(WM_USER + 3)
#define		WM_DB_OTHER			(WM_USER + 4)

#define CEDIT_WND_CLASS				_T("cedit_wnd_class")
#define CQUATE_WND_CLASS			_T("cquate_wnd_class")
#define PLOTTER_WND_CLASS			_T("plotter_wnd_class")

//#define _CQUATE_RELEASE_BUILD


typedef struct tagDBG_MSG
{
	int iLine;
	string fileName;
	string dirName;
} DBG_MSG;

typedef struct tagFindRecord
{
	int			nLine;
	int			nCol;
	string		strLine;
	string		strFileName;
} sFindRecord;

typedef struct tagOpenEvent
{
	string		fname;
} sOpenEvent;

typedef struct tagFileStops
{
	string		fname;
	vector<int>	stops;
} sFileStops;

typedef struct tagEvent
{
	int			iType;
	string		str;
	vector<int>	vi;
} sEvent;

typedef struct tagLocalEvent
{
	int	dwData;
	int iData;
	string strData;
} sLocalEvent;

typedef struct tagDebugFileInfo
{
	vector<int> stops;		// line no.
	int ip;					// instruction pointer line no. (if applicable)
} sDebugFileInfo;

typedef struct tagEditorState
{
	vector<string> open_files;
	string cur_file;
	map<string,sDebugFileInfo> debug_state;		// key = full filename, data = sDebugFileInfo
} sEditorState;


typedef struct tagEditFile
{
	string fullFile;
	int uid;
	vector<int> stops;
	int line_pos;
} sEditFile;


typedef struct tagEditEvent
{
	int iType;
	string strData;
	int iData;
} sEditEvent;


typedef struct tagHistInfo
{
	int month, day, year;
	int hour, min, sec;
	int totalSec;
	string date, time;
	string cmd;
} sHistInfo;


typedef struct tagFigure
{
	int uid;
	string name;
} sFigure;


typedef struct tagCommand
{
	int	iType;
	string strCmd;
} sCommand;


typedef struct tagReportEvent
{
	bool bReport;
	int iType;
	string varName;
	vector<string> list;
} sReportEvent;
