
#include "interpret.h"


bool cInterpret::doUnitaryOperation(cContainer& v, tokData& o1, int nOpType)
{
	switch (nOpType)
	{
	case LEX_OPERATOR_UNITARY_PLUS:		return tokUnitaryOp(v, o1, '+');
	case LEX_OPERATOR_UNITARY_MINUS:	return tokUnitaryOp(v, o1, '-');
	case LEX_OPERATOR_UNITARY_HERMITIAN:	return tokUnitaryOp(v, o1, '\'');
	case LEX_OPERATOR_RELATIONAL_NOT:	return tokUnitaryOp(v, o1, '!');
	case LEX_OPERATOR_POST_INCR: case LEX_OPERATOR_PRE_INCR:		return tokUnitaryOp(v, o1, 'i');
	case LEX_OPERATOR_POST_DECR: case LEX_OPERATOR_PRE_DECR:		return tokUnitaryOp(v, o1, 'd');
	default:
		reportInterp("unknown operator");
		return true;
	};

	return false;
}


bool cInterpret::doBinaryOperation(cContainer& v, tokData& o1, tokData& o2, int nOpType)
{
	switch (nOpType)
	{
	case LEX_OPERATOR_PLUS:				return tokBinaryOp(v, o1, o2, '+');
	case LEX_OPERATOR_MINUS:			return tokBinaryOp(v, o1, o2, '-');
	case LEX_OPERATOR_MULT:				return tokBinaryOp(v, o1, o2, '*');
	case LEX_OPERATOR_ELEMENT_MULT:		return tokBinaryOp(v, o1, o2, 'M');
	case LEX_OPERATOR_DIV:				return tokBinaryOp(v, o1, o2, '/');
	case LEX_OPERATOR_ELEMENT_DIV:		return tokBinaryOp(v, o1, o2, 'D');
	case LEX_OPERATOR_MOD:				return tokBinaryOp(v, o1, o2, '%');
	case LEX_OPERATOR_POW:				return tokBinaryOp(v, o1, o2, '^');
	case LEX_OPERATOR_ELEMENT_POW:		return tokBinaryOp(v, o1, o2, '^');		// for now, make these equalivalent
	case LEX_OPERATOR_RANGE:			return tokBinaryOp(v, o1, o2, 'R');

	case LEX_OPERATOR_LOGICAL_AND:		return tokBinaryOp(v, o1, o2, '&');
	case LEX_OPERATOR_LOGICAL_OR:		return tokBinaryOp(v, o1, o2, '|');
	case LEX_OPERATOR_RELATIONAL_LT:	return tokBinaryOp(v, o1, o2, '<');
	case LEX_OPERATOR_RELATIONAL_LE:	return tokBinaryOp(v, o1, o2, 'L');
	case LEX_OPERATOR_RELATIONAL_GT:	return tokBinaryOp(v, o1, o2, '>');
	case LEX_OPERATOR_RELATIONAL_GE:	return tokBinaryOp(v, o1, o2, 'G');
	case LEX_OPERATOR_RELATIONAL_EQ:	return tokBinaryOp(v, o1, o2, '=');
	case LEX_OPERATOR_RELATIONAL_NEQ:	return tokBinaryOp(v, o1, o2, 'N');

	case LEX_OPERATOR_CAT_VERT:			return tokBinaryOp(v, o1, o2, 'v');
	case LEX_OPERATOR_CAT_HORZ:			return tokBinaryOp(v, o1, o2, 'h');
	default:
		reportInterp("unknown operator");
		return true;
	};

	return false;
}

bool cInterpret::tokUnitaryOp(cContainer& v, tokData& o1, const char op)
{
	cContainer t;

	switch (o1.nSymbolType)
	{
	case LEX_TYPE_CONSTANT:
		return tokUnitaryOp(v, o1.fSymbolVal, op);

	case LEX_TYPE_RVALUE:
		return tokUnitaryOp(v, m_rvalue, op);	// will this ever be hit?

	case LEX_TYPE_EMPTY:
		t.SetType(VAR_TYPE_NUMERIC);
		return tokUnitaryOp(v, t, op);

	case LEX_TYPE_UNKNOWN:
		//if (m_pSymbolTable->getAddr(o1.ix, o1.ixh, o1.name)) { reportInterp("undefined symbol", o1); return true; }
		// fall through...

	case LEX_TYPE_VARIABLE:
		if (m_pSymbolTable->lookup(t, o1.name)) { reportInterp("undefined symbol", o1); return true; }
		return tokUnitaryOp(v, t, op);

	case LEX_TYPE_TEMP_VARIABLE:
		return tokUnitaryOp(v, m_tempVars[o1.ix], op);

	case LEX_TYPE_STRING:
		reportInterp("illegal operand", o1);
		return true;

	default:
		reportInterp("unknown symbol type", o1);
		return true;
	};
}

