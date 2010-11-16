#ifndef __OPENVCL_EXPRESSION_H__
#define __OPENVCL_EXPRESSION_H__

/*
 * Expression.h
 *
 * Copyright (C) 2004 Jesper Svennevid, Daniel Collin
 *
 * Licensed under the AFL v2.0. See the file LICENSE included with this
 * distribution for licensing terms.
 *
 */

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Includes
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <string>
#include <vector>
#include <stack>

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Class
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define EXP_TABLE_NAME() ms_operators
#define EXP_TABLE_DECLARE() static Expression::OperatorEntry ms_operators[]
#define EXP_TABLE_BEGIN(base) Expression::OperatorEntry base::ms_operators[] = {
#define EXP_TABLE_ENTRY(name,type) { &(ms_op##name), Expression::Operator::type },
#define EXP_TABLE_END() { NULL, Expression::Operator::INVALID } }

#define EXP_OP_NAME(name) Op##name

#define EXP_OP_DECLARE(name) \
	class Op##name : public Expression::Operator\
	{ \
	public: \
		Op##name( const std::string& name, unsigned int priority, unsigned int parameters ); \
		double execute( std::stack<double>& arguments ) const; \
	}; \
	static class Op##name ms_op##name

#define EXP_OP_IMPLEMENT(base,name,match,pri,args) \
	base::Op##name::Op##name( const std::string& name, unsigned int priority, unsigned int parameters ) \
	: Operator( name, priority, parameters ) {} \
  base::Op##name base::ms_op##name(match,Expression::Operator::pri,args); \
  double base::Op##name::execute( std::stack<double>& arguments ) const

#define EXP_OP_ARGUMENT() arguments.top(); arguments.pop()

namespace vcl
{

class Expression
{
public:

	class Operator
	{
	public:

		enum // standard priorities
		{
			PRI_CORE1 = 10, // + -
			PRI_CORE2 = 20, // * /

			PRI_UNARY = 50,
			PRI_FUNCTIONS = 50,

			PRI_PARENTHESIS = 0
		};

		enum Mode
		{
			INVALID	= 0x00,
			UNARY		= 0x01,
			BINARY	= 0x02
		};

		Operator( const std::string& name, unsigned int priority, unsigned int parameters );
		virtual ~Operator();

		const std::string& name() const;
		unsigned int priority() const;
		unsigned int parameters() const;
		virtual double execute( std::stack<double>& arguments ) const = 0;

	private:

		std::string m_name;
		unsigned int m_priority;
		unsigned int m_parameters;
	};

	class OperatorEntry
	{
	public:
		Operator* m_operator;
		Operator::Mode m_mode;
	};

	Expression();
	~Expression();

	static double evaluate( const std::string& expression, const OperatorEntry* customOperators = NULL );
	bool process( const std::string& expression );
	bool solve();

	void setCustomOperators( const OperatorEntry* customOperators );
	double result() const;

private:

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	EXP_TABLE_DECLARE();
	EXP_OP_DECLARE(Add);
	EXP_OP_DECLARE(Subtract);
	EXP_OP_DECLARE(Multiply);
	EXP_OP_DECLARE(Divide);
	EXP_OP_DECLARE(Positive);
	EXP_OP_DECLARE(Negate);
	EXP_OP_DECLARE(Comma);
	EXP_OP_DECLARE(Parenthesis);

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	class Node
	{
	public:

		enum Type
		{
			OPERAND,
			OPERATOR
		};

		Node();
		Node( double value );
		Node( Operator* op );

		Type type() const;
		double value() const;
		Operator* op() const;

	private:

		Type m_type;
		union
		{
			double m_value;
			Operator* m_op;
		};
	};

	bool flushParenthesis();
	void pushOperator( Operator* op );
//	void pushOperand( Operand* val );

	std::vector<Node> m_tokens;
	std::stack<Operator*> m_stack;	

	const OperatorEntry* m_customOperators;
	double m_result;

};

#include "Expression.inl"

}

#endif
