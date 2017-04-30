#include "interpreter.hpp"
#include <iostream>
#include <stack>

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

Interpreter::Interpreter(std::istream& textStream)
: lexer(std::make_unique<Lexer>(textStream, *this)),
	parser(std::make_unique<Parser>(*this)), canContinue(true), evaluateExpression(false),
	returnProxy(std::make_shared<Variable>())
{
	operatorRegistry.init();
}

bool Interpreter::isRunning() const {
	return canContinue;
}

bool Interpreter::hasScriptEvents() const {
	return canContinue;
}

void Interpreter::processScript() {
	while (canInterpretStatement()) {
		interpretNextStatement();
	}
}

void Interpreter::interpretNextStatement() {
	/*
		Interpreting strategy:

		- interpretNext() is called by user which interprets all statements until program is paused or yielded
		- 
	*/

	std::cout << "---" << std::endl;

	std::shared_ptr<ScopeFrame> currScope = scopeStack.back();
	
	if (evaluateExpression) {
		if (expressionStack.empty()) {
			evaluateExpression = false;
		}
		else {
			std::shared_ptr<Expression> exp = expressionStack.back();

			if (!exp->isExpectingValue()) {
				std::cout << "evaluating expressions" << std::endl;

				while (evaluateExpression && exp->canEval() && !exp->isExpectingValue()) {
					exp->evalNext();
				}

				if (!exp->canEval()) {
					exp->finishEval();
					std::cout << "finishing and popping expression ("
						<< (expressionStack.size() - 1) << ")" << std::endl;

					evaluateExpression = !expressionStack.empty() && expressionStack.back()->canEval();
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
			std::cout << "Evaluating argument" << std::endl;
			func->evalNextArg();
			return;
		}
		else {
			if (func->isExternalFunction()) {
				std::cout << "Calling external function" << std::endl;

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
			std::cout << "popping scope" << std::endl;
		}
		else {
			std::cout << "left to just root scope; stopping" << std::endl;
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
		std::cout << "interpreting var delcaration" << std::endl;
		interpretVarDecl(stmnt);
	}
	else if (stmnt->getType() == Statement::StatementType::VAR_ASSIGNMENT) {
		std::cout << "interpreting var assignment" << std::endl;
		interpretVarAssignment(stmnt);
	}
	else if (stmnt->getType() == Statement::StatementType::CONDITIONAL) {
		std::cout << "interpreting conditional" << std::endl;
		interpretIfStatement(stmnt);
	}
	else if (stmnt->getType() == Statement::StatementType::FUNC_DECL) {
		std::cout << "interpreting function declaration" << std::endl;
		interpretFuncDecl(stmnt);
	}
	else if (stmnt->getType() == Statement::StatementType::RETURN) {
		std::cout << "interpreting return statemenet" << std::endl;
		interpretReturn(stmnt);
	}
	else {
		std::cout << "skipping " << Statement::typeAsString(stmnt->getType()) << std::endl;
	}
}

bool Interpreter::canInterpretStatement() const {
	return canContinue;
}

void Interpreter::addExternalFunc(const std::string& name, ExternalFunction func) {
	std::shared_ptr<ScopeFrame> root = scopeStack[0];
	root->addVariable(name, std::make_shared<Variable>(func));
}

void Interpreter::parseText() {
	lexAllTokens();

	if (canContinue) {
		parseAllStatements();
	}

	scopeStack.push_back(std::make_shared<ScopeFrame>(parser->root));

	printAllChildren(parser->root);
	std::cout << "\nSTART OF SCRIPT" << std::endl;
}

void Interpreter::evalExpression(Statement* expression, std::shared_ptr<Variable> var,
		int startOffset) {
	std::cout << "pushing expression onto stack ("
		<< (expressionStack.size() + 1) << ")" << std::endl;

	std::shared_ptr<Expression> expr = std::make_shared<Expression>(*this, expression, var, startOffset);
	expressionStack.push_back(expr);
	evaluateExpression = true;

	if (!expr->isExpectingValue()) {
		while (evaluateExpression && expr->canEval() && !expr->isExpectingValue()) {
			expr->evalNext();
		}

		if (!expr->canEval()) {
			expr->finishEval();
			std::cout << "finishing and popping expression ("
						<< (expressionStack.size() - 1) << ")" << std::endl;

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
		std::cout << "skipping elif or else" << std::endl;
		return; // skip elif and else statements ; TODO: pop scope
	}
	
	++it;

	std::shared_ptr<Variable> condVar = std::make_shared<Variable>();

	evalExpression(statement->getChildren()[0], condVar);

	if (condVar->boolValue) {
		std::cout << "found sucessful first if" << std::endl;
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
			std::cout << "found successful statement" << std::endl;
			scopeStack.push_back(std::make_shared<ScopeFrame>(condition));

			return;
		}
		else {
			std::cout << "attempting another statement" << std::endl;
		}
	}

	if (elseStatement != nullptr) {
		std::cout << "defaulting to else statement" << std::endl;
		scopeStack.push_back(std::make_shared<ScopeFrame>(elseStatement));
	}
}

void Interpreter::interpretFuncDecl(Statement* statement) {
	scopeStack.back()->addVariable(statement->getTokens()[0].getContent(),
		std::make_shared<Variable>(statement));
}

void Interpreter::interpretReturn(Statement* statement) {
	if (scopeStack.size() == 1) {
		std::cout << "ERROR: trying to return in global scope" << std::endl; // TODO: proper error or have this
		// be script return value
		return;
	}

	if (functionStack.empty()) {
		std::cout << "ERROR: no function found to return from" << std::endl;
		return;
	}

	std::shared_ptr<FunctionFrame> func = functionStack.back();

	func->setBaseExpression(expressionStack.size());
	func->setReturning();
	std::cout << "flagging return with halt level = " << expressionStack.size() << std::endl;

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
			std::cout << "Return statement expression is finished" << std::endl;
			
			std::cout << "cleaning function frames" << std::endl;
			
			while (!scopeStack.back()->isFunction()) {
				std::cout << "\tcleaned scope" << std::endl;
				scopeStack.pop_back();
			}

			std::shared_ptr<Expression> exp = expressionStack.back();
			
			if (exp->isExpectingValue()) {
				exp->addValue(*returnProxy);
				evaluateExpression = true;
				std::cout << "loaded return value: "
					<< Variable::toString(*returnProxy) << std::endl;
			}

			std::cout << "popping function scope" << std::endl;
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
