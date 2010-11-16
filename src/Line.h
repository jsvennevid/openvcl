#ifndef __OPENVCL_LINE_H__
#define __OPENVCL_LINE_H__

/*
 * Line.h
 *
 * Copyright (C) 2004 Jesper Svennevid, Daniel Collin
 *
 * Licensed under the AFL v2.0. See the file LICENSE included with this
 * distribution for licensing terms.
 *
 */

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "File.h"

#include <string>

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace vcl
{

class Line
{
public:

	Line( const File& file, unsigned int number, unsigned int originalNumber, const std::string& content );
	Line( const Line& l );

	unsigned int number() const;					// raw line number
	const std::string& content() const;
	unsigned int originalNumber() const;	// line number from original source-file
	const File& file() const;

private:

	const File& m_file;
	unsigned int m_number;
	unsigned int m_originalNumber;
	std::string m_content;

};

#include "Line.inl"

}

#endif
