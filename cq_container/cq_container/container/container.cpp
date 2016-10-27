#include "container.h"

// call
bool fcall(cVariable& v, cContainer& c)	{ return true; }
bool fcall(cVariable& v, cContainer& c1, cContainer& c2) { return true; }


cContainer::cContainer() {
	m_nType = -1;
}
cContainer::cContainer(const cContainer& C) {
	Delete();
	SetType( C.GetType() );
	switch (C.GetType()) {
	case VAR_TYPE_NUMERIC:
		m = C.m;
		c.Delete();
		s.Delete();
		h.Delete();
		break;

	case VAR_TYPE_CUBE:
		c = C.c;
		m.Delete();
		s.Delete();
		h.Delete();
		break;

	case VAR_TYPE_STRING:
		s = C.s;
		m.Delete();
		c.Delete();
		h.Delete();
		break;

	case VAR_TYPE_HASH:
		h = C.h;
		m.Delete();
		c.Delete();
		s.Delete();
		break;
	};
}
cContainer::~cContainer() { Delete(); }

//
// File I/O
//
bool WriteContainer(cContainer& C, FILE *pf)
{
	string name = C.GetName();

	int len = C.s.GetSize();

	if (fwrite( &len, sizeof(int), 1, pf ) != 1) return true;		// name length
	if (fwrite( C.s.s.data(), sizeof(char), len, pf ) != len) return true;	// name

	// container type
	int type = C.GetType();
	if (fwrite( &type, sizeof(int), 1, pf ) != 1) return true;		// type

	return WriteData(C, pf);
}

bool ReadContainer(cContainer& C, FILE *pf)
{
	C.Delete();

	int len;
	char *pch;

	if (fread( &len, sizeof(int), 1, pf ) != 1) return true;		// name length
	pch = new char[len+1];
	if (fread( pch, sizeof(char), len, pf ) != len) return true;	// name
	pch[len] = '\0';

	C.SetName(pch);
	delete []pch;

	// container type
	int type;
	if (fread( &type, sizeof(int), 1, pf ) != 1) return true;		// type

	C.SetType(type);

	return ReadData(C, pf);
}

bool WriteData(cContainer& C, FILE *pf)
{
	int type = C.GetType();

	switch (type) {
	case VAR_TYPE_NUMERIC:
		WriteData(C.m, pf);
		break;
	case VAR_TYPE_CUBE:
		WriteData(C.c, pf);
		break;
	case VAR_TYPE_STRING:
		WriteData(C.s, pf);
		break;
	case VAR_TYPE_HASH:
		WriteData(C.h.h, pf);	// a recursive call
		break;
	default:	// empty
		// NOP
		break;
	}

	return false;
}

bool ReadData(cContainer& C, FILE *pf)
{
	int type = C.GetType();

	switch (type) {
	case VAR_TYPE_NUMERIC:
		ReadData(C.m, pf);
		break;
	case VAR_TYPE_CUBE:
		ReadData(C.c, pf);
		break;
	case VAR_TYPE_STRING:
		ReadData(C.s, pf);
		break;
	case VAR_TYPE_HASH:
		ReadData(C.h.h, pf);	// a recursive call
		break;
	default:	// empty
		// NOP
		break;
	}

	return false;
}

//
// single arg access (rvalue)
//
bool containerAccess(cContainer& c, double dvar, double darg)
{
	c.SetType( VAR_TYPE_NUMERIC );
	return matAccess( c.m, dvar, darg );
}

//bool containerAccess(cContainer& c, cContainer& var, double darg)
//{
//	c.Delete();
//
//	switch (var.GetType()) {
//
//	case VAR_TYPE_NUMERIC: case VAR_TYPE_CUBE:
//		c.SetType( VAR_TYPE_NUMERIC );
//		return matAccess( c.m, var.m, darg );
//		
//	case VAR_TYPE_STRING:
//		c.SetType( VAR_TYPE_STRING );
//		return stringAccess( c.s, var.s, darg );
//
//	default:
//		addError("indexing not supported for this type");
//		return true;
//	}
//	return false;
//}

bool containerAccess(cContainer& c, double darg, cContainer& arg)
{
	if (arg.GetType() != VAR_TYPE_NUMERIC) { addError("illegal argument"); return true; }

	c.Delete();
	c.SetType( VAR_TYPE_NUMERIC );

	return matAccess( c.m, darg, arg.m );
}

//bool containerAccess(cContainer& c, cContainer& var, cContainer& arg)
//{
//	c.Delete();
//	if (arg.GetType()!=VAR_TYPE_NUMERIC) { addError("illegal argument"); return true; }
//
//	switch (var.GetType()) {
//
//	case VAR_TYPE_NUMERIC:
//		c.SetType( VAR_TYPE_NUMERIC );
//		return matAccess( c.m, var.m, arg.m );
//
//	case VAR_TYPE_STRING:
//		c.SetType( VAR_TYPE_STRING );
//		return stringAccess( c.s, var.s, arg.m );
//
//	default:
//		addError("indexing not supported for this type");
//		return true;
//	}
//
//	if (var.GetType()!=VAR_TYPE_NUMERIC) { addError("indexing not supported for this type"); return true; }
//
//	c.SetType( VAR_TYPE_NUMERIC );
//
//	return matAccess( c.m, var.m, arg.m );
//}

// arg is a colon, so return var as a vector
bool containerAccessColon(cContainer& c, double dvar)
{
	c.Delete();
	c.SetType(VAR_TYPE_NUMERIC);
	c.m.Create(1,1);
	c.m.mr[0] = dvar;
	return false;
}

bool containerAccessColon(cContainer& c, cContainer& var)
{
	c.Delete();
	int sz = var.GetSize();

	switch (var.GetType()) {

	case VAR_TYPE_NUMERIC:
		c.SetType( VAR_TYPE_NUMERIC );
		if (var.IsReal()) {
			c.m.Create(sz,1);
			c.m.mr = var.m.mr;
		}
		else {
			c.m.CreateComplex(sz,1);
			c.m.mr = var.m.mr; c.m.mi = var.m.mi;
		}
		break;

	case VAR_TYPE_CUBE:
		c.SetType( VAR_TYPE_CUBE );
		if (var.IsReal()) {
			c.m.Create(sz,1);
			c.m.mr = var.c.mr;
		}
		else {
			c.m.CreateComplex(sz,1);
			c.m.mr = var.c.mr; c.m.mi = var.c.mi;
		}
		break;

	case VAR_TYPE_STRING:
		c.SetType( VAR_TYPE_STRING );
		c.s = var.s;
		break;

	default:
		addError("illegal indexing"); return true;
	}

	return false;
	//if ( !(var.GetType()==VAR_TYPE_NUMERIC || var.GetType()==VAR_TYPE_CUBE) ) { addError("indexing not supported for this type"); return true; }

	//int sz = var.GetSize();

	//if (var.IsReal()) {
	//	c.m.Create(sz,1);
	//	if (var.GetType()==VAR_TYPE_NUMERIC) c.m.mr = var.m.mr;
	//	else c.m.mr = var.c.mr;
	//}
	//else {
	//	c.m.Create(sz,1);
	//	if (var.GetType()==VAR_TYPE_NUMERIC) { c.m.mr = var.m.mr; c.m.mi = var.m.mi; }
	//	else { c.m.mr = var.c.mr; c.m.mi = var.c.mi; }
	//}
}


