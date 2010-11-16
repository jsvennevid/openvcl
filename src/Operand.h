#ifndef __OPENVCL_OPERAND_H__
#define __OPENVCL_OPERAND_H__

/*
 * Operand.h
 *
 * Copyright (C) 2004 Jesper Svennevid, Daniel Collin
 *
 * Licensed under the AFL v2.0. See the file LICENSE included with this
 * distribution for licensing terms.
 *
 */

#include <string>
#include <list>

namespace vcl
{

class Operand
{

public:

	enum
	{
		PREPROCESSOR	= 0x00000001,				// preprocessor operand
		UPPER					= 0x00000002,				// executing in upper execution path
		LOWER					= 0x00000004,				// executing in lower execution path
		DEST					= 0x00000008,				// operand has destination fields
		BROADCAST 		= 0x00000010|DEST,	// operand is broadcasting a source parameter (dest implied)
		XYZ						= 0x00000020|DEST,	// operand requires as destination '.xyz' if field specified
		MULTI					= 0x00000040,				// varargs operand
		DYNAMIC				= 0x00000080,				// operand may result in dynamic branching (branch unit only)
		IWRITE				= 0x00000100,				// operand writes to I (special flag for LOI)
		FILTERED			= 0x00000200				// operand should not be output into the resulting VSM/DSM
	};

	enum Unit
	{
		// non-VU
		INVALID,

		// Preprocessor pseudo-units
		ENTER,
		EXIT,

		// upper
		FMAC,

		// lower
		FDIV,
		LSU,
		IALU,
		BRU,
		RANDU,
		EFU
	};

	Operand( const std::string& name, unsigned int arguments, unsigned int flags, const std::string& pattern, Unit unit = INVALID, unsigned int throughput = 0, unsigned int latency = 0 );
	Operand( const Operand& o );

	bool isLowerExecutionPath() const;
	bool isUpperExecutionPath() const;
	bool isPreprocessor() const;

	const std::string& name() const;
	unsigned int arguments() const;
	unsigned int flags() const;
	const std::string& pattern() const;
	Unit unit() const;
	unsigned int throughput() const;
	unsigned int latency() const;

	void addAlternative( const Operand* operand );
	const std::list<const Operand*>& alternatives() const;

private:

	std::string m_name;
	unsigned int m_arguments;
	unsigned int m_flags;
	std::string m_pattern;
	Unit m_unit;
	unsigned int m_throughput;
	unsigned int m_latency;

	std::list<const Operand*> m_alternatives;
};

#include "Operand.inl"

}

#endif
