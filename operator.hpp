#pragma once

#include <string>

class Operator {
	public:
		enum ArgumentType {
			UNARY,
			BINARY,
			OTHER
		};

		enum OperatorType {
			ARITHMETIC,
			LOGICAL,
			COMPARISON,
			OTHER
		};

		Operator(const std::string& label, ArgumentType argType, OperatorType opType); // TODO: add action

		std::string getLabel() const;
		ArgumentType getArgumentType() const;
		OperatorType getOperatorType() const;

		virtual ~Operator() = default;
	private:
		std::string label;
		ArgumentType argumentType;
		OperatorType operatorType;
};

#include "operator.inl"