bool cInterpret::tokUnitaryOp(cContainer& v, double lf, char op)
{
	switch (op)
	{
	case '+':	v.SetTo(lf); break;		// NOP
	case '-':	v.SetTo(-lf); break;
	case '!':	v.SetTo( (double) (lf==0.) ); break;
	case '\'':	v.SetTo(lf); break;		// NOP
	case 'i':	v.SetTo(lf+1); break;
	case 'd':	v.SetTo(lf-1); break;
	default:
		reportInterp("illegal operator"); return true;
	};

	return false;
}

bool cInterpret::tokUnitaryOp(cContainer& v, cContainer& t, char op)
{
	string name;

	switch (op)
	{
	case '+':	v = t; break;		// NOP
	case '-':	return Mult(v, t, double(-1.));
	case '!':	return Not(v,t);
	case '\'':	return Herm(v,t);
	case 'i':	Add(v, t, 1.); name = t.GetName(); m_pSymbolTable->set(name, v); break;
	case 'd':	Add(v, t, -1.); name = t.GetName(); m_pSymbolTable->set(name, v); break;
	default:
		reportInterp("illegal operator"); return true;
	};

	return false;
}

bool cInterpret::tokEnd2Ix(tokData& arg)
{
	cContainer v;

	if (!m_pSymbolTable->lookup(v, arg.name )) {
		if (v.GetType()==VAR_TYPE_NUMERIC || v.GetType()==VAR_TYPE_CUBE) {
			if (arg.nargin==1) {
				arg.fSymbolVal = (double) (v.m.GetSize() - 1);
				return false;
			}
			switch (arg.ix) {
			case 0:
				arg.fSymbolVal = (double) (v.GetRows() - 1);
				break;
			case 1:
				arg.fSymbolVal = (double) (v.GetColumns() - 1);
				break;
			case 2:
				arg.fSymbolVal = (double) (v.GetDepth() - 1);
				break;
			}
		}
		else if (v.GetType()==VAR_TYPE_STRING) {
			arg.fSymbolVal = (double) (v.GetSize() - 1);
		}
		else {
			reportInterp("invalid use of '#' token", arg);
			return true;
		}
	}
	else {
			reportInterp("invalid use of '#' token", arg);
			return true;
	}
	return false;
}

bool cInterpret::tokBinaryOp(cContainer& v, tokData& o1, tokData& o2, const char op)
{
	cContainer t;

	switch (o1.nSymbolType)
	{
	case LEX_OPERATOR_ACCESS_END:
		if (tokEnd2Ix(o1)) return true;
		// fall through..

	case LEX_TYPE_CONSTANT:
		return tokBinaryOp(v, o1.fSymbolVal, o2, op);

	case LEX_TYPE_RVALUE:
		return tokBinaryOp(v, m_rvalue, o2, op);	// will this ever be hit?

	case LEX_TYPE_EMPTY:
		t.SetType(VAR_TYPE_NUMERIC);
		return tokBinaryOp(v, t, o2, op);	// will this ever be hit?

	case LEX_TYPE_UNKNOWN:
		//if (m_pSymbolTable->getAddr(o1.ix, o1.ixh, o1.name)) { reportInterp("undefined symbol", o1); return true; }
		// fall through...

	case LEX_TYPE_VARIABLE:
		if (m_pSymbolTable->lookup(t, o1.name)) { reportInterp("undefined symbol", o1); return true; }
		return tokBinaryOp(v, t, o2, op);

	case LEX_TYPE_TEMP_VARIABLE:
		return tokBinaryOp(v, m_tempVars[o1.ix], o2, op);

	case LEX_TYPE_STRING:
		return tokBinaryOp(v, o1.name, o2, op);

	default:
		reportInterp("unknown symbol type", o1);
		return true;
	};
}

bool cInterpret::tokBinaryOp(cContainer& v, string& str1, tokData& o2, char op)
{
	cContainer t;

	switch (o2.nSymbolType)
	{
	case LEX_OPERATOR_ACCESS_END:
		// fall through..
	case LEX_TYPE_CONSTANT:
		reportInterp("illegal operand", o2); return true;

	case LEX_TYPE_RVALUE:
		return tokBinaryOp(v, str1, m_rvalue, op);	// will this ever be hit?

	case LEX_TYPE_EMPTY:
		t.SetType(VAR_TYPE_NUMERIC);
		return tokBinaryOp(v, str1, t, op);	// will this ever be hit?

	case LEX_TYPE_UNKNOWN:
		//if (m_pSymbolTable->getAddr(o2.ix, o2.ixh, o2.name)) { reportInterp("undefined symbol", o2); return true; }
		// fall through...

	case LEX_TYPE_VARIABLE:
		if (m_pSymbolTable->lookup(t, o2.name)) { reportInterp("undefined symbol", o2); return true; }
		return tokBinaryOp(v, str1, t, op);

	case LEX_TYPE_TEMP_VARIABLE:
		return tokBinaryOp(v, str1, m_tempVars[o2.ix], op);

	case LEX_TYPE_STRING:
		return tokBinaryOp(v, str1, o2.name, op);

	default:
		reportInterp("unknown symbol type", o2);
		return true;
	}
}

