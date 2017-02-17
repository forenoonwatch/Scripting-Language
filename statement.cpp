#include "statement.hpp"

Statement::Statement(StatementType type, Statement* parent)
: type(type), parent(parent) {
	if (parent != nullptr) {
		parent->addChild(this);
	}
}

void Statement::addChild(Statement* child) {
	child->parent = this;
	children.push_back(child);
}

void Statement::addToken(Token token) {
	tokens.push_back(token);
}

std::string Statement::typeAsString(StatementType type) {
	switch (type) {
		case VAR_DECLARATION:
			return "VAR_DECLARATION";
		case VAR_ASSIGNMENT:
			return "VAR_ASSIGNMENT";
		case EXPRESSION:
			return "EXPRESSION";
		case FUNC_CALL:
			return "FUNC_CALL";
		case GROUPING:
			return "GROUPING";
		case CONDITIONAL:
			return "CONDITIONAL";
		case OTHER:
			return "OTHER";
		default:
			return "UNKNOWN";
	}

	return "UNKNOWN";
}