//
// two arg access (rvalue)
//
bool containerAccess(cContainer& c, double dvar, double darg1, double darg2)
{
	c.Delete();
	c.SetType( VAR_TYPE_NUMERIC );

	return matAccess( c.m, dvar, darg1, darg2 );
}

bool containerAccess(cContainer& c, double dvar, cContainer& arg1, double darg2)
{
	if (arg1.GetType()!=VAR_TYPE_NUMERIC) { addError("illegal argument"); return true; }

	c.Delete();
	c.SetType( VAR_TYPE_NUMERIC );

	return matAccess( c.m, dvar, arg1.m, darg2 );
}

bool containerAccess(cContainer& c, double dvar, double darg1, cContainer& arg2)
{
	if (arg2.GetType()!=VAR_TYPE_NUMERIC) { addError("illegal argument"); return true; }

	c.Delete();
	c.SetType( VAR_TYPE_NUMERIC );

	return matAccess( c.m, dvar, darg1, arg2.m );
}

bool containerAccess(cContainer& c, double dvar, cContainer& arg1, cContainer& arg2)
{
	if (arg1.GetType()!=VAR_TYPE_NUMERIC) { addError("illegal argument"); return true; }
	if (arg2.GetType()!=VAR_TYPE_NUMERIC) { addError("illegal argument"); return true; }

	c.Delete();
	c.SetType( VAR_TYPE_NUMERIC );

	return matAccess( c.m, dvar, arg1.m, arg2.m );
}

//bool containerAccess(cContainer& c, cContainer& var, double darg1, double darg2)
//{
//	if (var.GetType()!=VAR_TYPE_NUMERIC) { addError("indexing not supported for this type"); return true; }
//
//	c.Delete();
//	c.SetType( VAR_TYPE_NUMERIC );
//
//	return matAccess( c.m, var.m, darg1, darg2 );
//}
//
//bool containerAccess(cContainer& c, cContainer& var, double darg1, cContainer& arg2)
//{
//	if (var.GetType()!=VAR_TYPE_NUMERIC) { addError("indexing not supported for this type"); return true; }
//	if (arg2.GetType()!=VAR_TYPE_NUMERIC) { addError("illegal argument"); return true; }
//
//	c.Delete();
//	c.SetType( VAR_TYPE_NUMERIC );
//
//	return matAccess( c.m, var.m, darg1, arg2.m );
//}
//
//bool containerAccess(cContainer& c, cContainer& var, cContainer& arg1, double darg2)
//{
//	if (var.GetType()!=VAR_TYPE_NUMERIC) { addError("illegal argument"); return true; }
//	if (arg1.GetType()!=VAR_TYPE_NUMERIC) { addError("illegal argument"); return true; }
//
//	c.Delete();
//	c.SetType( VAR_TYPE_NUMERIC );
//
//	return matAccess( c.m, var.m, arg1.m, darg2 );
//}
//
//bool containerAccess(cContainer& c, cContainer& var, cContainer& arg1, cContainer& arg2)
//{
//	if (var.GetType()!=VAR_TYPE_NUMERIC) { addError("illegal argument"); return true; }
//	if (arg1.GetType()!=VAR_TYPE_NUMERIC) { addError("illegal argument"); return true; }
//	if (arg2.GetType()!=VAR_TYPE_NUMERIC) { addError("illegal argument"); return true; }
//
//	c.Delete();
//	c.SetType( VAR_TYPE_NUMERIC );
//
//	return matAccess( c.m, var.m, arg1.m, arg2.m );
//}

bool arg2ix( cContainer& Result, cContainer& arg1, cContainer& arg2, int nRows, int nCols )
{	// convert values in arg1,arg2 to indices into a matrix of size (nRows,nCols)

	if (arg1.GetType()==VAR_TYPE_NUMERIC && arg2.GetType()==VAR_TYPE_NUMERIC)
	{
		Result.Delete();
		Result.SetType( VAR_TYPE_NUMERIC );
		return arg2ix( Result.m, arg1.m, arg2.m, nRows, nCols );
	}
	else {
		addError("illegal argument"); return true;
	}

	return false;
}

bool arg2ix( cContainer& Result, cContainer& arg1, cContainer& arg2, cContainer& arg3, int nRows, int nCols, int nDepth )
{	// convert values in arg1,arg2,arg3 to indices into a matrix of size (nRows,nCols)

	if (arg1.GetType()==VAR_TYPE_NUMERIC && arg2.GetType()==VAR_TYPE_NUMERIC && arg3.GetType()==VAR_TYPE_NUMERIC)
	{
		Result.Delete();
		Result.SetType( VAR_TYPE_CUBE );
		return arg2ix( Result.c, arg1.m, arg2.m, arg3.m, nRows, nCols, nDepth );
	}
	else {
		addError("illegal argument"); return true;
	}

	return false;
}


// arg is a colon, so return var as a vector
bool containerAccessColon(cContainer& c, double dvar, bool b1, double darg2)
{
	cMatrix m;
	m.SetToRange(0,1);
	cContainer arg1;
	arg1.SetTo(m);
	return containerAccess(c, dvar, arg1, darg2);
}

bool containerAccessColon(cContainer& c, double dvar, bool b1, cContainer& arg2)
{
	cMatrix m;
	m.SetToRange(0,1);
	cContainer arg1;
	arg1.SetTo(m);
	return containerAccess(c, dvar, arg1, arg2);
}

bool containerAccessColon(cContainer& c, double dvar, double darg1, bool b2)
{
	cMatrix m;
	m.SetToRange(0,1);
	cContainer arg2;
	arg2.SetTo(m);
	return containerAccess(c, dvar, darg1, arg2);
}

bool containerAccessColon(cContainer& c, double dvar, cContainer& arg1, bool b2)
{
	cMatrix m;
	m.SetToRange(0,1);
	cContainer arg2;
	arg2.SetTo(m);
	return containerAccess(c, dvar, arg1, arg2);
}

