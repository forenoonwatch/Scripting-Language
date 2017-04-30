#include "error-log.hpp"

ErrorLog::ErrorLog(std::ostream& warningStream, std::ostream& errorStream,
	std::ostream& debugStream)
: warningStream(warningStream), errorStream(errorStream), debugStream(debugStream) {}

void ErrorLog::setLogDepth(LogDepth depth) {
	logDepth = depth;
}

ErrorLog::LogDepth ErrorLog::getLogDepth() const {
	return logDepth;
}

void ErrorLog::logDebug(const std::string& debug) {
	if (logDepth < LogDepth::DEBUG) {
		return;
	}

	debugStream << debug << "\n";
	debugStream.flush();
};

void ErrorLog::logWarning(const std::string& warning) {
	if (logDepth < LogDepth::WARNING) {
		return;
	}
	
	warningStream << warning << "\n";
	warningStream.flush();
}

void ErrorLog::logWarning(const ScriptError& warning) {
	logWarning(warning.getError());
}

void ErrorLog::logError(const std::string& error) {
	if (logDepth < LogDepth::ERROR) {
		return;
	}
	
	errorStream << error << "\n";
	errorStream.flush();
}

void ErrorLog::logError(const ScriptError& error) {
	logError(error.getError());
}
