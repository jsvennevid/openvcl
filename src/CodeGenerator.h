#ifndef __OPENVCL_CODEGENERATOR_H__
#define __OPENVCL_CODEGENERATOR_H__

/*
 * CodeGenerator.h
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

#include <list>
#include <string>
#include <istream>
#include <sstream>
#include <map>
#include "Token.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Class
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace vcl
{

class CodeGenerator
{
public:
	CodeGenerator();
	~CodeGenerator();

	bool beginProcess(const std::list<Token>& tokens);
	bool write(std::ostream& stream);

	void setEmitSource( bool emitSource );
	bool emitSource() const;

	void setName( const std::string& name );
	const std::string& name() const;

private:

	static unsigned int cleanFields( unsigned int fields, unsigned int flags, const Token& token );

	std::string generateInstruction(const Token& token);
	std::string generateOperand(const Token& token);

	std::string registerArg(const Token::Argument& arg, const Token& token);
	std::string immediateArg(const Token::Argument& arg, const Token& token );
	std::string accumulatorArg( const Token::Argument& arg, const Token& token );

	void addNopLine();

	std::list<std::string> m_codeLines;

	bool m_emitSource;
	std::string m_name;
};

#include "CodeGenerator.inl"

}

#endif //__OPENVCL_CODEGENERATOR_H__
