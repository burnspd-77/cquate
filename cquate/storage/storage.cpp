// storage.cpp: implementation of the cStorage class.
//
//////////////////////////////////////////////////////////////////////

#include "storage.h"
#include "variable.h"
#include "varFile.h"
#include "var_string.h"
#include "varUtil.h"
#include "matrix.h"
#include "state.h"
#include "defines.h"

// utility fcn
int str2fix(const char *pch)
{
	int nTypes = 4;
	int ix(0);

	for (int i=0; i<(int)strlen(pch); ++i)
	{
		ix = ix << 2;
		switch (pch[i])
		{
		case 'm':
			break;
		case 'c':
			ix += 1;
			break;
		case 's':
			ix += 2;
			break;
		case 'h':
			ix += 3;
			break;
		}
	}

	return ix;
}

//////////////////////////////////////////////////////////////////////
// cStorage Construction/Destruction
//////////////////////////////////////////////////////////////////////

cStorage::cStorage()
{
	m_bSafeMode = false;

	m_Spaces.clear();
	m_ixStackPos = 0;

	cVariableSpace spc;
	if (!m_bSafeMode) {
		//spc.m_pOpenFilesTable = &m_OpenFilesTable;
		m_Spaces.push_back( spc );		// begin with one clear space
	}

	GlobalFileIDCount = 0;				// this is an external static variable (see cVariableSpace.h)

	// init random number seed
	MersenneSeed( (unsigned int)112069 );

	m_ColorMapName = "rainbow";		// default colormap name

	vector<keyData> vkd;
	keyData key;
	cMyPair pairIn, pairOut;

	// initialize script table
	m_ScriptTable.initTable();

	vector<string> SystemCommands;
	vector<keyData> Functions, Commands, Language;
	keyData kd;

	// all defined functions
	//
	resetFcn( kd );


	// prog helpers
	kd.mat_2 = Length;	kd.mat_str = Length;	kd.mat_cbe_2 = Length;	kd.mat_hsh = Length;	kd.name = "length"; kd.help = "langref/elementary_functs/analysis/length.htm";
		pairIn.ix = str2fix("m"); pairIn.narg = 1; pairOut.ix = str2fix("m"); pairOut.narg = 1; kd.i2o[pairIn] = pairOut;
		pairIn.ix = str2fix("s"); pairIn.narg = 1; pairOut.ix = str2fix("m"); pairOut.narg = 1; kd.i2o[pairIn] = pairOut;
		pairIn.ix = str2fix("c"); pairIn.narg = 1; pairOut.ix = str2fix("m"); pairOut.narg = 1; kd.i2o[pairIn] = pairOut;
		pairIn.ix = str2fix("h"); pairIn.narg = 1; pairOut.ix = str2fix("m"); pairOut.narg = 1; kd.i2o[pairIn] = pairOut;
		Functions.push_back(kd);	resetFcn( kd );
	kd.mat_2 = Size;	kd.mat_str = Size;	kd.mat_cbe_2 = Size;	kd.mat_hsh = Size;			kd.name = "size"; kd.help = "langref/elementary_functs/analysis/size.htm";
		pairIn.ix = str2fix("m"); pairIn.narg = 1; pairOut.ix = str2fix("m"); pairOut.narg = 1; kd.i2o[pairIn] = pairOut;
		pairIn.ix = str2fix("s"); pairIn.narg = 1; pairOut.ix = str2fix("m"); pairOut.narg = 1; kd.i2o[pairIn] = pairOut;
		pairIn.ix = str2fix("c"); pairIn.narg = 1; pairOut.ix = str2fix("m"); pairOut.narg = 1; kd.i2o[pairIn] = pairOut;
		pairIn.ix = str2fix("h"); pairIn.narg = 1; pairOut.ix = str2fix("m"); pairOut.narg = 1; kd.i2o[pairIn] = pairOut;
		Functions.push_back(kd);	resetFcn( kd );
	kd.mat_2 = Rows;	kd.mat_cbe_2 = Rows;		kd.name = "rows"; kd.help = "langref/elementary_functs/analysis/rows.htm";
		pairIn.ix = str2fix("m"); pairIn.narg = 1; pairOut.ix = str2fix("m"); pairOut.narg = 1; kd.i2o[pairIn] = pairOut;
		pairIn.ix = str2fix("c"); pairIn.narg = 1; pairOut.ix = str2fix("m"); pairOut.narg = 1; kd.i2o[pairIn] = pairOut;
		Functions.push_back(kd);	resetFcn( kd );
	kd.mat_2 = Columns;	kd.mat_cbe_2 = Columns;		kd.name = "columns"; kd.help = "langref/elementary_functs/analysis/columns.htm";
		pairIn.ix = str2fix("m"); pairIn.narg = 1; pairOut.ix = str2fix("m"); pairOut.narg = 1; kd.i2o[pairIn] = pairOut;
		pairIn.ix = str2fix("c"); pairIn.narg = 1; pairOut.ix = str2fix("m"); pairOut.narg = 1; kd.i2o[pairIn] = pairOut;
		Functions.push_back(kd);	resetFcn( kd );
	//Functions.push_back("size");

	// time
	kd.mat_1 = TimeLapse;		kd.name = "time_lapse";	kd.help = "";
		pairIn.ix = str2fix("m"); pairIn.narg = 0; pairOut.ix = str2fix("m"); pairOut.narg = 1; kd.i2o[pairIn] = pairOut;
		Functions.push_back(kd);	resetFcn( kd );

	// basic math
	//kd.mat_3 = Add;		kd.name = "add";	kd.help = "";
	//	pairIn.ix = str2fix("mm"); pairIn.narg = 2; pairOut.ix = str2fix("m"); pairOut.narg = 1; kd.i2o[pairIn] = pairOut;
	//	Functions.push_back(kd);	resetFcn( kd );
	kd.mat_3 = Sub;		kd.name = "sub";	kd.help = "";
		pairIn.ix = str2fix("mm"); pairIn.narg = 2; pairOut.ix = str2fix("m"); pairOut.narg = 1; kd.i2o[pairIn] = pairOut;
		Functions.push_back(kd);	resetFcn( kd );

	// trigonometric
	kd.mat_2 = sin;			kd.cbe_cbe_2 = sin;		kd.name = "sin";	kd.help = "langref/elementary_functs/trig/sin.htm";
		pairIn.ix = str2fix("m"); pairIn.narg = 1; pairOut.ix = str2fix("m"); pairOut.narg = 1; kd.i2o[pairIn] = pairOut;
		pairIn.ix = str2fix("c"); pairIn.narg = 1; pairOut.ix = str2fix("c"); pairOut.narg = 1; kd.i2o[pairIn] = pairOut;
		Functions.push_back(kd);	resetFcn( kd );
	kd.mat_2 = cos;			kd.cbe_cbe_2 = cos;		kd.name = "cos";	kd.help = "langref/elementary_functs/trig/cos.htm";
		pairIn.ix = str2fix("m"); pairIn.narg = 1; pairOut.ix = str2fix("m"); pairOut.narg = 1; kd.i2o[pairIn] = pairOut;
		pairIn.ix = str2fix("c"); pairIn.narg = 1; pairOut.ix = str2fix("c"); pairOut.narg = 1; kd.i2o[pairIn] = pairOut;
		Functions.push_back(kd);	resetFcn( kd );
	kd.mat_2 = tan;			kd.cbe_cbe_2 = tan;		kd.name = "tan";	kd.help = "langref/elementary_functs/trig/tan.htm";
		pairIn.ix = str2fix("m"); pairIn.narg = 1; pairOut.ix = str2fix("m"); pairOut.narg = 1; kd.i2o[pairIn] = pairOut;
		pairIn.ix = str2fix("c"); pairIn.narg = 1; pairOut.ix = str2fix("c"); pairOut.narg = 1; kd.i2o[pairIn] = pairOut;
		Functions.push_back(kd);	resetFcn( kd );
	kd.mat_2 = asin;		kd.cbe_cbe_2 = asin;	kd.name = "asin";	kd.help = "langref/elementary_functs/trig/asin.htm";
		pairIn.ix = str2fix("m"); pairIn.narg = 1; pairOut.ix = str2fix("m"); pairOut.narg = 1; kd.i2o[pairIn] = pairOut;
		pairIn.ix = str2fix("c"); pairIn.narg = 1; pairOut.ix = str2fix("m"); pairOut.narg = 1; kd.i2o[pairIn] = pairOut;
		Functions.push_back(kd);	resetFcn( kd );
	kd.mat_2 = acos;		kd.cbe_cbe_2 = acos;	kd.name = "acos";	kd.help = "langref/elementary_functs/trig/acos.htm";
		pairIn.ix = str2fix("m"); pairIn.narg = 1; pairOut.ix = str2fix("m"); pairOut.narg = 1; kd.i2o[pairIn] = pairOut;
		pairIn.ix = str2fix("c"); pairIn.narg = 1; pairOut.ix = str2fix("c"); pairOut.narg = 1; kd.i2o[pairIn] = pairOut;
		Functions.push_back(kd);	resetFcn( kd );
	kd.mat_2 = atan;		kd.cbe_cbe_2 = atan;	kd.name = "atan";	kd.help = "langref/elementary_functs/trig/atan.htm";
		pairIn.ix = str2fix("m"); pairIn.narg = 1; pairOut.ix = str2fix("m"); pairOut.narg = 1; kd.i2o[pairIn] = pairOut;
		pairIn.ix = str2fix("c"); pairIn.narg = 1; pairOut.ix = str2fix("c"); pairOut.narg = 1; kd.i2o[pairIn] = pairOut;
		Functions.push_back(kd);	resetFcn( kd );
	kd.mat_2 = csc;			kd.cbe_cbe_2 = csc;		kd.name = "csc";	kd.help = "langref/elementary_functs/trig/csc.htm";
		pairIn.ix = str2fix("m"); pairIn.narg = 1; pairOut.ix = str2fix("m"); pairOut.narg = 1; kd.i2o[pairIn] = pairOut;
		pairIn.ix = str2fix("c"); pairIn.narg = 1; pairOut.ix = str2fix("c"); pairOut.narg = 1; kd.i2o[pairIn] = pairOut;
		Functions.push_back(kd);	resetFcn( kd );
	kd.mat_2 = sec;			kd.cbe_cbe_2 = sec;		kd.name = "sec";	kd.help = "langref/elementary_functs/trig/sec.htm";
		pairIn.ix = str2fix("m"); pairIn.narg = 1; pairOut.ix = str2fix("m"); pairOut.narg = 1; kd.i2o[pairIn] = pairOut;
		pairIn.ix = str2fix("c"); pairIn.narg = 1; pairOut.ix = str2fix("c"); pairOut.narg = 1; kd.i2o[pairIn] = pairOut;
		Functions.push_back(kd);	resetFcn( kd );
	kd.mat_2 = cot;			kd.cbe_cbe_2 = cot;		kd.name = "cot";	kd.help = "langref/elementary_functs/trig/cot.htm";
		pairIn.ix = str2fix("m"); pairIn.narg = 1; pairOut.ix = str2fix("m"); pairOut.narg = 1; kd.i2o[pairIn] = pairOut;
		pairIn.ix = str2fix("c"); pairIn.narg = 1; pairOut.ix = str2fix("c"); pairOut.narg = 1; kd.i2o[pairIn] = pairOut;
		Functions.push_back(kd);	resetFcn( kd );
	kd.mat_2 = acsc;		kd.cbe_cbe_2 = acsc;	kd.name = "acsc";	kd.help = "langref/elementary_functs/trig/acsc.htm";
		pairIn.ix = str2fix("m"); pairIn.narg = 1; pairOut.ix = str2fix("m"); pairOut.narg = 1; kd.i2o[pairIn] = pairOut;
		pairIn.ix = str2fix("c"); pairIn.narg = 1; pairOut.ix = str2fix("c"); pairOut.narg = 1; kd.i2o[pairIn] = pairOut;
		Functions.push_back(kd);	resetFcn( kd );
	kd.mat_2 = asec;		kd.cbe_cbe_2 = asec;	kd.name = "asec";	kd.help = "langref/elementary_functs/trig/asec.htm";
		pairIn.ix = str2fix("m"); pairIn.narg = 1; pairOut.ix = str2fix("m"); pairOut.narg = 1; kd.i2o[pairIn] = pairOut;
		pairIn.ix = str2fix("c"); pairIn.narg = 1; pairOut.ix = str2fix("c"); pairOut.narg = 1; kd.i2o[pairIn] = pairOut;
		Functions.push_back(kd);	resetFcn( kd );
	kd.mat_2 = acot;		kd.cbe_cbe_2 = acot;	kd.name = "acot";	kd.help = "langref/elementary_functs/trig/acot.htm";
		pairIn.ix = str2fix("m"); pairIn.narg = 1; pairOut.ix = str2fix("m"); pairOut.narg = 1; kd.i2o[pairIn] = pairOut;
		pairIn.ix = str2fix("c"); pairIn.narg = 1; pairOut.ix = str2fix("c"); pairOut.narg = 1; kd.i2o[pairIn] = pairOut;
		Functions.push_back(kd);	resetFcn( kd );
	kd.mat_3 = Atan2;		kd.cbe_cbe_3 = Atan2;	kd.name = "atan2";	kd.help = "langref/elementary_functs/trig/atan2.htm";
		pairIn.ix = str2fix("mm"); pairIn.narg = 2; pairOut.ix = str2fix("m"); pairOut.narg = 1; kd.i2o[pairIn] = pairOut;
		pairIn.ix = str2fix("cc"); pairIn.narg = 2; pairOut.ix = str2fix("c"); pairOut.narg = 1; kd.i2o[pairIn] = pairOut;
		Functions.push_back(kd);	resetFcn( kd );
	// hyperbolic
	kd.mat_2 = sinh;	kd.cbe_cbe_2 = sinh;	kd.name = "sinh";	kd.help = "langref/elementary_functs/trig/sinh.htm";
		pairIn.ix = str2fix("m"); pairIn.narg = 1; pairOut.ix = str2fix("m"); pairOut.narg = 1; kd.i2o[pairIn] = pairOut;
		pairIn.ix = str2fix("c"); pairIn.narg = 1; pairOut.ix = str2fix("c"); pairOut.narg = 1; kd.i2o[pairIn] = pairOut;
		Functions.push_back(kd);	resetFcn( kd );
	kd.mat_2 = cosh;	kd.cbe_cbe_2 = cosh;	kd.name = "cosh";	kd.help = "langref/elementary_functs/trig/cosh.htm";
		pairIn.ix = str2fix("m"); pairIn.narg = 1; pairOut.ix = str2fix("m"); pairOut.narg = 1; kd.i2o[pairIn] = pairOut;
		pairIn.ix = str2fix("c"); pairIn.narg = 1; pairOut.ix = str2fix("c"); pairOut.narg = 1; kd.i2o[pairIn] = pairOut;
		Functions.push_back(kd);	resetFcn( kd );
	kd.mat_2 = tanh;	kd.cbe_cbe_2 = tanh;	kd.name = "tanh";	kd.help = "langref/elementary_functs/trig/tanh.htm";
		pairIn.ix = str2fix("m"); pairIn.narg = 1; pairOut.ix = str2fix("m"); pairOut.narg = 1; kd.i2o[pairIn] = pairOut;
		pairIn.ix = str2fix("c"); pairIn.narg = 1; pairOut.ix = str2fix("c"); pairOut.narg = 1; kd.i2o[pairIn] = pairOut;
		Functions.push_back(kd);	resetFcn( kd );
	kd.mat_2 = asinh;	kd.cbe_cbe_2 = asinh;	kd.name = "asinh";	kd.help = "langref/elementary_functs/trig/asinh.htm";
		pairIn.ix = str2fix("m"); pairIn.narg = 1; pairOut.ix = str2fix("m"); pairOut.narg = 1; kd.i2o[pairIn] = pairOut;
		pairIn.ix = str2fix("c"); pairIn.narg = 1; pairOut.ix = str2fix("c"); pairOut.narg = 1; kd.i2o[pairIn] = pairOut;
		Functions.push_back(kd);	resetFcn( kd );
	kd.mat_2 = acosh;	kd.cbe_cbe_2 = acosh;	kd.name = "acosh";	kd.help = "langref/elementary_functs/trig/acosh.htm";
		pairIn.ix = str2fix("m"); pairIn.narg = 1; pairOut.ix = str2fix("m"); pairOut.narg = 1; kd.i2o[pairIn] = pairOut;
		pairIn.ix = str2fix("c"); pairIn.narg = 1; pairOut.ix = str2fix("c"); pairOut.narg = 1; kd.i2o[pairIn] = pairOut;
		Functions.push_back(kd);	resetFcn( kd );
	kd.mat_2 = atanh;	kd.cbe_cbe_2 = atanh;	kd.name = "atanh";	kd.help = "langref/elementary_functs/trig/atanh.htm";
		pairIn.ix = str2fix("m"); pairIn.narg = 1; pairOut.ix = str2fix("m"); pairOut.narg = 1; kd.i2o[pairIn] = pairOut;
		pairIn.ix = str2fix("c"); pairIn.narg = 1; pairOut.ix = str2fix("c"); pairOut.narg = 1; kd.i2o[pairIn] = pairOut;
		Functions.push_back(kd);	resetFcn( kd );
	kd.mat_2 = csch;	kd.cbe_cbe_2 = csch;	kd.name = "csch";	kd.help = "langref/elementary_functs/trig/csch.htm";
		pairIn.ix = str2fix("m"); pairIn.narg = 1; pairOut.ix = str2fix("m"); pairOut.narg = 1; kd.i2o[pairIn] = pairOut;
		pairIn.ix = str2fix("c"); pairIn.narg = 1; pairOut.ix = str2fix("c"); pairOut.narg = 1; kd.i2o[pairIn] = pairOut;
		Functions.push_back(kd);	resetFcn( kd );
	kd.mat_2 = sech;	kd.cbe_cbe_2 = sech;	kd.name = "sech";	kd.help = "langref/elementary_functs/trig/sech.htm";
		pairIn.ix = str2fix("m"); pairIn.narg = 1; pairOut.ix = str2fix("m"); pairOut.narg = 1; kd.i2o[pairIn] = pairOut;
		pairIn.ix = str2fix("c"); pairIn.narg = 1; pairOut.ix = str2fix("c"); pairOut.narg = 1; kd.i2o[pairIn] = pairOut;
		Functions.push_back(kd);	resetFcn( kd );
	kd.mat_2 = coth;	kd.cbe_cbe_2 = coth;	kd.name = "coth";	kd.help = "langref/elementary_functs/trig/coth.htm";
		pairIn.ix = str2fix("m"); pairIn.narg = 1; pairOut.ix = str2fix("m"); pairOut.narg = 1; kd.i2o[pairIn] = pairOut;
		pairIn.ix = str2fix("c"); pairIn.narg = 1; pairOut.ix = str2fix("c"); pairOut.narg = 1; kd.i2o[pairIn] = pairOut;
		Functions.push_back(kd);	resetFcn( kd );
	kd.mat_2 = acsch;	kd.cbe_cbe_2 = acsch;	kd.name = "acsch";	kd.help = "langref/elementary_functs/trig/acsch.htm";
		pairIn.ix = str2fix("m"); pairIn.narg = 1; pairOut.ix = str2fix("m"); pairOut.narg = 1; kd.i2o[pairIn] = pairOut;
		pairIn.ix = str2fix("c"); pairIn.narg = 1; pairOut.ix = str2fix("c"); pairOut.narg = 1; kd.i2o[pairIn] = pairOut;
		Functions.push_back(kd);	resetFcn( kd );
	kd.mat_2 = asech;	kd.cbe_cbe_2 = asech;	kd.name = "asech";	kd.help = "langref/elementary_functs/trig/asech.htm";
		pairIn.ix = str2fix("m"); pairIn.narg = 1; pairOut.ix = str2fix("m"); pairOut.narg = 1; kd.i2o[pairIn] = pairOut;
		pairIn.ix = str2fix("c"); pairIn.narg = 1; pairOut.ix = str2fix("c"); pairOut.narg = 1; kd.i2o[pairIn] = pairOut;
		Functions.push_back(kd);	resetFcn( kd );
	kd.mat_2 = acoth;	kd.cbe_cbe_2 = acoth;	kd.name = "acoth";	kd.help = "langref/elementary_functs/trig/acoth.htm";
		pairIn.ix = str2fix("m"); pairIn.narg = 1; pairOut.ix = str2fix("m"); pairOut.narg = 1; kd.i2o[pairIn] = pairOut;
		pairIn.ix = str2fix("c"); pairIn.narg = 1; pairOut.ix = str2fix("c"); pairOut.narg = 1; kd.i2o[pairIn] = pairOut;
		Functions.push_back(kd);	resetFcn( kd );
	//// logarithms
	kd.mat_2 = log;		kd.cbe_cbe_2 = log;		kd.name = "log";	kd.help = "langref/elementary_functs/exp/log.htm";
		pairIn.ix = str2fix("m"); pairIn.narg = 1; pairOut.ix = str2fix("m"); pairOut.narg = 1; kd.i2o[pairIn] = pairOut;
		pairIn.ix = str2fix("c"); pairIn.narg = 1; pairOut.ix = str2fix("c"); pairOut.narg = 1; kd.i2o[pairIn] = pairOut;
		Functions.push_back(kd);	resetFcn( kd );
	kd.mat_2 = log10;	kd.cbe_cbe_2 = log10;	kd.name = "log10";	kd.help = "langref/elementary_functs/exp/log.htm";
		pairIn.ix = str2fix("m"); pairIn.narg = 1; pairOut.ix = str2fix("m"); pairOut.narg = 1; kd.i2o[pairIn] = pairOut;
		pairIn.ix = str2fix("c"); pairIn.narg = 1; pairOut.ix = str2fix("c"); pairOut.narg = 1; kd.i2o[pairIn] = pairOut;
		Functions.push_back(kd);	resetFcn( kd );
	kd.mat_2 = log2;	kd.cbe_cbe_2 = log2;	kd.name = "log2";	kd.help = "langref/elementary_functs/exp/log.htm";
		pairIn.ix = str2fix("m"); pairIn.narg = 1; pairOut.ix = str2fix("m"); pairOut.narg = 1; kd.i2o[pairIn] = pairOut;
		pairIn.ix = str2fix("c"); pairIn.narg = 1; pairOut.ix = str2fix("c"); pairOut.narg = 1; kd.i2o[pairIn] = pairOut;
		Functions.push_back(kd);	resetFcn( kd );
	kd.mat_2 = sqrt;	kd.cbe_cbe_2 = sqrt;	kd.name = "sqrt";	kd.help = "langref/elementary_functs/exp/sqrt.htm";
		pairIn.ix = str2fix("m"); pairIn.narg = 1; pairOut.ix = str2fix("m"); pairOut.narg = 1; kd.i2o[pairIn] = pairOut;
		pairIn.ix = str2fix("c"); pairIn.narg = 1; pairOut.ix = str2fix("c"); pairOut.narg = 1; kd.i2o[pairIn] = pairOut;
		Functions.push_back(kd);	resetFcn( kd );
	kd.mat_2 = exp;		kd.cbe_cbe_2 = exp;		kd.name = "exp";	kd.help = "langref/elementary_functs/exp/exp.htm";
		pairIn.ix = str2fix("m"); pairIn.narg = 1; pairOut.ix = str2fix("m"); pairOut.narg = 1; kd.i2o[pairIn] = pairOut;
		pairIn.ix = str2fix("c"); pairIn.narg = 1; pairOut.ix = str2fix("c"); pairOut.narg = 1; kd.i2o[pairIn] = pairOut;
		Functions.push_back(kd);	resetFcn( kd );
	//// special functions
	kd.mat_2 = erf;		kd.cbe_cbe_2 = erf;		kd.name = "erf";	kd.help = "langref/elementary_functs/special/erf.htm";
		pairIn.ix = str2fix("m"); pairIn.narg = 1; pairOut.ix = str2fix("m"); pairOut.narg = 1; kd.i2o[pairIn] = pairOut;
		pairIn.ix = str2fix("c"); pairIn.narg = 1; pairOut.ix = str2fix("c"); pairOut.narg = 1; kd.i2o[pairIn] = pairOut;
		Functions.push_back(kd);	resetFcn( kd );
	kd.mat_2 = erfc;	kd.cbe_cbe_2 = erfc;	kd.name = "erfc";	kd.help = "langref/elementary_functs/special/erfc.htm";
		pairIn.ix = str2fix("m"); pairIn.narg = 1; pairOut.ix = str2fix("m"); pairOut.narg = 1; kd.i2o[pairIn] = pairOut;
		pairIn.ix = str2fix("c"); pairIn.narg = 1; pairOut.ix = str2fix("c"); pairOut.narg = 1; kd.i2o[pairIn] = pairOut;
		Functions.push_back(kd);	resetFcn( kd );
	kd.mat_3 = besseljn;	kd.cbe_cbe_3 = besseljn;	kd.name = "bessel_jn";	kd.help = "langref/elementary_functs/special/bessel_jn.htm";
		pairIn.ix = str2fix("mm"); pairIn.narg = 2; pairOut.ix = str2fix("m"); pairOut.narg = 1; kd.i2o[pairIn] = pairOut;
		pairIn.ix = str2fix("cc"); pairIn.narg = 2; pairOut.ix = str2fix("c"); pairOut.narg = 1; kd.i2o[pairIn] = pairOut;
		Functions.push_back(kd);	resetFcn( kd );
	kd.mat_3 = besselyn;	kd.cbe_cbe_3 = besselyn;	kd.name = "bessel_yn";	kd.help = "langref/elementary_functs/special/bessel_yn.htm";
		pairIn.ix = str2fix("mm"); pairIn.narg = 2; pairOut.ix = str2fix("m"); pairOut.narg = 1; kd.i2o[pairIn] = pairOut;
		pairIn.ix = str2fix("cc"); pairIn.narg = 2; pairOut.ix = str2fix("c"); pairOut.narg = 1; kd.i2o[pairIn] = pairOut;
		Functions.push_back(kd);	resetFcn( kd );
	kd.mat_2 = lgamma;	kd.cbe_cbe_2 = lgamma;	kd.name = "log_gamma";	kd.help = "langref/elementary_functs/special/log_gamma.htm";
		pairIn.ix = str2fix("m"); pairIn.narg = 1; pairOut.ix = str2fix("m"); pairOut.narg = 1; kd.i2o[pairIn] = pairOut;
		pairIn.ix = str2fix("c"); pairIn.narg = 1; pairOut.ix = str2fix("c"); pairOut.narg = 1; kd.i2o[pairIn] = pairOut;
		Functions.push_back(kd);	resetFcn( kd );
	kd.mat_2 = gamma;	kd.cbe_cbe_2 = gamma;	kd.name = "gamma";		kd.help = "langref/elementary_functs/special/gamma.htm";
		pairIn.ix = str2fix("m"); pairIn.narg = 1; pairOut.ix = str2fix("m"); pairOut.narg = 1; kd.i2o[pairIn] = pairOut;
		pairIn.ix = str2fix("c"); pairIn.narg = 1; pairOut.ix = str2fix("c"); pairOut.narg = 1; kd.i2o[pairIn] = pairOut;
		Functions.push_back(kd);	resetFcn( kd );
	kd.mat_2 = Factorial;	kd.cbe_cbe_2 = Factorial;	kd.name = "factorial";	kd.help = "langref/elementary_functs/special/factorial.htm";
		pairIn.ix = str2fix("m"); pairIn.narg = 1; pairOut.ix = str2fix("m"); pairOut.narg = 1; kd.i2o[pairIn] = pairOut;
		pairIn.ix = str2fix("c"); pairIn.narg = 1; pairOut.ix = str2fix("c"); pairOut.narg = 1; kd.i2o[pairIn] = pairOut;
		Functions.push_back(kd);	resetFcn( kd );

	//// misc
	kd.mat_2 = Real;	kd.cbe_cbe_2 = Real;	kd.name = "real";	kd.help = "langref/elementary_functs/analysis/real.htm";
		pairIn.ix = str2fix("m"); pairIn.narg = 1; pairOut.ix = str2fix("m"); pairOut.narg = 1; kd.i2o[pairIn] = pairOut;
		pairIn.ix = str2fix("c"); pairIn.narg = 1; pairOut.ix = str2fix("c"); pairOut.narg = 1; kd.i2o[pairIn] = pairOut;
		Functions.push_back(kd);	resetFcn( kd );
	kd.mat_2 = Imag;	kd.cbe_cbe_2 = Imag;	kd.name = "imag";	kd.help = "langref/elementary_functs/analysis/imag.htm";
		pairIn.ix = str2fix("m"); pairIn.narg = 1; pairOut.ix = str2fix("m"); pairOut.narg = 1; kd.i2o[pairIn] = pairOut;
		pairIn.ix = str2fix("c"); pairIn.narg = 1; pairOut.ix = str2fix("c"); pairOut.narg = 1; kd.i2o[pairIn] = pairOut;
		Functions.push_back(kd);	resetFcn( kd );
	kd.mat_2 = Conj;	kd.cbe_cbe_2 = Conj;	kd.name = "conj";	kd.help = "langref/elementary_functs/analysis/conj.htm";
		pairIn.ix = str2fix("m"); pairIn.narg = 1; pairOut.ix = str2fix("m"); pairOut.narg = 1; kd.i2o[pairIn] = pairOut;
		pairIn.ix = str2fix("c"); pairIn.narg = 1; pairOut.ix = str2fix("c"); pairOut.narg = 1; kd.i2o[pairIn] = pairOut;
		Functions.push_back(kd);	resetFcn( kd );
	kd.mat_2 = Round;		kd.cbe_cbe_2 = Round;		kd.name = "round";	kd.help = "langref/elementary_functs/analysis/round.htm";
		pairIn.ix = str2fix("m"); pairIn.narg = 1; pairOut.ix = str2fix("m"); pairOut.narg = 1; kd.i2o[pairIn] = pairOut;
		pairIn.ix = str2fix("c"); pairIn.narg = 1; pairOut.ix = str2fix("c"); pairOut.narg = 1; kd.i2o[pairIn] = pairOut;
		Functions.push_back(kd);	resetFcn( kd );
	kd.mat_2 = Fix;		kd.cbe_cbe_2 = Fix;		kd.name = "trunc";	kd.help = "langref/elementary_functs/analysis/trunc.htm";
		pairIn.ix = str2fix("m"); pairIn.narg = 1; pairOut.ix = str2fix("m"); pairOut.narg = 1; kd.i2o[pairIn] = pairOut;
		pairIn.ix = str2fix("c"); pairIn.narg = 1; pairOut.ix = str2fix("c"); pairOut.narg = 1; kd.i2o[pairIn] = pairOut;
		Functions.push_back(kd);	resetFcn( kd );
	kd.mat_2 = Floor;	kd.cbe_cbe_2 = Floor;	kd.name = "floor";	kd.help = "langref/elementary_functs/analysis/floor.htm";
		pairIn.ix = str2fix("m"); pairIn.narg = 1; pairOut.ix = str2fix("m"); pairOut.narg = 1; kd.i2o[pairIn] = pairOut;
		pairIn.ix = str2fix("c"); pairIn.narg = 1; pairOut.ix = str2fix("c"); pairOut.narg = 1; kd.i2o[pairIn] = pairOut;
		Functions.push_back(kd);	resetFcn( kd );
	kd.mat_2 = Ceil;	kd.cbe_cbe_2 = Ceil;	kd.name = "ceil";	kd.help = "langref/elementary_functs/analysis/ceil.htm";
		pairIn.ix = str2fix("m"); pairIn.narg = 1; pairOut.ix = str2fix("m"); pairOut.narg = 1; kd.i2o[pairIn] = pairOut;
		pairIn.ix = str2fix("c"); pairIn.narg = 1; pairOut.ix = str2fix("c"); pairOut.narg = 1; kd.i2o[pairIn] = pairOut;
		Functions.push_back(kd);	resetFcn( kd );
	kd.mat_2 = Phase;	kd.cbe_cbe_2 = Phase;	kd.name = "angle";	kd.help = "langref/elementary_functs/analysis/angle.htm";
		pairIn.ix = str2fix("m"); pairIn.narg = 1; pairOut.ix = str2fix("m"); pairOut.narg = 1; kd.i2o[pairIn] = pairOut;
		pairIn.ix = str2fix("c"); pairIn.narg = 1; pairOut.ix = str2fix("c"); pairOut.narg = 1; kd.i2o[pairIn] = pairOut;
		Functions.push_back(kd);	resetFcn( kd );
	kd.mat_2 = Trans;	kd.name = "trans";	kd.help = "langref/elementary_functs/analysis/trans.htm";
		pairIn.ix = str2fix("m"); pairIn.narg = 1; pairOut.ix = str2fix("m"); pairOut.narg = 1; kd.i2o[pairIn] = pairOut;
		Functions.push_back(kd);	resetFcn( kd );
	kd.mat_2 = Herm;	kd.name = "herm";	kd.help = "langref/elementary_functs/analysis/herm.htm";
		pairIn.ix = str2fix("m"); pairIn.narg = 1; pairOut.ix = str2fix("m"); pairOut.narg = 1; kd.i2o[pairIn] = pairOut;
		Functions.push_back(kd);	resetFcn( kd );
		
	kd.mat_2 = Flipud;	kd.name = "revud";		kd.help = "langref/elementary_functs/analysis/revud.htm";
		pairIn.ix = str2fix("m"); pairIn.narg = 1; pairOut.ix = str2fix("m"); pairOut.narg = 1; kd.i2o[pairIn] = pairOut;
		Functions.push_back(kd);	resetFcn( kd );
	kd.mat_2 = Fliplr;	kd.name = "revlr";		kd.help = "langref/elementary_functs/analysis/revlr.htm";
		pairIn.ix = str2fix("m"); pairIn.narg = 1; pairOut.ix = str2fix("m"); pairOut.narg = 1; kd.i2o[pairIn] = pairOut;
		Functions.push_back(kd);	resetFcn( kd );

	// new matrix construction
	kd.mat_2 = NewMat;	kd.mat_3 = NewMat;	kd.cbe_4 = NewCube; kd.name = "new";	kd.help = "langref/misc/new.htm";
		pairIn.ix = str2fix("m"); pairIn.narg = 1; pairOut.ix = str2fix("m"); pairOut.narg = 1; kd.i2o[pairIn] = pairOut;
		pairIn.ix = str2fix("mm"); pairIn.narg = 2; pairOut.ix = str2fix("m"); pairOut.narg = 1; kd.i2o[pairIn] = pairOut;
		pairIn.ix = str2fix("mmm"); pairIn.narg = 3; pairOut.ix = str2fix("c"); pairOut.narg = 1; kd.i2o[pairIn] = pairOut;
		Functions.push_back(kd);	resetFcn( kd );
	kd.hsh_1 = NewHash;	kd.name = "new_hash";	kd.help = "langref/misc/new_hash.htm";
		pairIn.ix = 0; pairIn.narg = 0; pairOut.ix = str2fix("h"); pairOut.narg = 1; kd.i2o[pairIn] = pairOut;
		Functions.push_back(kd);	resetFcn( kd );
	kd.mat_2 = NewDiag;	kd.mat_3 = NewDiag;	kd.mat_4 = NewDiag;	kd.name = "new_diag";	kd.help = "langref/misc/new_diag.htm";
		pairIn.ix = str2fix("m"); pairIn.narg = 1; pairOut.ix = str2fix("m"); pairOut.narg = 1; kd.i2o[pairIn] = pairOut;
		pairIn.ix = str2fix("mm"); pairIn.narg = 2; pairOut.ix = str2fix("m"); pairOut.narg = 1; kd.i2o[pairIn] = pairOut;
		pairIn.ix = str2fix("mmm"); pairIn.narg = 3; pairOut.ix = str2fix("m"); pairOut.narg = 1; kd.i2o[pairIn] = pairOut;
		Functions.push_back(kd);	resetFcn( kd );
	kd.mat_2 = NewIdentity;	kd.mat_3 = NewIdentity;	kd.name = "identity";			kd.help = "langref/misc/identity.htm";
		pairIn.ix = str2fix("m"); pairIn.narg = 1; pairOut.ix = str2fix("m"); pairOut.narg = 1; kd.i2o[pairIn] = pairOut;
		pairIn.ix = str2fix("mm"); pairIn.narg = 2; pairOut.ix = str2fix("m"); pairOut.narg = 1; kd.i2o[pairIn] = pairOut;
		Functions.push_back(kd);	resetFcn( kd );
	kd.mat_4 = Repeat;			kd.name = "repeat";			kd.help = "langref/misc/repeat.htm";
		pairIn.ix = str2fix("mmm"); pairIn.narg = 3; pairOut.ix = str2fix("m"); pairOut.narg = 1; kd.i2o[pairIn] = pairOut;
		Functions.push_back(kd);	resetFcn( kd );

	kd.mat_2 = Min;		kd.mat_3 = Min;			kd.name = "min";	kd.help = "langref/elementary_functs/analysis/min.htm";
		pairIn.ix = str2fix("m"); pairIn.narg = 1; pairOut.ix = str2fix("m"); pairOut.narg = 1; kd.i2o[pairIn] = pairOut;
		pairIn.ix = str2fix("mm"); pairIn.narg = 2; pairOut.ix = str2fix("m"); pairOut.narg = 1; kd.i2o[pairIn] = pairOut;
		Functions.push_back(kd);	resetFcn( kd );
	kd.mat_2 = Max;		kd.mat_3 = Max;			kd.name = "max";	kd.help = "langref/elementary_functs/analysis/max.htm";
		pairIn.ix = str2fix("m"); pairIn.narg = 1; pairOut.ix = str2fix("m"); pairOut.narg = 1; kd.i2o[pairIn] = pairOut;
		pairIn.ix = str2fix("mm"); pairIn.narg = 2; pairOut.ix = str2fix("m"); pairOut.narg = 1; kd.i2o[pairIn] = pairOut;
		Functions.push_back(kd);	resetFcn( kd );
	kd.mat_3 = MinIndex;	kd.name = "mini";		kd.help = "langref/elementary_functs/analysis/min.htm";	// more than 1 output
		pairIn.ix = str2fix("m"); pairIn.narg = 1; pairOut.ix = str2fix("mm"); pairOut.narg = 2; kd.i2o[pairIn] = pairOut;
		Functions.push_back(kd);	resetFcn( kd );
	kd.mat_3 = MaxIndex;	kd.name = "maxi";		kd.help = "langref/elementary_functs/analysis/max.htm";	// more than 1 output
		pairIn.ix = str2fix("m"); pairIn.narg = 1; pairOut.ix = str2fix("mm"); pairOut.narg = 2; kd.i2o[pairIn] = pairOut;
		Functions.push_back(kd);	resetFcn( kd );
	
	kd.mat_2 = Abs;			kd.name = "mag";	kd.help = "langref/elementary_functs/analysis/mag.htm";
		pairIn.ix = str2fix("m"); pairIn.narg = 1; pairOut.ix = str2fix("m"); pairOut.narg = 1; kd.i2o[pairIn] = pairOut;
		Functions.push_back(kd);	resetFcn( kd );
	kd.mat_2 = GetDiag;		kd.name = "get_diag"; kd.help = "langref/elementary_functs/analysis/get_diag.htm";
		pairIn.ix = str2fix("m"); pairIn.narg = 1; pairOut.ix = str2fix("m"); pairOut.narg = 1; kd.i2o[pairIn] = pairOut;
		Functions.push_back(kd);	resetFcn( kd );

	kd.mat_2 = Sum;			kd.name = "add";	kd.help = "langref/elementary_functs/analysis/add.htm";
		pairIn.ix = str2fix("m"); pairIn.narg = 1; pairOut.ix = str2fix("m"); pairOut.narg = 1; kd.i2o[pairIn] = pairOut;
		Functions.push_back(kd);	resetFcn( kd );
	kd.mat_2 = CumSum;		kd.mat_3 = CumSum;		kd.name = "add_prog";	kd.help = "langref/elementary_functs/analysis/add_prog.htm";
		pairIn.ix = str2fix("m"); pairIn.narg = 1; pairOut.ix = str2fix("m"); pairOut.narg = 1; kd.i2o[pairIn] = pairOut;
		pairIn.ix = str2fix("mm"); pairIn.narg = 2; pairOut.ix = str2fix("m"); pairOut.narg = 1; kd.i2o[pairIn] = pairOut;
		Functions.push_back(kd);	resetFcn( kd );
	kd.mat_2 = Prod;		kd.mat_3 = Prod;		kd.name = "mult";	kd.help = "langref/elementary_functs/analysis/mult.htm";
		pairIn.ix = str2fix("m"); pairIn.narg = 1; pairOut.ix = str2fix("m"); pairOut.narg = 1; kd.i2o[pairIn] = pairOut;
		pairIn.ix = str2fix("mm"); pairIn.narg = 2; pairOut.ix = str2fix("m"); pairOut.narg = 1; kd.i2o[pairIn] = pairOut;
		Functions.push_back(kd);	resetFcn( kd );
	kd.mat_2 = CumProd;		kd.mat_3 = CumProd;		kd.name = "mult_prog";	kd.help = "langref/elementary_functs/analysis/mult_prog.htm";
		pairIn.ix = str2fix("m"); pairIn.narg = 1; pairOut.ix = str2fix("m"); pairOut.narg = 1; kd.i2o[pairIn] = pairOut;
		pairIn.ix = str2fix("mm"); pairIn.narg = 2; pairOut.ix = str2fix("m"); pairOut.narg = 1; kd.i2o[pairIn] = pairOut;
		Functions.push_back(kd);	resetFcn( kd );
	kd.mat_2 = Find;		kd.name = "search";		kd.help = "langref/elementary_functs/analysis/search.htm";
		pairIn.ix = str2fix("m"); pairIn.narg = 1; pairOut.ix = str2fix("m"); pairOut.narg = 1; kd.i2o[pairIn] = pairOut;
		Functions.push_back(kd);	resetFcn( kd );
	kd.mat_4 = Resize;		kd.name = "resize";			kd.help = "langref/elementary_functs/analysis/resize.htm";
		pairIn.ix = str2fix("mmm"); pairIn.narg = 3; pairOut.ix = str2fix("m"); pairOut.narg = 1; kd.i2o[pairIn] = pairOut;
		Functions.push_back(kd);	resetFcn( kd );
	kd.mat_3 = Rotate;		kd.name = "turn_ccw";			kd.help = "langref/elementary_functs/analysis/turn_ccw.htm";
		pairIn.ix = str2fix("mm"); pairIn.narg = 2; pairOut.ix = str2fix("m"); pairOut.narg = 1; kd.i2o[pairIn] = pairOut;
		Functions.push_back(kd);	resetFcn( kd );
	kd.mat_3 = RotateCw;		kd.name = "turn_cw";			kd.help = "langref/elementary_functs/analysis/turn_cw.htm";
		pairIn.ix = str2fix("mm"); pairIn.narg = 2; pairOut.ix = str2fix("m"); pairOut.narg = 1; kd.i2o[pairIn] = pairOut;
		Functions.push_back(kd);	resetFcn( kd );

	kd.mat_1 = RandSeed;	kd.name = "seed";	kd.help = "langref/elementary_functs/rand/seed.htm";
		pairIn.ix = str2fix("m"); pairIn.narg = 1; pairOut.ix = 0; pairOut.narg = 0; kd.i2o[pairIn] = pairOut;
		Functions.push_back(kd);	resetFcn( kd );
	kd.mat_2 = Rand;	kd.mat_3 = Rand;	kd.cbe_4 = Rand;	kd.name = "rand";	kd.help = "langref/elementary_functs/rand/rand.htm";
		pairIn.ix = str2fix("m"); pairIn.narg = 1; pairOut.ix = str2fix("m"); pairOut.narg = 1; kd.i2o[pairIn] = pairOut;
		pairIn.ix = str2fix("mm"); pairIn.narg = 2; pairOut.ix = str2fix("m"); pairOut.narg = 1; kd.i2o[pairIn] = pairOut;
		pairIn.ix = str2fix("mmm"); pairIn.narg = 3; pairOut.ix = str2fix("c"); pairOut.narg = 1; kd.i2o[pairIn] = pairOut;
		Functions.push_back(kd);	resetFcn( kd );
	kd.mat_2 = Randn;	kd.mat_3 = Randn;	kd.cbe_4 = Randn;	kd.name = "grand";	kd.help = "langref/elementary_functs/rand/grand.htm";
		pairIn.ix = str2fix("m"); pairIn.narg = 1; pairOut.ix = str2fix("m"); pairOut.narg = 1; kd.i2o[pairIn] = pairOut;
		pairIn.ix = str2fix("mm"); pairIn.narg = 2; pairOut.ix = str2fix("m"); pairOut.narg = 1; kd.i2o[pairIn] = pairOut;
		pairIn.ix = str2fix("mmm"); pairIn.narg = 3; pairOut.ix = str2fix("c"); pairOut.narg = 1; kd.i2o[pairIn] = pairOut;
		Functions.push_back(kd);	resetFcn( kd );

	kd.mat_2 = Sort;		kd.name = "sort";		kd.help = "langref/elementary_functs/analysis/sort.htm";	// for 1 output
		pairIn.ix = str2fix("m"); pairIn.narg = 1; pairOut.ix = str2fix("m"); pairOut.narg = 1; kd.i2o[pairIn] = pairOut;
		Functions.push_back(kd);	resetFcn( kd );
	kd.mat_3 = SortIndex;	kd.name = "sorti";		kd.help = "langref/elementary_functs/analysis/sort.htm";	// for 2 outputs
		pairIn.ix = str2fix("m"); pairIn.narg = 1; pairOut.ix = str2fix("mm"); pairOut.narg = 2; kd.i2o[pairIn] = pairOut;
		Functions.push_back(kd);	resetFcn( kd );
	kd.mat_2 = Unique;		kd.name = "unique";			kd.help = "langref/elementary_functs/analysis/unique.htm";
		pairIn.ix = str2fix("m"); pairIn.narg = 1; pairOut.ix = str2fix("m"); pairOut.narg = 1; kd.i2o[pairIn] = pairOut;
		Functions.push_back(kd);	resetFcn( kd );
	kd.mat_2 = Median;		kd.mat_3 = Median;		kd.name = "median";	kd.help = "langref/elementary_functs/analysis/median.htm";
		pairIn.ix = str2fix("m"); pairIn.narg = 1; pairOut.ix = str2fix("m"); pairOut.narg = 1; kd.i2o[pairIn] = pairOut;
		pairIn.ix = str2fix("mm"); pairIn.narg = 2; pairOut.ix = str2fix("m"); pairOut.narg = 1; kd.i2o[pairIn] = pairOut;
		Functions.push_back(kd);	resetFcn( kd );
	kd.mat_2 = Mean;		kd.mat_3 = Mean;		kd.name = "mean";	kd.help = "langref/elementary_functs/analysis/mean.htm";
		pairIn.ix = str2fix("m"); pairIn.narg = 1; pairOut.ix = str2fix("m"); pairOut.narg = 1; kd.i2o[pairIn] = pairOut;
		pairIn.ix = str2fix("mm"); pairIn.narg = 2; pairOut.ix = str2fix("m"); pairOut.narg = 1; kd.i2o[pairIn] = pairOut;
		Functions.push_back(kd);	resetFcn( kd );
	kd.mat_2 = Var;			kd.mat_3 = Var;			kd.name = "variance";	kd.help = "langref/elementary_functs/analysis/variance.htm";
		pairIn.ix = str2fix("m"); pairIn.narg = 1; pairOut.ix = str2fix("m"); pairOut.narg = 1; kd.i2o[pairIn] = pairOut;
		pairIn.ix = str2fix("mm"); pairIn.narg = 2; pairOut.ix = str2fix("m"); pairOut.narg = 1; kd.i2o[pairIn] = pairOut;
		Functions.push_back(kd);	resetFcn( kd );
	kd.mat_2 = Std;			kd.mat_3 = Std;			kd.name = "stdev";	kd.help = "langref/elementary_functs/analysis/stdev.htm";
		pairIn.ix = str2fix("m"); pairIn.narg = 1; pairOut.ix = str2fix("m"); pairOut.narg = 1; kd.i2o[pairIn] = pairOut;
		pairIn.ix = str2fix("mm"); pairIn.narg = 2; pairOut.ix = str2fix("m"); pairOut.narg = 1; kd.i2o[pairIn] = pairOut;
		Functions.push_back(kd);	resetFcn( kd );
	kd.mat_2 = Cov;			kd.mat_3 = Cov;			kd.name = "covariance";	kd.help = "langref/elementary_functs/analysis/covariance.htm";
		pairIn.ix = str2fix("m"); pairIn.narg = 1; pairOut.ix = str2fix("m"); pairOut.narg = 1; kd.i2o[pairIn] = pairOut;
		pairIn.ix = str2fix("m"); pairIn.narg = 2; pairOut.ix = str2fix("m"); pairOut.narg = 1; kd.i2o[pairIn] = pairOut;
		Functions.push_back(kd);	resetFcn( kd );
	kd.mat_2 = Hist;		kd.mat_3 = Hist_2o;		kd.name = "histogram";	kd.help = "langref/elementary_functs/analysis/histogram.htm";
		pairIn.ix = str2fix("m"); pairIn.narg = 1; pairOut.ix = str2fix("m"); pairOut.narg = 1; kd.i2o[pairIn] = pairOut;
		pairIn.ix = str2fix("m"); pairIn.narg = 1; pairOut.ix = str2fix("mm"); pairOut.narg = 2; kd.i2o[pairIn] = pairOut;
		Functions.push_back(kd);	resetFcn( kd );
	kd.mat_3 = Histn;		kd.mat_4 = Histn_2o;	kd.name = "histogram_n";	kd.help = "langref/elementary_functs/analysis/histogram.htm";
		pairIn.ix = str2fix("mm"); pairIn.narg = 2; pairOut.ix = str2fix("m"); pairOut.narg = 1; kd.i2o[pairIn] = pairOut;
		pairIn.ix = str2fix("mm"); pairIn.narg = 2; pairOut.ix = str2fix("mm"); pairOut.narg = 2; kd.i2o[pairIn] = pairOut;
		Functions.push_back(kd);	resetFcn( kd );
	kd.mat_3 = Histe;		kd.name = "histogram_e";		kd.help = "langref/elementary_functs/analysis/histogram.htm";
		pairIn.ix = str2fix("mm"); pairIn.narg = 2; pairOut.ix = str2fix("m"); pairOut.narg = 1; kd.i2o[pairIn] = pairOut;
		Functions.push_back(kd);	resetFcn( kd );
	kd.mat_3 = Histc;		kd.name = "histogram_c";		kd.help = "langref/elementary_functs/analysis/histogram.htm";
		pairIn.ix = str2fix("mm"); pairIn.narg = 2; pairOut.ix = str2fix("m"); pairOut.narg = 1; kd.i2o[pairIn] = pairOut;
		Functions.push_back(kd);	resetFcn( kd );

	kd.mat_2 = Fft;			kd.name = "fft";		kd.help = "langref/elementary_functs/analysis/fft.htm";
		pairIn.ix = str2fix("m"); pairIn.narg = 1; pairOut.ix = str2fix("m"); pairOut.narg = 1; kd.i2o[pairIn] = pairOut;
		Functions.push_back(kd);	resetFcn( kd );
	kd.mat_2 = Ifft;		kd.name = "ifft";		kd.help = "langref/elementary_functs/analysis/fft.htm";
		pairIn.ix = str2fix("m"); pairIn.narg = 1; pairOut.ix = str2fix("m"); pairOut.narg = 1; kd.i2o[pairIn] = pairOut;
		Functions.push_back(kd);	resetFcn( kd );
	kd.mat_2 = Dft;			kd.name = "dft";		kd.help = "langref/elementary_functs/analysis/fft.htm";
		pairIn.ix = str2fix("m"); pairIn.narg = 1; pairOut.ix = str2fix("m"); pairOut.narg = 1; kd.i2o[pairIn] = pairOut;
		Functions.push_back(kd);	resetFcn( kd );
	kd.mat_2 = Idft;		kd.name = "idft";		kd.help = "langref/elementary_functs/analysis/fft.htm";
		pairIn.ix = str2fix("m"); pairIn.narg = 1; pairOut.ix = str2fix("m"); pairOut.narg = 1; kd.i2o[pairIn] = pairOut;
		Functions.push_back(kd);	resetFcn( kd );
	kd.mat_2 = Fftz;		kd.name = "fftz";		kd.help = "langref/elementary_functs/analysis/fftz.htm";
		pairIn.ix = str2fix("m"); pairIn.narg = 1; pairOut.ix = str2fix("m"); pairOut.narg = 1; kd.i2o[pairIn] = pairOut;
		Functions.push_back(kd);	resetFcn( kd );
	kd.mat_2 = Ifftz;		kd.name = "ifftz";		kd.help = "langref/elementary_functs/analysis/fftz.htm";
		pairIn.ix = str2fix("m"); pairIn.narg = 1; pairOut.ix = str2fix("m"); pairOut.narg = 1; kd.i2o[pairIn] = pairOut;
		Functions.push_back(kd);	resetFcn( kd );
	kd.mat_2 = Fft2;		kd.name = "fft_2d";		kd.help = "langref/elementary_functs/analysis/fft_2d.htm";
		pairIn.ix = str2fix("m"); pairIn.narg = 1; pairOut.ix = str2fix("m"); pairOut.narg = 1; kd.i2o[pairIn] = pairOut;
		Functions.push_back(kd);	resetFcn( kd );
	kd.mat_2 = Ifft2;		kd.name = "ifft_2d";		kd.help = "langref/elementary_functs/analysis/fft_2d.htm";
		pairIn.ix = str2fix("m"); pairIn.narg = 1; pairOut.ix = str2fix("m"); pairOut.narg = 1; kd.i2o[pairIn] = pairOut;
		Functions.push_back(kd);	resetFcn( kd );
	kd.mat_2 = Shfft;		kd.name = "shfft";		kd.help = "langref/elementary_functs/analysis/shfft.htm";
		pairIn.ix = str2fix("m"); pairIn.narg = 1; pairOut.ix = str2fix("m"); pairOut.narg = 1; kd.i2o[pairIn] = pairOut;
		Functions.push_back(kd);	resetFcn( kd );

	kd.mat_2 = Matrix2Norm;	kd.mat_3 = MatrixPNorm;	kd.name = "norm";	kd.help = "langref/elementary_functs/matrix/norm.htm";
		pairIn.ix = str2fix("m"); pairIn.narg = 1; pairOut.ix = str2fix("m"); pairOut.narg = 1; kd.i2o[pairIn] = pairOut;
		pairIn.ix = str2fix("mm"); pairIn.narg = 2; pairOut.ix = str2fix("m"); pairOut.narg = 1; kd.i2o[pairIn] = pairOut;
		Functions.push_back(kd);	resetFcn( kd );
	kd.mat_3 = arma_qr;				kd.name = "qr";	kd.help = "langref/elementary_functs/matrix/qr.htm";
		pairIn.ix = str2fix("m"); pairIn.narg = 1; pairOut.ix = str2fix("mm"); pairOut.narg = 2; kd.i2o[pairIn] = pairOut;
		Functions.push_back(kd);	resetFcn( kd );
	kd.mat_4 = arma_lu;				kd.name = "lu";	kd.help = "langref/elementary_functs/matrix/lu.htm";
		pairIn.ix = str2fix("m"); pairIn.narg = 1; pairOut.ix = str2fix("mmm"); pairOut.narg = 3; kd.i2o[pairIn] = pairOut;
		Functions.push_back(kd);	resetFcn( kd );
	kd.mat_2 = arma_svd;	kd.mat_3 = arma_svd;	kd.mat_4 = arma_svd;			kd.name = "svd";	kd.help = "langref/elementary_functs/matrix/svd.htm";
		pairIn.ix = str2fix("m"); pairIn.narg = 1; pairOut.ix = str2fix("m"); pairOut.narg = 1; kd.i2o[pairIn] = pairOut;
		pairIn.ix = str2fix("m"); pairIn.narg = 1; pairOut.ix = str2fix("mm"); pairOut.narg = 2; kd.i2o[pairIn] = pairOut;
		pairIn.ix = str2fix("m"); pairIn.narg = 1; pairOut.ix = str2fix("mmm"); pairOut.narg = 3; kd.i2o[pairIn] = pairOut;
		Functions.push_back(kd);	resetFcn( kd );
	kd.mat_2 = arma_inv;			kd.name = "inverse";	kd.help = "langref/elementary_functs/matrix/inverse.htm";
		pairIn.ix = str2fix("m"); pairIn.narg = 1; pairOut.ix = str2fix("m"); pairOut.narg = 1; kd.i2o[pairIn] = pairOut;
		Functions.push_back(kd);	resetFcn( kd );
	kd.mat_2 = arma_pinv;			kd.name = "ginverse";	kd.help = "langref/elementary_functs/matrix/ginverse.htm";
		pairIn.ix = str2fix("m"); pairIn.narg = 1; pairOut.ix = str2fix("m"); pairOut.narg = 1; kd.i2o[pairIn] = pairOut;
		Functions.push_back(kd);	resetFcn( kd );
	//kd.mat_2 = arma_chol;			kd.name = "chol";		Functions.push_back(kd);	resetFcn( kd );		// must force arg to be symmetric somehow
	kd.mat_2 = arma_eig;	kd.mat_3 = arma_eig;	kd.name = "eigenv";	kd.help = "langref/elementary_functs/matrix/eigenv.htm";
		pairIn.ix = str2fix("m"); pairIn.narg = 1; pairOut.ix = str2fix("m"); pairOut.narg = 1; kd.i2o[pairIn] = pairOut;
		pairIn.ix = str2fix("m"); pairIn.narg = 1; pairOut.ix = str2fix("mm"); pairOut.narg = 2; kd.i2o[pairIn] = pairOut;
		Functions.push_back(kd);	resetFcn( kd );
	kd.mat_2 = arma_eigr;	kd.mat_3 = arma_eigr;	kd.name = "eigenv_r";	kd.help = "langref/elementary_functs/matrix/eigenv_r.htm";
		pairIn.ix = str2fix("m"); pairIn.narg = 1; pairOut.ix = str2fix("m"); pairOut.narg = 1; kd.i2o[pairIn] = pairOut;
		pairIn.ix = str2fix("m"); pairIn.narg = 1; pairOut.ix = str2fix("mm"); pairOut.narg = 2; kd.i2o[pairIn] = pairOut;
		Functions.push_back(kd);	resetFcn( kd );
	kd.mat_2 = arma_det;			kd.name = "det";	kd.help = "langref/elementary_functs/matrix/det.htm";
		pairIn.ix = str2fix("m"); pairIn.narg = 1; pairOut.ix = str2fix("m"); pairOut.narg = 1; kd.i2o[pairIn] = pairOut;
		Functions.push_back(kd);	resetFcn( kd );
	kd.mat_3 = arma_solve;			kd.name = "solve";	kd.help = "langref/elementary_functs/matrix/solve.htm";
		pairIn.ix = str2fix("mm"); pairIn.narg = 2; pairOut.ix = str2fix("m"); pairOut.narg = 1; kd.i2o[pairIn] = pairOut;
		Functions.push_back(kd);	resetFcn( kd );
	kd.mat_2 = arma_eps;			kd.name = "thresh";	kd.help = "langref/misc/thresh.htm";
		pairIn.ix = str2fix("m"); pairIn.narg = 1; pairOut.ix = str2fix("m"); pairOut.narg = 1; kd.i2o[pairIn] = pairOut;
		Functions.push_back(kd);	resetFcn( kd );
	kd.mat_4 = arma_syl;			kd.name = "sylv";
		pairIn.ix = str2fix("mmm"); pairIn.narg = 3; pairOut.ix = str2fix("m"); pairOut.narg = 1; kd.i2o[pairIn] = pairOut;
		Functions.push_back(kd);	resetFcn( kd );
	kd.mat_2 = arma_rank;		kd.mat_3 = arma_rank;			kd.name = "rank";	kd.help = "langref/elementary_functs/matrix/rank.htm";
		pairIn.ix = str2fix("m"); pairIn.narg = 1; pairOut.ix = str2fix("m"); pairOut.narg = 1; kd.i2o[pairIn] = pairOut;
		pairIn.ix = str2fix("mm"); pairIn.narg = 2; pairOut.ix = str2fix("m"); pairOut.narg = 1; kd.i2o[pairIn] = pairOut;
		Functions.push_back(kd);	resetFcn( kd );
	kd.mat_2 = arma_trace;			kd.name = "trace";	kd.help = "langref/elementary_functs/matrix/trace.htm";
		pairIn.ix = str2fix("m"); pairIn.narg = 1; pairOut.ix = str2fix("m"); pairOut.narg = 1; kd.i2o[pairIn] = pairOut;
		Functions.push_back(kd);	resetFcn( kd );
	kd.mat_3 = arma_conv;			kd.name = "convolve";	kd.help = "langref/elementary_functs/analysis/convolve.htm";
		pairIn.ix = str2fix("mm"); pairIn.narg = 2; pairOut.ix = str2fix("m"); pairOut.narg = 1; kd.i2o[pairIn] = pairOut;
		Functions.push_back(kd);	resetFcn( kd );
	kd.mat_3 = Conv2;			kd.name = "convolve_2d";	kd.help = "langref/elementary_functs/analysis/convolve_2d.htm";
		pairIn.ix = str2fix("mm"); pairIn.narg = 2; pairOut.ix = str2fix("m"); pairOut.narg = 1; kd.i2o[pairIn] = pairOut;
		Functions.push_back(kd);	resetFcn( kd );

	kd.mat_2 = ShuffleRows;	kd.name = "shuf_rows";	kd.help = "langref/elementary_functs/analysis/shuf_rows.htm";
		pairIn.ix = str2fix("m"); pairIn.narg = 1; pairOut.ix = str2fix("m"); pairOut.narg = 1; kd.i2o[pairIn] = pairOut;
		Functions.push_back(kd);	resetFcn( kd );
	kd.mat_2 = ShuffleCols;	kd.name = "shuf_cols";	kd.help = "langref/elementary_functs/analysis/shuf_cols.htm";
		pairIn.ix = str2fix("m"); pairIn.narg = 1; pairOut.ix = str2fix("m"); pairOut.narg = 1; kd.i2o[pairIn] = pairOut;
		Functions.push_back(kd);	resetFcn( kd );
	kd.mat_3 = rm_rows;	kd.cbe2_mat = rm_rows;	kd.name = "rm_rows";	kd.help = "langref/misc/rm_rows.htm";
		pairIn.ix = str2fix("mm"); pairIn.narg = 2; pairOut.ix = str2fix("m"); pairOut.narg = 1; kd.i2o[pairIn] = pairOut;
		pairIn.ix = str2fix("cm"); pairIn.narg = 2; pairOut.ix = str2fix("c"); pairOut.narg = 1; kd.i2o[pairIn] = pairOut;
		Functions.push_back(kd);	resetFcn( kd );
	kd.mat_3 = rm_cols;	kd.cbe2_mat = rm_cols;	kd.name = "rm_cols";	kd.help = "langref/misc/rm_cols.htm";
		pairIn.ix = str2fix("mm"); pairIn.narg = 2; pairOut.ix = str2fix("m"); pairOut.narg = 1; kd.i2o[pairIn] = pairOut;
		pairIn.ix = str2fix("cm"); pairIn.narg = 2; pairOut.ix = str2fix("c"); pairOut.narg = 1; kd.i2o[pairIn] = pairOut;
		Functions.push_back(kd);	resetFcn( kd );
	kd.cbe2_mat = rm_depths;	kd.name = "rm_depths";		kd.help = "langref/misc/rm_depths.htm";
		pairIn.ix = str2fix("cm"); pairIn.narg = 2; pairOut.ix = str2fix("c"); pairOut.narg = 1; kd.i2o[pairIn] = pairOut;
		Functions.push_back(kd);	resetFcn( kd );

	kd.mat_2 = is_empty;	kd.mat_cbe_2 = is_empty;	kd.mat_str = is_empty;	kd.mat_hsh = is_empty;		kd.name = "is_empty";		kd.help = "langref/misc/is_empty.htm";
		pairIn.ix = str2fix("m"); pairIn.narg = 1; pairOut.ix = str2fix("m"); pairOut.narg = 1; kd.i2o[pairIn] = pairOut;
		pairIn.ix = str2fix("c"); pairIn.narg = 1; pairOut.ix = str2fix("m"); pairOut.narg = 1; kd.i2o[pairIn] = pairOut;
		pairIn.ix = str2fix("s"); pairIn.narg = 1; pairOut.ix = str2fix("m"); pairOut.narg = 1; kd.i2o[pairIn] = pairOut;
		pairIn.ix = str2fix("h"); pairIn.narg = 1; pairOut.ix = str2fix("m"); pairOut.narg = 1; kd.i2o[pairIn] = pairOut;
		Functions.push_back(kd);	resetFcn( kd );
	kd.mat_2 = is_real;			kd.mat_cbe_2 = is_real;			kd.name = "is_real";		kd.help = "langref/misc/is_real.htm";
		pairIn.ix = str2fix("m"); pairIn.narg = 1; pairOut.ix = str2fix("m"); pairOut.narg = 1; kd.i2o[pairIn] = pairOut;
		pairIn.ix = str2fix("c"); pairIn.narg = 1; pairOut.ix = str2fix("m"); pairOut.narg = 1; kd.i2o[pairIn] = pairOut;
		Functions.push_back(kd);	resetFcn( kd );
	kd.mat_2 = is_vector;		kd.mat_cbe_2 = is_vector;		kd.name = "is_vector";		kd.help = "langref/misc/is_vector.htm";
		pairIn.ix = str2fix("m"); pairIn.narg = 1; pairOut.ix = str2fix("m"); pairOut.narg = 1; kd.i2o[pairIn] = pairOut;
		pairIn.ix = str2fix("c"); pairIn.narg = 1; pairOut.ix = str2fix("m"); pairOut.narg = 1; kd.i2o[pairIn] = pairOut;
		Functions.push_back(kd);	resetFcn( kd );
	kd.mat_2 = is_scalar;		kd.mat_cbe_2 = is_scalar;		kd.name = "is_scalar";		kd.help = "langref/misc/is_scalar.htm";
		pairIn.ix = str2fix("m"); pairIn.narg = 1; pairOut.ix = str2fix("m"); pairOut.narg = 1; kd.i2o[pairIn] = pairOut;
		pairIn.ix = str2fix("c"); pairIn.narg = 1; pairOut.ix = str2fix("m"); pairOut.narg = 1; kd.i2o[pairIn] = pairOut;
		Functions.push_back(kd);	resetFcn( kd );
	kd.mat_2 = is_square;										kd.name = "is_square";		kd.help = "langref/misc/is_square.htm";
		pairIn.ix = str2fix("m"); pairIn.narg = 1; pairOut.ix = str2fix("m"); pairOut.narg = 1; kd.i2o[pairIn] = pairOut;
		Functions.push_back(kd);	resetFcn( kd );
	kd.mat_2 = is_finite;		kd.cbe_cbe_2 = is_finite;		kd.name = "is_finite";		kd.help = "langref/misc/is_finite.htm";
		pairIn.ix = str2fix("m"); pairIn.narg = 1; pairOut.ix = str2fix("m"); pairOut.narg = 1; kd.i2o[pairIn] = pairOut;
		pairIn.ix = str2fix("c"); pairIn.narg = 1; pairOut.ix = str2fix("c"); pairOut.narg = 1; kd.i2o[pairIn] = pairOut;
		Functions.push_back(kd);	resetFcn( kd );
	kd.mat_2 = is_nan;		kd.cbe_cbe_2 = is_nan;		kd.name = "is_nan";		kd.help = "langref/misc/is_nan.htm";
		pairIn.ix = str2fix("m"); pairIn.narg = 1; pairOut.ix = str2fix("m"); pairOut.narg = 1; kd.i2o[pairIn] = pairOut;
		pairIn.ix = str2fix("c"); pairIn.narg = 1; pairOut.ix = str2fix("c"); pairOut.narg = 1; kd.i2o[pairIn] = pairOut;
		Functions.push_back(kd);	resetFcn( kd );
	kd.mat_hsh_str = is_key;		kd.name = "is_key";		kd.help = "langref/misc/is_key.htm";
		pairIn.ix = str2fix("hs"); pairIn.narg = 2; pairOut.ix = str2fix("m"); pairOut.narg = 1; kd.i2o[pairIn] = pairOut;
		Functions.push_back(kd);	resetFcn( kd );
	kd.mat_str = is_valid_field;		kd.name = "is_valid_field";		kd.help = "langref/misc/is_valid_field.htm";
		pairIn.ix = str2fix("s"); pairIn.narg = 1; pairOut.ix = str2fix("m"); pairOut.narg = 1; kd.i2o[pairIn] = pairOut;
		Functions.push_back(kd);	resetFcn( kd );

	// constants
	kd.mat_1 = get_inf;				kd.name = "_inf";			kd.help = "langref/constants.htm";
		pairIn.ix = 0; pairIn.narg = 0; pairOut.ix = str2fix("m"); pairOut.narg = 1; kd.i2o[pairIn] = pairOut;
		Functions.push_back(kd);	resetFcn( kd );
	kd.mat_1 = get_i;				kd.name = "_i";				kd.help = "langref/constants.htm";
		pairIn.ix = 0; pairIn.narg = 0; pairOut.ix = str2fix("m"); pairOut.narg = 1; kd.i2o[pairIn] = pairOut;
		Functions.push_back(kd);	resetFcn( kd );
	kd.mat_1 = get_e;				kd.name = "_e";				kd.help = "langref/constants.htm";
		pairIn.ix = 0; pairIn.narg = 0; pairOut.ix = str2fix("m"); pairOut.narg = 1; kd.i2o[pairIn] = pairOut;
		Functions.push_back(kd);	resetFcn( kd );
	kd.mat_1 = get_pi;				kd.name = "_pi";			kd.help = "langref/constants.htm";
		pairIn.ix = 0; pairIn.narg = 0; pairOut.ix = str2fix("m"); pairOut.narg = 1; kd.i2o[pairIn] = pairOut;
		Functions.push_back(kd);	resetFcn( kd );
	kd.mat_1 = get_boltzmann;		kd.name = "_k";				kd.help = "langref/constants.htm";
		pairIn.ix = 0; pairIn.narg = 0; pairOut.ix = str2fix("m"); pairOut.narg = 1; kd.i2o[pairIn] = pairOut;
		Functions.push_back(kd);	resetFcn( kd );
	kd.mat_1 = get_avagadro;		kd.name = "_na";			kd.help = "langref/constants.htm";
		pairIn.ix = 0; pairIn.narg = 0; pairOut.ix = str2fix("m"); pairOut.narg = 1; kd.i2o[pairIn] = pairOut;
		Functions.push_back(kd);	resetFcn( kd );
	kd.mat_1 = get_goldenratio;		kd.name = "_golden";		kd.help = "langref/constants.htm";
		pairIn.ix = 0; pairIn.narg = 0; pairOut.ix = str2fix("m"); pairOut.narg = 1; kd.i2o[pairIn] = pairOut;
		Functions.push_back(kd);	resetFcn( kd );
	kd.mat_1 = get_eps;		kd.name = "_eps";					kd.help = "langref/constants.htm";	// the difference between 1 and the least value greater than 1 that is representable 
		pairIn.ix = 0; pairIn.narg = 0; pairOut.ix = str2fix("m"); pairOut.narg = 1; kd.i2o[pairIn] = pairOut;
		Functions.push_back(kd);	resetFcn( kd );
	kd.mat_1 = get_atomicmass;		kd.name = "_mu";			kd.help = "langref/constants.htm";	// atomic mass constant (in kg)
		pairIn.ix = 0; pairIn.narg = 0; pairOut.ix = str2fix("m"); pairOut.narg = 1; kd.i2o[pairIn] = pairOut;
		Functions.push_back(kd);	resetFcn( kd );
	kd.mat_1 = get_Z0;		kd.name = "_Z0";					kd.help = "langref/constants.htm";	// intrinsic impedance of vacuum (ohms)
		pairIn.ix = 0; pairIn.narg = 0; pairOut.ix = str2fix("m"); pairOut.narg = 1; kd.i2o[pairIn] = pairOut;
		Functions.push_back(kd);	resetFcn( kd );
	kd.mat_1 = get_G0;		kd.name = "_G0";					kd.help = "langref/constants.htm";	// conductance quantum (in siemens)
		pairIn.ix = 0; pairIn.narg = 0; pairOut.ix = str2fix("m"); pairOut.narg = 1; kd.i2o[pairIn] = pairOut;
		Functions.push_back(kd);	resetFcn( kd );
	//kd.mat_1 = get_KE;		kd.name = "_ke";			// Coulomb's constant (in meters per farad)
	//	pairIn.ix = 0; pairIn.narg = 0; pairOut.ix = str2fix("m"); pairOut.narg = 1; kd.i2o[pairIn] = pairOut;
	//	Functions.push_back(kd);	resetFcn( kd );
	kd.mat_1 = get_eps0;		kd.name = "_eps0";				kd.help = "langref/constants.htm";	// electric constant (in farads per meter)
		pairIn.ix = 0; pairIn.narg = 0; pairOut.ix = str2fix("m"); pairOut.narg = 1; kd.i2o[pairIn] = pairOut;
		Functions.push_back(kd);	resetFcn( kd );
	kd.mat_1 = get_me;		kd.name = "_me";					kd.help = "langref/constants.htm";	// electron mass
		pairIn.ix = 0; pairIn.narg = 0; pairOut.ix = str2fix("m"); pairOut.narg = 1; kd.i2o[pairIn] = pairOut;
		Functions.push_back(kd);	resetFcn( kd );
	kd.mat_1 = get_ec;		kd.name = "_ec";					kd.help = "langref/constants.htm";	// elementary charge (in coulombs)
		pairIn.ix = 0; pairIn.narg = 0; pairOut.ix = str2fix("m"); pairOut.narg = 1; kd.i2o[pairIn] = pairOut;
		Functions.push_back(kd);	resetFcn( kd );
	kd.mat_1 = get_ev;		kd.name = "_ev";					kd.help = "langref/constants.htm";	// electron volt (in joules)
		pairIn.ix = 0; pairIn.narg = 0; pairOut.ix = str2fix("m"); pairOut.narg = 1; kd.i2o[pairIn] = pairOut;
		Functions.push_back(kd);	resetFcn( kd );
	kd.mat_1 = get_F;		kd.name = "_F";						kd.help = "langref/constants.htm";	// Faraday constant (in coulombs)
		pairIn.ix = 0; pairIn.narg = 0; pairOut.ix = str2fix("m"); pairOut.narg = 1; kd.i2o[pairIn] = pairOut;
		Functions.push_back(kd);	resetFcn( kd );
	kd.mat_1 = get_alpha;		kd.name = "_alpha";				kd.help = "langref/constants.htm";	// fine-structure constant
		pairIn.ix = 0; pairIn.narg = 0; pairOut.ix = str2fix("m"); pairOut.narg = 1; kd.i2o[pairIn] = pairOut;
		Functions.push_back(kd);	resetFcn( kd );
	kd.mat_1 = get_alpha_inv;		kd.name = "_alpha_inv";		kd.help = "langref/constants.htm";	// inverse fine-structure constant
		pairIn.ix = 0; pairIn.narg = 0; pairOut.ix = str2fix("m"); pairOut.narg = 1; kd.i2o[pairIn] = pairOut;
		Functions.push_back(kd);	resetFcn( kd );
	kd.mat_1 = get_kj;		kd.name = "_kj";					kd.help = "langref/constants.htm";	// Josephson constant
		pairIn.ix = 0; pairIn.narg = 0; pairOut.ix = str2fix("m"); pairOut.narg = 1; kd.i2o[pairIn] = pairOut;
		Functions.push_back(kd);	resetFcn( kd );
	kd.mat_1 = get_mu_0;		kd.name = "_mu_0";				kd.help = "langref/constants.htm";	// magnetic constant (in henries per meter)
		pairIn.ix = 0; pairIn.narg = 0; pairOut.ix = str2fix("m"); pairOut.narg = 1; kd.i2o[pairIn] = pairOut;
		Functions.push_back(kd);	resetFcn( kd );
	kd.mat_1 = get_phi_0;		kd.name = "_phi_0";				kd.help = "langref/constants.htm";	// magnetic flux quantum (in webers)
		pairIn.ix = 0; pairIn.narg = 0; pairOut.ix = str2fix("m"); pairOut.narg = 1; kd.i2o[pairIn] = pairOut;
		Functions.push_back(kd);	resetFcn( kd );
	kd.mat_1 = get_R;		kd.name = "_R";						kd.help = "langref/constants.htm";	// molar gas constant (in joules per mole kelvin)
		pairIn.ix = 0; pairIn.narg = 0; pairOut.ix = str2fix("m"); pairOut.narg = 1; kd.i2o[pairIn] = pairOut;
		Functions.push_back(kd);	resetFcn( kd );
	kd.mat_1 = get_G;		kd.name = "_G";						kd.help = "langref/constants.htm";	// Newtonian constant of gravitation (in newton square meters per kilogram squared
		pairIn.ix = 0; pairIn.narg = 0; pairOut.ix = str2fix("m"); pairOut.narg = 1; kd.i2o[pairIn] = pairOut;
		Functions.push_back(kd);	resetFcn( kd );
	kd.mat_1 = get_h;		kd.name = "_h";						kd.help = "langref/constants.htm";	// Planck constant (in joule seconds)
		pairIn.ix = 0; pairIn.narg = 0; pairOut.ix = str2fix("m"); pairOut.narg = 1; kd.i2o[pairIn] = pairOut;
		Functions.push_back(kd);	resetFcn( kd );
	kd.mat_1 = get_hbar;		kd.name = "_hbar";				kd.help = "langref/constants.htm";	// Planck constant over 2 pi, aka reduced Planck constant (in joule seconds)
		pairIn.ix = 0; pairIn.narg = 0; pairOut.ix = str2fix("m"); pairOut.narg = 1; kd.i2o[pairIn] = pairOut;
		Functions.push_back(kd);	resetFcn( kd );
	kd.mat_1 = get_light_speed;		kd.name = "_c";				kd.help = "langref/constants.htm";	// speed of light in a vacuum
		pairIn.ix = 0; pairIn.narg = 0; pairOut.ix = str2fix("m"); pairOut.narg = 1; kd.i2o[pairIn] = pairOut;
		Functions.push_back(kd);	resetFcn( kd );
	kd.mat_1 = get_mp;		kd.name = "_mp";					kd.help = "langref/constants.htm";	// proton mass (in kg)
		pairIn.ix = 0; pairIn.narg = 0; pairOut.ix = str2fix("m"); pairOut.narg = 1; kd.i2o[pairIn] = pairOut;
		Functions.push_back(kd);	resetFcn( kd );
	kd.mat_1 = get_Rinf;		kd.name = "_Rinf";				kd.help = "langref/constants.htm";	// Rydberg constant (in reciprocal meters)
		pairIn.ix = 0; pairIn.narg = 0; pairOut.ix = str2fix("m"); pairOut.narg = 1; kd.i2o[pairIn] = pairOut;
		Functions.push_back(kd);	resetFcn( kd );
	kd.mat_1 = get_sigma;		kd.name = "_sigma";				kd.help = "langref/constants.htm";	// Stefan-Boltzmann constant
		pairIn.ix = 0; pairIn.narg = 0; pairOut.ix = str2fix("m"); pairOut.narg = 1; kd.i2o[pairIn] = pairOut;
		Functions.push_back(kd);	resetFcn( kd );
	kd.mat_1 = get_Rk;		kd.name = "_Rk";					kd.help = "langref/constants.htm";	// von Klitzing constant (in ohms)
		pairIn.ix = 0; pairIn.narg = 0; pairOut.ix = str2fix("m"); pairOut.narg = 1; kd.i2o[pairIn] = pairOut;
		Functions.push_back(kd);	resetFcn( kd );
	kd.mat_1 = get_wien_b;		kd.name = "_b";					kd.help = "langref/constants.htm";	// Wien wavelength displacement law constant
		pairIn.ix = 0; pairIn.narg = 0; pairOut.ix = str2fix("m"); pairOut.narg = 1; kd.i2o[pairIn] = pairOut;
		Functions.push_back(kd);	resetFcn( kd );
	// others:
	// earth radius (circular)
	// earth elliptical params (polar & equitorial radius, & the other parameter)
	// 

	// Version info
	kd.str_1 = GetVersion;		kd.name = "ver";		kd.help = "langref/string/ver.htm";
		pairIn.ix = 0; pairIn.narg = 0; pairOut.ix = str2fix("s"); pairOut.narg = 1; kd.i2o[pairIn] = pairOut;
		Functions.push_back(kd);		resetFcn( kd );
	
	// String
	kd.mat_str2 = FindMatch;		kd.name = "find_match";		kd.help = "langref/string/find_match.htm";
		pairIn.ix = str2fix("ss"); pairIn.narg = 2; pairOut.ix = str2fix("m"); pairOut.narg = 1; kd.i2o[pairIn] = pairOut;
		Functions.push_back(kd);		resetFcn( kd );
	kd.mat_str2_mat = FindNext;	kd.name = "find_next";		kd.help = "langref/string/find_next.htm";
		pairIn.ix = str2fix("ssm"); pairIn.narg = 3; pairOut.ix = str2fix("m"); pairOut.narg = 1; kd.i2o[pairIn] = pairOut;
		Functions.push_back(kd);		resetFcn( kd );
	kd.mat_str2_mat = FindPrev;	kd.name = "find_prev";		kd.help = "langref/string/find_prev.htm";
		pairIn.ix = str2fix("ssm"); pairIn.narg = 3; pairOut.ix = str2fix("m"); pairOut.narg = 1; kd.i2o[pairIn] = pairOut;
		Functions.push_back(kd);		resetFcn( kd );
	kd.str_4 = Replace;			kd.name = "replace";		kd.help = "langref/string/replace.htm";
		pairIn.ix = str2fix("sss"); pairIn.narg = 3; pairOut.ix = str2fix("s"); pairOut.narg = 1; kd.i2o[pairIn] = pairOut;
		Functions.push_back(kd);		resetFcn( kd );
	kd.str2_mat = Substr;	kd.str2_mat2 = Substr;	kd.name = "substr";		kd.help = "langref/string/substr.htm";
		pairIn.ix = str2fix("sm"); pairIn.narg = 2; pairOut.ix = str2fix("s"); pairOut.narg = 1; kd.i2o[pairIn] = pairOut;
		pairIn.ix = str2fix("smm"); pairIn.narg = 3; pairOut.ix = str2fix("s"); pairOut.narg = 1; kd.i2o[pairIn] = pairOut;
		Functions.push_back(kd);		resetFcn( kd );
	kd.str3_mat = ::Insert;		kd.name = "insert";			kd.help = "langref/string/insert.htm";
		pairIn.ix = str2fix("ssm"); pairIn.narg = 3; pairOut.ix = str2fix("s"); pairOut.narg = 1; kd.i2o[pairIn] = pairOut;
		Functions.push_back(kd);		resetFcn( kd );
	kd.str_3 = Append;		kd.str_4 = Append;	kd.str_ni_1o = Append;	kd.mat_3 = HorzCat;			kd.name = "hjoin";	kd.help = "langref/elementary_functs/analysis/hjoin.htm";
		pairIn.ix = str2fix("ss"); pairIn.narg = 2; pairOut.ix = str2fix("s"); pairOut.narg = 1; kd.i2o[pairIn] = pairOut;
		//pairIn.ix = str2fix("sss"); pairIn.narg = 3; pairOut.ix = str2fix("s"); pairOut.narg = 1; kd.i2o[pairIn] = pairOut;
		pairIn.ix = str2fix("mm"); pairIn.narg = 2; pairOut.ix = str2fix("m"); pairOut.narg = 1; kd.i2o[pairIn] = pairOut;
		Functions.push_back(kd);		resetFcn( kd );
	kd.str_3 = Append;		kd.str_4 = Append;	kd.str_ni_1o = Append;	kd.name = "join";	kd.help = "langref/elementary_functs/string/join.htm";
		pairIn.ix = str2fix("ss"); pairIn.narg = 2; pairOut.ix = str2fix("s"); pairOut.narg = 1; kd.i2o[pairIn] = pairOut;
		//pairIn.ix = str2fix("sss"); pairIn.narg = 3; pairOut.ix = str2fix("s"); pairOut.narg = 1; kd.i2o[pairIn] = pairOut;
		Functions.push_back(kd);		resetFcn( kd );
	kd.mat_3 = VertCat;			kd.name = "vjoin";			kd.help = "langref/elementary_functs/analysis/vjoin.htm";
		pairIn.ix = str2fix("mm"); pairIn.narg = 2; pairOut.ix = str2fix("m"); pairOut.narg = 1; kd.i2o[pairIn] = pairOut;
		Functions.push_back(kd);		resetFcn( kd );
	kd.str_2 = ToLower;			kd.name = "to_lower";		kd.help = "langref/string/to_lower.htm";
		pairIn.ix = str2fix("s"); pairIn.narg = 1; pairOut.ix = str2fix("s"); pairOut.narg = 1; kd.i2o[pairIn] = pairOut;
		Functions.push_back(kd);		resetFcn( kd );
	kd.str_2 = ToUpper;			kd.name = "to_upper";		kd.help = "langref/string/to_upper.htm";
		pairIn.ix = str2fix("s"); pairIn.narg = 1; pairOut.ix = str2fix("s"); pairOut.narg = 1; kd.i2o[pairIn] = pairOut;
		Functions.push_back(kd);		resetFcn( kd );
	kd.str_mat = ToString;		kd.name = "to_string";		kd.help = "langref/string/to_string.htm";
		pairIn.ix = str2fix("m"); pairIn.narg = 1; pairOut.ix = str2fix("s"); pairOut.narg = 1; kd.i2o[pairIn] = pairOut;
		Functions.push_back(kd);		resetFcn( kd );
	kd.mat_str = ToScalar;		kd.name = "to_num";		kd.help = "langref/string/to_num.htm";
		pairIn.ix = str2fix("s"); pairIn.narg = 1; pairOut.ix = str2fix("m"); pairOut.narg = 1; kd.i2o[pairIn] = pairOut;
		Functions.push_back(kd);		resetFcn( kd );

	// hash
	kd.hsh2_str = DeleteKey;			kd.name = "rm_key";		kd.help = "langref/misc/rm_key.htm";
		pairIn.ix = str2fix("hs"); pairIn.narg = 2; pairOut.ix = 3; pairOut.narg = 1; kd.i2o[pairIn] = pairOut;
		Functions.push_back(kd);		resetFcn( kd );
	kd.hsh2_str_mat = AddKey;	kd.hsh2_str_cbe = AddKey;	kd.hsh2_str_str = AddKey;	kd.hsh2_str_hsh = AddKey;			kd.name = "add_key";	kd.help = "langref/misc/add_key.htm";
		pairIn.ix = str2fix("hsm"); pairIn.narg = 3; pairOut.ix = 3; pairOut.narg = 1; kd.i2o[pairIn] = pairOut;
		pairIn.ix = str2fix("hsc"); pairIn.narg = 3; pairOut.ix = 3; pairOut.narg = 1; kd.i2o[pairIn] = pairOut;
		pairIn.ix = str2fix("hss"); pairIn.narg = 3; pairOut.ix = 3; pairOut.narg = 1; kd.i2o[pairIn] = pairOut;
		pairIn.ix = str2fix("hsh"); pairIn.narg = 3; pairOut.ix = 3; pairOut.narg = 1; kd.i2o[pairIn] = pairOut;
		Functions.push_back(kd);		resetFcn( kd );
	kd.str_hsh_str = NextKey;			kd.name = "next_key";		kd.help = "langref/misc/next_key.htm";
		pairIn.ix = str2fix("hs"); pairIn.narg = 2; pairOut.ix = str2fix("s"); pairOut.narg = 1; kd.i2o[pairIn] = pairOut;
		Functions.push_back(kd);		resetFcn( kd );

	// file I/O
	kd.mat_str2 = ::FOpen;		kd.name = "fopen";					kd.help = "langref/file_io/fopen.htm";
		pairIn.ix = str2fix("ss"); pairIn.narg = 2; pairOut.ix = str2fix("m"); pairOut.narg = 1; kd.i2o[pairIn] = pairOut;
		Functions.push_back(kd);		resetFcn( kd );
	kd.mat_1 = ::FClose;	kd.name = "fclose";						kd.help = "langref/file_io/fclose.htm";
		pairIn.ix = str2fix("m"); pairIn.narg = 1; pairOut.ix = 0; pairOut.narg = 0; kd.i2o[pairIn] = pairOut;
		Functions.push_back(kd);		resetFcn( kd );
	kd.mat_0 = NULL;	kd.str_1 = ::Load;	kd.name = "load_cq";		kd.help = "langref/file_io/load_cq.htm";
		pairIn.ix = str2fix("s"); pairIn.narg = 1; pairOut.ix = 0; pairOut.narg = 0; kd.i2o[pairIn] = pairOut;
		Functions.push_back(kd);		resetFcn( kd );
	kd.mat_0 = SaveAll;	kd.str_1 = SaveAll;	kd.name = "save_all";		kd.help = "langref/file_io/save_all.htm";
		pairIn.ix = 0; pairIn.narg = 0; pairOut.ix = 0; pairOut.narg = 0; kd.i2o[pairIn] = pairOut;
		pairIn.ix = str2fix("s"); pairIn.narg = 1; pairOut.ix = 0; pairOut.narg = 0; kd.i2o[pairIn] = pairOut;
		Functions.push_back(kd);		resetFcn( kd );
	kd.sc = SaveSymbols;	kd.scc = SaveSymbols;	kd.vc = SaveSymbols;	kd.name = "save_cq";		kd.help = "langref/file_io/save_cq.htm";
		pairIn.ix = str2fix("s"); pairIn.narg = 1; pairIn.nUnkown = 1; pairOut.ix = 0; pairOut.narg = 0; kd.i2o[pairIn] = pairOut;
		Functions.push_back(kd);		resetFcn( kd );
		pairOut.nUnkown = 0;	// reset
	kd.mat_0 = NULL;	kd.mat3_str = ::FRead;	kd.name = "fread";		kd.help = "langref/file_io/fread.htm";
		pairIn.ix = str2fix("mms"); pairIn.narg = 3; pairOut.ix = str2fix("m"); pairOut.narg = 1; kd.i2o[pairIn] = pairOut;
		Functions.push_back(kd);		resetFcn( kd );
	kd.mat_0 = NULL;	kd.mat3_str = ::FWrite;	kd.name = "fwrite";		kd.help = "langref/file_io/fwrite.htm";
		pairIn.ix = str2fix("mmms"); pairIn.narg = 4; pairOut.ix = 0; pairOut.narg = 0; kd.i2o[pairIn] = pairOut;
		Functions.push_back(kd);		resetFcn( kd );
	kd.cms = ::FScanf;	kd.ccms = ::FScanf;		kd.cccms = ::FScanf;		kd.vcms = ::FScanf;		kd.name = "fscanf";		kd.help = "langref/file_io/fscanf.htm";
		pairIn.ix = str2fix("ms"); pairIn.narg = 2; pairOut.ix = 0; pairOut.nUnkown = 1; pairOut.narg = 0; kd.i2o[pairIn] = pairOut;
		Functions.push_back(kd);	resetFcn( kd );
		pairOut.nUnkown = 0;	// reset
	kd.mat2_str = ::FPrintf;	kd.mmsvc = ::FPrintf;	kd.name = "fprintf";		kd.help = "langref/file_io/fprintf.htm";
		pairIn.ix = str2fix("ms"); pairIn.narg = 2; pairIn.nUnkown = 1; pairOut.ix = str2fix("m"); pairOut.narg = 1; kd.i2o[pairIn] = pairOut;
		Functions.push_back(kd);	resetFcn( kd );
		pairIn.nUnkown = 0;		// reset
	kd.mat_0 = NULL;	kd.mat_str = ::LoadAscii;	kd.name = "load_text";			kd.help = "langref/file_io/load_text.htm";
		pairIn.ix = str2fix("s"); pairIn.narg = 1; pairOut.ix = str2fix("m"); pairOut.narg = 1; kd.i2o[pairIn] = pairOut;
		Functions.push_back(kd);		resetFcn( kd );
	kd.mat_0 = NULL;	kd.sc = ::SaveAscii;	kd.scc = ::SaveAscii;	kd.vc = ::SaveAscii;	kd.name = "save_text";			kd.help = "langref/file_io/save_text.htm";
		pairIn.ix = str2fix("s"); pairIn.narg = 1; pairIn.nUnkown = 1; pairOut.ix = 0; pairOut.narg = 0; kd.i2o[pairIn] = pairOut;
		Functions.push_back(kd);		resetFcn( kd );
		pairIn.nUnkown = 0;		// reset
	kd.str_2 = FileStr;		kd.name = "file_str";					kd.help = "langref/file_io/file_str.htm";
		pairIn.ix = str2fix("s"); pairIn.narg = 1; pairOut.ix = str2fix("s"); pairOut.narg = 1; kd.i2o[pairIn] = pairOut;
		Functions.push_back(kd);	resetFcn( kd );


	kd.mat_1 = Show;	kd.str_1 = Show;	kd.cbe_1 = Show;	kd.hsh_1 = Show;		kd.name = "show";	kd.help = "langref/misc/show.htm";
		pairIn.ix = 0; pairIn.narg = 1; pairIn.nUnkown = 0; pairOut.ix = 0; pairOut.narg = 0; kd.i2o[pairIn] = pairOut;
		pairIn.ix = 1; pairIn.narg = 1; pairIn.nUnkown = 0; pairOut.ix = 0; pairOut.narg = 0; kd.i2o[pairIn] = pairOut;
		pairIn.ix = 2; pairIn.narg = 1; pairIn.nUnkown = 0; pairOut.ix = 0; pairOut.narg = 0; kd.i2o[pairIn] = pairOut;
		pairIn.ix = 3; pairIn.narg = 1; pairIn.nUnkown = 0; pairOut.ix = 0; pairOut.narg = 0; kd.i2o[pairIn] = pairOut;
		Functions.push_back(kd);		resetFcn( kd );
	kd.str_1 = Printf;	kd.sc = Printf;	kd.scc = Printf;	kd.vc = Printf;	kd.name = "printf";		kd.help = "langref/misc/printf.htm";
		pairIn.ix = str2fix("s"); pairIn.narg = 1; pairIn.nUnkown = 1; pairOut.ix = 0; pairOut.narg = 0; kd.i2o[pairIn] = pairOut;
		//pairIn.ix = str2fix("s"); pairIn.narg = 1; pairIn.nUnkown = 1; pairOut.ix = 0; pairOut.narg = 0; kd.i2o[pairIn] = pairOut;
		Functions.push_back(kd);	resetFcn( kd );
		pairIn.nUnkown = 0;		// reset

	// system commands
#ifndef _CQUATE_RELEASE_BUILD
	kd.mat_0 = Rundir;	/*kd.str_1 = Edit;*/		kd.name = "rundir";		kd.help = "commands/rundir.htm";
		pairIn.ix = 0; pairIn.narg = 0; pairOut.ix = 0; pairOut.narg = 0; kd.i2o[pairIn] = pairOut;
		Commands.push_back(kd);		resetCmd( kd );
#endif
	//kd.mat_0 = ForceCrash;		kd.name = "crash_me";		kd.help = "commands/quit.htm";
	//	pairIn.ix = 0; pairIn.narg = 0; pairOut.ix = 0; pairOut.narg = 0; kd.i2o[pairIn] = pairOut;
	//	Commands.push_back(kd);		resetCmd( kd );
	kd.mat_0 = Quit;		kd.name = "quit";		kd.help = "commands/quit.htm";
		pairIn.ix = 0; pairIn.narg = 0; pairOut.ix = 0; pairOut.narg = 0; kd.i2o[pairIn] = pairOut;
		Commands.push_back(kd);		resetCmd( kd );
	//kd.mat_0 = CallHelp;		kd.str_1 = CallHelp;		kd.name = "help";		kd.help = "commands/help.htm";
	//	pairIn.ix = 0; pairIn.narg = 0; pairOut.ix = 0; pairOut.narg = 0; kd.i2o[pairIn] = pairOut;
	//	pairIn.ix = str2fix("s"); pairIn.narg = 1; pairOut.ix = 0; pairOut.narg = 0; kd.i2o[pairIn] = pairOut;
	//	Commands.push_back(kd);		resetCmd( kd );
	//kd.mat_0 = CallHelp;		kd.str_1 = CallHelp;		kd.name = "?";		kd.help = "commands/help.htm";
	//	pairIn.ix = 0; pairIn.narg = 0; pairOut.ix = 0; pairOut.narg = 0; kd.i2o[pairIn] = pairOut;
	//	pairIn.ix = str2fix("s"); pairIn.narg = 1; pairOut.ix = 0; pairOut.narg = 0; kd.i2o[pairIn] = pairOut;
	//	Commands.push_back(kd);		resetCmd( kd );
	kd.mat_0 = ShowPath;	kd.name = "path";		kd.help = "commands/path.htm";
		pairIn.ix = 0; pairIn.narg = 0; pairOut.ix = 0; pairOut.narg = 0; kd.i2o[pairIn] = pairOut;
		Commands.push_back(kd);		resetCmd( kd );
	kd.mat_0 = ShowPwd;	kd.name = "pwd";		kd.help = "commands/pwd.htm";
		pairIn.ix = 0; pairIn.narg = 0; pairOut.ix = 0; pairOut.narg = 0; kd.i2o[pairIn] = pairOut;
		Commands.push_back(kd);		resetCmd( kd );
	kd.str_1 = NewPath;	kd.name = "new_path";		kd.help = "commands/new_path.htm";
		pairIn.ix = str2fix("s"); pairIn.narg = 1; pairOut.ix = 0; pairOut.narg = 0; kd.i2o[pairIn] = pairOut;
		Commands.push_back(kd);		resetCmd( kd );
	kd.str_1 = DelPath;	kd.name = "rm_path";		kd.help = "commands/rm_path.htm";
		pairIn.ix = str2fix("s"); pairIn.narg = 1; pairOut.ix = 0; pairOut.narg = 0; kd.i2o[pairIn] = pairOut;
		Commands.push_back(kd);		resetCmd( kd );
	kd.str_1 = Cd;		kd.name = "cd";			kd.help = "commands/cd.htm";
		pairIn.ix = str2fix("s"); pairIn.narg = 1; pairOut.ix = 0; pairOut.narg = 0; kd.i2o[pairIn] = pairOut;
		Commands.push_back(kd);		resetCmd( kd );
	kd.mat_0 = Ls;	 kd.str_1 = Ls;	kd.name = "ls";		kd.help = "commands/ls.htm";
		pairIn.ix = 0;				pairIn.narg = 0; pairOut.ix = 0; pairOut.narg = 0; kd.i2o[pairIn] = pairOut;
		pairIn.ix = str2fix("s");	pairIn.narg = 1; pairOut.ix = 0; pairOut.narg = 0; kd.i2o[pairIn] = pairOut;
		Commands.push_back(kd);		resetCmd( kd );
	kd.mat_0 = Vls;		kd.name = "vls";		kd.help = "commands/vls.htm";
		pairIn.ix = 0; pairIn.narg = 0; pairOut.ix = 0; pairOut.narg = 0; kd.i2o[pairIn] = pairOut;
		Commands.push_back(kd);		resetCmd( kd );
	kd.str_1 = Delete;	kd.str_2 = Delete;	kd.str_3 = Delete;	kd.str_ni_0o = Delete;	kd.name = "rm_var";		kd.help = "commands/rm_var.htm";
		pairIn.ix = str2fix("s"); pairIn.narg = 1; pairOut.ix = 0; pairOut.narg = 0; kd.i2o[pairIn] = pairOut;
		pairIn.ix = str2fix("ss"); pairIn.narg = 2; pairOut.ix = 0; pairOut.narg = 0; kd.i2o[pairIn] = pairOut;
		pairIn.ix = str2fix("sss"); pairIn.narg = 3; pairOut.ix = 0; pairOut.narg = 0; kd.i2o[pairIn] = pairOut;
		pairIn.ix = 0; pairIn.narg = 0; pairIn.nUnkown = 1; pairOut.ix = 0; pairOut.narg = 0; kd.i2o[pairIn] = pairOut;
		Commands.push_back(kd);		resetCmd( kd );
		pairIn.nUnkown = pairOut.nUnkown = 0;	// reset
	kd.mat_0 = DeleteAll;	kd.name = "rm_all";		kd.help = "commands/rm_all.htm";
		pairIn.ix = 0; pairIn.narg = 0; pairOut.ix = 0; pairOut.narg = 0; kd.i2o[pairIn] = pairOut;
		Commands.push_back(kd);		resetCmd( kd );
	kd.mat_0 = Cleanup;	kd.name = "cleanup";	kd.help = "commands/cleanup.htm";
		pairIn.ix = 0; pairIn.narg = 0; pairOut.ix = 0; pairOut.narg = 0; kd.i2o[pairIn] = pairOut;
		Commands.push_back(kd);	resetCmd( kd );
	//kd.mat_0 = DbgGo;	kd.name = "dg_go";		kd.help = "commands/dg_go.htm";
	//	pairIn.ix = 0; pairIn.narg = 0; pairOut.ix = 0; pairOut.narg = 0; kd.i2o[pairIn] = pairOut;
	//	Commands.push_back(kd);		resetCmd( kd );
	//kd.mat_0 = DbgStop;	kd.name = "dg_stop";		kd.help = "commands/dg_stop.htm";
	//	pairIn.ix = 0; pairIn.narg = 0; pairOut.ix = 0; pairOut.narg = 0; kd.i2o[pairIn] = pairOut;
	//	Commands.push_back(kd);		resetCmd( kd );
	//kd.mat_0 = DbgStack;	kd.name = "dg_stack";		kd.help = "commands/dg_stack.htm";
	//	pairIn.ix = 0; pairIn.narg = 0; pairOut.ix = 0; pairOut.narg = 0; kd.i2o[pairIn] = pairOut;
	//	Commands.push_back(kd);		resetCmd( kd );
	kd.mat_0 = History;	kd.name = "history";		kd.help = "commands/history.htm";
		pairIn.ix = 0; pairIn.narg = 0; pairOut.ix = 0; pairOut.narg = 0; kd.i2o[pairIn] = pairOut;
		Commands.push_back(kd);		resetCmd( kd );

	kd.str_1 = Which;	kd.name = "which";		kd.help = "commands/which.htm";
		pairIn.ix = str2fix("s"); pairIn.narg = 1; pairOut.ix = 0; pairOut.narg = 0; kd.i2o[pairIn] = pairOut;
		Commands.push_back(kd);		resetCmd( kd );
	kd.mat_0 = Scripts;	kd.name = "scripts";		kd.help = "commands/scripts.htm";
		pairIn.ix = 0; pairIn.narg = 0; pairOut.ix = 0; pairOut.narg = 0; kd.i2o[pairIn] = pairOut;
		Commands.push_back(kd);		resetCmd( kd );
	kd.mat_0 = Fcns;	kd.name = "fcns";		kd.help = "commands/fcns.htm";
		pairIn.ix = 0; pairIn.narg = 0; pairOut.ix = 0; pairOut.narg = 0; kd.i2o[pairIn] = pairOut;
		Commands.push_back(kd);		resetCmd( kd );

//	kd.mat_0 = OpenFileIds;	kd.name = "file_ids";		kd.help = "commands/file_ids.htm";
//		pairIn.ix = 0; pairIn.narg = 0; pairOut.ix = 0; pairOut.narg = 0; kd.i2o[pairIn] = pairOut;
//		Commands.push_back(kd);		resetCmd( kd );

	// language keywords
	kd.name = "if";			kd.help = "langref/prog_ctrl/if.htm";	Language.push_back(kd);
	kd.name = "else";		kd.help = "langref/prog_ctrl/if.htm";	Language.push_back(kd);

	kd.name = "for";		kd.help = "langref/prog_ctrl/for.htm";	Language.push_back(kd);

	kd.name = "while";		kd.help = "langref/prog_ctrl/while.htm";	Language.push_back(kd);
	kd.name = "do";			kd.help = "langref/prog_ctrl/do.htm";	Language.push_back(kd);

	kd.name = "switch";		kd.help = "langref/prog_ctrl/switch.htm";	Language.push_back(kd);
	kd.name = "case";		kd.help = "langref/prog_ctrl/switch.htm";	Language.push_back(kd);
	kd.name = "default";	kd.help = "langref/prog_ctrl/switch.htm";	Language.push_back(kd);
	kd.name = "break";		kd.help = "langref/prog_ctrl/break.htm";	Language.push_back(kd);
	kd.name = "continue";	kd.help = "langref/prog_ctrl/continue.htm";	Language.push_back(kd);

	kd.name = "function";	kd.help = "langref/prog_ctrl/function.htm";	Language.push_back(kd);
	kd.name = "return";		kd.help = "langref/prog_ctrl/function.htm";	Language.push_back(kd);

	// create keyword table
	insertFcns( Functions );
	insertCmds( Commands );
	insertKeywords( Language );
}

