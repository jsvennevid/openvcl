/*
 * Parser.cpp
 *
 * Copyright (C) 2004 Jesper Svennevid, Daniel Collin
 *
 * Licensed under the AFL v2.0. See the file LICENSE included with this
 * distribution for licensing terms.
 *
 */

#include "Parser.h"
#include "Error.h"

#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>

#include <fcntl.h>
#include <stdlib.h>

#ifdef _WIN32
#include <windows.h>
#endif

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace vcl
{

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Parser::Parser()
{
	m_state = INVALID_STATE;
	m_tempCounter = 0;

	m_preParser = DISABLED;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Parser::~Parser()
{
	if( m_cmdLine.deleteTemp() )
	{
		for( std::list< std::string >::const_iterator i = m_tempFiles.begin(); i != m_tempFiles.end(); i++ )
			remove( (*i).c_str() );
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool Parser::create( int argc, char* argv[] )
{
	if( !m_cmdLine.parse( argc, argv ) )
	{
		Error::Display( Error( "Invalid Arguments" ) );
		return false;
	}

	if( m_cmdLine.showVersion() )
		setState( SHOW_VERSION );
	else if( m_cmdLine.showUsage() )
		setState( SHOW_USAGE );
	else
		setState( READ_INPUT );

	setupOperands();

	// set original input-file
	m_inputFile = m_cmdLine.input();
	m_sourceFile = m_cmdLine.input();

	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool Parser::begin()
{
	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool Parser::run()
{
	switch( m_state )
	{
		case SHOW_VERSION: return showVersion();
		case SHOW_USAGE: return showUsage();
		case READ_INPUT: return readInput();
		case PREPROCESS: return preProcess();
		case TOKENIZE: return tokenize();
		case ALLOCATE_REGISTERS: return allocateRegisters();
		case GENERATE_CODE : return generateCode();
		case WRITE_OUTPUT: return writeOutput();
		default: return false;
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool Parser::end()
{
	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Parser::setupOperands()
{
	// setup command templates

	// tokenizer operands

	m_operands.push_back( Operand( ".syntax", 			1, Operand::PREPROCESSOR, "'old'|'new'" ) );

	m_operands.push_back( Operand( ".vu",						0, Operand::PREPROCESSOR, "" ) );
	m_operands.push_back( Operand( ".vsm",					0, Operand::PREPROCESSOR, "" ) );
	m_operands.push_back( Operand( ".raw",					0, Operand::PREPROCESSOR, "" ) );
	m_operands.push_back( Operand( ".endvsm",				0, Operand::PREPROCESSOR, "" ) );
	m_operands.push_back( Operand( ".endraw",				0, Operand::PREPROCESSOR, "" ) );

	m_operands.push_back( Operand( ".rawloop",			0, Operand::PREPROCESSOR, "" ) );
	m_operands.push_back( Operand( ".endrawloop",		0, Operand::PREPROCESSOR, "" ) );

	// VCL operands

	m_operands.push_back( Operand( ".init_vf",			1, Operand::PREPROCESSOR|Operand::MULTI|Operand::FILTERED,	"vf:noalias:range,..." ) );
	m_operands.push_back( Operand( ".init_vi",			1, Operand::PREPROCESSOR|Operand::MULTI|Operand::FILTERED,	"vi:noalias:range,..." ) );
	m_operands.push_back( Operand( ".rem_vf",				1, Operand::PREPROCESSOR|Operand::MULTI|Operand::FILTERED,	"vf:noalias:range,..." ) );
	m_operands.push_back( Operand( ".rem_vi",				1, Operand::PREPROCESSOR|Operand::MULTI|Operand::FILTERED,	"vi:noalias:range,..." ) );
	m_operands.push_back( Operand( ".init_vf_all",	0, Operand::PREPROCESSOR|Operand::FILTERED,									"" ) );
	m_operands.push_back( Operand( ".init_vi_all",	0, Operand::PREPROCESSOR|Operand::FILTERED,									"" ) );

	m_operands.push_back( Operand( "--LoopCS",			2, Operand::PREPROCESSOR|Operand::FILTERED,	"imm:integer,imm:integer" ) );
	m_operands.push_back( Operand( "--LoopExtra",		1, Operand::PREPROCESSOR|Operand::FILTERED, "imm:integer" ) );
	m_operands.push_back( Operand( "--LoopAbs",			1, Operand::PREPROCESSOR|Operand::FILTERED, "imm:integer" ) );
	m_operands.push_back( Operand( "--barrier",			0, Operand::PREPROCESSOR|Operand::FILTERED, "" ) );
	m_operands.push_back( Operand( "--cont",				0, Operand::PREPROCESSOR|Operand::FILTERED, "" ) );
	m_operands.push_back( Operand( "--enter",				0, Operand::PREPROCESSOR|Operand::FILTERED, "",										Operand::ENTER ) );
	m_operands.push_back( Operand( "--endenter",		0, Operand::PREPROCESSOR|Operand::FILTERED, "",										Operand::ENTER ) );
	m_operands.push_back( Operand( "--exit",				0, Operand::PREPROCESSOR|Operand::FILTERED, "",										Operand::EXIT ) );
	m_operands.push_back( Operand( "--exitm",				1, Operand::PREPROCESSOR|Operand::MULTI|Operand::FILTERED, "imm",	Operand::EXIT ) );
	m_operands.push_back( Operand( "--endexit",			0, Operand::PREPROCESSOR|Operand::FILTERED, "",										Operand::EXIT ) );

	m_operands.push_back( Operand( "in_vi",					1, Operand::PREPROCESSOR|Operand::FILTERED, "imm(vi):noalias",		Operand::ENTER ) );
	m_operands.push_back( Operand( "in_vf",					1, Operand::PREPROCESSOR|Operand::FILTERED, "imm(vf):noalias",		Operand::ENTER ) );
	m_operands.push_back( Operand( "in_hw_acc",			1, Operand::PREPROCESSOR|Operand::FILTERED, "acc",								Operand::ENTER ) );
	m_operands.push_back( Operand( "in_hw_clip",		1, Operand::PREPROCESSOR|Operand::FILTERED, "'clip'",							Operand::ENTER ) );
	m_operands.push_back( Operand( "in_hw_i",				1, Operand::PREPROCESSOR|Operand::FILTERED, "i",									Operand::ENTER ) );
	m_operands.push_back( Operand( "in_hw_p",				1, Operand::PREPROCESSOR|Operand::FILTERED, "p",									Operand::ENTER ) );
	m_operands.push_back( Operand( "in_hw_q",				1, Operand::PREPROCESSOR|Operand::FILTERED, "q",									Operand::ENTER ) );
	m_operands.push_back( Operand( "in_hw_r",				1, Operand::PREPROCESSOR|Operand::FILTERED, "r",									Operand::ENTER ) );
	m_operands.push_back( Operand( "in_hw_status",	1, Operand::PREPROCESSOR|Operand::FILTERED, "'status'",						Operand::ENTER ) );

	m_operands.push_back( Operand( "out_vi",				1, Operand::PREPROCESSOR|Operand::FILTERED, "imm(vi):noalias",		Operand::EXIT ) );
	m_operands.push_back( Operand( "out_vf",				1, Operand::PREPROCESSOR|Operand::FILTERED, "imm(vf):noalias",		Operand::EXIT ) );
	m_operands.push_back( Operand( "out_hw_acc",		1, Operand::PREPROCESSOR|Operand::FILTERED, "acc",								Operand::EXIT ) );
	m_operands.push_back( Operand( "out_hw_clip",		1, Operand::PREPROCESSOR|Operand::FILTERED, "'clip'",							Operand::EXIT ) );
	m_operands.push_back( Operand( "out_hw_i",			1, Operand::PREPROCESSOR|Operand::FILTERED, "i",									Operand::EXIT ) );
	m_operands.push_back( Operand( "out_hw_p",			1, Operand::PREPROCESSOR|Operand::FILTERED, "p",									Operand::EXIT ) );
	m_operands.push_back( Operand( "out_hw_q",			1, Operand::PREPROCESSOR|Operand::FILTERED, "q",									Operand::EXIT ) );
	m_operands.push_back( Operand( "out_hw_r",			1, Operand::PREPROCESSOR|Operand::FILTERED, "r",									Operand::EXIT ) );

	m_operands.push_back( Operand( ".mpg",					1, Operand::PREPROCESSOR|Operand::FILTERED, "imm" ) );
	m_operands.push_back( Operand( ".name",					1, Operand::PREPROCESSOR|Operand::FILTERED, "imm" ) );

	m_operands.push_back( Operand( ".global",				1, Operand::PREPROCESSOR, "imm" ) );

	// VU Common instructions

	m_operands.push_back( Operand( "NOP",						0, 0, "",	Operand::INVALID, 1, 4) );

	// VU Upper Execution Path

	const char* upper0 = "vf:dest:write,vf:dest,vf:bc";
	const char* upper1 = "vf:dest:write,vf:dest,vf:dest";
	const char* upper2 = "acc:dest:write,vf:dest,vf:bc";
	const char* upper3 = "acc:dest:write,vf:dest,vf:dest";
	const char* upper4 = "vf:dest:write,vf:dest";
	const char* upper5 = "vf:dest:write,vf:dest,i";
	const char* upper6 = "vf:dest:write,vf:dest,q";
	const char* upper7 = "acc:dest:write,vf:dest,i";
	const char* upper8 = "acc:dest:write,vf:dest,q";

	m_operands.push_back( Operand( "ABS",						2, Operand::UPPER|Operand::DEST,			upper4,					Operand::FMAC, 1, 4		) );

	m_operands.push_back( Operand( "ADD",						3, Operand::UPPER|Operand::DEST,			upper1,					Operand::FMAC, 1, 4		) );
	m_operands.push_back( Operand( "ADDi",					3, Operand::UPPER|Operand::DEST,			upper5,					Operand::FMAC, 1, 4		) );
	m_operands.push_back( Operand( "ADDq",					3, Operand::UPPER|Operand::DEST,			upper6,					Operand::FMAC, 1, 4		) );
	m_operands.push_back( Operand( "ADD",						3, Operand::UPPER|Operand::BROADCAST, upper0,					Operand::FMAC, 1, 4		) );
	m_operands.push_back( Operand( "ADDA",					3, Operand::UPPER|Operand::DEST,			upper3,					Operand::FMAC, 1, 4		) );
	m_operands.push_back( Operand( "ADDAi",					3, Operand::UPPER|Operand::DEST,			upper7,					Operand::FMAC, 1, 4		) );
	m_operands.push_back( Operand( "ADDAq",					3, Operand::UPPER|Operand::DEST,			upper8,					Operand::FMAC, 1, 4		) );
	m_operands.push_back( Operand( "ADDA",					3, Operand::UPPER|Operand::BROADCAST, upper2,					Operand::FMAC, 1, 4		) );

	m_operands.push_back( Operand( "SUB",						3, Operand::UPPER|Operand::DEST,			upper1,					Operand::FMAC, 1, 4		) );
	m_operands.push_back( Operand( "SUBi",					3, Operand::UPPER|Operand::DEST,			upper5,					Operand::FMAC, 1, 4		) );
	m_operands.push_back( Operand( "SUBq",					3, Operand::UPPER|Operand::DEST,			upper6,					Operand::FMAC, 1, 4		) );
	m_operands.push_back( Operand( "SUB",						3, Operand::UPPER|Operand::BROADCAST,	upper0,					Operand::FMAC, 1, 4		) );
	m_operands.push_back( Operand( "SUBA",					3, Operand::UPPER|Operand::DEST,			upper3,					Operand::FMAC, 1, 4		) );
	m_operands.push_back( Operand( "SUBAi",					3, Operand::UPPER|Operand::DEST,			upper7,					Operand::FMAC, 1, 4		) );
	m_operands.push_back( Operand( "SUBAq",					3, Operand::UPPER|Operand::DEST,			upper8,					Operand::FMAC, 1, 4		) );
	m_operands.push_back( Operand( "SUBA",					3, Operand::UPPER|Operand::BROADCAST, upper2,					Operand::FMAC, 1, 4		) );

	m_operands.push_back( Operand( "MUL",						3, Operand::UPPER|Operand::DEST,			upper1,					Operand::FMAC, 1, 4		) );
	m_operands.push_back( Operand( "MULi",					3, Operand::UPPER|Operand::DEST,			upper5,					Operand::FMAC, 1, 4		) );
	m_operands.push_back( Operand( "MULq",					3, Operand::UPPER|Operand::DEST,			upper6,					Operand::FMAC, 1, 4		) );
	m_operands.push_back( Operand( "MUL",						3, Operand::UPPER|Operand::BROADCAST, upper0,					Operand::FMAC, 1, 4		) );
	m_operands.push_back( Operand( "MULA",					3, Operand::UPPER|Operand::DEST,			upper3,					Operand::FMAC, 1, 4		) );
	m_operands.push_back( Operand( "MULAi",					3, Operand::UPPER|Operand::DEST,			upper7,					Operand::FMAC, 1, 4		) );
	m_operands.push_back( Operand( "MULAq",					3, Operand::UPPER|Operand::DEST,			upper8,					Operand::FMAC, 1, 4		) );
	m_operands.push_back( Operand( "MULA",					3, Operand::UPPER|Operand::BROADCAST, upper2,					Operand::FMAC, 1, 4		) );

	m_operands.push_back( Operand( "MADD",					3, Operand::UPPER|Operand::DEST,			upper1,					Operand::FMAC, 1, 4		) );
	m_operands.push_back( Operand( "MADDi",					3, Operand::UPPER|Operand::DEST,			upper5,					Operand::FMAC, 1, 4		) );
	m_operands.push_back( Operand( "MADDq",					3, Operand::UPPER|Operand::DEST,			upper6,					Operand::FMAC, 1, 4		) );
	m_operands.push_back( Operand( "MADD",					3, Operand::UPPER|Operand::BROADCAST, upper0,					Operand::FMAC, 1, 4		) );
	m_operands.push_back( Operand( "MADDA",					3, Operand::UPPER|Operand::DEST,			upper3,					Operand::FMAC, 1, 4		) );
	m_operands.push_back( Operand( "MADDAi",				3, Operand::UPPER|Operand::DEST,			upper7,					Operand::FMAC, 1, 4		) );
	m_operands.push_back( Operand( "MADDAq",				3, Operand::UPPER|Operand::DEST,			upper8,					Operand::FMAC, 1, 4		) );
	m_operands.push_back( Operand( "MADDA",					3, Operand::UPPER|Operand::BROADCAST, upper2,					Operand::FMAC, 1, 4		) );

	m_operands.push_back( Operand( "MSUB",					3, Operand::UPPER|Operand::DEST,			upper1,					Operand::FMAC, 1, 4		) );
	m_operands.push_back( Operand( "MSUBi",					3, Operand::UPPER|Operand::DEST,			upper5,					Operand::FMAC, 1, 4		) );
	m_operands.push_back( Operand( "MSUBq",					3, Operand::UPPER|Operand::DEST,			upper6,					Operand::FMAC, 1, 4		) );
	m_operands.push_back( Operand( "MSUB",					3, Operand::UPPER|Operand::BROADCAST, upper0,					Operand::FMAC, 1, 4		) );
	m_operands.push_back( Operand( "MSUBA",					3, Operand::UPPER|Operand::DEST,			upper3,					Operand::FMAC, 1, 4		) );
	m_operands.push_back( Operand( "MSUBAi",				3, Operand::UPPER|Operand::DEST,			upper7,					Operand::FMAC, 1, 4		) );
	m_operands.push_back( Operand( "MSUBAq",				3, Operand::UPPER|Operand::DEST,			upper8,					Operand::FMAC, 1, 4		) );
	m_operands.push_back( Operand( "MSUBA",					3, Operand::UPPER|Operand::BROADCAST,	upper2,					Operand::FMAC, 1, 4		) );

	m_operands.push_back( Operand( "MAX",						3, Operand::UPPER|Operand::DEST,			upper1,					Operand::FMAC, 1, 4		) );
	m_operands.push_back( Operand( "MAXi",					3, Operand::UPPER|Operand::DEST,			upper5,					Operand::FMAC, 1, 4		) );
	m_operands.push_back( Operand( "MAX",						3, Operand::UPPER|Operand::BROADCAST, upper0,					Operand::FMAC, 1, 4		) );

	m_operands.push_back( Operand( "MINI",					3, Operand::UPPER|Operand::DEST,			upper1,					Operand::FMAC, 1, 4		) );
	m_operands.push_back( Operand( "MINIi",					3, Operand::UPPER|Operand::DEST,			upper5,					Operand::FMAC, 1, 4		) );
	m_operands.push_back( Operand( "MINI",					3, Operand::UPPER|Operand::BROADCAST, upper0,					Operand::FMAC, 1, 4		) );

	m_operands.push_back( Operand( "OPMULA",				3, Operand::UPPER|Operand::XYZ,				upper3,					Operand::FMAC, 1, 4		) );
	m_operands.push_back( Operand( "OPMSUB",				3, Operand::UPPER|Operand::XYZ,				upper1,					Operand::FMAC, 1, 4		) );

	m_operands.push_back( Operand( "FTOI0",					2, Operand::UPPER|Operand::DEST,			upper4,					Operand::FMAC, 1, 4		) );
	m_operands.push_back( Operand( "FTOI4",					2, Operand::UPPER|Operand::DEST,			upper4,					Operand::FMAC, 1, 4		) );
	m_operands.push_back( Operand( "FTOI12",				2, Operand::UPPER|Operand::DEST,			upper4,					Operand::FMAC, 1, 4		) );
	m_operands.push_back( Operand( "FTOI15",				2, Operand::UPPER|Operand::DEST,			upper4,					Operand::FMAC, 1, 4		) );
	m_operands.push_back( Operand( "ITOF0",					2, Operand::UPPER|Operand::DEST,			upper4,					Operand::FMAC, 1, 4		) );
	m_operands.push_back( Operand( "ITOF4",					2, Operand::UPPER|Operand::DEST,			upper4,					Operand::FMAC, 1, 4		) );
	m_operands.push_back( Operand( "ITOF12",				2, Operand::UPPER|Operand::DEST,			upper4,					Operand::FMAC, 1, 4		) );
	m_operands.push_back( Operand( "ITOF15",				2, Operand::UPPER|Operand::DEST,			upper4,					Operand::FMAC, 1, 4		) );

	// TODO: complete templates for these ops
	m_operands.push_back( Operand( "CLIP",					2, Operand::UPPER|Operand::XYZ,				"vf:dest:write,vf:wcomp",					Operand::FMAC, 1, 4		) );
	m_operands.push_back( Operand( "CLIPw",					2, Operand::UPPER|Operand::XYZ,				"vf:dest:write,vf:wcomp",					Operand::FMAC, 1, 4		) );
	m_operands.push_back( Operand( "CLIPLw",				2, Operand::UPPER|Operand::XYZ,				"vf:dest:write,vf:wcomp",					Operand::FMAC, 1, 4		) );

	// VU Lower Execution Path

	const char* lower0 = "vf:dest:write,vf:dest";
	const char* lower1 = "vi:write,vi,vi";
	const char* lower2 = "vi:write,vi,imm";
	const char* lower3 = "vi,imm:branch";
	const char* lower4 = "p:write,vf";
	const char* lower5 = "p:write,vf:flag";
	const char* lower6 = "vi:write,imm";
	const char* lower7 = "vi:write,vi";

	m_operands.push_back( Operand( "DIV",						3, Operand::LOWER,										"q:write,vf:flag,vf:flag",			Operand::FDIV, 7, 7		) );
	m_operands.push_back( Operand( "SQRT",					2, Operand::LOWER,										"q:write,vf:flag",							Operand::FDIV, 7, 7		) );
	m_operands.push_back( Operand( "RSQRT",					3, Operand::LOWER,										"q:write,vf:flag,vf:flag",			Operand::FDIV, 13, 13	) );
	m_operands.push_back( Operand( "WAITQ",					0, Operand::LOWER, 										"",															Operand::FDIV, 1, 1		) );

	m_operands.push_back( Operand( "IADD",					3, Operand::LOWER, 										lower1,													Operand::IALU, 1, 1		) );
	m_operands.push_back( Operand( "IADDI",					3, Operand::LOWER, 										lower2,													Operand::IALU, 1, 1		) );
	m_operands.push_back( Operand( "IADDIU",				3, Operand::LOWER, 										lower2,													Operand::IALU, 1, 1		) );
	m_operands.push_back( Operand( "IAND",					3, Operand::LOWER, 										lower1,													Operand::IALU, 1, 1		) );
	m_operands.push_back( Operand( "IOR",						3, Operand::LOWER, 										lower1,													Operand::IALU, 1, 1		) );
	m_operands.push_back( Operand( "ISUB",					3, Operand::LOWER, 										lower1,													Operand::IALU, 1, 1		) );
	m_operands.push_back( Operand( "ISUBIU",				3, Operand::LOWER, 										lower2,													Operand::IALU, 1, 1		) );

	m_operands.push_back( Operand( "MOVE",					2, Operand::LOWER|Operand::DEST,			lower0,													Operand::INVALID, 1, 4) );
	m_operands.push_back( Operand( "MFIR",					2, Operand::LOWER|Operand::DEST,			"vf:dest:write,vi",							Operand::INVALID, 1, 4) );
	m_operands.push_back( Operand( "MTIR",					2, Operand::LOWER, 										"vi:write,vf:flag",							Operand::INVALID, 1, 1) );
	m_operands.push_back( Operand( "MR32",					2, Operand::LOWER|Operand::DEST,			"vf:dest:write,vf:dest:rotate",	Operand::INVALID, 1, 4) );

	m_operands.push_back( Operand( "LQ",						2, Operand::LOWER|Operand::DEST, 			"vf:dest:write,(vi):zero|imm(vi)",Operand::LSU, 1, 4		) );
	m_operands.push_back( Operand( "LQD",						2, Operand::LOWER|Operand::DEST,			"vf:dest:write,(vi):predec",		Operand::LSU, 1, 4		) );
	m_operands.push_back( Operand( "LQI",						2, Operand::LOWER|Operand::DEST,			"vf:dest:write,(vi):postinc",		Operand::LSU, 1, 4		) );
	m_operands.push_back( Operand( "SQ",						2, Operand::LOWER|Operand::DEST, 			"vf:dest,(vi):zero|imm(vi)",		Operand::LSU, 1, 4		) );
	m_operands.push_back( Operand( "SQD",						2, Operand::LOWER|Operand::DEST,			"vf:dest,(vi):predec",					Operand::LSU, 1, 4		) );
	m_operands.push_back( Operand( "SQI",						2, Operand::LOWER|Operand::DEST,			"vf:dest,(vi):postinc",					Operand::LSU, 1, 4		) );
	m_operands.push_back( Operand( "ILW",						2, Operand::LOWER|Operand::DEST,			"vi:write,(vi):zero:dest|imm(vi):dest",Operand::LSU, 1, 4		) );
	m_operands.push_back( Operand( "ISW",						2, Operand::LOWER|Operand::DEST,			"vi,(vi):zero:dest|imm(vi):dest",Operand::LSU, 1, 4		) );
	m_operands.push_back( Operand( "ILWR",					2, Operand::LOWER|Operand::DEST, 			"vi:write,(vi):dest",						Operand::LSU, 1, 4		) );
	m_operands.push_back( Operand( "ISWR",					2, Operand::LOWER|Operand::DEST, 			"vi,(vi):dest",									Operand::LSU, 1, 4		) );

	m_operands.push_back( Operand( "LOI",						1, Operand::LOWER|Operand::IWRITE,		"imm:evaluate:raw",							Operand::LSU,	1, 1		) );

	m_operands.push_back( Operand( "RINIT",					2, Operand::LOWER, 										"r:write,vf:flag",							Operand::RANDU, 1, 1	) );
	m_operands.push_back( Operand( "RGET",					2, Operand::LOWER|Operand::DEST, 			"vf:dest:write,r",							Operand::RANDU, 1, 4	) );
	m_operands.push_back( Operand( "RNEXT",					2, Operand::LOWER|Operand::DEST,			"vf:dest:write,r",							Operand::RANDU, 1, 4	) );
	m_operands.push_back( Operand( "RXOR",					2, Operand::LOWER,										"r:write,vf:flag",							Operand::RANDU, 1, 1	) );

	m_operands.push_back( Operand( "FSAND",					2, Operand::LOWER,										lower6,													Operand::IALU, 1, 1		) );
	m_operands.push_back( Operand( "FSEQ",					2, Operand::LOWER,										lower6,													Operand::IALU, 1, 1		) );
	m_operands.push_back( Operand( "FSOR",					2, Operand::LOWER,										lower6,													Operand::IALU, 1, 1		) );
	m_operands.push_back( Operand( "FSSET",					1, Operand::LOWER,										"imm",													Operand::IALU, 1, 4		) );
	m_operands.push_back( Operand( "FMAND",					2, Operand::LOWER,										lower7,													Operand::IALU, 1, 1		) );
	m_operands.push_back( Operand( "FMEQ",					2, Operand::LOWER,										lower7,													Operand::IALU, 1, 1		) );
	m_operands.push_back( Operand( "FMOR",					2, Operand::LOWER,										lower7,													Operand::IALU, 1, 1		) );
	m_operands.push_back( Operand( "FCAND",					2, Operand::LOWER,										lower6,													Operand::IALU, 1, 1		) );
	m_operands.push_back( Operand( "FCEQ",					2, Operand::LOWER,										lower6,													Operand::IALU, 1, 1		) );
	m_operands.push_back( Operand( "FCOR",					2, Operand::LOWER,										lower6,													Operand::IALU, 1, 1		) );
	m_operands.push_back( Operand( "FCSET",					1, Operand::LOWER,										"imm",													Operand::IALU, 1, 4		) );
	m_operands.push_back( Operand( "FCGET",					1, Operand::LOWER,										"vi:write",											Operand::IALU, 1, 1		) );

	m_operands.push_back( Operand( "IBEQ",					3, Operand::LOWER|Operand::DYNAMIC,		"vi,vi,imm:branch",							Operand::BRU, 2, 2		) );
	m_operands.push_back( Operand( "IBGEZ",					2, Operand::LOWER|Operand::DYNAMIC, 	lower3,													Operand::BRU, 2, 2		) );
	m_operands.push_back( Operand( "IBGTZ",					2, Operand::LOWER|Operand::DYNAMIC,		lower3,													Operand::BRU, 2, 2		) );
	m_operands.push_back( Operand( "IBLEZ",					2, Operand::LOWER|Operand::DYNAMIC,		lower3,													Operand::BRU, 2, 2		) );
	m_operands.push_back( Operand( "IBLTZ",					2, Operand::LOWER|Operand::DYNAMIC,		lower3,													Operand::BRU, 2, 2		) );
	m_operands.push_back( Operand( "IBNE",					3, Operand::LOWER|Operand::DYNAMIC,		"vi,vi,imm:branch",							Operand::BRU, 2, 2		) );
	m_operands.push_back( Operand( "B",							1, Operand::LOWER,							 			"imm:branch",										Operand::BRU, 2, 2		) );
	m_operands.push_back( Operand( "BAL",						2, Operand::LOWER,										"vi:write:address,imm:branch",	Operand::BRU, 2, 2		) );
	m_operands.push_back( Operand( "JR",						1, Operand::LOWER,							 			"vi:branch",										Operand::BRU, 2, 2		) );
	m_operands.push_back( Operand( "JALR",					2, Operand::LOWER,							 			"vi:write:address,vi:branch",		Operand::BRU, 2, 2		) );

	m_operands.push_back( Operand( "MFP",						2, Operand::LOWER|Operand::DEST,			"vf:dest:write,p",							Operand::EFU, 1, 4		) );
	m_operands.push_back( Operand( "WAITP",					0, Operand::LOWER,										"",															Operand::EFU, 1, 1		) );

	m_operands.push_back( Operand( "ESADD",					2, Operand::LOWER,										lower4,													Operand::EFU, 10, 11	) );
	m_operands.push_back( Operand( "ERSADD",				2, Operand::LOWER,										lower4,													Operand::EFU, 17, 18	) );
	m_operands.push_back( Operand( "ELENG",					2, Operand::LOWER,										lower4,													Operand::EFU, 17, 18	) );
	m_operands.push_back( Operand( "ERLENG",				2, Operand::LOWER,										lower4,													Operand::EFU, 23, 24	) );
	m_operands.push_back( Operand( "EATANxy",				2, Operand::LOWER,										lower4,													Operand::EFU, 53, 54	) );
	m_operands.push_back( Operand( "EATANxz",				2, Operand::LOWER,										lower4,													Operand::EFU, 53, 54	) );
	m_operands.push_back( Operand( "ESUM",					2, Operand::LOWER,										lower4,													Operand::EFU, 11, 12	) );
	m_operands.push_back( Operand( "ERCPR",					2, Operand::LOWER,										lower4,													Operand::EFU, 11, 12 	) );
	m_operands.push_back( Operand( "ERSQRT",				2, Operand::LOWER,										lower5,													Operand::EFU, 17, 18	) );
	m_operands.push_back( Operand( "ESIN",					2, Operand::LOWER,										lower5,													Operand::EFU, 28, 29	) );
	m_operands.push_back( Operand( "EATAN",					2, Operand::LOWER,										lower5,													Operand::EFU, 53, 54	) );
	m_operands.push_back( Operand( "EEXP",					2, Operand::LOWER,										lower5,													Operand::EFU, 43, 44	) );

	m_operands.push_back( Operand( "XGKICK",				1, Operand::LOWER,										"vi",														Operand::INVALID, 1, 1) );
	m_operands.push_back( Operand( "XTOP",					1, Operand::LOWER,										"vi:write",											Operand::INVALID, 1, 1) );
	m_operands.push_back( Operand( "XITOP",					1, Operand::LOWER,										"vi:write",											Operand::INVALID, 1, 1) );

	// operand simplifications

	Operand* add = getOperand( "ADD", Operand::UPPER|Operand::DEST );
	add->addAlternative( add );
	add->addAlternative( getOperand( "ADDA",	Operand::UPPER|Operand::BROADCAST ) );
	add->addAlternative( getOperand( "ADD",		Operand::UPPER|Operand::BROADCAST ) );
	add->addAlternative( getOperand( "ADDi",	Operand::UPPER|Operand::DEST ) );
	add->addAlternative( getOperand( "ADDq",	Operand::UPPER|Operand::DEST ) );
	add->addAlternative( getOperand( "ADDA",	Operand::UPPER|Operand::DEST ) );
	add->addAlternative( getOperand( "ADDAi",	Operand::UPPER|Operand::DEST ) );
	add->addAlternative( getOperand( "ADDAq",	Operand::UPPER|Operand::DEST ) );

	Operand* adda = getOperand( "ADDA", Operand::UPPER|Operand::DEST );
	adda->addAlternative( adda );
	adda->addAlternative( getOperand( "ADDA",	Operand::UPPER|Operand::BROADCAST ) );
	adda->addAlternative( getOperand( "ADDAi",	Operand::UPPER|Operand::DEST ) );
	adda->addAlternative( getOperand( "ADDAq",	Operand::UPPER|Operand::DEST ) );

	Operand* sub = getOperand( "SUB", Operand::UPPER|Operand::DEST );
	sub->addAlternative( sub );
	sub->addAlternative( getOperand( "SUBA",	Operand::UPPER|Operand::BROADCAST ) );
	sub->addAlternative( getOperand( "SUB",		Operand::UPPER|Operand::BROADCAST ) );
	sub->addAlternative( getOperand( "SUBi",	Operand::UPPER|Operand::DEST ) );
	sub->addAlternative( getOperand( "SUBq",	Operand::UPPER|Operand::DEST ) );
	sub->addAlternative( getOperand( "SUBA",	Operand::UPPER|Operand::DEST ) );
	sub->addAlternative( getOperand( "SUBAi",	Operand::UPPER|Operand::DEST ) );
	sub->addAlternative( getOperand( "SUBAq",	Operand::UPPER|Operand::DEST ) );

	Operand* suba = getOperand( "SUBA", Operand::UPPER|Operand::DEST );
	suba->addAlternative( suba );
	suba->addAlternative( getOperand( "SUBA",	Operand::UPPER|Operand::BROADCAST ) );
	suba->addAlternative( getOperand( "SUBAi",	Operand::UPPER|Operand::DEST ) );
	suba->addAlternative( getOperand( "SUBAq",	Operand::UPPER|Operand::DEST ) );

	Operand* mul = getOperand( "MUL", Operand::UPPER|Operand::DEST );
	mul->addAlternative( mul );
	mul->addAlternative( getOperand( "MULA",	Operand::UPPER|Operand::BROADCAST ) );
	mul->addAlternative( getOperand( "MUL",		Operand::UPPER|Operand::BROADCAST ) );
	mul->addAlternative( getOperand( "MULi",	Operand::UPPER|Operand::DEST ) );
	mul->addAlternative( getOperand( "MULq",	Operand::UPPER|Operand::DEST ) );
	mul->addAlternative( getOperand( "MULA",	Operand::UPPER|Operand::DEST ) );
	mul->addAlternative( getOperand( "MULAi",	Operand::UPPER|Operand::DEST ) );
	mul->addAlternative( getOperand( "MULAq",	Operand::UPPER|Operand::DEST ) );

	Operand* mula = getOperand( "MULA", Operand::UPPER|Operand::DEST );
	mula->addAlternative( mula );
	mula->addAlternative( getOperand( "MULA",	Operand::UPPER|Operand::BROADCAST ) );
	mula->addAlternative( getOperand( "MULAi",	Operand::UPPER|Operand::DEST ) );
	mula->addAlternative( getOperand( "MULAq",	Operand::UPPER|Operand::DEST ) );

	Operand* madd = getOperand( "MADD", Operand::UPPER|Operand::DEST );
	madd->addAlternative( madd );
	madd->addAlternative( getOperand( "MADDA",	Operand::UPPER|Operand::BROADCAST ) );
	madd->addAlternative( getOperand( "MADD",		Operand::UPPER|Operand::BROADCAST ) );
	madd->addAlternative( getOperand( "MADDi",	Operand::UPPER|Operand::DEST ) );
	madd->addAlternative( getOperand( "MADDq",	Operand::UPPER|Operand::DEST ) );
	madd->addAlternative( getOperand( "MADDA",	Operand::UPPER|Operand::DEST ) );
	madd->addAlternative( getOperand( "MADDAi",	Operand::UPPER|Operand::DEST ) );
	madd->addAlternative( getOperand( "MADDAq",	Operand::UPPER|Operand::DEST ) );

	Operand* madda = getOperand( "MADDA", Operand::UPPER|Operand::DEST );
	madda->addAlternative( madda );
	madda->addAlternative( getOperand( "MADDA",	Operand::UPPER|Operand::BROADCAST ) );
	madda->addAlternative( getOperand( "MADDAi",	Operand::UPPER|Operand::DEST ) );
	madda->addAlternative( getOperand( "MADDAq",	Operand::UPPER|Operand::DEST ) );

	Operand* msub = getOperand( "MSUB", Operand::UPPER|Operand::DEST );
	msub->addAlternative( msub );
	msub->addAlternative( getOperand( "MSUBA",	Operand::UPPER|Operand::BROADCAST ) );
	msub->addAlternative( getOperand( "MSUB",		Operand::UPPER|Operand::BROADCAST ) );
	msub->addAlternative( getOperand( "MSUBi",	Operand::UPPER|Operand::DEST ) );
	msub->addAlternative( getOperand( "MSUBq",	Operand::UPPER|Operand::DEST ) );
	msub->addAlternative( getOperand( "MSUBA",	Operand::UPPER|Operand::DEST ) );
	msub->addAlternative( getOperand( "MSUBAi",	Operand::UPPER|Operand::DEST ) );
	msub->addAlternative( getOperand( "MSUBAq",	Operand::UPPER|Operand::DEST ) );

	Operand* msuba = getOperand( "MSUBA", Operand::UPPER|Operand::DEST );
	msuba->addAlternative( msuba );
	msuba->addAlternative( getOperand( "MSUBA",	Operand::UPPER|Operand::BROADCAST ) );
	msuba->addAlternative( getOperand( "MSUBAi",	Operand::UPPER|Operand::DEST ) );
	msuba->addAlternative( getOperand( "MSUBAq",	Operand::UPPER|Operand::DEST ) );

	Operand* max = getOperand( "MAX", Operand::UPPER|Operand::DEST );
	max->addAlternative( max );
	max->addAlternative( getOperand( "MAX", 		Operand::UPPER|Operand::BROADCAST ) );
	max->addAlternative( getOperand( "MAXi", 		Operand::UPPER|Operand::DEST ) );

	Operand* mini = getOperand( "MINI", Operand::UPPER|Operand::DEST );
	mini->addAlternative( mini );
	mini->addAlternative( getOperand( "MINI", 	Operand::UPPER|Operand::BROADCAST ) );
	mini->addAlternative( getOperand( "MINIi",	Operand::UPPER|Operand::DEST ) );
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Parser::setState( State state )
{
	m_state = state;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool Parser::readInput()
{
	if( m_cmdLine.input().length() > 0 )
	{
		std::ifstream stream( m_inputFile.c_str() );

		if( !stream.good() )
		{
			Error::Display( Error( "Could not open input" ) );
			return false;
		}

		m_files[ m_sourceFile.c_str() ] = ( File( m_cmdLine.input().c_str() ) );

		return readInputStream( stream );
	}
	else
	{
		m_files[ "" ] = File("stdin");
		return readInputStream( std::cin );
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool Parser::readInputStream( std::istream& stream )
{
	std::string buffer;
	std::string temp;
	unsigned int curr = 1;
	unsigned int original = 1;

	std::map<std::string,File>::iterator currFile = m_files.find( m_sourceFile );
	if( currFile == m_files.end() )
	{
		Error::Display( Error( "INTERNAL ERROR: Could not locate filename in map" ) );
		return false;
	}

	while( stream.good() )
	{
		std::getline( stream, buffer, '\n' );
		bool store = true;

		switch( m_preParser )
		{
			case DISABLED: original = curr; break;

			case GASP:
			{
				if( buffer.length() > 0 )
				{
					if( ';' == buffer[0] )
					{
						bool numbers = false;
						bool valid = true;
						std::string::size_type offset = 0, i;
						for( i = 1; valid && (i < buffer.length()); ++i )
						{
							if( numbers )
							{
								if( (' ' == buffer[i]) )
									break;

								if( ('0' <= buffer[i]) && ('9' >= buffer[i]) )
									continue;
							}
							else
							{
								if( ('0' <= buffer[i]) && ('9' >= buffer[i]) )
								{
									offset = i;
									numbers = true;
									continue;
								}
							}

							if( ' ' != buffer[i] )
								valid = false;
						}

						if( valid && numbers )
							original = strtoul( buffer.substr( offset, i-offset ).c_str(), NULL, 10 );

						store = false;
					}
				}
			}
			break;

			case CPP:
			{
				if( buffer.length() > 0 )
				{
					if( '#' == buffer[0] )
					{
						int number = 0;
						std::string name;
						bool valid = true;
						bool finished = false;
						std::string::size_type offset = 0, i;
						enum { START, LINE, STRING } mode = START;

						for( i = 1; !finished && valid && (i < buffer.length()); ++i )
						{
							switch( mode )
							{
								case START:
								{
									if( ' ' == buffer[i] )
										break;

									if( ('0' <= buffer[i]) && ('9' >= buffer[i]) )
									{
										mode = LINE;
										offset = i;
										break;
									}

									valid = false;
								}
								break;

								case LINE:
								{
									if( ('0' <= buffer[i]) && ('9' >= buffer[i]) )
										break;

									if( ' ' == buffer[i] )
									{
										mode = STRING;
										number = strtoul( buffer.substr( offset, i-offset ).c_str(), NULL, 10 );
										break;
									}

									valid = false;
								}
								break;

								case STRING:
								{
									if( ' ' == buffer[i] )
										break;

									if( '"' == buffer[i] )
									{
										offset = buffer.substr(i+1).find_last_of('"');

										if( offset != std::string::npos )
										{
											name = buffer.substr(i+1,offset);
											finished = true;
											break;
										}
									}

									valid = false;
								}
								break;
							}
						}

						if( valid && finished )
						{
							std::map<std::string,File>::iterator newFile = m_files.find( name );
							if( m_files.end() != newFile )
							{
								currFile = newFile;
								original = number-1;
							}
							else
							{
								m_files[ name ] = File(name);
								original = number-1;

								currFile = m_files.find( name );
								if( currFile == m_files.end() )
								{
									Error::Display( Error( "INTERNAL ERROR: Could not locate filename in map" ) );
									return false;
								}
							}
						}

						store = false;
						break;
					}
				}
				original++;
			}
			break;
		}

		if( store )
		{
			m_lines.push_back( Line( currFile->second, curr, original, buffer.substr( 0, buffer.find('\r') ) ) );
			curr++;
		}
	}

	if( m_cmdLine.runGasp() || m_cmdLine.runCpp() )
		setState( PREPROCESS );
	else
		setState( TOKENIZE );

	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool Parser::preProcess()
{
	// create temporary files

	std::string src = tempFilename();
	std::string dest = tempFilename();

	if( !src.length() || !dest.length() )
	{
		Error::Display( Error( "Failed creating unique filename" ) );
		return false;
	}

	m_tempFiles.push_back(src);
	m_tempFiles.push_back(dest);

	// write contents to file

	std::ofstream sf(src.c_str());
	if( !sf.good() )
	{
		Error::Display( Error( "Failed creating temporary file" ) );
		return false;
	}

	for( std::list<Line>::const_iterator line = m_lines.begin(); line != m_lines.end(); line++ )
		sf << (*line).content() << std::endl;

	sf.close();

	// run preprocessing tool

	std::string preprocessor;

	if( m_cmdLine.runCpp() )
	{
		preprocessor = m_cmdLine.cpp() + " -I. -nostdinc -x assembler-with-cpp";

		for( std::list<std::string>::const_iterator i = m_cmdLine.includes().begin(); i != m_cmdLine.includes().end(); i++ )
			preprocessor += " -I\"" + *i + "\"";

		preprocessor += " -o \"" + dest + "\" \"" + src + "\"";

		m_preParser = CPP;
		m_cmdLine.setRunCpp( false );
	}
	else
	{
		preprocessor = m_cmdLine.gasp() + " -p -s -c ';'";

		for( std::list<std::string>::const_iterator i = m_cmdLine.includes().begin(); i != m_cmdLine.includes().end(); i++ )
			preprocessor += " -I\"" + *i + "\"";

		preprocessor += " -o \"" + dest + "\" \"" + src + "\"";

		m_preParser = GASP;
		m_cmdLine.setRunGasp( false );
	}

	if( system( preprocessor.c_str() ) )
	{
		Error::Display( Error( "Preprocessor failed" ) );
		return false;
	}

	// restart input reader

	m_sourceFile = src;
	m_inputFile = dest;

	m_lines.clear();
	m_files.clear();
	setState( READ_INPUT );

	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool Parser::tokenize()
{
	m_tokenizer.setNewSyntax( m_cmdLine.newSyntax() );
	m_tokenizer.setOperands( m_operands );

	for( std::list<Line>::const_iterator i = m_lines.begin(); i != m_lines.end(); i++ )
	{
		if( !m_tokenizer.parse( *i ) )
			return false;
	}

	setState( ALLOCATE_REGISTERS );

	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool Parser::allocateRegisters()
{
	m_registerAllocator.setAvailableFloats( m_tokenizer.availableFloats() );
	m_registerAllocator.setAvailableIntegers( m_tokenizer.availableIntegers() );
	m_registerAllocator.setDynamicThreshold( m_cmdLine.threshold() );

	if( !m_registerAllocator.process( m_tokenizer.tokens() ) )
		return false;

	setState( GENERATE_CODE );

	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool Parser::generateCode()
{
	m_codeGenerator.setEmitSource( m_cmdLine.emitSource() );
	m_codeGenerator.setName( m_registerAllocator.name() );

	if( !m_codeGenerator.beginProcess( m_tokenizer.tokens() ) )
		return false;

	setState( WRITE_OUTPUT );

	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


bool Parser::writeOutput()
{
	if( m_cmdLine.output().length() > 0 )
	{
		std::ofstream stream( m_cmdLine.output().c_str() );

		if( !stream.good() )
		{
			Error::Display( Error( "Could not open output file" ) );
			return false;
		}

		return writeOutputStream( stream );
	}
	else
	{
		return writeOutputStream( std::cout );
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool Parser::writeOutputStream( std::ostream& stream )
{
	m_codeGenerator.write( stream );

	setState( EXIT );

	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool Parser::showVersion()
{
	// TODO: can we automate updates of this somehow
	std::cout << "OpenVCL Version 0.3.3" << std::endl;

	return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool Parser::showUsage()
{
	m_cmdLine.showUsage( std::cout );

	return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

std::string Parser::tempFilename()
{
#ifdef _WIN32
	char path[MAX_PATH];
	char buffer[MAX_PATH];

	if( GetTempPath( sizeof( path ), path ) )
	{
		if( GetTempFileName( path, "vcl", 0, buffer ) )
		{
			return buffer;
		}
	}
#else
	for( unsigned int count = 0; count < TEMPFILE_ATTEMPTS; count++ )
	{
		std::stringstream buffer;
		
		buffer << "/tmp/vcl" << std::hex << std::setfill('0') << std::setw(8) << ((getpid()+rand()) << 8) + m_tempCounter++;
		std::string temp = buffer.str();

		std::ofstream file(temp.c_str(),std::ifstream::out);

		if(file.good())
		{
			file.close();
			return temp;
		}
	}
#endif
	return "";
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Operand* Parser::getOperand( const char* name, unsigned int flags )
{
	for( std::list<Operand>::iterator i = m_operands.begin(); i != m_operands.end(); ++i )
	{
		if( !(*i).name().compare( name ) && ((*i).flags() == flags) )
			return &*i;
	}

	return NULL;
}

}
