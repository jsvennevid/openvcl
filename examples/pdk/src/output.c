/*
 * gs.h
 *
 * Copyright (C) 2004 Jesper Svennevid, Daniel Collin
 *
 * Licensed under the AFL v2.0. See the file LICENSE included with this
 * distribution for licensing terms.
 *
 */

#include "output.h"
#include <kernel.h>
#include <stdio.h>

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void pdkOut( const char* pString, ... )
{
	static char buffer1[1024];
	va_list v;

	va_start(v,pString);
	vsprintf(buffer1,pString,v);
	va_end(v);

	printf( buffer1 );
}

