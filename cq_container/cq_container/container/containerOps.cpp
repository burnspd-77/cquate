#include "container.h"

// Logical operators
//
bool EqualTo(cContainer& res, cContainer& c1, cContainer& c2)
{
	if (c1.IsEmpty() || c2.IsEmpty()) {
		addError("illegal operation (empty argument)");
		return true;
	}
	if (c1.GetType() != c2.GetType()) {
		addError("illegal operation (incopatible arguments)");
		return true;
	}

	bool bs;
	res.Delete();

	switch (c1.GetType()) {
	case VAR_TYPE_NUMERIC:
		res.SetType( VAR_TYPE_NUMERIC );
		res.m = c1.m == c2.m;
		break;

	case VAR_TYPE_CUBE:
		res.SetType( VAR_TYPE_CUBE );
		res.c = c1.c == c2.c;
		break;

	case VAR_TYPE_STRING:
		bs = c1.s == c2.s;
		res.SetTo(bs);
		break;

	default:
		addError("illegal operation (incopatible arguments)");
		return true;
	};

	return false;
}

bool EqualTo(cContainer& res, cContainer& c1, const complex<double>& x)
{
	if (c1.IsEmpty()) {
		addError("illegal operation (empty argument)");
		return true;
	}

	switch (c1.GetType()) {
	case VAR_TYPE_NUMERIC:
		res.SetType( VAR_TYPE_NUMERIC );
		res.m = c1.m == x;
		break;

	case VAR_TYPE_CUBE:
		res.SetType( VAR_TYPE_CUBE );
		res.c = c1.c == x;
		break;

	default:
		addError("illegal operation (incopatible arguments)");
		return true;
	};

	return false;
}

bool EqualTo(cContainer& res, cContainer& c1, const double d)
{
	if (c1.IsEmpty()) {
		addError("illegal operation (empty argument)");
		return true;
	}

	switch (c1.GetType()) {
	case VAR_TYPE_NUMERIC:
		res.SetType( VAR_TYPE_NUMERIC );
		res.m = c1.m == d;
		break;

	case VAR_TYPE_CUBE:
		res.SetType( VAR_TYPE_CUBE );
		res.c = c1.c == d;
		break;

	default:
		addError("illegal operation (incopatible arguments)");
		return true;
	};

	return false;
}

bool EqualTo(cContainer& res, const double d, cContainer& c1)
{
	return EqualTo(res,c1,d);
}

bool EqualTo(cContainer& res, const complex<double>& x, cContainer& c1)
{
	return EqualTo(res,c1,x);
}

bool NotEqualTo(cContainer& res, cContainer& c1, cContainer& c2)
{
	if (c1.IsEmpty() || c2.IsEmpty()) {
		addError("illegal operation (empty argument)");
		return true;
	}
	if (c1.GetType() != c2.GetType()) {
		addError("illegal operation (incopatible arguments)");
		return true;
	}

	bool bs;
	res.Delete();

	switch (c1.GetType()) {
	case VAR_TYPE_NUMERIC:
		res.SetType( VAR_TYPE_NUMERIC );
		res.m = c1.m != c2.m;
		break;

	case VAR_TYPE_CUBE:
		res.SetType( VAR_TYPE_CUBE );
		res.c = c1.c != c2.c;
		break;

	case VAR_TYPE_STRING:
		bs = c1.s != c2.s;
		res.SetTo(bs);
		break;

	default:
		addError("illegal operation (incopatible arguments)");
		return true;
	};

	return false;
}

bool NotEqualTo(cContainer& res, cContainer& c1, const complex<double>& x)
{
	if (c1.IsEmpty()) {
		addError("illegal operation (empty argument)");
		return true;
	}

	switch (c1.GetType()) {
	case VAR_TYPE_NUMERIC:
		res.SetType( VAR_TYPE_NUMERIC );
		res.m = c1.m != x;
		break;

	case VAR_TYPE_CUBE:
		res.SetType( VAR_TYPE_CUBE );
		res.c = c1.c != x;
		break;

	default:
		addError("illegal operation (incopatible arguments)");
		return true;
	};

	return false;
}

bool NotEqualTo(cContainer& res, cContainer& c1, const double d)
{
	if (c1.IsEmpty()) {
		addError("illegal operation (empty argument)");
		return true;
	}

	switch (c1.GetType()) {
	case VAR_TYPE_NUMERIC:
		res.SetType( VAR_TYPE_NUMERIC );
		res.m = c1.m != d;
		break;

	case VAR_TYPE_CUBE:
		res.SetType( VAR_TYPE_CUBE );
		res.c = c1.c != d;
		break;

	default:
		addError("illegal operation (incopatible arguments)");
		return true;
	};

	return false;
}

bool NotEqualTo(cContainer& res, const double d, cContainer& c1)
{
	return NotEqualTo(res,c1,d);
}

