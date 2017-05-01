#include "interpreter.hpp"
#include "default-lib.hpp"

#include <iostream>

namespace {
	void printAllChildren(Statement* st, int depth = 0) {
		auto it = std::begin(st->getChildren());
		auto end = std::end(st->getChildren());

		while (it != end) {
			std::cout << "|" << Statement::typeAsString((*it)->getType()) << ":\t";

			for (int i = 0; i < depth; ++i) {
				std::cout << "\t";
			}


			for (auto tStart = std::begin((*it)->getTokens()); tStart != std::end((*it)->getTokens()); ++tStart) {
				if (tStart->getTokenType() == Token::TokenType::LINK) {
					std::cout << "[LINK->" << Statement::typeAsString(tStart->getLink()->getType())  << "], ";
				}
				else {
					std::cout << "[" << tStart->getContent() << "]" << ", ";
				}
			}

			std::cout << std::endl;

			printAllChildren(*it, depth + 1);

			++it;
		}
	}

	void printAllTokens(TokenStream& tokenStream) {
		while (tokenStream.canGet()) {
			std::cout << "[" << tokenStream.get().getContent() << "]" << std::endl;
		}
	}
};

Interpreter::Interpreter(std::istream& textStream, bool useDefaultLib)
: lexer(std::make_unique<Lexer>(textStream, *this)),
	parser(std::make_unique<Parser>(*this)), canContinue(true), evaluateExpression(false),
	returnProxy(std::make_shared<Variable>()), scriptYielded(false),
		defaultLib(nullptr) {
	operatorRegistry.init();

	if (useDefaultLib) {
		defaultLib = std::make_unique<DefaultLibrary>(*this);
	}

	//errorLog.setLogDepth(ErrorLog::LogDepth::DEBUG);
}

bool Interpreter::isRunning() const {
	return canContinue;
}

bool Interpreter::hasScriptEvents() const {
	if (defaultLib != nullptr) {
		defaultLib->update();
	}
	
	return canInterpretStatement();
}

void Interpreter::setLogDepth(ErrorLog::LogDepth depth) {
	errorLog.setLogDepth(depth);
}

void Interpreter::processScript() {
	while (canInterpretStatement()) {
		interpretNextStatement();
	}
}

void Interpreter::interpretNextStatement() {
	errorLog.logDebug("---");

	std::shared_ptr<ScopeFrame> currScope = scopeStack.back();
	
	if (evaluateExpression) {
		if (expressionStack.empty()) {
			evaluateExpression = false;
		}
		else {
			std::shared_ptr<Expression> exp = expressionStack.back();

			if (!exp->isExpectingValue()) {
				errorLog.logDebug("evaluating expressions");

				while (evaluateExpression && exp->canEval() && !exp->isExpectingValue()) {
					exp->evalNext();
				}

				if (!exp->canEval()) {
					exp->finishEval();
					errorLog.logDebug("finishing and popping expression ("
						+ std::to_string(expressionStack.size() - 1) + ")");

					evaluateExpression = !expressionStack.empty()
						&& expressionStack.back()->canEval();
					expressionStack.pop_back();
					
					cleanAfterExpression();
				}
			}

			return;
		}
	}

	if (currScope->isFunction()) {
		std::shared_ptr<FunctionFrame> func = std::static_pointer_cast<FunctionFrame>(currScope);

		if (func->canEvalArg()) {
			errorLog.logDebug("Evaluating argument");
			func->evalNextArg();
			return;
		}
		else {
			if (func->isExternalFunction()) {
				errorLog.logDebug("Calling external function");

				func->doExternalCall();
				func->getReturnValue().cloneInto(*returnProxy);

				func->setBaseExpression(expressionStack.size());
				func->setReturning();

				cleanAfterExpression();

				return;
			}
		}
	}

	while (canContinue && !currScope->canGetStatement()) {
		if (scopeStack.size() > 1) {
			scopeStack.pop_back();
			errorLog.logDebug("popping scope");
		}
		else {
			errorLog.logDebug("left to just root scope; stopping");
			canContinue = false;
			return;
		}
		
		if (!scopeStack.empty()) {
			currScope = scopeStack.back();
		}
		else {
			std::cout << "END OF SCRIPT" << std::endl;
			canContinue = false; // TODO: mark that script has finished correctly
			return;
		}
	}

	Statement* stmnt = currScope->getStatement();

	if (stmnt->getType() == Statement::StatementType::VAR_DECLARATION) {
		errorLog.logDebug("interpreting var delcaration");
		interpretVarDecl(stmnt);
	}
	else if (stmnt->getType() == Statement::StatementType::VAR_ASSIGNMENT) {
		errorLog.logDebug("interpreting var assignment");
		interpretVarAssignment(stmnt);
	}
	else if (stmnt->getType() == Statement::StatementType::CONDITIONAL) {
		errorLog.logDebug("interpreting conditional");
		interpretIfStatement(stmnt);
	}
	else if (stmnt->getType() == Statement::StatementType::FUNC_DECL) {
		errorLog.logDebug("interpreting function declaration");
		interpretFuncDecl(stmnt);
	}
	else if (stmnt->getType() == Statement::StatementType::FUNC_CALL) {
		errorLog.logDebug("interpreting function call");
		interpretFuncCall(stmnt);
	}
	else if (stmnt->getType() == Statement::StatementType::RETURN) {
		errorLog.logDebug("interpreting return statement");
		interpretReturn(stmnt);
	}
	else {
		errorLog.logDebug("skipping " + Statement::typeAsString(stmnt->getType()));
	}
}

