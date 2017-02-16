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

std::string Statement::typeAsSrting(StatementType type) {
	//TODO: implement
}