bool NotEqualTo(cContainer& res, const complex<double>& x, cContainer& c1)
{
	return NotEqualTo(res,c1,x);
}

bool LessThan(cContainer& res, cContainer& c1, cContainer& c2)
{
	if (c1.IsEmpty() || c2.IsEmpty()) {
		addError("illegal operation (empty argument)");
		return true;
	}
	if (c1.GetType() != c2.GetType()) {
		addError("illegal operation (incopatible arguments)");
		return true;
	}

	bool bs;
	res.Delete();

	switch (c1.GetType()) {
	case VAR_TYPE_NUMERIC:
		res.SetType( VAR_TYPE_NUMERIC );
		res.m = c1.m < c2.m;
		break;

	case VAR_TYPE_CUBE:
		res.SetType( VAR_TYPE_CUBE );
		res.c = c1.c < c2.c;
		break;

	case VAR_TYPE_STRING:
		bs = c1.s < c2.s;
		res.SetTo(bs);
		break;

	default:
		addError("illegal operation (incopatible arguments)");
		return true;
	};

	return false;
}

bool LessThan(cContainer& res, cContainer& c1, const complex<double>& x)
{
	if (c1.IsEmpty()) {
		addError("illegal operation (empty argument)");
		return true;
	}

	switch (c1.GetType()) {
	case VAR_TYPE_NUMERIC:
		res.SetType( VAR_TYPE_NUMERIC );
		res.m = c1.m < x;
		break;

	case VAR_TYPE_CUBE:
		res.SetType( VAR_TYPE_CUBE );
		res.c = c1.c < x;
		break;

	default:
		addError("illegal operation (incopatible arguments)");
		return true;
	};

	return false;
}

bool LessThan(cContainer& res, cContainer& c1, const double d)
{
	if (c1.IsEmpty()) {
		addError("illegal operation (empty argument)");
		return true;
	}

	switch (c1.GetType()) {
	case VAR_TYPE_NUMERIC:
		res.SetType( VAR_TYPE_NUMERIC );
		res.m = c1.m < d;
		break;

	case VAR_TYPE_CUBE:
		res.SetType( VAR_TYPE_CUBE );
		res.c = c1.c < d;
		break;

	default:
		addError("illegal operation (incopatible arguments)");
		return true;
	};

	return false;
}

bool LessThan(cContainer& res, const double d, cContainer& c1)
{
	return LessThan(res,c1,d);
}

bool LessThan(cContainer& res, const complex<double>& x, cContainer& c1)
{
	return LessThan(res,c1,x);
}

bool GreaterThan(cContainer& res, cContainer& c1, cContainer& c2)
{
	if (c1.IsEmpty() || c2.IsEmpty()) {
		addError("illegal operation (empty argument)");
		return true;
	}
	if (c1.GetType() != c2.GetType()) {
		addError("illegal operation (incopatible arguments)");
		return true;
	}

	bool bs;
	res.Delete();

	switch (c1.GetType()) {
	case VAR_TYPE_NUMERIC:
		res.SetType( VAR_TYPE_NUMERIC );
		res.m = c1.m > c2.m;
		break;

	case VAR_TYPE_CUBE:
		res.SetType( VAR_TYPE_CUBE );
		res.c = c1.c > c2.c;
		break;

	case VAR_TYPE_STRING:
		bs = c1.s > c2.s;
		res.SetTo(bs);
		break;

	default:
		addError("illegal operation (incopatible arguments)");
		return true;
	};

	return false;
}

bool GreaterThan(cContainer& res, cContainer& c1, const complex<double>& x)
{
	if (c1.IsEmpty()) {
		addError("illegal operation (empty argument)");
		return true;
	}

	switch (c1.GetType()) {
	case VAR_TYPE_NUMERIC:
		res.SetType( VAR_TYPE_NUMERIC );
		res.m = c1.m > x;
		break;

	case VAR_TYPE_CUBE:
		res.SetType( VAR_TYPE_CUBE );
		res.c = c1.c > x;
		break;

	default:
		addError("illegal operation (incopatible arguments)");
		return true;
	};

	return false;
}

bool GreaterThan(cContainer& res, cContainer& c1, const double d)
{
	if (c1.IsEmpty()) {
		addError("illegal operation (empty argument)");
		return true;
	}

	switch (c1.GetType()) {
	case VAR_TYPE_NUMERIC:
		res.SetType( VAR_TYPE_NUMERIC );
		res.m = c1.m > d;
		break;

	case VAR_TYPE_CUBE:
		res.SetType( VAR_TYPE_CUBE );
		res.c = c1.c > d;
		break;

	default:
		addError("illegal operation (incopatible arguments)");
		return true;
	};

	return false;
}

