#ifndef __OPENVCL_BRANCHSTATE_H__
#define __OPENVCL_BRANCHSTATE_H__

/*
 * BranchState.h
 *
 * Copyright (C) 2004 Jesper Svennevid, Daniel Collin
 *
 * Licensed under the AFL v2.0. See the file LICENSE included with this
 * distribution for licensing terms.
 *
 */
        

#include "Token.h"

#include <map>
#include <string>
#include <vector>

#include <assert.h>

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace vcl
{

class RegisterAllocator;
class Dependency;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class BranchState
{

public:

	class State
	{
	public:

		class Trace
		{
		public:
			Trace( Token* location, bool entryPoint );
			Trace( const Trace& t );

			Token* location() const;
			bool isEntryPoint() const;

		private:

			Token* m_location;
			bool m_entryPoint;

		};

		enum
		{
			INPUT = 0x00000001 // on first usage state should retrieve input register
		};

		State();
		State( const State& a );

		void setFields( unsigned int fields );
		unsigned int fields() const;

		void setDependency( Dependency* dependency );
		Dependency* dependency() const;

		void setFlags( unsigned int flags );
		unsigned int flags() const;

		void setAddress( std::list<Token>::iterator address );
		void clearAddress();

		bool isAddress() const;
		std::list<Token>::iterator address() const;

		void pushTrace( Token* location, bool entryPoint );
		const std::vector<Trace>& traces() const;
		void clearTrace();

	private:
		unsigned int m_fields;
		unsigned int m_flags;

		Dependency* m_dependency;

		bool m_isAddress;
		std::list<Token>::iterator m_address;
		std::vector<Trace> m_traces;
	};

	enum
	{
		// used in both register input and tracking
		Q,
		P,
		R,
		I,

		// used only in register input
		ACC,
		CLIP,
		STATUS
	};

	BranchState( RegisterAllocator& allocator );
	BranchState( const BranchState& a );

	RegisterAllocator& allocator() const;

	void setCurrent( std::list<Token>::iterator current );
	std::list<Token>::iterator current() const;

	void setExitPoint( std::list<Token>::iterator exitPoint );
	std::list<Token>::iterator exitPoint() const;

	void writeFloat( Token::Argument& argument );
	bool readFloat( Token::Argument& argument );

	void writeFloat( unsigned int regNumber, unsigned int fields );
	bool readFloat( unsigned int regNumber, unsigned int fields ) const;

	void writeInteger( Token::Argument& argument );
	bool readInteger( Token::Argument& argument );

	void writeInteger( unsigned int regNumber );
	bool readInteger( unsigned int regNumber );

	void updateDependency( Token::Argument& argument, State& state, Alias::Type type, const std::string& name, bool depend );

	void storeAddress( const Token::Argument& argument );
	bool address( const Token::Argument& argument, std::list<Token>::iterator& target, const std::map< std::string, std::list<Token>::iterator >& labels );

	void writeAccumulator( unsigned int fields );
	bool readAccumulator( unsigned int fields ) const;

	void writeQ();
	bool readQ() const;

	void writeP();
	bool readP() const;

	void writeR();
	bool readR() const;

	void writeI();
	bool readI() const;

	void storeBranch( const Token* target );
	bool isBranchTaken( const Token* target ) const;
	void pushTraces( Token* location, bool entryPoint );

	void setFloatInput( const std::string& name, unsigned int regNumber );
	void setIntegerInput( const std::string& name, unsigned int regNumber );

	void setFloatOutput( const std::string& name, unsigned int regNumber );
	void setIntegerOutput( const std::string& name, unsigned int regNumber );

	bool applyRegisterOutputs();

	const Register* inputRegister( Alias::Type type, const std::string& name );

	BranchState* clone();

private:

	RegisterAllocator& m_allocator;

	std::list<Token>::iterator m_current;
	std::list<Token>::iterator m_exitPoint;

	std::map< const Token*, std::vector<const Token*> > m_branches;

	std::map< std::string, State > m_floats;
	std::map< std::string, State > m_integers;

	unsigned int m_accumulatorFields;
	unsigned int m_registerFields;

	State m_floatRegisters[32];
	State m_integerRegisters[16];

	std::map<std::string,unsigned int> m_floatInputs;
	std::map<std::string,unsigned int> m_integerInputs;

	std::map<std::string,unsigned int> m_floatOutputs;
	std::map<std::string,unsigned int> m_integerOutputs;
};

#include "BranchState.inl"

}

#endif
