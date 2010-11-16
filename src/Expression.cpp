/*
 * Expression.cpp
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

#include "Expression.h"

#include <sstream>

#include <assert.h>

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Class
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace vcl
{

EXP_TABLE_BEGIN(Expression)
	EXP_TABLE_ENTRY(Add,			BINARY)
	EXP_TABLE_ENTRY(Subtract,	BINARY)
	EXP_TABLE_ENTRY(Multiply,	BINARY)
	EXP_TABLE_ENTRY(Divide,		BINARY)
	EXP_TABLE_ENTRY(Positive,	UNARY)
	EXP_TABLE_ENTRY(Negate,		UNARY)
	EXP_TABLE_ENTRY(Comma,		BINARY)
EXP_TABLE_END();

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Expression::~Expression()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

double Expression::evaluate( const std::string& expression, const OperatorEntry* customOperators )
{
	Expression e;

	e.setCustomOperators( customOperators );

	if( !e.process( expression ) )
		return 0;

	if( !e.solve() )
		return 0;

	return e.result();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool Expression::process( const std::string& expression )
{
	std::string::size_type curr, next;
	Operator::Mode mode = Operator::UNARY;

	m_tokens.clear();

	while( !m_stack.empty() )
		m_stack.pop();

	for( curr = 0; curr < expression.length(); curr = next )
	{
		next = curr+1;

		if( ((expression[curr] >= '0') && (expression[curr] <= '9')) || (expression[curr] == '.') )
		{
			// operand

			bool hasPeriod = (expression[curr] == '.');
			bool hasExponent = false;
			std::string::size_type exponent = curr;

			for( ; next < expression.length(); ++next )
			{
				if( ('.' == expression[next]) )
				{
					if( hasPeriod )
						return false;

					hasPeriod = true;
					continue;
				}

				if( ('e' == expression[next]) || ('E' == expression[next]) )
				{
					if( exponent != curr )
						return false;

					hasExponent = true;
					exponent = next;
					continue;
				}

				if( ('+' == expression[next]) || ('-' == expression[next]) )
				{
					if( (exponent != (next-1)) && hasExponent )
						return false;
					else if( !hasExponent )
						break;

					continue;
				}
				
				if( !((expression[next] >= '0') && (expression[next] <= '9')) )
					break;
			}

			std::istringstream os(expression.substr( curr, next-curr ).c_str());
			double value;

			os >> value;
			if( os.fail() )
				return false;

			m_tokens.push_back( Node( value ) );

			mode = Operator::BINARY;
		}
		else if( ')' == expression[curr] )
		{
			if( !flushParenthesis() )
				return false;
			mode = Operator::BINARY;
		}
		else if( '(' == expression[curr] )
		{
			m_stack.push( &ms_opParenthesis );
			mode = Operator::UNARY;
		}
		else if( ' ' != expression[curr] )
		{
			// operator

			Operator* lastMatch = NULL;

			for( next = curr; next < expression.length(); next++ )
			{
				const OperatorEntry* activeOperators = ms_operators;
				const OperatorEntry* customOperators = m_customOperators;

				const OperatorEntry* currEntry;
				do
				{
					for( currEntry = activeOperators; currEntry->m_operator; currEntry++ )
					{
						if( !expression.compare( curr, (next-curr)+1, currEntry->m_operator->name(), 0, (next-curr)+1 ) && (currEntry->m_mode & mode) )
						{
							lastMatch = currEntry->m_operator;
							break;
						}
					}

					activeOperators = customOperators;
					customOperators = NULL;
				}
				while( activeOperators && !currEntry->m_operator );

				if( !currEntry->m_operator )
					break;
			}


			if( !lastMatch || expression.compare( curr, lastMatch->name().length(), lastMatch->name() )  )
				return false;

			pushOperator( lastMatch );
			mode = Operator::UNARY;
		}
	}

	// apply remaining operators

	while( !m_stack.empty() )
	{
		Operator* op = m_stack.top();
		m_stack.pop();

		// unbalanced parentheses?
		if( op == &ms_opParenthesis )
			return false;

		m_tokens.push_back( Node( op ) );
	}

	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool Expression::solve()
{
	std::stack<double> arguments;

	for( std::vector<Node>::iterator i = m_tokens.begin(); i != m_tokens.end(); ++i )
	{
		Node& node = (*i);

		if( node.type() == Node::OPERAND )
		{
			arguments.push( node.value() );
		}
		else
		{
			Operator* op = node.op();
			assert( op );

			if( arguments.size() < op->parameters() )
				return false;

			arguments.push( op->execute( arguments ) );
		}
	}

	if( arguments.size() != 1 )
		return false;

	m_result = arguments.top();

	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool Expression::flushParenthesis()
{
	while( !m_stack.empty() )
	{
		Operator* op = m_stack.top();
		m_stack.pop();

		if( op == &ms_opParenthesis )
			return true;

		m_tokens.push_back( Node( op ) );
	}

	return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Expression::pushOperator( Operator* op )
{
	while( !m_stack.empty() && (m_stack.top()->priority() > op->priority()) )
	{
		m_tokens.push_back( Node( m_stack.top() ) );
		m_stack.pop();
	}

	m_stack.push( op );
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Expression::Operator::~Operator()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

EXP_OP_IMPLEMENT(Expression,Add,"+",PRI_CORE1,2)
{
	double op1 = arguments.top(); arguments.pop();
	double op2 = arguments.top(); arguments.pop();

	return (op2 + op1);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

EXP_OP_IMPLEMENT(Expression,Subtract,"-",PRI_CORE1,2)
{
	double op1 = arguments.top(); arguments.pop();
	double op2 = arguments.top(); arguments.pop();

	return (op2 - op1);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

EXP_OP_IMPLEMENT(Expression,Multiply,"*",PRI_CORE2,2)
{
	double op1 = arguments.top(); arguments.pop();
	double op2 = arguments.top(); arguments.pop();

	return (op2 * op1);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

EXP_OP_IMPLEMENT(Expression,Divide,"/",PRI_CORE2,2)
{
	double op1 = EXP_OP_ARGUMENT();
	double op2 = EXP_OP_ARGUMENT();

	return (op2 / op1);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// this operator doesn't really do anything, but it must be present to support multi-argument functions

EXP_OP_IMPLEMENT(Expression,Comma,",",PRI_UNARY,1)
{
	double op = EXP_OP_ARGUMENT();
	return op;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// this operator doesn't really do anything, but it must be present to support basic math rules

EXP_OP_IMPLEMENT(Expression,Positive,"+",PRI_UNARY,1)
{
	double op = EXP_OP_ARGUMENT();
	return op;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

EXP_OP_IMPLEMENT(Expression,Negate,"-",PRI_UNARY,1)
{
	double op = EXP_OP_ARGUMENT();
	return -op;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// this class is special, as it will not end up in the token-list

EXP_OP_IMPLEMENT(Expression,Parenthesis,"(",PRI_PARENTHESIS,1)
{
	return .0;
}

}