bool cInterpret::tokBinaryOp(cContainer& v, string& str1, cContainer& v2, char op)
{
	string s;
	switch (v2.GetType()) {
	case VAR_TYPE_STRING:
		s = v2.s.GetString();
		return tokBinaryOp(v, str1, s, op);
	default:
		reportInterp("illegal operand"); return true;
	};
	return false;
}

bool cInterpret::tokBinaryOp(cContainer& v, string& str1, string& str2, char op)
{
	switch (op)
	{
	case '+':			v.SetTo( str1.append(str2) ); return false;
	default:			reportInterp("illegal operation"); return true;
	};
}


bool cInterpret::tokBinaryOp(cContainer& v, double lf, tokData& o2, char op)
{
	bool bTemp = false, bUkn = false;

	switch (o2.nSymbolType)
	{
	case LEX_OPERATOR_ACCESS_END:
		if (tokEnd2Ix(o2)) return true;
		// fall through..

	case LEX_TYPE_CONSTANT:
		switch (op)
		{
		case '+':			v.SetTo(lf + o2.fSymbolVal); break;
		case '-':			v.SetTo(lf - o2.fSymbolVal); break;
		case '*': case 'M': v.SetTo(lf * o2.fSymbolVal); break;
		case '/': case 'D': v.SetTo(lf / o2.fSymbolVal); break;
		case '%':			v.SetTo( fmod(lf, o2.fSymbolVal) ); break;
		case '^':			v.SetTo( ::pow(lf, o2.fSymbolVal) ); break;
		case 'R':			SetToRange( v, lf, o2.fSymbolVal ); break;

		case '&':			v.SetTo( lf && o2.fSymbolVal ); break;
		case '|':			v.SetTo( lf || o2.fSymbolVal ); break;
		case '<':			v.SetTo( lf < o2.fSymbolVal ); break;
		case 'L':			v.SetTo( lf <= o2.fSymbolVal ); break;
		case '>':			v.SetTo( lf > o2.fSymbolVal ); break;
		case 'G':			v.SetTo( lf >= o2.fSymbolVal ); break;
		case '=':			v.SetTo( lf == o2.fSymbolVal ); break;
		case 'N':			v.SetTo( lf != o2.fSymbolVal ); break;

		case 'v':			VertCat( v, lf, o2.fSymbolVal ); break;
		case 'h':			HorzCat( v, lf, o2.fSymbolVal ); break;
		default: reportInterp("illegal operator", o2); return true;
		};
		return false;

	case LEX_TYPE_UNKNOWN:
		//if (m_pSymbolTable->getAddr(o2.ix, o2.ixh, o2.name)) { reportInterp("undefined symbol", o2, o2.name); return true; }
		bUkn = true;
		// fall through...

	case LEX_TYPE_TEMP_VARIABLE:
		bTemp = true;	// fall through...
	case LEX_TYPE_VARIABLE:
		if (bTemp && !bUkn) m_vTmp = m_tempVars[o2.ix];
		else { if (m_pSymbolTable->lookup(m_vTmp, o2.name)) return true; }
	
		switch (op)
		{
		case '+':			return Add(v,m_vTmp,lf);		//v = lf + m_vTmp; break;
		case '-':			return Sub(v,lf,m_vTmp);		//v = lf - m_vTmp; break;
		case '*': case 'M': return Mult(v,m_vTmp,lf);		//v = lf * m_vTmp; break;
		case '/': case 'D': return DotDiv(v,lf,m_vTmp);		//v = lf / m_vTmp; break;
		case '%':			return Mod(v, lf, m_vTmp);
		case '^':			return DotPow(v, lf, m_vTmp);
		case 'R':			return SetToRange( v, lf, m_vTmp );

		case '&':			return And(v, lf, m_vTmp);	//v = lf && m_rvalue; break;
		case '|':			return Or(v, lf, m_vTmp);		//v = lf || m_rvalue; break;
		case '<':			return LessThan(v, lf, m_vTmp);	//v = lf < m_rvalue; break;
		case 'L':			return LessEqualTo(v, lf, m_vTmp);	//v = lf <= m_rvalue; break;
		case '>':			return GreaterThan(v, lf, m_vTmp);	//v = lf > m_rvalue; break;
		case 'G':			return GreaterEqualTo(v, lf, m_vTmp);	//v = lf >= m_rvalue; break;
		case '=':			return EqualTo(v, lf, m_vTmp);		//v = lf == m_rvalue; break;
		case 'N':			return NotEqualTo(v, lf, m_vTmp);	//v = lf != m_rvalue; break;

		case 'v':			return VertCat( v, lf, m_vTmp );
		case 'h':			return HorzCat( v, lf, m_vTmp );
		default: reportInterp("illegal operator", o2); return true;
		};
		return false;

	case LEX_TYPE_RVALUE:
		switch (op)
		{
		case '+':			return Add(v,m_rvalue,lf);		//v = lf + m_rvalue; break;
		case '-':			return Sub(v,lf,m_rvalue);		//v = lf - m_rvalue; break;
		case '*': case 'M': return Mult(v,m_rvalue,lf);		//v = lf * m_rvalue; break;
		case '/': case 'D': return DotDiv(v,lf,m_rvalue);	//v = lf / m_rvalue; break;
		case '%':			return Mod(v, lf, m_rvalue);
		case '^':			return DotPow(v, lf, m_rvalue);
		case 'R':			return SetToRange( v, lf, m_rvalue );

		case '&':			return And(v, lf, m_rvalue);	//v = lf && m_rvalue; break;
		case '|':			return Or(v, lf, m_rvalue);		//v = lf || m_rvalue; break;
		case '<':			return LessThan(v, lf, m_rvalue);	//v = lf < m_rvalue; break;
		case 'L':			return LessEqualTo(v, lf, m_rvalue);	//v = lf <= m_rvalue; break;
		case '>':			return GreaterThan(v, lf, m_rvalue);	//v = lf > m_rvalue; break;
		case 'G':			return GreaterEqualTo(v, lf, m_rvalue);	//v = lf >= m_rvalue; break;
		case '=':			return EqualTo(v, lf, m_rvalue);		//v = lf == m_rvalue; break;
		case 'N':			return NotEqualTo(v, lf, m_rvalue);	//v = lf != m_rvalue; break;

		case 'v':			return VertCat( v, lf, m_rvalue );
		case 'h':			return HorzCat( v, lf, m_rvalue );
		default: reportInterp("illegal operator", o2); return true;
		};
		return false;

	case LEX_TYPE_STRING:
		reportInterp("illegal operand", o2);
		return true;

	default:
		reportInterp("unknown symbol type", o2);
		return true;
	};
}

