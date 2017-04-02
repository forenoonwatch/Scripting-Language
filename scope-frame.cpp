#include "scope-frame.hpp"

ScopeFrame::ScopeFrame(Statement* scope)
: currStatement(std::begin(scope->getChildren())), end(std::end(scope->getChildren())) {}

bool ScopeFrame::canGetStatement() const {
	return currStatement != end;
}

Statement* ScopeFrame::peekStatement() {
	return *currStatement;
}

Statement* ScopeFrame::getStatement() {
	return *(currStatement++);
}

void ScopeFrame::addVariable(const std::string& name, std::shared_ptr<Variable> var) {
	variableMap.emplace(name, var);
}

std::shared_ptr<Variable> ScopeFrame::getVariable(const std::string& name) {
	return variableMap[name];
}

FunctionFrame::FunctionFrame(Statement* scope): ScopeFrame(scope) {}

Variable& FunctionFrame::getReturnValue() {
	return returnValue;
}