bool GreaterThan(cContainer& res, const double d, cContainer& c1)
{
	return LessEqualTo(res,c1,d);
}

bool GreaterThan(cContainer& res, const complex<double>& x, cContainer& c1)
{
	return LessEqualTo(res,c1,x);
}


bool LessEqualTo(cContainer& res, cContainer& c1, cContainer& c2)
{
	if (c1.IsEmpty() || c2.IsEmpty()) {
		addError("illegal operation (empty argument)");
		return true;
	}
	if (c1.GetType() != c2.GetType()) {
		addError("illegal operation (incopatible arguments)");
		return true;
	}

	bool bs;
	res.Delete();

	switch (c1.GetType()) {
	case VAR_TYPE_NUMERIC:
		res.SetType( VAR_TYPE_NUMERIC );
		res.m = c1.m <= c2.m;
		break;

	case VAR_TYPE_CUBE:
		res.SetType( VAR_TYPE_CUBE );
		res.c = c1.c <= c2.c;
		break;

	case VAR_TYPE_STRING:
		bs = c1.s <= c2.s;
		res.SetTo(bs);
		break;

	default:
		addError("illegal operation (incopatible arguments)");
		return true;
	};

	return false;
}

bool LessEqualTo(cContainer& res, cContainer& c1, const complex<double>& x)
{
	if (c1.IsEmpty()) {
		addError("illegal operation (empty argument)");
		return true;
	}

	switch (c1.GetType()) {
	case VAR_TYPE_NUMERIC:
		res.SetType( VAR_TYPE_NUMERIC );
		res.m = c1.m <= x;
		break;

	case VAR_TYPE_CUBE:
		res.SetType( VAR_TYPE_CUBE );
		res.c = c1.c <= x;
		break;

	default:
		addError("illegal operation (incopatible arguments)");
		return true;
	};

	return false;
}

bool LessEqualTo(cContainer& res, cContainer& c1, const double d)
{
	if (c1.IsEmpty()) {
		addError("illegal operation (empty argument)");
		return true;
	}

	switch (c1.GetType()) {
	case VAR_TYPE_NUMERIC:
		res.SetType( VAR_TYPE_NUMERIC );
		res.m = c1.m <= d;
		break;

	case VAR_TYPE_CUBE:
		res.SetType( VAR_TYPE_CUBE );
		res.c = c1.c <= d;
		break;

	default:
		addError("illegal operation (incopatible arguments)");
		return true;
	};

	return false;
}

bool LessEqualTo(cContainer& res, const double d, cContainer& c1)
{
	return GreaterThan(res,c1,d);
}

bool LessEqualTo(cContainer& res, const complex<double>& x, cContainer& c1)
{
	return GreaterThan(res,c1,x);
}

bool GreaterEqualTo(cContainer& res, cContainer& c1, cContainer& c2)
{
	if (c1.IsEmpty() || c2.IsEmpty()) {
		addError("illegal operation (empty argument)");
		return true;
	}
	if (c1.GetType() != c2.GetType()) {
		addError("illegal operation (incopatible arguments)");
		return true;
	}

	bool bs;
	res.Delete();

	switch (c1.GetType()) {
	case VAR_TYPE_NUMERIC:
		res.SetType( VAR_TYPE_NUMERIC );
		res.m = c1.m >= c2.m;
		break;

	case VAR_TYPE_CUBE:
		res.SetType( VAR_TYPE_CUBE );
		res.c = c1.c >= c2.c;
		break;

	case VAR_TYPE_STRING:
		bs = c1.s >= c2.s;
		res.SetTo(bs);
		break;

	default:
		addError("illegal operation (incopatible arguments)");
		return true;
	};

	return false;
}

bool GreaterEqualTo(cContainer& res, cContainer& c1, const complex<double>& x)
{
	if (c1.IsEmpty()) {
		addError("illegal operation (empty argument)");
		return true;
	}

	switch (c1.GetType()) {
	case VAR_TYPE_NUMERIC:
		res.SetType( VAR_TYPE_NUMERIC );
		res.m = c1.m >= x;
		break;

	case VAR_TYPE_CUBE:
		res.SetType( VAR_TYPE_CUBE );
		res.c = c1.c >= x;
		break;

	default:
		addError("illegal operation (incopatible arguments)");
		return true;
	};

	return false;
}

bool GreaterEqualTo(cContainer& res, cContainer& c1, const double d)
{
	if (c1.IsEmpty()) {
		addError("illegal operation (empty argument)");
		return true;
	}

	switch (c1.GetType()) {
	case VAR_TYPE_NUMERIC:
		res.SetType( VAR_TYPE_NUMERIC );
		res.m = c1.m >= d;
		break;

	case VAR_TYPE_CUBE:
		res.SetType( VAR_TYPE_CUBE );
		res.c = c1.c >= d;
		break;

	default:
		addError("illegal operation (incopatible arguments)");
		return true;
	};

	return false;
}

