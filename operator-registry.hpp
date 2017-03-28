#pragma once

#include "variable.hpp"
#include <map>
#include <memory>
#include <string>

class OperatorRegistry {
	public:
		OperatorRegistry();

		void init();

		bool isValidOperatorChar(char) const;
		bool isValidOperator(const std::string& token) const;
	
		void applyOperator(const std::string&, const Variable&, const Variable&, Variable&) const;
		bool hasPrecedence(const std::string&, const std::string&) const;

		virtual ~OperatorRegistry() = default;
	private:
		std::map<char, bool> operatorChars;
		std::map<std::string, bool> operatorStrings;

		void addOperator(const std::string&);

		int getOperatorLevel(const std::string&) const;
};
