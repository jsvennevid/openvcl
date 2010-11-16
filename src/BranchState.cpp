/*
 * BranchState.cpp
 *
 * Copyright (C) 2004 Jesper Svennevid, Daniel Collin
 *
 * Licensed under the AFL v2.0. See the file LICENSE included with this
 * distribution for licensing terms.
 *
 */

#include "BranchState.h"
#include "RegisterAllocator.h"

#include <assert.h>

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace vcl
{

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void BranchState::writeFloat( unsigned int regNumber, unsigned int fields )
{
	assert( regNumber < 32 );
	m_floatRegisters[regNumber].setFields( m_floatRegisters[regNumber].fields()|fields );
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void BranchState::writeFloat( Token::Argument& argument )
{
	assert( argument.type() == Token::Argument::FLOAT_REGISTER );

	if( argument.content() == Token::Argument::REGISTER )
	{
		writeFloat( argument.regNumber(), argument.fields() );
	}
	else if( argument.content() == Token::Argument::ALIAS )
	{
		std::map< std::string, State >::iterator i = m_floats.find( argument.alias() );

		if( i == m_floats.end() )
		{
			State newState;

			m_floats[ argument.alias() ] = newState;
			i = m_floats.find( argument.alias() ); // this could be retrieved directly, but I've had issues with inserting into maps and retrieving the iterator in the same operation

			assert( i != m_floats.end() );
		}

		updateDependency( argument, i->second, Alias::FLOAT, i->first, ((i->second.fields() & ~argument.fields()) != 0) );

		i->second.setFields( i->second.fields() | argument.fields() );
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool BranchState::readFloat( unsigned int regNumber, unsigned int fields ) const
{
	assert( regNumber < 32 );

	if( fields & (~(m_floatRegisters[regNumber].fields())) )
		return false;

	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool BranchState::readFloat( Token::Argument& argument )
{
	assert( argument.type() == Token::Argument::FLOAT_REGISTER );

	if( argument.content() == Token::Argument::REGISTER )
	{
		return readFloat( argument.regNumber(), argument.fields() );
	}
	else if( argument.content() == Token::Argument::ALIAS )
	{
		std::map< std::string, State >::iterator i = m_floats.find( argument.alias() );

		// has this alias been written already?

		if( i == m_floats.end() )
			return false;

		// make sure no uninitialized portion of the alias is read

		if( argument.fields() & (~(i->second.fields())) )
			return false;

		// update dependency information

		updateDependency( argument, i->second, Alias::FLOAT, i->first, true );
	}
	else return false;

	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void BranchState::writeInteger( unsigned int regNumber )
{
	assert( regNumber < 16 );
	m_integerRegisters[regNumber].setFields( 1 );
	m_integerRegisters[regNumber].clearAddress();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void BranchState::writeInteger( Token::Argument& argument )
{
	assert( argument.type() == Token::Argument::INTEGER_REGISTER );

	if( argument.content() == Token::Argument::REGISTER )
	{
		writeInteger( argument.regNumber() );
	}
	else if( argument.content() == Token::Argument::ALIAS )
	{
		std::map< std::string, State >::iterator i = m_integers.find( argument.alias() );

		if( i == m_integers.end() )
		{
			State newState;

			m_integers[ argument.alias() ] = newState;
			i = m_integers.find( argument.alias() ); // this could be retrieved directly, but I've had issues with inserting into maps and retrieving the iterator in the same operation

			assert( i != m_integers.end() );
		}

		i->second.setFields( 1 );
		i->second.clearAddress();

		updateDependency( argument, i->second, Alias::INTEGER, i->first, false );
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool BranchState::readInteger( unsigned int regNumber )
{
	assert( regNumber < 16 );

	if( !m_integerRegisters[regNumber].fields() )
		return false;

	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool BranchState::readInteger( Token::Argument& argument )
{
	assert( argument.type() == Token::Argument::INTEGER_REGISTER );

	if( argument.content() == Token::Argument::REGISTER )
	{
		return readInteger( argument.regNumber() );
	}
	else if( argument.content() == Token::Argument::ALIAS )
	{
		std::map< std::string, State >::iterator i = m_integers.find( argument.alias() );

		// has this alias been written already?

		if( i == m_integers.end() )
			return false;

		// make sure no uninitialized portion of the alias is read

		if( !i->second.fields() )
			return false;

		// update dependency information

		updateDependency( argument, i->second, Alias::INTEGER, i->first, true );
	}
	else return false;

	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void BranchState::updateDependency( Token::Argument& argument, State& state, Alias::Type type, const std::string& name, bool depend )
{
	Dependency* newDependency;
	if( !argument.dependency() )
	{
		Alias* newAlias = state.dependency() && depend ? state.dependency()->alias() : NULL;
		if( !newAlias )
			newAlias = m_allocator.obtainAlias( type );

		newDependency = new Dependency;
		newDependency->setAlias( newAlias );
		newDependency->setToken( *current() );
		newDependency->setState( this );			

		argument.setDependency( newDependency );
	}

	newDependency = argument.dependency();

	if( state.flags() & State::INPUT )
	{
		if( depend )
		{
			const Register* reg = inputRegister( type, name );
			assert( reg );
			newDependency->alias()->setAllocatedRegister( reg );
		}
		state.setFlags( state.flags() & ~State::INPUT );
	}

	if( !depend )
	{
		newDependency->alias()->addRange( newDependency->token().line().number(), newDependency->token().line().number() );
		state.clearTrace();
		state.setDependency( newDependency );
		return;
	}

	if( state.traces().size() > 0 )
	{
		Token* entry = &(newDependency->token());
		for( std::vector<State::Trace>::const_iterator k = state.traces().begin(); k != state.traces().end(); ++k )
		{
			if( (*k).isEntryPoint() )
				entry = &*(*k).location();
			else
				newDependency->alias()->addRange( entry->line().number(), (*(*k).location()).line().number() );
		}
		newDependency->alias()->addRange( entry->line().number(), (*current()).line().number() );

		if( state.dependency() )
			newDependency->depend( state.dependency() );

		state.clearTrace();
	}
	else
	{
		if( state.dependency() )
		{
			newDependency->depend( state.dependency() );
			newDependency->alias()->addRange( state.dependency()->token().line().number(), newDependency->token().line().number() );
		}
		else
			newDependency->alias()->addRange( newDependency->token().line().number(), newDependency->token().line().number() );
	}

	state.setDependency( newDependency );
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void BranchState::storeBranch( const Token* target )
{
	std::map< const Token*, std::vector<const Token*> >::iterator i = m_branches.find( &*m_current );
	if( i != m_branches.end() )
	{
		i->second.push_back( target );
	}
	else
	{
		std::vector<const Token*> targets;
		targets.push_back( target );

		m_branches[ &*m_current ] = targets;
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool BranchState::isBranchTaken( const Token* target ) const
{
	std::map< const Token*, std::vector<const Token*> >::const_iterator i = m_branches.find( &*m_current );
	if( i != m_branches.end() )
	{
		for( std::vector<const Token*>::const_iterator j = i->second.begin(); j != i->second.end(); ++j )
		{
			if( *j == target )
				return true;
		}
	}

	return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void BranchState::pushTraces( Token* location, bool entryPoint )
{
	std::map< std::string, State >::iterator i;

	for( i = m_floats.begin(); i != m_floats.end(); ++i )
		i->second.pushTrace( location, entryPoint );

	for( i = m_integers.begin(); i != m_integers.end(); ++i )
		i->second.pushTrace( location, entryPoint );
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void BranchState::setFloatInput( const std::string& name, unsigned int regNumber )
{
	State newState;
	newState.setFields( 0x0f );
	newState.setFlags( State::INPUT );

	m_floats[ name ] = newState;
	m_floatInputs[ name ] = regNumber;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void BranchState::setIntegerInput( const std::string& name, unsigned int regNumber )
{
	State newState;
	newState.setFields( 0x01 );
	newState.setFlags( State::INPUT );

	m_integers[ name ] = newState;
	m_integerInputs[ name ] = regNumber;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void BranchState::setFloatOutput( const std::string& name, unsigned int regNumber )
{
	m_floatOutputs[ name ] = regNumber;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void BranchState::setIntegerOutput( const std::string& name, unsigned int regNumber )
{
	m_integerOutputs[ name ] = regNumber;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void BranchState::storeAddress( const Token::Argument& argument )
{
	assert( argument.type() == Token::Argument::INTEGER_REGISTER );

	std::list<Token>::iterator result = current();
	result++;

	if( argument.content() == Token::Argument::REGISTER )
	{
		assert( argument.regNumber() < 16 );
		m_integerRegisters[ argument.regNumber() ].setAddress( result );
	}
	else if( argument.content() == Token::Argument::ALIAS )
	{
		std::map<std::string,State>::iterator i = m_integers.find( argument.alias() );
		if( m_integers.end() == i )
			return;

		i->second.setAddress( result );
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool BranchState::address( const Token::Argument& argument, std::list<Token>::iterator& target, const std::map< std::string, std::list<Token>::iterator >& labels )
{
	if( argument.type() == Token::Argument::INTEGER_REGISTER )
	{
		if( argument.content() == Token::Argument::REGISTER )
		{
			if( m_integerRegisters[ argument.regNumber() ].isAddress() )
			{
				target = m_integerRegisters[ argument.regNumber() ].address();
				return true;
			}
		}
		else if( argument.content() == Token::Argument::ALIAS )
		{
			std::map<std::string,State>::iterator i = m_integers.find( argument.alias() );
			if( m_integers.end() == i )
				return false;

			if( i->second.isAddress() )
			{
				target = i->second.address();
				return true;
			}
		}
	}
	else if( argument.type() == Token::Argument::IMMEDIATE )
	{
		std::map< std::string, std::list<Token>::iterator >::const_iterator i = labels.find( argument.immediate() );
		if( labels.end() == i )
			return false;

		target = i->second;
		return true;
	}

	return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

const Register* BranchState::inputRegister( Alias::Type type, const std::string& name )
{
	if( Alias::FLOAT == type )
	{
		std::map<std::string,unsigned int>::iterator i = m_floatInputs.find( name );
		if( m_floatInputs.end() == i )
			return NULL;

		return allocator().floatRegister( i->second );
	}
	else if( Alias::INTEGER == type )
	{
		std::map<std::string,unsigned int>::iterator i = m_integerInputs.find( name );
		if( m_integerInputs.end() == i )
			return NULL;

		return allocator().integerRegister( i->second );
	}
	else return NULL;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool BranchState::applyRegisterOutputs()
{
	std::map< std::string, State >::iterator i;

	// float outputs

	for( i = m_floats.begin(); i != m_floats.end(); ++i )
	{
		if( i->second.dependency() )
		{
			std::map< std::string, unsigned int >::iterator j = m_floatOutputs.find( i->first );
			if( j != m_floatOutputs.end() )
			{
				Alias* alias = i->second.dependency()->alias();
				assert( alias );

				const Register* floatRegister = allocator().floatRegister( j->second );
				assert( floatRegister );

				if( alias->allocatedRegister() && (alias->allocatedRegister() != floatRegister) )
					return false;

				alias->setAllocatedRegister( floatRegister );
			}
		}
	}

	// integer outputs

	for( i = m_integers.begin(); i != m_integers.end(); ++i )
	{
		if( i->second.dependency() )
		{
			std::map< std::string, unsigned int >::iterator j = m_integerOutputs.find( i->first );
			if( j != m_integerOutputs.end() )
			{
				Alias* alias = i->second.dependency()->alias();
				assert( alias );

				const Register* integerRegister = allocator().integerRegister( j->second );
				assert( integerRegister );

				if( alias->allocatedRegister() && (alias->allocatedRegister() != integerRegister) )
					return false;

				alias->setAllocatedRegister( integerRegister );
			}
		}
	}

	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

}
