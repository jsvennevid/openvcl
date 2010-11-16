/*
 * main.cpp
 *
 * Copyright (C) 2004 Jesper Svennevid, Daniel Collin
 *
 * Licensed under the AFL v2.0. See the file LICENSE included with this
 * distribution for licensing terms.
 *
 */

#include "Parser.h"
#include <stdlib.h>

int main( int argc, char* argv[] )
{
	vcl::Parser parser;

	if( !parser.create( argc, argv ) )
		return EXIT_FAILURE;

	if( !parser.begin() )
		return EXIT_FAILURE;

	while( parser.run() );

	if( !parser.end() )
		return EXIT_FAILURE;

	return EXIT_SUCCESS;
}