cStorage::~cStorage()
{
	m_Spaces.clear();
}


void cStorage::resetFcn( keyData& kd )
{
	kd.help.clear();

	kd.mat_0 = NULL; kd.mat_0i_no = NULL; kd.mat_ni_0o = NULL; kd.str_1 = NULL; kd.str_0i_no = NULL;
	kd.mat_1 = NULL; kd.mat_1i_no = NULL; kd.mat_ni_1o = NULL; kd.str_2 = NULL; kd.str_1i_no = NULL;
	kd.mat_2 = NULL; kd.mat_2i_no = NULL; kd.str_ni_0o = NULL; kd.str_ni_1o = NULL; kd.str_3 = NULL; kd.str_2i_no = NULL;
	kd.mat_3 = NULL; kd.mat_3i_no = NULL;					   kd.str_4 = NULL; kd.str_3i_no = NULL;
	kd.mat_4 = NULL;

	kd.cbe_1 = NULL;     kd.cbe_2 = NULL;     kd.cbe_3 = NULL;     kd.cbe_4 = NULL;
	kd.cbe_0i_no = NULL; kd.cbe_1i_no = NULL; kd.cbe_2i_no = NULL; kd.cbe_3i_no = NULL;
	kd.cbe_ni_0o = NULL; kd.cbe_ni_1o = NULL;

	kd.cbe_cbe_2 = NULL;	kd.cbe_cbe_3 = NULL;	kd.cbe_cbe_4 = NULL;
	kd.mat_cbe_2 = NULL;	kd.mat_cbe_3 = NULL;

	kd.hsh_1 = NULL;

	kd.cbe_str = NULL;
	kd.cbe_str2 = NULL;
	kd.cbe2_mat = NULL;
	kd.mat_str = NULL;
	kd.mat_str2 = NULL;
	kd.mat_str2_mat = NULL;
	kd.mat2_str = NULL;
	kd.mat3_str = NULL;
	kd.str_mat = NULL;
	kd.str2_mat2 = NULL;
	kd.str2_mat = NULL;
	kd.str3_mat = NULL;
	kd.mat_hsh = NULL;
	kd.hsh_str = NULL;
	kd.str_hsh_str = NULL;
	kd.mat_hsh_str = NULL;
	kd.hsh2_str = NULL;
	kd.hsh2_str_mat = NULL;
	kd.hsh2_str_cbe = NULL;
	kd.hsh2_str_str = NULL;
	kd.hsh2_str_hsh = NULL;

	kd.svc = NULL;
	kd.c = NULL;
	kd.cc = NULL;
	kd.ccc = NULL;
	kd.vc = NULL;

	kd.sc = NULL;
	kd.scc = NULL;

	kd.svc = NULL;

	kd.cms = NULL;
	kd.ccms = NULL;
	kd.cccms = NULL;
	kd.vcms = NULL;

	kd.mmsvc = NULL;

	kd.msvc = NULL;

	kd.i2o.clear();

	//kd.hsh_get = NULL;	kd.hsh_set = NULL;
}

