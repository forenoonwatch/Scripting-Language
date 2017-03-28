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
	else if (op.compare("==") == 0) {
		Variable::comp(lhs, rhs, out);
	}
	else if (op.compare("!=") == 0) {
		Variable::comp(lhs, rhs, out);
		out.boolValue = !out.boolValue;
	}
	else if (op.compare("<") == 0) {
		Variable::less(lhs, rhs, out);
	}
	else if (op.compare(">") == 0) {
		Variable::less(rhs, lhs, out);
	}
	else if (op.compare("<=") == 0) {
		Variable::lessEq(lhs, rhs, out);
	}
	else if (op.compare(">=") == 0) {
		Variable::lessEq(rhs, lhs, out);
	}

}

bool OperatorRegistry::hasPrecedence(const std::string& op1, const std::string& op2) const {
	if (op2.compare("(") == 0 || op2.compare(")") == 0) {
		return false;
	}

	return getOperatorLevel(op1) > getOperatorLevel(op2);	
}

inline void OperatorRegistry::addOperator(const std::string& token) {
	operatorStrings[token] = true;

	for (std::size_t i = 0, l = token.size(); i < l; ++i) {
		if (!isValidOperatorChar(token[i])) {
			operatorChars[token[i]] = true;
		}
	}
}

inline int OperatorRegistry::getOperatorLevel(const std::string& op) const {
	if (op.compare("==") == 0 || op.compare("!=") == 0 || op.compare("<") == 0
			|| op.compare(">") == 0 || op.compare(">=") == 0 || op.compare("<=") == 0) {
		return 2;
	}

	if (op.compare("*") == 0 || op.compare("/") == 0 || op.compare("%")) {
		return 1;
	}

	return 0;
} 
