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

			while (evaluateExpression && exp->canEval()) {
				exp->evalNext();
			}

			if (!exp->canEval()) {
				evaluateExpression = false;

				if (reinterpret_cast<>) {
					
				}

				expressionStack.pop_back();
			}
		}
	}

	std::shared_ptr<ScopeFrame> currScope = scopeStack.back();

	while (canContinue && !currScope->canGetStatement()) {
		scopeStack.pop_back();

		std::cout << "popping scope" << std::endl;
		
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
	std::shared_ptr<Expression> expr = std::make_shared<Expression>(*this, expression, var);
	expressionStack.push_back(expr);
	evaluateExpression = true;

	while (evaluateExpression && expr->canEval()) {
		expr->evalNext();
	}

	if (!expr->canEval()) {
		evaluateExpression = false;
		expressionStack.pop_back();
	}

	// http://www.geeksforgeeks.org/expression-evaluation/
	/*std::stack<Variable> values;
	std::stack<std::string> operators;

	for (auto it = std::begin(expression->getTokens()), end = std::end(expression->getTokens()); it != end; ++it) {
		if (it->getTokenType() == Token::TokenType::NUMERIC || it->getTokenType() == Token::TokenType::STRING) {
			values.push(Variable::fromToken(*it));
		}
		else if (it->getTokenType() == Token::TokenType::IDENTIFIER) {
			if (it->getContent().compare("true") == 0 || it->getContent().compare("false") == 0) {
				values.push(Variable::fromToken(*it));
			}
			else {
				values.push(*resolveVariable(it->getContent()));
			}
		}
		else if (it->getContent().compare("(") == 0) {
			operators.push(it->getContent());
		}
		else if (it->getContent().compare(")") == 0) {
			while (operators.top().compare("(") != 0) {
				Variable v1 = values.top();
				values.pop();

				Variable v2 = values.top();
				values.pop();

				Variable out;
				operatorRegistry.applyOperator(operators.top(), v2, v1, out);
				values.push(out);

				operators.pop();
			}

			operators.pop();
		}
		else if (it->getTokenType() == Token::TokenType::OPERATOR) {
			while (!operators.empty() && operatorRegistry.hasPrecedence(it->getContent(), operators.top())) {
				Variable v1 = values.top();
				values.pop();

				Variable v2 = values.top();
				values.pop();
				
				Variable out;
				operatorRegistry.applyOperator(operators.top(), v2, v1, out);
				values.push(out);

				operators.pop();
			}

			operators.push(it->getContent());
		}
	}

	while (!operators.empty()) {
		Variable v1 = values.top();
		values.pop();

		Variable v2 = values.top();
		values.pop();
		
		Variable out;
		operatorRegistry.applyOperator(operators.top(), v2, v1, out);
		values.push(out);

		operators.pop();
	}

	*var = std::move(values.top());*/
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
	evalExpression(expression, var);

	scopeStack.back()->addVariable(varName, var);
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

std::shared_ptr<Variable> Interpreter::getVariable(const std::string& varName) {
	return resolveVariable(varName);
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