//bool containerAccessColon(cContainer& c, cContainer& var, bool b1, double darg2)
//{
//	cMatrix m;
//	m.SetToRange(0,var.GetRows());
//	cContainer arg1;
//	arg1.SetTo(m);
//	return containerAccess(c, var, arg1, darg2);
//}
//
//bool containerAccessColon(cContainer& c, cContainer& var, bool b1, cContainer& arg2)
//{
//	cMatrix m;
//	m.SetToRange(0,var.GetRows());
//	cContainer arg1;
//	arg1.SetTo(m);
//	return containerAccess(c, var, arg1, arg2);
//}
//
//bool containerAccessColon(cContainer& c, cContainer& var, double darg1, bool b2)
//{
//	cMatrix m;
//	m.SetToRange(0,var.GetColumns());
//	cContainer arg2;
//	arg2.SetTo(m);
//	return containerAccess(c, var, darg1, arg2);
//}
//
//bool containerAccessColon(cContainer& c, cContainer& var, cContainer& arg1, bool b2)
//{
//	cMatrix m;
//	m.SetToRange(0,var.GetColumns());
//	cContainer arg2;
//	arg2.SetTo(m);
//	return containerAccess(c, var, arg1, arg2);
//}

bool containerAccessColon(cContainer& c, double dvar, bool b1, bool b2)
{
	cMatrix m1, m2;
	m1.SetToRange(0,1);		m2.SetToRange(0,1);
	cContainer arg1, arg2;
	arg1.SetTo(m1);			arg2.SetTo(m2);
	return containerAccess(c, dvar, arg1, arg2);
}

//bool containerAccessColon(cContainer& c, cContainer& var, bool b1, bool b2)
//{
//	cMatrix m1, m2;
//	m1.SetToRange(0,var.GetRows());		m2.SetToRange(0,var.GetColumns());
//	cContainer arg1, arg2;
//	arg1.SetTo(m1);			arg2.SetTo(m2);
//	return containerAccess(c, var, arg1, arg2);
//}

//
// three arg access
//
// NOTE: if at least one argument is a double, then result will be a matrix (VAR_TYPE_NUMERIC)

bool containerAccess(cContainer& v, double dvar, double darg1, double darg2, double darg3)
{
	v.Delete();
	v.SetType( VAR_TYPE_NUMERIC );

	return cubeAccess( v.m, dvar, darg1, darg2, darg3 );
}

bool containerAccess(cContainer& v, double dvar, double darg1, double darg2, cContainer& arg3)
{
	if (arg3.GetType()!=VAR_TYPE_NUMERIC) { addError("illegal argument"); return true; }

	v.Delete();
	v.SetType( VAR_TYPE_NUMERIC );

	return cubeAccess( v.m, dvar, darg1, darg2, arg3.m );
}

bool containerAccess(cContainer& v, double dvar, double darg1, cContainer& arg2, double darg3)
{
	if (arg2.GetType()!=VAR_TYPE_NUMERIC) { addError("illegal argument"); return true; }

	v.Delete();
	v.SetType( VAR_TYPE_NUMERIC );

	return cubeAccess( v.m, dvar, darg1, arg2.m, darg3 );
}

bool containerAccess(cContainer& v, double dvar, double darg1, cContainer& arg2, cContainer& arg3)
{
	if (arg2.GetType()!=VAR_TYPE_NUMERIC || arg3.GetType()!=VAR_TYPE_NUMERIC ) { addError("illegal argument"); return true; }

	v.Delete();
	v.SetType( VAR_TYPE_NUMERIC );

	return cubeAccess( v.m, dvar, darg1, arg2.m, arg3.m );
}

bool containerAccess(cContainer& v, double dvar, cContainer& arg1, double darg2, double darg3)
{
	if (arg1.GetType()!=VAR_TYPE_NUMERIC) { addError("illegal argument"); return true; }

	v.Delete();
	v.SetType( VAR_TYPE_NUMERIC );

	return cubeAccess( v.m, dvar, arg1.m, darg2, darg3 );
}

bool containerAccess(cContainer& v, double dvar, cContainer& arg1, double darg2, cContainer& arg3)
{
	if (arg1.GetType()!=VAR_TYPE_NUMERIC || arg3.GetType()!=VAR_TYPE_NUMERIC) { addError("illegal argument"); return true; }

	v.Delete();
	v.SetType( VAR_TYPE_NUMERIC );

	return cubeAccess( v.m, dvar, arg1.m, darg2, arg3.m );
}

bool containerAccess(cContainer& v, double dvar, cContainer& arg1, cContainer& arg2, double darg3)
{
	if (arg1.GetType()!=VAR_TYPE_NUMERIC || arg2.GetType()!=VAR_TYPE_NUMERIC) { addError("illegal argument"); return true; }

	v.Delete();
	v.SetType( VAR_TYPE_NUMERIC );

	return cubeAccess( v.m, dvar, arg1.m, arg2.m, darg3 );
}

bool containerAccess(cContainer& v, double dvar, cContainer& arg1, cContainer& arg2, cContainer& arg3)
{
	if (arg1.GetType()!=VAR_TYPE_NUMERIC || arg2.GetType()!=VAR_TYPE_NUMERIC || arg3.GetType()!=VAR_TYPE_NUMERIC) {
		addError("illegal argument"); return true;
	}

	v.Delete();
	v.SetType( VAR_TYPE_NUMERIC );

	double lf;

	if (arg1.m.IsScalar()) {
		lf = arg1.m.Get0R();
		return cubeAccess( v.m, dvar, lf, arg2.m, arg3.m );
	}
	else if (arg2.m.IsScalar()) {
		lf = arg2.m.Get0R();
		return cubeAccess( v.m, dvar, arg1.m, lf, arg3.m );
	}
	else if (arg3.m.IsScalar()) {
		lf = arg1.m.Get0R();
		return cubeAccess( v.m, dvar, arg1.m, arg2.m, lf );
	}

	v.SetType( VAR_TYPE_CUBE );
	return cubeAccess( v.c, dvar, arg1.m, arg2.m, arg3.m );
}