bool cInterpret::tokBinaryOp(cContainer& v, cContainer& t, tokData& o2, char op)
{
	bool bTemp = false, bUkn = false;
	string ss;

	switch (o2.nSymbolType)
	{
	case LEX_OPERATOR_ACCESS_END:
		if (tokEnd2Ix(o2)) return true;
		// fall through..

	case LEX_TYPE_CONSTANT:
		m_vTmp.SetTo( o2.fSymbolVal );
		//if (checkCompatability(t, m_vTmp, op))
		if (true)
		{
			switch (op)
			{
			case '+':			return Add(v,t,o2.fSymbolVal);		//v = t + o2.fSymbolVal; break;
			case '-':			return Sub(v,t,o2.fSymbolVal);		//v = t - o2.fSymbolVal; break;
			case '*': case 'M': return Mult(v,t,o2.fSymbolVal);		//v = t * o2.fSymbolVal; break;
			case '/': case 'D': return DotDiv(v,t,o2.fSymbolVal);	//v = t / o2.fSymbolVal; break;
			case '%':			return Mod(v, t, o2.fSymbolVal);
			case '^':			return DotPow(v, t, o2.fSymbolVal);
			case 'R':			return SetToRange( v, t, o2.fSymbolVal );

			case '&':			return And(v, t, o2.fSymbolVal);	//v = t && o2.fSymbolVal; break;
			case '|':			return Or(v, t, o2.fSymbolVal);		//v = t || o2.fSymbolVal; break;
			case '<':			return LessThan(v, t, o2.fSymbolVal);	//v = t < o2.fSymbolVal; break;
			case 'L':			return LessEqualTo(v, t, o2.fSymbolVal);	//v = t <= o2.fSymbolVal; break;
			case '>':			return GreaterThan(v, t, o2.fSymbolVal);	//v = t > o2.fSymbolVal; break;
			case 'G':			return GreaterEqualTo(v, t, o2.fSymbolVal);	//v = t >= o2.fSymbolVal; break;
			case '=':			return EqualTo(v, t, o2.fSymbolVal);	//v = t == o2.fSymbolVal; break;
			case 'N':			return NotEqualTo(v, t, o2.fSymbolVal);	//v = t != o2.fSymbolVal; break;

			case 'v':			return VertCat( v, t, o2.fSymbolVal );
			case 'h':			return HorzCat( v, t, o2.fSymbolVal );
			default: reportInterp("illegal operator", o2); return true;
			};
		}
		else
		{
			reportInterp("incompatible operands", o2);
			return true;
		}
		return false;

	case LEX_TYPE_UNKNOWN:
		//if (m_pSymbolTable->getAddr(o2.ix, o2.ixh, o2.name)) { reportInterp("undefined symbol", o2); return true; }
		bUkn = true;
		// fall through...

	case LEX_TYPE_TEMP_VARIABLE:
		bTemp = true;
	case LEX_TYPE_VARIABLE:
		if (bTemp && !bUkn) m_vTmp = m_tempVars[o2.ix];
		else { if (m_pSymbolTable->lookup(m_vTmp, o2.name)) return true; }

		//if (checkCompatability(t, m_vTmp, op))
		if (true)
		{
			switch (op)
			{
			case '+':		return Add(v, t, m_vTmp);		//v = t + m_vTmp; break;
			case '-': 		return Sub(v, t, m_vTmp);		//v = t - m_vTmp; break;
			case '*': 		return Mult(v, t, m_vTmp);	//v = t * m_vTmp; break;
			case 'M': 		return DotMult(v, t, m_vTmp);
			case '/': 		return DotDiv(v, t, m_vTmp);	//v = t / m_vTmp; break;
			case 'D': 		return DotDiv(v, t, m_vTmp);
			case '%': 		return Mod(v, t, m_vTmp);
			case '^': 		return DotPow(v, t, m_vTmp);		//v = t.Pow(m_vTmp); break;
			case 'R': 		return SetToRange(v, t, m_vTmp);

			case '&':		return And(v, t, m_vTmp);	//v = t && m_vTmp; break;
			case '|':		return Or(v, t, m_vTmp);	//v = t || m_vTmp; break;
			case '<':		return LessThan(v, t, m_vTmp);	//v = t < m_vTmp; break;
			case 'L':		return LessEqualTo(v, t, m_vTmp);	//v = t <= m_vTmp; break;
			case '>':		return GreaterThan(v, t, m_vTmp);	//v = t > m_vTmp; break;
			case 'G':		return GreaterEqualTo(v, t, m_vTmp);	//v = t >= m_vTmp; break;
			case '=':		return EqualTo(v, t, m_vTmp);	//v = t == m_vTmp; break;
			case 'N':		return NotEqualTo(v, t, m_vTmp);	//v = t != m_vTmp; break;

			case 'v': 		return VertCat( v, t, m_vTmp );
			case 'h': 		return HorzCat( v, t, m_vTmp );
			default: reportInterp("illegal operator"); return true;
			};
			return false;
		}
		else
		{
			reportInterp("incompatible operands");
			return true;
		}

	case LEX_TYPE_RVALUE:
		//if (checkCompatability(t, m_rvalue, op))
		if (true)
		{
			switch (op)
			{
			case '+': 		return Add(v, t, m_rvalue);		//v = t + m_rvalue; break;
			case '-': 		return Sub(v, t, m_rvalue);		//v = t - m_rvalue; break;
			case '*': 		return Mult(v, t, m_rvalue);	//v = t * m_rvalue; break;
			case 'M': 		return DotMult(v, t, m_rvalue);
			case '/': 		return DotDiv(v, t, m_rvalue);	//v = t / m_rvalue; break;
			case 'D': 		return DotDiv(v, t, m_rvalue);
			case '%': 		return Mod(v, t, m_rvalue);
			case '^': 		return DotPow(v, t, m_rvalue);	//v = t.Pow(m_rvalue); break;
			case 'R': 		return SetToRange( v, t, m_rvalue );

			case '&':		return And(v, t, m_rvalue);		//v = t && m_rvalue; break;
			case '|':		return Or(v, t, m_rvalue);	//v = t || m_rvalue; break;
			case '<':		return LessThan(v, t, m_rvalue);	//v = t < m_rvalue; break;
			case 'L':		return LessEqualTo(v, t, m_rvalue);	//v = t <= m_rvalue; break;
			case '>':		return GreaterThan(v, t, m_rvalue);	//v = t > m_rvalue; break;
			case 'G':		return GreaterEqualTo(v, t, m_rvalue);	//v = t >= m_rvalue; break;
			case '=':		return EqualTo(v, t, m_rvalue);		//v = t == m_rvalue; break;
			case 'N':		return NotEqualTo(v, t, m_rvalue);	//v = t != m_rvalue; break;

			case 'v': 		VertCat( v, t, m_rvalue ); break;
			case 'h': 		HorzCat( v, t, m_rvalue ); break;
			default: reportInterp("illegal operator"); return true;
			};
			return false;
		}
		else
		{
			reportInterp("incompatible operands");
			return true;
		}

	case LEX_TYPE_STRING:
		if (t.GetType() != VAR_TYPE_STRING) { reportInterp("incompatable operands"); return true; }

		switch (op) {
		case '+':
			ss = t.s.s + o2.name;
			v.SetTo( ss );
			break;
		default:
			reportInterp("illegal operation"); return true;
		};

		return false;

	default:
		reportInterp("unknown symbol type");
		return true;
	};
	return false;
}