void cStorage::resetCmd( keyData& kd )
{
	//kd.mat_0 = NULL; kd.mat_0i_no = NULL; kd.mat_ni_0o = NULL; kd.str_1 = NULL; kd.str_0i_no = NULL;
	//kd.mat_1 = NULL; kd.mat_1i_no = NULL; kd.mat_ni_1o = NULL; kd.str_2 = NULL; kd.str_1i_no = NULL;
	//kd.mat_2 = NULL; kd.mat_2i_no = NULL; kd.str_ni_1o = NULL; kd.str_3 = NULL; kd.str_2i_no = NULL;
	//kd.mat_3 = NULL; kd.mat_3i_no = NULL;					   kd.str_4 = NULL; kd.str_3i_no = NULL;
	//kd.mat_4 = NULL;

	//kd.cbe_1 = NULL;     kd.cbe_2 = NULL;     kd.cbe_3 = NULL;     kd.cbe_4 = NULL;
	//kd.cbe_0i_no = NULL; kd.cbe_1i_no = NULL; kd.cbe_2i_no = NULL; kd.cbe_3i_no = NULL;
	//kd.cbe_ni_0o = NULL; kd.cbe_ni_1o = NULL;

	kd.mat_0 = NULL; kd.mat_0i_no = NULL; kd.mat_ni_0o = NULL; kd.str_1 = NULL; kd.str_0i_no = NULL;
	kd.mat_1 = NULL; kd.mat_1i_no = NULL; kd.mat_ni_1o = NULL; kd.str_2 = NULL; kd.str_1i_no = NULL;
	kd.mat_2 = NULL; kd.mat_2i_no = NULL; kd.str_ni_0o = NULL; kd.str_ni_1o = NULL; kd.str_3 = NULL; kd.str_2i_no = NULL;
	kd.mat_3 = NULL; kd.mat_3i_no = NULL;					   kd.str_4 = NULL; kd.str_3i_no = NULL;
	kd.mat_4 = NULL;

	kd.cbe_1 = NULL;     kd.cbe_2 = NULL;     kd.cbe_3 = NULL;     kd.cbe_4 = NULL;
	kd.cbe_0i_no = NULL; kd.cbe_1i_no = NULL; kd.cbe_2i_no = NULL; kd.cbe_3i_no = NULL;
	kd.cbe_ni_0o = NULL; kd.cbe_ni_1o = NULL;

	kd.cbe_cbe_2 = NULL;	kd.cbe_cbe_3 = NULL;	kd.cbe_cbe_4 = NULL;
	kd.mat_cbe_2 = NULL;	kd.mat_cbe_3 = NULL;

	kd.hsh_1 = NULL;

	kd.cbe_str = NULL;
	kd.cbe_str2 = NULL;
	kd.cbe2_mat = NULL;
	kd.mat_str = NULL;
	kd.mat_str2 = NULL;
	kd.mat_str2_mat = NULL;
	kd.mat2_str = NULL;
	kd.mat3_str = NULL;
	kd.str_mat = NULL;
	kd.str2_mat2 = NULL;
	kd.str2_mat = NULL;
	kd.str3_mat = NULL;
	kd.mat_hsh = NULL;
	kd.hsh_str = NULL;
	kd.str_hsh_str = NULL;
	kd.mat_hsh_str = NULL;
	kd.hsh2_str = NULL;
	kd.hsh2_str_mat = NULL;
	kd.hsh2_str_cbe = NULL;
	kd.hsh2_str_str = NULL;
	kd.hsh2_str_hsh = NULL;

	kd.svc = NULL;
	kd.c = NULL;
	kd.cc = NULL;
	kd.ccc = NULL;
	kd.vc = NULL;

	kd.sc = NULL;
	kd.scc = NULL;

	kd.svc = NULL;

	kd.cms = NULL;
	kd.ccms = NULL;
	kd.cccms = NULL;
	kd.vcms = NULL;

	kd.mmsvc = NULL;

	kd.msvc = NULL;

	kd.i2o.clear();
}