bool Interpreter::canInterpretStatement() const {
	return canContinue && !scriptYielded;
}

void Interpreter::addExternalFunc(const std::string& name, ExternalFunction func) {
	std::shared_ptr<ScopeFrame> root = scopeStack[0];
	root->addVariable(name, std::make_shared<Variable>(func));
}

void Interpreter::parseText() {
	lexAllTokens();

	errorLog.logDebug("Finished lexing stage");

	if (canContinue) {
		parseAllStatements();
	}

	errorLog.logDebug("Finished parsing stage");

	scopeStack.push_back(std::make_shared<ScopeFrame>(parser->root));

	if (errorLog.getLogDepth() >= ErrorLog::LogDepth::DEBUG) {
		printAllChildren(parser->root);
	}

	if (defaultLib != nullptr) {
		defaultLib->loadExternalFunctions();
	}
	
	errorLog.logDebug("\nSTART OF SCRIPT");
}

void Interpreter::evalExpression(Statement* expression, std::shared_ptr<Variable> var,
		int startOffset) {
	errorLog.logDebug("pushing expression onto stack ("
		+ std::to_string(expressionStack.size() + 1) + ")");

	std::shared_ptr<Expression> expr = std::make_shared<Expression>(*this, expression, var, startOffset);
	expressionStack.push_back(expr);
	evaluateExpression = true;

	if (!expr->isExpectingValue()) {
		while (evaluateExpression && expr->canEval() && !expr->isExpectingValue()) {
			expr->evalNext();
		}

		if (!expr->canEval()) {
			expr->finishEval();
			errorLog.logDebug("finishing and popping expression ("
				+ std::to_string(expressionStack.size() - 1) + ")");

			evaluateExpression = !expressionStack.empty() && expressionStack.back()->canEval();
			expressionStack.pop_back();

			cleanAfterExpression();
		}
	}
}

std::shared_ptr<Variable> Interpreter::resolveVariable(const std::string& name,
		int offsetCount) {
	for (int i = scopeStack.size() - 1 - offsetCount; i >= 0; --i) {
		if (scopeStack[i]->getVariable(name) != nullptr) {
			return scopeStack[i]->getVariable(name);
		}
	}

	return nullptr;
}

void Interpreter::interpretVarDecl(Statement* statement) {
	std::string varName = statement->getTokens()[0].getContent();
	Statement* expression = statement->getChildren()[0];

	std::shared_ptr<Variable> var = std::make_shared<Variable>();
	scopeStack.back()->addVariable(varName, var);
	
	evalExpression(expression, var);
}

void Interpreter::interpretVarAssignment(Statement* statement) {
	std::string varName = statement->getTokens()[0].getContent();
	Statement* expression = statement->getChildren()[0];

	std::shared_ptr<Variable> var = resolveVariable(varName); // TODO: throw err if nullptr

	evalExpression(expression, var);
}

