
#include "cube.h"


//
// "=="
//

cCube cCube::operator==(const cCube& R) const
{
	cCube Result;
	if (Result.Create(GetRows(),GetColumns(),GetDepth())) return Result;
	int i;
	complex<double> c1, c2;

	for (i=0; i<GetSize(); i++) {
		GetC(c1,i);
		R.GetC(c2,i);
		Result.SetR(i, c1==c2 );
	}
	return Result;
}

cCube cCube::operator==(const complex<double>& x) const
{
	cCube Result;
	if (Result.Create(GetRows(),GetColumns(),GetDepth())) return Result;
	int i;
	complex<double> c1;

	for (i=0; i<GetSize(); i++) {
		GetC(c1,i);
		Result.SetR(i, c1==x );
	}
	return Result;
}

cCube cCube::operator==(const double d) const
{
	cCube Result;
	if (Result.Create(GetRows(),GetColumns(),GetDepth())) return Result;
	int i;
	complex<double> c1;

	for (i=0; i<GetSize(); i++) {
		GetC(c1,i);
		Result.SetR(i, c1==d );
	}
	return Result;
}

cCube operator==(const double d, const cCube R)
{
	cCube Result;
	if (Result.Create(R.GetRows(),R.GetColumns(),R.GetDepth())) return Result;
	int i;
	complex<double> c1;

	for (i=0; i<R.GetSize(); i++) {
		R.GetC(c1,i);
		Result.SetR(i, c1==d );
	}
	return Result;
}

cCube operator==(const complex<double>& x, const cCube R)
{
	cCube Result;
	if (Result.Create(R.GetRows(),R.GetColumns(),R.GetDepth())) return Result;
	int i;
	complex<double> c1;

	for (i=0; i<R.GetSize(); i++) {
		R.GetC(c1,i);
		Result.SetR(i, c1==x );
	}
	return Result;
}


//
// "!="
//

cCube cCube::operator!=(const cCube& R) const
{
	cCube Result;
	if (Result.Create(GetRows(),GetColumns(),GetDepth())) return Result;
	int i;
	complex<double> c1, c2;

	for (i=0; i<GetSize(); i++) {
		GetC(c1,i);
		R.GetC(c2,i);
		Result.SetR(i, c1!=c2 );
	}
	return Result;
}

cCube cCube::operator!=(const complex<double>& x) const
{
	cCube Result;
	if (Result.Create(GetRows(),GetColumns(),GetDepth())) return Result;
	int i;
	complex<double> c1;

	for (i=0; i<GetSize(); i++) {
		GetC(c1,i);
		Result.SetR(i, c1!=x );
	}
	return Result;
}

cCube cCube::operator!=(const double d) const
{
	cCube Result;
	if (Result.Create(GetRows(),GetColumns(),GetDepth())) return Result;
	int i;
	complex<double> c1;

	for (i=0; i<GetSize(); i++) {
		GetC(c1,i);
		Result.SetR(i, c1!=d );
	}
	return Result;
}

cCube operator!=(const double d, const cCube& R)
{
	cCube Result;
	if (Result.Create(R.GetRows(),R.GetColumns(),R.GetDepth())) return Result;
	int i;
	complex<double> c1;

	for (i=0; i<R.GetSize(); i++) {
		R.GetC(c1,i);
		Result.SetR(i, c1!=d );
	}
	return Result;
}

cCube operator!=(const complex<double>& x, const cCube& R)
{
	cCube Result;
	if (Result.Create(R.GetRows(),R.GetColumns(),R.GetDepth())) return Result;
	int i;
	complex<double> c1;

	for (i=0; i<R.GetSize(); i++) {
		R.GetC(c1,i);
		Result.SetR(i, c1!=x );
	}
	return Result;
}


//
// "<"
//

cCube cCube::operator<(const cCube& R) const
{
	cCube Result;
	if (Result.Create(GetRows(),GetColumns(),GetDepth())) return Result;
	int i;
	double d1, d2;

	for (i=0; i<GetSize(); ++i) {
		GetR(d1,i);
		R.GetR(d2,i);
		Result.SetR(i, d1<d2);
	}

	return Result;
}

cCube cCube::operator<(const complex<double>& x) const
{
	cCube Result;
	if (Result.Create(GetRows(),GetColumns(),GetDepth())) return Result;
	int i;
	double d1, d2 = x.real();

	for (i=0; i<GetSize(); ++i) {
		GetR(d1,i);
		Result.SetR(i, d1<d2);
	}

	return Result;
}

cCube cCube::operator<(const double d) const
{
	cCube Result;
	if (Result.Create(GetRows(),GetColumns(),GetDepth())) return Result;
	int i;
	double d1;

	for (i=0; i<GetSize(); ++i) {
		GetR(d1,i);
		Result.SetR(i, d1<d);
	}

	return Result;
}