//
// tertiary operators
//
bool cInterpret::doTertiaryOperation(cContainer& v, tokData& o1, tokData& o2, tokData& o3, int nOpType)
{
	switch (nOpType)
	{
	case LEX_OPERATOR_RANGE:		return tokTertiaryOp(v, o1, o2, o3, 'R');
	default:
		reportInterp("unknown operator");
		return true;
	};
}

bool cInterpret::tokTertiaryOp(cContainer& v, tokData& o1, tokData& o2, tokData& o3, const char op)
{
	cContainer t;

	switch (o1.nSymbolType)
	{
	case LEX_OPERATOR_ACCESS_END:
		if (tokEnd2Ix(o1)) return true;
		// fall through..

	case LEX_TYPE_CONSTANT:
		return tokTertiaryOp(v, o1.fSymbolVal, o2, o3, op);

	case LEX_TYPE_RVALUE:
		return tokTertiaryOp(v, m_rvalue, o2, o3, op);

	case LEX_TYPE_EMPTY:
		t.SetType(VAR_TYPE_NUMERIC);
		return tokTertiaryOp(v, t, o2, o3, op);

	case LEX_TYPE_UNKNOWN:
		//if (m_pSymbolTable->getAddr(o1.ix, o1.ixh, o1.name)) { reportInterp("undefined symbol", o1); return true; }
		// fall through...

	case LEX_TYPE_VARIABLE:
		if (m_pSymbolTable->lookup(t, o1.name)) { reportInterp("undefined symbol", o1); return true; }
		return tokTertiaryOp(v, t, o2, o3, op);

	case LEX_TYPE_TEMP_VARIABLE:
		return tokTertiaryOp(v, m_tempVars[o1.ix], o2, o3, op);

	default:
		reportInterp("illegal operation or unknown symbol", o1);
		return true;
	};
}

