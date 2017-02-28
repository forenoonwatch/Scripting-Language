#include "operator-registry.hpp"

OperatorRegistry::OperatorRegistry() {
	// TODO: replace with correct code for giving operators functionality
	addOperator(std::make_shared<Operator>("=", Operator::ArgumentType::BINARY, Operator::OperatorType::OP_OTHER));
	addOperator(std::make_shared<Operator>("~", Operator::ArgumentType::UNARY, Operator::OperatorType::LOGICAL));
	addOperator(std::make_shared<Operator>("(", Operator::ArgumentType::BINARY, Operator::OperatorType::OP_OTHER));
	addOperator(std::make_shared<Operator>(")", Operator::ArgumentType::BINARY, Operator::OperatorType::OP_OTHER));
	addOperator(std::make_shared<Operator>(",", Operator::ArgumentType::BINARY, Operator::OperatorType::OP_OTHER));
	addOperator(std::make_shared<Operator>(";", Operator::ArgumentType::ARG_OTHER, Operator::OperatorType::OP_OTHER));

	addOperator(std::make_shared<Operator>("+", Operator::ArgumentType::BINARY, Operator::OperatorType::ARITHMETIC));
	addOperator(std::make_shared<Operator>("-", Operator::ArgumentType::BINARY, Operator::OperatorType::ARITHMETIC)); // TODO: figure out how to do unary -
	addOperator(std::make_shared<Operator>("*", Operator::ArgumentType::BINARY, Operator::OperatorType::ARITHMETIC));
	addOperator(std::make_shared<Operator>("/", Operator::ArgumentType::BINARY, Operator::OperatorType::ARITHMETIC));
	addOperator(std::make_shared<Operator>("%", Operator::ArgumentType::BINARY, Operator::OperatorType::ARITHMETIC));

	addOperator(std::make_shared<Operator>("==", Operator::ArgumentType::BINARY, Operator::OperatorType::LOGICAL));
	addOperator(std::make_shared<Operator>(">=", Operator::ArgumentType::BINARY, Operator::OperatorType::LOGICAL));
	addOperator(std::make_shared<Operator>("<=", Operator::ArgumentType::BINARY, Operator::OperatorType::LOGICAL));
	addOperator(std::make_shared<Operator>(">", Operator::ArgumentType::BINARY, Operator::OperatorType::LOGICAL));
	addOperator(std::make_shared<Operator>("<", Operator::ArgumentType::BINARY, Operator::OperatorType::LOGICAL));
	addOperator(std::make_shared<Operator>("~=", Operator::ArgumentType::BINARY, Operator::OperatorType::LOGICAL));
}

bool OperatorRegistry::isValidOperatorChar(char chr) const {
	return operatorChars.find(chr) != std::end(operatorChars);
}

bool OperatorRegistry::isValidOperator(const std::string& token) const {
	return operators.find(token) != std::end(operators);
}

bool OperatorRegistry::isArithmeticOperator(const std::string& token) const {
	return operators.find(token)->second->getOperatorType() == Operator::OperatorType::ARITHMETIC;
}

bool OperatorRegistry::isLogicalOperator(const std::string& token) const {
	return operators.find(token)->second->getOperatorType() == Operator::OperatorType::LOGICAL;
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
