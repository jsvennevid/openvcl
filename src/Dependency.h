#ifndef __OPENVCL_DEPENDENCY_H__
#define __OPENVCL_DEPENDENCY_H__

/*
 * Dependency.h
 *
 * Copyright (C) 2004 Jesper Svennevid, Daniel Collin
 *
 * Licensed under the AFL v2.0. See the file LICENSE included with this
 * distribution for licensing terms.
 *
 */

#include "Alias.h"

#include <map>

#include <assert.h>

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace vcl
{

class BranchState;
class Token;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class Dependency
{
public:

	Dependency();

	void depend( Dependency* input );
	void propagate( Alias* newAlias );

	Alias* alias() const;
	void setAlias( Alias* alias );

	void setToken( Token& token );
	Token& token() const;

	void setState( BranchState* state );
	BranchState* state() const;

	std::map<Dependency*,Dependency*>& input();
	std::map<Dependency*,Dependency*>& output();

private:

	Alias*				m_alias;
	BranchState*	m_state;
	Token*				m_token;

	std::map<Dependency*,Dependency*> m_input;
	std::map<Dependency*,Dependency*> m_output;
};

#include "Dependency.inl"

}

#endif
