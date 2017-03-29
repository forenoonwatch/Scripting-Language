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
parser(std::make_unique<Parser>(*this)), canContinue(true) {
	operatorRegistry.init();
}

void Interpreter::interpretNextStatement() {
	if (currStatement == currEnd) {
		canContinue = false;
		std::cout << "Finished interpreting" << std::endl;
		return; // TODO: check parent or check current scope to see if actually finished parsing
	}

	Statement* stmnt = *currStatement;

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
	else {
		std::cout << "skipping " << Statement::typeAsString(stmnt->getType()) << std::endl;
	}

	++currStatement;
}

bool Interpreter::canInterpretStatement() const {
	return canContinue;
}

void Interpreter::parseText() {
	lexAllTokens();

	if (canContinue) {
		parseAllStatements();
	}

	printAllChildren(parser->root);

	parser->currRoot = parser->root;
	
	currStatement = std::begin(parser->currRoot->getChildren());
	currEnd = std::end(parser->currRoot->getChildren());
}

void Interpreter::evalExpression(Statement* expression, std::shared_ptr<Variable> var) {
	// http://www.geeksforgeeks.org/expression-evaluation/
	std::stack<Variable> values;
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
				values.push(*variableMap[it->getContent()]);
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

	*var = std::move(values.top());
}

void Interpreter::interpretVarDecl(Statement* statement) {
	std::string varName = statement->getTokens()[0].getContent();
	Statement* expression = statement->getChildren()[0];

	std::shared_ptr<Variable> var = std::make_shared<Variable>();
	evalExpression(expression, var);
	
	variableMap.emplace(varName, var);
}

void Interpreter::interpretVarAssignment(Statement* statement) {
	std::string varName = statement->getTokens()[0].getContent();
	Statement* expression = statement->getChildren()[0];

	std::shared_ptr<Variable> var = variableMap[varName]; // TODO: throw err if nullptr

	evalExpression(expression, var);
}

void Interpreter::interpretIfStatement(Statement* statement) {
	auto it = std::begin(statement->getChildren());

	Statement* condition = *(it++);
	std::shared_ptr<Variable> condVar = std::make_shared<Variable>();

	evalExpression(condition, condVar); // TODO: throw err if not bool type, or default to "is not nil" like lua
	
	if (condVar->boolValue) {
		std::cout << "condition passed!" << std::endl;
	}
	else {
		std::cout << "coniditon failed; trying other conditions or moving on" << std::endl;

		interpretScope();
	}
}

void Interpreter::interpretScope() {
	std::vector<Statement*>::iterator lastStatement = currStatement;
	Statement* startStatement = *currStatement;

	currStatement = std::begin(startStatement->getChildren());
	currEnd = std::end(startStatement->getChildren());

	while (currStatement != currEnd) {
		if ((*currStatement)->getType() == Statement::StatementType::EXPRESSION) {
			++currStatement;
			continue;
		}

		if ((*currStatement)->getType() == Statement::StatementType::CONDITIONAL) {
			std::cout << "finished parsing if block" << std::endl;
			break;
		}
		
		interpretNextStatement();
	}

	std::cout << "got here" << std::endl;
	
	currStatement = lastStatement;

	std::cout << "problemo" << std::endl;
}

std::shared_ptr<Variable> Interpreter::getVariable(const std::string& varName) {
	return variableMap[varName];
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