bool cInterpret::tokTertiaryOp(cContainer& v, double lf, tokData& o2, tokData& o3, char op)
{
	cContainer t;

	switch (o2.nSymbolType)
	{
	case LEX_OPERATOR_ACCESS_END:
		if (tokEnd2Ix(o2)) return true;
		// fall through..

	case LEX_TYPE_CONSTANT:
		return tokTertiaryOp(v, lf, o2.fSymbolVal, o3, op);

	case LEX_TYPE_RVALUE:
		return tokTertiaryOp(v, lf, m_rvalue, o3, op);

	case LEX_TYPE_EMPTY:
		t.SetType(VAR_TYPE_NUMERIC);
		return tokTertiaryOp(v, lf, t, o3, op);

	case LEX_TYPE_UNKNOWN:
		//if (m_pSymbolTable->getAddr(o2.ix, o2.ixh, o2.name)) { reportInterp("undefined symbol", o2); return true; }
		// fall through...

	case LEX_TYPE_VARIABLE:
		if (m_pSymbolTable->lookup(t, o2.name)) { reportInterp("undefined symbol", o2); return true; }
		return tokTertiaryOp(v, lf, t, o3, op);

	case LEX_TYPE_TEMP_VARIABLE:
		return tokTertiaryOp(v, lf, m_tempVars[o2.ix], o3, op);

	default:
		reportInterp("illegal operation or unknown symbol", o2);
		return true;
	};
}

bool cInterpret::tokTertiaryOp(cContainer& v, tokData& o1, double lf, tokData& o3, char op)
{
	cContainer t;

	switch (o1.nSymbolType)
	{
	case LEX_OPERATOR_ACCESS_END:
		if (tokEnd2Ix(o1)) return true;
		// fall through..

	case LEX_TYPE_CONSTANT:
		return tokTertiaryOp(v, o1.fSymbolVal, lf, o3, op);

	case LEX_TYPE_RVALUE:
		return tokTertiaryOp(v, m_rvalue, lf, o3, op);

	case LEX_TYPE_EMPTY:
		t.SetType(VAR_TYPE_NUMERIC);
		return tokTertiaryOp(v, t, lf, o3, op);

	case LEX_TYPE_UNKNOWN:
		//if (m_pSymbolTable->getAddr(o1.ix, o1.ixh, o1.name)) { reportInterp("undefined symbol", o1); return true; }
		// fall through...

	case LEX_TYPE_VARIABLE:
		if (m_pSymbolTable->lookup(t, o1.name)) { reportInterp("undefined symbol", o1); return true; }
		return tokTertiaryOp(v, t, lf, o3, op);

	case LEX_TYPE_TEMP_VARIABLE:
		return tokTertiaryOp(v, m_tempVars[o1.ix], lf, o3, op);

	default:
		reportInterp("illegal operation or unknown symbol", o1);
		return true;
	};
}