cCube operator<(const double d, const cCube& R)
{
	cCube Result;
	if (Result.Create(R.GetRows(),R.GetColumns(),R.GetDepth())) return Result;
	int i;
	double d1;

	for (i=0; i<R.GetSize(); ++i) {
		R.GetR(d1,i);
		Result.SetR(i, d<d1);
	}

	return Result;
}

cCube operator<(const complex<double>& x, const cCube& R)
{
	cCube Result;
	if (Result.Create(R.GetRows(),R.GetColumns(),R.GetDepth())) return Result;
	int i;
	double d1, d2;
	d1 = x.real();

	for (i=0; i<R.GetSize(); ++i) {
		R.GetR(d2,i);
		Result.SetR(i, d1<d2);
	}

	return Result;
}


//
// ">"
//

cCube cCube::operator>(const cCube& R) const
{
	cCube Result;
	if (Result.Create(GetRows(),GetColumns(),GetDepth())) return Result;
	int i;
	double d1, d2;

	for (i=0; i<GetSize(); ++i) {
		GetR(d1,i);
		R.GetR(d2,i);
		Result.SetR(i, d1>d2);
	}

	return Result;
}

cCube cCube::operator>(const complex<double>& x) const
{
	cCube Result;
	if (Result.Create(GetRows(),GetColumns(),GetDepth())) return Result;
	int i;
	double d1, d2 = x.real();

	for (i=0; i<GetSize(); ++i) {
		GetR(d1,i);
		Result.SetR(i, d1>d2);
	}

	return Result;
}

cCube cCube::operator>(const double d) const
{
	cCube Result;
	if (Result.Create(GetRows(),GetColumns(),GetDepth())) return Result;
	int i;
	double d1;

	for (i=0; i<GetSize(); ++i) {
		GetR(d1,i);
		Result.SetR(i, d1>d);
	}

	return Result;
}

cCube operator>(const double d, const cCube& R)
{
	cCube Result;
	if (Result.Create(R.GetRows(),R.GetColumns(),R.GetDepth())) return Result;
	int i;
	double d1;

	for (i=0; i<R.GetSize(); ++i) {
		R.GetR(d1,i);
		Result.SetR(i, d>d1);
	}

	return Result;
}

cCube operator>(const complex<double>& x, const cCube& R)
{
	cCube Result;
	if (Result.Create(R.GetRows(),R.GetColumns(),R.GetDepth())) return Result;
	int i;
	double d1, d2;
	d1 = x.real();

	for (i=0; i<R.GetSize(); ++i) {
		R.GetR(d2,i);
		Result.SetR(i, d1>d2);
	}

	return Result;
}


//
// "<="
//

cCube cCube::operator<=(const cCube& R) const
{
	cCube Result;
	if (Result.Create(GetRows(),GetColumns(),GetDepth())) return Result;
	int i;
	double d1, d2;

	for (i=0; i<GetSize(); ++i) {
		GetR(d1,i);
		R.GetR(d2,i);
		Result.SetR(i, d1<=d2);
	}

	return Result;
}

cCube cCube::operator<=(const complex<double>& x) const
{
	cCube Result;
	if (Result.Create(GetRows(),GetColumns(),GetDepth())) return Result;
	int i;
	double d1, d2 = x.real();

	for (i=0; i<GetSize(); ++i) {
		GetR(d1,i);
		Result.SetR(i, d1<=d2);
	}

	return Result;
}

cCube cCube::operator<=(const double d) const
{
	cCube Result;
	if (Result.Create(GetRows(),GetColumns(),GetDepth())) return Result;
	int i;
	double d1;

	for (i=0; i<GetSize(); ++i) {
		GetR(d1,i);
		Result.SetR(i, d1<=d);
	}

	return Result;
}

cCube operator<=(const double d, const cCube& R)
{
	cCube Result;
	if (Result.Create(R.GetRows(),R.GetColumns(),R.GetDepth())) return Result;
	int i;
	double d1;

	for (i=0; i<R.GetSize(); ++i) {
		R.GetR(d1,i);
		Result.SetR(i, d<=d1);
	}

	return Result;
}

cCube operator<=(const complex<double>& x, const cCube& R)
{
	cCube Result;
	if (Result.Create(R.GetRows(),R.GetColumns(),R.GetDepth())) return Result;
	int i;
	double d1, d2;
	d1 = x.real();

	for (i=0; i<R.GetSize(); ++i) {
		R.GetR(d2,i);
		Result.SetR(i, d1<=d2);
	}

	return Result;
}


//
// ">="
//

cCube cCube::operator>=(const cCube& R) const
{
	cCube Result;
	if (Result.Create(GetRows(),GetColumns(),GetDepth())) return Result;
	int i;
	double d1, d2;

	for (i=0; i<GetSize(); ++i) {
		GetR(d1,i);
		R.GetR(d2,i);
		Result.SetR(i, d1>=d2);
	}

	return Result;
}

cCube cCube::operator>=(const complex<double>& x) const
{
	cCube Result;
	if (Result.Create(GetRows(),GetColumns(),GetDepth())) return Result;
	int i;
	double d1, d2 = x.real();

	for (i=0; i<GetSize(); ++i) {
		GetR(d1,i);
		Result.SetR(i, d1>=d2);
	}

	return Result;
}

