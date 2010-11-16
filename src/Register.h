#ifndef __OPENVCL_REGISTER_H__
#define __OPENVCL_REGISTER_H__

/*
 * Register.h
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

class Register
{
	public:

	Register();

	void setName( const std::string& name );
	const std::string& name() const;

	void setAvailable( bool available );
	bool available() const;

	void setBusy( bool busy );
	bool busy() const;

	private:

	std::string m_name;
	bool m_available;
	bool m_busy;
};

#include "Register.inl"

}

#endif
