/*
 * CommandLine.cpp
 *
 * Copyright (C) 2004 Jesper Svennevid, Daniel Collin
 *
 * Licensed under the AFL v2.0. See the file LICENSE included with this
 * distribution for licensing terms.
 *
 */

#include "CommandLine.h"

#include <string.h>
#include <stdlib.h>

namespace vcl
{

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

CommandLine::CommandLine()
{
	m_emitSource	= false;
	m_reduceCode	= true;
	m_dumbCode	= false;
	m_generateE	= true;
	m_alignCode	= true;
	m_runGasp	= false;
	m_runCpp	= false;
	m_deleteTemp	= true;
	m_unrollLoops	= true;
	m_generateMpg	= false;
	m_newSyntax	= false;
	m_showUsage = false;

	m_threshold = 16;
	m_timeout	= 4;
	m_showVersion = false;

	m_gasp = "gasp";
	m_cpp = "cpp";

	// setup arguments

	m_options.push_back(Option('c',NULL,EMIT_SOURCECODE,false));
	m_options.push_back(Option('C',NULL,REDUCE_CODE,false));
	m_options.push_back(Option('d',NULL,DUMB_CODE,false));
	m_options.push_back(Option('e',NULL,GENERATE_E,false));
	m_options.push_back(Option('f',NULL,ALIGN_CODE,false));
	m_options.push_back(Option('g',NULL,RUN_GASP,false));
	m_options.push_back(Option('G',NULL,RUN_CPP,false));
	m_options.push_back(Option('h',NULL,SHOW_USAGE,false));
	m_options.push_back(Option('I',NULL,INCLUDE,true));
	m_options.push_back(Option('K',NULL,DELETE_TEMP,false));
	m_options.push_back(Option('L',NULL,UNROLL_LOOPS,false));
	m_options.push_back(Option('m',NULL,GENERATE_MPG,false));
	m_options.push_back(Option('n',NULL,NEW_SYNTAX,false));
	m_options.push_back(Option('o',NULL,OUTPUT,true));
	m_options.push_back(Option('t',NULL,TIMEOUT,true));
	m_options.push_back(Option('u',NULL,LABEL,true));

	m_options.push_back(Option('M',NULL,IGNORE,false));
	m_options.push_back(Option('P',NULL,IGNORE,false));
	m_options.push_back(Option('Z',NULL,IGNORE,false));

	m_options.push_back(Option('\0',"gasp",GASP_NAME,true));
	m_options.push_back(Option('\0',"cpp",CPP_NAME,true));
	m_options.push_back(Option('\0',"bthres",BRANCH_THRESHOLD,true));
	m_options.push_back(Option('\0',"version",SHOW_VERSION,false));
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool CommandLine::parse( int argc, char* argv[] )
{
	for( int i = 1; i < argc; i++ )
	{
		const Option* option = NULL;
		std::string argument;
		bool separate = false;

		if( !strncmp("--",argv[i],2) )
		{
			std::string operand = &argv[i][2];

			if( (argc-1) > i )
				argument = argv[i+1];

			separate = true;

			for( std::vector<Option>::const_iterator j = m_options.begin(); j != m_options.end(); j++ )
			{
				if( operand == (*j).longName() )
				{
					option = &(*j);
					break;
				}
			}

			if( !option )
				return false;
		}
		else if( !strncmp("-",argv[i],1) )
		{
			char c = argv[i][1];

			if( !c )
				return false;

			if( !argv[i][2] )
			{
				if( (argc-1) > i )
					argument = argv[i+1];

				separate = true;
			}
			else
				argument = &argv[i][2];

			for( std::vector<Option>::const_iterator j = m_options.begin(); j != m_options.end(); j++ )
			{
				if( c == (*j).shortName() )
				{
					option = &(*j);
					break;
				}
			}

			if( !option )
				return false;
		}
		else
		{
			m_input = argv[i];
		}

		if( option )
		{
			if( !argument.length() && option->argument() )
				return false;

			switch( option->action() )
			{
				case EMIT_SOURCECODE: m_emitSource = true; break;
				case REDUCE_CODE: m_reduceCode = false; break;
				case DUMB_CODE: m_dumbCode = true; break;
				case GENERATE_E: m_generateE = false; break;
				case ALIGN_CODE: m_alignCode = false; break;
				case RUN_GASP: m_runGasp = true; break;
				case RUN_CPP: m_runCpp = true; break;
				case SHOW_USAGE: m_showUsage = true; break;
				case INCLUDE: m_includes.push_back( argument ); break;
				case DELETE_TEMP: m_deleteTemp = false; break;
				case UNROLL_LOOPS: m_unrollLoops = false; break;
				case GENERATE_MPG: m_generateMpg = true; break;
				case NEW_SYNTAX: m_newSyntax = true; break;
				case OUTPUT: m_output = argument; break;
				case TIMEOUT: m_timeout = strtoul(argument.c_str(),NULL,10); break;
				case LABEL: m_label = argument; break;

				case GASP_NAME: m_gasp = argument; break;
				case CPP_NAME: m_cpp = argument; break;
				case BRANCH_THRESHOLD: m_threshold = strtoul(argument.c_str(),NULL,10); break;
				case SHOW_VERSION: m_showVersion = true; break;

				case IGNORE: break;
				break;

				default: return false;
			}

			if( separate && option->argument() )
				i++;
		}
	}

	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void CommandLine::showUsage( std::ostream& stream )
{
	stream << "OpenVCL [parameters] <input>" << std::endl;

	stream << std::endl << "  Parameters:" << std::endl;

	stream << "  -c              Emit nearly original source code as comments." << std::endl;
	stream << "  -C              Disable the code reduction pass." << std::endl;
	stream << "  -d              Dumb code is generated." << std::endl;
	stream << "  -e              Disable the generation of [E] bits at the end of the code." << std::endl;
	stream << "  -f              Disable the generation of alignment directives (.align n)" << std::endl;
	stream << "  -g              Run gasp on the input before any VCL-specific task is done" << std::endl;
	stream << "                    Commandline: \"-p -s -c ';'\". \"-I\" is also passed." << std::endl;
	stream << "  -G              Run the C preprocessor on the input." << std::endl;
	stream << "  -h              Print out the command-line help." << std::endl;
	stream << "  -I<includepath> To be used with \"-g\"; tells gasp where to find include-files." << std::endl;
	stream << "  -K              Temporary files created by pre-processors are not deleted." << std::endl;
	stream << "  -L              Globally disable loop code generation." << std::endl;
	stream << "  -M              Kept for compatibility with VCL." << std::endl;
	stream << "  -m              Generate \".mpg\" and DMA tags automatically." << std::endl;
	stream << "  -n              Enable new syntax." << std::endl;
	stream << "  -o<output>      Output filename." << std::endl;
	stream << "  -P              Kept for compatibility with VCL." << std::endl;
	stream << "  -t<timeout>     Specify optimizer timeout." << std::endl;
	stream << "  -u<string>      <string> is used as a unique string for label generation." << std::endl;
	stream << "  -Z              Kept for compatibility with VCL." << std::endl;

	stream << std::endl << "  OpenVCL exclusive parameters:" << std::endl;

	stream << "  --gasp <name>   Execute <name> instead of 'gasp' when preprocessing." << std::endl;
	stream << "  --cpp <name>    Execute <name> instead of 'cpp' when preprocessing." << std::endl;
	stream << "  --bthres <val>  Number of times a dynamic branch can be visited. (Default: 16)" << std::endl;
	stream << "  --version       Show program version." << std::endl;

	stream << std::endl << "  If no input or output file are specified, standard I/O will be used instead." << std::endl;

	stream << std::flush;
}

}
