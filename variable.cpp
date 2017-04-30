#include "variable.hpp"
#include <cstdlib>

Variable::Variable(Statement* func): funcValue(func), type(VariableType::FUNCTION) {}

Variable::Variable(ExternalFunction func): efValue(func),
	type(VariableType::EXTERNAL_FUNC) {}

void Variable::cloneInto(Variable& var) const {
	var.type = type;
	var.stringValue = stringValue;
	var.intValue = intValue;
	var.floatValue = floatValue;
	var.boolValue = boolValue;
	var.funcValue = funcValue;
}

void Variable::operator()(std::vector<std::shared_ptr<Variable>>& args,
		std::shared_ptr<Variable> ret, Interpreter& terp) const {
	efValue(args, ret, terp);
}

double Variable::getAsFloat() const {
	if (type == VariableType::FLOAT) {
		return floatValue;
	}
	else if (type == VariableType::INT) {
		return intValue;
	}
	else if (type == VariableType::BOOL) {
		return boolValue ? 1 : 0;
	}
	
	return 0;
}

int Variable::getAsInt() const {
	if (type == VariableType::FLOAT) {
		return static_cast<int>(floatValue);
	}
	else if (type == VariableType::INT) {
		return intValue;
	}
	else if (type == VariableType::BOOL) {
		return boolValue ? 1 : 0;
	}
	
	return 0;
}

Variable Variable::fromToken(const Token& token) {
	Variable out;

	if (token.getTokenType() == Token::TokenType::STRING) {
		out.stringValue = token.getContent(); // TODO: eval string here
		out.type = VariableType::STRING;
	}
	else if (token.getTokenType() == Token::TokenType::NUMERIC) {
		if (token.getContent().find(".") != std::string::npos) {
			out.floatValue = std::atof(token.getContent().c_str());
			out.type = VariableType::FLOAT;
		}
		else {
			out.intValue = std::atoi(token.getContent().c_str());
			out.type = VariableType::INT;
		}
	}
	else if (token.getTokenType() == Token::TokenType::IDENTIFIER) {
		if (token.getContent().compare("true") == 0) {
			out.boolValue = true;
			out.type = VariableType::BOOL;
		}
		else if (token.getContent().compare("false") == 0) {
			out.boolValue = false;
			out.type = VariableType::BOOL;
		}
		else {
			// TODO: runtime error
		}
	}
	else {
		// TODO: runtime error
	}

	return out;
}

void Variable::add(const Variable& a, const Variable& b, Variable& out) {
	if (a.type == VariableType::STRING) {
		out.type = VariableType::STRING;

		if (b.type == VariableType::STRING) {
			out.stringValue = a.stringValue + b.stringValue;
		}
		else if (b.type == VariableType::INT) {
			out.stringValue = a.stringValue + std::to_string(b.intValue);
		}
		else if (b.type == VariableType::FLOAT) {
			out.stringValue = a.stringValue + std::to_string(b.floatValue);
		}
		else {
			// TODO: runtime error
		}
	}
	else if (a.type == VariableType::INT) {
		out.type = VariableType::INT;

		if (b.type == VariableType::INT) {
			out.intValue = a.intValue + b.intValue;
		}
		else if (b.type == VariableType::FLOAT) {
			out.floatValue = a.intValue + b.floatValue;
		}
		else {
			// TODO: runtime error
		}
	}
	else if (a.type == VariableType::FLOAT) {
		out.type = VariableType::FLOAT;

		if (out.type == VariableType::INT) {
			out.floatValue = a.floatValue + b.intValue;
		}
		else if (out.type == VariableType::FLOAT) {
			out.floatValue = a.floatValue + b.floatValue;
		}
		else {
			// TODO: runtime error
		}
	}
	else {
		// TODO: runtime error
	}
}