cCube cCube::operator>=(const double d) const
{
	cCube Result;
	if (Result.Create(GetRows(),GetColumns(),GetDepth())) return Result;
	int i;
	double d1;

	for (i=0; i<GetSize(); ++i) {
		GetR(d1,i);
		Result.SetR(i, d1>=d);
	}

	return Result;
}

cCube operator>=(const double d, const cCube& R)
{
	cCube Result;
	if (Result.Create(R.GetRows(),R.GetColumns(),R.GetDepth())) return Result;
	int i;
	double d1;

	for (i=0; i<R.GetSize(); ++i) {
		R.GetR(d1,i);
		Result.SetR(i, d>=d1);
	}

	return Result;
}

cCube operator>=(const complex<double>& x, const cCube& R)
{
	cCube Result;
	if (Result.Create(R.GetRows(),R.GetColumns(),R.GetDepth())) return Result;
	int i;
	double d1, d2;
	d1 = x.real();

	for (i=0; i<R.GetSize(); ++i) {
		R.GetR(d2,i);
		Result.SetR(i, d1>=d2);
	}

	return Result;
}


//
// "&&" ("&")
//

cCube cCube::operator&&(const cCube& R) const
{
	cCube Result;
	if (Result.Create(GetRows(),GetColumns(),GetDepth())) return Result;
	int i;
	double d1, d2;

	for (i=0; i<GetSize(); ++i) {
		GetR(d1,i);
		R.GetR(d2,i);
		Result.SetR(i, d1&&d2);
	}

	return Result;
}

cCube cCube::operator&&(const complex<double>& x) const
{
	cCube Result;
	if (Result.Create(GetRows(),GetColumns(),GetDepth())) return Result;
	int i;
	double d1, d2 = x.real();

	for (i=0; i<GetSize(); ++i) {
		GetR(d1,i);
		Result.SetR(i, d1&&d2);
	}

	return Result;
}

cCube cCube::operator&&(const double d) const
{
	cCube Result;
	if (Result.Create(GetRows(),GetColumns(),GetDepth())) return Result;
	int i;
	double d1;

	for (i=0; i<GetSize(); ++i) {
		GetR(d1,i);
		Result.SetR(i, d1&&d);
	}

	return Result;
}

cCube operator&&(const double d, const cCube& R)
{
	cCube Result;
	if (Result.Create(R.GetRows(),R.GetColumns(),R.GetDepth())) return Result;
	int i;
	double d1;

	for (i=0; i<R.GetSize(); ++i) {
		R.GetR(d1,i);
		Result.SetR(i, d&&d1);
	}

	return Result;
}

cCube operator&&(const complex<double>& x, const cCube& R)
{
	cCube Result;
	if (Result.Create(R.GetRows(),R.GetColumns(),R.GetDepth())) return Result;
	int i;
	double d1, d2;
	d1 = x.real();

	for (i=0; i<R.GetSize(); ++i) {
		R.GetR(d2,i);
		Result.SetR(i, d1&&d2);
	}

	return Result;
}


//
// "||" ("|")
//

cCube cCube::operator||(const cCube& R) const
{
	cCube Result;
	if (Result.Create(GetRows(),GetColumns(),GetDepth())) return Result;
	int i;
	double d1, d2;

	for (i=0; i<GetSize(); ++i) {
		GetR(d1,i);
		R.GetR(d2,i);
		Result.SetR(i, d1||d2);
	}

	return Result;
}

cCube cCube::operator||(const complex<double>& x) const
{
	cCube Result;
	if (Result.Create(GetRows(),GetColumns(),GetDepth())) return Result;
	int i;
	double d1, d2 = x.real();

	for (i=0; i<GetSize(); ++i) {
		GetR(d1,i);
		Result.SetR(i, d1||d2);
	}

	return Result;
}

cCube cCube::operator||(const double d) const
{
	cCube Result;
	if (Result.Create(GetRows(),GetColumns(),GetDepth())) return Result;
	int i;
	double d1;

	for (i=0; i<GetSize(); ++i) {
		GetR(d1,i);
		Result.SetR(i, d1||d);
	}

	return Result;
}

cCube operator||(const double d, const cCube& R)
{
	cCube Result;
	if (Result.Create(R.GetRows(),R.GetColumns(),R.GetDepth())) return Result;
	int i;
	double d1;

	for (i=0; i<R.GetSize(); ++i) {
		R.GetR(d1,i);
		Result.SetR(i, d||d1);
	}

	return Result;
}

cCube operator||(const complex<double>& x, const cCube& R)
{
	cCube Result;
	if (Result.Create(R.GetRows(),R.GetColumns(),R.GetDepth())) return Result;
	int i;
	double d1, d2;
	d1 = x.real();

	for (i=0; i<R.GetSize(); ++i) {
		R.GetR(d2,i);
		Result.SetR(i, d1||d2);
	}

	return Result;
}

