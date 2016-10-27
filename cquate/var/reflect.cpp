
#include "state.h"
#include "variable.h"
#include "varUtil.h"

#ifdef __linux__
#include <sys/ioctl.h>
#include <unistd.h>
#endif
#include <stdio.h>

extern cState prog_state;
extern cStorage T;


// display to cmd line
bool QuietShow(cContainer& var)
{
	sReportEvent re;
	re.bReport = true;
	re.iType = RPT_MATRIX;
	re.varName = var.GetName();
	prog_state.pushReport( re );

	prog_state.feedReports( re, true );

	return false;
}

bool QuietShow(cMatrix& var)
{
	sReportEvent re;
	re.bReport = true;
	re.iType = RPT_MATRIX;
	re.varName = var.GetName();

	prog_state.feedReports( re, false );

	return false;
}


bool QuietShow(cCube& var)
{
	sReportEvent re;
	re.bReport = true;
	re.iType = RPT_MATRIX;
	re.varName = var.GetName();

	prog_state.feedReports( re, false );

	return false;
}

bool QuietShow(cString& var)
{
	string name = var.GetName();

	sReportEvent re;
	re.bReport = true;
	re.iType = RPT_MATRIX;
	if (name.empty())
		T.Set(name, var);

	prog_state.feedReports( re, false );

	return false;
}

bool QuietShow(cContainer::cUserHash& var)
{
	sReportEvent re;
	re.bReport = true;
	re.iType = RPT_MATRIX;
	re.varName = var.GetName();

	prog_state.feedReports( re, true );

	return false;
}

bool Show(cContainer& var)
{
	sReportEvent re;
	re.bReport = true;
	re.iType = RPT_MATRIX;
	re.varName = var.GetName();
	//prog_state.pushReport( re );

	prog_state.WindowPrintMatrix( var );

	return false;
}

bool Show(cMatrix& var)
{
	sReportEvent re;
	re.bReport = true;
	re.iType = RPT_MATRIX;

	prog_state.WindowPrintMatrix( var );

	return false;
}

bool Show(cCube& var)
{
	sReportEvent re;
	re.bReport = true;
	re.iType = RPT_MATRIX;
	re.varName = var.GetName();

	cContainer c;
	c.SetTo( var );

	prog_state.WindowPrintMatrix( c );

	return false;
}

bool Show(cString& var)
{
	string name = var.GetName();
	string out = name+" "+var.s;

	if (printf(var.s.c_str()) < 0) { addError("printf: error"); return true; }

	return false;
}

bool Show(cContainer::cUserHash& var)
{
	sReportEvent re;
	re.bReport = true;
	re.iType = RPT_MATRIX;
	re.varName = var.GetName();

	prog_state.WindowPrintHash( var );

	return false;
}

bool History()
{
	sReportEvent re;
	re.bReport = true;
	re.iType = RPT_LIST;

	re.list = prog_state.getCmdHistory(500);

	prog_state.pushReport( re );
	return false;
}


bool Which(cString& v)
{
	sReportEvent re;
	re.bReport = true;
	re.iType = RPT_LIST;

	string ret;
	
	if (prog_state.whichScript(ret, v.s)) {
		if (prog_state.whichFcn(ret, v.s)) {
			addError("no matching script or function found");
			return true;
		}
	}

	re.list.push_back( ret );
	prog_state.pushReport( re );
	return false;
}


bool Scripts()
{
	sReportEvent re;
	re.bReport = true;
	re.iType = RPT_LIST;

	re.list = prog_state.getAllScripts();

	prog_state.pushReport( re );
	return false;
}

bool Fcns()
{
	sReportEvent re;
	re.bReport = true;
	re.iType = RPT_LIST;

	re.list = prog_state.getAllFcns();

	prog_state.pushReport( re );
	return false;
}

bool OpenFileIds()
{
	sReportEvent re;
	re.bReport = true;
	re.iType = RPT_LIST;

	re.list = prog_state.getOpenFileIDs();
	
	prog_state.pushReport( re );

	return false;
}
