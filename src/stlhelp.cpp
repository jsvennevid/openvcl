/*
 * stlhelp.cpp
 *
 * Copyright (C) 2004 Jesper Svennevid, Daniel Collin
 *
 * Licensed under the AFL v2.0. See the file LICENSE included with this
 * distribution for licensing terms.
 *
 */

#include "stlhelp.h"

namespace vcl
{

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

std::string trim( const std::string& s )
{
	std::string::size_type start = s.length();
	std::string::size_type stop = 0;

	for( std::string::const_iterator i = s.begin(); i != s.end(); ++i )
	{
		if( !isBlank(*i) )
		{
			std::string::size_type ofs = i-s.begin();

			if( stop <= ofs )
				stop = ofs+1;
			if( start > ofs )
				start = ofs;
		}
	}	

	return s.substr( start, stop-start );
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void makelower(std::string& s)
{
	for(std::string::size_type i = 0; i < s.length(); ++i)
	{
		char c = s[i];

		if( c >= 'A' && c <= 'Z' )
			c = c - ('A'-'a');

		s[i] = c;
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int casecompare( const std::string& s1, const std::string& s2 )
{
	std::string::size_type l1 = s1.length();
	std::string::size_type l2 = s2.length();

	if( l1 > l2 ) return 1;
	if( l1 < l2 ) return -1;

	for( std::string::size_type i = 0; i < l1; i++ )
	{
		char c1 = s1[i];
		char c2 = s2[i];

		if( c1 == c2 )
			continue;

		if( c1 >= 'a' && c1 <= 'z' )
			c1 = c1 - ('a'-'A');

		if( c2 >= 'a' && c2 <= 'z' )
			c2 = c2 - ('a'-'A');

		if( c1 != c2 )
			return 1;
 	}

	return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int ncasecompare( std::string::size_type pos, std::string::size_type length, const std::string& s1, const std::string& s2 )
{
	std::string::size_type l1 = s1.length();
	std::string::size_type l2 = s2.length();

	std::string::size_type start = pos;
	std::string::size_type stop = std::min(pos+length,l1);

	if( l2 < stop ) return -1;

	for( std::string::size_type i = start; i < stop; i++ )
	{
		char c1 = s1[i];
		char c2 = s2[i];

		if( c1 == c2 )
			continue;

		if( c1 >= 'a' && c1 <= 'z' )
			c1 = c1 - ('a'-'A');

		if( c2 >= 'a' && c2 <= 'z' )
			c2 = c2 - ('a'-'A');

		if( c1 != c2 )
			return 1;
 	}

	return 0;
}

}
