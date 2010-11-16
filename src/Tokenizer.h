#ifndef __OPENVCL_TOKENIZER_H__
#define __OPENVCL_TOKENIZER_H__

/*
 * Tokenizer.h
 *
 * Copyright (C) 2004 Jesper Svennevid, Daniel Collin
 *
 * Licensed under the AFL v2.0. See the file LICENSE included with this
 * distribution for licensing terms.
 *
 */

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "Line.h"
#include "Token.h"

#include <string>
#include <sstream>
#include <list>

#include <assert.h>

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace vcl
{

class Tokenizer
{
public:

	Tokenizer();

	void setOperands( const std::list<Operand>& operands );

	bool parse( const Line& line );

	std::list<Token>& tokens();

	void setNewSyntax( bool newSyntax );
	bool newSyntax() const;

	unsigned int availableFloats() const;
	unsigned int availableIntegers() const;

protected:

	enum ContentMode
	{
		RESET,
		OPERAND,
		ARGUMENT,
		FINISH
	};

	enum ExecutionPath
	{
			MERGED,
			UPPER,
			LOWER
	};

	bool vsmMode() const;
	void setVsmMode( bool mode );

	bool inComment() const;
	void setComment( bool mode );

	ExecutionPath executionPath() const;
	void setExecutionPath( ExecutionPath path );

	ContentMode contentMode() const;
	void setContentMode( ContentMode mode );

	const Operand* identifyToken( Token& token );
	bool handleToken( Token& token );

	void setAvailableFloats( unsigned int floats );
	void setAvailableIntegers( unsigned int ints );

private:

	static bool isEmpty( const std::string& s );

	bool m_vsmMode;
	bool m_comment;
	bool m_newSyntax;

	unsigned int m_availableIntegers;
	unsigned int m_availableFloats;

	ContentMode m_contentMode;
	ExecutionPath m_executionPath;

	std::list<Token> m_tokens;

	const std::list<Operand>* m_operands;
};

#include "Tokenizer.inl"

}

#endif
