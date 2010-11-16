/*
 * Dependency.cpp
 *
 * Copyright (C) 2004 Jesper Svennevid, Daniel Collin
 *
 * Licensed under the AFL v2.0. See the file LICENSE included with this
 * distribution for licensing terms.
 *
 */


#include "Dependency.h"
#include "BranchState.h"
#include "RegisterAllocator.h"

#include <assert.h>

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace vcl
{

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Dependency::depend( Dependency* dependency )
{
	assert( dependency );

	if( dependency->alias() != alias() )
	{
		Alias* oldAlias = dependency->alias();

		// merge dependency graphs
		alias()->merge( oldAlias );
		dependency->propagate( alias() );

		m_state->allocator().releaseAlias( oldAlias );
	}

	dependency->m_output[ this ] = this;
	m_input[ dependency ] = dependency;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Dependency::propagate( Alias* newAlias )
{
	if( alias() == newAlias )
		return;

	setAlias( newAlias );

	std::map<Dependency*,Dependency*>::iterator i;

	for( i = m_input.begin(); i != m_input.end(); ++i )
		i->first->propagate( newAlias );

	for( i = m_output.begin(); i != m_output.end(); ++i )
		i->first->propagate( newAlias );
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

}
