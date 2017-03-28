#include "operator-registry.hpp"

OperatorRegistry::OperatorRegistry() {}

void OperatorRegistry::init() {
	addOperator("=");
	addOperator("(");
	addOperator(")");
	addOperator(",");

	addOperator("+");
	addOperator("-");
	addOperator("*");
	addOperator("/");
	addOperator("%");

	addOperator("==");
	addOperator("!=");
	addOperator("<");
	addOperator(">");
	addOperator("<=");
	addOperator(">=");
}

bool OperatorRegistry::isValidOperatorChar(char chr) const {
	return operatorChars.find(chr) != std::end(operatorChars);
}

bool OperatorRegistry::isValidOperator(const std::string& token) const {
	return operatorStrings.find(token) != std::end(operatorStrings);
}

void OperatorRegistry::applyOperator(const std::string& op, const Variable& lhs,
		const Variable& rhs, Variable& out) const {
	
	if (op.compare("+") == 0) {
		Variable::add(lhs, rhs, out);
	}
	else if (op.compare("-") == 0) {
		Variable::sub(lhs, rhs, out);
	}
	else if (op.compare("*") == 0) {
		Variable::mul(lhs, rhs, out);
	}
	else if (op.compare("/") == 0) {
		Variable::div(lhs, rhs, out);
	}
	else if (op.compare("%") == 0) {
		Variable::mod(lhs, rhs, out);
	}
}

bool OperatorRegistry::hasPrecedence(const std::string& op1, const std::string& op2) const {
	if (op2.compare("(") == 0 || op2.compare(")") == 0) {
		return false;
	}
	
	return !((op1.compare("*") == 0 || op1.compare("/") == 0 || op1.compare("%") == 0)
		&& (op2.compare("+") == 0 || op2.compare("-") == 0));
}

inline void OperatorRegistry::addOperator(const std::string& token) {
	operatorStrings[token] = true;

	for (std::size_t i = 0, l = token.size(); i < l; ++i) {
		if (!isValidOperatorChar(token[i])) {
			operatorChars[token[i]] = true;
		}
	}
}
