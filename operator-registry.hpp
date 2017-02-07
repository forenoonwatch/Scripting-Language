#pragma once

#include <map>
#include <memory>
#include <string>

class OperatorRegistry {
	public:
		OperatorRegistry();

		bool isValidOperatorChar(char) const;

		virtual ~OperatorRegistry() = default;
	private:
		std::map<char, bool> operatorChars;
		std::map<std::string, std::shared_ptr<Operator>> operators;

		void addOperator(std::shared_ptr<Operator> operator);
};