/*
bool containerAccess(cContainer& v, cContainer& var, double darg1, double darg2, double darg3)
{
	v.Delete();
	v.SetType( VAR_TYPE_NUMERIC );

	return cubeAccess( v.m, var.c, darg1, darg2, darg3 );
}

bool containerAccess(cContainer& v, cContainer& var, double darg1, double darg2, cContainer& arg3)
{
	v.Delete();
	v.SetType( VAR_TYPE_NUMERIC );

	return cubeAccess( v.m, var.c, darg1, darg2, arg3.m );
}

bool containerAccess(cContainer& v, cContainer& var, double darg1, cContainer& arg2, double darg3)
{
	v.Delete();
	v.SetType( VAR_TYPE_NUMERIC );

	return cubeAccess( v.m, var.c, darg1, arg2.m, darg3 );
}

bool containerAccess(cContainer& v, cContainer& var, double darg1, cContainer& arg2, cContainer& arg3)
{
	if (arg2.GetType()!=VAR_TYPE_NUMERIC || arg3.GetType()!=VAR_TYPE_NUMERIC) { addError("illegal argument"); return true; }

	v.Delete();
	v.SetType( VAR_TYPE_NUMERIC );

	return cubeAccess( v.m, var.c, darg1, arg2.m, arg3.m );
}

bool containerAccess(cContainer& v, cContainer& var, cContainer& arg1, double darg2, double darg3)
{
	v.Delete();
	v.SetType( VAR_TYPE_NUMERIC );

	return cubeAccess( v.m, var.c, arg1.m, darg2, darg3 );
}

bool containerAccess(cContainer& v, cContainer& var, cContainer& arg1, double darg2, cContainer& arg3)
{
	if (arg1.GetType()!=VAR_TYPE_NUMERIC || arg3.GetType()!=VAR_TYPE_NUMERIC) { addError("illegal argument"); return true; }

	v.Delete();
	v.SetType( VAR_TYPE_NUMERIC );

	return cubeAccess( v.m, var.c, arg1.m, darg2, arg3.m );
}

bool containerAccess(cContainer& v, cContainer& var, cContainer& arg1, cContainer& arg2, double darg3)
{
	if (arg1.GetType()!=VAR_TYPE_NUMERIC || arg2.GetType()!=VAR_TYPE_NUMERIC) { addError("illegal argument"); return true; }

	v.Delete();
	v.SetType( VAR_TYPE_NUMERIC );

	return cubeAccess( v.m, var.c, arg1.m, arg2.m, darg3 );
}

bool containerAccess(cContainer& v, cContainer& var, cContainer& arg1, cContainer& arg2, cContainer& arg3)
{
	if (arg1.GetType()!=VAR_TYPE_NUMERIC || arg2.GetType()!=VAR_TYPE_NUMERIC || arg3.GetType()!=VAR_TYPE_NUMERIC) {
		addError("illegal argument"); return true;
	}

	v.Delete();
	v.SetType( VAR_TYPE_NUMERIC );

	double darg1, darg2, darg3;

	if (arg1.m.IsScalar()) {
		darg1 = arg1.m.Get0R();
		return cubeAccess( v.m, var.c, darg1, arg2.m, arg3.m );
	}
	else if (arg2.m.IsScalar()) {
		darg2 = arg2.m.Get0R();
		return cubeAccess( v.m, var.c, arg1.m, darg2, arg3.m );
	}
	else if (arg3.m.IsScalar()) {
		darg3 = arg3.m.Get0R();
		return cubeAccess( v.m, var.c, arg1.m, arg2.m, darg3 );
	}

	v.SetType( VAR_TYPE_CUBE );
	return cubeAccess( v.c, var.c, arg1.m, arg2.m, arg3.m );
}
*/

// one argument is a colon operator
//
// single colon
bool containerAccessColon(cContainer& c, double dvar, bool b1, double darg2, double darg3)
{
	cMatrix m;
	m.SetToRange(0,1);
	cContainer arg1;
	arg1.SetTo(m);
	return containerAccess(c, dvar, arg1, darg2, darg3);
}

bool containerAccessColon(cContainer& c, double dvar, bool b1, double darg2, cContainer& arg3)
{
	cMatrix m;
	m.SetToRange(0,1);
	cContainer arg1;
	arg1.SetTo(m);
	return containerAccess(c, dvar, arg1, darg2, arg3);
}

bool containerAccessColon(cContainer& c, double dvar, bool b1, cContainer& arg2, double darg3)
{
	cMatrix m;
	m.SetToRange(0,1);
	cContainer arg1;
	arg1.SetTo(m);
	return containerAccess(c, dvar, arg1, arg2, darg3);
}

bool containerAccessColon(cContainer& c, double dvar, bool b1, cContainer& arg2, cContainer& arg3)
{
	cMatrix m;
	m.SetToRange(0,1);
	cContainer arg1;
	arg1.SetTo(m);
	return containerAccess(c, dvar, arg1, arg2, arg3);
}

bool containerAccessColon(cContainer& c, double dvar, double darg1, bool b2, double darg3)
{
	cMatrix m;
	m.SetToRange(0,1);
	cContainer arg2;
	arg2.SetTo(m);
	return containerAccess(c, dvar, darg1, arg2, darg3);
}

bool containerAccessColon(cContainer& c, double dvar, double darg1, bool b2, cContainer& arg3)
{
	cMatrix m;
	m.SetToRange(0,1);
	cContainer arg2;
	arg2.SetTo(m);
	return containerAccess(c, dvar, darg1, arg2, arg3);
}

bool containerAccessColon(cContainer& c, double dvar, cContainer& arg1, bool b2, double darg3)
{
	cMatrix m;
	m.SetToRange(0,1);
	cContainer arg2;
	arg2.SetTo(m);
	return containerAccess(c, dvar, arg1, arg2, darg3);
}

bool containerAccessColon(cContainer& c, double dvar, cContainer& arg1, bool b2, cContainer& arg3)
{
	cMatrix m;
	m.SetToRange(0,1);
	cContainer arg2;
	arg2.SetTo(m);
	return containerAccess(c, dvar, arg1, arg2, arg3);
}

bool containerAccessColon(cContainer& c, double dvar, double darg1, double darg2, bool b3)
{
	cMatrix m;
	m.SetToRange(0,1);
	cContainer arg3;
	arg3.SetTo(m);
	return containerAccess(c, dvar, darg1, darg2, arg3);
}

bool containerAccessColon(cContainer& c, double dvar, double darg1, cContainer& arg2, bool b3)
{
	cMatrix m;
	m.SetToRange(0,1);
	cContainer arg3;
	arg3.SetTo(m);
	return containerAccess(c, dvar, darg1, arg2, arg3);
}

bool containerAccessColon(cContainer& c, double dvar, cContainer& arg1, double darg2, bool b3)
{
	cMatrix m;
	m.SetToRange(0,1);
	cContainer arg3;
	arg3.SetTo(m);
	return containerAccess(c, dvar, arg1, darg2, arg3);
}

bool containerAccessColon(cContainer& c, double dvar, cContainer& arg1, cContainer& arg2, bool b3)
{
	cMatrix m;
	m.SetToRange(0,1);
	cContainer arg3;
	arg3.SetTo(m);
	return containerAccess(c, dvar, arg1, arg2, arg3);
}