void Variable::sub(const Variable& a, const Variable& b, Variable& out) {
	if (a.type == VariableType::INT) {
		out.type = VariableType::INT;

		if (b.type == VariableType::INT) {
			out.intValue = a.intValue - b.intValue;
		}
		else if (b.type == VariableType::FLOAT) {
			out.floatValue = a.intValue - b.floatValue;
		}
		else {
			// TODO: runtime error
		}
	}
	else if (a.type == VariableType::FLOAT) {
		out.type = VariableType::FLOAT;

		if (out.type == VariableType::INT) {
			out.floatValue = a.floatValue - b.intValue;
		}
		else if (out.type == VariableType::FLOAT) {
			out.floatValue = a.floatValue - b.floatValue;
		}
		else {
			// TODO: runtime error
		}
	}
	else {
		// TODO: runtime error
	}
}

void Variable::mul(const Variable& a, const Variable& b, Variable& out) {
	if (a.type == VariableType::INT) {
		out.type = VariableType::INT;

		if (b.type == VariableType::INT) {
			out.intValue = a.intValue * b.intValue;
		}
		else if (b.type == VariableType::FLOAT) {
			out.floatValue = a.intValue * b.floatValue;
		}
		else {
			// TODO: runtime error
		}
	}
	else if (a.type == VariableType::FLOAT) {
		out.type = VariableType::FLOAT;

		if (out.type == VariableType::INT) {
			out.floatValue = a.floatValue * b.intValue;
		}
		else if (out.type == VariableType::FLOAT) {
			out.floatValue = a.floatValue * b.floatValue;
		}
		else {
			// TODO: runtime error
		}
	}
	else {
		// TODO: runtime error
	}
}

void Variable::div(const Variable& a, const Variable& b, Variable& out) {
	if (a.type == VariableType::INT) {
		out.type = VariableType::INT;

		if (b.type == VariableType::INT) {
			out.intValue = a.intValue / b.intValue;
		}
		else if (b.type == VariableType::FLOAT) {
			out.floatValue = a.intValue / b.floatValue;
		}
		else {
			// TODO: runtime error
		}
	}
	else if (a.type == VariableType::FLOAT) {
		out.type = VariableType::FLOAT;

		if (out.type == VariableType::INT) {
			out.floatValue = a.floatValue / b.intValue;
		}
		else if (out.type == VariableType::FLOAT) {
			out.floatValue = a.floatValue / b.floatValue;
		}
		else {
			// TODO: runtime error
		}
	}
	else {
		// TODO: runtime error
	}
}

void Variable::mod(const Variable& a, const Variable& b, Variable& out) {
	if (a.type == VariableType::INT) {
		out.type = VariableType::INT;

		if (b.type == VariableType::INT) {
			out.intValue = a.intValue % b.intValue;
		}
		else {
			// TODO: runtime error
		}
	}
	else {
		// TODO: runtime error
	}
}

void Variable::comp(const Variable& a, const Variable& b, Variable& out) {
	out.type = VariableType::BOOL;

	if (a.type == VariableType::STRING) {
		if (b.type == VariableType::STRING) {
			out.boolValue = a.stringValue.compare(b.stringValue) == 0;
		}
		else if (b.type != VariableType::OTHER) {
			out.boolValue = false;
		}
		else {
			// TODO: runtime error
		}
	}
	else if (a.type == VariableType::INT) {
		if (b.type == VariableType::INT) {
			out.boolValue = a.intValue == b.intValue;
		}
		else if (b.type == VariableType::FLOAT) {
			out.boolValue = a.intValue == b.floatValue;
		}
		else if (b.type != VariableType::OTHER) {
			out.boolValue = false;
		}
		else {
			// TODO: runtime error
		}
	}
	else if (a.type == VariableType::FLOAT) {
		if (b.type == VariableType::INT) {
			out.boolValue = a.floatValue == b.intValue;
		}
		else if (b.type == VariableType::FLOAT) {
			out.boolValue = a.floatValue == b.floatValue;
		}
		else if (b.type != VariableType::OTHER) {
			out.boolValue = false;
		}
		else {
			// TODO: runtime error
		}
	}
	else if (a.type == VariableType::BOOL) {
		if (b.type == VariableType::BOOL) {
			out.boolValue = a.boolValue == b.boolValue;
		}
		else if (a.type != VariableType::OTHER) {
			out.boolValue = false;
		}
		else {
			// TODO: runtime error
		}
	}
	else {
		// TODO: runtime error
	}
}

