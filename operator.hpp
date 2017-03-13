#pragma once

#include <string>

enum class OperatorType {
	ARITHMETIC,
	LOGICAL,
	COMPARISON,
	OP_OTHER
};
		
enum class ArgumentType {
	UNARY,
	BINARY,
	ARG_OTHER
};


template <typename A, typename B>
class Operator {
	public:
		typedef A (*OperatorCallback)(const A&, const B&);

		Operator(const std::string& label,
			ArgumentType argType, OperatorType opType); 

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
