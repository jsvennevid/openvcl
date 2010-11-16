#ifndef __OPENVCL_FILE_H__
#define __OPENVCL_FILE_H__

/*
 * File.h
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

class File
{
public:

File();
File( const File& f );
File( const std::string& name );

const std::string& name() const;

private:

	std::string m_name;

};

#include "File.inl"

}

#endif
