#include "Error.hpp"
using namespace et;

#include <backward.hpp>
using namespace backward;

#include <iostream>
#include <sstream>

static bool g_enable_trace_on_exception = true;

ETALER_EXPORT void et::enableTraceOnException(bool enable)
{
	g_enable_trace_on_exception = true;
}

ETALER_EXPORT bool et::getEnableTraceOnException()
{
	return g_enable_trace_on_exception;
}

std::string et::genStackTrace(size_t skip)
{
#ifndef BACKWARD_SYSTEM_UNKNOWN
	std::stringstream ss;
	StackTrace st;
	// Skip the at least function calls we don't want
	// 1. unwind 2. load_here, 3. genStackTrace
	st.skip_n_firsts(3+skip);
	st.load_here(32);
	Printer p;
	p.color_mode = ColorMode::never;
	p.print(st, ss);
	return ss.str();
#else
	#pragma warning Cannot provide stack unwinding support for this system
	static bool warning_printed = false;
	if(warning_printed == false) {
		warning_printed = true;
		std::cerr << "Warning: Cannot provide stack unwinding support for this system." << std::endl;
	}
	return "";
#endif
}

ETALER_EXPORT EtError::EtError(const std::string &msg)
	: msg_(msg)
{
	if(getEnableTraceOnException())
		msg_ += "\n"+genStackTrace(1); // Skip the EtError ctor
}