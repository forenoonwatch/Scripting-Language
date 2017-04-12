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
parser(std::make_unique<Parser>(*this)), canContinue(true), evaluateExpression(false) {
	operatorRegistry.init();
}

void Interpreter::interpretNextStatement() {
	/*
		Interpreting strategy:

		- interpretNext() is called by user which interprets all statements until program is paused or yielded
		- each scope requires its own iterator, since program can pause or be yielded at any moment
		- therefore, a stack<iterator> is required, with root-iterator at the bottom, and each scope pushing and popping
		- their iterator
		- BETTER LONG TERM IDEA: have a ScopeInfo class with statement iterator data, memory allocation data,
		  scope only function data, etc
		- variable resolution can be done working through the stack
		- stack must be a std::vector<std::shared_ptr<ScopeInfo>>

		- 
	*/

	if (evaluateExpression) {
		if (expressionStack.empty()) {
			evaluateExpression = false;
		}
		else {
			std::shared_ptr<Expression> exp = expressionStack.back();

			if (!exp->isExpectingValue()) {
				while (evaluateExpression && exp->canEval() && !exp->isExpectingValue()) {
					exp->evalNext();
				}

				if (!exp->canEval()) {
					exp->finishEval();
					std::cout << "finishing and popping expression" << std::endl;
					evaluateExpression = false;
					expressionStack.pop_back();
				}
			}
		}
	}

	std::shared_ptr<ScopeFrame> currScope = scopeStack.back();

	while (canContinue && !currScope->canGetStatement()) {
		if (currScope->isFunction() && !expressionStack.empty()) {
			std::shared_ptr<Expression> exp = expressionStack.back();

			if (exp->isExpectingValue()) {
				exp->addValue(std::static_pointer_cast<FunctionFrame>(currScope)->getReturnValue());
				evaluateExpression = true;
				scopeStack.pop_back();
				std::cout << "back on evaluating expressions; popped scope" << std::endl;
				return;
			}
		}

		
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

void Interpreter::parseText() {
	lexAllTokens();

	if (canContinue) {
		parseAllStatements();
	}

	scopeStack.push_back(std::make_shared<ScopeFrame>(parser->root));

	printAllChildren(parser->root);
	std::cout << "\nSTART OF SCRIPT" << std::endl;
}

void Interpreter::evalExpression(Statement* expression, std::shared_ptr<Variable> var) {
	std::cout << "pushing expression onto stack" << std::endl;

	std::shared_ptr<Expression> expr = std::make_shared<Expression>(*this, expression, var);
	expressionStack.push_back(expr);
	evaluateExpression = true;

	if (!expr->isExpectingValue()) {
		while (evaluateExpression && expr->canEval() && !expr->isExpectingValue()) {
			expr->evalNext();
		}

		if (!expr->canEval()) {
			expr->finishEval();
			std::cout << "finishing and popping expression" << std::endl;
			evaluateExpression = false;
			expressionStack.pop_back();
		}
	}
}

std::shared_ptr<Variable> Interpreter::resolveVariable(const std::string& name) {	
	for (int i = scopeStack.size() - 1; i >= 0; --i) {
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

	std::shared_ptr<FunctionFrame> func = std::static_pointer_cast<FunctionFrame>(scopeStack.back());
	std::shared_ptr<Variable> returnProxy = std::make_shared<Variable>();

	evalExpression(statement->getChildren()[0], returnProxy);
	
	returnProxy->cloneInto(func->getReturnValue());
}

std::shared_ptr<Variable> Interpreter::getVariable(const std::string& varName) {
	return resolveVariable(varName);
}

void Interpreter::evalCallArgs(std::shared_ptr<FunctionFrame> func, Statement* body, Statement* call) {
	Statement* params = body->getChildren()[0];

	std::vector<Statement*>::iterator arg = std::begin(call->getChildren());
	std::vector<Statement*>::iterator argEnd = std::end(call->getChildren());

	for (auto paramName = std::begin(params->getTokens()), paramEnd = std::end(params->getTokens());
			paramName != paramEnd; ++paramName) {
		if (arg != argEnd) {
			std::shared_ptr<Variable> argVar = std::make_shared<Variable>();
			evalExpression(*arg, argVar);
			
			func->addVariable(paramName->getContent(), argVar);
			std::cout << "setting parameter " << paramName->getContent() << " to " << argVar->intValue << std::endl;

			++arg;
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
