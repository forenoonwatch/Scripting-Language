#include "expression.hpp"
#include "scope-frame.hpp"

Expression::Expression(Interpreter& interp, Statement* expr, std::shared_ptr<Variable> var)
: interpreter(interp), it(std::begin(expr->getTokens())), end(std::end(expr->getTokens())),
	writeVar(var) {}

bool Expression::canEval() const {
	return it != end;
}

void Expression::evalNext() {
	// http://www.geeksforgeeks.org/expression-evaluation/
	
	if (it->getTokenType() == Token::TokenType::NUMERIC || it->getTokenType() == Token::TokenType::STRING) {
		values.push(Variable::fromToken(*it));
	}
	else if (it->getTokenType() == Token::TokenType::IDENTIFIER) {
		if (it->getContent().compare("true") == 0 || it->getContent().compare("false") == 0) {
			values.push(Variable::fromToken(*it));
		}
		else {
			values.push(*interpreter.resolveVariable(it->getContent()));
		}
	}
	else if (it->getTokenType() == Token::TokenType::LINK) {
		if (it->getLink()->getType() == Statement::StatementType::FUNC_CALL) {
			std::shared_ptr<Variable> funcVar = interpreter.resolveVariable(it->getLink()
				->getTokens()[0].getContent());

			if (funcVar != nullptr && funcVar->type == Variable::VariableType::FUNCTION
					&& funcVar->funcValue != nullptr) {
				std::shared_ptr<Variable> returnVar = std::make_shared<Variable>();
				interpreter.scopeStack.push_back(std::make_shared<FunctionFrame>(funcVar->funcValue,
					returnVar));

				interpreter.evaluateExpression = false;
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
	}
	else if (it->getTokenType() == Token::TokenType::OPERATOR) {
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

	++it;

	if (it == end) {
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

		*writeVar = std::move(values.top());
	}
}
