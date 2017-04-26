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

bool ScopeFrame::isFunction() const {
	return false;
}

void ScopeFrame::addVariable(const std::string& name, std::shared_ptr<Variable> var) {
	variableMap.emplace(name, var);
}

std::shared_ptr<Variable> ScopeFrame::getVariable(const std::string& name) {
	return variableMap[name];
}

FunctionFrame::FunctionFrame(Interpreter& interpreter, Statement* scope, Statement* call)
: interpreter(interpreter), ScopeFrame(scope), arg(std::begin(call->getChildren())), argEnd(std::end(call->getChildren())),
	params(scope->getChildren()[0]), paramName(std::begin(params->getTokens())), paramEnd(std::end(params->getTokens())) {}

Variable& FunctionFrame::getReturnValue() {
	return returnValue;
}

bool FunctionFrame::isFunction() const {
	return true;
}

bool FunctionFrame::canEvalArg() const {
	return paramName != paramEnd && arg != argEnd;
}

void FunctionFrame::evalNextArg() {
	std::cout << "setting parameter " << paramName->getContent() << std::endl;
	
	std::shared_ptr<Variable> argVar = std::make_shared<Variable>();
	addVariable(paramName->getContent(), argVar);
	
	interpreter.evalExpression(*arg, argVar);

	++arg;
	++paramName;
}
