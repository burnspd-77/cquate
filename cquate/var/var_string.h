
#pragma once

#include "matrix.h"
#include "c_string.h"
#include "lexTypes.h"

//bool Append(cString& Result, cString& v1, cString& v2);
//bool Append(cString& Result, cString& v1, cString& v2, cString& v3);
//bool Append(cString& Result, vector<cString>& vars);
//
//bool FindStr(cMatrix& Result, cString& var, cString& query);
//bool FindStr(cMatrix& Result, cString& var, cString& query, cMatrix& start_pos);
//
//bool FindRev(cString& Result, cString& var, cString& query);
//bool FindRev(cString& Result, cString& var, cString& query, cMatrix& start_pos);
//
//bool Substr(cString& Result, cString& var, cMatrix& start_pos);
//bool Substr(cString& Result, cString& var, cMatrix& start_pos, cMatrix& num_chars);
//
//bool InsertStr(cString& Result, cString& var, cMatrix& vix, cString& ins);

bool FileStr(cString& Result, cString& fname);
bool GetLine(cString& Result, cString& vid);
