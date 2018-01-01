/// Name: affignexcept.h
/// Purpose: Custom exception class
/// Author: John Kalyan
/// Copyright: (c) 2017 John Kalyan
/// License: MIT License

#pragma once

#include <exception>

class AffignException : public std::exception
{
public:
	AffignException(const char* text);
	const char* what() const noexcept;
private:
	AffignException();
	const char* msg;
};
