#ifndef __OPENVCL_ERROR_H__
#define __OPENVCL_ERROR_H__

#include "Token.h"

#include <string>

namespace vcl
{

class Error
{
public:

	Error();
	Error( const Error& e );
	Error( const std::string& string, bool warning = false );
	Error( const std::string& string, const Line& line, bool warning = false );
	Error( const std::string& string, const Token& token, bool warning = false );
	Error( const std::string& string, const Token& token, const Token::Argument& argument, bool warning = false );

	std::string toString() const;

	static void SetFilename( const std::string& filename );
	static void Display( const Error& error );

private:

	enum
	{
		STRING		= 0x01,
		LINE			= 0x02,
		TOKEN			= 0x04,
		ARGUMENT	= 0x08
	};

	unsigned int m_content;

	std::string m_string;
	const Line* m_line;
	const Token* m_token;
	const Token::Argument* m_argument;
	bool m_warning;

	static std::list<Error> ms_errors;
};

}

#endif
