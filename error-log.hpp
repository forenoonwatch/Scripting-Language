#pragma once

#include <string>
#include <iostream>
#include "script-error.hpp"

class ErrorLog {
	public:
		enum LogDepth {
			NONE,
			ERROR,
			WARNING,
			DEBUG
		};

		ErrorLog(std::ostream& warningStream = std::cout,
			std::ostream& errorStream = std::cerr,
			std::ostream& debugStream = std::cout);

		void setLogDepth(LogDepth depth);
		LogDepth getLogDepth() const;

		void logDebug(const std::string&);

		void logWarning(const std::string&);
		void logWarning(const ScriptError&);

		void logError(const std::string&);
		void logError(const ScriptError&);

		virtual ~ErrorLog() = default;
	private:
		std::ostream& warningStream;
		std::ostream& errorStream;
		std::ostream& debugStream;

		LogDepth logDepth = LogDepth::WARNING;
};
