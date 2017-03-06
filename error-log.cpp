#include "error-log.hpp"

ErrorLog::ErrorLog(std::ostream& warningStream, std::ostream& errorStream)
: warningStream(warningStream), errorStream(errorStream) {}

void ErrorLog::logWarning(const std::string& warning) {
	warningStream << warning << "\n";
}

void ErrorLog::logWarning(const ScriptError& warning) {
	warningStream << warning.getError() << "\n";
}

void ErrorLog::logError(const std::string& error) {
	errorStream << error << "\n";
}

void ErrorLog::logError(const ScriptError& error) {
	errorStream << error.getError() << "\n";
}