bool cInterpret::tokTertiaryOp(cContainer& v, cContainer& v1, tokData& o2, tokData& o3, char op)
{
	cContainer t;

	switch (o2.nSymbolType)
	{
	case LEX_OPERATOR_ACCESS_END:
		if (tokEnd2Ix(o2)) return true;
		// fall through..

	case LEX_TYPE_CONSTANT:
		return tokTertiaryOp(v, v1, o2.fSymbolVal, o3, op);

	case LEX_TYPE_RVALUE:
		return tokTertiaryOp(v, v1, m_rvalue, o3, op);

	case LEX_TYPE_EMPTY:
		t.SetType(VAR_TYPE_NUMERIC);
		return tokTertiaryOp(v, v1, t, o3, op);

	case LEX_TYPE_UNKNOWN:
		//if (m_pSymbolTable->getAddr(o2.ix, o2.ixh, o2.name)) { reportInterp("undefined symbol", o2); return true; }
		// fall through...

	case LEX_TYPE_VARIABLE:
		if (m_pSymbolTable->lookup(t, o2.name)) { reportInterp("undefined symbol", o2); return true; }
		return tokTertiaryOp(v, v1, t, o3, op);

	case LEX_TYPE_TEMP_VARIABLE:
		return tokTertiaryOp(v, v1, m_tempVars[o2.ix], o3, op);

	default:
		reportInterp("illegal operation or unknown symbol", o2);
		return true;
	};
}

bool cInterpret::tokTertiaryOp(cContainer& v, tokData& o1, cContainer& v2, tokData& o3, char op)
{
	cContainer t;

	switch (o1.nSymbolType)
	{
	case LEX_OPERATOR_ACCESS_END:
		if (tokEnd2Ix(o1)) return true;
		// fall through..

	case LEX_TYPE_CONSTANT:
		return tokTertiaryOp(v, o1.fSymbolVal, v2, o3, op);

	case LEX_TYPE_RVALUE:
		return tokTertiaryOp(v, m_rvalue, v2, o3, op);

	case LEX_TYPE_EMPTY:
		t.SetType(VAR_TYPE_NUMERIC);
		return tokTertiaryOp(v, t, v2, o3, op);

	case LEX_TYPE_UNKNOWN:
		//if (m_pSymbolTable->getAddr(o1.ix, o1.ixh, o1.name)) { reportInterp("undefined symbol", o1); return true; }
		// fall through...

	case LEX_TYPE_VARIABLE:
		if (m_pSymbolTable->lookup(t, o1.name)) { reportInterp("undefined symbol", o1); return true; }
		return tokTertiaryOp(v, t, v2, o3, op);

	case LEX_TYPE_TEMP_VARIABLE:
		return tokTertiaryOp(v, m_tempVars[o1.ix], v2, o3, op);

	default:
		reportInterp("illegal operation or unknown symbol", o1);
		return true;
	};
}

bool cInterpret::tokTertiaryOp(cContainer& v, cContainer& v1, cContainer& v2, tokData& o3, char op)
{
	cContainer t;

	switch (o3.nSymbolType)
	{
	case LEX_OPERATOR_ACCESS_END:
		if (tokEnd2Ix(o3)) return true;
		// fall through..

	case LEX_TYPE_CONSTANT:
		switch (op)
		{
		case 'R':			return SetToRange( v, v1, v2, o3.fSymbolVal ); break;
		default: reportInterp("illegal operator", o3); return true;
		};

	case LEX_TYPE_RVALUE:
		switch (op)
		{
		case 'R':			return SetToRange( v, v1, v2, m_rvalue ); break;
		default: reportInterp("illegal operator", o3); return true;
		};

	case LEX_TYPE_EMPTY:
		addError("illegal argument");
		return true;

	case LEX_TYPE_UNKNOWN:
		//if (m_pSymbolTable->getAddr(o3.ix, o3.ixh, o3.name)) { reportInterp("undefined symbol", o3); return true; }
		// fall through...

	case LEX_TYPE_VARIABLE:
		if (m_pSymbolTable->lookup(t, o3.name)) { reportInterp("undefined symbol", o3); return true; }
		switch (op)
		{
		case 'R':			return SetToRange( v, v1, v2, t ); break;
		default: reportInterp("illegal operator", o3); return true;
		};

	case LEX_TYPE_TEMP_VARIABLE:
		switch (op)
		{
		case 'R':			return SetToRange( v, v1, v2, m_tempVars[o3.ix] ); break;
		default: reportInterp("illegal operator", o3); return true;
		};

	default:
		reportInterp("illegal operation or unknown symbol", o3);
		return true;
	};
}