/*
bool containerAccessColon(cContainer& c, cContainer& var, bool b1, double darg2, double darg3)
{
	cMatrix m;
	m.SetToRange(0,var.GetRows());
	cContainer arg1;
	arg1.SetTo(m);
	return containerAccess(c, var, arg1, darg2, darg3);
}

bool containerAccessColon(cContainer& c, cContainer& var, bool b1, double darg2, cContainer& arg3)
{
	cMatrix m;
	m.SetToRange(0,var.GetRows());
	cContainer arg1;
	arg1.SetTo(m);
	return containerAccess(c, var, arg1, darg2, arg3);
}

bool containerAccessColon(cContainer& c, cContainer& var, bool b1, cContainer& arg2, double darg3)
{
	cMatrix m;
	m.SetToRange(0,var.GetRows());
	cContainer arg1;
	arg1.SetTo(m);
	return containerAccess(c, var, arg1, arg2, darg3);
}

bool containerAccessColon(cContainer& c, cContainer& var, bool b1, cContainer& arg2, cContainer& arg3)
{
	cMatrix m;
	m.SetToRange(0,var.GetRows());
	cContainer arg1;
	arg1.SetTo(m);
	return containerAccess(c, var, arg1, arg2, arg3);
}

bool containerAccessColon(cContainer& c, cContainer& var, double darg1, bool b2, double darg3)
{
	cMatrix m;
	m.SetToRange(0,var.GetColumns());
	cContainer arg2;
	arg2.SetTo(m);
	return containerAccess(c, var, darg1, arg2, darg3);
}

bool containerAccessColon(cContainer& c, cContainer& var, double darg1, bool b2, cContainer& arg3)
{
	cMatrix m;
	m.SetToRange(0,var.GetColumns());
	cContainer arg2;
	arg2.SetTo(m);
	return containerAccess(c, var, darg1, arg2, arg3);
}

bool containerAccessColon(cContainer& c, cContainer& var, cContainer& arg1, bool b2, double darg3)
{
	cMatrix m;
	m.SetToRange(0,var.GetColumns());
	cContainer arg2;
	arg2.SetTo(m);
	return containerAccess(c, var, arg1, arg2, darg3);
}

bool containerAccessColon(cContainer& c, cContainer& var, cContainer& arg1, bool b2, cContainer& arg3)
{
	cMatrix m;
	m.SetToRange(0,var.GetColumns());
	cContainer arg2;
	arg2.SetTo(m);
	return containerAccess(c, var, arg1, arg2, arg3);
}

bool containerAccessColon(cContainer& c, cContainer& var, double darg1, double darg2, bool b3)
{
	cMatrix m;
	m.SetToRange(0,var.GetDepth());
	cContainer arg3;
	arg3.SetTo(m);
	return containerAccess(c, var, darg1, darg2, arg3);
}

bool containerAccessColon(cContainer& c, cContainer& var, double darg1, cContainer& arg2, bool b3)
{
	cMatrix m;
	m.SetToRange(0,var.GetDepth());
	cContainer arg3;
	arg3.SetTo(m);
	return containerAccess(c, var, darg1, arg2, arg3);
}

bool containerAccessColon(cContainer& c, cContainer& var, cContainer& arg1, double darg2, bool b3)
{
	cMatrix m;
	m.SetToRange(0,var.GetDepth());
	cContainer arg3;
	arg3.SetTo(m);
	return containerAccess(c, var, arg1, darg2, arg3);
}

bool containerAccessColon(cContainer& c, cContainer& var, cContainer& arg1, cContainer& arg2, bool b3)
{
	cMatrix m;
	m.SetToRange(0,var.GetDepth());
	cContainer arg3;
	arg3.SetTo(m);
	return containerAccess(c, var, arg1, arg2, arg3);
}
*/

// two colons
bool containerAccessColon(cContainer& c, double dvar, bool b1, bool b2, double darg3)
{
	cMatrix m1, m2;
	m1.SetToRange(0,1);		m2.SetToRange(0,1);
	cContainer arg1, arg2;
	arg1.SetTo(m1);			arg2.SetTo(m2);
	return containerAccess(c, dvar, arg1, arg2, darg3);
}
bool containerAccessColon(cContainer& c, double dvar, bool b1, bool b2, cContainer& arg3)
{
	cMatrix m1, m2;
	m1.SetToRange(0,1);		m2.SetToRange(0,1);
	cContainer arg1, arg2;
	arg1.SetTo(m1);			arg2.SetTo(m2);
	return containerAccess(c, dvar, arg1, arg2, arg3);
}

bool containerAccessColon(cContainer& c, double dvar, bool b1, double darg2, bool b3)
{
	cMatrix m1, m3;
	m1.SetToRange(0,1);		m3.SetToRange(0,1);
	cContainer arg1, arg3;
	arg1.SetTo(m1);			arg3.SetTo(m3);
	return containerAccess(c, dvar, arg1, darg2, arg3);
}
bool containerAccessColon(cContainer& c, double dvar, bool b1, cContainer& arg2, bool b3)
{
	cMatrix m1, m3;
	m1.SetToRange(0,1);		m3.SetToRange(0,1);
	cContainer arg1, arg3;
	arg1.SetTo(m1);			arg3.SetTo(m3);
	return containerAccess(c, dvar, arg1, arg2, arg3);
}

bool containerAccessColon(cContainer& c, double dvar, double darg1, bool b2, bool b3)
{
	cMatrix m2, m3;
	m2.SetToRange(0,1);		m3.SetToRange(0,1);
	cContainer arg2, arg3;
	arg2.SetTo(m2);			arg3.SetTo(m3);
	return containerAccess(c, dvar, darg1, arg2, arg3);
}
bool containerAccessColon(cContainer& c, double dvar, cContainer& arg1, bool b2, bool b3)
{
	cMatrix m2, m3;
	m2.SetToRange(0,1);		m3.SetToRange(0,1);
	cContainer arg2, arg3;
	arg2.SetTo(m2);			arg3.SetTo(m3);
	return containerAccess(c, dvar, arg1, arg2, arg3);
}

/*
bool containerAccessColon(cContainer& c, cContainer& var, bool b1, bool b2, double darg3)
{
	cMatrix m1, m2;
	m1.SetToRange(0,var.GetRows());		m2.SetToRange(0,var.GetColumns());
	cContainer arg1, arg2;
	arg1.SetTo(m1);			arg2.SetTo(m2);
	return containerAccess(c, var, arg1, arg2, darg3);
}
bool containerAccessColon(cContainer& c, cContainer& var, bool b1, bool b2, cContainer& arg3)
{
	cMatrix m1, m2;
	m1.SetToRange(0,var.GetRows());		m2.SetToRange(0,var.GetColumns());
	cContainer arg1, arg2;
	arg1.SetTo(m1);			arg2.SetTo(m2);
	return containerAccess(c, var, arg1, arg2, arg3);
}

bool containerAccessColon(cContainer& c, cContainer& var, bool b1, double darg2, bool b3)
{
	cMatrix m1, m3;
	m1.SetToRange(0,var.GetRows());		m3.SetToRange(0,var.GetDepth());
	cContainer arg1, arg3;
	arg1.SetTo(m1);			arg3.SetTo(m3);
	return containerAccess(c, var, arg1, darg2, arg3);
}
bool containerAccessColon(cContainer& c, cContainer& var, bool b1, cContainer& arg2, bool b3)
{
	cMatrix m1, m3;
	m1.SetToRange(0,var.GetRows());		m3.SetToRange(0,var.GetDepth());
	cContainer arg1, arg3;
	arg1.SetTo(m1);			arg3.SetTo(m3);
	return containerAccess(c, var, arg1, arg2, arg3);
}

bool containerAccessColon(cContainer& c, cContainer& var, double darg1, bool b2, bool b3)
{
	cMatrix m2, m3;
	m2.SetToRange(0,var.GetColumns());		m3.SetToRange(0,var.GetDepth());
	cContainer arg2, arg3;
	arg2.SetTo(m2);			arg3.SetTo(m3);
	return containerAccess(c, var, darg1, arg2, arg3);
}
bool containerAccessColon(cContainer& c, cContainer& var, cContainer& arg1, bool b2, bool b3)
{
	cMatrix m2, m3;
	m2.SetToRange(0,var.GetColumns());		m3.SetToRange(0,var.GetDepth());
	cContainer arg2, arg3;
	arg2.SetTo(m2);			arg3.SetTo(m3);
	return containerAccess(c, var, arg1, arg2, arg3);
}
*/

