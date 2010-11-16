#ifndef __OPENVCL_STLHELP_H__
#define __OPENVCL_STLHELP_H__

/*
 * stlhelp.h
 *
 * Copyright (C) 2004 Jesper Svennevid, Daniel Collin
 *
 * Licensed under the AFL v2.0. See the file LICENSE included with this
 * distribution for licensing terms.
 *
 */

#include <string>

namespace vcl
{

inline bool isBlank(char c) { return c == ' ' || c == '\t'; }  
std::string trim( const std::string& s );
void makelower(std::string& s);
int casecompare( const std::string& s1, const std::string& s2 );
int ncasecompare( std::string::size_type pos, std::string::size_type length, const std::string& s1, const std::string& s2 );


}

#endif
