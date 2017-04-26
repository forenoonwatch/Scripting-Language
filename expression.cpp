#include "expression.hpp"
#include "scope-frame.hpp"

Expression::Expression(Interpreter& interp, Statement* expr, std::shared_ptr<Variable> var,
		int startOffset)
: interpreter(interp), it(std::begin(expr->getTokens())), end(std::end(expr->getTokens())),
	writeVar(var), expectValue(false), lastToken(std::string(), Token::TokenType::OTHER),
	startOffset(startOffset) {}

bool Expression::canEval() const {
	return it != end;
}

void Expression::evalNext() {
	// http://www.geeksforgeeks.org/expression-evaluation/
	
	if (it->getTokenType() == Token::TokenType::NUMERIC || it->getTokenType() == Token::TokenType::STRING) {
		Variable var = Variable::fromToken(*it);

		if (!unaryOps.empty()) {
			interpreter.operatorRegistry.applyOperator(unaryOps.top(), var, var);
			unaryOps.pop();
		}

		values.push(var);
	}
	else if (it->getTokenType() == Token::TokenType::IDENTIFIER) {
		if (it->getContent().compare("true") == 0 || it->getContent().compare("false") == 0) {
			values.push(Variable::fromToken(*it));
		}
		else {
			Variable var = *interpreter.resolveVariable(it->getContent(), startOffset);

			if (!unaryOps.empty()) {
				interpreter.operatorRegistry.applyOperator(unaryOps.top(), var, var);
				unaryOps.pop();
			}

			values.push(var);
		}
	}
	else if (it->getTokenType() == Token::TokenType::LINK) {
		if (it->getLink()->getType() == Statement::StatementType::FUNC_CALL) {
			std::shared_ptr<Variable> funcVar = interpreter.resolveVariable(it->getLink()
				->getTokens()[0].getContent(), startOffset);

			std::cout << "interpreting function call" << std::endl;

			if (funcVar != nullptr && funcVar->type == Variable::VariableType::FUNCTION
					&& funcVar->funcValue != nullptr) {
				std::shared_ptr<FunctionFrame> funcFrame = std::make_shared<FunctionFrame>(interpreter,
					funcVar->funcValue, it->getLink());
				
				interpreter.scopeStack.push_back(funcFrame);
				
				interpreter.evaluateExpression = false;
				expectValue = true;
				return;
			}
			else {
				std::cout << "invalid function name" << std::endl;
				// TODO: runtime error
			}
		}
		else {
			// TODO: runtime error
		}
	}
	else if (it->getContent().compare("(") == 0) {
		operators.push(it->getContent());
	}
	else if (it->getContent().compare(")") == 0) {
		while (operators.top().compare("(") != 0) {
			Variable v1 = values.top();
			values.pop();

			Variable v2 = values.top();
			values.pop();

			Variable out;
			interpreter.operatorRegistry.applyOperator(operators.top(), v2, v1, out);
			values.push(out);

			operators.pop();
		}

		operators.pop();

		if (!unaryOps.empty()) {
			Variable var = values.top();
			values.pop();

			interpreter.operatorRegistry.applyOperator(unaryOps.top(), var, var);
			values.push(var);

			unaryOps.pop();
		}
	}
	else if (it->getTokenType() == Token::TokenType::OPERATOR) {
		if ((lastToken.getTokenType() == Token::TokenType::OPERATOR || lastToken.getTokenType() == Token::TokenType::OTHER)
				&& lastToken.getContent().compare("(") != 0 && lastToken.getContent().compare(")") != 0) {
			unaryOps.push(it->getContent());
		}
		else {
			while (!operators.empty()
					&& interpreter.operatorRegistry.hasPrecedence(it->getContent(), operators.top())) {
				Variable v1 = values.top();
				values.pop();

				Variable v2 = values.top();
				values.pop();
			
				Variable out;
				interpreter.operatorRegistry.applyOperator(operators.top(), v2, v1, out);
				values.push(out);

				operators.pop();
			}

			operators.push(it->getContent());
		}
	}

	lastToken = *it;
	++it;
}

void Expression::finishEval() {
	while (!operators.empty()) {
		Variable v1 = values.top();
		values.pop();

		Variable v2 = values.top();
		values.pop();
			
		Variable out;
		interpreter.operatorRegistry.applyOperator(operators.top(), v2, v1, out);
		values.push(out);

		operators.pop();
	}

	std::cout << "setting value of " << Variable::toString(values.top()) << std::endl;
	*writeVar = std::move(values.top());
}

void Expression::addValue(const Variable& value) {
	values.push(value);
	expectValue = false;
	++it;
}

bool Expression::isExpectingValue() const {
	return expectValue;
}
