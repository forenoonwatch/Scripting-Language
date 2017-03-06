#pragma once

#include <string>
#include <iostream>
#include "script-error.hpp"

class ErrorLog {
	public:
		ErrorLog(std::ostream& warningStream = std::cout,
			std::ostream& errorStream = std::cerr);

		void logWarning(const std::string&);
		void logWarning(const ScriptError&);

		void logError(const std::string&);
		void logError(const ScriptError&);

		virtual ~ErrorLog() = default;
	private:
		std::ostream& warningStream;
		std::ostream& errorStream;
};
