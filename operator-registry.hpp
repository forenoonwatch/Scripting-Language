#pragma once

#include <map>
#include <memory>
#include <string>
#include "operator.hpp"

class OperatorRegistry {
	public:
		OperatorRegistry();

		bool isValidOperatorChar(char) const;
		bool isValidOperator(const std::string& token) const;
		
		bool isArithmeticOperator(const Token& token) const;
		bool isLogicalOperator(const Token& token) const;

		virtual ~OperatorRegistry() = default;
	private:
		std::map<char, bool> operatorChars;
		std::map<std::string, std::shared_ptr<Operator>> operators;

		void addOperator(std::shared_ptr<Operator> op);
};