void Variable::less(const Variable& a, const Variable& b, Variable& out) {
	out.type = VariableType::BOOL;

	if (a.type == VariableType::INT) {
		if (b.type == VariableType::INT) {
			out.boolValue = a.intValue < b.intValue;
		}
		else if (b.type == VariableType::FLOAT) {
			out.boolValue = a.intValue < b.floatValue;
		}
		else {
			// TODO: runtime error
		}
	}
	else if (a.type == VariableType::FLOAT) {
		if (b.type == VariableType::INT) {
			out.boolValue = a.floatValue < b.intValue;
		}
		else if (b.type == VariableType::FLOAT) {
			out.boolValue = a.floatValue < b.floatValue;
		}
		else {
			// TODO: runtime error
		}
	}
	else {
		// TODO: runtime error
	}
}

void Variable::lessEq(const Variable& a, const Variable& b, Variable& out) {
	out.type = VariableType::BOOL;

	if (a.type == VariableType::INT) {
		if (b.type == VariableType::INT) {
			out.boolValue = a.intValue <= b.intValue;
		}
		else if (b.type == VariableType::FLOAT) {
			out.boolValue = a.intValue <= b.floatValue;
		}
		else {
			// TODO: runtime error
		}
	}
	else if (a.type == VariableType::FLOAT) {
		if (b.type == VariableType::INT) {
			out.boolValue = a.floatValue <= b.intValue;
		}
		else if (b.type == VariableType::FLOAT) {
			out.boolValue = a.floatValue <= b.floatValue;
		}
		else {
			// TODO: runtime error
		}
	}
	else {
		// TODO: runtime error
	}
}

void Variable::logicalAnd(const Variable& a, const Variable& b, Variable& out) {
	out.type = VariableType::BOOL;

	if (a.type == VariableType::BOOL) {
		if (b.type == VariableType::BOOL) {
			out.boolValue = a.boolValue && b.boolValue;
		}
		else {
			out.boolValue = a.boolValue && b.type != VariableType::NIL;
		}
	}
	else {
		if (b.type == VariableType::BOOL) {
			out.boolValue = a.type != VariableType::NIL && b.boolValue;
		}
		else {
			out.boolValue = a.type != VariableType::NIL
				&& b.type != VariableType::NIL;
		}
	}
}

void Variable::logicalOr(const Variable& a, const Variable& b, Variable& out) {
	out.type = VariableType::BOOL;

	if (a.type == VariableType::BOOL) {
		if (b.type == VariableType::BOOL) {
			out.boolValue = a.boolValue || b.boolValue;
		}
		else {
			out.boolValue = a.boolValue || b.type != VariableType::NIL;
		}
	}
	else {
		if (b.type == VariableType::BOOL) {
			out.boolValue = a.type != VariableType::NIL || b.boolValue;
		}
		else {
			out.boolValue = a.type != VariableType::NIL
				|| b.type != VariableType::NIL;
		}
	}
}

void Variable::unaryMinus(const Variable& var, Variable& out) {
	if (var.type == VariableType::INT) {
		out.intValue = -var.intValue;
		out.type = VariableType::INT;
	}
	else if (var.type == VariableType::FLOAT) {
		out.floatValue = -var.floatValue;
		out.type = VariableType::FLOAT;
	}
	else {
		// TODO: runtime error
	}
}

void Variable::logicalNot(const Variable& var, Variable& out) {
	out.type = VariableType::BOOL;

	if (var.type == VariableType::BOOL) {
		out.boolValue = !var.boolValue;
	}
	else {
		out.boolValue = var.type != VariableType::NIL;
	}
}

std::string Variable::toString(const Variable& var) {
	switch (var.type) {
		case VariableType::INT:
			return std::to_string(var.intValue);
		case VariableType::FLOAT:
			return std::to_string(var.floatValue);
		case VariableType::BOOL:
			return var.boolValue ? "true" : "false";
		case VariableType::STRING:
			return var.stringValue;
		case VariableType::FUNCTION:
			return "Function";
		case VariableType::NIL:
			return "nil";
		default:
			return "Unknown Variable Type";
	}

	return "Unknown Variable Type";
}
