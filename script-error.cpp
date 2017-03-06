#include "script-error.hpp"

SyntaxError::SyntaxError(const std::string& expected, const std::string& got)
: expected(expected), got(got) {}

std::string SyntaxError::getError() const {
	return "Syntax error: expected " + expected + " got " + got;
}