// three colons
bool containerAccessColon(cContainer& c, double dvar, bool b1, bool b2, bool b3)
{
	c.SetTo(dvar);
	return false;
}
/*
bool containerAccessColon(cContainer& c, cContainer& var, bool b1, bool b2, bool b3)
{
	c = var;
	return false;
}
*/

// SetToRange tertiary
bool SetToRange( cContainer& v, double dStart, double dIncr, double dEnd )
{
	v.Delete();

	if (dStart==dEnd) { v.SetTo( dStart ); }
	else { return v.SetToRange( dStart, dEnd, dIncr ); }	// NOP if dIncr goes in wrong direction

	return false;
}

bool SetToRange( cContainer& v, double dStart, double dEnd )
{
	double dSpan = dEnd - dStart + 1.;
	double dIncr = 1.;

	return SetToRange( v, dStart, dIncr, dEnd );
}

bool SetToRange( cContainer& v, double d1, cContainer& v2 )
{
	double d2;
	if ( v2.GetSafe0(d2) ) return true;
	return SetToRange( v, d1, d2 );
}

bool SetToRange( cContainer& v, cContainer& v1, double d2 )
{
	double d1;
	if ( v1.GetSafe0(d1) ) return true;
	return SetToRange( v, d1, d2 );
}

bool SetToRange( cContainer& v, cContainer& v1, cContainer& v2 )
{
	double d1, d2;
	if ( v1.GetSafe0(d1) ) return true;
	if ( v2.GetSafe0(d2) ) return true;
	return SetToRange( v, d1, d2 );
}


bool SetToRange( cContainer& v, double d1, double d2, cContainer& vend )
{
	v.Delete();

	if (vend.IsScalar()) {	// ignore imag part
		double dend;
		if (vend.GetSafe0(dend)) { addError("illegal argument"); return true; }
		return SetToRange(v, d1, d2, dend);
	}

	addError("illegal argument"); return true;	// bad argument
}

bool SetToRange( cContainer& v, double d1, cContainer& v2, cContainer& vend )
{
	v.Delete();

	if (v2.IsScalar()) {	// ignore imag part
		double d2;
		if ( v2.GetSafe0(d2) ) { addError("illegal argument"); return true; }
		return SetToRange(v, d1, d2, vend);
	}

	addError("illegal argument"); return true;	// bad argument
}

bool SetToRange( cContainer& v, cContainer& v1, double d2, cContainer& vend )
{
	v.Delete();

	if (v1.IsScalar()) {	// ignore imag part
		double d1;
		if ( v1.GetSafe0(d1) ) { addError("illegal argument"); return true; }
		return SetToRange(v, d1, d2, vend);
	}

	addError("illegal argument"); return true;	// bad argument
}

bool SetToRange( cContainer& v, cContainer& v1, cContainer& v2, cContainer& vend )
{
	v.Delete();

	if (v1.IsScalar()) {	// ignore imag part
		double d1;
		if ( v1.GetSafe0(d1) ) { addError("illegal argument"); return true; }
		return SetToRange(v, d1, v2, vend);
	}

	addError("illegal argument"); return true;	// bad argument
}

bool SetToRange( cContainer& v, double d1, cContainer& v2, double dend )
{
	v.Delete();

	if (v2.IsScalar()) {	// ignore imag part
		double d2;
		if ( v2.GetSafe0(d2) ) { addError("illegal argument"); return true; }
		return SetToRange(v, d1, d2, dend);
	}

	addError("illegal argument"); return true;	// bad argument
}

bool SetToRange( cContainer& v, cContainer& v1, double d2, double dend )
{
	v.Delete();

	if (v1.IsScalar()) {	// ignore imag part
		double d1;
		if ( v1.GetSafe0(d1) ) { addError("illegal argument"); return true; }
		return SetToRange(v, d1, d2, dend);
	}

	addError("illegal argument"); return true;	// bad argument
}

bool SetToRange( cContainer& v, cContainer& v1, cContainer& v2, double dend )
{
	v.Delete();

	if (v1.IsScalar()) {	// ignore imag part
		double d1;
		if ( v1.GetSafe0(d1) ) { addError("illegal argument"); return true; }
		return SetToRange(v, d1, v2, dend);
	}

	addError("illegal argument"); return true;	// bad argument
}

//
// simple math
//
bool Add(cContainer& res, cContainer& a, cContainer& b)
{
	if (a.GetType()==VAR_TYPE_NUMERIC && b.GetType()==VAR_TYPE_NUMERIC)
	{
		res.Delete();
		res.SetType( VAR_TYPE_NUMERIC );
		return Add(res.m, a.m, b.m);
	}
	else if (a.GetType()==VAR_TYPE_CUBE && b.GetType()==VAR_TYPE_NUMERIC)
	{
		res.Delete();
		res.SetType( VAR_TYPE_CUBE );
		return Add(res.c, a.c, b.m);
	}
	else if (a.GetType()==VAR_TYPE_NUMERIC && b.GetType()==VAR_TYPE_CUBE)
	{
		res.Delete();
		res.SetType( VAR_TYPE_CUBE );
		return Add(res.c, b.c, a.m);
	}
	else if (a.GetType()==VAR_TYPE_CUBE && b.GetType()==VAR_TYPE_CUBE)
	{
		res.Delete();
		res.SetType( VAR_TYPE_CUBE );
		return Add(res.c, a.c, b.c);
	}
	else if (a.GetType()==VAR_TYPE_STRING && b.GetType()==VAR_TYPE_STRING) {
		res.Delete();
		res.SetType( VAR_TYPE_STRING );
		return HorzCat( res.s, a.s, b.s );
	}
	else {
		addError("incompatible types for addition");
		return true;
	}
}

