
#include "container.h"

bool sameBlock(const sBlock& f1, const sBlock& f2)
{
	if (f1.fileName == f2.fileName && f1.name == f2.name) {
		return true;
	}

	return false;
}

void clearBlock(sBlock& f)
{
	f.nLineOffset = 0;
	f.t_lastParsed = -1;
	f.nType = 0;
	f.stmts.clear();
	f.fileName.clear();
	f.pathName.clear();
	f.name.clear();
	f.inArgs.clear();
	f.outArgs.clear();
	f.outMsg.clear();
	f.bError = false;
}

// File I/O
//
bool WriteData(cFunctionPtr& v, FILE *pf)
{
	// NOP
	return false;
}

bool ReadData(cFunctionPtr& m, FILE *pf)
{
	// NOP
	return false;
}