bool GreaterEqualTo(cContainer& res, const double d, cContainer& c1)
{
	return LessThan(res,c1,d);
}

bool GreaterEqualTo(cContainer& res, const complex<double>& x, cContainer& c1)
{
	return LessThan(res,c1,x);
}

bool And(cContainer& res, cContainer& c1, cContainer& c2)
{
	if (c1.IsEmpty() || c2.IsEmpty()) {
		addError("illegal operation (empty argument)");
		return true;
	}
	if (c1.GetType() != c2.GetType()) {
		addError("illegal operation (incopatible arguments)");
		return true;
	}

	res.Delete();

	switch (c1.GetType()) {
	case VAR_TYPE_NUMERIC:
		res.SetType( VAR_TYPE_NUMERIC );
		res.m = c1.m && c2.m;
		break;

	case VAR_TYPE_CUBE:
		res.SetType( VAR_TYPE_CUBE );
		res.c = c1.c && c2.c;
		break;

	default:
		addError("illegal operation (incopatible arguments)");
		return true;
	};

	return false;
}

bool And(cContainer& res, cContainer& c1, const complex<double>& x)
{
	if (c1.IsEmpty()) {
		addError("illegal operation (empty argument)");
		return true;
	}

	switch (c1.GetType()) {
	case VAR_TYPE_NUMERIC:
		res.SetType( VAR_TYPE_NUMERIC );
		res.m = c1.m && x;
		break;

	case VAR_TYPE_CUBE:
		res.SetType( VAR_TYPE_CUBE );
		res.c = c1.c && x;
		break;

	default:
		addError("illegal operation (incopatible arguments)");
		return true;
	};

	return false;
}

bool And(cContainer& res, cContainer& c1, const double d)
{
	if (c1.IsEmpty()) {
		addError("illegal operation (empty argument)");
		return true;
	}

	switch (c1.GetType()) {
	case VAR_TYPE_NUMERIC:
		res.SetType( VAR_TYPE_NUMERIC );
		res.m = c1.m && d;
		break;

	case VAR_TYPE_CUBE:
		res.SetType( VAR_TYPE_CUBE );
		res.c = c1.c && d;
		break;

	default:
		addError("illegal operation (incopatible arguments)");
		return true;
	};

	return false;
}

bool And(cContainer& res, const double d, cContainer& c1)
{
	return And(res,c1,d);
}

bool And(cContainer& res, const complex<double>& x, cContainer& c1)
{
	return And(res,c1,x);
}

bool Or(cContainer& res, cContainer& c1, cContainer& c2)
{
	if (c1.IsEmpty() || c2.IsEmpty()) {
		addError("illegal operation (empty argument)");
		return true;
	}
	if (c1.GetType() != c2.GetType()) {
		addError("illegal operation (incopatible arguments)");
		return true;
	}

	res.Delete();

	switch (c1.GetType()) {
	case VAR_TYPE_NUMERIC:
		res.SetType( VAR_TYPE_NUMERIC );
		res.m = c1.m || c2.m;
		break;

	case VAR_TYPE_CUBE:
		res.SetType( VAR_TYPE_CUBE );
		res.c = c1.c || c2.c;
		break;

	default:
		addError("illegal operation (incopatible arguments)");
		return true;
	};

	return false;
}

bool Or(cContainer& res, cContainer& c1, const complex<double>& x)
{
	if (c1.IsEmpty()) {
		addError("illegal operation (empty argument)");
		return true;
	}

	switch (c1.GetType()) {
	case VAR_TYPE_NUMERIC:
		res.SetType( VAR_TYPE_NUMERIC );
		res.m = c1.m || x;
		break;

	case VAR_TYPE_CUBE:
		res.SetType( VAR_TYPE_CUBE );
		res.c = c1.c || x;
		break;

	default:
		addError("illegal operation (incopatible arguments)");
		return true;
	};

	return false;
}

bool Or(cContainer& res, cContainer& c1, const double d)
{
	if (c1.IsEmpty()) {
		addError("illegal operation (empty argument)");
		return true;
	}

	switch (c1.GetType()) {
	case VAR_TYPE_NUMERIC:
		res.SetType( VAR_TYPE_NUMERIC );
		res.m = c1.m || d;
		break;

	case VAR_TYPE_CUBE:
		res.SetType( VAR_TYPE_CUBE );
		res.c = c1.c || d;
		break;

	default:
		addError("illegal operation (incopatible arguments)");
		return true;
	};

	return false;
}

bool Or(cContainer& res, const double d, cContainer& c1)
{
	return Or(res,c1,d);
}

bool Or(cContainer& res, const complex<double>& x, cContainer& c1)
{
	return Or(res,c1,x);
}