bool Add(cContainer& res, cContainer& a, double b)
{
	if (a.GetType()==VAR_TYPE_NUMERIC)
	{
		res.Delete();
		res.SetType( VAR_TYPE_NUMERIC );
		return Add(res.m, a.m, b);
	}
	else if (a.GetType()==VAR_TYPE_CUBE)
	{
		res.Delete();
		res.SetType( VAR_TYPE_CUBE );
		return Add(res.c, a.c, b);
	}
	else {
		addError("incompatible types for addition");
		return true;
	}
}


bool Sub(cContainer& res, cContainer& a, cContainer& b)
{
	if (a.GetType()==VAR_TYPE_NUMERIC && b.GetType()==VAR_TYPE_NUMERIC)
	{
		res.Delete();
		res.SetType( VAR_TYPE_NUMERIC );
		return Sub(res.m, a.m, b.m);
	}
	else if (a.GetType()==VAR_TYPE_CUBE && b.GetType()==VAR_TYPE_NUMERIC)
	{
		res.Delete();
		res.SetType( VAR_TYPE_CUBE );
		return Sub(res.c, a.c, b.m);
	}
	else if (a.GetType()==VAR_TYPE_NUMERIC && b.GetType()==VAR_TYPE_CUBE)
	{
		res.Delete();
		res.SetType( VAR_TYPE_CUBE );
		return Sub(res.c, b.c, a.m);
	}
	else if (a.GetType()==VAR_TYPE_CUBE && b.GetType()==VAR_TYPE_CUBE)
	{
		res.Delete();
		res.SetType( VAR_TYPE_CUBE );
		return Sub(res.c, a.c, b.c);
	}
	else {
		addError("incompatible types for subtraction");
		return true;
	}
}

bool Sub(cContainer& res, cContainer& a, double b)
{
	if (a.GetType()==VAR_TYPE_NUMERIC)
	{
		res.Delete();
		res.SetType( VAR_TYPE_NUMERIC );
		return Sub(res.m, a.m, b);
	}
	else if (a.GetType()==VAR_TYPE_CUBE)
	{
		res.Delete();
		res.SetType( VAR_TYPE_CUBE );
		return Sub(res.c, a.c, b);
	}
	else {
		addError("incompatible types for subtraction");
		return true;
	}
}

bool Sub(cContainer& res, double a, cContainer& b)
{
	if (b.GetType()==VAR_TYPE_NUMERIC)
	{
		res.Delete();
		res.SetType( VAR_TYPE_NUMERIC );
		return Sub(res.m, a, b.m);
	}
	else if (b.GetType()==VAR_TYPE_CUBE)
	{
		res.Delete();
		res.SetType( VAR_TYPE_CUBE );
		return Sub(res.c, a, b.c);
	}
	else {
		addError("incompatible types for subtraction");
		return true;
	}
}

bool Mult(cContainer& res, cContainer& a, cContainer& b)
{
	if (a.GetType()==VAR_TYPE_NUMERIC && b.GetType()==VAR_TYPE_NUMERIC)
	{
		res.Delete();
		res.SetType( VAR_TYPE_NUMERIC );
		return Mult(res.m, a.m, b.m);
	}
	else {
		addError("incompatible types for multiplication");
		return true;
	}
}

bool Mult(cContainer& res, cContainer& a, double b)
{
	if (a.GetType()==VAR_TYPE_NUMERIC)
	{
		res.Delete();
		res.SetType( VAR_TYPE_NUMERIC );
		return Mult(res.m, a.m, b);
	}
	else if (a.GetType()==VAR_TYPE_CUBE)
	{
		res.Delete();
		res.SetType( VAR_TYPE_CUBE );
		return Mult(res.c, a.c, b);
	}
	else {
		addError("incompatible types for multiplication");
		return true;
	}
}

bool DotMult(cContainer& res, cContainer& a, cContainer& b)
{
	if (a.GetType()==VAR_TYPE_NUMERIC && b.GetType()==VAR_TYPE_NUMERIC)
	{
		res.Delete();
		res.SetType( VAR_TYPE_NUMERIC );
		return DotMult(res.m, a.m, b.m);
	}
	else if (a.GetType()==VAR_TYPE_CUBE && b.GetType()==VAR_TYPE_CUBE)
	{
		res.Delete();
		res.SetType( VAR_TYPE_CUBE );
		return DotMult(res.c, a.c, b.c);
	}
	else {
		addError("incompatible types for multiplication");
		return true;
	}
}

bool DotMult(cContainer& res, cContainer& a, double b)
{
	if (a.GetType()==VAR_TYPE_NUMERIC)
	{
		res.Delete();
		res.SetType( VAR_TYPE_NUMERIC );
		return Mult(res.m, a.m, b);
	}
	else if (a.GetType()==VAR_TYPE_CUBE)
	{
		res.Delete();
		res.SetType( VAR_TYPE_CUBE );
		return Mult(res.c, a.c, b);
	}
	else {
		addError("incompatible types for multiplication");
		return true;
	}
}

bool DotDiv(cContainer& res, cContainer& a, cContainer& b)
{
	if (a.GetType()==VAR_TYPE_NUMERIC && b.GetType()==VAR_TYPE_NUMERIC)
	{
		res.Delete();
		res.SetType( VAR_TYPE_NUMERIC );
		return DotDiv(res.m, a.m, b.m);
	}
	else if (a.GetType()==VAR_TYPE_CUBE && b.GetType()==VAR_TYPE_CUBE)
	{
		res.Delete();
		res.SetType( VAR_TYPE_CUBE );
		return DotDiv(res.c, a.c, b.c);
	}
	else {
		addError("incompatible types for division");
		return true;
	}
}

bool DotDiv(cContainer& res, cContainer& a, double b)
{
	if (a.GetType()==VAR_TYPE_NUMERIC)
	{
		res.Delete();
		res.SetType( VAR_TYPE_NUMERIC );
		return DotDiv(res.m, a.m, b);
	}
	else if (a.GetType()==VAR_TYPE_CUBE)
	{
		res.Delete();
		res.SetType( VAR_TYPE_CUBE );
		return DotDiv(res.c, a.c, b);
	}
	else {
		addError("incompatible types for division");
		return true;
	}
}

bool DotDiv(cContainer& res, double a, cContainer& b)
{
	if (b.GetType()==VAR_TYPE_NUMERIC)
	{
		res.Delete();
		res.SetType( VAR_TYPE_NUMERIC );
		return DotDiv(res.m, a, b.m);
	}
	else if (b.GetType()==VAR_TYPE_CUBE)
	{
		res.Delete();
		res.SetType( VAR_TYPE_CUBE );
		return DotDiv(res.c, a, b.c);
	}
	else {
		addError("incompatible types for division");
		return true;
	}
}

