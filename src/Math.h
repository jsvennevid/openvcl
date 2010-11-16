#ifndef __OPENVCL_MATH_H__
#define __OPENVCL_MATH_H__

/*
 * Math.h
 *
 * Copyright (C) 2004 Jesper Svennevid, Daniel Collin
 *
 * Licensed under the AFL v2.0. See the file LICENSE included with this
 * distribution for licensing terms.
 *
 */

#include "Expression.h"

namespace vcl
{

class Math
{
public:

	static const Expression::OperatorEntry* mathOperators();

private:

	EXP_TABLE_DECLARE();
	EXP_OP_DECLARE(Abs);
	EXP_OP_DECLARE(Exp);
	EXP_OP_DECLARE(ASin);
	EXP_OP_DECLARE(Sin);
	EXP_OP_DECLARE(Sinh);
	EXP_OP_DECLARE(ACos);
	EXP_OP_DECLARE(Cos);
	EXP_OP_DECLARE(Cosh);
	EXP_OP_DECLARE(ATan);
	EXP_OP_DECLARE(ATan2);
	EXP_OP_DECLARE(Tan);
	EXP_OP_DECLARE(Pow);
	EXP_OP_DECLARE(Log);
	EXP_OP_DECLARE(Log10);
	EXP_OP_DECLARE(Sqrt);
	EXP_OP_DECLARE(Pi);

	EXP_OP_DECLARE(Mod);

};

}

#endif
