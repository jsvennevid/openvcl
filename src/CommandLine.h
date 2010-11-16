#ifndef __OPENVCL_COMMANDLINE_H__
#define __OPENVCL_COMMANDLINE_H__

/*
 * CommandLine.h
 *
 * Copyright (C) 2004 Jesper Svennevid, Daniel Collin
 *
 * Licensed under the AFL v2.0. See the file LICENSE included with this
 * distribution for licensing terms.
 *
 */

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <string>
#include <list>
#include <istream>
#include <vector>

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace vcl
{

class CommandLine
{
public:

	CommandLine();

	bool parse( int argc, char* argv[] );

	bool emitSource() const;
	bool reduceCode() const;
	bool dumbCode() const;
	bool generateE() const;
	bool alignCode() const;
	bool deleteTemp() const;
	bool unrollLoops() const;
	bool generateMpg() const;
	bool showUsage() const;

	void setRunGasp( bool runGasp );
	bool runGasp() const;
	void setRunCpp( bool runCpp );
	bool runCpp() const;
	void setNewSyntax( bool newSyntax );
	bool newSyntax() const;

	unsigned int threshold() const;
	unsigned int timeout() const;
	bool showVersion() const;

	std::list<std::string>& includes();
	
	std::string& input();
	std::string& output();

	std::string& label();
	std::string& gasp();
	std::string& cpp();

	void showUsage( std::ostream& stream );

private:

	enum Action	
	{
		// standard VCL options

		EMIT_SOURCECODE,
		REDUCE_CODE,
		DUMB_CODE,
		GENERATE_E,
		ALIGN_CODE,
		RUN_GASP,
		RUN_CPP,
		INCLUDE,
		DELETE_TEMP,
		UNROLL_LOOPS,
		GENERATE_MPG,
		NEW_SYNTAX,
		OUTPUT,
		SHOW_USAGE,
		TIMEOUT,
		LABEL,

		// extended options

		GASP_NAME,
		CPP_NAME,
		BRANCH_THRESHOLD,
		SHOW_VERSION,

		IGNORE
	};

	class Option
	{
	public:

		Option( char s, const char* l, Action ac, bool a );
		Option( const Option& o );

		char shortName() const;
		const std::string& longName() const;
		Action action() const;
		bool argument() const;

	private:

		char m_shortName;
		std::string m_longName;
		Action m_action;
		bool m_argument;
	};

	bool m_emitSource;
	bool m_reduceCode;
	bool m_dumbCode;
	bool m_generateE;
	bool m_alignCode;
	bool m_runGasp;
	bool m_runCpp;
	bool m_deleteTemp;
	bool m_unrollLoops;
	bool m_generateMpg;
	bool m_newSyntax;
	bool m_showUsage;

	unsigned int m_threshold;
	unsigned int m_timeout;
	bool m_showVersion;

	std::vector<Option> m_options;

	std::list< std::string > m_includes;

	std::string m_input;
	std::string m_output;

	std::string m_label;
	std::string m_gasp;
	std::string m_cpp;
};

#include "CommandLine.inl"

}

#endif
