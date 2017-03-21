#pragma once

#include <map>
#include <memory>
#include <string>

class OperatorRegistry {
	public:
		OperatorRegistry();

		void init();

		bool isValidOperatorChar(char) const;
		bool isValidOperator(const std::string& token) const;
		
		virtual ~OperatorRegistry() = default;
	private:
		std::map<char, bool> operatorChars;
		std::map<std::string, bool> operatorStrings;

		void addOperator(const std::string&);
};
