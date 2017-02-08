#include "statement.hpp"

Statement::Statement(std::shared_ptr<Statement> parent)
: parent(parent) {}

void Statement::addChild(std::shared_ptr<Statement> child) {
	child->parent = std::make_shared<Statement>(*this);
	children.push_back(child);
}

void Statement::addToken(Token token) {
	tokens.push_back(token);
}