void cStorage::insertFcns( vector<keyData>& kds )
{
	keyData kd;
	
	for (int i=0; i<(int)kds.size(); ++i)
	{
		kd = kds[i];
		kd.nType = LEX_TYPE_KEYWORD_FUNCTION;
		m_KeywordTable[kd.name] = kd;

		m_help.add(kd.name, kd.help);
	}

}

void cStorage::insertCmds( vector<keyData>& kds )
{
	keyData kd;
	
	for (int i=0; i<(int)kds.size(); ++i)
	{
		kd = kds[i];
		kd.nType = LEX_TYPE_KEYWORD_COMMAND;
		m_CommandTable[kd.name] = kd;

		m_help.add(kd.name, kd.help);
	}
}

void cStorage::insertKeywords( vector<keyData>& kds )
{
	keyData kd;
	
	for (int i=0; i<(int)kds.size(); ++i)
	{
		kd = kds[i];
		kd.nType = LEX_TYPE_KEYWORD_LANGUAGE;
		m_KeywordTable[kd.name] = kd;

		m_help.add(kd.name, kd.help);
	}
}

bool cStorage::helpQuery(string& help_page, string& query)
{
	return m_help.lookup(help_page, query);
}

int cStorage::getIx()
{
	return m_ixStackPos;
}

