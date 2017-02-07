#include "operator-registry.hpp"

OperatorRegistry::OperatorRegistry() {
	// add operators
}

bool OperatorRegistry::isValidOperatorChar(char chr) const {
	return operatorChars[chr];
}

void OperatorRegistry::addOperator(std::shared_ptr<Operator> operator) {
	operators[operator->getLabel()] = operator;
}
