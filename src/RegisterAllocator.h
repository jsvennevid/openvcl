#ifndef __OPENVCL_REGISTERALLOCATOR_H__
#define __OPENVCL_REGISTERALLOCATOR_H__

/*
 * RegisterAllocator.h
 *
 * Copyright (C) 2004 Jesper Svennevid, Daniel Collin
 *
 * Licensed under the AFL v2.0. See the file LICENSE included with this
 * distribution for licensing terms.
 *
 */
        
#ifndef __OPENVCL_REGISTER_H__
#include "Register.h"
#endif

#ifndef __OPENVCL_TOKEN_H__
#include "Token.h"
#endif

#ifndef __OPENVCL_BranchState_H__
#include "BranchState.h"
#endif

#include <map>
#include <vector>
#include <sstream>

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace vcl
{

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class RegisterAllocator
{

public:

	RegisterAllocator();
	~RegisterAllocator();

	void setAvailableFloats( unsigned int floats );
	void setAvailableIntegers( unsigned int integers );

	bool process( std::list<Token>& tokens );

	void releaseAlias( Alias* alias );
	Alias* obtainAlias( Alias::Type type );

	const Register* floatRegister( unsigned int regNumber ) const;
	const Register* integerRegister( unsigned int regNumber ) const;

	const std::string& name();

	void setDynamicThreshold( unsigned int threshold );

protected:

private:

	enum State
	{
		OUTSIDE,
		ENTER,
		CODE,
		EXIT
	};

	void setState( State state );
	State state() const;

	bool collectLabels( std::list<Token>::iterator start, std::list<Token>::iterator end );
	bool processBranchState( BranchState* state, std::list<Token>::iterator end );

	bool processCommonDirective( Token& token );

	bool processAliases();

	bool updateDynamicTracker( const Token* src );

	bool setupLocks( std::map< std::string, unsigned int >& inputs, std::map< unsigned int, std::vector<std::string> >& locks, std::map< std::string, Register*>& regs, std::map<std::string, BranchState::State>& aliases, Register* rarray, unsigned int max );
	bool releaseLocks( std::map< unsigned int, std::vector<std::string> >& locks, std::map< std::string, Register*>& regs, unsigned int line );
	void releaseLocks( std::map< unsigned int, std::vector<std::string> >& locks, std::map< std::string, Register*>& regs );
	const Register* allocateRegister( const std::string& name, std::map<std::string,Register*>& regs, std::map < unsigned int, std::vector<std::string> >& locks, std::map<std::string, BranchState::State>& aliases, Register* rarray, unsigned int max );

	std::map< std::string, std::list<Token>::iterator > m_labels;
	std::list< BranchState* > m_states;

//	void generateReadErrorReport( const Token::Argument& arg, const Token& token );

	State m_currState;
	Register m_floats[32];
	Register m_integers[16];	

	std::string m_name;

	std::map< const Token*, unsigned int > m_dynamicTracker;
	unsigned int m_dynamicThreshold;

	std::map<Alias*,Alias*> m_aliases;
};

#include "RegisterAllocator.inl"

}

#endif
