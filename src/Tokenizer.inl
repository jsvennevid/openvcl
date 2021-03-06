///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

inline bool Tokenizer::inComment() const
{
	return m_comment;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

inline void Tokenizer::setComment( bool mode )
{
	m_comment = mode;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

inline Tokenizer::ContentMode Tokenizer::contentMode() const
{
	return m_contentMode;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

inline void Tokenizer::setContentMode( ContentMode mode )
{
	m_contentMode = mode;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

inline Tokenizer::ExecutionPath Tokenizer::executionPath() const
{
	return m_executionPath;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

inline void Tokenizer::setExecutionPath( ExecutionPath path )
{
	m_executionPath = path;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

inline bool Tokenizer::vsmMode() const
{
	return m_vsmMode;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

inline void Tokenizer::setVsmMode( bool mode )
{
	m_vsmMode = mode;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

inline void Tokenizer::setOperands( const std::list<Operand>& operands )
{
	m_operands = &operands;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

inline std::list<Token>& Tokenizer::tokens()
{
	return m_tokens;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

inline void Tokenizer::setNewSyntax( bool newSyntax )
{
	m_newSyntax = newSyntax;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

inline bool Tokenizer::newSyntax() const
{
	return m_newSyntax;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

inline unsigned int Tokenizer::availableFloats() const
{
	return m_availableFloats;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

inline unsigned int Tokenizer::availableIntegers() const
{
	return m_availableIntegers;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

inline void Tokenizer::setAvailableFloats( unsigned int floats )
{
	m_availableFloats = floats;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

inline void Tokenizer::setAvailableIntegers( unsigned int integers )
{
	m_availableIntegers = integers;
}
