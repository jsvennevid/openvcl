/*
 * RegisterAllocator.cpp
 *
 * Copyright (C) 2004 Jesper Svennevid, Daniel Collin
 *
 * Licensed under the AFL v2.0. See the file LICENSE included with this
 * distribution for licensing terms.
 *
 */


#include "RegisterAllocator.h"
#include "BranchState.h"
#include "Error.h"

#include <iostream>
#include <iomanip>
#include <stdlib.h>
#include <assert.h>

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace vcl
{

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

RegisterAllocator::RegisterAllocator()
{
	unsigned int i;

	for( i = 0; i < sizeof(m_floats)/sizeof(Register); i++ )
	{
		std::stringstream s;
		s << "VF" << std::setw(2) << std::setfill('0') << i;
		m_floats[i].setName( s.str() );
	}

	for( i = 0; i < sizeof(m_integers)/sizeof(Register); i++ )
	{
		std::stringstream s;
		s << "VI" << std::setw(2) << std::setfill('0') << i;
		m_integers[i].setName( s.str() );
	}

	m_dynamicThreshold = 16;
	m_currState = OUTSIDE;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

RegisterAllocator::~RegisterAllocator()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void RegisterAllocator::setAvailableFloats( unsigned int floats )
{
	for( unsigned int i = 1; i < 32; i++ )
		m_floats[i].setAvailable( ( floats & (1<<i) ) != 0 );
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void RegisterAllocator::setAvailableIntegers( unsigned int integers  )
{
	for( unsigned int i = 1; i < 16; i++ )
		m_integers[i].setAvailable( ( integers & (1<<i) ) != 0 );
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool RegisterAllocator::process( std::list<Token>& tokens )
{
	BranchState* branchState = NULL;

	if( !collectLabels( tokens.begin(), tokens.end() ) )
		return false;

	for( std::list<Token>::iterator i = tokens.begin(), next = tokens.begin(); i != tokens.end(); i = next )
	{
		next++;

		switch( state() )
		{
			case OUTSIDE:
			{
				if( !(*i).operand() )
					continue;

				if( !((*i).operand()->flags() & Operand::PREPROCESSOR) )
				{
					Error::Display( Error( "Code not allowed outside entry point", *i ) );
					return false;
				}

				if( "--enter" == (*i).operand()->name() )
				{
					next = i;

					branchState = new BranchState( *this );
					assert( branchState );

					setState( ENTER );
				}
				if( ("--exit" == (*i).operand()->name()) || ("--exitm" == (*i).operand()->name()) )
				{
					next = i;
					setState( EXIT );
				}
				else
				{
				}
			}
			break;

			case ENTER:
			{
				if( !(*i).operand() )
					continue;

				if( (*i).operand()->unit() != Operand::ENTER )
				{
					Error::Display( Error( "Invalid operand inside --enter/--endenter block", *i ) );
					delete branchState;
					return false;
				}

				(*i).setFlags( (*i).flags() | Token::IGNORED );

				if( "in_vf" == (*i).operand()->name() )
				{
					const Token::Argument& arg = *((*i).arguments().begin());
					branchState->setFloatInput( arg.immediate(), arg.regNumber() );
				}
				else if( "in_vi" == (*i).operand()->name() )
				{
					const Token::Argument& arg = *((*i).arguments().begin());
					branchState->setIntegerInput( arg.immediate(), arg.regNumber() );
				}
				else if( "in_hw_acc" == (*i).operand()->name() )
					branchState->writeAccumulator( Token::X|Token::Y|Token::Z|Token::W );
				else if( "in_hw_i" == (*i).operand()->name() )
					branchState->writeI();
				else if( "in_hw_p" == (*i).operand()->name() )
					branchState->writeP();
				else if( "in_hw_q" == (*i).operand()->name() )
					branchState->writeQ();
				else if( "in_hw_r" == (*i).operand()->name() )
					branchState->writeR();
				else if( "--endenter" == (*i).operand()->name() )
				{
					m_states.push_back( branchState );
					setState( CODE );
				}
			}
			break;

			case CODE:
			{
				// locate end of codeblock
				for( ; next != tokens.end(); next++ )
				{
					if( !(*next).operand() )
						continue;

					if( !((*next).operand()->flags() & Operand::PREPROCESSOR) )
						continue;

					if( ((*next).operand()->unit() == Operand::EXIT) || ((*next).operand()->unit() == Operand::ENTER) )
						break;

					if( !processCommonDirective( (*next) ) )
						return false;
				}

				branchState->setCurrent( i );
				branchState->setExitPoint( next );
				branchState->pushTraces( &*i, true );

				setState( EXIT );
			}
			break;

			case EXIT:
			{
				if( !(*i).operand() )
					continue;

				if( (*i).operand()->unit() != Operand::EXIT )
				{
					Error::Display( Error( "Invalid operand inside --exit/--endexit block", *i ) );
					return false;
				}

				(*i).setFlags( (*i).flags() | Token::IGNORED );

				if( "out_vf" == (*i).operand()->name() )
				{
					const Token::Argument& arg = *((*i).arguments().begin());

					if( branchState )
						branchState->setFloatOutput( arg.immediate(), arg.regNumber() );
				}
				else if( "out_vi" == (*i).operand()->name() )
				{
					const Token::Argument& arg = *((*i).arguments().begin());
					if( branchState )
						branchState->setIntegerOutput( arg.immediate(), arg.regNumber() );
				}
				else if( "--endexit" == (*i).operand()->name() )
				{
					setState( OUTSIDE );
				}
			}
			break;
		}

		if( !processCommonDirective( (*i) ) )
			return false;
	}

	std::list<BranchState*>::iterator j;
	for( j = m_states.begin(); j != m_states.end(); j++ )
	{
		if( !processBranchState( *j, tokens.end() ) )
			return false;
	}

	if( m_aliases.size() > 0 )
	{
		if( !processAliases() )
		{
			Error::Display( Error( "Register allocation ran out of registers" ) );
			return false; 
		}
	}

	while( !m_states.empty() )
	{
		delete m_states.back();
		m_states.pop_back();
	}

	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool RegisterAllocator::processBranchState( BranchState* state, std::list<Token>::iterator end )
{
	std::list<Token>::iterator curr, next;
	for( ; state->current() != end; state->setCurrent( curr ) )
	{
		curr = state->current();
		Token& token = *curr;
		curr++;

		if( !token.operand() )
			continue;

		// if we've reached a new entry or exit-point, abort branch
		if( (token.operand()->unit() == Operand::EXIT) || (token.operand()->unit() == Operand::ENTER) )
			break;

		token.setFlags( token.flags() | Token::PROCESSED );

		for( std::list<Token::Argument>::reverse_iterator i = token.arguments().rbegin(); i != token.arguments().rend(); i++ )
		{
			switch( (*i).type() )
			{
				case Token::Argument::FLOAT_REGISTER:
				{
					if( (*i).flags() & Token::Argument::WRITE )
						state->writeFloat( (*i) );
					else
					{
						if( !state->readFloat( (*i) ) )
						{
							Error::Display( Error( "Read-attempt from uninitialized element", token, *i ) );
							return false;
						}
					}
				}
				break;

				case Token::Argument::INTEGER_REGISTER:
				{
					if( (*i).flags() & Token::Argument::WRITE )
						state->writeInteger( (*i) );
					else
					{
						if( !state->readInteger( (*i) ) )
						{
							Error::Display( Error( "Read-attempt from uninitialized element", token, *i ) );
							return false;
						}
					}
				}
				break;

				case Token::Argument::ACCUMULATOR:
				{
					if( (*i).flags() & Token::Argument::WRITE )
						state->writeAccumulator( (*i).fields() );
					else
					{
						if( !state->readAccumulator( (*i).fields() ) )
						{
							Error::Display( Error( "Read-attempt from uninitialized element", token, *i ) );
							return false;
						}
					}
				}
				break;

				case Token::Argument::Q:
				{
					if( (*i).flags() & Token::Argument::WRITE )
						state->writeQ();
					else
					{
						if( !state->readQ() )
						{
							Error::Display( Error( "Read-attempt from uninitialized element", token, *i ) );
							return false;
						}
					}
				}
				break;

				case Token::Argument::P:
				{
					if( (*i).flags() & Token::Argument::WRITE )
						state->writeP();
					else
					{
						if( !state->readP() )
						{
							Error::Display( Error( "Read-attempt from uninitialized element", token, *i ) );
							return false;
						}
					}
				}
				break;

				case Token::Argument::R:
				{
					if( (*i).flags() & Token::Argument::WRITE )
						state->writeR();
					else
					{
						if( !state->readR() )
						{
							Error::Display( Error( "Read-attempt from uninitialized element", token, *i ) );
							return false;
						}
					}
				}
				break;

				case Token::Argument::I:
				{
					if( (*i).flags() & Token::Argument::WRITE )
						state->writeI();
					else
					{
						if( !state->readI() )
						{
							Error::Display( Error( "Read-attempt from uninitialized element", token, *i ) );
							return false;
						}
					}
				}
				break;

				case Token::Argument::IMMEDIATE:
				{
					if( token.operand()->flags() & Operand::IWRITE )
					{
						// special case for LOI
						state->writeI();
					}
				}
				break;

				default: break;
			}
		}

		if( token.operand()->unit() == Operand::BRU )
		{
			// branch instruction

			std::list<Token::Argument>::const_iterator branchDest = token.arguments().end();
			std::list<Token::Argument>::const_iterator branchStore = token.arguments().end();

			std::list<Token::Argument>::const_iterator i;
			for( i = token.arguments().begin(); i != token.arguments().end(); i++ )
			{
				if( (*i).flags() & Token::Argument::BRANCH )
					branchDest = i;

				if( (*i).flags() & Token::Argument::ADDRESS )
					branchStore = i;
			}

			if( token.arguments().end() == branchDest )
			{
				Error::Display( Error( "Invalid branch", token ) );
				return false;
			}

			if( token.arguments().end() != branchStore )
				state->storeAddress( *branchStore );

			std::list<Token>::iterator target;

			if( token.operand()->flags() & Operand::DYNAMIC )
			{
				if( !state->address( *branchDest, target, m_labels ) )
					continue;

				if( state->isBranchTaken( &*target ) )
					break;

				if( !updateDynamicTracker( &*state->current() ) )
					break;

				state->storeBranch( &*target );

				BranchState* newState = new BranchState( *state );
				assert( newState );

				newState->pushTraces( &*curr, false );
				newState->pushTraces( &*target, true );
				newState->setCurrent( target );

				m_states.push_back( newState );
			}
			else
			{
				if( !state->address( *branchDest, target, m_labels ) )
					break;

				if( state->isBranchTaken( &*target ) )
					break;

				state->storeBranch( &*target );

				state->pushTraces( &*curr, false );
				state->pushTraces( &*target, true );

				curr = target;
			}
		}
	}

	if( state->current() == state->exitPoint() )
	{
		if( !state->applyRegisterOutputs() )
		{
			Error::Display( Error( "Output register conflict", *(state->exitPoint()) ) );
			return false;
		}
	}

	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool RegisterAllocator::collectLabels( std::list<Token>::iterator start, std::list<Token>::iterator end )
{
	m_labels.clear();

	for( std::list<Token>::iterator i = start; i != end; i++ )
	{
		if( (*i).label().length() )
		{
			std::map<std::string,std::list<Token>::iterator>::iterator j = m_labels.find( (*i).label() );

			if( j != m_labels.end() )
			{
				Error::Display( Error( "Duplicate label '" + (*i).label() + "'", *i ) );
				return false;
			}

			m_labels[ (*i).label() ] = i;
		}
	}

	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool RegisterAllocator::processAliases()
{
	// allocate registers that do not overlap usage

	for( std::map<Alias*,Alias*>::iterator i = m_aliases.begin(); i != m_aliases.end(); ++i )
	{
		Alias* dest = i->first;

		// preallocated register
		if( dest->allocatedRegister() )
			continue;

		unsigned int maxLimit = dest->type() == Alias::FLOAT ? 32 : 16;
		for( unsigned int j = 0; j < maxLimit; ++j )
		{
			const Register* candidate = (dest->type() == Alias::FLOAT) ? &m_floats[j] : &m_integers[j];

			if( !candidate->available() )
				continue;

			for( std::map<Alias*,Alias*>::iterator k = m_aliases.begin(); k != m_aliases.end(); ++k )
			{
				Alias* src = k->first;

				if( !src->allocatedRegister() )
					continue;

				if( src->type() != dest->type() )
					continue;

				if( src->allocatedRegister() != candidate )
					continue;

				if( !dest->intersects( src ) )
					continue;

				candidate = NULL;
				break;
			}

			if( candidate )
			{
				dest->setAllocatedRegister( candidate );
				break;
			}
		}

		if( !dest->allocatedRegister() )
			return false;
		//assert( dest->allocatedRegister() );
	}

	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Alias* RegisterAllocator::obtainAlias( Alias::Type type )
{
	Alias* alias = new Alias( type );

	m_aliases[ alias ] = alias;

	return alias;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void RegisterAllocator::releaseAlias( Alias* alias )
{
	std::map<Alias*,Alias*>::iterator i = m_aliases.find( alias );
	assert( i != m_aliases.end() );

	m_aliases.erase( i );

	delete alias;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool RegisterAllocator::processCommonDirective( Token& token )
{
	if( !token.operand() )
		return true;

	if( token.operand()->name() == ".name" )
	{
		m_name = token.arguments().begin()->immediate();
		token.setFlags( token.flags() | Token::IGNORED );
		return true;
	}

	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool RegisterAllocator::updateDynamicTracker( const Token* src )
{
	std::map< const Token*, unsigned int >::iterator i = m_dynamicTracker.find( src );

	if( m_dynamicTracker.end() == i )
	{
		m_dynamicTracker[ src ] = 1;
		return true;
	}

	if( i->second > m_dynamicThreshold )
		return false;

	m_dynamicTracker[ src ] = i->second+1;
	return true;
}


}