void Interpreter::interpretIfStatement(Statement* statement) {
	auto it = std::begin(statement->getTokens());

	if (it->getContent().compare("if") != 0) {
		errorLog.logDebug("skipping elif or else");
		return; // skip elif and else statements ; TODO: pop scope
	}
	
	++it;

	std::shared_ptr<Variable> condVar = std::make_shared<Variable>();

	evalExpression(statement->getChildren()[0], condVar);

	if (condVar->boolValue) {
		errorLog.logDebug("found sucessful first if");
		scopeStack.push_back(std::make_shared<ScopeFrame>(statement));

		return;
	}

	Statement* elseStatement = nullptr;

	for (auto end = std::end(statement->getTokens()); it != end; ++it) {
		Statement* condition = it->getLink();

		if (condition->getTokens()[0].getContent().compare("else") == 0) {
			elseStatement = condition;
			continue;
		}

		// TODO: throw err if not bool type, or default to "is not nil" like lua
		evalExpression(condition->getChildren()[0], condVar);

		if (condVar->boolValue) {
			errorLog.logDebug("found successful statement");
			scopeStack.push_back(std::make_shared<ScopeFrame>(condition));

			return;
		}
		else {
			errorLog.logDebug("attempting another statement");
		}
	}

	if (elseStatement != nullptr) {
		errorLog.logDebug("defaulting to else statement");
		scopeStack.push_back(std::make_shared<ScopeFrame>(elseStatement));
	}
}

void Interpreter::interpretFuncDecl(Statement* statement) {
	scopeStack.back()->addVariable(statement->getTokens()[0].getContent(),
		std::make_shared<Variable>(statement));
}

void Interpreter::interpretFuncCall(Statement* call) {
	std::shared_ptr<Variable> funcVar = resolveVariable(call->getTokens()[0].getContent());
	
	errorLog.logDebug("interpreting direct function call");

	if (funcVar != nullptr && (funcVar->type == Variable::VariableType::FUNCTION
			|| funcVar->type == Variable::VariableType::EXTERNAL_FUNC)) {
		if (funcVar->type == Variable::VariableType::FUNCTION) {
			addFunctionCall(funcVar->funcValue, call);
		}
		else {
			addFunctionCall(call, funcVar);
		}
	}
	else {
		errorLog.logError("invalid function name");
		// TODO: runtime error
	}
}

void Interpreter::interpretReturn(Statement* statement) {
	if (scopeStack.size() == 1) {
		errorLog.logError("ERROR: trying to return in global scope"); // TODO: proper error or have this
		// be script return value
		return;
	}

	if (functionStack.empty()) {
		errorLog.logError("ERROR: no function found to return from");
		return;
	}

	std::shared_ptr<FunctionFrame> func = functionStack.back();

	func->setBaseExpression(expressionStack.size());
	func->setReturning();
	errorLog.logDebug("flagging return with halt level = "
		+ std::to_string(expressionStack.size()));

	evalExpression(statement->getChildren()[0], returnProxy);
}

std::shared_ptr<Variable> Interpreter::getVariable(const std::string& varName) {
	return resolveVariable(varName);
}

void Interpreter::addFunctionCall(Statement* body, Statement* call) {
	std::shared_ptr<FunctionFrame> func = std::make_shared<FunctionFrame>(*this,
		body, call);

	scopeStack.push_back(func);
	functionStack.push_back(func);
}

void Interpreter::addFunctionCall(Statement* scope,
		std::shared_ptr<Variable> externalCall) {
	std::shared_ptr<FunctionFrame> func = std::make_shared<FunctionFrame>(*this,
		scope, externalCall);

	scopeStack.push_back(func);
	functionStack.push_back(func);
}

void Interpreter::cleanAfterExpression() {
	std::shared_ptr<FunctionFrame> func = nullptr;

	while (functionStack.size() > 0) {
		func = functionStack.back();

		if (func->getIsReturning()
				&& expressionStack.size() <= func->getBaseExpression()) {
			errorLog.logDebug("Return statement expression is finished");
			
			errorLog.logDebug("cleaning function frames");
			
			while (!scopeStack.back()->isFunction()) {
				errorLog.logDebug("\tcleaned scope");
				scopeStack.pop_back();
			}

			if (!expressionStack.empty()) {
				std::shared_ptr<Expression> exp = expressionStack.back();
			
				if (exp->isExpectingValue()) {
					exp->addValue(*returnProxy);
					evaluateExpression = true;
					errorLog.logDebug("loaded return value: "
						+ Variable::toString(*returnProxy));
				}
			}

			errorLog.logDebug("popping function scope");
			scopeStack.pop_back();
			functionStack.pop_back();
		}
		else {
			break;
		}
	}
}

inline void Interpreter::lexAllTokens() {
	while (lexer->canConsumeToken()) {
		lexer->consumeNextToken();
	}

	if (lexer->hasErrored()) {
		canContinue = false;
	}
}

inline void Interpreter::parseAllStatements() {
	while (parser->canConsumeStatement()) {
		parser->consumeNextStatement();
	}

	if (parser->hasErrored()) {
		canContinue = false;
	}
}

bool Interpreter::isYielded() const {
	return scriptYielded;
}

void Interpreter::setYielded(bool yielded) {
	scriptYielded = yielded;
}