bool cStorage::setIx(int ix)
{
	if (ix<0 || ix>=(int)m_Spaces.size())
		return true;

	m_ixStackPos = ix;
	return false;
}

// return pointer to hash of active space
cHash * cStorage::getHashPtr()
{
	int ix = getIx();

	if (m_Spaces.empty()) return NULL;

	return m_Spaces[ix].getHashPtr();
}

std::map<string, keyData> * cStorage::getKeywordPtr()
{
	return &m_KeywordTable;
}
std::map<string, keyData> * cStorage::getCommandPtr()
{
	return &m_CommandTable;
}

cScriptHash * cStorage::getScriptPtr()
{
	return &m_ScriptTable;
}

cFcnHash * cStorage::getFcnPtr()
{
	return &m_FcnTable;
}



//
// File I/O
//


void cStorage::Empty()
{
	m_Spaces.clear();
	cVariableSpace spc;
	m_Spaces.push_back(spc);
}


//bool cStorage::Lookup(unsigned int& Type, cMatrix *pM, string *pStr, string& Symbol)
//{
//	int i = getIx();
//	if (i < 0) return true;
//	return m_Spaces[i].Lookup( Type, pM, pStr, Symbol );
//}

bool cStorage::LookupEmpty(cContainer& v)
{
	int i = getIx();
	if (i < 0) return true;
	return m_Spaces[i].lookupEmpty( v );
}

