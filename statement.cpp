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
			return "VARD";
		case VAR_ASSIGNMENT:
			return "VARA";
		case EXPRESSION:
			return "EXPR";
		case FUNC_CALL:
			return "CALL";
		case PARAMETERS:
			return "PARM";
		case CONDITIONAL:
			return "COND";
		case FUNC_DECL:
			return "FUNC";
		case RETURN:
			return "RTRN";
		case WHILE_LOOP:
			return "WHLE";
		case FOR_LOOP:
			return "FORL";
		case OTHER:
			return "OTHR";
		default:
			return "UNKN";
	}

	return "UNKNOWN";
}
