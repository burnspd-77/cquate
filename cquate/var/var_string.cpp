
#include "var_string.h"
#include "fb.h"
#include "state.h"
#include "c_string.h"
#include "state.h"

extern cState prog_state;
//
//bool Append(cString& Result, cString& v1, cString& v2)
//{
//	Result.Empty();
//	Result = v1;
//	Result.s.append(v2.s);
//
//	return false;
//}
//
//bool Append(cString& Result, cString& v1, cString& v2, cString& v3)
//{
//	Result = v1;
//	Result.s.append(v2.s);
//	Result.s.append(v3.s);
//
//	return false;
//}
//
//bool Append(cString& Result, vector<cString>& vars)
//{
//	int i;
//	Result.Empty();
//	if (vars.size()==0) return false;
//
//	for (i=0; i<vars.size(); ++i)
//	{
//		Result.s.append( vars[i].s );
//	}
//
//	return false;
//}
//
//bool FindStr(cMatrix& Result, cString& var, cString& query)
//{
//	Result.Empty();
//	if (Result.Create(1,1)) { addError("out of memory"); return true; }
//
//	size_t ix = var.s.find(query.s);
//	if (ix != string::npos) {
//		Result.mr[0] = double(ix);
//	}
//	else { Result.mr[0] = -1; }
//
//	return false;
//}
//
//bool FindStr(cMatrix& Result, cString& var, cString& query, cMatrix& start_pos)
//{
//	Result.Empty();
//
//	if (start_pos.GetSize() != 1) { addError("illegal argument"); return true; }
//	if (Result.Create(1,1)) { addError("out of memory"); return true; }
//
//	size_t istart = (size_t) start_pos.Get0R();
//
//	size_t ix = var.s.find(query.s, istart);
//	if (ix != string::npos) {
//		Result.mr[0] = double(ix);
//	}
//	else { Result.mr[0] = -1; }
//
//	return false;
//}
//
//bool FindRev(cMatrix& Result, cString& var, cString& query)
//{
//	Result.Empty();
//	if (Result.Create(1,1)) { addError("out of memory"); return true; }
//
//	size_t ix = var.s.find_last_of(query.s);
//	if (ix != string::npos) {
//		Result.mr[0] = double(ix);
//	}
//	else { Result.mr[0] = -1; }
//
//	return false;
//}
//
//bool FindRev(cMatrix& Result, cString& var, cString& query, cMatrix& start_pos)
//{
//	Result.Empty();
//
//	if (start_pos.GetSize() != 1) { addError("illegal argument"); return true; }
//	if (Result.Create(1,1)) { addError("out of memory"); return true; }
//
//	size_t stStart = (size_t) start_pos.mr[0];
//
//	size_t ix = var.s.rfind(query.s, stStart);
//	if (ix != string::npos) {
//		Result.mr[0] = double(ix);
//	}
//	else { Result.mr[0] = -1; }
//
//	return false;
//}
//
//bool Substr(cString& Result, cString& var, cMatrix& start_pos)
//{
//	Result.Empty();
//
//	if (start_pos.GetSize() != 1) { addError("illegal argument"); return true; }
//
//	int ix =  (int) start_pos.mr[0];
//	size_t stStart;
//	if (ix < 0)
//		stStart = string::npos;
//	else
//		stStart = (size_t) ix;
//
//	Result.s = var.s.substr(stStart);
//
//	return false;
//}
//
//bool Substr(cString& Result, cString& var, cMatrix& start_pos, cMatrix& num_chars)
//{
//	Result.Empty();
//
//	if (start_pos.GetSize() != 1 ||
//		num_chars.GetSize() != 1) { addError("illegal argument"); return true; }
//
//	int ix =  (int) start_pos.mr[0];
//	int nchars = (int) num_chars.mr[0];
//
//	size_t stStart, stNum;
//	if (ix < 0)
//		stStart = string::npos;
//	else
//		stStart = (size_t) ix;
//
//	if (nchars < 0)
//		stNum = string::npos;
//	else
//		stNum = (size_t) nchars;
//
//	Result.s = var.s.substr(stStart, stNum);
//
//	return false;
//}
//
//bool InsertStr(cString& Result, cString& var, cMatrix& vix, cString& ins)
//{
//	if (vix.GetSize() != 1) { addError("illegal argument"); return true; }
//
//	Result.Empty();
//
//	int ix =  (int) vix.mr[0];
//
//	size_t stIx;
//	if (ix < 0)
//		stIx = string::npos;
//	else
//		stIx = (size_t) ix;
//
//	Result = var;
//	Result.s.insert( stIx, ins.s );
//
//	return false;
//}

bool FileStr(cString& Result, cString& fname)
{
	Result.Empty();

	cFileBuffer f;
	if (f.Load( fname.s.c_str() )) { addError(f.strError); return true; }

	Result.s = f.begin;		// copy filebuf to Result
	f.Delete();				// clean up

	return false;
}

bool GetLine(cString& Result, cMatrix& vid)
{
	Result.Empty();
	if (vid.GetSize() != 1) { addError("illegal argument"); return true; }

	int fid = (int) vid.mr[0];

	return FGetline(Result.s, fid);
}


