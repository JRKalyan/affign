/// Name: affignexcept.cpp
/// Purpose: Custom exception class
/// Author: John Kalyan
/// Copyright: (c) 2017 John Kalyan
/// License: MIT License

#include "affignexcept.h"

// Constructor ----------------------------------------------------------------

AffignException::AffignException(const char* text) : msg(text) {};

// Methods --------------------------------------------------------------------

/// returns message of exception
const char* AffignException::what() const noexcept
{
	return msg;
}
