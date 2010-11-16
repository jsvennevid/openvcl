#include "Error.h"

#include <iostream>
#include <sstream>

namespace vcl
{

std::list<Error> Error::ms_errors;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Error::Error()
{
	m_content = 0;
	m_token = NULL;
	m_argument = NULL;
	m_warning = false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Error::Error( const Error& e )
{
	m_content = e.m_content;

	m_string = e.m_string;
	m_token = e.m_token;
	m_argument = e.m_argument;

	m_warning = e.m_warning;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Error::Error( const std::string& string, bool warning )
{
	m_content = 0;

	m_string = string;
	m_token = NULL;
	m_argument = NULL;

	m_warning = warning;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Error::Error( const std::string& string, const Token& token, bool warning )
{
	m_content = TOKEN;

	m_string = string;
	m_token = &token;
	m_warning = warning;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Error::Error( const std::string& string, const Line& line, bool warning )
{
	m_content = LINE;

	m_string = string;
	m_line = &line;
	m_warning = warning;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Error::Error( const std::string& string, const Token& token, const Token::Argument& argument, bool warning )
{
	m_content = TOKEN|ARGUMENT;

	m_string = string;
	m_token = &token;
	m_argument = &argument;

	m_warning = warning;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

std::string Error::toString() const
{
	std::stringstream s;

	if( m_content & TOKEN )
	{
		s << m_token->line().file().name() << "(" << m_token->line().originalNumber() << ") : ";
	}
	else if( m_content & LINE )
	{
		s << m_line->file().name() << "(" << m_line->originalNumber() << ") : ";
	}
	else
	{
		s << "openvcl: ";
	}

	if( m_warning )
		s << "warning: ";

	s << m_string;

	if( m_content & ARGUMENT )
	{
		static const char* fieldnames = "xyzw";

		s << " (" << m_token->name() << " ";

		for( std::list<Token::Argument>::const_iterator j = m_token->arguments().begin(); j != m_token->arguments().end(); j++ )
		{
			if( j != m_token->arguments().begin() )
				s << ", ";

			if( m_argument == &(*j) )
				s << ">>> ";

			s << (*j).text();

			if( m_argument == &(*j) )
				s << " <<<";
		}

		if( m_argument->fields() )
		{
			s << " [";
			for( unsigned int i = 0; i < 4; i++ )
			{
				if( m_argument->fields() & (1<<i) )
					s << fieldnames[i];
				else
					s << " ";
			}
			s << "]";
		}

		s << ")";
	}

	return s.str();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Error::Display( const Error& e )
{
	std::cerr << e.toString() << std::endl;
}

}
