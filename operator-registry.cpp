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

void OperatorRegistry::addOperator(const std::string& token) {
	operatorStrings[token] = true;

	for (std::size_t i = 0, l = token.size(); i < l; ++i) {
		if (!isValidOperatorChar(token[i])) {
			operatorChars[token[i]] = true;
		}
	}
}
