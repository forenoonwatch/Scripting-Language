#include "scope-frame.hpp"
#include <vector>
#include <algorithm>

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

void ScopeFrame::writeVarsToVector(std::vector<std::shared_ptr<Variable>>& vec) const {
	std::shared_ptr<Variable> var;

	for (auto it = std::begin(variableMap), end = std::end(variableMap);
			it != end; ++it) {
		var = std::make_shared<Variable>();
		std::cout << (it->second != nullptr ? "true" : "false") << std::endl;
		it->second->cloneInto(*var);

		vec.push_back(var);
	}
}

FunctionFrame::FunctionFrame(Interpreter& interpreter, Statement* scope, Statement* call)
: interpreter(interpreter), ScopeFrame(scope), arg(std::begin(call->getChildren())), argEnd(std::end(call->getChildren())),
	params(scope->getChildren()[0]), paramName(std::begin(params->getTokens())), paramEnd(std::end(params->getTokens())), isReturning(false), externalCall(nullptr) {}

FunctionFrame::FunctionFrame(Interpreter& interpreter, Statement* call,
	std::shared_ptr<Variable> externalCall)
: interpreter(interpreter), ScopeFrame(call), arg(std::begin(call->getChildren())),
	argEnd(std::end(call->getChildren())), isReturning(false),
	externalCall(externalCall) {}

bool FunctionFrame::canGetStatement() const {
	return ScopeFrame::canGetStatement() && !isReturning;
}

Variable& FunctionFrame::getReturnValue() {
	return returnValue;
}

void FunctionFrame::setReturning() {
	isReturning = true;
}

bool FunctionFrame::getIsReturning() const {
	return isReturning;
}

bool FunctionFrame::isFunction() const {
	return true;
}

bool FunctionFrame::isExternalFunction() const {
	return externalCall != nullptr;
}

bool FunctionFrame::canEvalArg() const {
	if (isExternalFunction()) {
		return arg != argEnd;
	}

	return paramName != paramEnd && arg != argEnd;
}

void FunctionFrame::evalNextArg() {
	std::shared_ptr<Variable> argVar = std::make_shared<Variable>();
	interpreter.evalExpression(*arg, argVar, 1);

	if (isExternalFunction()) {
		std::cout << "setting parameter arg" << std::to_string(argEnd - arg) << std::endl;
		addVariable("arg" + std::to_string(argEnd - arg), argVar);
	}
	else {
		std::cout << "setting parameter " << paramName->getContent() << std::endl;
		addVariable(paramName->getContent(), argVar);
		++paramName;
	}

	++arg;
}

void FunctionFrame::setBaseExpression(int baseExpression) {
	this->baseExpression = baseExpression;
}

int FunctionFrame::getBaseExpression() const {
	return baseExpression;
}

void FunctionFrame::doExternalCall() {
	std::vector<std::shared_ptr<Variable>> vArgs;
	writeVarsToVector(vArgs);
	std::reverse(std::begin(vArgs), std::end(vArgs));

	std::shared_ptr<Variable> returnProxy = std::make_shared<Variable>();
	
	(*externalCall)(vArgs, returnProxy);

	returnValue = *returnProxy;
}