bool cStorage::Lookup(cContainer& v, string& Symbol)
{
	int i = getIx();
	if (i < 0) return true;
	return m_Spaces[i].Lookup( v, Symbol );
}

bool cStorage::LookupInfo(string& Symbol)
{
	int type;
	int r, c;
	int i = getIx();
	if (i < 0) return true;
	return m_Spaces[i].LookupInfo(type, r, c, Symbol);
}

bool cStorage::LookupInfo(int& Type, int& rows, int& columns, string& Symbol)
{
	int i = getIx();
	if (i < 0) return true;
	return m_Spaces[i].LookupInfo(Type, rows, columns, Symbol);
}

bool cStorage::LookupInfo(int& Type, int& rows, int& columns, int& depth, string& Symbol)
{
	int i = getIx();
	if (i < 0) return true;
	return m_Spaces[i].LookupInfo(Type, rows, columns, depth, Symbol);
}

bool cStorage::LookupSize(int& Type, int& size, string& Symbol)
{
	int r, c, d;
	int i = getIx();
	if (i < 0) return true;
	if ( LookupInfo(Type, r, c, d, Symbol) )
		return true;

	size = r * c * d;
	return false;
}

void cStorage::Set(string& Symbol, cMatrix& A)
{
	int i = getIx();
	m_Spaces[i].Set( Symbol, A );
}

