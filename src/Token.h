#ifndef __OPENVCL_TOKEN_H__
#define __OPENVCL_TOKEN_H__

/*
 * Token.h
 *
 * Copyright (C) 2004 Jesper Svennevid, Daniel Collin
 *
 * Licensed under the AFL v2.0. See the file LICENSE included with this
 * distribution for licensing terms.
 *
 */

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "Line.h"
#include "Operand.h"
#include "Register.h"
#include "Dependency.h"

#include <list>
#include <string>

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace vcl
{

class Dependency;

class Token
{
public:

	class Argument
	{
	public:

		enum Type
		{
			INVALID_TYPE,
			STRING,
			FLOAT_REGISTER,
			INTEGER_REGISTER,
			IMMEDIATE,
			ACCUMULATOR,
			I,
			Q,
			R,
			P
		};

		enum Content
		{
			INVALID_CONTENT,
			REGISTER,
			ALIAS,
			RANGE
		};

		enum // Flags
		{
			INDIRECT	= 0x00000001,
			POSTINC		= 0x00000002,
			PREDEC		= 0x00000004,
			BROADCAST	= 0x00000008,
			FLAG			= 0x00000010,
			WRITE			= 0x00000020,
			BRANCH		= 0x00000040,
			ADDRESS		= 0x00000080,
			DEST			= 0x00000100,
			EVALUATE	= 0x00000200,
			ROTATE		= 0x00000400
		};

		Argument( const std::string& text );
		Argument( const Argument& a );

		const std::string& text() const;

		void resetResult();

		void setType( Type type );
		Type type() const;
		Content content() const;

		void setRegNumber( int reg );
		int regNumber() const;

		void setAlias( const std::string& alias );
		const std::string& alias() const;

		void setImmediate( const std::string& alias );
		const std::string& immediate() const;

		void setRange( int lower, int upper );
		int lower() const;
		int upper() const;

		void setFlags( unsigned int flags );
		unsigned int flags() const;

		void setFields( unsigned int fields );
		unsigned int fields() const;

		void setDependency( Dependency* dependency );
		Dependency* dependency() const;

	private:

		std::string m_text;

		Type m_type;
		Content m_content;

		int m_regNumber;
		std::string m_alias;
		std::string m_immediate;

		unsigned int m_flags;
		unsigned int m_fields;

		Dependency* m_dependency;
	};

	enum
	{
		PREORDERED	= 0x00000001,	// should not be rescheduled
		E						= 0x00000002,	// [E]
		D						= 0x00000004, // [D]
		T						= 0x00000008,	// [T]
		IGNORED			= 0x00000010,	// ignored by the code-generator
		PROCESSED		= 0x00000020	// has been processed by the register allocator
	};

	enum Modifier
	{
		DEST,
		POSTINC,
		PREDEC,
		FLAG,
		BROADCAST,
		NOALIAS,
		WRITE,
		BRANCH,
		ADDRESS,
		ZERO,
		EVALUATE,
		ROTATE,
		RAW
	};

	enum
	{
		X = 0x00000001,
		Y = 0x00000002,
		Z = 0x00000004,
		W = 0x00000008
	};

	Token( const Line& line );
	Token( const Token& o );

	void reset();
	bool process( bool newSyntax );

	const Line& line() const;

	void setLabel( const std::string& label );
	const std::string& label() const;

	void setName( const std::string& opName );
	const std::string& name() const;
	
	void addArgument( const std::string& arg );
	std::list<Argument>& arguments();
	const std::list<Argument>& arguments() const;

	void setFlags( unsigned int newFlags );
	unsigned int flags() const;

	void setOperand( const Operand* o );
	const Operand* operand() const;

	void setBroadcast( unsigned int flag );
	unsigned int broadcast() const;

	void setFields( unsigned int fields );
	unsigned int fields() const;

	void operator=(const Token& token);

private:

	bool processOperand( const Operand* operand, bool newSyntax, Token::Argument*& error );
	void printInformation( const Operand* operand, std::ostream& stream );

	bool extractRegister( std::string name, Argument& argument, unsigned int modifiers, bool indirect, unsigned int& fields, bool newSyntax );

	static Argument::Type classifyArgument( const std::string& name );
	static bool compareParentheses( const std::string& argument, int parentheses );

	unsigned int extractModifiers( const std::string& list );
	static bool hasModifier( Modifier modifier, unsigned int modifiers );
	static bool extractFields( const std::string& fields, unsigned int& flags );
	static unsigned int countFields( unsigned int fields );
	static bool validateAlias( const std::string& alias );

	const Line& m_line;

	std::string m_label;
	std::string m_name;
	std::list<Argument> m_arguments;
	unsigned int m_flags;

	unsigned int m_broadcast;
	unsigned int m_fields;

	const Operand* m_operand;
};

#include "Token.inl"

}

#endif
