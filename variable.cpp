#include "variable.hpp"
#include <cstdlib>

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