bool cInterpret::tokTertiaryOp(cContainer& v, cContainer& v1, double lf, tokData& o3, char op)
{
	cContainer t;

	switch (o3.nSymbolType)
	{
	case LEX_OPERATOR_ACCESS_END:
		if (tokEnd2Ix(o3)) return true;
		// fall through..

	case LEX_TYPE_CONSTANT:
		switch (op)
		{
		case 'R':			return SetToRange( v, v1, lf, o3.fSymbolVal ); break;
		default: reportInterp("illegal operator", o3); return true;
		};

	case LEX_TYPE_RVALUE:
		switch (op)
		{
		case 'R':			return SetToRange( v, v1, lf, m_rvalue ); break;
		default: reportInterp("illegal operator", o3); return true;
		};

	case LEX_TYPE_EMPTY:
		addError("illegal argument");
		return true;

	case LEX_TYPE_UNKNOWN:
		//if (m_pSymbolTable->getAddr(o3.ix, o3.ixh, o3.name)) { reportInterp("undefined symbol", o3); return true; }
		// fall through...

	case LEX_TYPE_VARIABLE:
		if (m_pSymbolTable->lookup(t, o3.name)) { reportInterp("undefined symbol", o3); return true; }
		switch (op)
		{
		case 'R':			return SetToRange( v, v1, lf, t ); break;
		default: reportInterp("illegal operator", o3); return true;
		};

	case LEX_TYPE_TEMP_VARIABLE:
		switch (op)
		{
		case 'R':			return SetToRange( v, v1, lf, m_tempVars[o3.ix] ); break;
		default: reportInterp("illegal operator", o3); return true;
		};

	default:
		reportInterp("illegal operation or unknown symbol", o3);
		return true;
	};
}

bool cInterpret::tokTertiaryOp(cContainer& v, double lf, cContainer& v2, tokData& o3, char op)
{
	cContainer t;

	switch (o3.nSymbolType)
	{
	case LEX_OPERATOR_ACCESS_END:
		if (tokEnd2Ix(o3)) return true;
		// fall through..

	case LEX_TYPE_CONSTANT:
		switch (op)
		{
		case 'R':			return SetToRange( v, lf, v2, o3.fSymbolVal ); break;
		default: reportInterp("illegal operator", o3); return true;
		};

	case LEX_TYPE_RVALUE:
		switch (op)
		{
		case 'R':			return SetToRange( v, lf, v2, m_rvalue ); break;
		default: reportInterp("illegal operator", o3); return true;
		};

	case LEX_TYPE_EMPTY:
		addError("illegal argument");
		return true;

	case LEX_TYPE_UNKNOWN:
		//if (m_pSymbolTable->getAddr(o3.ix, o3.ixh, o3.name)) { reportInterp("undefined symbol", o3); return true; }
		// fall through...

	case LEX_TYPE_VARIABLE:
		if (m_pSymbolTable->lookup(t, o3.name)) { reportInterp("undefined symbol", o3); return true; }
		switch (op)
		{
		case 'R':			return SetToRange( v, lf, v2, t ); break;
		default: reportInterp("illegal operator", o3); return true;
		};

	case LEX_TYPE_TEMP_VARIABLE:
		switch (op)
		{
		case 'R':			return SetToRange( v, lf, v2, m_tempVars[o3.ix] ); break;
		default: reportInterp("illegal operator", o3); return true;
		};

	default:
		reportInterp("illegal operation or unknown symbol", o3);
		return true;
	};
}

bool cInterpret::tokTertiaryOp(cContainer& v, double lf1, double lf2, tokData& o3, char op)
{
	cContainer t;

	switch (o3.nSymbolType)
	{
	case LEX_OPERATOR_ACCESS_END:
		if (tokEnd2Ix(o3)) return true;
		// fall through..

	case LEX_TYPE_CONSTANT:
		switch (op)
		{
		case 'R':			return SetToRange( v, lf1, lf2, o3.fSymbolVal ); break;
		default: reportInterp("illegal operator", o3); return true;
		};

	case LEX_TYPE_RVALUE:
		switch (op)
		{
		case 'R':			return SetToRange( v, lf1, lf2, m_rvalue ); break;
		default: reportInterp("illegal operator", o3); return true;
		};

	case LEX_TYPE_EMPTY:
		addError("illegal argument");
		return true;

	case LEX_TYPE_UNKNOWN:
		//if (m_pSymbolTable->getAddr(o3.ix, o3.ixh, o3.name)) { reportInterp("undefined symbol", o3); return true; }
		// fall through...

	case LEX_TYPE_VARIABLE:
		if (m_pSymbolTable->lookup(t, o3.name)) { reportInterp("undefined symbol", o3); return true; }
		switch (op)
		{
		case 'R':			return SetToRange( v, lf1, lf2, t ); break;
		default: reportInterp("illegal operator", o3); return true;
		};

	case LEX_TYPE_TEMP_VARIABLE:
		switch (op)
		{
		case 'R':			return SetToRange( v, lf1, lf2, m_tempVars[o3.ix] ); break;
		default: reportInterp("illegal operator", o3); return true;
		};

	default:
		reportInterp("illegal operation or unknown symbol", o3);
		return true;
	};
}
