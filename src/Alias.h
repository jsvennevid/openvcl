#ifndef __OPENVCL_ALIAS_H__
#define __OPENVCL_ALIAS_H__

/*
 * Alias.h
 *
 * Copyright (C) 2004 Jesper Svennevid, Daniel Collin
 *
 * Licensed under the AFL v2.0. See the file LICENSE included with this
 * distribution for licensing terms.
 *
 */

#include "Register.h"

#include <list>

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace vcl
{

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class Alias
{

public:

	enum Type
	{
		FLOAT,
		INTEGER
	};

	struct Range
	{
		unsigned int m_start;
		unsigned int m_stop;
	};

	Alias( Type type );

	void setAllocatedRegister( const Register* allocated );
	const Register* allocatedRegister() const;

	Type type() const;

	void addRange( unsigned int start, unsigned int stop );
	void merge( Alias* alias );
	bool intersects( Alias* alias );

private:	

	Type m_type;

	const Register* m_allocatedRegister;
	std::list<Range> m_ranges;

};

#include "Alias.inl"

}

#endif
