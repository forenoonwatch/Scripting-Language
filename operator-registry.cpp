#include "operator-registry.hpp"

OperatorRegistry::OperatorRegistry() {
	// TODO: replace with correct code for giving operators functionality
	addOperator(std::make_shared<Operator>("="));
	addOperator(std::make_shared<Operator>("~"));
	addOperator(std::make_shared<Operator>("("));
	addOperator(std::make_shared<Operator>(")"));
	addOperator(std::make_shared<Operator>(","));
	addOperator(std::make_shared<Operator>(";"));

	addOperator(std::make_shared<Operator>("+"));
	addOperator(std::make_shared<Operator>("-")); // TODO: figure out how to do unary -
	addOperator(std::make_shared<Operator>("*"));
	addOperator(std::make_shared<Operator>("/"));
	addOperator(std::make_shared<Operator>("%"));

	addOperator(std::make_shared<Operator>("=="));
	addOperator(std::make_shared<Operator>(">="));
	addOperator(std::make_shared<Operator>("<="));
	addOperator(std::make_shared<Operator>(">"));
	addOperator(std::make_shared<Operator>("<"));
	addOperator(std::make_shared<Operator>("~="));
}

bool OperatorRegistry::isValidOperatorChar(char chr) const {
	return operatorChars.find(chr) != std::end(operatorChars);
}

bool OperatorRegistry::isValidOperator(const std::string& token) const {
	return operators.find(token) != std::end(operators);
}

void OperatorRegistry::addOperator(std::shared_ptr<Operator> op) {
	std::string label = op->getLabel();
	operators[label] = op;

	for (std::size_t i = 0, l = label.size(); i < l; ++i) {
		if (!isValidOperatorChar(label[i])) {
			operatorChars[label[i]] = true;
		}
	}
}