void cStorage::Set(string& Symbol, char *pch)
{
	int i = getIx();
	m_Spaces[i].Set( Symbol, pch );
}

void cStorage::Set(string& Symbol, cString& str)
{
	int i = getIx();
	m_Spaces[i].Set( Symbol, str );
}


void cStorage::Set(string& Symbol, cContainer& A)
{
	int i = getIx();
	m_Spaces[i].Set( Symbol, A );
}


bool cStorage::Set(string& Dest, string& Src)
{
	int i = getIx();
	return m_Spaces[i].Set( Dest, Src );
}

bool cStorage::Insert( vector<cContainer>& vars )
{
	string name;
	int ix = getIx();
	for (int i=0; i<(int)vars.size(); ++i)
	{
		name = vars[i].GetName();
		m_Spaces[ix].Set( name, vars[i] );
	}
	return false;
}

bool cStorage::Remove(string& Symbol)
{
	if (m_Spaces.empty()) return true;

	int i = getIx();
	return m_Spaces[i].Remove( Symbol );
}


void cStorage::Clear()
{
	if (m_Spaces.empty()) return;

	int i = getIx();
	return m_Spaces[i].Clear();
}


bool cStorage::IsDefinedSymbol(string& Symbol)
{
	if (m_Spaces.empty()) return false;

	int i = getIx();
	return m_Spaces[i].IsDefinedSymbol(Symbol);
}


