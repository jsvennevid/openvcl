#ifndef __OPENVCL_PARSER_H__
#define __OPENVCL_PARSER_H__

/*
 * Parser.h
 *
 * Copyright (C) 2004 Jesper Svennevid, Daniel Collin
 *
 * Licensed under the AFL v2.0. See the file LICENSE included with this
 * distribution for licensing terms.
 *
 */

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "Line.h"
#include "CommandLine.h"
#include "Tokenizer.h"
#include "Operand.h"
#include "CodeGenerator.h"
#include "RegisterAllocator.h"

#include <string>
#include <list>
#include <istream>

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace vcl
{

class Parser
{
public:

	enum State
	{
		INVALID_STATE,
		SHOW_USAGE,
		SHOW_VERSION,

		READ_INPUT,
		PREPROCESS,
		TOKENIZE,
		ALLOCATE_REGISTERS,
		GENERATE_CODE,

		WRITE_OUTPUT,

		EXIT
	};

	enum PreParser
	{
		DISABLED,
		GASP,
		CPP
	};

	Parser();
	~Parser();

	bool create( int argc, char* argv[] );

	bool begin();
	bool run();
	bool end();

private:

	enum
	{
		TEMPFILE_ATTEMPTS = 10
	};

	void setupOperands();
	Operand* getOperand( const char* name, unsigned int flags );

	void setState( State state );

	bool showVersion();
	bool showUsage();
	bool readInput();
	bool preProcess();
	bool tokenize();
	bool allocateRegisters();
	bool generateCode();
	bool writeOutput();

	bool readInputStream( std::istream& stream );
	bool writeOutputStream( std::ostream& stream );

	std::string tempFilename();

	State m_state;
	CommandLine m_cmdLine;
	Tokenizer m_tokenizer;
	CodeGenerator m_codeGenerator;
	RegisterAllocator m_registerAllocator;

	PreParser m_preParser;

	unsigned int m_tempCounter;

	std::list< std::string > m_tempFiles;

	std::list<Line> m_lines;
	std::list<Operand> m_operands;

	std::map<std::string,File> m_files;

	std::string m_inputFile;
	std::string m_sourceFile;
};

}

#endif