bool Mod(cContainer& res, cContainer& a, cContainer& b)
{
	if (a.GetType()==VAR_TYPE_NUMERIC && b.GetType()==VAR_TYPE_NUMERIC)
	{
		res.Delete();
		res.SetType( VAR_TYPE_NUMERIC );
		return Mod(res.m, a.m, b.m);
	}
	if (a.GetType()==VAR_TYPE_CUBE && b.GetType()==VAR_TYPE_CUBE)
	{
		res.Delete();
		res.SetType( VAR_TYPE_CUBE );
		return Mod(res.c, a.c, b.c);
	}
	else {
		addError("incompatible types for mod");
		return true;
	}
}

bool Mod(cContainer& res, double a, cContainer& b)
{
	if (b.GetType()==VAR_TYPE_NUMERIC)
	{
		res.Delete();
		res.SetType( VAR_TYPE_NUMERIC );
		return Mod(res.m, a, b.m);
	}
	else if (b.GetType()==VAR_TYPE_CUBE)
	{
		res.Delete();
		res.SetType( VAR_TYPE_CUBE );
		return Mod(res.c, a, b.c);
	}
	else {
		addError("incompatible types for mod");
		return true;
	}
}

bool Mod(cContainer& res, cContainer& a, double b)
{
	if (a.GetType()==VAR_TYPE_NUMERIC)
	{
		res.Delete();
		res.SetType( VAR_TYPE_NUMERIC );
		return Mod(res.m, a.m, b);
	}
	else if (a.GetType()==VAR_TYPE_CUBE)
	{
		res.Delete();
		res.SetType( VAR_TYPE_CUBE );
		return Mod(res.c, a.c, b);
	}
	else {
		addError("incompatible types for mod");
		return true;
	}
}

bool DotPow(cContainer& res, cContainer& a, cContainer& b)
{
	if (a.GetType()==VAR_TYPE_NUMERIC && b.GetType()==VAR_TYPE_NUMERIC)
	{
		res.Delete();
		res.SetType( VAR_TYPE_NUMERIC );
		return DotPow(res.m, a.m, b.m);
	}
	else if (a.GetType()==VAR_TYPE_CUBE && b.GetType()==VAR_TYPE_CUBE)
	{
		res.Delete();
		res.SetType( VAR_TYPE_CUBE );
		return DotPow(res.c, a.c, b.c);
	}
	else {
		addError("incompatible types for exponential");
		return true;
	}
}

bool DotPow(cContainer& res, double a, cContainer& b)
{
	if (b.GetType()==VAR_TYPE_NUMERIC)
	{
		res.Delete();
		res.SetType( VAR_TYPE_NUMERIC );
		return DotPow(res.m, a, b.m);
	}
	else if (b.GetType()==VAR_TYPE_CUBE)
	{
		res.Delete();
		res.SetType( VAR_TYPE_CUBE );
		return DotPow(res.c, a, b.c);
	}
	else {
		addError("incompatible types for exponential");
		return true;
	}
}

bool DotPow(cContainer& res, cContainer& a, double b)
{
	if (a.GetType()==VAR_TYPE_NUMERIC)
	{
		res.Delete();
		res.SetType( VAR_TYPE_NUMERIC );
		return DotPow(res.m, a.m, b);
	}
	else if (a.GetType()==VAR_TYPE_CUBE)
	{
		res.Delete();
		res.SetType( VAR_TYPE_CUBE );
		return DotPow(res.c, a.c, b);
	}
	else {
		addError("incompatible types for exponential");
		return true;
	}
}

bool Herm(cContainer& res, cContainer& a)
{
	if (a.GetType()==VAR_TYPE_NUMERIC)
	{
		res.Delete();
		res.SetType( VAR_TYPE_NUMERIC );
		return Herm(res.m, a.m);
	}
	else return true;
}

bool Not(cContainer& res, cContainer& a)
{
	if (a.GetType()==VAR_TYPE_NUMERIC)
	{
		res.Delete();
		res.SetType( VAR_TYPE_NUMERIC );
		return Not(res.m, a.m);
	}
	else if (a.GetType()==VAR_TYPE_CUBE)
	{
		res.Delete();
		res.SetType( VAR_TYPE_CUBE );
		return Not(res.c, a.c);
	}
	else return true;
}

bool VertCat(cContainer& res, cContainer& a, cContainer& b)
{
	if (a.GetType()==VAR_TYPE_NUMERIC && b.GetType()==VAR_TYPE_NUMERIC)
	{
		res.Delete();
		res.SetType( VAR_TYPE_NUMERIC );
		return VertCat(res.m, a.m, b.m);
	}
	else return true;

	return false;
}

bool VertCat(cContainer& res, double a, cContainer& b)
{
	if (b.GetType()==VAR_TYPE_NUMERIC)
	{
		res.Delete();
		res.SetType( VAR_TYPE_NUMERIC );
		return VertCat(res.m, a, b.m);
	}
	else return true;
}

bool VertCat(cContainer& res, cContainer& a, double b)
{
	if (a.GetType()==VAR_TYPE_NUMERIC)
	{
		res.Delete();
		res.SetType( VAR_TYPE_NUMERIC );
		return VertCat(res.m, a.m, b);
	}
	else return true;
}


bool VertCat(cContainer& res, double a, double b)
{
	res.Delete();
	res.SetType( VAR_TYPE_NUMERIC );
	return VertCat(res.m, a, b);
}


bool HorzCat(cContainer& res, cContainer& a, cContainer& b)
{
	if (a.GetType()==VAR_TYPE_NUMERIC && b.GetType()==VAR_TYPE_NUMERIC)
	{
		res.Delete();
		res.SetType( VAR_TYPE_NUMERIC );
		return HorzCat(res.m, a.m, b.m);
	}
	else if (a.GetType()==VAR_TYPE_STRING && b.GetType()==VAR_TYPE_STRING)
	{
		res.Delete();
		res.SetType( VAR_TYPE_STRING );
		return Append(res.s, a.s, b.s);
	}
	else return true;
}

bool HorzCat(cContainer& res, double a, cContainer& b)
{
	if (b.GetType()==VAR_TYPE_NUMERIC)
	{
		res.Delete();
		res.SetType( VAR_TYPE_NUMERIC );
		return HorzCat(res.m, a, b.m);
	}
	else return true;
}

bool HorzCat(cContainer& res, cContainer& a, double b)
{
	if (a.GetType()==VAR_TYPE_NUMERIC)
	{
		res.Delete();
		res.SetType( VAR_TYPE_NUMERIC );
		return HorzCat(res.m, a.m, b);
	}
	else return true;
}

bool HorzCat(cContainer& res, double a, double b)
{
	res.Delete();
	res.SetType(VAR_TYPE_NUMERIC);
	return HorzCat(res.m, a, b);
}
