/*
 * Tokenizer.cpp
 *
 * Copyright (C) 2004 Jesper Svennevid, Daniel Collin
 *
 * Licensed under the AFL v2.0. See the file LICENSE included with this
 * distribution for licensing terms.
 *
 */

#include "Tokenizer.h"
#include "Error.h"
#include "stlhelp.h"

#include <iostream>
#include <sstream>

namespace vcl
{

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Tokenizer::Tokenizer()
{
	setVsmMode( false );
	setComment( false );

	m_availableFloats = 0;
	m_availableIntegers = 0;

	m_operands = NULL;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool Tokenizer::parse( const Line& line )
{
	std::string::const_iterator begin = line.content().begin();
	std::string::const_iterator end = line.content().end();

	std::string::const_iterator start;
	std::string::const_iterator stop;
	std::string::const_iterator curr;

	bool oldVsmMode = vsmMode();

	int parentheses = 0;
	char lc = '\0';

	Token token(line);

	assert( m_operands );

	setExecutionPath( vsmMode() ? UPPER : MERGED );
	setContentMode( RESET );

	start = stop = curr = begin;
	while( curr != end )
	{
		if( false == inComment() )
		{
			// deal with comments

			if( ';' == *curr )
				break;

			if( (end != (curr+1)) && ('/' == *curr) && ('*' == *(curr+1)) )
			{
				setComment( true );
				curr+=2;
				continue;
			}

			if( (end != (curr+1)) && ('/' == *curr) && ('/' == *(curr+1)) )
				break;

			// process character

			switch( contentMode() )
			{
				case RESET:
				{
					if( !isBlank(*curr) )
					{
						token.reset();
						setContentMode( OPERAND );
						start = stop = curr;
					}
					else curr++;
				}
				break;

				case OPERAND:
				{
					if( (start == stop) && isBlank(*start) )
						start = stop = curr;

					if( (start == stop) && (isBlank(*curr) || (':' == *curr)) )
					{
						stop = curr;

						if( (start == stop) && (':' == *curr) )
						{
							Error::Display( Error( "Invalid label", token ) );
							return false;
						}
					}

					if( (start != stop) )
					{
						if( ':' == *stop )
						{
							if( token.label().length() )
							{
								Error::Display( Error( "Label already present", token ) );
								return false;
							}

							token.setLabel( line.content().substr( start-begin, stop-start ) );

							curr++;
							start = stop = stop+1;
						}
						else
						{
							token.setName( line.content().substr( start-begin, stop-start ) );

							token.setOperand( identifyToken( token ) );
							if( !token.operand() )
								return false;

							if( token.operand()->arguments() > 0 )
								setContentMode( ARGUMENT );
							else
							{
								if( handleToken( token ) )
									m_tokens.push_back(token);

								token.reset();

								if( oldVsmMode )
								{

									setContentMode( RESET );
									setExecutionPath( LOWER );
								}
								else
									setContentMode( FINISH );
							}
							start = stop = curr;
						}
						continue;
					}

					curr++;
				}
				break;

				case ARGUMENT:
				{
					if( (start == stop) && isBlank(*start) )
						start = stop = curr;

					if( (start == stop) || (parentheses > 0) )
					{
						if( ((isBlank(*curr) || (',' == *curr))) && !parentheses )
							stop = curr;
						else
						{
							if( '(' == *curr )
								parentheses++;
							else if( ')' == *curr )
							{
								if( --parentheses < 0 )
								{
									Error::Display( Error( "Unbalanced parentheses", token ) );
									return false;
								}
							}
						}
					}

					if( (start != stop) && !parentheses )
					{
						if( ',' == *curr )
						{
							token.addArgument( line.content().substr( start-begin, stop-start ) );

							curr++;
							start = stop = curr;
							continue;
						}

						if( !isBlank(*curr) )
						{
							if( !parentheses && !(('+' == lc)||('-' == lc)||('*' == lc)||('/' == lc))
									&& !(('(' == *curr)&&(')' == lc)) )

							token.addArgument( line.content().substr( start-begin, stop-start ) );

							if( vsmMode() )
							{
								if( UPPER != executionPath() )
								{
									Error::Display( Error( "Too many instructions", token ) );
									return false;
								}

								if( handleToken( token ) )
									m_tokens.push_back( token );

								setContentMode( RESET );
								setExecutionPath( LOWER );
							}
							else
							{
								Error::Display( Error( "VSM mode not enabled" ) );
								return false;
							}
						}
						else
							stop = start;
					}

					curr++;
				}
				break;

				case FINISH:
				{
					if( !isBlank(*curr) )
					{
						Error::Display( Error( "Invalid characters", token ) );
						return false;
					}

					curr++;
				}
				break;
			}

			if( !isBlank(*curr) )
				lc = *curr;
		}
		else
		{
			if( ';' == *curr )
				break;

			if( (end != (curr+1)) && ('*' == *curr) && ('/' == *(curr+1)) )
			{
				setComment( false );
				curr++;
			}

			curr++;
		}
	}

	switch( contentMode() )
	{
		case OPERAND:
		{
			if( (start != curr) && !isEmpty( line.content().substr( start-begin, curr-start ) ) )
			{
				token.setName( line.content().substr( start-begin, curr-start ) );

				token.setOperand( identifyToken( token ) );
				if( !token.operand() )
					return false;

				if( (token.operand()->arguments() > 0) && !(token.operand()->flags() & Operand::MULTI) )
				{
					Error::Display( Error( "Invalid number of arguments", token ) );
					return false;
				}
			}
 		}
		break;

		case ARGUMENT:
		{
			bool valid = false;

			while( isBlank(*(curr-1)) && curr > start )
				curr--;

			for( std::string::const_iterator i = start; i!= curr; i++ )
			{
				if( !isBlank(*i) )
				{
					valid = true;
					break;
				}
			}

			if( (start == stop) && (*(start-1) == ',') )
			{
				if( !valid && !(token.operand()->flags() & Operand::MULTI) )
				{
					Error::Display( Error( "Missing argument", token ) );
					return false;
				}
			}

			if( valid )
				token.addArgument( line.content().substr( start-begin, curr-start ) );
		}
		break;

		default: break;
	}

	if( handleToken( token ) )
		m_tokens.push_back( token );
	else
	{
		if( (oldVsmMode && vsmMode()) && (LOWER != executionPath()) )
		{
			Error::Display( Error( "Too few instructions", token ) );
			return false;
		}
	}

	if( parentheses )
	{
		Error::Display( Error( "Unbalanced parentheses", token ) );
		return false;
	}

	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool Tokenizer::handleToken( Token& token )
{

	// drop empty tokens & store labels

	if( !token.label().length() && !token.name().length() )
		return false;

	if( token.label().length() && !token.name().length() )
		return true;

	assert( token.operand() );

	// fail on errors

	if( (token.arguments().size() != token.operand()->arguments()) && !(token.operand()->flags() & Operand::MULTI) )
	{
		Error::Display( Error( "Wrong number of arguments", token ) );
		return false;
	}

	if( token.operand()->flags()&Operand::PREPROCESSOR && (LOWER == executionPath()) )
	{
		Error::Display( Error( "Cannot mix preprocessor commands and instructions", token ) );
		return false;
	}

	if( !token.process( newSyntax() ) )
		return false;

	// check fields

	if( ((Token::X|Token::Y|Token::Z) != token.fields()) && ((token.operand()->flags() & Operand::XYZ) == Operand::XYZ))
	{
		Error::Display( Error( "Invalid destination field for operand", token ) );
		return false;
	}

	// raw VSM mode

	if( vsmMode() )
		token.setFlags( token.flags() | Token::PREORDERED );

	if( (token.operand()->name() == ".vsm") || (token.operand()->name() == ".raw") )
	{
		if( vsmMode() )
		{
			Error::Display( Error( "VSM mode already active", token ) );
			return false;
		}

		setVsmMode( true );

		return false;
	}
	else if( (token.operand()->name() == ".endvsm") || (token.operand()->name() == ".endvsm") )
	{
		if( !vsmMode() )
		{
			Error::Display( Error( "Not in VSM mode", token ) );
			return false;
		}

		setVsmMode( false );

		return false;
	}

	// .syntax new/old

	if( (token.operand()->name() == ".syntax") )
	{
		if( !casecompare( "new", (*(token.arguments().begin())).immediate() ) )
			setNewSyntax( true );
		else
			setNewSyntax( false );

		return false;
	}

	// .init_vf_all

	if( (token.operand()->name() == ".init_vf_all") )
	{
		if( availableFloats() && (availableFloats() != 0xffffffff) )
		{
			Error::Display( Error( "Cannot mix .init_vf_all and .init_vf", token ) );
			return false;
		}

		setAvailableFloats ( 0xffffffff );
		token.setFlags( token.flags() | Token::IGNORED );
	}

	// .init_vf

	if( (token.operand()->name() == ".init_vf") )
	{
		if( availableFloats() == 0xffffffff )
		{
			Error::Display( Error( "Cannot mix .init_vf_all and .init_vf", token ) );
			return false;
		}

		for( std::list<Token::Argument>::iterator i = token.arguments().begin(); i != token.arguments().end(); i++ )
		{
			if( !(*i).regNumber() )
			{
				Error::Display( Error( "Cannot insert constant register VF00 into register allocation", token ) );
				return false;
			}

			setAvailableFloats ( availableFloats() | (1<<(*i).regNumber()) );
		}

		token.setFlags( token.flags() | Token::IGNORED );
	}

	// .init_vi_all

	if( (token.operand()->name() == ".init_vi_all") )
	{
		if( availableIntegers() && (availableIntegers() != 0xffff) )
		{
			Error::Display( Error( "Cannot mix .init_vi_all and .init_vi on line ", token ) );
			return false;
		}

		setAvailableIntegers ( 0xffff );
		token.setFlags( token.flags() | Token::IGNORED );
	}

	// .init_vi

	if( (token.operand()->name() == ".init_vi") )
	{
		if( availableIntegers() == 0xffff )
		{
			Error::Display( Error( "Cannot mix .init_vi_all and .init_vi", token ) );
			return false;
		}

		for( std::list<Token::Argument>::iterator i = token.arguments().begin(); i != token.arguments().end(); i++ )
		{
			if( !(*i).regNumber() )
			{
				Error::Display( Error( "Cannot insert constant register VI00 into register allocation", token ) );
				return false;
			}

			setAvailableIntegers ( availableIntegers() | (1<<(*i).regNumber()) );
		}

		token.setFlags( token.flags() | Token::IGNORED );
	}

	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

const Operand* Tokenizer::identifyToken( Token& token )
{
	assert( m_operands );

	// make the comparison without the modifier

	std::string name;
	std::string dest;
	static char fields[4+1] = "xyzw";
	static unsigned int flags[4] = { Token::X, Token::Y, Token::Z, Token::W };

	std::string::size_type pos = token.name().find('.');
	if( (pos != std::string::npos) && (pos > 0) )
	{
		name = token.name().substr( 0, pos );
		dest = token.name().substr( pos+1 );		

		// parse destination field

		unsigned int dflags = 0;
		std::string::const_iterator k = dest.begin();
		for( unsigned int j = 0; (j < 4) && (dest.end() != k); j++ )
		{
			if( fields[j] == *k )
			{
				dflags |= flags[j];
				k++;
			}
		}

		if( dest.end() != k )
		{
			Error::Display( Error( "Invalid destination-field", token ) );
			return NULL;
		}

		token.setFields( dflags );
	}
	else
		name = token.name();

	if( name.rfind(']') == (name.length()-1) )
	{
		std::string::size_type pos = name.find('[');
		if( pos != std::string::npos )
		{
			for( std::string::size_type i = pos+1; i < name.length()-1; ++i )
			{
				char c = name[i];

				if( c >= 'a' && c <= 'z' )
					c = c - ('a'-'A');

				switch( c )
				{
					case 'E': token.setFlags( token.flags() | Token::E ); break;
					case 'D': token.setFlags( token.flags() | Token::D ); break;
					case 'T': token.setFlags( token.flags() | Token::T ); break;
					default:
					{
						Error::Display( Error( "Invalid bit-flag", token ) );
						return NULL;
					}
					break;
				}
			}
			name = name.substr( 0, pos );
		}
	}

	for( std::list<Operand>::const_iterator i = m_operands->begin(); i != m_operands->end(); i++ )
	{
		const Operand& operand = *i;

		if( (operand.flags() & Operand::BROADCAST) == Operand::BROADCAST )
		{
			// broadcast instruction

			if( operand.name().length() != (name.length()-1) )
				continue;

			for( unsigned int j = 0; j < 4; j++ )
			{
				std::string newname = operand.name() + fields[j];

				if( !casecompare( newname, name ) )
				{
					token.setBroadcast( flags[j] );
					return &operand;
				}
			}


		}
		else
		{

			if( !casecompare( operand.name(), name ) )
				return &operand;
		}
	}

	Error::Display( Error( "Unknown operand '" + token.name() + "'", token ) );
	return NULL;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool Tokenizer::isEmpty( const std::string& s )
{
	for( std::string::const_iterator i = s.begin(); i != s.end(); ++i )
	{
		if( !isBlank(*i) )
			return false;
	}

	return true;
}

}