int cStorage::GetNumSymbols(void)
{
	if (m_Spaces.empty()) return 0;

	int i = getIx();
	return m_Spaces[i].GetNumSymbols();
}


vector<string>* cStorage::GetNumericSymbols(void)
{
	if (m_Spaces.empty()) return NULL;

	int i = getIx();
	return m_Spaces[i].GetNumericSymbols();
}

vector<string>* cStorage::GetStringSymbols(void)
{
	if (m_Spaces.empty()) return NULL;

	int i = getIx();
	return m_Spaces[i].GetStringSymbols();
}

vector<string>* cStorage::GetSymbolList(void)
{
	if (m_Spaces.empty()) return NULL;

	int i = getIx();
	return m_Spaces[i].GetSymbolList();
}

// load a matrix in ASCII format
//
bool cStorage::LoadAscii(cMatrix& Result, string& fname)
{
	int i = getIx();
	return m_Spaces[i].LoadAscii(Result, fname);
}

// count the number of bytes in the first line of the ASCII file fname
//
int cStorage::TextLineLength(string& fname)
{
	int i = getIx();
	return m_Spaces[i].TextLineLength(fname);
}

// count the number of values in the line of text in *p_buffer
//
int cStorage::ScanLine( char *p_buffer )
{
	int i = getIx();
	return m_Spaces[i].ScanLine(p_buffer);
}


//bool cStorage::LoadDataVis(string& fname)
//{
//	int i = getIx();
//	return m_Spaces[i].LoadDataVis(fname);
//}

void cStorage::TrimWhite( string& New, const string Old )
{
	int i = getIx();
	m_Spaces[i].TrimWhite(New, Old);
}

bool cStorage::Load(string& fname)
{
	int i = getIx();
	return m_Spaces[i].Load(fname);
}

bool cStorage::Save(string& fname)
{
	int i = getIx();
	return m_Spaces[i].Save(fname);
}


bool cStorage::Save(vector<string> *pVarArray, string& fname)
{
	int i = getIx();
	return m_Spaces[i].Save( pVarArray, fname );
}


bool cStorage::SaveAscii(vector<string>& VarArray, string& fname)
{
	int i = getIx();
	return m_Spaces[i].SaveAscii( VarArray, fname );
}


//bool cStorage::SavePreferences(string& fname)
//{
//	CcmdView *plv = CcmdView::GetView();
//	CcirrusPlotDoc *pDoc = (CcirrusPlotDoc *) plv->GetDocument();
//	CString str;
//
//	FILE *pF = fopen(fname.c_str(), "wb");
//	if (pF == NULL) {
//		str.Format("Error writing preferences file: %s", fname.c_str());
//		AfxMessageBox(str);
//		return TRUE;
//	}
//
//	// graphic and color prefs
//	fwrite( &(pDoc->iGraphDragMode), sizeof(int), 1, pF );				// Graph data dragging property
//	fwrite( &(pDoc->MaxVerticesDrag), sizeof(int), 1, pF );				// Graph data dragging property
//	fwrite( &(plv->m_FocusColor), sizeof(COLORREF), 1, pF );
//	fwrite( &(plv->m_LostFocusColor), sizeof(COLORREF), 1, pF );
//	fwrite( &(plv->m_WarningColor), sizeof(COLORREF), 1, pF );
//	fwrite( &(plv->m_ErrorColor), sizeof(COLORREF), 1, pF );
//	fwrite( &(plv->m_NormalColor), sizeof(COLORREF), 1, pF );
//	fwrite( &(plv->m_TypedColor), sizeof(COLORREF), 1, pF );
//
//	// save working directory history
//	int N = pDoc->CWD_History.GetSize();
////	int N = 0;
//	fwrite( &N, sizeof(int), 1, pF );		// % of directories in the history
//
//	CString path;
//	int L;
//	char *pch;
//	for (int i=0; i<N; i++) {
//		path = pDoc->CWD_History.ElementAt(i);
//		L = path.GetLength();
//		pch = new char[L+1];
//		strcpy(pch, path);
//		fwrite( &L, sizeof(int), 1, pF );
//		fwrite( pch, sizeof(char), L, pF );
//		delete []pch;
//	}
//
//	// save splitter window pane positions
//	fwrite( &(pDoc->m_iDefaultValueViewWidth), sizeof(int), 1, pF);
//	fwrite( &(pDoc->m_iDefaultValueViewHeight), sizeof(int), 1, pF);
//	fwrite( &(pDoc->m_iDefaultMaxConsoleHeight), sizeof(int), 1, pF);
//
//	// save the default editor (incl. full path)
//	L = pDoc->m_EditorProgram.GetLength();
//	pch = new char[L+1];
//	strcpy( pch, pDoc->m_EditorProgram );
//	// write the data
//	fwrite( &L, sizeof(int), 1, pF );			// length of full path
//	if (L>0)
//		fwrite( pch, sizeof(char), L, pF );			// path chars
//
//	delete []pch;
//	fclose( pF );
//	return FALSE;
//}

//bool cStorage::LoadPreferences(string& fname)
//{
//	CcmdView *plv = CcmdView::GetView();
//	CcirrusPlotDoc *pDoc = (CcirrusPlotDoc *) plv->GetDocument();
//	CString str;
//
//	// get value view
//	POSITION rP = pDoc->GetFirstViewPosition();
//	CView *pv = pDoc->GetNextView( rP );
//	if ( !pv->IsKindOf( RUNTIME_CLASS(CDataView) ) ) {
//		pv = pDoc->GetNextView( rP );
//		if ( !pv->IsKindOf( RUNTIME_CLASS(CDataView) ) ) {
//			pv = pDoc->GetNextView( rP );
//			if ( !pv->IsKindOf( RUNTIME_CLASS(CDataView) ) ) {
//				pDoc->Error.Report(string("Couldn't find CDataView class"));
//				return TRUE;
//			}
//		}
//	}
//	CDataView *pvv = (CDataView *) pv;
//	// plv now points to the console view
//
//
//	FILE *pF = fopen(fname.c_str(), "rb");
//	if (pF == NULL) {
//		str.Format("Error reading preferences file: %s", fname.c_str());
//		AfxMessageBox(str);
//		return TRUE;
//	}
//
//	// graphic and color prefs
//	size_t nRet;
//	if ( fread( &(pDoc->iGraphDragMode), sizeof(int), 1, pF ) != 1 )	{ 	fclose( pF ); return TRUE; }		// Graph data dragging property
//	if ( fread( &(pDoc->MaxVerticesDrag), sizeof(int), 1, pF ) != 1 )	{ fclose( pF ); return TRUE; }				// Graph data dragging property
//	if ( fread( &(plv->m_FocusColor), sizeof(COLORREF), 1, pF ) != 1 )	{ fclose( pF ); return TRUE; }
//	if ( fread( &(plv->m_LostFocusColor), sizeof(COLORREF), 1, pF ) != 1 ) { fclose( pF ); return TRUE; }
//	if ( fread( &(plv->m_WarningColor), sizeof(COLORREF), 1, pF ) != 1 ) { fclose( pF ); return TRUE; }
//	if ( fread( &(plv->m_ErrorColor), sizeof(COLORREF), 1, pF ) != 1 )	{ fclose( pF ); return TRUE; }
//	if ( fread( &(plv->m_NormalColor), sizeof(COLORREF), 1, pF ) != 1 ) { fclose( pF ); return TRUE; }
//	if ( fread( &(plv->m_TypedColor), sizeof(COLORREF), 1, pF ) != 1 )	{ fclose( pF ); return TRUE; }
//
//	//pvv->SetFocusColor( plv->m_FocusColor );
//	//pvv->SetLostFocusColor( plv->m_LostFocusColor );
//
//	// load working directory history
//	int N, L;
//	if ( fread( &N, sizeof(int), 1, pF ) != 1 ) return TRUE;			// number of directories in history
//
//	char *pch;
//	CString path;
//	pDoc->CWD_History.RemoveAll();
//
//	for (int i=0; i<N; i++) {
//		fread( &L, sizeof(int), 1, pF );
//		pch = new char[L+1];
//		fread( pch, sizeof(char), L, pF );
//		pch[L] = '\0';
//		path = _T( pch );
//		pDoc->CWD_History.push_back(path);
//		delete []pch;
//	}
//
//	// load splitter window pane positions
//	fread( &(pDoc->m_iDefaultValueViewWidth), sizeof(int), 1, pF);
//	fread( &(pDoc->m_iDefaultValueViewHeight), sizeof(int), 1, pF);
//	fread( &(pDoc->m_iDefaultMaxConsoleHeight), sizeof(int), 1, pF);
//
//	pDoc->m_iDefaultValueViewWidth = ::Max( pDoc->m_iDefaultValueViewWidth, 1 );
//	pDoc->m_iDefaultValueViewHeight = ::Max( pDoc->m_iDefaultValueViewHeight, 1 );
//	pDoc->m_iDefaultMaxConsoleHeight = ::Max( pDoc->m_iDefaultMaxConsoleHeight, 1 );
//
//	// load the default editor (incl. full path)
//	fread( &L, sizeof(int), 1, pF );
//	if (L>0) {
//		pch = new char[L+1];
//		fread( pch, sizeof(char), L, pF );
//		pch[L] = '\0';				// null terminate
//		pDoc->m_EditorProgram = pch;
//		delete []pch;
//	}
//
//	fclose( pF );
//	return FALSE;
//}

/*
bool cStorage::LoadPreferences(string& fname)
{
	CcmdView *plv = CcmdView::GetView();
	CcirrusPlotDoc *pDoc = (CcirrusPlotDoc *) plv->GetDocument();
	CString str;

	FILE *pF = fopen(fname.c_str(), "rb");
	if (pF == NULL) {
		str.Format("Error reading preferences file: %s", fname.c_str());
		AfxMessageBox(str);
		return TRUE;
	}

	// graphic and color prefs
	size_t nRet;
	if ( fread( &(pDoc->iGraphDragMode), sizeof(int), 1, pF ) != 1 )	{ 	fclose( pF ); return TRUE; }		// Graph data dragging property
	if ( fread( &(pDoc->MaxVerticesDrag), sizeof(int), 1, pF ) != 1 )	{ fclose( pF ); return TRUE; }				// Graph data dragging property
	if ( fread( &(plv->m_FocusColor), sizeof(COLORREF), 1, pF ) != 1 )	{ fclose( pF ); return TRUE; }
	if ( fread( &(plv->m_LostFocusColor), sizeof(COLORREF), 1, pF ) != 1 ) { fclose( pF ); return TRUE; }
	if ( fread( &(plv->m_WarningColor), sizeof(COLORREF), 1, pF ) != 1 ) { fclose( pF ); return TRUE; }
	if ( fread( &(plv->m_ErrorColor), sizeof(COLORREF), 1, pF ) != 1 )	{ fclose( pF ); return TRUE; }
	if ( fread( &(plv->m_NormalColor), sizeof(COLORREF), 1, pF ) != 1 ) { fclose( pF ); return TRUE; }
	if ( fread( &(plv->m_TypedColor), sizeof(COLORREF), 1, pF ) != 1 )	{ fclose( pF ); return TRUE; }

	//pvv->SetFocusColor( plv->m_FocusColor );
	//pvv->SetLostFocusColor( plv->m_LostFocusColor );

	// load working directory history
	int N, L;
	if ( fread( &N, sizeof(int), 1, pF ) != 1 ) return TRUE;			// number of directories in history

	char *pch;
	string path;
	pDoc->m_WorkingDirHistory.clear();

	for (int i=0; i<N; i++) {
		fread( &L, sizeof(int), 1, pF );
		pch = new char[L+1];
		fread( pch, sizeof(char), L, pF );
		pch[L] = '\0';
		path =  pch;
		pDoc->m_WorkingDirHistory.push_back(path);
		delete []pch;
	}

	// load splitter window pane positions
	fread( &(pDoc->m_iDefaultValueViewWidth), sizeof(int), 1, pF);
	fread( &(pDoc->m_iDefaultValueViewHeight), sizeof(int), 1, pF);
	fread( &(pDoc->m_iDefaultMaxConsoleHeight), sizeof(int), 1, pF);

	pDoc->m_iDefaultValueViewWidth = ::Max( pDoc->m_iDefaultValueViewWidth, 1 );
	pDoc->m_iDefaultValueViewHeight = ::Max( pDoc->m_iDefaultValueViewHeight, 1 );
	pDoc->m_iDefaultMaxConsoleHeight = ::Max( pDoc->m_iDefaultMaxConsoleHeight, 1 );

	// load the default editor (incl. full path)
	fread( &L, sizeof(int), 1, pF );
	if (L>0) {
		pch = new char[L+1];
		fread( pch, sizeof(char), L, pF );
		pch[L] = '\0';				// null terminate
		pDoc->m_EditorProgram = pch;
		delete []pch;
	}

	fclose( pF );

	return false;
}
*/