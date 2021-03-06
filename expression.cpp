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

			interpreter.errorLog.logDebug("interpreting function call");

			if (funcVar != nullptr
					&& (funcVar->type == Variable::VariableType::FUNCTION
					|| funcVar->type == Variable::VariableType::EXTERNAL_FUNC)) {
				
				if (funcVar->type == Variable::VariableType::FUNCTION) {
					interpreter.addFunctionCall(funcVar->funcValue, it->getLink());
				}
				else {
					interpreter.addFunctionCall(it->getLink(), funcVar);
				}

				interpreter.evaluateExpression = false;
				expectValue = true;

				lastToken = *it;
				return;
			}
			else {
				interpreter.errorLog.logError("invalid function name");
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
			interpreter.errorLog.logDebug("loaded unary operator");
		}
		else {
			interpreter.errorLog.logDebug("loaded binary operator ("
				+ it->getContent() + ")");

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
	while (!unaryOps.empty()) {
		Variable v = values.top();
		values.pop();

		Variable out;
		interpreter.operatorRegistry.applyOperator(unaryOps.top(), v, out);
		values.push(out);

		unaryOps.pop();
	}

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

	interpreter.errorLog.logDebug("setting value of "
		+ Variable::toString(values.top()));
	*writeVar = std::move(values.top());
}

void Expression::addValue(const Variable& value) {
	values.push(value);
	expectValue = false;
	++it;

	interpreter.errorLog.logDebug("Added value of " + Variable::toString(value)
		+ "; expression can continue");
}

bool Expression::isExpectingValue() const {
	return expectValue;
}
